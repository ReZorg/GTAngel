/**
 * AttentionIso — Phase-Locked Luminescence
 * The attention isomorphism: four correspondences rendered as a
 * mapping console — KSM mechanism ↔ transformer mechanism.
 */
import { ATTENTION_ISO } from "@/lib/engineData";

export default function AttentionIso() {
  return (
    <div className="space-y-px">
      {ATTENTION_ISO.map((row, i) => (
        <div
          key={i}
          className="grid md:grid-cols-[1fr_auto_1fr] gap-3 md:gap-6 items-center border border-border bg-card/50 hover:bg-accent/30 transition-colors p-5">
          <div>
            <div className="overline-label mb-1">KSM ENGINE</div>
            <div className="font-display font-medium text-base md:text-lg text-foreground">
              {row.ksm}
            </div>
          </div>
          <div
            className="font-display text-2xl text-center hidden md:block"
            style={{ color: "var(--gold)" }}
            aria-hidden>
            ≅
          </div>
          <div>
            <div className="overline-label mb-1">TRANSFORMER</div>
            <div
              className="font-display font-medium text-base md:text-lg"
              style={{ color: "var(--thread-a)" }}>
              {row.attn}
            </div>
          </div>
          <p className="md:col-span-3 text-sm text-muted-foreground leading-relaxed max-w-[80ch]">
            {row.note}
          </p>
        </div>
      ))}
    </div>
  );
}
