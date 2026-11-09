import fs from 'node:fs'

const source = fs.readFileSync('/home/ubuntu/gtangel-v18-work/gtangel_agi_v18_full.cpp', 'utf8')

const required = [
  ['bounded payload', 'body.size() > 512'],
  ['short-lived ping', 'ttlMs\\\":3000'],
  ['coordinate-free publish signature', 'publishObservedPeer(const std::string& recipientId, int bearingBucket'],
  ['expiry pruning', 'it->expiresAtMs <= now'],
  ['first-person pixel confirmation', 'confirmAgainstLocalFrame(highResRenderer.framebuffer, !navThirdPerson)'],
  ['semantic avatar evidence', 'frame.objectId[index] != v15render::ObjectAvatar'],
  ['depth-band evidence', 'depthMatches(prediction.rangeBand, frame.depth[index])'],
  ['attention-only overlay', 'peerLoop.drawAttentionMarkers'],
  ['explicit advisory context', 'peerAttention = peerLoop.attentionContext()'],
  ['peer toggle', "wp == 'P'"],
  ['local visual policy input', 'neuralPolicy.infer(trajectoryVisual, modelProprioception)'],
  ['locked actuator', 'bool neuralActuationUnlocked{false}'],
  ['locked ethics approval', 'bool neuralEthicsApproved{false}'],
]

const failures = []
for (const [label, token] of required) {
  if (!source.includes(token)) failures.push(`missing ${label}`)
}

const forbidden = [
  ['absolute world coordinates in ping schema', /(?:worldPosition|latitude|longitude|\"positionX\"|\"positionY\"|\"positionZ\")/],
  ['peer input passed to neural policy', /neuralPolicy\.infer\([^)]*peer/i],
  ['peer input passed to NavTrainer', /navTrainer\.(?:update|discretize)\([^)]*peer/i],
  ['peer input passed to shield', /neuralShield\.decide\([^;]*peer/i],
  ['peer attention passed to neural policy', /neuralPolicy\.infer\([^)]*peerAttention/i],
  ['peer attention passed to NavTrainer', /navTrainer\.(?:update|discretize)\([^)]*peerAttention/i],
  ['peer attention passed to shield', /neuralShield\.decide\([^;]*peerAttention/i],
  ['network mutation of depth buffer', /peerLoop[^;]*(?:framebuffer\.)?depth\s*=/i],
  ['network mutation of object-id buffer', /peerLoop[^;]*(?:framebuffer\.)?objectId\s*=/i],
  ['enabled actuator default', /neuralActuationUnlocked\{true\}/],
  ['enabled peer default', /peerLoop\.setEnabled\(true\)/],
]
for (const [label, pattern] of forbidden) {
  if (pattern.test(source)) failures.push(`forbidden ${label}`)
}

const result = {
  ok: failures.length === 0,
  sourceBytes: Buffer.byteLength(source),
  requiredChecks: required.length,
  forbiddenChecks: forbidden.length,
  failures,
}
fs.writeFileSync('/home/ubuntu/gtangel-v18-work/v18_source_verification.json', `${JSON.stringify(result, null, 2)}\n`)
console.log(JSON.stringify(result))
if (failures.length) process.exit(1)
