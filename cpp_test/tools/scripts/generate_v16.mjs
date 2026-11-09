import fs from 'node:fs'
import path from 'node:path'

const root = '/home/ubuntu/gtangel-v16-work'
const basePath = '/home/ubuntu/gtangel-v15-build/gtangel_agi_v15_full.cpp'
const modulePath = path.join(root, 'gtangel_agi_v16_visuomotor.cpp')
const outputPath = path.join(root, 'gtangel_agi_v16_full.cpp')

let source = fs.readFileSync(basePath, 'utf8').replaceAll('\r\n', '\n')
const moduleSource = fs.readFileSync(modulePath, 'utf8').replaceAll('\r\n', '\n')
const moduleAnchor = '} // namespace v15render'
const moduleStart = source.indexOf(moduleAnchor)
if (moduleStart < 0) throw new Error('v16 module anchor not found')
const moduleIndex = moduleStart + moduleAnchor.length
source = `${source.slice(0, moduleIndex)}\n\n${moduleSource}${source.slice(moduleIndex)}`

const replacements = []
const replaceExact = (label, before, after) => {
  if (!source.includes(before)) throw new Error(`v16 wire anchor not found: ${label}`)
  source = source.replace(before, after)
  replacements.push(label)
}

replaceExact('members', `    bool highResViewportVisible{true};
    // avatar3d integration (v14)`, `    bool highResViewportVisible{true};
    // v16 local visuomotor dataset capture
    v16motor::TrajectoryRecorder trajectoryRecorder;
    v16motor::VisualTensor96 trajectoryVisual;
    v16motor::ControllerTarget previousControllerTarget;
    bool trajectoryRecording{false};
    bool trajectoryRecorderReady{false};
    uint64_t trajectoryEpisodeId{1};
    // avatar3d integration (v14)`)

replaceExact('reset', `        addLog("v15 render pyramid: 768x768 source -> 64x64 retina -> 12x12 cognition");
    }`, `        addLog("v15 render pyramid: 768x768 source -> 64x64 retina -> 12x12 cognition");
        trajectoryRecorder.stop();
        trajectoryRecording = false;
        trajectoryRecorderReady = trajectoryRecorder.selfCheck("C:\\\\ddd\\\\GTAngel\\\\cpp_test\\\\datasets");
        previousControllerTarget = v16motor::ControllerTarget{};
        addLog(trajectoryRecorderReady
            ? std::string("v16 capture ready: ") + trajectoryRecorder.directory()
            : std::string("v16 capture unavailable: ") + trajectoryRecorder.lastError());
    }`)

replaceExact('expert target', `            avatar3d::NavTrainer::Action a = navTraining ? navTrainer.choose(s)
                                                          : avatar3d::NavTrainer::WAIT;
            float turn = 0, move = 0;`, `            avatar3d::NavTrainer::Action a = navTraining ? navTrainer.choose(s)
                                                          : avatar3d::NavTrainer::WAIT;
            const v16motor::ControllerTarget expertTarget = v16motor::controllerFromDiscreteAction((int)a);
            float turn = 0, move = 0;`)

replaceExact('record trajectory', `            if (navTraining) {
                navTrainer.update(s, a, reward, visualAfter.state);
                navTrainer.stepsTotal++;
                navEpisodeSteps++;
            }
            // Feed the mapped framebuffer into the existing sensorimotor buffer`, `            if (navTraining) {
                navTrainer.update(s, a, reward, visualAfter.state);
                navTrainer.stepsTotal++;
                navEpisodeSteps++;
            }
            trajectoryVisual.derive(highResRenderer.framebuffer);
            const float visualConfidence = visualAfter.goalVisible
                ? std::min(1.0f, visualAfter.goalPixels / 96.0f)
                : 0.0f;
            const float visualCollisionRisk = visualAfter.obstacleVisible
                ? std::clamp(1.0f - visualAfter.nearestObstacleDepth / 5.0f, 0.0f, 1.0f)
                : 0.0f;
            v16motor::ProprioceptiveVector proprioception = v16motor::makeProprioception(
                navSpeed, navHeading, turn * 0.1f, collided, true, previousControllerTarget,
                (float)endo.valenceArousal().arousal, reward, visualConfidence, visualCollisionRisk);
            v16motor::OutcomeFlags outcome = v16motor::OutcomeFlags::None;
            if (collided) outcome = outcome | v16motor::OutcomeFlags::Collision;
            if (reachedGoal) outcome = outcome | v16motor::OutcomeFlags::GoalReached;
            if (trajectoryRecording && !trajectoryRecorder.capture(
                    (uint32_t)navEpisodeSteps, trajectoryVisual, proprioception, expertTarget,
                    outcome, reward, 1.0f, visualCollisionRisk)) {
                trajectoryRecording = false;
                addLog("v16 recorder stopped after a dataset write failure");
            }
            previousControllerTarget = expertTarget;
            // Feed the mapped framebuffer into the existing sensorimotor buffer`)

replaceExact('rail status', `    DrawText(hdc, railX + 12, 618, line, COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, 662, "[F] colour/depth/ID/luma", COL_TEXT2, g_hFontSmall);`, `    DrawText(hdc, railX + 12, 618, line, COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "DATASET %s  records=%lu  %.1f MB",
        g_agi.trajectoryRecording ? "RECORDING" : "IDLE",
        (unsigned long)g_agi.trajectoryRecorder.records(),
        g_agi.trajectoryRecorder.bytesWritten() / (1024.0 * 1024.0));
    DrawText(hdc, railX + 12, 638, line, g_agi.trajectoryRecording ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "CAPTURE %s", g_agi.trajectoryRecorder.ready() ? "READY" : "ERROR");
    DrawText(hdc, railX + 12, 650, line, g_agi.trajectoryRecorder.ready() ? COL_GREEN : COL_RED, g_hFontSmall);

    DrawText(hdc, railX + 12, 662, "[F] colour/depth/ID/luma", COL_TEXT2, g_hFontSmall);`)

replaceExact('keyboard recorder', `            } else if (wp == 'F') {
                int mode = ((int)g_agi.highResDisplayMode + 1) % 4;`, `            } else if (wp == 'C' || wp == VK_F9) {
                if (g_agi.trajectoryRecording) {
                    const bool stopped = g_agi.trajectoryRecorder.stop();
                    g_agi.trajectoryRecording = false;
                    g_agi.addLog(stopped
                        ? std::string("v16 capture stopped: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture close failed: ") + g_agi.trajectoryRecorder.lastError());
                } else {
                    const bool started = g_agi.trajectoryRecorder.start(
                        R"(C:\\ddd\\GTAngel\\cpp_test\\datasets)", g_agi.trajectoryEpisodeId++);
                    g_agi.trajectoryRecording = started;
                    g_agi.addLog(started
                        ? std::string("v16 capture started: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture failed: ") + g_agi.trajectoryRecorder.lastError());
                }
            } else if (wp == 'F') {
                int mode = ((int)g_agi.highResDisplayMode + 1) % 4;`)

replaceExact('help text', `    DrawText(hdc, railX + 12, 698, "[N] train  [S] spine  [E] emit", COL_TEXT2, g_hFontSmall);`, `    DrawText(hdc, railX + 12, 698, "[N] train  [C/F9] capture", COL_TEXT2, g_hFontSmall);`)

source = source.replaceAll('GTAngelAGIv15', 'GTAngelAGIv16')
source = source.replaceAll('Visible Self v15', 'Visible Self v16')

const invariants = [
  ['recorder module', 'class TrajectoryRecorder'],
  ['96 tensor', 'static constexpr int TensorWidth = 96'],
  ['capture member', 'v16motor::TrajectoryRecorder trajectoryRecorder'],
  ['capture call', 'trajectoryRecorder.capture('],
  ['capture hotkey', "wp == 'C' || wp == VK_F9"],
  ['source provenance', 'trajectoryVisual.derive(highResRenderer.framebuffer)'],
  ['visual learner', 'visualNavEncoder.encode(perceptionPyramid.retina)'],
  ['no privileged live discretization', 'agentLoop.addStep("navigation", 6, [this]() {'],
]
for (const [label, token] of invariants) {
  if (!source.includes(token)) throw new Error(`v16 invariant missing: ${label}`)
}
const navigation = source.slice(
  source.indexOf('agentLoop.addStep("navigation", 6, [this]() {'),
  source.indexOf('agentLoop.addStep("civic_angel_spine", 2, [this]() {'),
)
if (navigation.includes('navTrainer.discretize(')) throw new Error('v16 live navigation still uses privileged discretization')

fs.writeFileSync(outputPath, source)
console.log(JSON.stringify({ output: outputPath, bytes: Buffer.byteLength(source), replacements, invariants: invariants.length }, null, 2))
