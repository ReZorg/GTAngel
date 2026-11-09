import React from "react";
import { useTick } from "@/contexts/TickContext";
import { trpc } from "@/lib/trpc";

export default function SylvesterNonionPanel() {
  const { tick } = useTick();

  const trpcRun = trpc.ksm.sylvesterRun.useQuery(undefined, { staleTime: 60_000 });
  const trpcInvariants = trpc.ksm.sylvesterInvariants.useQuery(undefined, { staleTime: 60_000 });

  const run = trpcRun.data;
  const invariants = trpcInvariants.data;

  if (!run || !run.length || !invariants) {
    return (
      <div className="p-4 border border-border font-mono text-xs text-muted-foreground animate-pulse">
        Loading Sylvester Nonion Clock-Tower telemetry…
      </div>
    );
  }

  const currentRec = run[tick % run.length];

  return (
    <div className="space-y-4">
      <div className="grid grid-cols-2 gap-3 font-mono text-xs">
        <div className="border border-border bg-background/50 p-3">
          <div className="text-[10px] text-muted-foreground uppercase tracking-wider mb-1">
            Clock Tower Frame (Tick {tick})
          </div>
          <div className="text-sm font-bold text-amber-400">
            S₃<sup>{currentRec.frameShift.clockPower}</sup> · T₃<sup>{currentRec.frameShift.shiftPower}</sup>
          </div>
          <div className="text-[11px] text-muted-foreground mt-1">
            Cycle {currentRec.cycle} · Phase {tick % 12}/12
          </div>
        </div>

        <div className="border border-border bg-background/50 p-3">
          <div className="text-[10px] text-muted-foreground uppercase tracking-wider mb-1">
            Nonion Norm & Trace
          </div>
          <div className="text-sm font-bold text-cyan-400">
            Norm: {currentRec.nicheNorm.toFixed(3)}
          </div>
          <div className="text-[11px] text-muted-foreground mt-1">
            Trace: {currentRec.nicheTrace.re.toFixed(2)} + {currentRec.nicheTrace.im.toFixed(2)}i
          </div>
        </div>
      </div>

      {/* 3-5 Bilateral Correspondence Indicator */}
      <div className="border border-border bg-card/40 p-3 rounded">
        <div className="font-mono text-[10px] text-muted-foreground uppercase tracking-wider mb-2 flex items-center justify-between">
          <span>Dual Ternary-Quinary Lie Algebra (g₃,₅)</span>
          <span className="text-emerald-400">DNA / Icosi-Dodec 3-5 Match</span>
        </div>
        <div className="grid grid-cols-3 gap-2 font-mono text-center text-xs">
          <div className="border border-border p-2 bg-background/40">
            <div className="text-muted-foreground text-[10px]">Ternary (Z₃)</div>
            <div className="text-cyan-400 font-bold">20 Faces × 3</div>
          </div>
          <div className="border border-border p-2 bg-background/40">
            <div className="text-muted-foreground text-[10px]">Dual Bilateral Edges</div>
            <div className="text-amber-400 font-bold">60 Edges</div>
          </div>
          <div className="border border-border p-2 bg-background/40">
            <div className="text-muted-foreground text-[10px]">Quinary (Z₅)</div>
            <div className="text-magenta-400 font-bold" style={{ color: "var(--thread-b)" }}>12 Faces × 5</div>
          </div>
        </div>
      </div>

      {/* Invariants list */}
      <div className="border border-border bg-background/60 p-3">
        <div className="font-mono text-[10px] text-muted-foreground uppercase tracking-wider mb-2">
          Sylvester & Dual Lie Invariants
        </div>
        <div className="grid grid-cols-2 gap-1.5 font-mono text-[11px]">
          {invariants.map((inv) => {
            const isPass = inv.faults === 0;
            return (
              <div key={inv.invariantId} className="flex items-center justify-between border-b border-border/40 py-1">
                <span className="text-muted-foreground">{inv.invariantId} {inv.name}</span>
                <span className={isPass ? "text-emerald-400 font-bold" : "text-rose-400 font-bold"}>
                  {isPass ? "PASS" : `FAIL (${inv.faults})`}
                </span>
              </div>
            );
          })}
        </div>
      </div>
    </div>
  );
}
