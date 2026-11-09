/**
 * PipelineLoop — Phase-Locked Luminescence
 * The closed-loop pipeline: 5 stage stations along a vertical rail,
 * with animated data-packet dots (tick-synchronized) and expandable
 * operator consoles. The loop visually closes: a return rail from
 * Redeploy back to Experience.
 */
import { useState } from "react";
import { PIPELINE_STAGES } from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

export default function PipelineLoop() {
  const { tick } = useTick();
  const [open, setOpen] = useState<string | null>("gate");

  // packet position cycles through the 5 stations tick by tick
  const packetAt = tick % PIPELINE_STAGES.length;

  return (
    <div className="relative">
      {/* return rail — the loop closing, right side */}
      <div
        className="hidden lg:block absolute top-6 bottom-6 -right-6 w-px"
        style={{
          background:
            "linear-gradient(180deg, oklch(0.75 0.17 155 / 40%), oklch(0.82 0.13 210 / 25%))",
        }}
        aria-hidden>
        {/* upward-travelling return packet */}
        <div
          className="packet-dot absolute w-2 h-2 -left-[3.5px] rounded-full"
          style={{
            background: "var(--gate-green)",
            boxShadow: "0 0 8px var(--gate-green)",
            top: `${100 - ((tick * 20) % 100)}%`,
            transition: "top 1.6s linear",
          }}
        />
        <div
          className="absolute -top-3 -left-1.5 font-mono text-[9px] rotate-90 origin-left whitespace-nowrap"
          style={{ color: "oklch(0.75 0.17 155 / 70%)" }}>
          ↺ weights return
        </div>
      </div>

      <div className="space-y-0">
        {PIPELINE_STAGES.map((st, i) => {
          const isOpen = open === st.id;
          const hasPacket = packetAt === i;
          return (
            <div key={st.id} className="relative flex gap-6">
              {/* rail column */}
              <div className="flex flex-col items-center w-10 shrink-0">
                <div
                  className="w-4 h-4 rounded-full border-2 mt-1 shrink-0"
                  style={{
                    borderColor: hasPacket
                      ? "var(--thread-a)"
                      : "oklch(0.87 0.01 250 / 30%)",
                    background: hasPacket
                      ? "var(--thread-a)"
                      : "var(--background)",
                    boxShadow: hasPacket
                      ? "0 0 10px var(--thread-a)"
                      : "none",
                    transition: "all 400ms cubic-bezier(0.77,0,0.175,1)",
                  }}
                />
                {i < PIPELINE_STAGES.length - 1 && (
                  <div
                    className="w-px flex-1 my-1"
                    style={{
                      background:
                        "linear-gradient(180deg, oklch(0.82 0.13 210 / 30%), oklch(0.87 0.01 250 / 10%))",
                    }}
                  />
                )}
              </div>

              {/* station card */}
              <div className="pb-10 flex-1 min-w-0">
                <button
                  onClick={() => setOpen(isOpen ? null : st.id)}
                  className="text-left w-full group"
                  aria-expanded={isOpen}>
                  <div className="flex items-baseline gap-4">
                    <span
                      className="font-display font-bold text-4xl md:text-5xl leading-none"
                      style={{
                        color: hasPacket
                          ? "var(--thread-a)"
                          : "oklch(0.87 0.01 250 / 22%)",
                        transition: "color 400ms",
                      }}>
                      {st.n}
                    </span>
                    <div>
                      <h3 className="font-display font-medium text-xl md:text-2xl text-foreground group-hover:text-primary transition-colors">
                        {st.name}
                        <span className="font-mono text-xs ml-3 text-muted-foreground">
                          {isOpen ? "▾ console" : "▸ console"}
                        </span>
                      </h3>
                      <p className="font-mono text-xs text-muted-foreground mt-1">
                        {st.tagline}
                      </p>
                    </div>
                  </div>
                </button>

                <p className="text-sm text-secondary-foreground/90 leading-relaxed mt-3 max-w-[62ch]">
                  {st.desc}
                </p>

                {/* operator console */}
                {isOpen && (
                  <div
                    className="mt-4 rounded-sm border border-border bg-card/80 backdrop-blur-sm overflow-x-auto"
                    style={{
                      boxShadow: "inset 0 1px 0 oklch(0.87 0.01 250 / 6%)",
                    }}>
                    <div className="flex items-center gap-1.5 px-3 py-2 border-b border-border">
                      <span className="w-2 h-2 rounded-full bg-destructive/60" />
                      <span
                        className="w-2 h-2 rounded-full"
                        style={{ background: "oklch(0.8 0.14 75 / 60%)" }}
                      />
                      <span
                        className="w-2 h-2 rounded-full"
                        style={{ background: "oklch(0.75 0.17 155 / 60%)" }}
                      />
                      <span className="font-mono text-[10px] text-muted-foreground ml-2">
                        stage://{st.id}
                      </span>
                    </div>
                    <pre className="p-4 font-mono text-xs leading-relaxed whitespace-pre">
                      {st.console.map((line, li) => (
                        <div
                          key={li}
                          style={{
                            color: line.startsWith("$")
                              ? "var(--thread-a)"
                              : line.includes("PASS") ||
                                  line.includes("GATE OPEN") ||
                                  line.includes("✓")
                                ? "var(--gate-green)"
                                : "oklch(0.75 0.01 250)",
                          }}>
                          {line}
                        </div>
                      ))}
                    </pre>
                  </div>
                )}
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}
