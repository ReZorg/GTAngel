/**
 * WindowEncoding — Phase-Locked Luminescence
 * The 5×4 single-window encoding: U1 + U2 + the three concurrent
 * Particular rows. One 4-step window encodes the entire 3 cycles of 12 —
 * at any tick the three threads occupy the same column of the three word
 * rows (one row = one 4-step offset). Toggle between the base alignment
 * and the 2-step shift that centers the T6 symmetry.
 */
import { useState } from "react";
import { WINDOW_BASE, WINDOW_CENTERED, THREADS } from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

export default function WindowEncoding() {
  const { tick } = useTick();
  const [centered, setCentered] = useState(true);
  const win = centered ? WINDOW_CENTERED : WINDOW_BASE;

  // live column: window column = tick mod 4
  const liveCol = tick % 4;

  const cellColor = (v: string) => {
    if (v.startsWith("T9") || v.startsWith("T8R")) return "var(--gold)";
    if (v.includes("_")) return "var(--gold)";
    return undefined;
  };

  return (
    <div className="border border-border bg-card/60 backdrop-blur-sm">
      <div className="flex items-center justify-between px-4 py-2.5 border-b border-border font-mono text-[11px]">
        <span className="text-muted-foreground">
          5×4 window · one 4-step window encodes 3×12
        </span>
        <div className="flex items-center gap-px border border-border">
          <button
            onClick={() => setCentered(false)}
            className={`px-3 py-1 transition-colors ${!centered ? "bg-accent text-foreground" : "text-muted-foreground hover:text-foreground"}`}>
            BASE
          </button>
          <div className="w-px self-stretch bg-border" />
          <button
            onClick={() => setCentered(true)}
            className={`px-3 py-1 transition-colors ${centered ? "bg-accent text-foreground" : "text-muted-foreground hover:text-foreground"}`}>
            T6-CENTERED
          </button>
        </div>
      </div>

      <div className="p-4 overflow-x-auto">
        <table className="font-mono text-sm border-collapse w-full">
          <thead>
            <tr>
              <th className="text-left text-[10px] text-muted-foreground font-normal pr-4 pb-2">
                ROW
              </th>
              {[0, 1, 2, 3].map((c) => (
                <th
                  key={c}
                  className="text-center text-[10px] font-normal pb-2 px-3"
                  style={{
                    color:
                      c === liveCol
                        ? "var(--thread-a)"
                        : "oklch(0.62 0.015 250)",
                  }}>
                  w{c + 1}
                  {c === liveCol ? " ●" : ""}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {/* U1 row */}
            <tr className="border-t border-border/60">
              <td className="pr-4 py-2 text-[11px]" style={{ color: "var(--gold)" }}>
                U1
              </td>
              {win.u1.map((v, c) => (
                <td
                  key={c}
                  className="text-center py-2 px-3 transition-colors"
                  style={{
                    color: "var(--gold)",
                    background:
                      c === liveCol ? "oklch(0.82 0.13 210 / 8%)" : undefined,
                  }}>
                  {v}
                </td>
              ))}
            </tr>
            {/* U2 row */}
            <tr className="border-t border-border/60">
              <td className="pr-4 py-2 text-[11px]" style={{ color: "var(--gold)", opacity: 0.7 }}>
                U2
              </td>
              {win.u2.map((v, c) => (
                <td
                  key={c}
                  className="text-center py-2 px-3 transition-colors"
                  style={{
                    color: "var(--gold)",
                    opacity: 0.75,
                    background:
                      c === liveCol ? "oklch(0.82 0.13 210 / 8%)" : undefined,
                  }}>
                  {v}
                </td>
              ))}
            </tr>
            {/* three Particular rows — thread-colored */}
            {win.rows.map((row, r) => (
              <tr key={r} className="border-t border-border/60">
                <td
                  className="pr-4 py-2 text-[11px]"
                  style={{ color: THREADS[r].colorVar }}>
                  P{r + 1}
                </td>
                {row.map((v, c) => {
                  const isLive = c === liveCol;
                  return (
                    <td
                      key={c}
                      className="text-center py-2 px-3 transition-colors"
                      style={{
                        color: isLive
                          ? THREADS[r].colorVar
                          : cellColor(v) ?? "oklch(0.87 0.01 250 / 80%)",
                        background: isLive
                          ? "oklch(0.82 0.13 210 / 8%)"
                          : undefined,
                        textShadow: isLive
                          ? `0 0 8px ${THREADS[r].colorVar}`
                          : undefined,
                        fontWeight: isLive ? 600 : 400,
                      }}>
                      {v}
                      <span className="block text-[9px] text-muted-foreground font-normal">
                        s{win.stepAt(r, c)}
                      </span>
                    </td>
                  );
                })}
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <div className="px-4 py-2.5 border-t border-border font-mono text-[11px] text-muted-foreground leading-relaxed">
        {centered ? (
          <>
            shifted −2 to center the{" "}
            <span style={{ color: "var(--gold)" }}>T6 symmetry</span> — U2's
            twin T6 cells sit in the middle columns, U1's 2-2 alternation
            frames them
          </>
        ) : (
          <>
            base alignment — word rows are steps 1-4 / 5-8 / 9-12; each
            column is one tick's triad (threads read the same column, one
            row apart)
          </>
        )}
      </div>
    </div>
  );
}
