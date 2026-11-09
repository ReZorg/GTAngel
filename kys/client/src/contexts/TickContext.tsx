/**
 * TickContext — Phase-Locked Luminescence
 * The single shared tick clock. One heartbeat, many organs:
 * ALL animation on the page derives from this context. No component
 * may run its own step timer.
 */
import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useRef,
  useState,
  type ReactNode,
} from "react";
import { TICK_MS } from "@/lib/engineData";

interface TickState {
  /** global tick counter (g). Threads execute word[(g + 4k) % 12] */
  tick: number;
  running: boolean;
  tickMs: number;
  play: () => void;
  pause: () => void;
  step: () => void;
  setTickMs: (ms: number) => void;
  /** pause and jump directly to an absolute tick (scrubber) */
  jumpTo: (t: number) => void;
}

const TickContext = createContext<TickState | null>(null);

export function TickProvider({ children }: { children: ReactNode }) {
  const [tick, setTick] = useState(0);
  const [running, setRunning] = useState(true);
  const [tickMs, setTickMs] = useState(TICK_MS);
  const timerRef = useRef<ReturnType<typeof setInterval> | null>(null);

  useEffect(() => {
    if (!running) return;
    timerRef.current = setInterval(() => setTick((t) => t + 1), tickMs);
    return () => {
      if (timerRef.current) clearInterval(timerRef.current);
    };
  }, [running, tickMs]);

  const play = useCallback(() => setRunning(true), []);
  const pause = useCallback(() => setRunning(false), []);
  const step = useCallback(() => {
    setRunning(false);
    setTick((t) => t + 1);
  }, []);
  const jumpTo = useCallback((t: number) => {
    setRunning(false);
    setTick(Math.max(0, Math.round(t)));
  }, []);

  return (
    <TickContext.Provider
      value={{ tick, running, tickMs, play, pause, step, setTickMs, jumpTo }}>
      {children}
    </TickContext.Provider>
  );
}

export function useTick(): TickState {
  const ctx = useContext(TickContext);
  if (!ctx) throw new Error("useTick must be used within TickProvider");
  return ctx;
}

/**
 * Scanline — 1px sweep synchronized to the global tick.
 * Rendered once at page root; re-keyed on tick to restart the CSS animation.
 */
export function TickScanline() {
  const { tick, running } = useTick();
  if (!running) return null;
  return <div key={tick} className="scanline" aria-hidden />;
}
