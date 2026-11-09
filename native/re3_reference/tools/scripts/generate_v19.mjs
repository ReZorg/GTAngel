import fs from 'node:fs'

const basePath = '/home/ubuntu/gtangel-v19-work/gtangel_agi_v18_full.cpp'
const modulePath = '/home/ubuntu/gtangel-v19-work/gtangel_agi_v19_gameplay.cpp'
const outputPath = '/home/ubuntu/gtangel-v19-work/gtangel_agi_v19_full.cpp'
let source = fs.readFileSync(basePath, 'utf8')
const gameplay = fs.readFileSync(modulePath, 'utf8')

const exact = (from, to, label) => {
  if (!source.includes(from)) throw new Error(`Missing v19 anchor: ${label}`)
  source = source.replace(from, to)
}

exact('// ═══ Application State ═══', `${gameplay}\n\n// ═══ Application State ═══`, 'gameplay module')
exact('    Vec3 goal{8.0f, 0.0f, 8.0f};\n    float worldSize{20.0f};', `    Vec3 goal{8.0f, 0.0f, 8.0f};
    Vec3 npcPos{-4.0f, 0.0f, 4.0f};
    float npcHeading{2.35f};
    float npcSpeed{0.0f};
    Vec3 questShard{-6.0f, 0.0f, -5.0f};
    bool questShardVisible{true};
    bool beaconRestored{false};
    bool questComplete{false};
    float worldSize{20.0f};`, 'world gameplay fields')
exact('    ObjectAvatar = 4,\n    ObjectGrid = 5', `    ObjectAvatar = 4,
    ObjectGrid = 5,
    ObjectNpc = 6,
    ObjectQuestItem = 7`, 'semantic object ids')
exact('        buildGoal(world);\n        if (thirdPerson) buildAvatar(avatar);', `        buildGoal(world);
        buildNpc(world);
        buildQuestShard(world);
        if (thirdPerson) buildAvatar(avatar);`, 'render gameplay objects')
exact('        Colour goal{(uint8_t)std::lround(80.0f + 60.0f * pulse),\n                    (uint8_t)std::lround(180.0f + 70.0f * pulse),\n                    (uint8_t)std::lround(70.0f + 45.0f * pulse), 255};', `        Colour goal = world.beaconRestored
            ? Colour{(uint8_t)std::lround(65.0f + 45.0f * pulse),
                     (uint8_t)std::lround(205.0f + 45.0f * pulse),
                     (uint8_t)std::lround(225.0f + 30.0f * pulse), 255}
            : Colour{(uint8_t)std::lround(145.0f + 70.0f * pulse),
                     (uint8_t)std::lround(75.0f + 40.0f * pulse),
                     (uint8_t)std::lround(35.0f + 25.0f * pulse), 255};`, 'beacon state material')
exact('    void addTaperedBone(Vec3 start, Vec3 end, float startRadius, float endRadius, Colour colour) {', '    void addTaperedBone(Vec3 start, Vec3 end, float startRadius, float endRadius, Colour colour, uint16_t objectId = ObjectAvatar) {', 'bone semantic parameter')
exact('            addQuad(start + start0, start + start1, end + end1, end + end0, colour, ObjectAvatar);', '            addQuad(start + start0, start + start1, end + end1, end + end0, colour, objectId);', 'bone semantic use')
exact('    void addJoint(Vec3 centre, float radius) {', '    void addJoint(Vec3 centre, float radius, Colour colour = Colour{235, 154, 247, 255}, uint16_t objectId = ObjectAvatar) {', 'joint semantic parameter')
exact('        Colour colour{235, 154, 247, 255};\n        addTriangle(top, front, right, colour, ObjectAvatar);\n        addTriangle(top, right, back, colour, ObjectAvatar);\n        addTriangle(top, back, left, colour, ObjectAvatar);\n        addTriangle(top, left, front, colour, ObjectAvatar);\n        addTriangle(bottom, right, front, colour, ObjectAvatar);\n        addTriangle(bottom, back, right, colour, ObjectAvatar);\n        addTriangle(bottom, left, back, colour, ObjectAvatar);\n        addTriangle(bottom, front, left, colour, ObjectAvatar);', `        addTriangle(top, front, right, colour, objectId);
        addTriangle(top, right, back, colour, objectId);
        addTriangle(top, back, left, colour, objectId);
        addTriangle(top, left, front, colour, objectId);
        addTriangle(bottom, right, front, colour, objectId);
        addTriangle(bottom, back, right, colour, objectId);
        addTriangle(bottom, left, back, colour, objectId);
        addTriangle(bottom, front, left, colour, objectId);`, 'joint semantic use')
exact('    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {', `    void buildNpc(const avatar3d::GameWorld& world) {
        avatar3d::AvatarSkeleton npc;
        npc.pose(world.npcPos, world.npcHeading, std::min(1.0f, world.npcSpeed), 0.05f);
        const Colour core = world.questComplete ? Colour{248, 194, 96, 255} : Colour{139, 79, 190, 255};
        const Colour limb = world.questComplete ? Colour{250, 224, 154, 255} : Colour{205, 126, 228, 255};
        const auto& frames = Re3InformedAvatarClump::frames();
        for (const auto& atomic : Re3InformedAvatarClump::atomics()) {
            const int fromJoint = frames[atomic.fromFrame].joint;
            const int toJoint = frames[atomic.toFrame].joint;
            const Colour colour = atomic.startRadius > 0.1f ? core : limb;
            addTaperedBone(npc.joints[fromJoint], npc.joints[toJoint],
                           atomic.startRadius * 1.04f, atomic.endRadius * 1.04f,
                           colour, ObjectNpc);
        }
        for (int joint = 0; joint < avatar3d::AvatarSkeleton::COUNT; ++joint)
            addJoint(npc.joints[joint], joint == avatar3d::AvatarSkeleton::Head ? 0.15f : 0.078f,
                     joint == avatar3d::AvatarSkeleton::Head ? Colour{255, 214, 170, 255} : limb,
                     ObjectNpc);
        const float ringRadius = 0.68f;
        const float ringWidth = 0.055f;
        for (int segment = 0; segment < 16; ++segment) {
            const float a0 = 6.2831853f * (float)segment / 16.0f;
            const float a1 = 6.2831853f * (float)(segment + 1) / 16.0f;
            const Vec3 p0 = world.npcPos + Vec3{cosf(a0) * (ringRadius-ringWidth), 0.025f, sinf(a0) * (ringRadius-ringWidth)};
            const Vec3 p1 = world.npcPos + Vec3{cosf(a1) * (ringRadius-ringWidth), 0.025f, sinf(a1) * (ringRadius-ringWidth)};
            const Vec3 p2 = world.npcPos + Vec3{cosf(a1) * (ringRadius+ringWidth), 0.025f, sinf(a1) * (ringRadius+ringWidth)};
            const Vec3 p3 = world.npcPos + Vec3{cosf(a0) * (ringRadius+ringWidth), 0.025f, sinf(a0) * (ringRadius+ringWidth)};
            addQuad(p0, p1, p2, p3, world.questComplete ? Colour{250, 197, 95, 255} : Colour{173, 92, 219, 255}, ObjectNpc);
        }
    }

    void buildQuestShard(const avatar3d::GameWorld& world) {
        if (!world.questShardVisible) return;
        const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        const float bob = 0.62f + 0.12f * sinf((float)(ticks % 1000000) * 0.005f);
        const float radius = 0.28f;
        const Vec3 c = world.questShard + Vec3{0.0f, bob, 0.0f};
        const Vec3 top = c + Vec3{0, radius * 1.5f, 0};
        const Vec3 bottom = c + Vec3{0, -radius * 1.5f, 0};
        const std::array<Vec3, 4> ring{{
            c + Vec3{radius, 0, 0}, c + Vec3{0, 0, radius},
            c + Vec3{-radius, 0, 0}, c + Vec3{0, 0, -radius}}};
        const Colour shard{72, 224, 244, 255};
        for (int i = 0; i < 4; ++i) {
            const int next = (i + 1) % 4;
            addTriangle(top, ring[i], ring[next], shard, ObjectQuestItem);
            addTriangle(bottom, ring[next], ring[i], Colour{34, 154, 225, 255}, ObjectQuestItem);
        }
    }

    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {`, 'npc and shard mesh methods')
exact('    v18peer::AttentionContext peerAttention;\n    // avatar3d integration (v14)', '    v18peer::AttentionContext peerAttention;\n    // v19 local gameplay core: no telemetry required.\n    v19game::GameplayLoop gameplay;\n    // avatar3d integration (v14)', 'gameplay member')
exact('        navWorld.generate(1337, 14);\n        navPos = avatar3d::Vec3{0, 0, 0};', '        navWorld.generate(1337, 14);\n        gameplay.reset(navWorld);\n        navPos = avatar3d::Vec3{0, 0, 0};', 'gameplay reset')
exact('        agentLoop.addStep("navigation", 6, [this]() {\n            // Advance the avatar one step with the current policy action.', '        agentLoop.addStep("navigation", 6, [this]() {\n            gameplay.tick(0.05f, navPos, navHeading, navWorld);\n            // Advance the avatar one step with the current policy action.', 'gameplay tick')
exact('            } else switch (a) {', '            } else switch (a) {', 'navigation action switch exists')
exact('                default: break;\n            }\n            navHeading += turn * 0.1f;', `                default: break;
            }
            const v19game::PlayerControls playerControls = gameplay.samplePlayerControls();
            if (playerControls.active) {
                turn = playerControls.turn;
                move = playerControls.move;
            }
            navHeading += turn * 0.1f;`, 'manual movement override')
exact('                if (navWorld.goalDistance(navPos) < 1.0f) {', '                if (false && navWorld.goalDistance(navPos) < 1.0f) {', 'disable legacy terminal goal reset')
exact('            avatar.pose(navPos, navHeading, fabsf(navSpeed), 0.05f);', '            gameplay.tick(0.0f, navPos, navHeading, navWorld);\n            avatar.pose(navPos, navHeading, fabsf(navSpeed), 0.05f);', 'post-move gameplay world sync')

exact(`            addQuad(p0, p1, p2, p3, goal, ObjectGoal);
        }
    }

    void addTaperedBone`, `            addQuad(p0, p1, p2, p3, goal, ObjectGoal);
        }
        if (world.beaconRestored) {
            const Colour light{64, 212, 238, 255};
            const float r = 0.42f;
            addQuad({world.goal.x-r, 0.05f, world.goal.z},
                    {world.goal.x+r, 0.05f, world.goal.z},
                    {world.goal.x+0.12f, 7.5f, world.goal.z},
                    {world.goal.x-0.12f, 7.5f, world.goal.z}, light, ObjectGoal);
            addQuad({world.goal.x, 0.05f, world.goal.z-r},
                    {world.goal.x, 0.05f, world.goal.z+r},
                    {world.goal.x, 7.5f, world.goal.z+0.12f},
                    {world.goal.x, 7.5f, world.goal.z-0.12f}, light, ObjectGoal);
        }
    }

    void addTaperedBone`, 'restored beacon light column')

const renderStart = source.indexOf('static void RenderV15Frame(HDC hdc, RECT& rc) {')
const renderEnd = source.indexOf('\n\nstatic void RenderFrame(HDC hdc, RECT& rc) {', renderStart)
if (renderStart < 0 || renderEnd < 0) throw new Error('Missing v19 render replacement boundary')
const render = `static void RenderV15Frame(HDC hdc, RECT& rc) {
    const int clientW = rc.right - rc.left;
    const int clientH = rc.bottom - rc.top;
    DrawRect(hdc, 0, 0, clientW, clientH, COL_BG);
    g_agi.highResRenderer.framebuffer.blit(hdc, 0, 0, g_agi.highResDisplayMode);

    const int railX = 768;
    const int railW = std::max(0, clientW - railX);
    DrawRect(hdc, railX, 0, railW, clientH, RGB(12, 14, 27));
    DrawRect(hdc, railX, 0, 3, clientH, COL_BRIDGE);
    DrawText(hdc, railX + 12, 12, "GTAngel v19 / PLAYABLE CORE", COL_ACCENT, g_hFont);
    DrawText(hdc, railX + 12, 34, "ARIADNE AND THE BROKEN SIGNAL", COL_CYAN, g_hFontSmall);

    char line[256];
    snprintf(line, sizeof(line), "Stage  %s", g_agi.gameplay.stageName().c_str());
    DrawText(hdc, railX + 12, 62, line, g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_GOLD, g_hFontMono);
    DrawText(hdc, railX + 12, 84, "CURRENT PURPOSE", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 102, g_agi.gameplay.objective().c_str(), COL_TEXT, g_hFontSmall);
    DrawLine(hdc, railX + 12, 124, railX + railW - 12, 124, RGB(70, 70, 100));

    DrawText(hdc, railX + 12, 140, "ARIADNE", COL_PURPLE, g_hFont);
    snprintf(line, sizeof(line), "Routine  %s", g_agi.gameplay.routineName());
    DrawText(hdc, railX + 12, 164, line, COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "Disposition  %.0f%%", g_agi.gameplay.ariadne.disposition * 100.0f);
    DrawText(hdc, railX + 12, 182, line, COL_TEXT2, g_hFontSmall);
    DrawBar(hdc, railX + 12, 202, std::max(20, railW - 24), 8,
            g_agi.gameplay.ariadne.disposition, COL_PURPLE, RGB(36, 34, 55));

    DrawText(hdc, railX + 12, 232, "INVENTORY", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 252,
        g_agi.gameplay.quest.hasSignalShard ? "Signal Shard  x1" : "Signal Shard  --",
        g_agi.gameplay.quest.hasSignalShard ? COL_CYAN : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 270,
        g_agi.gameplay.quest.hasResonanceSigil ? "Resonance Sigil  x1" : "Resonance Sigil  --",
        g_agi.gameplay.quest.hasResonanceSigil ? COL_GOLD : COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, 304, "WORLD", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 324,
        g_agi.gameplay.quest.beaconRestored ? "Civic beacon  RESTORED" : "Civic beacon  BROKEN",
        g_agi.gameplay.quest.beaconRestored ? COL_CYAN : COL_RED, g_hFontSmall);
    snprintf(line, sizeof(line), "Frame %llu   %.2f ms",
        (unsigned long long)g_agi.highResRenderer.metrics.sequence,
        g_agi.highResRenderer.metrics.renderMs);
    DrawText(hdc, railX + 12, 344, line, COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, 378, "LAST GROUNDED EVENT", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 398, g_agi.gameplay.quest.lastEvent.c_str(), COL_TEXT, g_hFontSmall);
    snprintf(line, sizeof(line), "Events %u   Save %s",
        g_agi.gameplay.quest.groundedEvents,
        g_agi.gameplay.quest.persistenceStatus.c_str());
    DrawText(hdc, railX + 12, 418, line, COL_TEXT2, g_hFontSmall);

    if (g_agi.gameplay.quest.journalOpen) {
        DrawText(hdc, railX + 12, 452, "QUEST JOURNAL", COL_GOLD, g_hFontSmall);
        DrawText(hdc, railX + 12, 472, "1. Meet the keeper", g_agi.gameplay.quest.stage > v19game::QuestStage::Offered ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 490, "2. Recover the shard", g_agi.gameplay.quest.stage > v19game::QuestStage::SeekShard ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 508, "3. Restore the beacon", g_agi.gameplay.quest.stage > v19game::QuestStage::RestoreBeacon ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 526, "4. Return to Ariadne", g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    }

    DrawText(hdc, railX + 12, 570, "CONTROLS", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 590, "WASD move / turn", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 608, "E interact   1-3 choose", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 626, "J journal    R recover", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 644, "F5 save      F9 load", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 670, "Telemetry: OPTIONAL / OFF", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 692, "Player authority: LOCAL", COL_GOLD, g_hFontSmall);
    DrawText(hdc, railX + 12, 716, "[SPACE] start / pause", COL_TEXT2, g_hFontSmall);

    const std::string prompt = g_agi.gameplay.interactionPrompt(g_agi.navPos, g_agi.navHeading, g_agi.navWorld);
    if (!prompt.empty()) {
        DrawRect(hdc, 164, 674, 440, 42, RGB(20, 18, 34));
        DrawLine(hdc, 164, 674, 604, 674, COL_GOLD);
        DrawText(hdc, 190, 688, prompt.c_str(), COL_TEXT, g_hFont);
    }

    const avatar3d::Vec3 objective = g_agi.gameplay.objectivePosition(g_agi.navWorld);
    const float dx = objective.x - g_agi.navPos.x;
    const float dz = objective.z - g_agi.navPos.z;
    float bearing = atan2f(dx, dz) - g_agi.navHeading;
    while (bearing > 3.14159265f) bearing -= 6.2831853f;
    while (bearing < -3.14159265f) bearing += 6.2831853f;
    const int markerX = std::clamp(384 + (int)(bearing / 1.2f * 330.0f), 28, 740);
    DrawLine(hdc, markerX - 8, 28, markerX, 18, COL_GOLD);
    DrawLine(hdc, markerX, 18, markerX + 8, 28, COL_GOLD);
    DrawLine(hdc, markerX + 8, 28, markerX, 38, COL_GOLD);
    DrawLine(hdc, markerX, 38, markerX - 8, 28, COL_GOLD);

    if (g_agi.gameplay.quest.dialogueOpen) {
        DrawRect(hdc, 36, 520, 696, 214, RGB(13, 11, 25));
        DrawLine(hdc, 36, 520, 732, 520, COL_PURPLE);
        DrawText(hdc, 58, 540, "ARIADNE", COL_PURPLE, g_hFontLarge);
        const std::string dialogue = g_agi.gameplay.dialogueLine();
        RECT dialogueRect{58, 574, 710, 616};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COL_TEXT);
        HGDIOBJ previousDialogueFont = SelectObject(hdc, g_hFont);
        ::DrawTextA(hdc, dialogue.c_str(), -1, &dialogueRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(hdc, previousDialogueFont);
        const auto choices = g_agi.gameplay.dialogueChoices();
        if (!choices[0].empty()) DrawText(hdc, 58, 620, choices[0].c_str(), COL_GOLD, g_hFont);
        if (!choices[1].empty()) DrawText(hdc, 58, 648, choices[1].c_str(), COL_CYAN, g_hFont);
        if (!choices[2].empty()) DrawText(hdc, 58, 676, choices[2].c_str(), COL_TEXT2, g_hFont);
    }
}`
source = source.slice(0, renderStart) + render + source.slice(renderEnd)

exact("            } else if (wp == 'R') {\n                g_agi.reset();", "            } else if (wp == VK_HOME) {\n                g_agi.reset();", 'move global reset to Home')
exact("            } else if (wp == 'C' || wp == VK_F9) {", "            } else if (wp == 'C') {", 'reserve F9 for load')
exact("            } else if (wp == 'S') {\n                g_agi.civicSpine.setEnabled", "            } else if (wp == VK_F10) {\n                g_agi.civicSpine.setEnabled", 'move telemetry stream from S')
exact("            } else if (wp == 'E') {\n                g_agi.civicSpine.requestImmediate();", "            } else if (wp == VK_F11) {\n                g_agi.civicSpine.requestImmediate();", 'move telemetry immediate from E')
exact("            } else if (wp == VK_ESCAPE) PostQuitMessage(0);", `            } else if (wp == 'E') {
                g_agi.addLog(g_agi.gameplay.interact(g_agi.navPos, g_agi.navHeading, g_agi.navWorld));
            } else if (wp == '1' || wp == '2' || wp == '3') {
                g_agi.addLog(g_agi.gameplay.choose((int)(wp - '0'), g_agi.navWorld));
            } else if (wp == 'J') {
                g_agi.gameplay.quest.journalOpen = !g_agi.gameplay.quest.journalOpen;
                g_agi.addLog(g_agi.gameplay.quest.journalOpen ? "Quest journal opened" : "Quest journal closed");
            } else if (wp == 'R') {
                g_agi.addLog(g_agi.gameplay.recover(g_agi.navWorld));
            } else if (wp == VK_F5) {
                g_agi.addLog(g_agi.gameplay.save() ? "Gameplay saved" : "Gameplay save failed");
            } else if (wp == VK_F9) {
                g_agi.addLog(g_agi.gameplay.load(g_agi.navWorld) ? "Gameplay loaded" : "Gameplay load failed");
            } else if (wp == VK_ESCAPE) PostQuitMessage(0);`, 'gameplay input controls')
source = source.replaceAll('GTAngel v18', 'GTAngel v19')
source = source.replaceAll('GTAngelAGIv18', 'GTAngelAGIv19')
source = source.replace('GTAngel - Visible Self v18 / 768x768 Render Pyramid',
  'GTAngel v19 - Ariadne and the Broken Signal / 768x768 World')

const required = [
  'namespace v19game', 'Ariadne and the Broken Signal', 'ObjectNpc = 6', 'ObjectQuestItem = 7',
  'buildNpc(world)', 'buildQuestShard(world)', 'v19game::GameplayLoop gameplay',
  'gameplay.samplePlayerControls()', "wp == 'E'", 'wp == VK_F5', 'wp == VK_F9',
  'Telemetry: OPTIONAL / OFF', 'false && navWorld.goalDistance(navPos)'
]
for (const token of required) if (!source.includes(token)) throw new Error(`Missing generated v19 invariant: ${token}`)
if (source.includes("wp == 'S') {\n                g_agi.civicSpine.setEnabled")) throw new Error('S movement still toggles telemetry')
if (source.includes("wp == 'E') {\n                g_agi.civicSpine.requestImmediate")) throw new Error('E interaction still requests telemetry')

fs.writeFileSync(outputPath, source)
console.log(JSON.stringify({ ok: true, outputPath, bytes: Buffer.byteLength(source), requiredInvariants: required.length }))
