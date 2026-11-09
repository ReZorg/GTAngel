/**
 * SteppingRulePanel — Phase-Locked Luminescence
 * The stepping rule made visible as a live instrument face:
 * a 3×12 mask-rotation matrix. Row k = thread k; column g = tick;
 * cell = word[(g + 4k) % 12]. The current tick column is illuminated
 * and sweeps with the shared clock — the causal mask, rotating.
 */
import {
  CANONICAL_WORD,
  THREADS,
  threadStep,
} from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

export default function SteppingRulePanel() {
  const { tick } = useTick();
  const col = tick % 12;

  return (
    <div className="relative border border-border bg-card/60 backdrop-blur-sm">
      {/* header strip */}
      <div className="flex items-center justify-between px-4 py-2.5 border-b border-border">
        <span className="font-mono text-[11px] text-muted-foreground">
          stepping rule · word[(g + 4k) % 12]
        </span>
        <span className="font-mono text-[11px]" style={{ color: "var(--thread-a)" }}>
          g={tick} → col {col + 1}
        </span>
      </div>

      <div className="p-4 overflow-x-auto">
        <table className="font-mono text-[11px] border-separate border-spacing-1 min-w-full">
          <thead>
            <tr>
              <th className="text-left text-muted-foreground font-normal pr-2 pb-1">
                g→
              </th>
              {Array.from({ length: 12 }).map((_, g) => (
                <th
                  key={g}
                  className="font-normal pb-1 text-center min-w-[34px]"
                  style={{
                    color:
                      g === col
                        ? "oklch(0.95 0.02 90)"
                        : "oklch(0.62 0.015 250 / 60%)",
                  }}>
                  {g}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {THREADS.map((th, k) => (
              <tr key={th.id}>
                <td className="pr-2 whitespace-nowrap" style={{ color: th.colorVar }}>
                  {th.id}·{th.phase}
                </td>
                {Array.from({ length: 12 }).map((_, g) => {
                  const w = CANONICAL_WORD[threadStep(g, k)];
                  const active = g === col;
                  const isT8 = w.startsWith("T8");
                  return (
                    <td
                      key={g}
                      className="text-center px-1 py-1 border transition-all duration-300"
                      style={{
                        borderColor: active
                          ? th.colorVar
                          : "oklch(0.87 0.01 250 / 8%)",
                        background: active
                          ? `color-mix(in oklch, ${th.colorVar} 12%, transparent)`
                          : "transparent",
                        color: active
                          ? th.colorVar
                          : isT8
                            ? "oklch(0.83 0.12 90 / 75%)"
                            : w.endsWith("E")
                              ? "oklch(0.87 0.01 250 / 65%)"
                              : "oklch(0.62 0.1 295 / 75%)",
                        boxShadow: active ? `0 0 8px color-mix(in oklch, ${th.colorVar} 35%, transparent)` : "none",
                      }}>
                      {w}
                    </td>
                  );
                })}
              </tr>
            ))}
            {/* budget row */}
            <tr>
              <td className="pr-2 text-muted-foreground pt-2">Σ</td>
              <td
                colSpan={12}
                className="pt-2 text-muted-foreground text-left pl-1">
                each row: <span style={{ color: "var(--mode-e)" }}>7E</span> +{" "}
                <span style={{ color: "var(--mode-r)" }}>5R</span> — the budget
                only holds because rows are <em>offsets</em>, not columns
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      {/* footer: the anti-pattern warning */}
      <div className="px-4 py-2.5 border-t border-border font-mono text-[10px] text-muted-foreground">
        ⚠ fixed step-columns break the budget → 12E/0R · 6E/6R · 3E/9R
      </div>
    </div>
  );
}
