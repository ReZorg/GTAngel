import "@pixi/unsafe-eval";
import { Application, Ticker } from "pixi.js";
import catalog from "./gesture-library.json";
import {
  compileLibrary,
  createMotionPlayer,
  composeMotions,
} from "./gesture-engine.mjs";
const $ = (id) => document.getElementById(id);
let app,
  model,
  settings,
  definitions,
  qa = false,
  generation = 0,
  disposed = false,
  resizeObserver,
  lastReadout = 0,
  saveQueue = Promise.resolve();
let pointer = { active: false, x: 0, y: 0 },
  softwareRendering = false,
  gpuRenderer = "unavailable",
  lastNativeInputs = null,
  loadingPromise = null;
let library,
  player,
  clock = 0;
const source = "models/DTEMorphRig/DTEMorphRig.model3.json";
function failure(error) {
  const message = String(error?.stack || error).slice(0, 2200);
  console.error("[Purism gestures]", message);
  $("loading").hidden = true;
  $("error").hidden = false;
  $("error").textContent = message;
  window.widget?.reportError(message).catch(() => {});
}
window.addEventListener("error", (event) =>
  failure(event.error || event.message),
);
window.addEventListener("unhandledrejection", (event) => failure(event.reason));
function raw() {
  return model?.internalModel?.coreModel?.getModel?.();
}
function fingerprint(native) {
  let h = 2166136261;
  for (const p of native.drawables.vertexPositions) {
    const words = new Uint32Array(p.buffer, p.byteOffset, p.length);
    for (const word of words) h = Math.imul(h ^ word, 16777619) >>> 0;
  }
  return h;
}
function snapshot() {
  const n = raw();
  return {
    ready: !!n,
    parameters: lastNativeInputs ? { ...lastNativeInputs } : {},
    parameterReadbackPhase: "actual typed-array inputs before native update",
    isPurism: !!n && n instanceof window.PurismCore.Model,
    nativeError: n?.getLastError?.(),
    meshes: n?.drawables.ids.length,
    vertices: n
      ? Array.from(n.drawables.vertexCounts).reduce((s, v) => s + v, 0)
      : 0,
    geometryFingerprint: n ? fingerprint(n) : null,
    finite: n?.drawables.vertexPositions.every((p) =>
      Array.from(p).every(Number.isFinite),
    ),
    opacities: n?.drawables.ids.map((id, i) => ({
      id,
      opacity: n.drawables.opacities[i],
    })),
    canvas: app
      ? {
          width: app.view.width,
          height: app.view.height,
          resolution: app.renderer.resolution,
        }
      : null,
    softwareRendering,
    gpuRenderer,
    sharedModelAutoUpdate: model?.automator?.autoUpdate ?? null,
    settings: JSON.parse(JSON.stringify(settings)),
    motion: player?.snapshot(),
    clock,
    children: app?.stage.children.length,
    paused: !app?.ticker.started,
    nodeAccessible:
      typeof window.require !== "undefined" ||
      typeof window.process !== "undefined",
    pageURL: location.href,
  };
}
function fit() {
  if (!app || !model || disposed) return;
  const r = $("stage").getBoundingClientRect(),
    width = Math.max(20, Math.round(r.width)),
    height = Math.max(20, Math.round(r.height));
  app.renderer.resize(width, height);
  model.scale.set(1);
  const b = model.getLocalBounds();
  model.scale.set(
    Math.min(width / Math.max(1, b.width), height / Math.max(1, b.height)) *
      0.9 *
      settings.zoom,
  );
  model.anchor.set(0.5, 0.5);
  model.position.set(width / 2, height / 2);
}
function tickerPolicy() {
  if (!app) return;
  if (
    settings.paused ||
    document.hidden ||
    !model ||
    loadingPromise ||
    disposed
  )
    app.ticker.stop();
  else app.ticker.start();
}
function beforeNativeUpdate() {
  const n = raw();
  if (!n) return;
  const values = player.snapshot().values;
  n.parameters.values.set(n.parameters.defaultValues);
  for (const [id, value] of Object.entries(values)) {
    const i = n.parameters.ids.indexOf(id);
    if (i >= 0) n.parameters.values[i] = value;
  }
  if (player.snapshot().id === "idle" && pointer.active)
    for (const [id, value] of [
      ["ParamEyeBallX", pointer.x],
      ["ParamEyeBallY", pointer.y],
    ]) {
      const i = n.parameters.ids.indexOf(id);
      if (i >= 0) n.parameters.values[i] = value;
    }
  lastNativeInputs = Object.fromEntries(
    n.parameters.ids.map((id, i) => [id, n.parameters.values[i]]),
  );
}
function readout() {
  const n = raw();
  if (!n) return;
  const s = player.snapshot();
  $("runtime-status").textContent = `PURISM · ${
    n.parameters.ids.length
  } controls · native ${n.getLastError() === 0 ? "OK" : n.getLastError()}`;
  $("motion-status").textContent = (s.label || "Manual pose") + " · " + s.phase;
  $("motion-time").textContent = s.elapsed.toFixed(2) + " s";
  $("timeline").max = String(Math.max(0.001, s.duration));
  $("timeline").value = String(s.elapsed);
  const blend = lastNativeInputs?.ParamPoseBlend ?? 0;
  $("view-blend").value = String(blend);
  $("view-status").textContent =
    blend < 0.01
      ? "Front view"
      : blend > 0.99
        ? "Explore view"
        : `View crossfade ${(blend * 100).toFixed(0)}%`;
  $("idle").classList.toggle("active", s.id === "idle");
  for (const d of definitions) {
    const value = lastNativeInputs?.[d.id],
      output = document.querySelector(`output[data-value="${d.id}"]`);
    if (output && Number.isFinite(value)) output.textContent = value.toFixed(2);
  }
}
function renderHeld() {
  if (!model || !app) return;
  model.internalModel.update(0, clock * 1000);
  app.renderer.render(app.stage);
  readout();
}
function refreshControls() {
  if (!settings) return;
  $("pause").classList.toggle("active", settings.paused);
  $("pause").textContent = settings.paused ? "Resume" : "Pause";
  $("pin").classList.toggle("active", settings.alwaysOnTop);
  $("pin").setAttribute("aria-pressed", String(settings.alwaysOnTop));
  $("zoom").value = String(settings.zoom);
  document.body.classList.toggle(
    "studio",
    softwareRendering || settings.background === "studio",
  );
  $("background").classList.toggle("active", settings.background === "studio");
  $("background").disabled = softwareRendering;
  if (softwareRendering) {
    $("background").title =
      "Studio: software-WebGL transparent compositing is unreliable";
    document.querySelector(".private-note").textContent =
      "Studio · software WebGL";
  }
  for (const d of definitions) {
    const input = document.querySelector(`input[data-param="${d.id}"]`);
    if (input) input.value = String(settings.params[d.id]);
  }
  tickerPolicy();
  fit();
  if (settings.paused) renderHeld();
}
function persist(patch) {
  saveQueue = saveQueue
    .then(async () => {
      settings = await window.widget.updateSettings(patch);
      refreshControls();
    })
    .catch(failure);
  return saveQueue;
}
function observed() {
  return library.resolve(lastNativeInputs || player.snapshot().values);
}
function manual(patch) {
  saveQueue = saveQueue
    .then(async () => {
      settings = await window.widget.updateSettings({
        params: { ...observed(), ...patch },
      });
      player.hold(settings.params);
      refreshControls();
      renderHeld();
    })
    .catch(failure);
  return saveQueue;
}
function pose(id) {
  saveQueue = saveQueue
    .then(async () => {
      const from = observed();
      settings = await window.widget.selectPreset(id);
      player.transition(from, settings.params, 0.4);
      refreshControls();
      renderHeld();
    })
    .catch(failure);
  return saveQueue;
}
function play(clip, { looped = $("loop").checked } = {}) {
  const from = observed();
  player.play(clip, from, {
    speed: Number($("speed").value),
    looped,
    blendIn: 0.35,
  });
  persist({ paused: false });
  renderHeld();
}
function loadModel() {
  if (loadingPromise) return loadingPromise;
  loadingPromise = performLoad().finally(() => {
    loadingPromise = null;
    tickerPolicy();
  });
  return loadingPromise;
}
async function performLoad() {
  const mine = ++generation;
  $("reload").disabled = true;
  $("loading").hidden = false;
  $("error").hidden = true;
  app.ticker.stop();
  if (model) {
    model.internalModel.off("beforeModelUpdate", beforeNativeUpdate);
    model.destroy();
    model = null;
  }
  app.stage.removeChildren();
  lastNativeInputs = null;
  let candidate;
  try {
    const { Live2DModel } = await import(
      "pixi-live2d-display-lipsyncpatch/cubism4"
    );
    Live2DModel.registerTicker(Ticker);
    candidate = await Live2DModel.from(source, {
      autoInteract: false,
      autoUpdate: false,
    });
    if (
      !window.DTEReleaseGuardQA.guardCubismRendererRelease(
        candidate.internalModel.renderer,
      )
    )
      throw Error("Native release guard missing");
    if (disposed || mine !== generation) {
      candidate.destroy();
      return;
    }
    const n = candidate.internalModel.coreModel.getModel();
    if (
      !(n instanceof window.PurismCore.Model) ||
      n.parameters.ids.length !== definitions.length ||
      !definitions.every((d) => n.parameters.ids.includes(d.id))
    )
      throw Error("Native rig/parameter contract mismatch");
    model = candidate;
    candidate = null;
    app.stage.addChild(model);
    model.internalModel.on("beforeModelUpdate", beforeNativeUpdate);
    renderHeld();
    fit();
    refreshControls();
    $("loading").hidden = true;
  } catch (error) {
    candidate?.destroy();
    if (mine === generation && model) {
      model.internalModel.off("beforeModelUpdate", beforeNativeUpdate);
      model.destroy();
      model = null;
      app.stage.removeChildren();
    }
    throw error;
  } finally {
    if (mine === generation) $("reload").disabled = false;
  }
}
async function boot() {
  const deadline = performance.now() + 15000;
  while (!window.PurismCore || window.Live2DCubismCore !== window.PurismCore) {
    if (performance.now() > deadline)
      throw Error("Verified Purism core unavailable");
    await new Promise((r) => setTimeout(r, 30));
  }
  const initial = await window.widget.getState();
  settings = initial.settings;
  definitions = initial.definitions;
  qa = initial.qa;
  library = compileLibrary(catalog, definitions);
  player = createMotionPlayer(library);
  player.hold(settings.params);
  if (settings.mode === "idle")
    player.play(library.clips.idle, settings.params, {
      looped: true,
      blendIn: 0.35,
    });
  app = new Application({
    view: $("avatar"),
    width: 380,
    height: 580,
    backgroundAlpha: 0,
    antialias: true,
    resolution: Math.min(window.devicePixelRatio || 1, 2),
    autoDensity: true,
    powerPreference: "high-performance",
    autoStart: false,
  });
  app.ticker.maxFPS = 30;
  const gl = app.renderer.gl,
    debug = gl.getExtension("WEBGL_debug_renderer_info");
  gpuRenderer = String(
    gl.getParameter(debug ? debug.UNMASKED_RENDERER_WEBGL : gl.RENDERER),
  );
  softwareRendering = /swiftshader|llvmpipe|softpipe|software/i.test(
    gpuRenderer,
  );
  if (softwareRendering && settings.background !== "studio")
    settings = await window.widget.updateSettings({ background: "studio" });
  for (const d of definitions) {
    const row = document.createElement("div");
    row.className = "parameter-row";
    const label = document.createElement("label");
    label.textContent = d.id.replace(/^Param/, "");
    label.htmlFor = "control-" + d.id;
    const input = document.createElement("input");
    input.type = "range";
    input.id = "control-" + d.id;
    input.dataset.param = d.id;
    input.min = d.min;
    input.max = d.max;
    input.step = (d.max - d.min) / 200;
    input.value = settings.params[d.id];
    input.addEventListener("input", () =>
      manual({ [d.id]: Number(input.value) }),
    );
    const output = document.createElement("output");
    output.dataset.value = d.id;
    output.textContent = Number(settings.params[d.id]).toFixed(2);
    row.append(label, input, output);
    $("sliders").append(row);
  }
  for (const p of Object.values(library.poses)) {
    const option = document.createElement("option");
    option.value = p.id;
    option.textContent = p.label;
    $("pose-select").append(option);
  }
  for (const c of Object.values(library.clips)) {
    const option = document.createElement("option");
    option.value = c.id;
    option.textContent = c.label;
    $("gesture-select").append(option);
  }
  $("gesture-select").value = "wave";
  $("loop").checked = true;
  $("idle").addEventListener("click", () => {
    player.play(library.clips.idle, observed(), {
      looped: true,
      blendIn: 0.35,
    });
    persist({ mode: "idle", paused: false });
  });
  $("pause").addEventListener("click", () => {
    if (settings.paused) player.resume();
    persist({ paused: !settings.paused });
  });
  $("pin").addEventListener("click", () =>
    persist({ alwaysOnTop: !settings.alwaysOnTop }),
  );
  $("background").addEventListener("click", () =>
    persist({
      background: settings.background === "studio" ? "transparent" : "studio",
    }),
  );
  $("zoom").addEventListener("input", () =>
    persist({ zoom: Number($("zoom").value) }),
  );
  $("controls").addEventListener("click", () => {
    $("inspector").hidden = !$("inspector").hidden;
    $("controls").setAttribute("aria-expanded", String(!$("inspector").hidden));
    fit();
  });
  $("reload").addEventListener("click", () => loadModel().catch(failure));
  $("minimize").addEventListener("click", () =>
    window.widget.minimize().catch(failure),
  );
  $("close").addEventListener("click", () =>
    window.widget.close().catch(failure),
  );
  $("pose-apply").addEventListener("click", () => pose($("pose-select").value));
  $("play").addEventListener("click", () =>
    play(library.clips[$("gesture-select").value]),
  );
  $("stop").addEventListener("click", () => manual({}));
  $("view-blend").addEventListener("input", () =>
    manual({ ParamPoseBlend: Number($("view-blend").value) }),
  );
  $("sequence").addEventListener("click", () =>
    play(
      composeMotions(
        ["look", "wave", "step", "present"].map((id) => library.clips[id]),
      ),
      { looped: false },
    ),
  );
  $("timeline").addEventListener("input", () => {
    player.seek(Number($("timeline").value));
    persist({ paused: true });
    renderHeld();
  });
  $("avatar").addEventListener("pointermove", (e) => {
    const r = $("avatar").getBoundingClientRect();
    pointer = {
      active: true,
      x: Math.max(-1, Math.min(1, ((e.clientX - r.left) / r.width) * 2 - 1)),
      y: Math.max(-1, Math.min(1, 1 - ((e.clientY - r.top) / r.height) * 2)),
    };
  });
  $("avatar").addEventListener("pointerleave", () => {
    pointer.active = false;
  });
  resizeObserver = new ResizeObserver(fit);
  resizeObserver.observe($("stage"));
  document.addEventListener("visibilitychange", tickerPolicy);
  app.ticker.add(() => {
    const dt = Math.min(app.ticker.deltaMS, 100);
    clock += dt / 1000;
    player.step(dt / 1000);
    model?.update(dt);
    const now = performance.now();
    if (now - lastReadout > 100) {
      readout();
      lastReadout = now;
    }
  });
  await loadModel();
  if (qa)
    window.__widgetQA = Object.freeze({
      snapshot,
      reload: loadModel,
      settled: () => saveQueue,
      seek: (t) => {
        player.seek(t);
        renderHeld();
        return snapshot();
      },
      sampleGesture: (id, t) => {
        player.play(library.clips[id], observed(), {
          blendIn: 0,
          looped: false,
        });
        player.seek(t);
        renderHeld();
        return snapshot();
      },
    });
}
function dispose() {
  if (disposed) return;
  disposed = true;
  ++generation;
  resizeObserver?.disconnect();
  document.removeEventListener("visibilitychange", tickerPolicy);
  if (model) {
    model.internalModel.off("beforeModelUpdate", beforeNativeUpdate);
    model.destroy();
    model = null;
  }
  app?.destroy(true);
  app = null;
}
window.addEventListener("beforeunload", dispose, { once: true });
boot().catch(failure);
