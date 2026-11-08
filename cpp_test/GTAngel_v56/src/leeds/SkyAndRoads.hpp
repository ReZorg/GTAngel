    void buildSun() {
        const Vec3 light = normalize3({-0.35f, 0.82f, -0.44f});
        const Vec3 centre = {eye_.x + light.x * 260.0f, eye_.y + light.y * 260.0f, eye_.z + light.z * 260.0f};
        Vec3 forward = {eye_.x - centre.x, eye_.y - centre.y, eye_.z - centre.z};
        float fl = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
        if (fl <= 1e-6f) return;
        forward = {forward.x / fl, forward.y / fl, forward.z / fl};
        Vec3 up = fabsf(forward.y) > 0.99f ? Vec3{1.0f, 0.0f, 0.0f} : Vec3{0.0f, 1.0f, 0.0f};
        Vec3 right = normalize3(cross3(forward, up));
        up = normalize3(cross3(right, forward));
        const float coreR = 13.0f, coronaR = 22.0f;
        const Colour core{255, 244, 252, 255};
        const Colour corona{170, 150, 235, 255};
        for (int i = 0; i < 8; ++i) {
            const float a0 = (float)i * 0.7854f, a1 = (float)(i + 1) * 0.7854f;
            const Vec3 p0 = {centre.x + right.x * coronaR * cosf(a0) + up.x * coronaR * sinf(a0),
                             centre.y + right.y * coronaR * cosf(a0) + up.y * coronaR * sinf(a0),
                             centre.z + right.z * coronaR * cosf(a0) + up.z * coronaR * sinf(a0)};
            const Vec3 p1 = {centre.x + right.x * coronaR * cosf(a1) + up.x * coronaR * sinf(a1),
                             centre.y + right.y * coronaR * cosf(a1) + up.y * coronaR * sinf(a1),
                             centre.z + right.z * coronaR * cosf(a1) + up.z * coronaR * sinf(a1)};
            addTriangle(centre, p0, p1, corona, ObjectSky);
        }
        for (int i = 0; i < 8; ++i) {
            const float a0 = (float)i * 0.7854f, a1 = (float)(i + 1) * 0.7854f;
            const Vec3 p0 = {centre.x + right.x * coreR * cosf(a0) + up.x * coreR * sinf(a0),
                             centre.y + right.y * coreR * cosf(a0) + up.y * coreR * sinf(a0),
                             centre.z + right.z * coreR * cosf(a0) + up.z * coreR * sinf(a0)};
            const Vec3 p1 = {centre.x + right.x * coreR * cosf(a1) + up.x * coreR * sinf(a1),
                             centre.y + right.y * coreR * cosf(a1) + up.y * coreR * sinf(a1),
                             centre.z + right.z * coreR * cosf(a1) + up.z * coreR * sinf(a1)};
            addTriangle(centre, p0, p1, core, ObjectSky);
        }
    }

    void buildMoon() {
        // LW30: pale moon disc opposite the sun light, billboard technique.
        const Vec3 dir = normalize3({0.35f, 0.75f, 0.44f});
        const Vec3 centre = {eye_.x + dir.x * 240.0f, eye_.y + dir.y * 240.0f, eye_.z + dir.z * 240.0f};
        Vec3 forward = {eye_.x - centre.x, eye_.y - centre.y, eye_.z - centre.z};
        float fl = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
        if (fl <= 1e-6f) return;
        forward = {forward.x / fl, forward.y / fl, forward.z / fl};
        Vec3 up = fabsf(forward.y) > 0.99f ? Vec3{1.0f, 0.0f, 0.0f} : Vec3{0.0f, 1.0f, 0.0f};
        Vec3 right = normalize3(cross3(forward, up));
        up = normalize3(cross3(right, forward));
        const float coreR = 9.0f, haloR = 14.5f;
        const Colour core{235, 240, 250, 255};
        const Colour halo{150, 170, 205, 255};
        for (int i = 0; i < 8; ++i) {
            const float a0 = (float)i * 0.7854f, a1 = (float)(i + 1) * 0.7854f;
            Vec3 h0 = {right.x * cosf(a0) + up.x * sinf(a0), right.y * cosf(a0) + up.y * sinf(a0), right.z * cosf(a0) + up.z * sinf(a0)};
            Vec3 h1 = {right.x * cosf(a1) + up.x * sinf(a1), right.y * cosf(a1) + up.y * sinf(a1), right.z * cosf(a1) + up.z * sinf(a1)};
            addTriangle(centre, centre + h0 * coreR, centre + h1 * coreR, core, ObjectSky);
            if (i % 2 == 0)
                addTriangle(centre + h0 * coreR, centre + h0 * haloR, centre + h1 * haloR, halo, ObjectSky);
        }
    }
    void buildGround(const avatar3d::GameWorld& world) {
        const int half = (int)std::ceil(world.worldSize * 0.5f);
        // The 128 m map stays physically bounded; only the local 40 m tile
        // window is rasterized. No low-resolution layer becomes authoritative.
        const int minX=std::max(-half,(int)std::floor(eye_.x)-36);
        const int maxX=std::min( half,(int)std::ceil(eye_.x)+36);
        const int minZ=std::max(-half,(int)std::floor(eye_.z)-36);
        const int maxZ=std::min( half,(int)std::ceil(eye_.z)+36);
        for (int x = minX; x < maxX; x += 2) {
            for (int z = minZ; z < maxZ; z += 2) {
                const Colour colour = groundColour(x >> 1, z >> 1);
                addQuad({(float)x, 0.0f, (float)z},
                        {(float)(x + 2), 0.0f, (float)z},
                        {(float)(x + 2), 0.0f, (float)(z + 2)},
                        {(float)x, 0.0f, (float)(z + 2)}, colour, ObjectGround);
            }
        }
    }

    void buildRoads(const avatar3d::GameWorld& world) {
        // LW34: roads, sidewalks, curbs, and lane markings tiled in 2m chunks
        // within the local 42m camera window so near-plane culling never drops
        // long quads when vertices lie behind the camera.
        for (const auto& road : world.roads) {
            const float minX = std::min(road.x0, road.x1) - road.width * 0.5f;
            const float maxX = std::max(road.x0, road.x1) + road.width * 0.5f;
            const float minZ = std::min(road.z0, road.z1) - road.width * 0.5f;
            const float maxZ = std::max(road.z0, road.z1) + road.width * 0.5f;
            const bool horizontal = std::fabs(road.z1 - road.z0) < 1e-3f;
            const float startS = horizontal ? std::max(minX, eye_.x - 36.0f) : std::max(minZ, eye_.z - 36.0f);
            const float endS   = horizontal ? std::min(maxX, eye_.x + 36.0f) : std::min(maxZ, eye_.z + 36.0f);
            const float step = 2.0f;
            const Colour edgeLine{215, 220, 228, 255};
            const Colour yellowDash{75, 205, 245, 255};
            const Colour curbTop{132, 138, 148, 255};
            for (float s = std::floor(startS / step) * step; s < endS; s += step) {
                const float s0 = std::max(s, startS), s1 = std::min(s + step, endS);
                if (s1 <= s0) continue;
                const int tileIdx = (int)std::floor(s0 / step);
                const Colour asphalt = (tileIdx & 1) ? Colour{42, 40, 38, 255} : Colour{46, 44, 42, 255};
                const Colour walkCol = (tileIdx & 1) ? Colour{114, 118, 126, 255} : Colour{104, 108, 116, 255};
                if (horizontal) {
                    const float cz = (road.z0 + road.z1) * 0.5f;
                    // Asphalt roadbed
                    addQuad({s0, 0.008f, minZ}, {s1, 0.008f, minZ}, {s1, 0.008f, maxZ}, {s0, 0.008f, maxZ}, asphalt, ObjectGround);
                    // Sidewalks + curb strips on both flanks
                    addQuad({s0, 0.014f, minZ - 1.45f}, {s1, 0.014f, minZ - 1.45f}, {s1, 0.014f, minZ - 0.18f}, {s0, 0.014f, minZ - 0.18f}, walkCol, ObjectGround);
                    addQuad({s0, 0.014f, maxZ + 0.18f}, {s1, 0.014f, maxZ + 0.18f}, {s1, 0.014f, maxZ + 1.45f}, {s0, 0.014f, maxZ + 1.45f}, walkCol, ObjectGround);
                    addQuad({s0, 0.018f, minZ - 0.18f}, {s1, 0.018f, minZ - 0.18f}, {s1, 0.018f, minZ - 0.02f}, {s0, 0.018f, minZ - 0.02f}, curbTop, ObjectGround);
                    addQuad({s0, 0.018f, maxZ + 0.02f}, {s1, 0.018f, maxZ + 0.02f}, {s1, 0.018f, maxZ + 0.18f}, {s0, 0.018f, maxZ + 0.18f}, curbTop, ObjectGround);
                    // White edge lines
                    addQuad({s0, 0.015f, minZ + 0.18f}, {s1, 0.015f, minZ + 0.18f}, {s1, 0.015f, minZ + 0.30f}, {s0, 0.015f, minZ + 0.30f}, edgeLine, ObjectGround);
                    addQuad({s0, 0.015f, maxZ - 0.30f}, {s1, 0.015f, maxZ - 0.30f}, {s1, 0.015f, maxZ - 0.18f}, {s0, 0.015f, maxZ - 0.18f}, edgeLine, ObjectGround);
                    // LW44: Zebra crosswalk stripes at the central intersection approaches
                    if (std::fabs(s0 - 3.0f) < 0.9f || std::fabs(s0 + 5.0f) < 0.9f) {
                        for (float zc = minZ + 0.45f; zc < maxZ - 0.45f; zc += 0.55f) {
                            addQuad({s0 + 0.15f, 0.017f, zc}, {s1 - 0.15f, 0.017f, zc},
                                    {s1 - 0.15f, 0.017f, zc + 0.28f}, {s0 + 0.15f, 0.017f, zc + 0.28f}, edgeLine, ObjectGround);
                        }
                    } else if ((tileIdx & 1) == 0) {
                        addQuad({s0 + 0.25f, 0.016f, cz - 0.08f}, {s1 - 0.25f, 0.016f, cz - 0.08f},
                                {s1 - 0.25f, 0.016f, cz + 0.08f}, {s0 + 0.25f, 0.016f, cz + 0.08f}, yellowDash, ObjectGround);
                    }
                } else {
                    const float cx = (road.x0 + road.x1) * 0.5f;
                    addQuad({minX, 0.008f, s0}, {maxX, 0.008f, s0}, {maxX, 0.008f, s1}, {minX, 0.008f, s1}, asphalt, ObjectGround);
                    addQuad({minX - 1.45f, 0.014f, s0}, {minX - 0.18f, 0.014f, s0}, {minX - 0.18f, 0.014f, s1}, {minX - 1.45f, 0.014f, s1}, walkCol, ObjectGround);
                    addQuad({maxX + 0.18f, 0.014f, s0}, {maxX + 1.45f, 0.014f, s0}, {maxX + 1.45f, 0.014f, s1}, {maxX + 0.18f, 0.014f, s1}, walkCol, ObjectGround);
                    addQuad({minX - 0.18f, 0.018f, s0}, {minX - 0.02f, 0.018f, s0}, {minX - 0.02f, 0.018f, s1}, {minX - 0.18f, 0.018f, s1}, curbTop, ObjectGround);
                    addQuad({maxX + 0.02f, 0.018f, s0}, {maxX + 0.18f, 0.018f, s0}, {maxX + 0.18f, 0.018f, s1}, {maxX + 0.02f, 0.018f, s1}, curbTop, ObjectGround);
                    addQuad({minX + 0.18f, 0.015f, s0}, {minX + 0.30f, 0.015f, s0}, {minX + 0.30f, 0.015f, s1}, {minX + 0.18f, 0.015f, s1}, edgeLine, ObjectGround);
                    addQuad({maxX - 0.30f, 0.015f, s0}, {maxX - 0.18f, 0.015f, s0}, {maxX - 0.18f, 0.015f, s1}, {maxX - 0.30f, 0.015f, s1}, edgeLine, ObjectGround);
                    if (std::fabs(s0 - 3.0f) < 0.9f || std::fabs(s0 + 5.0f) < 0.9f) {
                        for (float xc = minX + 0.45f; xc < maxX - 0.45f; xc += 0.55f) {
                            addQuad({xc, 0.017f, s0 + 0.15f}, {xc + 0.28f, 0.017f, s0 + 0.15f},
                                    {xc + 0.28f, 0.017f, s1 - 0.15f}, {xc, 0.017f, s1 - 0.15f}, edgeLine, ObjectGround);
                        }
                    } else if ((tileIdx & 1) == 0) {
                        addQuad({cx - 0.08f, 0.016f, s0 + 0.25f}, {cx + 0.08f, 0.016f, s0 + 0.25f},
                                {cx + 0.08f, 0.016f, s1 - 0.25f}, {cx - 0.08f, 0.016f, s1 - 0.25f}, yellowDash, ObjectGround);
                    }
                }
            }
        }
    }

    void addShadedQuad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Colour base, uint16_t objectId,
                       int bands, bool ramp = true) {
        // LW33: brightness-ramped, noise-varied strips. Corners are ordered
        // [bottom0, top0, top1, bottom1]; strips lerp along the vertical edges.
        const uint32_t seed = (uint32_t)(int)(a.x * 83492791.0f) ^ (uint32_t)(int)(a.z * 19349663.0f);
        for (int i = 0; i < bands; ++i) {
            const float t0 = (float)i / bands, t1 = (float)(i + 1) / bands;
            Vec3 q0{a.x+(b.x-a.x)*t0, a.y+(b.y-a.y)*t0, a.z+(b.z-a.z)*t0};
            Vec3 q1{a.x+(b.x-a.x)*t1, a.y+(b.y-a.y)*t1, a.z+(b.z-a.z)*t1};
            Vec3 q2{d.x+(c.x-d.x)*t1, d.y+(c.y-d.y)*t1, d.z+(c.z-d.z)*t1};
            Vec3 q3{d.x+(c.x-d.x)*t0, d.y+(c.y-d.y)*t0, d.z+(c.z-d.z)*t0};
            const float m = (ramp ? 0.76f + 0.34f * t0 : 1.0f) *
                            (0.93f + 0.13f * (((uint32_t)(i * 2654435761u) ^ seed) >> 8 & 255u) / 255.0f);
            addQuad(q0, q1, q2, q3,
                    Colour{(uint8_t)std::min(255, (int)(base.b * m)),
                           (uint8_t)std::min(255, (int)(base.g * m)),
                           (uint8_t)std::min(255, (int)(base.r * m)), 255}, objectId);
        }
    }
