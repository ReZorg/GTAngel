/**
 * CorpusStream — Phase-Locked Luminescence
 * A vertically scrolling stream of the real ksm_corpus.jsonl records
 * (first full cycle, verbatim). Hover pauses. Syntax-tinted mono.
 */
import { CORPUS_SAMPLE } from "@/lib/engineData";

function tintRecord(line: string) {
  // lightweight tinting: triad terms cyan, U-policy gold, keys muted
  const parts = line.split(/("(?:U[12]_Policy_UT\d[ER_]?)"|"UT\d[ER_]"|"T\d[ER]"|"tick"|"steps"|"triad"|"u"|"interfaces")/g);
  return parts.map((p, i) => {
    if (/^"T\d[ER]"$/.test(p))
      return (
        <span key={i} style={{ color: "var(--thread-a)" }}>
          {p}
        </span>
      );
    if (/U[12]_Policy|^"UT\d[ER_]"$/.test(p))
      return (
        <span key={i} style={{ color: "var(--gold)" }}>
          {p}
        </span>
      );
    if (/^"(tick|steps|triad|u|interfaces)"$/.test(p))
      return (
        <span key={i} className="text-muted-foreground">
          {p}
        </span>
      );
    return <span key={i}>{p}</span>;
  });
}

export default function CorpusStream() {
  const doubled = [...CORPUS_SAMPLE, ...CORPUS_SAMPLE];
  return (
    <div
      className="relative h-[340px] overflow-hidden border border-border bg-card/60"
      role="figure"
      aria-label="Live scroll of ksm_corpus.jsonl training records">
      {/* header strip */}
      <div className="absolute top-0 left-0 right-0 z-10 flex items-center justify-between px-4 py-2 border-b border-border bg-background/85 backdrop-blur-sm">
        <span className="font-mono text-[11px] text-muted-foreground">
          ksm_corpus.jsonl · 36 records · 3 cycles · verbatim
        </span>
        <span
          className="font-mono text-[11px]"
          style={{ color: "var(--gate-green)" }}>
          ● GATE OPEN
        </span>
      </div>

      {/* fades */}
      <div className="absolute inset-x-0 top-9 h-10 z-[5] bg-gradient-to-b from-background/90 to-transparent pointer-events-none" />
      <div className="absolute inset-x-0 bottom-0 h-12 z-[5] bg-gradient-to-t from-background/90 to-transparent pointer-events-none" />

      <div className="corpus-scroll pt-12 px-4 space-y-2">
        {doubled.map((rec, i) => (
          <pre
            key={i}
            className="font-mono text-[11px] leading-snug whitespace-pre-wrap break-all text-secondary-foreground/80 border-l pl-3"
            style={{ borderColor: "oklch(0.82 0.13 210 / 25%)" }}>
            {tintRecord(rec)}
          </pre>
        ))}
      </div>
    </div>
  );
}
