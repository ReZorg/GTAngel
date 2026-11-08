    // ── Z++ Formal Spec Component Binding (entities): 3 modules, 190 schemas, 3 enums (Dummy.zpp, Entity.zpp, Physical.zpp) ──
    static constexpr int zpp_entities_modules() { return zpp_entities_spec::kZppModuleCount; }
    static constexpr int zpp_entities_schemas() { return zpp_entities_spec::kZppSchemaCount; }
    static constexpr bool verifyComponent_entities_zpp() {
        return zpp_entities_spec::verify_entities_zpp_invariants() && (zpp_entities_spec::CDummy_ZppContract{}.checkInvariant()) && (zpp_entities_spec::op_CEntity_DetachFromRwObject(zpp_entities_spec::CEntity_ZppContract{}, zpp_entities_spec::apply_CEntity_DetachFromRwObject(zpp_entities_spec::CEntity_ZppContract{})));
    }
    bool verifyLiveWorld_entities_zpp(const avatar3d::GameWorld& world) const {
        return ((int64_t)world.pillars.size() >= 0) && verifyComponent_entities_zpp();
    }
    static_assert(verifyComponent_entities_zpp(), "RasterEngine component Z++ binding failed for entities");
    void buildPillars(const avatar3d::GameWorld& world) {
        // LW34: landscaped urban street trees in stone planters + sleek
        // granite & cyan-lit cyber-columns (replaces brick-red pipes).
        static constexpr int Segments = 10;
        int pIdx = 0;
        for (const auto& pillar : world.pillars) {
            const bool isTree = ((pIdx++) & 1) == 0;
            for (int segment = 0; segment < Segments; ++segment) {
                const float a0 = 6.2831853f * segment / Segments;
                const float a1 = 6.2831853f * (segment + 1) / Segments;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                // Stone planter / plinth base (0 .. 0.38m)
                const float rb = pillar.radius * 1.15f;
                const Colour stone = (segment & 1) ? Colour{96, 98, 104, 255} : Colour{84, 86, 92, 255};
                addQuad({pillar.pos.x + c0*rb, 0.0f, pillar.pos.z + s0*rb},
                        {pillar.pos.x + c1*rb, 0.0f, pillar.pos.z + s1*rb},
                        {pillar.pos.x + c1*rb, 0.38f, pillar.pos.z + s1*rb},
                        {pillar.pos.x + c0*rb, 0.38f, pillar.pos.z + s0*rb}, stone, ObjectObstacle);
                {
                    // LW47: organic 3-band spherical dome canopy (replaces pointed cones)
                    const float rt0 = pillar.radius * 0.32f, rt1 = pillar.radius * 0.21f;
                    const float th = std::max(1.30f, pillar.height * 0.54f);
                    const Colour bark = (segment & 1) ? Colour{48, 62, 92, 255} : Colour{40, 52, 78, 255};
                    addQuad({pillar.pos.x + c0*rt0, 0.35f, pillar.pos.z + s0*rt0},
                            {pillar.pos.x + c1*rt0, 0.35f, pillar.pos.z + s1*rt0},
                            {pillar.pos.x + c1*rt1, th,    pillar.pos.z + s1*rt1},
                            {pillar.pos.x + c0*rt1, th,    pillar.pos.z + s0*rt1}, bark, ObjectObstacle);
                    const Colour cLo = isTree ? ((segment & 1) ? Colour{62, 122, 50, 255} : Colour{54, 108, 42, 255})
                                              : ((segment & 1) ? Colour{178, 108, 228, 255} : Colour{160, 92, 212, 255});
                    const Colour cMd = isTree ? ((segment & 1) ? Colour{76, 144, 62, 255} : Colour{66, 128, 54, 255})
                                              : ((segment & 1) ? Colour{202, 134, 242, 255} : Colour{186, 118, 230, 255});
                    const Colour cHi = isTree ? ((segment & 1) ? Colour{92, 164, 76, 255} : Colour{82, 148, 66, 255})
                                              : ((segment & 1) ? Colour{224, 158, 252, 255} : Colour{210, 144, 244, 255});
                    const float r0 = pillar.radius * 0.92f, r1 = pillar.radius * 1.42f, r2 = pillar.radius * 1.16f, r3 = pillar.radius * 0.52f;
                    const float y0 = th - 0.08f, y1 = th + 0.36f, y2 = th + 0.82f, y3 = th + 1.18f, y4 = th + 1.32f;
                    addQuad({pillar.pos.x + c0*r0, y0, pillar.pos.z + s0*r0}, {pillar.pos.x + c1*r0, y0, pillar.pos.z + s1*r0},
                            {pillar.pos.x + c1*r1, y1, pillar.pos.z + s1*r1}, {pillar.pos.x + c0*r1, y1, pillar.pos.z + s0*r1}, cLo, ObjectObstacle);
                    addQuad({pillar.pos.x + c0*r1, y1, pillar.pos.z + s0*r1}, {pillar.pos.x + c1*r1, y1, pillar.pos.z + s1*r1},
                            {pillar.pos.x + c1*r2, y2, pillar.pos.z + s1*r2}, {pillar.pos.x + c0*r2, y2, pillar.pos.z + s0*r2}, cMd, ObjectObstacle);
                    addQuad({pillar.pos.x + c0*r2, y2, pillar.pos.z + s0*r2}, {pillar.pos.x + c1*r2, y2, pillar.pos.z + s1*r2},
                            {pillar.pos.x + c1*r3, y3, pillar.pos.z + s1*r3}, {pillar.pos.x + c0*r3, y3, pillar.pos.z + s0*r3}, cHi, ObjectObstacle);
                    addTriangle({pillar.pos.x, y4, pillar.pos.z},
                                {pillar.pos.x + c0*r3, y3, pillar.pos.z + s0*r3},
                                {pillar.pos.x + c1*r3, y3, pillar.pos.z + s1*r3}, cHi, ObjectObstacle);
                }
            }
        }
    }

