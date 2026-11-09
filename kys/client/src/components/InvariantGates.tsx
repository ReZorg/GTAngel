/**
 * InvariantGates — Phase-Locked Luminescence
 * Six angular chevron interlock badges. On each global tick, one gate
 * cycles through "checking" (amber) then flips to gate-green pass —
 * the perpetual re-verification of the six invariants. Click a gate
 * to expand its statement + detail.
 */
import { useState } from "react";
import { INVARIANTS } from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from "@/components/ui/tooltip";

export default function InvariantGates() {
  const { tick } = useTick();
  const [open, setOpen] = useState<string | null>(null);

  // the gate currently being re-checked this tick
  const checking = tick % INVARIANTS.length;
  const openInv = INVARIANTS.find((v) => v.id === open);

  return (
    <TooltipProvider delayDuration={150}>
    <div>
      <div className="grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-6 gap-3">
        {INVARIANTS.map((inv, i) => {
          const isChecking = checking === i;
          const isOpen = open === inv.id;
          const color = isChecking ? "oklch(0.8 0.14 75)" : "var(--gate-green)";
          return (
            <Tooltip key={inv.id}>
            <TooltipTrigger asChild>
            <button
              onClick={() => setOpen(isOpen ? null : inv.id)}
              className={`relative text-left p-3 pt-4 border transition-all duration-200 ${
                isOpen ? "bg-accent/60" : "bg-card/60 hover:bg-accent/40"
              }`}
              style={{
                borderColor: isOpen
                  ? color
                  : "oklch(0.87 0.01 250 / 14%)",
                clipPath:
                  "polygon(0 0, calc(100% - 14px) 0, 100% 14px, 100% 100%, 14px 100%, 0 calc(100% - 14px))",
              }}
              aria-expanded={isOpen}>
              {/* chevron indicator */}
              <div
                key={`${inv.id}-${isChecking ? "chk" : "ok"}`}
                className="chevron-flip flex items-center gap-2"
                style={{ color }}>
                <svg width="18" height="14" viewBox="0 0 18 14" aria-hidden>
                  <path
                    d="M1 1 L9 7 L1 13 M9 1 L17 7 L9 13"
                    fill="none"
                    stroke="currentColor"
                    strokeWidth="2"
                    strokeLinecap="round"
                    style={{
                      filter: `drop-shadow(0 0 4px ${color})`,
                    }}
                  />
                </svg>
                <span className="font-mono text-[10px] font-semibold tracking-widest">
                  {isChecking ? "CHECK" : "PASS"}
                </span>
              </div>
              <div className="font-mono text-[10px] text-muted-foreground mt-2">
                {inv.id}
              </div>
              <div className="font-display font-medium text-sm text-foreground leading-tight mt-0.5">
                {inv.name}
              </div>
            </button>
            </TooltipTrigger>
            <TooltipContent
              side="top"
              className="max-w-xs border border-border bg-popover/95 backdrop-blur-md p-0 rounded-none"
              style={{
                clipPath:
                  "polygon(0 0, calc(100% - 10px) 0, 100% 10px, 100% 100%, 0 100%)",
              }}>
              <div className="px-3 py-2 border-b border-border font-mono text-[10px] tracking-widest text-muted-foreground">
                {inv.id} · VALIDATION LOGIC
              </div>
              <pre className="px-3 py-2 font-mono text-[10px] leading-relaxed whitespace-pre-wrap" style={{ color: "var(--thread-a)" }}>
                {inv.logic}
              </pre>
              <div className="px-3 py-2 border-t border-border font-mono text-[10px]" style={{ color: "var(--gate-green)" }}>
                {inv.faults}
              </div>
            </TooltipContent>
            </Tooltip>
          );
        })}
      </div>

      {/* expanded detail panel */}
      {openInv && (
        <div
          className="mt-4 border border-border bg-card/80 p-5 backdrop-blur-sm"
          style={{
            clipPath:
              "polygon(0 0, calc(100% - 18px) 0, 100% 18px, 100% 100%, 0 100%)",
          }}>
          <div className="font-mono text-xs" style={{ color: "var(--gate-green)" }}>
            {openInv.id} · VERIFIED 36/36 TICKS
          </div>
          <div className="font-display font-medium text-lg text-foreground mt-1">
            {openInv.statement}
          </div>
          <p className="text-sm text-muted-foreground leading-relaxed mt-2 max-w-[70ch]">
            {openInv.detail}
          </p>
          <div className="mt-3 font-mono text-[11px] flex flex-wrap gap-x-6 gap-y-1">
            <span style={{ color: "var(--thread-a)" }}>{openInv.logic}</span>
            <span style={{ color: "var(--gate-green)" }}>{openInv.faults}</span>
          </div>
        </div>
      )}
    </div>
    </TooltipProvider>
  );
}
