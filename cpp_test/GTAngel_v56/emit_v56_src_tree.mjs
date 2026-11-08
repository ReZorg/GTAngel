import fs from 'node:fs';
import path from 'node:path';

const full = fs.readFileSync('gtangel_agi_v56_full.cpp', 'utf8');
const lines = full.split(/\r?\n/);

// Slice 1-based inclusive line ranges
function sliceLines(start1, end1) {
  return lines.slice(start1 - 1, end1).join('\n') + '\n';
}

function findLine(predicate, from1 = 1) {
  for (let i = from1 - 1; i < lines.length; ++i) {
    if (predicate(lines[i], i + 1)) return i + 1;
  }
  throw new Error('Anchor not found from line ' + from1);
}

const L_avatar3d       = findLine(l => l === 'namespace avatar3d {');
const L_poseLib        = findLine(l => l.startsWith('class DtePoseLibrary {'), L_avatar3d);
const L_gameWorld      = findLine(l => l.startsWith('struct GameWorld {'), L_poseLib);
const L_navTrainer     = findLine(l => l.startsWith('// ── Egocentric software renderer ──'), L_gameWorld);
const L_v15render      = findLine(l => l === 'namespace v15render {', L_navTrainer);
const L_dteSkinned     = findLine(l => l.startsWith('// DTE v22 native skin asset reader'), L_v15render);
const L_rasterEngine   = findLine(l => l.startsWith('template<class Surface> class RasterEngine {'), L_dteSkinned);
const L_buildSun       = findLine(l => l === '    void buildSun() {', L_rasterEngine);
const L_buildBuildings = findLine(l => l === '    void buildBuildings(const avatar3d::GameWorld& world) {', L_buildSun);
const L_buildPillars   = findLine(l => l === '    void buildPillars(const avatar3d::GameWorld& world) {', L_buildBuildings);
const L_buildGoal      = findLine(l => l === '    void buildGoal(const avatar3d::GameWorld& world) {', L_buildPillars);
const L_buildWalkers   = findLine(l => l === '    void buildWalkers(const avatar3d::GameWorld& world) {', L_buildGoal);
const L_buildShard     = findLine(l => l === '    void buildQuestShard(const avatar3d::GameWorld& world) {', L_buildWalkers);
const L_drawCoupe      = findLine(l => l.startsWith('    void drawSculptedCoupe('), L_buildShard);
const L_buildPickups   = findLine(l => l === '    void buildPickups(const avatar3d::GameWorld& world) {', L_drawCoupe);
const L_buildProps     = findLine(l => l === '    void buildProps(const avatar3d::GameWorld& world) {', L_buildPickups);
const L_buildAvatar    = findLine(l => l === '    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {', L_buildProps);
const L_v16motor       = findLine(l => l === '#ifndef GTANGEL_V16_VISUOMOTOR_CPP', L_buildAvatar);
const L_v18peer        = findLine(l => l === '#ifndef GTANGEL_V18_PEER_RELATIONAL_CPP', L_v16motor);
const L_v18peerEnd     = findLine(l => l === '#endif', L_v18peer);
const L_uechovr        = findLine(l => l === 'namespace uechovr {', L_v18peerEnd);
const L_v20cnsFwd      = findLine(l => l === 'namespace v20cns { class WorldEffector; }', L_uechovr);
const L_v20cns         = findLine(l => l === 'struct GTAngelAGI;', L_v20cnsFwd);
const L_v20save        = findLine(l => l === 'namespace v20save {', L_v20cns);
const L_gtangelStruct  = findLine(l => l === 'struct GTAngelAGI {', L_v20save);
const L_renderV15Frame = findLine(l => l.startsWith('static void RenderV15Frame('), L_gtangelStruct);

const folders = [
  'animation', 'audio', 'buildings', 'collision', 'control', 'core',
  'docs', 'entities', 'extras', 'fakerw', 'leeds', 'math',
  'modelinfo', 'objects', 'peds', 'renderer', 'rw', 'save',
  'skel', 'text', 'vehicles', 'weapons'
];

for (const rootDir of ['src', '../src']) {
  for (const f of folders) {
    fs.mkdirSync(path.join(rootDir, f), { recursive: true });
  }

  const writeMod = (relPath, content) => {
    fs.writeFileSync(path.join(rootDir, relPath), content, 'utf8');
  };

  // 1. src/core/PlatformPreamble.hpp (OS & STL headers)
  writeMod('core/PlatformPreamble.hpp', sliceLines(1, L_avatar3d - 1));

  // 2. src/math/Vec3.hpp (Vec3 + vector algebra inside namespace avatar3d)
  writeMod('math/Vec3.hpp', sliceLines(L_avatar3d, L_poseLib - 1));

  // 3. src/animation/AvatarSkeleton.hpp (DtePoseLibrary + AvatarSkeleton)
  writeMod('animation/AvatarSkeleton.hpp', sliceLines(L_poseLib, L_gameWorld - 1));

  // 4. src/collision/WorldCollision.hpp (GameWorld, collides, cameraBlocked, collisionFreeEye, tickTraffic/Walkers)
  writeMod('collision/WorldCollision.hpp', sliceLines(L_gameWorld, L_navTrainer - 1));

  // 5. src/core/CognitiveSubsystems.hpp (NavTrainer, civicangel, layla, cogzero, echo, kys, elizaos, mech0, rebhvt, gtangel)
  writeMod('core/CognitiveSubsystems.hpp', sliceLines(L_navTrainer, L_v15render - 1));

  // 6. src/fakerw/RwTypes.hpp (v15render::Colour, ObjectId, shade, cross3, normalize3)
  writeMod('fakerw/RwTypes.hpp', sliceLines(L_v15render, L_dteSkinned - 1));

  // 7. src/modelinfo/DteSkinnedAsset.hpp (DteSkinnedAsset 2K mesh & skinning, FrameBuffer768, Re3InformedAvatarClump)
  writeMod('modelinfo/DteSkinnedAsset.hpp', sliceLines(L_dteSkinned, L_rasterEngine - 1));

  // 8. src/rw/RasterEngineHeader.hpp (RasterEngine class shell, render(), addTriangle, addQuad)
  writeMod('rw/RasterEngineHeader.hpp', sliceLines(L_rasterEngine, L_buildSun - 1));

  // 9. src/leeds/SkyAndRoads.hpp (buildSun, buildMoon, buildGround, buildRoads)
  writeMod('leeds/SkyAndRoads.hpp', sliceLines(L_buildSun, L_buildBuildings - 1));

  // 10. src/buildings/Buildings.hpp (buildBuildings: skyscrapers, penthouses, sills, storefronts, awnings, neon signs)
  writeMod('buildings/Buildings.hpp', sliceLines(L_buildBuildings, L_buildPillars - 1));

  // 11. src/entities/Pillars.hpp & Props.hpp (buildPillars trees/planters, buildProps street lamps & signal gantries)
  writeMod('entities/Pillars.hpp', sliceLines(L_buildPillars, L_buildGoal - 1));
  writeMod('entities/Props.hpp', sliceLines(L_buildProps, L_buildAvatar - 1));

  // 12. src/objects/WorldObjects.hpp, QuestShard.hpp & Pickups.hpp
  writeMod('objects/WorldObjects.hpp', sliceLines(L_buildGoal, L_buildWalkers - 1));
  writeMod('objects/QuestShard.hpp', sliceLines(L_buildShard, L_drawCoupe - 1));
  writeMod('objects/Pickups.hpp', sliceLines(L_buildPickups, L_buildProps - 1));

  // 13. src/peds/Peds.hpp (buildWalkers 20 animated citizens & buildNpc Ariadne)
  writeMod('peds/Peds.hpp', sliceLines(L_buildWalkers, L_buildShard - 1));

  // 14. src/vehicles/Vehicles.hpp (drawSculptedCoupe, buildVehicle, buildTraffic: CAutomobile, Police, Taxi, Ambulance, Bus, Bike, Heli, Train)
  writeMod('vehicles/Vehicles.hpp', sliceLines(L_drawCoupe, L_buildPickups - 1));

  // 15. src/renderer/SoftwareRasterizer.hpp (buildAvatar, per-pixel PBR rasterize(), HighResRenderer, VisualNavEncoder)
  writeMod('renderer/SoftwareRasterizer.hpp', sliceLines(L_buildAvatar, L_v16motor - 1));

  // 16. src/control/MotorAndLearning.hpp (v16motor, v17policy, v27learn, v27dte, v27gnn)
  writeMod('control/MotorAndLearning.hpp', sliceLines(L_v16motor, L_v18peer - 1) + '#endif\n#endif\n');

  // 17. src/weapons/SafetyAndPeerShield.hpp (v18peer safety shield & guard/purification invariants)
  writeMod('weapons/SafetyAndPeerShield.hpp', sliceLines(L_v18peer, L_v18peerEnd));

  // 18. src/audio/VrAndEndocrineAudio.hpp (uechovr + dte::EndocrineBus 10-hormone resonance bus)
  writeMod('audio/VrAndEndocrineAudio.hpp', sliceLines(L_uechovr, L_v20cnsFwd - 1));

  // 19. src/text/StoryAndQuestText.hpp (v19game::GameplayLoop 4-Act narrative, dialogue & quest journal)
  writeMod('text/StoryAndQuestText.hpp', sliceLines(L_v20cnsFwd, L_v20cns - 1));

  // 20. src/extras/DteBridgeAndCommons.hpp (v20cns, v25dte APL253 catalogue, v26bridge system7/8)
  writeMod('extras/DteBridgeAndCommons.hpp', sliceLines(L_v20cns, L_v20save - 1));

  // 21. src/save/Persistence.hpp (v20save JSON/binary save/load persistence)
  writeMod('save/Persistence.hpp', sliceLines(L_v20save, L_gtangelStruct - 1));

  // 22. src/core/GTAngelAGI.hpp (GTAngelAGI orchestrator, tick(), GNN feature encoder, runSelfTest())
  writeMod('core/GTAngelAGI.hpp', sliceLines(L_gtangelStruct, L_renderV15Frame - 1));

  // 23. src/skel/Win32CockpitSkel.hpp (RenderV15Frame 1440x996 unified cockpit, WndProc, WinMain)
  writeMod('skel/Win32CockpitSkel.hpp', sliceLines(L_renderV15Frame, lines.length));

  // 24. src/docs/ARCHITECTURE.md
  writeMod('docs/ARCHITECTURE.md', '# GTAngel v56 Modular Source Architecture (src/)\n\nAll 22 re3-style subsystems are organized under src/<module>/ and compiled through src/main.cpp.\n');

  // 25. src/main.cpp (modular translation unit assembling the 22 src/ folders)
  writeMod('main.cpp', [
    '// GTAngel v56 — Modular 22-Folder Source Entry Point (src/main.cpp)',
    '#include "core/PlatformPreamble.hpp"',
    '#include "math/Vec3.hpp"',
    '#include "animation/AvatarSkeleton.hpp"',
    '#include "collision/WorldCollision.hpp"',
    '#include "core/CognitiveSubsystems.hpp"',
    '#include "fakerw/RwTypes.hpp"',
    '#include "modelinfo/DteSkinnedAsset.hpp"',
    '#include "rw/RasterEngineHeader.hpp"',
    '#include "leeds/SkyAndRoads.hpp"',
    '#include "buildings/Buildings.hpp"',
    '#include "entities/Pillars.hpp"',
    '#include "objects/WorldObjects.hpp"',
    '#include "peds/Peds.hpp"',
    '#include "objects/QuestShard.hpp"',
    '#include "vehicles/Vehicles.hpp"',
    '#include "objects/Pickups.hpp"',
    '#include "entities/Props.hpp"',
    '#include "renderer/SoftwareRasterizer.hpp"',
    '#include "control/MotorAndLearning.hpp"',
    '#include "weapons/SafetyAndPeerShield.hpp"',
    '#include "audio/VrAndEndocrineAudio.hpp"',
    '#include "text/StoryAndQuestText.hpp"',
    '#include "extras/DteBridgeAndCommons.hpp"',
    '#include "save/Persistence.hpp"',
    '#include "core/GTAngelAGI.hpp"',
    '#include "skel/Win32CockpitSkel.hpp"',
    ''
  ].join('\n'));
}

console.log('Emitted 22 modular src/ directories + src/main.cpp');
