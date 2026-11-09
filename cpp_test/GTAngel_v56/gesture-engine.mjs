// Pure authored-animation mathematics. This does not infer emotions or replace cognition.
const finite = (v, label) => {
  if (typeof v !== "number" || !Number.isFinite(v))
    throw new TypeError(`${label} must be finite`);
  return v;
};
const plain = (v) =>
  v !== null &&
  typeof v === "object" &&
  !Array.isArray(v) &&
  (Object.getPrototypeOf(v) === Object.prototype ||
    Object.getPrototypeOf(v) === null);
const copy = (v) => Object.fromEntries(Object.entries(v));
export function minimumJerk(u) {
  finite(u, "progress");
  const t = Math.max(0, Math.min(1, u));
  return t * t * t * (10 + t * (-15 + 6 * t));
}
export function compileLibrary(library, definitions) {
  if (
    !plain(library) ||
    !plain(library.poses) ||
    !Array.isArray(library.motifs) ||
    !Array.isArray(definitions) ||
    definitions.length < 1
  )
    throw TypeError("Invalid motion library");
  const limits = new Map(),
    defaults = {};
  for (const d of definitions) {
    if (
      !plain(d) ||
      typeof d.id !== "string" ||
      !/^Param[A-Za-z0-9]+$/.test(d.id) ||
      limits.has(d.id)
    )
      throw TypeError("Invalid/duplicate parameter definition");
    for (const key of ["min", "max", "default"])
      finite(d[key], d.id + "." + key);
    if (d.min >= d.max || d.default < d.min || d.default > d.max)
      throw RangeError("Invalid parameter domain");
    limits.set(d.id, d);
    defaults[d.id] = d.default;
  }
  const resolve = (map) => {
    if (!plain(map)) throw TypeError("Pose values must be a plain map");
    const values = copy(defaults);
    for (const [id, v] of Object.entries(map)) {
      const d = limits.get(id);
      if (!d) throw RangeError("Unknown parameter " + id);
      finite(v, id);
      if (v < d.min || v > d.max)
        throw RangeError("Authored target outside native domain " + id);
      values[id] = v;
    }
    return values;
  };
  const poses = {};
  for (const [id, p] of Object.entries(library.poses)) {
    if (
      !/^[A-Za-z][A-Za-z0-9_-]{0,47}$/.test(id) ||
      !plain(p) ||
      typeof p.label !== "string"
    )
      throw TypeError("Invalid pose");
    poses[id] = { id, label: p.label, values: resolve(p.values) };
  }
  const clips = {};
  for (const m of library.motifs) {
    if (
      !plain(m) ||
      typeof m.id !== "string" ||
      !/^[A-Za-z][A-Za-z0-9_-]{0,47}$/.test(m.id) ||
      clips[m.id] ||
      typeof m.label !== "string" ||
      typeof m.loop !== "boolean" ||
      !Array.isArray(m.frames) ||
      m.frames.length < 2 ||
      m.frames.length > 256
    )
      throw TypeError("Invalid motif");
    finite(m.duration, "duration");
    if (m.duration <= 0 || m.duration > 60)
      throw RangeError("Motif duration outside 0..60 seconds");
    let previous = -1;
    const frames = m.frames.map((f) => {
      if (!plain(f) || !poses[f.pose]) throw TypeError("Unknown pose key");
      finite(f.time, "key time");
      if (f.time <= previous || f.time < 0 || f.time > m.duration)
        throw RangeError("Invalid key ordering");
      previous = f.time;
      const base = poses[f.pose].values;
      return {
        time: f.time,
        values: resolve({ ...base, ...(f.overrides || {}) }),
      };
    });
    if (frames[0].time !== 0 || frames.at(-1).time !== m.duration)
      throw RangeError("Motif must span full duration");
    if (
      m.loop &&
      !Object.keys(defaults).every(
        (id) => frames[0].values[id] === frames.at(-1).values[id],
      )
    )
      throw RangeError("Loop endpoints must match");
    clips[m.id] = {
      id: m.id,
      label: m.label,
      duration: m.duration,
      loop: m.loop,
      frames,
    };
  }
  return { defaults, limits, poses, clips, resolve };
}
export function interpolate(from, to, u) {
  const weight = minimumJerk(u),
    result = {};
  for (const id of Object.keys(from)) {
    if (!Object.hasOwn(to, id)) throw TypeError("Parameter schema mismatch");
    finite(from[id], id);
    finite(to[id], id);
    result[id] = from[id] + (to[id] - from[id]) * weight;
  }
  if (Object.keys(to).length !== Object.keys(from).length)
    throw TypeError("Parameter schema mismatch");
  return result;
}
export function sampleMotion(clip, time, { loop = clip.loop } = {}) {
  finite(time, "sample time");
  if (typeof loop !== "boolean") throw TypeError("loop must be boolean");
  let t = loop
    ? ((time % clip.duration) + clip.duration) % clip.duration
    : Math.max(0, Math.min(clip.duration, time));
  for (let i = 1; i < clip.frames.length; i++) {
    const right = clip.frames[i];
    if (t <= right.time) {
      const left = clip.frames[i - 1];
      return interpolate(
        left.values,
        right.values,
        (t - left.time) / (right.time - left.time),
      );
    }
  }
  return copy(clip.frames.at(-1).values);
}
export function exportMotion3(clip, fps = 30) {
  if (!Number.isInteger(fps) || fps < 10 || fps > 120)
    throw RangeError("Invalid export frame rate");
  const count = Math.ceil(clip.duration * fps),
    ids = Object.keys(clip.frames[0].values),
    rows = Array.from({ length: count + 1 }, (_, i) => ({
      time: (clip.duration * i) / count,
      values: sampleMotion(clip, (clip.duration * i) / count, { loop: false }),
    }));
  return {
    Version: 3,
    Meta: {
      Duration: clip.duration,
      Fps: fps,
      Loop: clip.loop,
      AreBeziersRestricted: true,
      CurveCount: ids.length,
      TotalSegmentCount: ids.length * count,
      TotalPointCount: ids.length * (count + 1),
      UserDataCount: 0,
      TotalUserDataSize: 0,
    },
    Curves: ids.map((id) => ({
      Target: "Parameter",
      Id: id,
      Segments: rows.flatMap((row, i) =>
        i ? [0, row.time, row.values[id]] : [row.time, row.values[id]],
      ),
    })),
  };
}
export function composeMotions(clips, id = "sequence", blendSeconds = 0.35) {
  if (!Array.isArray(clips) || clips.length < 1 || clips.length > 12)
    throw RangeError("Sequence requires 1..12 clips");
  finite(blendSeconds, "blend");
  if (blendSeconds <= 0 || blendSeconds > 3)
    throw RangeError("Invalid sequence blend");
  let time = 0;
  const frames = [];
  for (const clip of clips) {
    if (frames.length) {
      time += blendSeconds;
      frames.push({ time, values: copy(clip.frames[0].values) });
    }
    for (let i = 0; i < clip.frames.length; i++) {
      if (frames.length && i === 0) continue;
      frames.push({
        time: time + clip.frames[i].time,
        values: copy(clip.frames[i].values),
      });
    }
    time += clip.duration;
  }
  if (time > 180 || frames.length > 2048)
    throw RangeError("Sequence budget exceeded");
  return {
    id,
    label: "Authored gesture sequence",
    duration: time,
    loop: false,
    frames,
  };
}
export function createMotionPlayer(library) {
  let current = copy(library.defaults),
    active = null,
    elapsed = 0,
    blend = 0,
    from = null,
    rate = 1,
    loop = false,
    phase = "hold";
  const snapshot = () => ({
    phase,
    id: active?.id ?? null,
    label: active?.label ?? null,
    elapsed,
    duration: active?.duration ?? 0,
    blendElapsed: blend,
    rate,
    loop,
    values: copy(current),
  });
  return {
    transition(source, target, seconds = 0.35) {
      finite(seconds, "transition seconds");
      if (seconds <= 0 || seconds > 3) throw RangeError("Invalid transition");
      from = library.resolve(source);
      const to = library.resolve(target);
      active = {
        id: "pose-transition",
        label: "Pose transition",
        duration: seconds,
        loop: false,
        frames: [
          { time: 0, values: from },
          { time: seconds, values: to },
        ],
      };
      elapsed = 0;
      blend = 0;
      phase = "play";
      loop = false;
      rate = 1;
      current = copy(from);
      return snapshot();
    },
    play(clip, source, { speed = 1, looped = clip.loop, blendIn = 0.35 } = {}) {
      finite(speed, "speed");
      finite(blendIn, "blendIn");
      if (
        speed < 0.25 ||
        speed > 2 ||
        blendIn < 0 ||
        blendIn > 3 ||
        typeof looped !== "boolean"
      )
        throw RangeError("Invalid playback options");
      if (!clip || !Array.isArray(clip.frames) || clip.duration <= 0)
        throw TypeError("Invalid compiled clip");
      from = library.resolve(source);
      active = clip;
      elapsed = 0;
      blend = 0;
      rate = speed;
      loop = looped;
      phase = blendIn > 0 ? "blend" : "play";
      active = { ...clip, blendIn };
      current = blendIn > 0 ? copy(from) : sampleMotion(active, 0, { loop });
      return snapshot();
    },
    step(dt) {
      finite(dt, "delta seconds");
      if (dt < 0) throw RangeError("Negative delta");
      const delta = Math.min(dt, 0.1) * rate;
      if (phase === "blend") {
        blend = Math.min(active.blendIn, blend + delta);
        current = interpolate(
          from,
          active.frames[0].values,
          blend / active.blendIn,
        );
        if (blend >= active.blendIn) phase = "play";
      } else if (phase === "play") {
        elapsed += delta;
        if (!loop && elapsed >= active.duration) {
          elapsed = active.duration;
          phase = "hold";
        } else if (loop) elapsed %= active.duration;
        current = sampleMotion(active, elapsed, { loop });
      }
      return copy(current);
    },
    seek(seconds) {
      if (!active) throw Error("No active motion");
      finite(seconds, "seek seconds");
      elapsed = Math.max(0, Math.min(active.duration, seconds));
      phase = "hold";
      current = sampleMotion(active, elapsed, { loop: false });
      return copy(current);
    },
    resume() {
      if (active && phase === "hold" && (elapsed < active.duration || loop))
        phase = "play";
      return copy(current);
    },
    hold(values) {
      current = library.resolve(values);
      active = null;
      phase = "hold";
      elapsed = 0;
      blend = 0;
      return copy(current);
    },
    snapshot,
  };
}
