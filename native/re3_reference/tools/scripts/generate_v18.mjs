import fs from 'node:fs'
import path from 'node:path'

const root = '/home/ubuntu/gtangel-v18-work'
const basePath = '/home/ubuntu/gtangel-v17-work/gtangel_agi_v17_full.cpp'
const modulePath = path.join(root, 'gtangel_agi_v18_peer_relational.cpp')
const outputPath = path.join(root, 'gtangel_agi_v18_full.cpp')

let source = fs.readFileSync(basePath, 'utf8').replaceAll('\r\n', '\n')
const moduleSource = fs.readFileSync(modulePath, 'utf8').replaceAll('\r\n', '\n')
const moduleAnchor = '} // namespace v17policy'
const moduleStart = source.indexOf(moduleAnchor)
if (moduleStart < 0) throw new Error('v18 module anchor not found')
const moduleIndex = moduleStart + moduleAnchor.length
source = `${source.slice(0, moduleIndex)}\n\n${moduleSource}${source.slice(moduleIndex)}`

const replacements = []
const replaceExact = (label, before, after) => {
  if (!source.includes(before)) throw new Error(`v18 wire anchor not found: ${label}`)
  source = source.replace(before, after)
  replacements.push(label)
}

replaceExact('members', `    uint64_t neuralAccepted{0};
    // avatar3d integration (v14)`, `    uint64_t neuralAccepted{0};
    // v18 non-local relation loop: provisional attention only.
    v18peer::PeerRelationalLoop peerLoop;
    // avatar3d integration (v14)`)

replaceExact('reset peer loop', `        addLog(neuralPolicyLoaded
            ? "v17 neural policy loaded in SHADOW mode; actuator remains expert-controlled"
            : std::string("v17 neural policy unavailable: ") + neuralPolicy.error());
    }`, `        addLog(neuralPolicyLoaded
            ? "v17 neural policy loaded in SHADOW mode; actuator remains expert-controlled"
            : std::string("v17 neural policy unavailable: ") + neuralPolicy.error());
        peerLoop.configureFromEnvironment();
        peerLoop.setEnabled(false);
        addLog("v18 peer relay initialized OFF; network relations remain attention-only");
    }`)

replaceExact('local confirmation', `            highResRenderer.render(navWorld, avatar, eye, navHeading,
                                   navThirdPerson ? -0.4f : 0.0f, navThirdPerson);
            perceptionPyramid.derive(highResRenderer.framebuffer);`, `            highResRenderer.render(navWorld, avatar, eye, navHeading,
                                   navThirdPerson ? -0.4f : 0.0f, navThirdPerson);
            peerLoop.confirmAgainstLocalFrame(highResRenderer.framebuffer, !navThirdPerson);
            perceptionPyramid.derive(highResRenderer.framebuffer);`)

replaceExact('viewport markers', `    g_agi.highResRenderer.framebuffer.blit(hdc, 0, 0, g_agi.highResDisplayMode);

    const int railX = 768;`, `    g_agi.highResRenderer.framebuffer.blit(hdc, 0, 0, g_agi.highResDisplayMode);
    g_agi.peerLoop.drawAttentionMarkers(hdc, 0, 0, 768);

    const int railX = 768;`)

replaceExact('right rail peer status', `    snprintf(line, sizeof(line), "DATASET %s records=%lu capture=%s",
        g_agi.trajectoryRecording ? "REC" : "IDLE", (unsigned long)g_agi.trajectoryRecorder.records(),
        g_agi.trajectoryRecorder.ready() ? "READY" : "ERROR");
    DrawText(hdc, railX + 12, 660, line, g_agi.trajectoryRecording ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 680, "[M] neural shadow  [F] targets", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 698, "[N] train  [C/F9] capture", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 716, "768 local pixels -> neural proposal", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 738, "Actuator authority: LOCAL SHIELD", COL_GOLD, g_hFontSmall);`, `    const auto peer = g_agi.peerLoop.telemetry();
    snprintf(line, sizeof(line), "PEER %s rx=%lu ok=%lu exp=%lu",
        peer.enabled ? (peer.configured ? "ON" : "LOCAL") : "OFF", peer.received, peer.confirmed, peer.expired);
    DrawText(hdc, railX + 12, 660, line, peer.enabled ? COL_CYAN : COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "NET %.1fms reject=%lu queue=%lu", peer.lastLatencyMs, peer.rejected, peer.queued);
    DrawText(hdc, railX + 12, 678, line, peer.rejected ? COL_GOLD : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 698, "[P] peer attention  [M] neural", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 716, "768 local pixels + sparse relations", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 738, "Actuator authority: LOCAL SHIELD", COL_GOLD, g_hFontSmall);`)

replaceExact('peer hotkey', `            } else if (wp == 'M') {`, `            } else if (wp == 'P') {
                g_agi.peerLoop.setEnabled(!g_agi.peerLoop.enabled());
                g_agi.addLog(g_agi.peerLoop.enabled()
                    ? "v18 peer attention enabled; local confirmation required"
                    : "v18 peer attention disabled");
            } else if (wp == 'M') {`)

source = source.replaceAll('GTAngelAGIv17', 'GTAngelAGIv18')
source = source.replaceAll('Visible Self v17', 'Visible Self v18')
source = source.replaceAll('GTAngel v15 / VISIBLE SELF', 'GTAngel v18 / VISIBLE SELF')

const invariants = [
  ['peer relay class', 'class PeerRelationalLoop'],
  ['coordinate-free publish signature', 'publishObservedPeer(const std::string& recipientId, int bearingBucket'],
  ['512 byte bound', 'body.size() > 512'],
  ['expiry pruning', 'it->expiresAtMs <= now'],
  ['first-person confirmation', 'confirmAgainstLocalFrame(highResRenderer.framebuffer, !navThirdPerson)'],
  ['attention marker only', 'peerLoop.drawAttentionMarkers'],
  ['peer operator toggle', "wp == 'P'"],
  ['neural input remains local', 'neuralPolicy.infer(trajectoryVisual, modelProprioception)'],
  ['actuator remains locked', 'bool neuralActuationUnlocked{false}'],
]
for (const [label, token] of invariants) {
  if (!source.includes(token)) throw new Error(`v18 invariant missing: ${label}`)
}
for (const forbidden of ['peerLoop.neural', 'peerLoop.navTrainer', 'peerLoop.framebuffer.depth =', 'peerLoop.framebuffer.objectId =']) {
  if (source.includes(forbidden)) throw new Error(`v18 forbidden coupling detected: ${forbidden}`)
}

fs.writeFileSync(outputPath, source)
console.log(JSON.stringify({ output: outputPath, bytes: Buffer.byteLength(source), replacements, invariants: invariants.length }, null, 2))
