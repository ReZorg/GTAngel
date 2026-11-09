import fs from 'node:fs'

const sourcePath = process.argv[2] || '/home/ubuntu/gtangel-v17-work/gtangel_agi_v17_full.cpp'
const source = fs.readFileSync(sourcePath, 'utf8')
const required = [
  ['actuator lock', 'bool neuralActuationUnlocked{false}'],
  ['ethics lock', 'bool neuralEthicsApproved{false}'],
  ['shadow default', 'bool neuralShadowEnabled{true}'],
  ['local visual tensor', 'trajectoryVisual.derive(highResRenderer.framebuffer)'],
  ['local proprioception', 'modelProprioception = v16motor::makeProprioception('],
  ['native inference', 'neuralPolicy.infer(trajectoryVisual, modelProprioception)'],
  ['expert fallback', 'decision.committed = expert'],
  ['visual collision gate', 'visualCollisionRisk > 0.20f'],
  ['model collision gate', 'prediction.collisionProbability > 0.20f'],
  ['uncertainty gate', 'prediction.uncertainty > 0.35f'],
  ['ethics gate', '!ethicsApproved'],
  ['capture close UI log', 'v16 capture close failed:'],
  ['capture rail status', 'trajectoryRecorder.ready() ? "READY" : "ERROR"'],
  ['operator shadow toggle', "wp == 'M'"],
]
for (const [label, token] of required) {
  if (!source.includes(token)) throw new Error(`missing v17 invariant: ${label}`)
}
const forbidden = [
  ['unlocked actuator', 'neuralActuationUnlocked{true}'],
  ['network input to model', 'neuralPolicy.infer(peer'],
  ['privileged coordinate input', 'neuralPolicy.infer(navPos'],
]
for (const [label, token] of forbidden) {
  if (source.includes(token)) throw new Error(`forbidden v17 invariant: ${label}`)
}
console.log(JSON.stringify({ verification: 'pass', source: sourcePath, required: required.length, forbidden: forbidden.length }, null, 2))
