    void buildGoal(const avatar3d::GameWorld& world) {
        static constexpr int Segments = 16;
        const auto pulseTicks = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        float pulse = 0.78f + 0.22f * sinf((float)(pulseTicks % 1000000) * 0.004f);
        Colour goal = world.beaconRestored
            ? Colour{(uint8_t)std::lround(65.0f + 45.0f * pulse),
                     (uint8_t)std::lround(205.0f + 45.0f * pulse),
                     (uint8_t)std::lround(225.0f + 30.0f * pulse), 255}
            : Colour{(uint8_t)std::lround(145.0f + 70.0f * pulse),
                     (uint8_t)std::lround(75.0f + 40.0f * pulse),
                     (uint8_t)std::lround(35.0f + 25.0f * pulse), 255};
        for (int segment = 0; segment < Segments; ++segment) {
            float a0 = 6.2831853f * segment / Segments;
            float a1 = 6.2831853f * (segment + 1) / Segments;
            const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
            // Stepped stone pedestal base (0 .. 0.55m) + glowing energy ring
            const Colour ped = (segment & 1) ? Colour{92, 88, 98, 255} : Colour{78, 74, 84, 255};
            addQuad({world.goal.x + c0*0.58f, 0.0f,  world.goal.z + s0*0.58f},
                    {world.goal.x + c1*0.58f, 0.0f,  world.goal.z + s1*0.58f},
                    {world.goal.x + c1*0.44f, 0.52f, world.goal.z + s1*0.44f},
                    {world.goal.x + c0*0.44f, 0.52f, world.goal.z + s0*0.44f}, ped, ObjectGoal);
            // Faceted crystal spire (0.52m .. 2.85m) + apex pyramid (2.85m .. 3.45m)
            const Colour spire = (segment & 1) ? goal : Colour{(uint8_t)(goal.b*4/5), (uint8_t)(goal.g*4/5), (uint8_t)(goal.r*4/5), 255};
            Vec3 p0{world.goal.x + c0 * 0.28f, 0.52f, world.goal.z + s0 * 0.28f};
            Vec3 p1{world.goal.x + c1 * 0.28f, 0.52f, world.goal.z + s1 * 0.28f};
            Vec3 p2{world.goal.x + c1 * 0.14f, 2.85f, world.goal.z + s1 * 0.14f};
            Vec3 p3{world.goal.x + c0 * 0.14f, 2.85f, world.goal.z + s0 * 0.14f};
            addQuad(p0, p1, p2, p3, spire, ObjectGoal);
            addTriangle({world.goal.x, 3.45f, world.goal.z}, p3, p2, goal, ObjectGoal);
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
        // LW39 Act II landmark 1: Obsidian & Gold Memory Obelisk at (-9.0, 0, -2.4)
        {
            const Vec3 ob{-9.0f, 0.0f, -2.4f};
            const Colour basalt{54, 46, 62, 255}, runeGold{75, 205, 250, 255}, violet{195, 110, 235, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({ob.x + c0*0.52f, 0.0f,  ob.z + s0*0.52f},
                        {ob.x + c1*0.52f, 0.0f,  ob.z + s1*0.52f},
                        {ob.x + c1*0.38f, 0.45f, ob.z + s1*0.38f},
                        {ob.x + c0*0.38f, 0.45f, ob.z + s0*0.38f}, basalt, ObjectQuestItem);
                addQuad({ob.x + c0*0.26f, 0.45f, ob.z + s0*0.26f},
                        {ob.x + c1*0.26f, 0.45f, ob.z + s1*0.26f},
                        {ob.x + c1*0.14f, 2.65f, ob.z + s1*0.14f},
                        {ob.x + c0*0.14f, 2.65f, ob.z + s0*0.14f}, (s & 1) ? basalt : violet, ObjectQuestItem);
                addTriangle({ob.x, 3.25f, ob.z},
                            {ob.x + c0*0.16f, 2.65f, ob.z + s0*0.16f},
                            {ob.x + c1*0.16f, 2.65f, ob.z + s1*0.16f}, runeGold, ObjectQuestItem);
            }
        }
        // LW39 Act II landmark 2: Harmonic Transit Relay Spire at (9.0, 0, 2.4)
        {
            const Vec3 rl{9.0f, 0.0f, 2.4f};
            const Colour steel{88, 96, 112, 255}, harmonic{225, 235, 90, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({rl.x + c0*0.48f, 0.0f,  rl.z + s0*0.48f},
                        {rl.x + c1*0.48f, 0.0f,  rl.z + s1*0.48f},
                        {rl.x + c1*0.22f, 2.75f, rl.z + s1*0.22f},
                        {rl.x + c0*0.22f, 2.75f, rl.z + s0*0.22f}, (s & 1) ? steel : harmonic, ObjectQuestItem);
                addTriangle({rl.x, 3.40f, rl.z},
                            {rl.x + c0*0.24f, 2.75f, rl.z + s0*0.24f},
                            {rl.x + c1*0.24f, 2.75f, rl.z + s1*0.24f}, harmonic, ObjectQuestItem);
            }
        }
        // LW40 Act III landmark 3: Faustian Shadow Node at (2.6, 0, -18.0)
        // & Skyline Resonance Tower at (-18.0, 0, 2.6).
        {
            const Vec3 sn{2.6f, 0.0f, -18.0f};
            const Colour darkCore{44, 34, 72, 255}, ember{75, 85, 235, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({sn.x + c0*0.50f, 0.0f,  sn.z + s0*0.50f},
                        {sn.x + c1*0.50f, 0.0f,  sn.z + s1*0.50f},
                        {sn.x + c1*0.20f, 2.90f, sn.z + s1*0.20f},
                        {sn.x + c0*0.20f, 2.90f, sn.z + s0*0.20f}, (s & 1) ? darkCore : ember, ObjectQuestItem);
                addTriangle({sn.x, 3.55f, sn.z},
                            {sn.x + c0*0.22f, 2.90f, sn.z + s0*0.22f},
                            {sn.x + c1*0.22f, 2.90f, sn.z + s1*0.22f}, ember, ObjectQuestItem);
            }
            const Vec3 st{-18.0f, 0.0f, 2.6f};
            const Colour towerCol{115, 195, 85, 255}, crownCol{185, 245, 130, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({st.x + c0*0.54f, 0.0f,  st.z + s0*0.54f},
                        {st.x + c1*0.54f, 0.0f,  st.z + s1*0.54f},
                        {st.x + c1*0.22f, 3.20f, st.z + s1*0.22f},
                        {st.x + c0*0.22f, 3.20f, st.z + s0*0.22f}, (s & 1) ? towerCol : crownCol, ObjectQuestItem);
                addTriangle({st.x, 3.85f, st.z},
                            {st.x + c0*0.24f, 3.20f, st.z + s0*0.24f},
                            {st.x + c1*0.24f, 3.20f, st.z + s1*0.24f}, crownCol, ObjectQuestItem);
            }
        }
    }

    void addTaperedBone(Vec3 start, Vec3 end, float startRadius, float endRadius, Colour colour, uint16_t objectId = ObjectAvatar) {
        Vec3 axis = normalize3(end - start);
        Vec3 reference = fabsf(axis.y) < 0.86f ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        Vec3 u = normalize3(cross3(axis, reference));
        Vec3 v = normalize3(cross3(axis, u));
        static constexpr int Segments = 10;
        for (int segment = 0; segment < Segments; ++segment) {
            float a0 = 6.2831853f * segment / Segments;
            float a1 = 6.2831853f * (segment + 1) / Segments;
            Vec3 start0 = u * (cosf(a0) * startRadius) + v * (sinf(a0) * startRadius);
            Vec3 start1 = u * (cosf(a1) * startRadius) + v * (sinf(a1) * startRadius);
            Vec3 end0 = u * (cosf(a0) * endRadius) + v * (sinf(a0) * endRadius);
            Vec3 end1 = u * (cosf(a1) * endRadius) + v * (sinf(a1) * endRadius);
            addQuad(start + start0, start + start1, end + end1, end + end0, colour, objectId);
        }
    }

    void addJoint(Vec3 centre, float radius, Colour colour = Colour{235, 154, 247, 255}, uint16_t objectId = ObjectAvatar) {
        Vec3 top = centre + Vec3{0, radius, 0};
        Vec3 bottom = centre + Vec3{0, -radius, 0};
        Vec3 left = centre + Vec3{-radius, 0, 0};
        Vec3 right = centre + Vec3{radius, 0, 0};
        Vec3 front = centre + Vec3{0, 0, radius};
        Vec3 back = centre + Vec3{0, 0, -radius};
        addTriangle(top, front, right, colour, objectId);
        addTriangle(top, right, back, colour, objectId);
        addTriangle(top, back, left, colour, objectId);
        addTriangle(top, left, front, colour, objectId);
        addTriangle(bottom, right, front, colour, objectId);
        addTriangle(bottom, back, right, colour, objectId);
        addTriangle(bottom, left, back, colour, objectId);
        addTriangle(bottom, front, left, colour, objectId);
    }

    void addBlobShadow(Vec3 centre, float radius) {
        // LW32: soft dark ellipse grounding each entity.
        static constexpr int Segments = 10;
        const Colour shade{26, 24, 22, 255};
        for (int i = 0; i < Segments; ++i) {
            const float a0 = 6.2831853f * i / Segments, a1 = 6.2831853f * (i + 1) / Segments;
            addTriangle(centre + Vec3{0, 0.013f, 0},
                        centre + Vec3{cosf(a0) * radius, 0.013f, sinf(a0) * radius},
                        centre + Vec3{cosf(a1) * radius, 0.013f, sinf(a1) * radius},
                        shade, ObjectGround);
        }
    }
    void boxLimb(Vec3 a, Vec3 b, float w, float d, Colour colour, uint16_t objectId = ObjectNpc) {
        // LW31: oriented rectangular limb prism between two skeleton joints.
        Vec3 axis = normalize3(b - a);
        if (!std::isfinite(axis.x) || !std::isfinite(axis.y) || !std::isfinite(axis.z)) return;
        Vec3 reference = fabsf(axis.y) < 0.86f ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        Vec3 u = normalize3(cross3(axis, reference));
        Vec3 v = normalize3(cross3(axis, u));
        const float hw = w * 0.5f, hd = d * 0.5f;
        Vec3 p[8];
        for (int i = 0; i < 2; ++i) {
            const Vec3 base = i ? b : a;
            p[i*4+0] = base + u*hw + v*hd;
            p[i*4+1] = base + u*hw - v*hd;
            p[i*4+2] = base - u*hw - v*hd;
            p[i*4+3] = base - u*hw + v*hd;
        }
        for (int s = 0; s < 4; ++s) {
            const int n = (s + 1) & 3;
            addQuad(p[s], p[n], p[4+n], p[4+s], colour, objectId);
        }
    }
