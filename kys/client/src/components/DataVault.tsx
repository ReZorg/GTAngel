/**
 * DataVault — section 07: persistence + file storage console.
 * - Left: live DB status (persisted ticks, invariant runs, active word version)
 *   with a re-seed action.
 * - Right: the vault — export ksm_corpus.jsonl to S3 storage, list stored
 *   files (exports + uploads), upload artifacts (≤ 2 MB, auth required).
 * Styling follows the Phase-Locked Luminescence instrument language.
 */
import { useRef, useState } from "react";
import { toast } from "sonner";
import { useAuth } from "@/_core/hooks/useAuth";
import { startLogin } from "@/const";
import { trpc } from "@/lib/trpc";

function fmtBytes(n?: number | null) {
  if (n == null) return "—";
  if (n < 1024) return `${n} B`;
  if (n < 1024 * 1024) return `${(n / 1024).toFixed(1)} KB`;
  return `${(n / (1024 * 1024)).toFixed(2)} MB`;
}

export default function DataVault() {
  const { user, isAuthenticated } = useAuth();
  const utils = trpc.useUtils();
  const fileInput = useRef<HTMLInputElement>(null);
  const [uploading, setUploading] = useState(false);

  const config = trpc.ksm.config.useQuery();
  const ticks = trpc.ksm.ticks.useQuery(undefined, { staleTime: 30_000 });
  const invariants = trpc.ksm.invariants.useQuery(undefined, {
    staleTime: 30_000,
  });
  const files = trpc.ksm.files.list.useQuery();

  const seed = trpc.ksm.seed.useMutation({
    onSuccess: (r) => {
      toast.success(
        `Seeded ${r.ticks} ticks · ${r.invariants} invariants · ${r.allPass ? "all gates pass" : "FAULTS PRESENT"}`,
      );
      utils.ksm.invalidate();
    },
    onError: (e) => toast.error(`Seed failed: ${e.message}`),
  });

  const exportCorpus = trpc.ksm.exportCorpus.useMutation({
    onSuccess: (r) => {
      toast.success(`Exported ${r.records} records (${fmtBytes(r.bytes)}) to storage`);
      utils.ksm.files.list.invalidate();
    },
    onError: (e) => toast.error(`Export failed: ${e.message}`),
  });

  const upload = trpc.ksm.files.upload.useMutation({
    onSuccess: () => {
      toast.success("Artifact stored");
      utils.ksm.files.list.invalidate();
    },
    onError: (e) => toast.error(`Upload failed: ${e.message}`),
  });

  const remove = trpc.ksm.files.remove.useMutation({
    onMutate: async ({ id }) => {
      await utils.ksm.files.list.cancel();
      const prev = utils.ksm.files.list.getData();
      utils.ksm.files.list.setData(undefined, (old) =>
        old?.filter((f) => f.id !== id),
      );
      return { prev };
    },
    onError: (e, _v, ctx) => {
      if (ctx?.prev) utils.ksm.files.list.setData(undefined, ctx.prev);
      toast.error(`Remove failed: ${e.message}`);
    },
    onSettled: () => utils.ksm.files.list.invalidate(),
  });

  const handleFile = async (f: File) => {
    if (f.size > 2 * 1024 * 1024) {
      toast.error("File exceeds the 2 MB limit");
      return;
    }
    setUploading(true);
    try {
      const buf = await f.arrayBuffer();
      let binary = "";
      const bytes = new Uint8Array(buf);
      const chunk = 0x8000;
      for (let i = 0; i < bytes.length; i += chunk) {
        binary += String.fromCharCode.apply(
          null,
          Array.from(bytes.subarray(i, i + chunk)),
        );
      }
      await upload.mutateAsync({
        filename: f.name,
        mimeType: f.type || "application/octet-stream",
        dataBase64: btoa(binary),
      });
    } finally {
      setUploading(false);
      if (fileInput.current) fileInput.current.value = "";
    }
  };

  const tickCount = ticks.data?.length ?? 0;
  const invRows = invariants.data ?? [];
  const allPass = invRows.length > 0 && invRows.every((r) => r.faults === 0);
  const wordVersion = config.data?.version ?? "—";

  return (
    <div className="grid lg:grid-cols-[0.9fr_1.3fr] gap-10 items-start">
      {/* ---- left: DB status panel ---- */}
      <div className="border border-border bg-card/60">
        <div className="flex items-center justify-between px-4 py-2 border-b border-border">
          <span className="font-mono text-[10px] tracking-[0.25em] text-muted-foreground">
            PERSISTENCE · MYSQL
          </span>
          <span
            className="w-2 h-2 rounded-full"
            style={{
              background:
                tickCount > 0 ? "var(--gate-green)" : "var(--gold)",
              boxShadow:
                tickCount > 0
                  ? "0 0 6px var(--gate-green)"
                  : "0 0 6px var(--gold)",
            }}
          />
        </div>
        <div className="grid grid-cols-3 gap-px bg-border">
          {[
            { v: tickCount, l: "ticks persisted" },
            { v: invRows.length, l: "invariant rows" },
            { v: allPass ? "PASS" : tickCount ? "…" : "—", l: "gate status" },
          ].map((s) => (
            <div key={s.l} className="bg-card p-4 text-center">
              <div
                className="font-display font-bold text-2xl"
                style={{
                  color:
                    s.v === "PASS" || (typeof s.v === "number" && s.v > 0)
                      ? "var(--gate-green)"
                      : "var(--muted-foreground)",
                }}>
                {ticks.isLoading || invariants.isLoading ? "…" : s.v}
              </div>
              <div className="font-mono text-[10px] text-muted-foreground mt-1">
                {s.l}
              </div>
            </div>
          ))}
        </div>
        <div className="px-4 py-3 border-t border-border font-mono text-[11px] text-muted-foreground leading-relaxed">
          active word version:{" "}
          <span style={{ color: "var(--gold)" }}>{wordVersion}</span>
          <br />
          tables: corpusTicks · invariantChecks · engineConfigs · storedFiles
        </div>
        <div className="px-4 pb-4">
          <button
            onClick={() => seed.mutate()}
            disabled={seed.isPending}
            className="w-full font-mono text-xs px-4 py-2.5 border transition-all hover:translate-x-0.5 disabled:opacity-50"
            style={{ borderColor: "var(--thread-a)", color: "var(--thread-a)" }}>
            {seed.isPending
              ? "RE-SEEDING…"
              : "⟳ RE-SEED FROM DETERMINISTIC ENGINE"}
          </button>
          <div className="mt-2 font-mono text-[10px] text-muted-foreground">
            regenerates all 36 ticks + re-runs the 6 gates server-side —
            idempotent, checkable
          </div>
        </div>
      </div>

      {/* ---- right: the vault ---- */}
      <div className="border border-border bg-card/60">
        <div className="flex items-center justify-between px-4 py-2 border-b border-border">
          <span className="font-mono text-[10px] tracking-[0.25em] text-muted-foreground">
            VAULT · S3 OBJECT STORAGE
          </span>
          <div className="flex gap-2">
            <button
              onClick={() => exportCorpus.mutate()}
              disabled={exportCorpus.isPending}
              className="font-mono text-[11px] px-3 py-1.5 border transition-all hover:translate-x-0.5 disabled:opacity-50"
              style={{
                borderColor: "var(--gate-green)",
                color: "var(--gate-green)",
              }}>
              {exportCorpus.isPending ? "EXPORTING…" : "↓ EXPORT CORPUS .JSONL"}
            </button>
            {isAuthenticated ? (
              <button
                onClick={() => fileInput.current?.click()}
                disabled={uploading}
                className="font-mono text-[11px] px-3 py-1.5 border transition-all hover:translate-x-0.5 disabled:opacity-50"
                style={{ borderColor: "var(--thread-b)", color: "var(--thread-b)" }}>
                {uploading ? "STORING…" : "↑ UPLOAD ARTIFACT"}
              </button>
            ) : (
              <button
                onClick={() => startLogin()}
                className="font-mono text-[11px] px-3 py-1.5 border transition-all hover:translate-x-0.5"
                style={{ borderColor: "var(--thread-b)", color: "var(--thread-b)" }}>
                SIGN IN TO UPLOAD
              </button>
            )}
          </div>
        </div>
        <input
          ref={fileInput}
          type="file"
          className="hidden"
          onChange={(e) => {
            const f = e.target.files?.[0];
            if (f) void handleFile(f);
          }}
        />

        {/* file table */}
        <div className="max-h-[340px] overflow-y-auto">
          {files.isLoading ? (
            <div className="p-6 font-mono text-xs text-muted-foreground">
              reading vault…
            </div>
          ) : (files.data?.length ?? 0) === 0 ? (
            <div className="p-6 font-mono text-xs text-muted-foreground leading-relaxed">
              vault empty — export the corpus or upload an artifact.
              <br />
              bytes live in S3; the database keeps only keys and metadata.
            </div>
          ) : (
            <table className="w-full font-mono text-[11px]">
              <thead>
                <tr className="text-muted-foreground border-b border-border">
                  <th className="text-left px-4 py-2 font-normal">FILE</th>
                  <th className="text-left px-2 py-2 font-normal">KIND</th>
                  <th className="text-right px-2 py-2 font-normal">SIZE</th>
                  <th className="text-right px-4 py-2 font-normal">ACTIONS</th>
                </tr>
              </thead>
              <tbody>
                {files.data!.map((f) => (
                  <tr key={f.id} className="border-b border-border/50">
                    <td className="px-4 py-2 text-secondary-foreground max-w-[220px] truncate">
                      {f.filename}
                    </td>
                    <td className="px-2 py-2">
                      <span
                        style={{
                          color:
                            f.kind === "export"
                              ? "var(--gate-green)"
                              : "var(--thread-b)",
                        }}>
                        {f.kind}
                      </span>
                    </td>
                    <td className="px-2 py-2 text-right text-muted-foreground">
                      {fmtBytes(f.sizeBytes)}
                    </td>
                    <td className="px-4 py-2 text-right whitespace-nowrap">
                      <a
                        href={f.url}
                        download={f.filename}
                        target="_blank"
                        rel="noreferrer"
                        className="hover:underline"
                        style={{ color: "var(--thread-a)" }}>
                        get
                      </a>
                      {(user?.role === "admin" || f.uploadedBy === user?.id) && (
                        <button
                          onClick={() => remove.mutate({ id: f.id })}
                          className="ml-3 text-muted-foreground hover:text-destructive transition-colors">
                          del
                        </button>
                      )}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
        <div className="px-4 py-2 border-t border-border font-mono text-[10px] text-muted-foreground">
          uploads ≤ 2 MB · auth required for upload/delete · exports are public
        </div>
      </div>
    </div>
  );
}
