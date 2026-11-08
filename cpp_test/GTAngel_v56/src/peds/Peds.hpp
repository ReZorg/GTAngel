    // ── Z++ Formal Spec Contract (peds): 17 modules, 673 schemas, 31 enums (CivilianPed.zpp, CopPed.zpp, DummyPed.zpp, EmergencyPed.zpp, Gangs.zpp, Ped.zpp, PedAI.zpp, PedAttractor.zpp) ──
    static constexpr int zpp_peds_modules() { return 17; }
    static constexpr int zpp_peds_schemas() { return 673; }
    void buildWalkers(const avatar3d::GameWorld& world) {
        // Population rendering: compact humanoids (torso + head + legs) so
        // the retina sees real social entities, not empty pillars.
        for (const auto& w : world.walkers) {
            avatar3d::AvatarSkeleton sk;
            sk.pose(w.pos, w.heading, w.speed, 0.05f);
            addBlobShadow(w.pos, 0.30f); // LW32: ground each ped
            // LW27: per-walker clothing palette (deterministic position hash).
            const unsigned wh = (unsigned)(((int)(w.pos.x * 7.0f)) ^ ((int)(w.pos.z * 13.0f) << 2));
            Colour suit;
            switch ((wh >> 2) & 3u) {
                case 0: suit = Colour{96, 130, 182, 255}; break;  // classic teal
                case 1: suit = Colour{40, 60, 170, 255}; break;   // rust
                case 2: suit = Colour{70, 150, 90, 255}; break;   // olive
                default: suit = Colour{150, 90, 60, 255}; break;  // plum
            }
            // LW34: smooth 10-segment anatomical pedestrians with jacket,
            // trousers, belt, boots, skin-toned head/hands, and styled hair.
            using J = avatar3d::AvatarSkeleton;
            sk.walkBlend = std::clamp(w.speed / 0.65f, 0.45f, 1.0f);
            sk.walkPhase = world.walkerTime * w.speed * 4.8f + w.phase;
            sk.idleTime  = world.walkerTime + w.phase;
            sk.pose(w.pos, w.heading, w.speed, 0.0f);
            const Colour skin = Colour{178, 198, 238, 255};
            const Colour pants = Colour{(uint8_t)(suit.b * 6 / 10), (uint8_t)(suit.g * 6 / 10), (uint8_t)(suit.r * 6 / 10), 255};
            const Colour hair = (wh & 4u) ? Colour{38, 44, 58, 255}
                              : (wh & 8u) ? Colour{70, 145, 215, 255}
                                          : Colour{165, 115, 210, 255};
            const Colour shoe = Colour{42, 38, 36, 255};
            addTaperedBone(sk.joints[J::Root], sk.joints[J::Spine], 0.135f, 0.145f, pants, ObjectNpc);
            addTaperedBone(sk.joints[J::Spine], sk.joints[J::Neck], 0.155f, 0.130f, suit, ObjectNpc);
            addTaperedBone(sk.joints[J::Neck], sk.joints[J::Head], 0.060f, 0.065f, skin, ObjectNpc);
            addJoint(sk.joints[J::Head], 0.118f, skin, ObjectNpc);
            addTaperedBone(sk.joints[J::Head] + Vec3{0.0f, 0.01f, -0.02f},
                           sk.joints[J::Head] + Vec3{0.0f, 0.11f, -0.03f}, 0.122f, 0.085f, hair, ObjectNpc);
            // LW45: facial eyes + handheld glowing cyan datapad on select citizens
            {
                const float wch = cosf(w.heading), wsh = sinf(w.heading);
                auto hLoc = [&](float lx, float ly, float lz) {
                    return sk.joints[J::Head] + Vec3{lx*wch + lz*wsh, ly, -lx*wsh + lz*wch};
                };
                addJoint(hLoc(-0.042f, 0.01f, 0.105f), 0.022f, Colour{32, 28, 36, 255}, ObjectNpc);
                addJoint(hLoc( 0.042f, 0.01f, 0.105f), 0.022f, Colour{32, 28, 36, 255}, ObjectNpc);
                if ((wh & 3u) == 0u)
                    addJoint(sk.joints[J::HandR] + Vec3{0.0f, 0.02f, 0.0f}, 0.048f, Colour{235, 220, 85, 255}, ObjectNpc);
            }
            // Collar/scarf accent + optional long coat/skirt flare
            const Colour accent = (wh & 2u) ? Colour{215, 195, 95, 255} : Colour{220, 225, 235, 255};
            addTaperedBone(sk.joints[J::Neck] + Vec3{0, -0.04f, 0}, sk.joints[J::Neck] + Vec3{0, 0.02f, 0}, 0.145f, 0.085f, accent, ObjectNpc);
            if (wh & 1u)
                addTaperedBone(sk.joints[J::Root] + Vec3{0, -0.20f, 0}, sk.joints[J::Root] + Vec3{0, 0.05f, 0}, 0.195f, 0.140f, suit, ObjectNpc);
            for (int side = 0; side < 2; ++side) {
                const int hip = side ? J::HipR : J::HipL, knee = side ? J::KneeR : J::KneeL;
                const int foot = side ? J::FootR : J::FootL;
                const int sh = side ? J::ShoulderR : J::ShoulderL, el = side ? J::ElbowR : J::ElbowL;
                const int hd = side ? J::HandR : J::HandL;
                addJoint(sk.joints[sh], 0.068f, suit, ObjectNpc);
                addTaperedBone(sk.joints[sh], sk.joints[el], 0.058f, 0.048f, suit, ObjectNpc);
                addJoint(sk.joints[el], 0.048f, suit, ObjectNpc);
                addTaperedBone(sk.joints[el], sk.joints[hd], 0.046f, 0.036f, skin, ObjectNpc);
                if (side == 0 && ((wh & 1u) != 0u)) {
                    // LW52 (Ped.zpp / Bones.zpp BONE_spine = 2): commuter backpack on citizen torso
                    const Vec3 backDir{-sinf(w.heading) * 0.11f, 0.02f, -cosf(w.heading) * 0.11f};
                    addTaperedBone(sk.joints[J::Spine] + backDir, sk.joints[J::Neck] + backDir, 0.078f, 0.068f, accent, ObjectNpc);
                }
                addJoint(sk.joints[hd], 0.042f, skin, ObjectNpc);
                addTaperedBone(sk.joints[hip], sk.joints[knee], 0.078f, 0.062f, pants, ObjectNpc);
                addJoint(sk.joints[knee], 0.058f, pants, ObjectNpc);
                addTaperedBone(sk.joints[knee], sk.joints[foot], 0.060f, 0.046f, pants, ObjectNpc);
                addJoint(sk.joints[foot] + Vec3{0.0f, 0.02f, 0.03f}, 0.062f, shoe, ObjectNpc);
            }
        }
    }

    void buildNpc(const avatar3d::GameWorld& world) {
        // LW37: sculpted Ariadne keeper — tailored cyber-coat, flared coat
        // skirt, shoulder mantle, skin-toned head/hands, silver-lilac hair,
        // and glowing resonance circlet.
        avatar3d::AvatarSkeleton npc;
        npc.walkBlend = std::clamp(world.npcSpeed / 0.65f, 0.0f, 1.0f);
        npc.walkPhase = world.walkerTime * 3.8f;
        npc.idleTime  = world.walkerTime;
        npc.pose(world.npcPos, world.npcHeading, std::min(1.0f, world.npcSpeed), 0.0f);
        addBlobShadow(world.npcPos, 0.36f);
        using J = avatar3d::AvatarSkeleton;
        const Colour coat  = world.questComplete ? Colour{110, 205, 245, 255} : Colour{168, 88, 210, 255};
        const Colour trim  = world.questComplete ? Colour{180, 240, 255, 255} : Colour{235, 165, 248, 255};
        const Colour boots = Colour{48, 38, 56, 255};
        const Colour skin  = Colour{182, 204, 242, 255};
        const Colour hair  = Colour{235, 210, 225, 255};
        // Flared lower coat skirt (hip to mid-thigh) + fitted upper bodice
        addTaperedBone(npc.joints[J::Root] + Vec3{0, -0.22f, 0}, npc.joints[J::Spine], 0.22f, 0.135f, coat, ObjectNpc);
        addTaperedBone(npc.joints[J::Spine], npc.joints[J::Neck], 0.145f, 0.135f, coat, ObjectNpc);
        addTaperedBone(npc.joints[J::Neck] + Vec3{0, -0.06f, 0}, npc.joints[J::Neck] + Vec3{0, 0.02f, 0}, 0.175f, 0.095f, trim, ObjectNpc);
        addTaperedBone(npc.joints[J::Neck], npc.joints[J::Head], 0.058f, 0.062f, skin, ObjectNpc);
        addJoint(npc.joints[J::Head], 0.128f, skin, ObjectNpc);
        addJoint(npc.joints[J::Head] + Vec3{0.0f, 0.052f, -0.018f}, 0.138f, hair, ObjectNpc);
        addJoint(npc.joints[J::Head] + Vec3{0.0f, 0.125f, 0.02f}, 0.055f, Colour{95, 235, 255, 255}, ObjectNpc);
        for (int side = 0; side < 2; ++side) {
            const float s = side ? 1.0f : -1.0f;
            const int hip = side ? J::HipR : J::HipL, knee = side ? J::KneeR : J::KneeL, foot = side ? J::FootR : J::FootL;
            const int sh = side ? J::ShoulderR : J::ShoulderL, el = side ? J::ElbowR : J::ElbowL, hd = side ? J::HandR : J::HandL;
            // Twin hair braids + tailored sleeves + boots
            addTaperedBone(npc.joints[J::Head] + Vec3{s*0.10f, 0.02f, -0.04f},
                           npc.joints[J::Spine] + Vec3{s*0.14f, 0.05f, -0.08f}, 0.048f, 0.028f, hair, ObjectNpc);
            addJoint(npc.joints[sh], 0.072f, trim, ObjectNpc);
            addTaperedBone(npc.joints[sh], npc.joints[el], 0.058f, 0.048f, coat, ObjectNpc);
            addTaperedBone(npc.joints[el], npc.joints[hd], 0.046f, 0.038f, coat, ObjectNpc);
            addJoint(npc.joints[hd], 0.042f, skin, ObjectNpc);
            addTaperedBone(npc.joints[hip], npc.joints[knee], 0.072f, 0.058f, coat, ObjectNpc);
            addTaperedBone(npc.joints[knee], npc.joints[foot], 0.058f, 0.046f, boots, ObjectNpc);
            addJoint(npc.joints[foot] + Vec3{0.0f, 0.02f, 0.03f}, 0.062f, boots, ObjectNpc);
        }
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

