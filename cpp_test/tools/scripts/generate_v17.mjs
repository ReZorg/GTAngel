import fs from 'node:fs'
import path from 'node:path'

const root = '/home/ubuntu/gtangel-v17-work'
const basePath = '/home/ubuntu/gtangel-v16-work/gtangel_agi_v16_full.cpp'
const modulePath = path.join(root, 'gtangel_agi_v17_neural_policy.cpp')
const outputPath = path.join(root, 'gtangel_agi_v17_full.cpp')

let source = fs.readFileSync(basePath, 'utf8').replaceAll('\r\n', '\n')
const moduleSource = fs.readFileSync(modulePath, 'utf8').replaceAll('\r\n', '\n')
const moduleAnchor = '} // namespace v16motor'
const moduleStart = source.indexOf(moduleAnchor)
if (moduleStart < 0) throw new Error('v17 module anchor not found')
const moduleIndex = moduleStart + moduleAnchor.length
source = `${source.slice(0, moduleIndex)}\n\n${moduleSource}${source.slice(moduleIndex)}`

const replacements = []
const replaceExact = (label, before, after) => {
  if (!source.includes(before)) throw new Error(`v17 wire anchor not found: ${label}`)
  source = source.replace(before, after)
  replacements.push(label)
}

replaceExact('members', `    bool trajectoryRecorderReady{false};
    uint64_t trajectoryEpisodeId{1};
    // avatar3d integration (v14)`, `    bool trajectoryRecorderReady{false};
    uint64_t trajectoryEpisodeId{1};
    // v17 local neural policy: proposal only unless every local gate accepts.
    v17policy::NativeVisuomotorPolicy neuralPolicy;
    v17policy::EmbodiedSafetyShield neuralShield;
    v17policy::Prediction neuralPrediction;
    v17policy::ShieldDecision neuralDecision;
    bool neuralShadowEnabled{true};
    bool neuralActuationUnlocked{false};
    bool neuralEthicsApproved{false};
    bool neuralPolicyLoaded{false};
    uint64_t neuralFrames{0};
    uint64_t neuralAccepted{0};
    // avatar3d integration (v14)`)

replaceExact('reset model', `        addLog(trajectoryRecorderReady
            ? std::string("v16 capture ready: ") + trajectoryRecorder.directory()
            : std::string("v16 capture unavailable: ") + trajectoryRecorder.lastError());
    }`, `        addLog(trajectoryRecorderReady
            ? std::string("v16 capture ready: ") + trajectoryRecorder.directory()
            : std::string("v16 capture unavailable: ") + trajectoryRecorder.lastError());
        neuralPolicyLoaded = neuralPolicy.load("C:\\\\ddd\\\\GTAngel\\\\cpp_test\\\\models\\\\gtangel_visuomotor.gvmw");
        if (!neuralPolicyLoaded) neuralPolicyLoaded = neuralPolicy.load("models/gtangel_visuomotor.gvmw");
        neuralShadowEnabled = true;
        neuralActuationUnlocked = false; // no collision-positive or goal-positive records in corpus 1
        neuralEthicsApproved = false; // requires an explicit local ethics authority decision in a future qualified build
        neuralFrames = 0;
        neuralAccepted = 0;
        addLog(neuralPolicyLoaded
            ? "v17 neural policy loaded in SHADOW mode; actuator remains expert-controlled"
            : std::string("v17 neural policy unavailable: ") + neuralPolicy.error());
    }`)

replaceExact('pre-action inference', `            const v16motor::ControllerTarget expertTarget = v16motor::controllerFromDiscreteAction((int)a);
            float turn = 0, move = 0;
            switch (a) {`, `            const v16motor::ControllerTarget expertTarget = v16motor::controllerFromDiscreteAction((int)a);
            trajectoryVisual.derive(highResRenderer.framebuffer);
            const float beforeVisualConfidence = visualBefore.goalVisible
                ? std::min(1.0f, visualBefore.goalPixels / 96.0f) : 0.0f;
            const float beforeCollisionRisk = visualBefore.obstacleVisible
                ? std::clamp(1.0f - visualBefore.nearestObstacleDepth / 5.0f, 0.0f, 1.0f) : 0.0f;
            const v16motor::ProprioceptiveVector modelProprioception = v16motor::makeProprioception(
                navSpeed, navHeading, 0.0f, false, true, previousControllerTarget,
                (float)endo.valenceArousal().arousal, lastStep.reward,
                beforeVisualConfidence, beforeCollisionRisk);
            if (neuralPolicyLoaded && neuralShadowEnabled) {
                neuralPrediction = neuralPolicy.infer(trajectoryVisual, modelProprioception);
                neuralDecision = neuralShield.decide(
                    neuralPrediction, expertTarget, beforeCollisionRisk,
                    neuralActuationUnlocked, neuralEthicsApproved);
                ++neuralFrames;
                if (neuralDecision.accepted) ++neuralAccepted;
            } else {
                neuralPrediction = v17policy::Prediction{};
                neuralDecision = v17policy::ShieldDecision{};
                neuralDecision.committed = expertTarget;
                neuralDecision.reason = neuralPolicyLoaded ? "shadow disabled" : "model unavailable";
            }
            float turn = 0, move = 0;
            if (neuralDecision.accepted) {
                turn = -0.75f * neuralDecision.committed.values[0];
                move = std::clamp(1.5f * neuralDecision.committed.values[1], -0.5f, 1.6f);
            } else switch (a) {`)

replaceExact('terminal hidden reset', `                    navTrainer.successes++;
                    navPos = avatar3d::Vec3{0, 0, 0}; // environment terminal transition`, `                    navTrainer.successes++;
                    navPos = avatar3d::Vec3{0, 0, 0}; // environment terminal transition
                    neuralPolicy.reset();`)

replaceExact('actual previous controller', `            previousControllerTarget = expertTarget;
            // Feed the mapped framebuffer`, `            previousControllerTarget = neuralDecision.committed;
            // Feed the mapped framebuffer`)

replaceExact('right rail neural status', `    const auto embodied = g_agi.civicSpine.embodiedTelemetry();
    snprintf(line, sizeof(line), "EMBODIED %s  accepted=%lu fail=%lu",
        embodied.configured ? (embodied.busy ? "BUSY" : "READY") : "LOCAL",
        embodied.accepted, embodied.failures);
    DrawText(hdc, railX + 12, 598, line, embodied.failures ? COL_RED : COL_CYAN, g_hFontSmall);
    snprintf(line, sizeof(line), "Nav %s  success %.0f%%  collisions %d",
        g_agi.navTraining ? "TRAIN" : "IDLE",
        g_agi.navTrainer.successRate() * 100.0f, g_agi.navTrainer.collisions);
    DrawText(hdc, railX + 12, 618, line, COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "DATASET %s  records=%lu  %.1f MB",
        g_agi.trajectoryRecording ? "RECORDING" : "IDLE",
        (unsigned long)g_agi.trajectoryRecorder.records(),
        g_agi.trajectoryRecorder.bytesWritten() / (1024.0 * 1024.0));
    DrawText(hdc, railX + 12, 638, line, g_agi.trajectoryRecording ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "CAPTURE %s", g_agi.trajectoryRecorder.ready() ? "READY" : "ERROR");
    DrawText(hdc, railX + 12, 650, line, g_agi.trajectoryRecorder.ready() ? COL_GREEN : COL_RED, g_hFontSmall);

    DrawText(hdc, railX + 12, 662, "[F] colour/depth/ID/luma", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 680, "[D] dashboard  [V] camera", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 698, "[N] train  [C/F9] capture", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 716, "Source 768 -> retina 64 -> field 12", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 738, "Policy state: rendered evidence only", COL_GOLD, g_hFontSmall);`, `    const auto embodied = g_agi.civicSpine.embodiedTelemetry();
    snprintf(line, sizeof(line), "EMBODIED %s  accepted=%lu fail=%lu",
        embodied.configured ? (embodied.busy ? "BUSY" : "READY") : "LOCAL",
        embodied.accepted, embodied.failures);
    DrawText(hdc, railX + 12, 588, line, embodied.failures ? COL_RED : COL_CYAN, g_hFontSmall);
    snprintf(line, sizeof(line), "Nav %s success %.0f%% collisions %d",
        g_agi.navTraining ? "TRAIN" : "IDLE", g_agi.navTrainer.successRate() * 100.0f, g_agi.navTrainer.collisions);
    DrawText(hdc, railX + 12, 606, line, COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "NEURAL %s %.2fms frames=%llu",
        g_agi.neuralPolicyLoaded ? (g_agi.neuralShadowEnabled ? "SHADOW" : "OFF") : "MISSING",
        g_agi.neuralPrediction.latencyMs, (unsigned long long)g_agi.neuralFrames);
    DrawText(hdc, railX + 12, 624, line, g_agi.neuralPolicyLoaded ? COL_CYAN : COL_RED, g_hFontSmall);
    snprintf(line, sizeof(line), "SHIELD %.3f %s", g_agi.neuralDecision.disagreementRms, g_agi.neuralDecision.reason.c_str());
    DrawText(hdc, railX + 12, 642, line, g_agi.neuralDecision.accepted ? COL_GREEN : COL_GOLD, g_hFontSmall);
    snprintf(line, sizeof(line), "DATASET %s records=%lu capture=%s",
        g_agi.trajectoryRecording ? "REC" : "IDLE", (unsigned long)g_agi.trajectoryRecorder.records(),
        g_agi.trajectoryRecorder.ready() ? "READY" : "ERROR");
    DrawText(hdc, railX + 12, 660, line, g_agi.trajectoryRecording ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 680, "[M] neural shadow  [F] targets", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 698, "[N] train  [C/F9] capture", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 716, "768 local pixels -> neural proposal", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 738, "Actuator authority: LOCAL SHIELD", COL_GOLD, g_hFontSmall);`)

replaceExact('neural hotkey', `            } else if (wp == 'C' || wp == VK_F9) {`, `            } else if (wp == 'M') {
                g_agi.neuralShadowEnabled = !g_agi.neuralShadowEnabled;
                g_agi.neuralPolicy.reset();
                g_agi.addLog(g_agi.neuralShadowEnabled ? "v17 neural SHADOW enabled" : "v17 neural inference disabled");
            } else if (wp == 'C' || wp == VK_F9) {`)

source = source.replaceAll('GTAngelAGIv16', 'GTAngelAGIv17')
source = source.replaceAll('Visible Self v16', 'Visible Self v17')

const invariants = [
  ['native loader', 'class NativeVisuomotorPolicy'],
  ['local shield', 'class EmbodiedSafetyShield'],
  ['shadow default', 'bool neuralShadowEnabled{true}'],
  ['actuator lock', 'bool neuralActuationUnlocked{false}'],
  ['ethics lock', 'bool neuralEthicsApproved{false}'],
  ['pixel tensor input', 'neuralPolicy.infer(trajectoryVisual, modelProprioception)'],
  ['expert fallback', 'decision.committed = expert'],
  ['capture close visible log', 'v16 capture close failed:'],
  ['vision-only policy state', 'visualNavEncoder.encode(perceptionPyramid.retina)'],
  ['operator shadow toggle', "wp == 'M'"],
]
for (const [label, token] of invariants) {
  if (!source.includes(token)) throw new Error(`v17 invariant missing: ${label}`)
}
if (source.includes('neuralActuationUnlocked{true}')) throw new Error('v17 neural actuator must remain locked for corpus 1')

fs.writeFileSync(outputPath, source)
console.log(JSON.stringify({ output: outputPath, bytes: Buffer.byteLength(source), replacements, invariants: invariants.length }, null, 2))
