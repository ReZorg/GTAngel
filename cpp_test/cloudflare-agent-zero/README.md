# GTAngel Agent Zero Inference Adapter

## Operational state: local-only; Cloudflare Workers AI disabled

This directory preserves the previously validated Cloudflare adapter as a **dormant reference implementation**. It must not invoke Workers AI in its checked-in state. The owner disabled Workers AI after a serious billing incident, so GTAngel v16 defaults exclusively to local OpenAI-compatible inference such as `llama.cpp`.

The architecture remains proposal-only: an inference backend may recommend `WAIT`, `FORWARD`, `LEFT`, `RIGHT`, `BACK`, or `SPRINT`, but only the native GTAngel governance gate may commit movement.

## Safe default: local `llama.cpp`

Install a current `llama.cpp` release and run a compact quantized model through its OpenAI-compatible server. A current upstream launch pattern is:

```powershell
llama serve -hf ggml-org/Qwen3.5-0.8B-GGUF --host 127.0.0.1 --port 8080
```

Then launch `gtangel_agi_v16.exe`. GTAngel automatically probes `http://127.0.0.1:8080`; no remote URL or billing-bearing service is required. If inference is unavailable or malformed, v16 fails closed to `WAIT` and preserves the imperfection in its event ledger.

For a guarded Windows launch, set `GTANGEL_GGUF` to an existing local model and run `..\run_local_agent_zero.bat`. The launcher clears all GTAngel remote-inference variables before starting `llama-server` on loopback.

## Cost circuit breakers

The dormant Worker has three independent barriers:

| Barrier | Checked-in state | Effect |
|---|---|---|
| Cloudflare connector | Disabled | Manus cannot deploy or operate the Worker through the connector. |
| Package deploy command | Hard-blocked | `pnpm deploy` exits with an error instead of publishing. |
| Runtime billing lock | `WORKERS_AI_BILLING_LOCK = "LOCKED"` | `/v1/chat/completions` returns HTTP 402 before `env.AI.run` can execute. |
| Native remote inference gate | Not consented | GTAngel ignores `GTANGEL_AGENT_ZERO_URL` unless `GTANGEL_ALLOW_REMOTE_INFERENCE=I_ACCEPT_REMOTE_INFERENCE_COSTS`. |

The runtime lock is deliberately two-key. Even if the Worker variable were changed to `UNLOCKED_BY_OWNER`, every paid inference request would also need the header `x-gtangel-paid-inference: I_ACCEPT_WORKERS_AI_CHARGES`. The native v16 client does not send this header, so the checked-in GTAngel/Worker pair cannot invoke Workers AI accidentally.

## Local validation only

The dormant source may still be type-checked without deployment:

```powershell
cd C:\ddd\GTAngel\cpp_test\cloudflare-agent-zero
pnpm install
pnpm check
```

Do not run or modify a deployment path unless the owner later gives new, explicit authorization after reviewing Cloudflare billing limits and account protections. No Cloudflare connector needs to be enabled for GTAngel v16.

## Authority and evidence contract

The adapter validates that any generated response contains both `ACTION=<choice>` and `CONFIDENCE=<0..1>`. It never calls native actions. The v16 event spine applies health, obstacle, contradiction, confidence, and collision gates. Only the committed outcome drives endocrine state, after which a renderer-neutral FACS expression intent is recorded. All stages share one correlation ID and can be exported to JSONL with the `L` key.

## Sources

The local engine is [llama.cpp](https://github.com/ggml-org/llama.cpp). The dormant adapter was validated against Cloudflare’s [Qwen3-30B-A3B-FP8 model contract](https://developers.cloudflare.com/workers-ai/models/qwen3-30b-a3b-fp8/), [function-calling contract](https://developers.cloudflare.com/workers-ai/features/function-calling/), and [OpenAI-compatible API](https://developers.cloudflare.com/workers-ai/configuration/open-ai-compatibility/), but the paid path is intentionally disabled.
