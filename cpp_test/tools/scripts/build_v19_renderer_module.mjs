import fs from 'node:fs'

const input = '/home/ubuntu/gtangel-v15-build/gtangel_agi_v15_hires_renderer.cpp'
const output = '/home/ubuntu/gtangel-v19-work/gtangel_agi_v19_hires_renderer.cpp'
let source = fs.readFileSync(input, 'utf8')

const exact = (from, to, label) => {
  if (!source.includes(from)) throw new Error(`Missing renderer anchor: ${label}`)
  source = source.replace(from, to)
}

exact('    ObjectAvatar = 4,\n    ObjectGrid = 5', `    ObjectAvatar = 4,
    ObjectGrid = 5,
    ObjectNpc = 6,
    ObjectQuestItem = 7`, 'semantic object IDs')
exact('        buildGoal(world);\n        if (thirdPerson) buildAvatar(avatar);', `        buildGoal(world);
        buildNpc(world);
        buildQuestShard(world);
        if (thirdPerson) buildAvatar(avatar);`, 'gameplay render calls')
exact('        Colour goal{(uint8_t)std::lround(80.0f + 60.0f * pulse),\n                    (uint8_t)std::lround(180.0f + 70.0f * pulse),\n                    (uint8_t)std::lround(70.0f + 45.0f * pulse), 255};', `        Colour goal = world.beaconRestored
            ? Colour{(uint8_t)std::lround(65.0f + 45.0f * pulse),
                     (uint8_t)std::lround(205.0f + 45.0f * pulse),
                     (uint8_t)std::lround(225.0f + 30.0f * pulse), 255}
            : Colour{(uint8_t)std::lround(145.0f + 70.0f * pulse),
                     (uint8_t)std::lround(75.0f + 40.0f * pulse),
                     (uint8_t)std::lround(35.0f + 25.0f * pulse), 255};`, 'beacon material state')
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

    void addTaperedBone`, 'beacon light column')
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
        const float radius = 0.28f;
        const Vec3 c = world.questShard + Vec3{0.0f, 0.68f, 0.0f};
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

    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {`, 'NPC and shard geometry')

for (const token of ['ObjectNpc = 6', 'ObjectQuestItem = 7', 'buildNpc(world)', 'buildQuestShard(world)', 'world.beaconRestored'])
  if (!source.includes(token)) throw new Error(`Missing v19 renderer invariant: ${token}`)

fs.writeFileSync(output, source)
console.log(JSON.stringify({ ok: true, output, bytes: Buffer.byteLength(source) }))
