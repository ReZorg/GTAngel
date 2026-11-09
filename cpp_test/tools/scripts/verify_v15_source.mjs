import fs from 'node:fs'
import path from 'node:path'

const sourcePath = path.join(path.dirname(new URL(import.meta.url).pathname), 'gtangel_agi_v15_full.cpp')
const source = fs.readFileSync(sourcePath, 'utf8').replaceAll('\r\n', '\n')

const startMarker = 'agentLoop.addStep("navigation", 6, [this]() {'
const endMarker = 'agentLoop.addStep("civic_angel_spine", 2, [this]() {'
const start = source.indexOf(startMarker)
const end = source.indexOf(endMarker, start)
if (start < 0 || end < 0) throw new Error('live navigation AgentLoop boundary not found')
const liveNavigation = source.slice(start, end)

const failures = []
const requireText = (text, label) => {
  if (!liveNavigation.includes(text)) failures.push(`missing ${label}`)
}
const forbidText = (text, label) => {
  if (liveNavigation.includes(text)) failures.push(`forbidden ${label}`)
}

requireText('visualNavEncoder.encode(perceptionPyramid.retina)', 'rendered-evidence encoder')
requireText('visualNavEncoder.progressReward(visualBefore, visualAfter)', 'visual progress reward')
requireText('navTrainer.update(s, a, reward, visualAfter.state)', 'visual next-state policy update')
forbidText('navTrainer.discretize(', 'privileged policy discretization')
forbidText('navWorld.goalDistance(navPos) - navWorld.goalDistance(next)', 'privileged distance shaping')
forbidText('navWorld.pillars', 'pillar-coordinate policy input')

const encoderCalls = liveNavigation.match(/visualNavEncoder\.encode\(perceptionPyramid\.retina\)/g)?.length ?? 0
if (encoderCalls !== 2) failures.push(`expected two rendered-evidence states, found ${encoderCalls}`)

if (!source.includes('struct VisualNavEvidence') || !source.includes('class VisualNavStateEncoder')) {
  failures.push('visual state encoder implementation missing')
}

if (failures.length) {
  console.error(JSON.stringify({ ok: false, failures }, null, 2))
  process.exit(1)
}

console.log(JSON.stringify({
  ok: true,
  source: 'gtangel_agi_v15_full.cpp',
  liveNavigationBytes: Buffer.byteLength(liveNavigation),
  encoderCalls,
  policyState: '64x64 rendered retina',
  policyReward: 'rendered goal-depth change plus environment terminal/collision outcome',
  forbiddenInputsAbsent: ['navTrainer.discretize', 'direct distance shaping', 'pillar coordinates'],
}, null, 2))
