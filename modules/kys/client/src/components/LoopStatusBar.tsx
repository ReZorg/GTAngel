/**
 * LoopStatusBar — Phase-Locked Luminescence
 * A persistent, thin loop-grammar strip that sits in the header:
 * the five stations in order with a live token dot circulating on
 * the shared tick clock, plus the return glyph closing the loop.
 * Keeps the closed loop recognizable even when the hero phasor is
 * offscreen.
 */
import { PIPELINE_STAGES } from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

export default function LoopStatusBar() {
  const { tick } = useTick();
  const at = tick % PIPELINE_STAGES.length;

  return (
    <div className="hidden lg:flex items-center gap-0 font-mono text-[10px] select-none">
      {PIPELINE_STAGES.map((st, i) => {
        const active = at === i;
        return (
          <div key={st.id} className="flex items-center">
            <a
              href={`#${i === 0 ? "pipeline" : "pipeline"}`}
              className="flex items-center gap-1.5 px-2 py-1 transition-colors"
              style={{
                color: active
                  ? "var(--thread-a)"
                  : "oklch(0.62 0.015 250 / 75%)",
              }}>
              <span
                className="w-1.5 h-1.5 rounded-full"
                style={{
                  background: active
                    ? "var(--thread-a)"
                    : "oklch(0.62 0.015 250 / 35%)",
                  boxShadow: active ? "0 0 6px var(--thread-a)" : "none",
                  transition: "all 300ms",
                }}
              />
              {st.name.toUpperCase().replace("INVARIANT ", "")}
            </a>
            {i < PIPELINE_STAGES.length - 1 && (
              <span className="text-muted-foreground/40">→</span>
            )}
          </div>
        );
      })}
      <span className="ml-1" style={{ color: "var(--gate-green)" }}>
        ↺
      </span>
    </div>
  );
}
