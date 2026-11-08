    void buildProps(const avatar3d::GameWorld& world) {
        // LW36: tapered cylindrical street-light columns, curved luminaire arm,
        // compact LED cobra-head fixture, and ground-level (y=0.019f) circular
        // sodium light pool (fixes the old y=2.71m floating pool square).
        for (const auto& lamp : world.props) {
            const Vec3 base = lamp.pos;
            const int lampIdx = (int)((lamp.pos.x + lamp.pos.z) * 0.5f) & 1;
            const float armDir = lampIdx ? 0.65f : -0.65f;
            const Colour poleCol{82, 88, 98, 255}, baseCol{58, 62, 72, 255};
            addTaperedBone(base, base + Vec3{0, 0.45f, 0}, 0.11f, 0.075f, baseCol, ObjectProp);
            addTaperedBone(base + Vec3{0, 0.45f, 0}, base + Vec3{0, 2.85f, 0}, 0.065f, 0.042f, poleCol, ObjectProp);
            const Vec3 head = base + Vec3{armDir, 2.95f, 0};
            addTaperedBone(base + Vec3{0, 2.75f, 0}, head, 0.042f, 0.032f, poleCol, ObjectProp);
            addTaperedBone(head + Vec3{-armDir*0.18f, 0.04f, 0}, head + Vec3{armDir*0.18f, 0.02f, 0}, 0.085f, 0.065f, baseCol, ObjectProp);
            addJoint(head + Vec3{0, -0.04f, 0}, 0.095f, Colour{150, 235, 255, 255}, ObjectProp);
            const Vec3 pool{head.x, 0.019f, head.z};
            for (int i = 0; i < 10; ++i) {
                const float a0 = 6.2831853f * i / 10.0f, a1 = 6.2831853f * (i + 1) / 10.0f;
                addTriangle(pool,
                            {pool.x + cosf(a0)*0.95f, 0.019f, pool.z + sinf(a0)*0.95f},
                            {pool.x + cosf(a1)*0.95f, 0.019f, pool.z + sinf(a1)*0.95f},
                            Colour{58, 86, 108, 255}, ObjectProp);
            }
        }
        // LW46: high-clearance outer-corner traffic signal gantries (+/-3.65m)
        const int sigPhase = ((int)(world.trafficTime * 0.35f)) & 3;
        const Vec3 sigCorners[4] = {{3.65f, 0.0f, 3.65f}, {-3.65f, 0.0f, -3.65f}, {3.65f, 0.0f, -3.65f}, {-3.65f, 0.0f, 3.65f}};
        for (int sc = 0; sc < 4; ++sc) {
            const Vec3 sp = sigCorners[sc];
            const float armX = (sp.x > 0.0f) ? -1.75f : 1.75f;
            addTaperedBone(sp, sp + Vec3{0, 3.85f, 0}, 0.068f, 0.048f, Colour{72, 78, 92, 255}, ObjectProp);
            addTaperedBone(sp + Vec3{0, 3.78f, 0}, sp + Vec3{armX, 3.82f, 0}, 0.042f, 0.032f, Colour{72, 78, 92, 255}, ObjectProp);
            const Vec3 boxPos = sp + Vec3{armX * 0.84f, 3.58f, 0};
            addTaperedBone(boxPos + Vec3{0, -0.24f, 0}, boxPos + Vec3{0, 0.24f, 0}, 0.075f, 0.075f, Colour{32, 34, 42, 255}, ObjectProp);
            const bool ewGreen = ((sc < 2) ? (sigPhase < 2) : (sigPhase >= 2));
            const Colour lens = ewGreen ? Colour{95, 245, 85, 255}
                              : (sigPhase & 1) ? Colour{65, 215, 255, 255}
                                               : Colour{55, 65, 255, 255};
            for (float lz : {-0.08f, 0.08f})
                addJoint(boxPos + Vec3{0, ewGreen ? -0.10f : 0.10f, lz}, 0.078f, lens, ObjectProp);
        }
    }

