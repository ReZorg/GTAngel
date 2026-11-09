import fs from 'node:fs'

const sourcePath = process.argv[2] || '/home/ubuntu/gtangel-v19-work/gtangel_agi_v19_full.cpp'
const source = fs.readFileSync(sourcePath, 'utf8')

const required = [
  ['main NPC identity', 'std::string name{"Ariadne"}'],
  ['quest definition', 'Ariadne and the Broken Signal'],
  ['grounded shard transition', 'horizontalDistance(player, world.questShard) <= 1.35f'],
  ['grounded beacon transition', 'horizontalDistance(player, world.goal) <= 1.55f'],
  ['grounded NPC interaction', 'canSpeak(player, playerHeading)'],
  ['reward item', 'hasResonanceSigil = true'],
  ['atomic save replace', 'MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH'],
  ['NPC semantic object', 'ObjectNpc = 6'],
  ['quest-item semantic object', 'ObjectQuestItem = 7'],
  ['NPC render path', 'buildNpc(world)'],
  ['shard render path', 'buildQuestShard(world)'],
  ['beacon world consequence', 'if (world.beaconRestored)'],
  ['local WASD sampler', "GetAsyncKeyState('W')"],
  ['manual controller override', 'gameplay.samplePlayerControls()'],
  ['interaction key', "wp == 'E'"],
  ['choice keys', "wp == '1' || wp == '2' || wp == '3'"],
  ['journal key', "wp == 'J'"],
  ['save key', 'wp == VK_F5'],
  ['load key', 'wp == VK_F9'],
  ['telemetry explicit optional state', 'Telemetry: OPTIONAL / OFF'],
  ['fixed 1024x768 client target', 'RECT windowRect = {0, 0, 1024, 768}'],
  ['adjusted non-client frame', 'AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE)'],
  ['wrapped dialogue', 'DT_WORDBREAK | DT_NOPREFIX'],
  ['legacy terminal reset disabled', 'false && navWorld.goalDistance(navPos)']
]

const forbidden = [
  ['E key cannot emit telemetry', "wp == 'E') {\n                g_agi.civicSpine.requestImmediate"],
  ['S key cannot toggle telemetry', "wp == 'S') {\n                g_agi.civicSpine.setEnabled"],
  ['peer loop cannot feed gameplay', 'gameplay.choose(peerAttention'],
  ['telemetry cannot advance quest', 'civicSpine.tick(gameplay'],
  ['timer cannot complete quest', 'groundedEvents > 100'],
  ['legacy goal cannot reset episode', 'if (navWorld.goalDistance(navPos) < 1.0f) {']
]

const failures = []
for (const [label, token] of required) if (!source.includes(token)) failures.push(`missing: ${label}`)
for (const [label, token] of forbidden) if (source.includes(token)) failures.push(`forbidden: ${label}`)

const report = {
  ok: failures.length === 0,
  sourcePath,
  requiredChecks: required.length,
  forbiddenChecks: forbidden.length,
  failures
}
fs.writeFileSync('/home/ubuntu/gtangel-v19-work/v19_source_verification.json', JSON.stringify(report, null, 2) + '\n')
console.log(JSON.stringify(report))
if (failures.length) process.exit(1)
