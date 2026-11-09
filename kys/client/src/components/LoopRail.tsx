/**
 * LoopRail — Phase-Locked Luminescence
 * Fixed left rail: the pipeline loop as a live minimap. The user's
 * scroll position is the token flowing around the loop. Desktop only.
 */
import { useEffect, useState } from "react";
import { PIPELINE_STAGES } from "@/lib/engineData";

const SECTIONS = [
  { id: "hero", label: "∴" },
  ...PIPELINE_STAGES.map((s) => ({ id: `stage-${s.id}`, label: s.n })),
  { id: "isomorphism", label: "≅" },
];

export default function LoopRail() {
  const [activeIdx, setActiveIdx] = useState(0);

  useEffect(() => {
    const onScroll = () => {
      const mid = window.innerHeight * 0.4;
      let best = 0;
      SECTIONS.forEach((s, i) => {
        const el = document.getElementById(s.id);
        if (el && el.getBoundingClientRect().top <= mid) best = i;
      });
      setActiveIdx(best);
    };
    onScroll();
    window.addEventListener("scroll", onScroll, { passive: true });
    return () => window.removeEventListener("scroll", onScroll);
  }, []);

  return (
    <nav
      className="hidden xl:flex fixed left-6 top-1/2 -translate-y-1/2 z-50 flex-col items-center gap-0"
      aria-label="Pipeline loop minimap">
      {SECTIONS.map((s, i) => {
        const active = i === activeIdx;
        return (
          <div key={s.id} className="flex flex-col items-center">
            <button
              onClick={() =>
                document
                  .getElementById(s.id)
                  ?.scrollIntoView({ behavior: "smooth" })
              }
              className="group relative flex items-center justify-center w-8 h-8"
              aria-label={`Go to ${s.id}`}>
              <span
                className="font-mono text-[10px] transition-all duration-300"
                style={{
                  color: active
                    ? "var(--thread-a)"
                    : "oklch(0.62 0.015 250 / 60%)",
                  textShadow: active ? "0 0 8px var(--thread-a)" : "none",
                  transform: active ? "scale(1.3)" : "scale(1)",
                }}>
                {s.label}
              </span>
            </button>
            {i < SECTIONS.length - 1 && (
              <div
                className="w-px h-6"
                style={{
                  background:
                    i < activeIdx
                      ? "var(--thread-a)"
                      : "oklch(0.87 0.01 250 / 15%)",
                  boxShadow: i < activeIdx ? "0 0 4px var(--thread-a)" : "none",
                  transition: "all 300ms",
                }}
              />
            )}
          </div>
        );
      })}
      {/* loop-closing arc back to top */}
      <svg width="20" height="28" viewBox="0 0 20 28" className="mt-1" aria-hidden>
        <path
          d="M10 2 C 2 2 2 26 10 26"
          fill="none"
          stroke={
            activeIdx === SECTIONS.length - 1
              ? "var(--gate-green)"
              : "oklch(0.87 0.01 250 / 15%)"
          }
          strokeWidth="1"
          strokeDasharray="3 3"
        />
        <path
          d="M7 5 L10 2 L13 5"
          fill="none"
          stroke={
            activeIdx === SECTIONS.length - 1
              ? "var(--gate-green)"
              : "oklch(0.87 0.01 250 / 25%)"
          }
          strokeWidth="1"
        />
      </svg>
    </nav>
  );
}
