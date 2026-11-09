# GTAngel v16 trajectory capture correction

**Status:** Corrected and serialization-tested on 12 September 2026.

## Finding

The submitted `gtangel_v16_event_ledger.jsonl` contains normal world-observation, proposal, governance, action, endocrine, and expression events, but no capture lifecycle events. The expected `C:\ddd\GTAngel\cpp_test\datasets` directory was absent. The generated C++ revealed the primary defect: the capture hotkey passed `"C:\ddd\GTAngel\cpp_test\datasets"` with unescaped backslashes in generated source. MinGW reported unknown escape sequences, so the runtime path did not reliably identify the intended directory.

The ledger also shows repeated `WAIT` actions while governed control was disabled. That is valid fail-closed behaviour, but it would not produce a diverse imitation-learning corpus. Enable the native expert navigator with **N** during capture.

## Correction

The corrected build is named `gtangel_agi_v16_capture.exe` so it cannot be confused with a previously running v16 process. It now uses a valid raw Windows path and performs a write probe during reset. If the preferred directory is unavailable, it tries the process-local `datasets` directory and then `%TEMP%\GTAngel\datasets`.

The right rail displays `CAPTURE READY` or `CAPTURE ERROR`, `DATASET RECORDING`, record count, and file size. Both **C** and **F9** toggle recording. The application log prints the exact absolute output path or the exact failure. A separate `gtangel_v16_capture_ledger.jsonl` records self-check, start, stop, and write-failure events.

## Verification

The recorder was exercised against the actual v15 768×768 renderer and 96×96 tensor derivation. A one-record serialization self-test produced a 27,820-byte `.gtvm` file. The training parser accepted its packed header, frame sequence, visual tensor, 16-value proprioceptive vector, and six-value controller target. This self-test file is verification evidence only and must never be used as training data.

## Operator sequence

1. Close every older GTAngel process.
2. Run `C:\ddd\GTAngel\cpp_test\gtangel_agi_v16_capture.exe`.
3. Confirm the right rail says `CAPTURE READY`.
4. Press **Space** to run and **N** to enable the safe expert navigator.
5. Press **C** or **F9**; confirm `DATASET RECORDING` and a rising record count.
6. Press **C** or **F9** again to flush and close the file.
7. Read the exact path in the application log or `gtangel_v16_capture_ledger.jsonl`.

The trainer continues to reject corpora smaller than 256 authentic records and does not generate synthetic training rows.
