#pragma once
    // ── Z++ Formal Spec Component Binding (buildings): 3 modules, 18 schemas, 0 enums (Building.zpp, Solid.zpp, Treadable.zpp) ──
    static constexpr int zpp_buildings_modules() { return zpp_buildings_spec::kZppModuleCount; }
    static constexpr int zpp_buildings_schemas() { return zpp_buildings_spec::kZppSchemaCount; }
    static constexpr bool verifyComponent_buildings_zpp() {
        return zpp_buildings_spec::verify_buildings_zpp_invariants() && (zpp_buildings_spec::CBuilding_ZppContract{}.checkInvariant()) && (zpp_buildings_spec::op_CBuilding_ReplaceWithNewModel(zpp_buildings_spec::CBuilding_ZppContract{}, zpp_buildings_spec::apply_CBuilding_ReplaceWithNewModel(zpp_buildings_spec::CBuilding_ZppContract{})));
    }
    bool verifyLiveWorld_buildings_zpp(const avatar3d::GameWorld& world) const {
        for (size_t i = 0; i < world.buildings.size(); ++i) {
            const auto& b = world.buildings[i];
            zpp_buildings_spec::CBuilding_ZppContract c{};
            c.instance_addr = (b.w > 0.0f && b.d > 0.0f && b.h > 0.0f) ? (int64_t)(i + 1) : -1;
            c.m_type = 1u;
            c.bUsesCollision = 1u;
            c.bIsBIGBuilding = (b.h > 4.2f) ? 1u : 0u;
            if (!c.checkInvariant()) return false;
        }
        return true;
    }
    static_assert(verifyComponent_buildings_zpp(), "RasterEngine component Z++ binding failed for buildings");
    void buildBuildings(const avatar3d::GameWorld& world) {
        if (!verifyLiveWorld_buildings_zpp(world)) return;
        // LW34: architectural night-city buildings with multi-column x multi-floor
        // exterior window grids on all 4 facades (placed OUTSIDE the walls so
        // depth-buffering never hides them), stone plinths, floor-ledge cornices,
        // ground-floor glass storefronts, neon signs, 4-sided parapets, and
        // rooftop mechanical penthouses.
        const unsigned flicker = (unsigned)(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count() / 4u);
        for (const auto& b : world.buildings) {
            if (std::fabs(b.pos.x - eye_.x) > 48.0f || std::fabs(b.pos.z - eye_.z) > 48.0f) continue;
            const float hw = b.w * 0.5f, hd = b.d * 0.5f;
            const Vec3 a{b.pos.x - hw, 0, b.pos.z - hd}, c{b.pos.x + hw, 0, b.pos.z + hd};
            const uint32_t bh = (uint32_t)((int)b.pos.x * 83492791) ^ (uint32_t)((int)b.pos.z * 19349663);
            Colour wall, wallAlt, trim;
            switch ((bh >> 4) & 3) {
                case 0: wall = Colour{108, 96, 86, 255};  wallAlt = Colour{92, 82, 72, 255};  trim = Colour{142, 130, 118, 255}; break;
                case 1: wall = Colour{98, 84, 106, 255};  wallAlt = Colour{84, 70, 92, 255};  trim = Colour{136, 118, 145, 255}; break;
                case 2: wall = Colour{86, 98, 112, 255};  wallAlt = Colour{72, 84, 96, 255};  trim = Colour{124, 138, 152, 255}; break;
                default: wall = Colour{110, 94, 78, 255}; wallAlt = Colour{94, 78, 64, 255}; trim = Colour{148, 132, 114, 255}; break;
            }
            const Colour roof{68, 64, 72, 255};
            addShadedQuad({a.x, 0, a.z}, {a.x, b.h, a.z}, {c.x, b.h, a.z}, {c.x, 0, a.z}, wall, ObjectObstacle, 6);
            addShadedQuad({c.x, 0, a.z}, {c.x, b.h, a.z}, {c.x, b.h, c.z}, {c.x, 0, c.z}, wallAlt, ObjectObstacle, 6);
            addShadedQuad({c.x, 0, c.z}, {c.x, b.h, c.z}, {a.x, b.h, c.z}, {a.x, 0, c.z}, wall, ObjectObstacle, 6);
            addShadedQuad({a.x, 0, c.z}, {a.x, b.h, c.z}, {a.x, b.h, a.z}, {a.x, 0, a.z}, wallAlt, ObjectObstacle, 6);
            // Roof slab + 4-sided parapet crown + cornice belt
            addQuad({a.x, b.h, a.z}, {a.x, b.h, c.z}, {c.x, b.h, c.z}, {c.x, b.h, a.z}, roof, ObjectObstacle);
            const float ph = b.h + 0.24f;
            addQuad({a.x - 0.04f, b.h - 0.12f, a.z - 0.04f}, {a.x - 0.04f, ph, a.z - 0.04f}, {c.x + 0.04f, ph, a.z - 0.04f}, {c.x + 0.04f, b.h - 0.12f, a.z - 0.04f}, trim, ObjectObstacle);
            addQuad({c.x + 0.04f, b.h - 0.12f, a.z - 0.04f}, {c.x + 0.04f, ph, a.z - 0.04f}, {c.x + 0.04f, ph, c.z + 0.04f}, {c.x + 0.04f, b.h - 0.12f, c.z + 0.04f}, trim, ObjectObstacle);
            addQuad({c.x + 0.04f, b.h - 0.12f, c.z + 0.04f}, {c.x + 0.04f, ph, c.z + 0.04f}, {a.x - 0.04f, ph, c.z + 0.04f}, {a.x - 0.04f, b.h - 0.12f, c.z + 0.04f}, trim, ObjectObstacle);
            addQuad({a.x - 0.04f, b.h - 0.12f, c.z + 0.04f}, {a.x - 0.04f, ph, c.z + 0.04f}, {a.x - 0.04f, ph, a.z - 0.04f}, {a.x - 0.04f, b.h - 0.12f, a.z - 0.04f}, trim, ObjectObstacle);
            // LW44: 4-corner architectural stone pilasters + mid-facade cornice belt
            for (float cxp : {a.x, c.x}) for (float czp : {a.z, c.z}) {
                addTaperedBone({cxp, 0.0f, czp}, {cxp, b.h + 0.22f, czp}, 0.14f, 0.12f, trim, ObjectObstacle);
            }
            if (b.h > 4.2f) {
                // Stepped-back upper skyscraper tower tier + illuminated curtain-wall & antenna spire
                const float tw = hw * 0.62f, td = hd * 0.62f, thTop = b.h + 1.85f;
                addShadedQuad({b.pos.x - tw, b.h, b.pos.z - td}, {b.pos.x - tw, thTop, b.pos.z - td}, {b.pos.x + tw, thTop, b.pos.z - td}, {b.pos.x + tw, b.h, b.pos.z - td}, wallAlt, ObjectObstacle, 3);
                addShadedQuad({b.pos.x + tw, b.h, b.pos.z - td}, {b.pos.x + tw, thTop, b.pos.z - td}, {b.pos.x + tw, thTop, b.pos.z + td}, {b.pos.x + tw, b.h, b.pos.z + td}, wall, ObjectObstacle, 3);
                addShadedQuad({b.pos.x + tw, b.h, b.pos.z + td}, {b.pos.x + tw, thTop, b.pos.z + td}, {b.pos.x - tw, thTop, b.pos.z + td}, {b.pos.x - tw, b.h, b.pos.z + td}, wallAlt, ObjectObstacle, 3);
                addShadedQuad({b.pos.x - tw, b.h, b.pos.z + td}, {b.pos.x - tw, thTop, b.pos.z + td}, {b.pos.x - tw, thTop, b.pos.z - td}, {b.pos.x - tw, b.h, b.pos.z - td}, wall, ObjectObstacle, 3);
                addQuad({b.pos.x - tw, thTop, b.pos.z - td}, {b.pos.x - tw, thTop, b.pos.z + td}, {b.pos.x + tw, thTop, b.pos.z + td}, {b.pos.x + tw, thTop, b.pos.z - td}, trim, ObjectObstacle);
                // LW45: illuminated penthouse curtain-wall windows on all 4 upper-tier facades
                const Colour pentGlass{165, 225, 250, 255};
                const float wy0 = b.h + 0.28f, wy1 = thTop - 0.26f;
                addQuad({b.pos.x - tw*0.68f, wy0, b.pos.z - td - 0.03f}, {b.pos.x - tw*0.68f, wy1, b.pos.z - td - 0.03f},
                        {b.pos.x + tw*0.68f, wy1, b.pos.z - td - 0.03f}, {b.pos.x + tw*0.68f, wy0, b.pos.z - td - 0.03f}, pentGlass, ObjectObstacle);
                addQuad({b.pos.x - tw*0.68f, wy0, b.pos.z + td + 0.03f}, {b.pos.x - tw*0.68f, wy1, b.pos.z + td + 0.03f},
                        {b.pos.x + tw*0.68f, wy1, b.pos.z + td + 0.03f}, {b.pos.x + tw*0.68f, wy0, b.pos.z + td + 0.03f}, pentGlass, ObjectObstacle);
                addQuad({b.pos.x - tw - 0.03f, wy0, b.pos.z - td*0.68f}, {b.pos.x - tw - 0.03f, wy1, b.pos.z - td*0.68f},
                        {b.pos.x - tw - 0.03f, wy1, b.pos.z + td*0.68f}, {b.pos.x - tw - 0.03f, wy0, b.pos.z + td*0.68f}, pentGlass, ObjectObstacle);
                addQuad({b.pos.x + tw + 0.03f, wy0, b.pos.z - td*0.68f}, {b.pos.x + tw + 0.03f, wy1, b.pos.z - td*0.68f},
                        {b.pos.x + tw + 0.03f, wy1, b.pos.z + td*0.68f}, {b.pos.x + tw + 0.03f, wy0, b.pos.z + td*0.68f}, pentGlass, ObjectObstacle);
                addTaperedBone({b.pos.x, thTop, b.pos.z}, {b.pos.x, thTop + 1.15f, b.pos.z}, 0.06f, 0.02f, trim, ObjectObstacle);
                if (((flicker + (bh & 3u)) & 1u) == 0u)
                    addJoint({b.pos.x, thTop + 1.22f, b.pos.z}, 0.11f, Colour{60, 70, 255, 255}, ObjectObstacle);
            } else {
                const float pw = hw * 0.38f, pd = hd * 0.38f, pmh = b.h + 0.85f;
                addQuad({b.pos.x - pw, b.h, b.pos.z - pd}, {b.pos.x - pw, pmh, b.pos.z - pd}, {b.pos.x + pw, pmh, b.pos.z - pd}, {b.pos.x + pw, b.h, b.pos.z - pd}, wallAlt, ObjectObstacle);
                addQuad({b.pos.x + pw, b.h, b.pos.z - pd}, {b.pos.x + pw, pmh, b.pos.z - pd}, {b.pos.x + pw, pmh, b.pos.z + pd}, {b.pos.x + pw, b.h, b.pos.z + pd}, wall, ObjectObstacle);
                addQuad({b.pos.x + pw, b.h, b.pos.z + pd}, {b.pos.x + pw, pmh, b.pos.z + pd}, {b.pos.x - pw, pmh, b.pos.z + pd}, {b.pos.x - pw, b.h, b.pos.z + pd}, wallAlt, ObjectObstacle);
                addQuad({b.pos.x - pw, b.h, b.pos.z + pd}, {b.pos.x - pw, pmh, b.pos.z + pd}, {b.pos.x - pw, pmh, b.pos.z - pd}, {b.pos.x - pw, b.h, b.pos.z - pd}, wall, ObjectObstacle);
                addQuad({b.pos.x - pw, pmh, b.pos.z - pd}, {b.pos.x - pw, pmh, b.pos.z + pd}, {b.pos.x + pw, pmh, b.pos.z + pd}, {b.pos.x + pw, pmh, b.pos.z - pd}, trim, ObjectObstacle);
                if (((flicker + (bh & 3u)) & 1u) == 0u)
                    addJoint({b.pos.x, pmh + 0.18f, b.pos.z}, 0.11f, Colour{60, 70, 255, 255}, ObjectObstacle);
            }
            // Ground-floor storefronts + doors + neon signboards on -x and -z facades
            const float doorW = std::min(0.85f, hd * 0.28f), doorH = std::min(2.0f, b.h * 0.45f);
            const float dz = (a.z + c.z) * 0.5f, dx = (a.x + c.x) * 0.5f;
            const Colour glassStore{185, 215, 245, 255}, frameCol{42, 38, 46, 255};
            addQuad({a.x - 0.035f, 0.0f, dz - doorW * 1.4f}, {a.x - 0.035f, doorH, dz - doorW * 1.4f},
                    {a.x - 0.035f, doorH, dz + doorW * 1.4f}, {a.x - 0.035f, 0.0f, dz + doorW * 1.4f}, frameCol, ObjectObstacle);
            addQuad({a.x - 0.042f, 0.08f, dz - doorW * 1.2f}, {a.x - 0.042f, doorH - 0.12f, dz - doorW * 1.2f},
                    {a.x - 0.042f, doorH - 0.12f, dz + doorW * 1.2f}, {a.x - 0.042f, 0.08f, dz + doorW * 1.2f}, glassStore, ObjectObstacle);
            // LW46: ground-floor illuminated shopfronts on +x, -z, and +z facades too
            addQuad({c.x + 0.042f, 0.08f, dz - doorW * 1.2f}, {c.x + 0.042f, doorH - 0.12f, dz - doorW * 1.2f},
                    {c.x + 0.042f, doorH - 0.12f, dz + doorW * 1.2f}, {c.x + 0.042f, 0.08f, dz + doorW * 1.2f}, glassStore, ObjectObstacle);
            addQuad({dx - doorW * 1.2f, 0.08f, a.z - 0.042f}, {dx - doorW * 1.2f, doorH - 0.12f, a.z - 0.042f},
                    {dx + doorW * 1.2f, doorH - 0.12f, a.z - 0.042f}, {dx + doorW * 1.2f, 0.08f, a.z - 0.042f}, glassStore, ObjectObstacle);
            addQuad({dx - doorW * 1.2f, 0.08f, c.z + 0.042f}, {dx - doorW * 1.2f, doorH - 0.12f, c.z + 0.042f},
                    {dx + doorW * 1.2f, doorH - 0.12f, c.z + 0.042f}, {dx + doorW * 1.2f, 0.08f, c.z + 0.042f}, glassStore, ObjectObstacle);
            const uint32_t nh = (bh >> 6) & 3u;
            const Colour neon = nh == 0u ? Colour{235, 95, 210, 255}
                              : nh == 1u ? Colour{245, 225, 85, 255}
                              : nh == 2u ? Colour{95, 235, 135, 255}
                                         : Colour{90, 155, 255, 255};
            const float sy = std::min(2.25f, b.h * 0.52f);
            addQuad({a.x - 0.05f, sy, dz - doorW * 1.5f}, {a.x - 0.05f, sy + 0.32f, dz - doorW * 1.5f},
                    {a.x - 0.05f, sy + 0.32f, dz + doorW * 1.5f}, {a.x - 0.05f, sy, dz + doorW * 1.5f}, neon, ObjectObstacle);
            addQuad({dx - doorW * 1.5f, sy, a.z - 0.05f}, {dx - doorW * 1.5f, sy + 0.32f, a.z - 0.05f},
                    {dx + doorW * 1.5f, sy + 0.32f, a.z - 0.05f}, {dx + doorW * 1.5f, sy, a.z - 0.05f}, neon, ObjectObstacle);
            // LW45: 3D projecting entrance canopy/awning + vertical corner neon blade sign
            addQuad({a.x - 0.48f, doorH + 0.04f, dz - doorW * 1.45f}, {a.x - 0.02f, doorH + 0.22f, dz - doorW * 1.45f},
                    {a.x - 0.02f, doorH + 0.22f, dz + doorW * 1.45f}, {a.x - 0.48f, doorH + 0.04f, dz + doorW * 1.45f}, trim, ObjectObstacle);
            if (b.h > 3.6f) {
                const float by0 = 2.4f, by1 = std::min(b.h - 0.4f, 4.8f);
                addQuad({a.x - 0.38f, by0, a.z - 0.04f}, {a.x - 0.38f, by1, a.z - 0.04f},
                        {a.x - 0.06f, by1, a.z - 0.04f}, {a.x - 0.06f, by0, a.z - 0.04f}, neon, ObjectObstacle);
            }
            // LW47: rooftop neon sky-sign billboard on select buildings
            if ((bh & 1u) != 0u) {
                const float ry0 = b.h + 0.28f, ry1 = b.h + 0.76f;
                addQuad({b.pos.x - hw*0.55f, ry0, a.z - 0.05f}, {b.pos.x - hw*0.55f, ry1, a.z - 0.05f},
                        {b.pos.x + hw*0.55f, ry1, a.z - 0.05f}, {b.pos.x + hw*0.55f, ry0, a.z - 0.05f}, neon, ObjectObstacle);
            }
            // Multi-column x multi-floor exterior window grids on all 4 facades (OUTSIDE walls!)
            const int colsZ = std::max(2, std::min(5, (int)(b.d / 1.8f)));
            const int colsX = std::max(2, std::min(5, (int)(b.w / 1.8f)));
            for (float y = 2.65f; y < b.h - 0.75f; y += 1.35f) {
                // LW47: 3D projecting stone floor-sill belt below each window row
                addQuad({a.x - 0.055f, y - 0.09f, a.z - 0.055f}, {a.x - 0.055f, y - 0.01f, a.z - 0.055f},
                        {c.x + 0.055f, y - 0.01f, a.z - 0.055f}, {c.x + 0.055f, y - 0.09f, a.z - 0.055f}, trim, ObjectObstacle);
                addQuad({a.x - 0.055f, y - 0.09f, c.z + 0.055f}, {a.x - 0.055f, y - 0.01f, c.z + 0.055f},
                        {c.x + 0.055f, y - 0.01f, c.z + 0.055f}, {c.x + 0.055f, y - 0.09f, c.z + 0.055f}, trim, ObjectObstacle);
                const int floorIdx = (int)(y / 1.35f);
                for (int col = 0; col < colsZ; ++col) {
                    const float u0 = (col + 0.22f) / colsZ, u1 = (col + 0.78f) / colsZ;
                    const float z0 = a.z + b.d * u0, z1 = a.z + b.d * u1;
                    const unsigned hW = (unsigned)(floorIdx * 13 + col * 7 + (int)b.pos.x * 3 + (int)b.pos.z * 5 + flicker);
                    const Colour winL = (hW & 3u) == 0u ? Colour{52, 48, 58, 255}
                                      : (hW & 1u)       ? Colour{155, 215, 250, 255}
                                                        : Colour{235, 210, 145, 255};
                    addQuad({a.x - 0.035f, y, z0}, {a.x - 0.035f, y + 0.78f, z0},
                            {a.x - 0.035f, y + 0.78f, z1}, {a.x - 0.035f, y, z1}, winL, ObjectObstacle);
                    addQuad({c.x + 0.035f, y, z0}, {c.x + 0.035f, y + 0.78f, z0},
                            {c.x + 0.035f, y + 0.78f, z1}, {c.x + 0.035f, y, z1}, winL, ObjectObstacle);
                }
                for (int col = 0; col < colsX; ++col) {
                    const float u0 = (col + 0.22f) / colsX, u1 = (col + 0.78f) / colsX;
                    const float x0 = a.x + b.w * u0, x1 = a.x + b.w * u1;
                    const unsigned hW = (unsigned)(floorIdx * 17 + col * 11 + (int)b.pos.x * 5 + (int)b.pos.z * 3 + flicker);
                    const Colour winF = (hW & 3u) == 0u ? Colour{52, 48, 58, 255}
                                      : (hW & 1u)       ? Colour{155, 215, 250, 255}
                                                        : Colour{235, 210, 145, 255};
                    addQuad({x0, y, a.z - 0.035f}, {x0, y + 0.78f, a.z - 0.035f},
                            {x1, y + 0.78f, a.z - 0.035f}, {x1, y, a.z - 0.035f}, winF, ObjectObstacle);
                    addQuad({x0, y, c.z + 0.035f}, {x0, y + 0.78f, c.z + 0.035f},
                            {x1, y + 0.78f, c.z + 0.035f}, {x1, y, c.z + 0.035f}, winF, ObjectObstacle);
                }
            }
        }
    }

