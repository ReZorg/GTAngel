/**
 * TickControls — Phase-Locked Luminescence
 * Operator switchgear for the shared tick clock: run / pause / step,
 * plus the live tick readout. Controls feel like interlocks — instant
 * response, tactile press states.
 */
import {
  CANONICAL_WORD,
  RUN_STATS,
  THREADS,
  threadStep,
  universalAt,
} from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

export default function TickControls() {
  const { tick, running, play, pause, step, jumpTo, tickMs, setTickMs } =
    useTick();
  const stepTriple = THREADS.map(
    (th) => ((tick + th.offset) % 12) + 1
  ).join(",");
  const corpusTick = tick % RUN_STATS.ticks;
  const { u1, u2 } = universalAt(tick);

  return (
    <div className="inline-flex flex-col border border-border bg-card/70 backdrop-blur-sm font-mono text-xs max-w-full">
      <div className="flex flex-wrap items-center gap-px">
      <button
        onClick={running ? pause : play}
        className="px-4 py-2.5 hover:bg-accent active:scale-95 transition-all font-semibold tracking-widest"
        style={{ color: running ? "var(--thread-a)" : "var(--gold)" }}>
        {running ? "❚❚ HOLD" : "▶ RUN"}
      </button>
      <div className="w-px self-stretch bg-border" />
      <button
        onClick={step}
        className="px-4 py-2.5 hover:bg-accent active:scale-95 transition-all tracking-widest text-secondary-foreground">
        ⇥ STEP
      </button>
      <div className="w-px self-stretch bg-border" />
      <div className="px-4 py-2.5 text-muted-foreground">
        g=<span className="text-foreground">{tick}</span>
        <span className="mx-2 opacity-40">·</span>
        steps{" "}
        <span className="text-foreground">{`{${stepTriple}}`}</span>
      </div>
      <div className="w-px self-stretch bg-border" />
      <div className="px-4 py-2.5 flex items-center gap-2">
        {THREADS.map((th) => {
          const w = CANONICAL_WORD[threadStep(tick, th.offset / 4)];
          return (
            <span key={th.id} style={{ color: th.colorVar }}>
              {th.id}:{w}
            </span>
          );
        })}
        <span className="opacity-40">·</span>
        <span style={{ color: "var(--gold)" }}>U1:{u1}</span>
        <span style={{ color: "var(--gold)", opacity: 0.7 }}>U2:{u2}</span>
      </div>
      </div>

      {/* scrubber row — drag through the 36 corpus ticks */}
      <div className="flex flex-wrap items-center gap-3 px-4 py-2.5 border-t border-border">
        <span className="text-muted-foreground tracking-widest text-[10px]">
          SCRUB
        </span>
        <input
          type="range"
          min={0}
          max={RUN_STATS.ticks - 1}
          value={corpusTick}
          onChange={(e) => jumpTo(Number(e.target.value))}
          className="tick-scrubber flex-1 min-w-[140px]"
          aria-label="Scrub through the 36 corpus ticks"
        />
        <span className="text-foreground tabular-nums w-14">
          {String(corpusTick).padStart(2, "0")}/{RUN_STATS.ticks - 1}
          <span className="text-muted-foreground ml-1">
            c{Math.floor(corpusTick / 12) + 1}
          </span>
        </span>
        <span className="w-px self-stretch bg-border" />
        <span className="text-muted-foreground tracking-widest text-[10px]">
          RATE
        </span>
        <input
          type="range"
          min={200}
          max={4000}
          step={100}
          /* inverted: right = faster */
          value={4200 - tickMs}
          onChange={(e) => setTickMs(4200 - Number(e.target.value))}
          className="tick-scrubber w-24"
          aria-label="Tick rate"
        />
        <span className="text-muted-foreground tabular-nums">
          {(tickMs / 1000).toFixed(1)}s
        </span>
      </div>
    </div>
  );
}
