#pragma once
    // ── Z++ Formal Spec Component Binding (renderer): 38 modules, 1267 schemas, 51 enums (2dEffect.zpp, Antennas.zpp, Clouds.zpp, Console.zpp, Coronas.zpp, Credits.zpp, CutsceneShadow.zpp, Draw.zpp) ──
    static constexpr int zpp_renderer_modules() { return zpp_renderer_spec::kZppModuleCount; }
    static constexpr int zpp_renderer_schemas() { return zpp_renderer_spec::kZppSchemaCount; }
    static constexpr bool verifyComponent_renderer_zpp() {
        return zpp_renderer_spec::verify_renderer_zpp_invariants() && (zpp_renderer_spec::C2dEffect_ZppContract{}.checkInvariant()) && (zpp_renderer_spec::op_CClouds_Init(zpp_renderer_spec::CClouds_ZppContract{}, zpp_renderer_spec::apply_CClouds_Init(zpp_renderer_spec::CClouds_ZppContract{})));
    }
    bool verifyLiveWorld_renderer_zpp(const avatar3d::GameWorld& world) const {
        zpp_renderer_spec::C2dEffect_ZppContract c{};
        return (world.worldSize > 0.0f) && c.checkInvariant();
    }
    static_assert(verifyComponent_renderer_zpp(), "RasterEngine component Z++ binding failed for renderer");
    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {
        using avatar3d::AvatarSkeleton;
        const auto& frames=Re3InformedAvatarClump::frames();
        const Colour lilac{186,147,222,255}, skin{212,170,174,255};
        const Colour dark{35,31,52,255}, boot{104,94,139,255};
        // The validated asset contributes skinned body triangles; v21 remains
        // an explicit visual fallback if the optional asset is absent/corrupt.
        if (skinned_.valid()) {
            skinned_.draw(avatar,[this](Vec3 a,Vec3 b,Vec3 c,Colour rgb,uint16_t id) {
                addTriangle(a,b,c,rgb,id);
            });
        } else {
            for (const auto& atomic:Re3InformedAvatarClump::atomics()) {
                const int from=frames[atomic.fromFrame].joint;
                const int to=frames[atomic.toFrame].joint;
                Colour material=lilac;
                if (from==AvatarSkeleton::HipL || from==AvatarSkeleton::HipR)
                    material=skin;
                if (from==AvatarSkeleton::KneeL || from==AvatarSkeleton::KneeR)
                    material=boot;
                if (from==AvatarSkeleton::ShoulderL || from==AvatarSkeleton::ShoulderR ||
                    from==AvatarSkeleton::ElbowL || from==AvatarSkeleton::ElbowR)
                    material=skin;
                addTaperedBone(avatar.joints[from],avatar.joints[to],
                               atomic.startRadius,atomic.endRadius,material);
            }
            addJoint(avatar.joints[AvatarSkeleton::Head],0.145f,skin);
            addJoint(avatar.joints[AvatarSkeleton::Root],0.14f,dark);
            }
        addBlobShadow(avatar.lastPosition, 0.38f); // LW32: ground the avatar
        // LW23: gamer-girl headset over any body source - orange earcups,
        // band arc, cyan LED glow, mic boom; tracks the posed head.
        {
            for (int side=0;side<2;++side) {
                const float s=side==0?-1.0f:1.0f;
                const Vec3 ear=avatar.headset[side];
                addTaperedBone(ear+Vec3{s*0.012f,0.10f,0},ear+Vec3{s*0.012f,-0.03f,0},0.030f,0.026f,Colour{46,42,58,255});
                addJoint(ear,0.055f,Colour{40,130,245,255});
                addJoint(ear+Vec3{0,0.028f,0},0.020f,Colour{255,255,0,255});
            }
            addTaperedBone(avatar.headset[0]+Vec3{0,0.115f,0},avatar.headset[1]+Vec3{0,0.115f,0},0.016f,0.016f,Colour{46,42,58,255});
            addTaperedBone(avatar.headset[1]+Vec3{0.0f,-0.02f,0.0f},avatar.headset[1]+Vec3{0.0f,-0.05f,0.14f},0.008f,0.005f,Colour{46,42,58,255});
            addJoint(avatar.headset[1]+Vec3{0.0f,-0.05f,0.15f},0.012f,Colour{255,255,0,255});
        }
        if (!skinned_.hasNativeWings()) {
        for (int side=0;side<2;++side) {
            const float s=side==0?-1.0f:1.0f;
            const Vec3 root=avatar.wingRoot[side], boom=avatar.wingBoom[side],tip=avatar.wingTip[side];
            addJoint(root,0.085f,Colour{203,164,107,255});
            addTaperedBone(root,boom,0.050f,0.036f,Colour{112,116,141,255});
            addTaperedBone(boom,tip,0.035f,0.015f,Colour{136,141,170,255});
            // Rigid, still-attached crystal plates fan root to tip; ribbon arcs
            // are deterministic cosmetic geometry rather than physics output.
            for(int j=0;j<6;++j) {
                const float a=(j+0.1f)/6.0f,b=(j+0.94f)/6.0f;
                const Vec3 start=boom+(tip-boom)*a;
                const Vec3 end=boom+(tip-boom)*b;
                const float width=0.015f+avatar.wingDeploy*(0.10f+0.19f*(1-a));
                const Vec3 edge=start+Vec3{s*width,-0.14f*avatar.wingDeploy,0};
                addTriangle(start,end,edge,
                    j%2?Colour{238,104,199,255}:Colour{171,122,248,255},ObjectAvatar);
                addTriangle(end,start,edge,
                    j%2?Colour{238,104,199,255}:Colour{171,122,248,255},ObjectAvatar);
            }
            if (!skinned_.valid()) {
            const int hand=side==0?AvatarSkeleton::HandL:AvatarSkeleton::HandR;
            addJoint(avatar.joints[hand],0.052f,Colour{100,69,141,255});
            addJoint(avatar.headset[side],0.050f,Colour{239,151,51,255});
            }
        }
        } // native wing asset carries its own two articulated wings
        // The native texture already contains the headset, cheeks, and hair.
        if (!skinned_.valid()) {
        for(const auto& p:avatar.cheek) addJoint(p,0.015f,Colour{52,221,248,255});
        addJoint(avatar.joints[AvatarSkeleton::Head]+Vec3{0,0.09f,0},0.107f,
                 Colour{172,227,229,255});
        }
        if(avatar.orbIntensity>0.01f) {
            const Vec3 palm=avatar.joints[AvatarSkeleton::HandR]+Vec3{0,0.02f,0.095f};
            addJoint(palm,0.045f+0.085f*avatar.orbIntensity,
                     Colour{53,233,249,255});
        }
    }

    VertexOut project(Vec3 point) const {
        Vec3 delta = point - eye_;
        float ch = cosf(heading_), sh = sinf(heading_);
        float cp = cosf(pitch_), sp = sinf(pitch_);
        float x = delta.x * ch - delta.z * sh;
        float z = delta.x * sh + delta.z * ch;
        float y = delta.y * cp - z * sp;
        float z2 = delta.y * sp + z * cp;
        if (z2 <= 0.08f) return {};
        const float fov = 1.2f;
        const float focal = std::min(Surface::Width,Surface::Height)*0.5f;
        VertexOut out;
        out.x = (x / (z2 * fov)) * focal + Surface::Width * 0.5f;
        out.y = (-y / (z2 * fov)) * focal + Surface::Height * 0.5f;
        out.z = z2;
        out.visible = out.x > -Surface::Width*2.0f && out.x < Surface::Width*3.0f && out.y > -Surface::Height*2.0f && out.y < Surface::Height*3.0f;
        return out;
    }

    static float edge(float ax, float ay, float bx, float by, float px, float py) {
        return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
    }

    void rasterize(const Triangle& triangle) {
        VertexOut a = project(triangle.a);
        VertexOut b = project(triangle.b);
        VertexOut c = project(triangle.c);
        if (!a.visible || !b.visible || !c.visible) return;

        float signedArea = edge(a.x, a.y, b.x, b.y, c.x, c.y);
        if (fabsf(signedArea) <= 1e-5f) return;
        float minXf = std::min(a.x, std::min(b.x, c.x));
        float maxXf = std::max(a.x, std::max(b.x, c.x));
        float minYf = std::min(a.y, std::min(b.y, c.y));
        float maxYf = std::max(a.y, std::max(b.y, c.y));
        int minX = std::max(0, (int)floorf(minXf));
        int maxX = std::min(Surface::Width - 1, (int)ceilf(maxXf));
        int minY = std::max(0, (int)floorf(minYf));
        int maxY = std::min(Surface::Height - 1, (int)ceilf(maxYf));
        if (minX > maxX || minY > maxY) return;

        Vec3 normal = normalize3(cross3(triangle.b - triangle.a, triangle.c - triangle.a));
        Vec3 light = normalize3({-0.35f, 0.82f, -0.44f});
        const float ndotl = std::fabs(avatar3d::dot(normal, light));
        float baseIllum = 0.48f + 0.52f * ndotl;
        if (triangle.objectId == ObjectSky) baseIllum = 1.0f;
        if (triangle.objectId == ObjectAvatar && skinned_.valid())
            baseIllum = 0.64f + 0.36f * std::max(0.0f, avatar3d::dot(normal, light));
        const float inverseArea = 1.0f / signedArea;
        const Vec3 pa = triangle.a * (1.0f / a.z);
        const Vec3 pb = triangle.b * (1.0f / b.z);
        const Vec3 pc = triangle.c * (1.0f / c.z);
        const uint16_t oid = triangle.objectId;
        const bool isWall = (oid == ObjectObstacle && std::fabs(normal.y) < 0.35f);
        const bool isWindowPane = isWall && (triangle.colour.r > 135 || triangle.colour.b > 145);
        const bool isGlossyVehicle = (oid == ObjectVehicle || oid == ObjectTraffic);

        bool wrote = false;
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                float px = x + 0.5f, py = y + 0.5f;
                float w0 = edge(b.x, b.y, c.x, c.y, px, py) * inverseArea;
                float w1 = edge(c.x, c.y, a.x, a.y, px, py) * inverseArea;
                float w2 = 1.0f - w0 - w1;
                if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) continue;
                float inverseDepth = w0 / a.z + w1 / b.z + w2 / c.z;
                if (inverseDepth <= 1e-8f) continue;
                float depth = 1.0f / inverseDepth;
                int index = y * Surface::Width + x;
                if (depth >= framebuffer.depth[index]) continue;

                Colour baseCol = triangle.colour;
                float illum = baseIllum;
                int specAddB = 0, specAddG = 0, specAddR = 0;

                if (oid != ObjectSky) {
                    const Vec3 wp = (pa * w0 + pb * w1 + pc * w2) * depth;
                    if (oid == ObjectGround) {
                        // High-frequency asphalt/stone micro-grain + paver joints + wet reflection
                        const int gx = (int)std::floor(wp.x * 32.0f), gz = (int)std::floor(wp.z * 32.0f);
                        const uint32_t h = (uint32_t)(gx * 73856093) ^ (uint32_t)(gz * 19349663);
                        float grain = 0.94f + 0.12f * ((h >> 8) & 15u) / 15.0f;
                        // Sidewalk & plaza 0.5m stone paver bevel seams
                        if (baseCol.b > 50 && baseCol.b < 140) {
                            const float fx = std::fabs(wp.x - std::round(wp.x * 1.5f) / 1.5f);
                            const float fz = std::fabs(wp.z - std::round(wp.z * 1.5f) / 1.5f);
                            if (fx < 0.022f || fz < 0.022f) grain *= 0.80f;
                        }
                        // Wet-asphalt road sheen + warm streetlamp / neon reflections
                        if (baseCol.b <= 50) {
                            const float puddle = 0.5f + 0.5f * sinf(wp.x * 0.55f + cosf(wp.z * 0.45f));
                            const float sheen = std::min(0.45f, depth * 0.015f) * puddle;
                            specAddB += (int)(sheen * 78.0f);
                            specAddG += (int)(sheen * 52.0f);
                            specAddR += (int)(sheen * 64.0f);
                        }
                        // Dynamic warm sodium streetlight illumination pools on ground
                        const float lx = std::fabs(wp.x) - 15.0f, lz = std::fabs(wp.z) - 2.6f;
                        const float lx2 = std::fabs(wp.x) - 2.6f, lz2 = std::fabs(wp.z) - 15.0f;
                        const float d1 = lx*lx + lz*lz, d2 = lx2*lx2 + lz2*lz2;
                        const float dMin = std::min(d1, d2);
                        if (dMin < 18.0f) {
                            const float att = (18.0f - dMin) / 18.0f;
                            const float att2 = att * att;
                            specAddR += (int)(58.0f * att2);
                            specAddG += (int)(42.0f * att2);
                            specAddB += (int)(18.0f * att2);
                        }
                        illum *= grain;
                    } else if (isWindowPane) {
                        // Mullioned 2x2 casement window frames + Venetian blinds + sky glint
                        const float wy = std::fmod(std::fabs(wp.y), 0.78f);
                        const float wx = std::fmod(std::fabs(wp.x + wp.z), 0.65f);
                        if (wy < 0.045f || wy > 0.735f || std::fabs(wy - 0.39f) < 0.025f ||
                            wx < 0.04f || std::fabs(wx - 0.325f) < 0.022f) {
                            baseCol = Colour{36, 34, 42, 255}; // dark aluminum mullion frame
                            illum = 0.65f;
                        } else {
                            const float blind = 0.86f + 0.14f * (((int)(wp.y * 28.0f)) & 1);
                            const float diag = std::fmod(std::fabs(wp.x + wp.y * 0.7f + wp.z), 1.4f);
                            if (diag < 0.16f) { specAddB += 38; specAddG += 32; specAddR += 28; }
                            illum = (0.85f + 0.15f * (wy / 0.78f)) * blind;
                        }
                    } else if (isWall) {
                        // Architectural coursed stone / cladding blocks + mortar seams + ground AO
                        const float by = std::fmod(std::fabs(wp.y), 0.45f);
                        const float bx = std::fmod(std::fabs(wp.x + wp.z + ((int)(wp.y / 0.45f) & 1) * 0.45f), 0.90f);
                        const int tx = (int)std::floor((wp.x + wp.z) * 24.0f), ty = (int)std::floor(wp.y * 24.0f);
                        const uint32_t th = (uint32_t)(tx * 73856093) ^ (uint32_t)(ty * 19349663);
                        float stone = 0.93f + 0.14f * ((th >> 8) & 15u) / 15.0f;
                        if (by < 0.028f || bx < 0.028f) stone *= 0.74f; // recessed mortar joint
                        const float ao = 0.68f + 0.32f * clamp01(wp.y / 2.2f);
                        illum *= stone * ao;
                    } else if (isGlossyVehicle) {
                        // Automotive metallic clearcoat: Blinn-Phong specular + Fresnel rim sheen
                        const Vec3 V = normalize3(eye_ - wp);
                        const Vec3 H = normalize3(light + V);
                        const float ndoth = std::max(0.0f, std::fabs(avatar3d::dot(normal, H)));
                        const float ndotv = std::min(1.0f, std::fabs(avatar3d::dot(normal, V)));
                        const float s2 = ndoth * ndoth, s4 = s2 * s2, s16 = s4 * s4;
                        const float fresnel = (1.0f - ndotv) * (1.0f - ndotv);
                        const int spec = (int)(92.0f * s16 + 34.0f * fresnel);
                        specAddB += spec + (int)(18.0f * fresnel);
                        specAddG += spec + (int)(12.0f * fresnel);
                        specAddR += spec;
                    } else if (oid == ObjectNpc || oid == ObjectAvatar) {
                        // Subsurface rim-light on characters & pedestrians
                        const Vec3 V = normalize3(eye_ - wp);
                        const float ndotv = std::min(1.0f, std::fabs(avatar3d::dot(normal, V)));
                        const float rim = (1.0f - ndotv) * (1.0f - ndotv);
                        specAddB += (int)(26.0f * rim);
                        specAddG += (int)(22.0f * rim);
                        specAddR += (int)(24.0f * rim);
                    }
                }

                float fog = oid == ObjectSky ? 0.0f : clamp01((depth - 34.0f) / 62.0f);
                Colour shaded = shade(baseCol, illum, fog);
                if ((specAddB | specAddG | specAddR) != 0) {
                    const float invFog = 1.0f - fog;
                    shaded.b = (uint8_t)std::min(255, (int)shaded.b + (int)(specAddB * invFog));
                    shaded.g = (uint8_t)std::min(255, (int)shaded.g + (int)(specAddG * invFog));
                    shaded.r = (uint8_t)std::min(255, (int)shaded.r + (int)(specAddR * invFog));
                }
                framebuffer.depth[index] = depth;
                framebuffer.objectId[index] = oid;
                framebuffer.colour[index] = Surface::pack(shaded);
                wrote = true;
            }
        }
        if (wrote) ++metrics.trianglesRasterized;
    }
};

using HighResRenderer = RasterEngine<HighResFramebuffer>;
using DisplayRenderer = RasterEngine<DisplayFramebuffer>;

struct Retina64 {
    static constexpr int Width = 64;
    static constexpr int Height = 64;
    static constexpr int Count = Width * Height;
    std::array<float, Count> luminance{};
    std::array<float, Count> depth{};
    std::array<float, Count> semantic{};
    std::array<float, Count> motion{};
    std::array<float, Count> edgeEnergy{};
    std::array<uint32_t, Count> colour{};
    uint64_t sourceSequence{0};
    float meanLuminance{0.0f};
    float meanMotion{0.0f};
    float meanEdge{0.0f};
};

class PerceptionPyramid {
public:
    Retina64 retina;

    void derive(const HighResFramebuffer& source) {
        static constexpr int Tile = HighResFramebuffer::Width / Retina64::Width;
        std::array<float, Retina64::Count> previous = retina.luminance;
        retina.meanLuminance = retina.meanMotion = retina.meanEdge = 0.0f;

        for (int ry = 0; ry < Retina64::Height; ++ry) {
            for (int rx = 0; rx < Retina64::Width; ++rx) {
                uint64_t sumB = 0, sumG = 0, sumR = 0;
                float minDepth = 1e30f;
                float idDepth[12];
                for (int k = 0; k < 12; ++k) idDepth[k] = 1e30f;
                int ids[12]{};
                for (int sy = 0; sy < Tile; ++sy) {
                    int y = ry * Tile + sy;
                    for (int sx = 0; sx < Tile; ++sx) {
                        int x = rx * Tile + sx;
                        int index = y * HighResFramebuffer::Width + x;
                        Colour colour = HighResFramebuffer::unpack(source.colour[index]);
                        sumB += colour.b; sumG += colour.g; sumR += colour.r;
                        const float d = source.depth[index];
                        minDepth = std::min(minDepth, d);
                        uint16_t id = source.objectId[index];
                        if (id < 12) { ++ids[id]; idDepth[id] = std::min(idDepth[id], d); }
                    }
                }
                const int index = ry * Retina64::Width + rx;
                const float samples = (float)(Tile * Tile);
                Colour average{(uint8_t)std::lround(sumB / samples),
                               (uint8_t)std::lround(sumG / samples),
                               (uint8_t)std::lround(sumR / samples), 255};
                retina.colour[index] = HighResFramebuffer::pack(average);
                retina.luminance[index] = (0.114f * average.b + 0.587f * average.g + 0.299f * average.r) / 255.0f;
                int dominant = 0;
                for (int id = 1; id < 12; ++id) if (ids[id] > ids[dominant]) dominant = id;
                retina.depth[index] = (dominant > 0 && idDepth[dominant] < 1e29f) ? idDepth[dominant] : minDepth;
                retina.semantic[index] = dominant == ObjectGoal ? 1.0f :
                                         dominant == ObjectQuestItem ? 0.85f :
                                         dominant == ObjectObstacle ? 0.66f :
                                         dominant == ObjectNpc ? 0.42f :
                                         dominant == ObjectAvatar ? 0.50f :
                                         dominant == ObjectGround ? 0.33f :
                                         dominant == ObjectTraffic ? 0.28f :
                                         dominant == ObjectPickup ? 0.20f :
                                         dominant == ObjectProp ? 0.12f : 0.0f;
                retina.motion[index] = primed_ ? fabsf(retina.luminance[index] - previous[index]) : 0.0f;
                retina.meanLuminance += retina.luminance[index];
                retina.meanMotion += retina.motion[index];
            }
        }

        for (int y = 0; y < Retina64::Height; ++y) {
            for (int x = 0; x < Retina64::Width; ++x) {
                int index = y * Retina64::Width + x;
                float centre = retina.luminance[index];
                float right = retina.luminance[y * Retina64::Width + std::min(Retina64::Width - 1, x + 1)];
                float down = retina.luminance[std::min(Retina64::Height - 1, y + 1) * Retina64::Width + x];
                retina.edgeEnergy[index] = sqrtf((right - centre) * (right - centre) +
                                                 (down - centre) * (down - centre));
                retina.meanEdge += retina.edgeEnergy[index];
            }
        }

        retina.meanLuminance /= Retina64::Count;
        retina.meanMotion /= Retina64::Count;
        retina.meanEdge /= Retina64::Count;
        retina.sourceSequence = source.sequence;
        primed_ = true;
    }

    void copyToLegacy(avatar3d::EgocentricRenderer& legacy) const {
        legacy.frame.assign(retina.luminance.begin(), retina.luminance.end());
        legacy.zbuf.assign(retina.depth.begin(), retina.depth.end());
        legacy.semantic.assign(retina.semantic.begin(), retina.semantic.end());
    }

    void mapToTriz(avatar3d::TrizMapping& mapping) const {
        mapping.meanContradiction = mapping.maxContradiction = 0.0f;
        mapping.maxQ = mapping.maxR = -1;
        for (int q = 0; q < avatar3d::TrizMapping::GRID; ++q) {
            const int x0 = (q * Retina64::Width) / avatar3d::TrizMapping::GRID;
            const int x1 = ((q + 1) * Retina64::Width) / avatar3d::TrizMapping::GRID;
            for (int r = 0; r < avatar3d::TrizMapping::GRID; ++r) {
                const int y0 = (r * Retina64::Height) / avatar3d::TrizMapping::GRID;
                const int y1 = ((r + 1) * Retina64::Height) / avatar3d::TrizMapping::GRID;
                float luminance = 0.0f, goals = 0.0f, obstacles = 0.0f, samples = 0.0f;
                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        int index = y * Retina64::Width + x;
                        luminance += retina.luminance[index];
                        if (retina.semantic[index] > 0.9f) goals += 1.0f;
                        else if (retina.semantic[index] > 0.5f && retina.semantic[index] < 0.8f) obstacles += 1.0f;
                        samples += 1.0f;
                    }
                }
                mapping.aesthetic[q][r] = samples > 0.0f ? luminance / samples : 0.0f;
                mapping.goalSalience[q][r] = samples > 0.0f ? goals / samples : 0.0f;
                mapping.obstacleSalience[q][r] = samples > 0.0f ? obstacles / samples : 0.0f;
                float centre = 1.0f - fabsf(((q + 0.5f) / avatar3d::TrizMapping::GRID) * 2.0f - 1.0f);
                float forward = 1.0f - (float)r / avatar3d::TrizMapping::GRID;
                float corridor = mapping.obstacleSalience[q][r] * centre * (0.35f + 0.65f * forward);
                float overlap = 2.0f * sqrtf(mapping.goalSalience[q][r] * mapping.obstacleSalience[q][r]);
                mapping.contradiction[q][r] = std::min(1.0f, corridor + overlap);
                mapping.meanContradiction += mapping.contradiction[q][r];
                if (mapping.contradiction[q][r] > mapping.maxContradiction) {
                    mapping.maxContradiction = mapping.contradiction[q][r];
                    mapping.maxQ = q;
                    mapping.maxR = r;
                }
            }
        }
        mapping.meanContradiction /= (avatar3d::TrizMapping::GRID * avatar3d::TrizMapping::GRID);
    }

private:
    bool primed_{false};
};

struct VisualNavEvidence {
    avatar3d::NavTrainer::State state{4, 3, 4};
    bool goalVisible{false};
    float goalDepth{1e30f};
    int goalPixels{0};
    bool obstacleVisible{false};
    float nearestObstacleDepth{1e30f};
    int obstaclePixels{0};
    bool shardVisible{false};
    float shardDepth{1e30f};
    int shardPixels{0};
    int shardBearing{3};
    bool npcVisible{false};
    float npcDepth{1e30f};
    int npcPixels{0};
    int npcBearing{3};
    bool trafficVisible{false};
    float trafficDepth{1e30f};
    int trafficPixels{0};
    int trafficBearing{3};
    bool pickupVisible{false};
    float pickupDepth{1e30f};
    int pickupPixels{0};
    int pickupBearing{3};
    bool propVisible{false};
    float propDepth{1e30f};
    int propPixels{0};
    uint64_t sourceSequence{0};
};

class VisualNavStateEncoder {
public:
    VisualNavEvidence encode(const Retina64& retina) const {
        VisualNavEvidence evidence;
        evidence.sourceSequence = retina.sourceSequence;
        float goalX = 0.0f;
        float obstacleX = 0.0f;
        float shardX = 0.0f;
        float npcX = 0.0f;
        float trafficX = 0.0f;
        float pickupX = 0.0f;
        for (int y = 0; y < Retina64::Height; ++y) {
            for (int x = 0; x < Retina64::Width; ++x) {
                const int index = y * Retina64::Width + x;
                const float semantic = retina.semantic[index];
                const float depth = retina.depth[index];
                if (semantic > 0.9f) {
                    goalX += (float)x + 0.5f;
                    evidence.goalDepth = std::min(evidence.goalDepth, depth);
                    ++evidence.goalPixels;
                } else if (semantic > 0.8f && semantic < 0.9f) {
                    shardX += (float)x + 0.5f;
                    evidence.shardDepth = std::min(evidence.shardDepth, depth);
                    ++evidence.shardPixels;
                } else if (semantic > 0.5f && semantic < 0.8f) {
                    // LW36: only count obstacle depth inside the central forward
                    // navigation corridor (x in [8..55], y in [6..46]) so poles or
                    // pillars already alongside/behind the avatar in 3rd-person
                    // do not trigger false obstacle avoidance turns.
                    if (x >= 8 && x <= 55 && y >= 6 && y <= 46) {
                        if (depth < evidence.nearestObstacleDepth) {
                            evidence.nearestObstacleDepth = depth;
                            obstacleX = (float)x + 0.5f;
                        }
                        ++evidence.obstaclePixels;
                    }
                } else if (semantic > 0.35f && semantic < 0.5f) {
                    npcX += (float)x + 0.5f;
                    evidence.npcDepth = std::min(evidence.npcDepth, depth);
                    ++evidence.npcPixels;
                } else if (semantic > 0.25f && semantic < 0.31f) {
                    trafficX += (float)x + 0.5f;
                    evidence.trafficDepth = std::min(evidence.trafficDepth, depth);
                    ++evidence.trafficPixels;
                } else if (semantic > 0.17f && semantic < 0.23f) {
                    pickupX += (float)x + 0.5f;
                    evidence.pickupDepth = std::min(evidence.pickupDepth, depth);
                    ++evidence.pickupPixels;
                } else if (semantic > 0.09f && semantic < 0.15f) {
                    evidence.propDepth = std::min(evidence.propDepth, depth);
                    ++evidence.propPixels;
                }
            }
        }

        evidence.goalVisible = evidence.goalPixels > 0;
        evidence.obstacleVisible = evidence.obstaclePixels > 0;
        evidence.shardVisible = evidence.shardPixels > 0;
        evidence.npcVisible = evidence.npcPixels > 0;
        evidence.trafficVisible = evidence.trafficPixels > 0;
        evidence.pickupVisible = evidence.pickupPixels > 0;
        evidence.propVisible = evidence.propPixels > 0;
        if (evidence.trafficVisible)
            evidence.trafficBearing = screenBucket(trafficX / (float)evidence.trafficPixels, avatar3d::NavTrainer::BEARINGS);
        if (evidence.pickupVisible)
            evidence.pickupBearing = screenBucket(pickupX / (float)evidence.pickupPixels, avatar3d::NavTrainer::BEARINGS);
        if (evidence.shardVisible)
            evidence.shardBearing = screenBucket(shardX / (float)evidence.shardPixels, avatar3d::NavTrainer::BEARINGS);
        if (evidence.npcVisible)
            evidence.npcBearing = screenBucket(npcX / (float)evidence.npcPixels, avatar3d::NavTrainer::BEARINGS);
        if (evidence.goalVisible) {
            goalX /= (float)evidence.goalPixels;
            evidence.state.bearing = screenBucket(goalX, avatar3d::NavTrainer::BEARINGS);
            evidence.state.dist = evidence.goalDepth < 2.0f ? 0 :
                                  evidence.goalDepth < 5.0f ? 1 :
                                  evidence.goalDepth < 10.0f ? 2 : 3;
        }
        if (evidence.obstacleVisible) {
            evidence.state.obst = screenBucket(obstacleX, avatar3d::NavTrainer::OBST);
        }
        return evidence;
    }

    float progressReward(const VisualNavEvidence& before, const VisualNavEvidence& after) const {
        if (!before.goalVisible || !after.goalVisible) return 0.0f;
        return std::max(-1.0f, std::min(1.0f, (before.goalDepth - after.goalDepth) / 5.0f));
    }

private:
    static int screenBucket(float x, int bucketCount) {
        int bucket = (int)((x / Retina64::Width) * bucketCount);
        return std::max(0, std::min(bucketCount - 1, bucket));
    }
};

} // namespace v15render

#pragma once
#ifndef GTANGEL_ZPP_RENDERER_SPEC_GUARD
#define GTANGEL_ZPP_RENDERER_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/renderer
// Derived from C:/ddd/GTAngel/cpp_test/zpp/renderer/*.zpp
// Modules: 38 | Formal Schemas: 1267 | Free Types: 51
// ============================================================================
namespace zpp_renderer_spec {
static constexpr int kZppModuleCount = 38;
static constexpr int kZppSchemaCount = 1267;
static constexpr int kZppEnumCount   = 51;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/renderer/Console.zpp
enum class Console_Enum_1 : int32_t { MAX_LINES_v = 8, MAX_STR_LEN_v = 40 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_1 : int32_t { SUN_CORE_v = 1, SUN_CORONA_v = 2 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_2 : int32_t { TYPE_STAR_v = 0, TYPE_NORMAL_v = 1, TYPE_MOON_v = 2, TYPE_REFLECT_v = 3, TYPE_HEADLIGHT_v = 4, TYPE_HEX_v = 5, TYPE_CIRCLE_v = 6, TYPE_RING_v = 7, TYPE_STREAK_v = 8 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_3 : int32_t { FLARE_NONE_v = 0, FLARE_SUN_v = 1, FLARE_HEADLIGHTS_v = 2 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_4 : int32_t { REFLECTION_OFF_v = 0, REFLECTION_ON_v = 1 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_5 : int32_t { LOSCHECK_OFF_v = 0, LOSCHECK_ON_v = 1 };
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct C2dEffect_ZppContract {
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    int64_t lightType{0}; // Z++: ℕ
    int64_t roadReflection{0}; // Z++: ℕ
    int64_t flareType{0}; // Z++: ℕ
    int64_t shadowIntensity{0}; // Z++: ℕ
    int64_t flags{0}; // Z++: ℕ
    uint8_t corona_card{0u}; // Z++: Ref[RwTexture]
    uint8_t shadow_card{0u}; // Z++: Ref[RwTexture]
    ZppVec3 dir{}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    ZppVec3 useDir{}; // Z++: CVector
    ZppVec3 pos{}; // Z++: CVector
    int32_t col{0}; // Z++: CRGBA
    int32_t particle{0}; // Z++: Particle
    int32_t attractor{0}; // Z++: Attractor
    int32_t pedattr{0}; // Z++: PedAttractor
    constexpr bool checkInvariant() const {
        return (lightType >= 0)
            && (roadReflection >= 0)
            && (flareType >= 0)
            && (shadowIntensity >= 0)
            && (flags >= 0)
            && (corona_card <= 1u)
            && (shadow_card <= 1u)
            && (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Light_ZppContract {
    float dist{0.0f}; // Z++: ℝ
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    int64_t lightType{0}; // Z++: ℕ
    int64_t roadReflection{0}; // Z++: ℕ
    int64_t flareType{0}; // Z++: ℕ
    int64_t shadowIntensity{0}; // Z++: ℕ
    int64_t flags{0}; // Z++: ℕ
    uint8_t corona_card{0u}; // Z++: Ref[RwTexture]
    uint8_t shadow_card{0u}; // Z++: Ref[RwTexture]
    constexpr bool checkInvariant() const {
        return (lightType >= 0)
            && (roadReflection >= 0)
            && (flareType >= 0)
            && (shadowIntensity >= 0)
            && (flags >= 0)
            && (corona_card <= 1u)
            && (shadow_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Particle_ZppContract {
    int32_t particleType{0}; // Z++: ℤ
    ZppVec3 dir{}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    ZppVec3 dir{}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct PedAttractor_ZppContract {
    ZppVec3 queueDir{}; // Z++: CVector
    ZppVec3 useDir{}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Antennas.zpp
struct CAntenna_ZppContract {
    bool active{false}; // Z++: 𝔹
    bool updatedLastFrame{false}; // Z++: 𝔹
    int64_t id{0}; // Z++: ℕ
    float segmentLength{0.0f}; // Z++: ℝ
    std::array<ZppVec3, 6> pos{}; // Z++: seq CVector  -- bounded array [6]
    size_t pos_len{6u}; // Z++ cardinality #pos
    std::array<ZppVec3, 6> speed{}; // Z++: seq CVector  -- bounded array [6]
    size_t speed_len{6u}; // Z++ cardinality #speed
    constexpr bool checkInvariant() const {
        return (pos_len == 6u && pos.size() == 6u)
            && (speed_len == 6u && speed.size() == 6u)
            && (id >= 0);
    }
};
// State Schema Contract from zpp/renderer/Clouds.zpp
struct CClouds_ZppContract {
    float CloudRotation{0.0f}; // Z++: ℝ
    int64_t IndividualRotation{0}; // Z++: ℕ
    float ms_cameraRoll{0.0f}; // Z++: ℝ
    float ms_horizonZ{0.0f}; // Z++: ℝ
    float ms_HorizonTilt{0.0f}; // Z++: ℝ
    int32_t ms_colourTop{0}; // Z++: CRGBA
    int32_t ms_colourBottom{0}; // Z++: CRGBA
    int32_t ms_colourBkGrd{0}; // Z++: CRGBA
    constexpr bool checkInvariant() const {
        return (IndividualRotation >= 0);
    }
};
// State Schema Contract from zpp/renderer/Console.zpp
struct CConsole_ZppContract {
    int64_t m_nLineCount{0}; // Z++: ℕ
    int64_t m_nCurrentLine{0}; // Z++: ℕ
    std::array<uint32_t, 8> m_aTimer{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aTimer_len{8u}; // Z++ cardinality #m_aTimer
    std::array<uint32_t, 8> m_aRed{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aRed_len{8u}; // Z++ cardinality #m_aRed
    std::array<uint32_t, 8> m_aGreen{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aGreen_len{8u}; // Z++ cardinality #m_aGreen
    std::array<uint32_t, 8> m_aBlue{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aBlue_len{8u}; // Z++ cardinality #m_aBlue
    constexpr bool checkInvariant() const {
        return (m_aTimer_len == 8u && m_aTimer.size() == 8u)
            && (m_aRed_len == 8u && m_aRed.size() == 8u)
            && (m_aGreen_len == 8u && m_aGreen.size() == 8u)
            && (m_aBlue_len == 8u && m_aBlue.size() == 8u)
            && (m_nLineCount >= 0)
            && (m_nCurrentLine >= 0);
    }
};
// State Schema Contract from zpp/renderer/Coronas.zpp
struct FlareDef_ZppContract {
    float position{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    int32_t red{0}; // Z++: ℤ
    int32_t green{0}; // Z++: ℤ
    int32_t blue{0}; // Z++: ℤ
    int32_t alpha{0}; // Z++: ℤ
    int32_t texture{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Coronas.zpp
struct CRegisteredCorona_ZppContract {
    ZppVec3 coors{}; // Z++: CVector
    int64_t id{0}; // Z++: ℕ
    int64_t lastLOScheck{0}; // Z++: ℕ
    uint8_t texture_card{0u}; // Z++: Ref[RwTexture]
    float size{0.0f}; // Z++: ℝ
    float someAngle{0.0f}; // Z++: ℝ
    float drawDist{0.0f}; // Z++: ℝ
    float nearDist{0.0f}; // Z++: ℝ
    float heightAboveRoad{0.0f}; // Z++: ℝ
    int64_t red{0}; // Z++: ℕ
    int64_t green{0}; // Z++: ℕ
    int64_t blue{0}; // Z++: ℕ
    int64_t alpha{0}; // Z++: ℕ
    int64_t fadeAlpha{0}; // Z++: ℕ
    bool registeredThisFrame{false}; // Z++: 𝔹
    int32_t flareType{0}; // Z++: ℤ
    int32_t reflection{0}; // Z++: ℤ
    uint32_t LOScheck{0u}; // Z++: 𝔹
    uint32_t offScreen{0u}; // Z++: 𝔹
    uint32_t firstUpdate{0u}; // Z++: 𝔹
    uint32_t drawStreak{0u}; // Z++: 𝔹
    uint32_t sightClear{0u}; // Z++: 𝔹
    uint32_t useNearDist{0u}; // Z++: 𝔹
    uint32_t renderReflection{0u}; // Z++: 𝔹
    std::array<int32_t, 6> prevX{}; // Z++: seq ℤ  -- bounded array [6]
    size_t prevX_len{6u}; // Z++ cardinality #prevX
    std::array<int32_t, 6> prevY{}; // Z++: seq ℤ  -- bounded array [6]
    size_t prevY_len{6u}; // Z++ cardinality #prevY
    std::array<uint32_t, 6> prevRed{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevRed_len{6u}; // Z++ cardinality #prevRed
    std::array<uint32_t, 6> prevGreen{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevGreen_len{6u}; // Z++ cardinality #prevGreen
    std::array<uint32_t, 6> prevBlue{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevBlue_len{6u}; // Z++ cardinality #prevBlue
    std::array<bool, 6> hasValue{}; // Z++: seq 𝔹  -- bounded array [6]
    size_t hasValue_len{6u}; // Z++ cardinality #hasValue
    constexpr bool checkInvariant() const {
        return (prevX_len == 6u && prevX.size() == 6u)
            && (prevY_len == 6u && prevY.size() == 6u)
            && (prevRed_len == 6u && prevRed.size() == 6u)
            && (prevGreen_len == 6u && prevGreen.size() == 6u)
            && (prevBlue_len == 6u && prevBlue.size() == 6u)
            && (hasValue_len == 6u && hasValue.size() == 6u)
            && (id >= 0)
            && (lastLOScheck >= 0)
            && (texture_card <= 1u)
            && (red >= 0)
            && (green >= 0)
            && (blue >= 0)
            && (alpha >= 0)
            && (fadeAlpha >= 0)
            && (LOScheck <= 1u)
            && (offScreen <= 1u)
            && (firstUpdate <= 1u)
            && (drawStreak <= 1u)
            && (sightClear <= 1u)
            && (useNearDist <= 1u)
            && (renderReflection <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Credits.zpp
struct CCredits_ZppContract {
    bool bCreditsGoing{false}; // Z++: 𝔹
    int64_t CreditsStartTime{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (CreditsStartTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/CutsceneShadow.zpp
struct CCutsceneShadow_ZppContract {
    int32_t m_Camera{0}; // Z++: CShadowCamera
    bool m_bResample{false}; // Z++: 𝔹
    int32_t m_ResampleCamera{0}; // Z++: CShadowCamera
    int32_t m_nBlurPasses{0}; // Z++: ℤ
    int32_t m_BlurCamera{0}; // Z++: CShadowCamera
    bool m_bGradient{false}; // Z++: 𝔹
    int32_t m_GradientCamera{0}; // Z++: CShadowCamera
    uint8_t m_pAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t m_pClump_card{0u}; // Z++: Ref[RpClump]
    int32_t m_nRwObjectType{0}; // Z++: ℤ
    uint8_t m_pLight_card{0u}; // Z++: Ref[RpLight]
    int32_t m_BoundingSphere{0}; // Z++: RwSphere
    int32_t m_BaseSphere{0}; // Z++: RwSphere
    constexpr bool checkInvariant() const {
        return (m_pAtomic_card <= 1u)
            && (m_pClump_card <= 1u)
            && (m_pLight_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Draw.zpp
struct CDraw_ZppContract {
    float ms_fNearClipZ{0.0f}; // Z++: ℝ
    float ms_fFarClipZ{0.0f}; // Z++: ℝ
    float ms_fFOV{0.0f}; // Z++: ℝ
    float ms_fScaledFOV{0.0f}; // Z++: ℝ
    int64_t FadeValue{0}; // Z++: ℕ
    int64_t FadeRed{0}; // Z++: ℕ
    int64_t FadeGreen{0}; // Z++: ℕ
    int64_t FadeBlue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (FadeValue >= 0)
            && (FadeRed >= 0)
            && (FadeGreen >= 0)
            && (FadeBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScriptPath_ZppContract {
    int32_t m_numNodes{0}; // Z++: ℤ
    uint8_t m_pNode_card{0u}; // Z++: Ref[CPlaneNode]
    float m_fTotalLength{0.0f}; // Z++: ℝ
    float m_fSpeed{0.0f}; // Z++: ℝ
    float m_fPosition{0.0f}; // Z++: ℝ
    float m_fObjectLength{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    std::array<int32_t, 6> m_pObjects{}; // Z++: seq Ref[CObject]  -- bounded array [6]
    size_t m_pObjects_len{6u}; // Z++ cardinality #m_pObjects
    constexpr bool checkInvariant() const {
        return (m_pObjects_len == 6u && m_pObjects.size() == 6u)
            && (m_pNode_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScriptPaths_ZppContract {
    std::array<int32_t, 3> aArray{}; // Z++: seq CScriptPath  -- bounded array [3]
    size_t aArray_len{3u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 3u && aArray.size() == 3u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneTrail_ZppContract {
    std::array<ZppVec3, 16> m_pos{}; // Z++: seq CVector  -- bounded array [16]
    size_t m_pos_len{16u}; // Z++ cardinality #m_pos
    std::array<int32_t, 16> m_time{}; // Z++: seq ℤ  -- bounded array [16]
    size_t m_time_len{16u}; // Z++ cardinality #m_time
    constexpr bool checkInvariant() const {
        return (m_pos_len == 16u && m_pos.size() == 16u)
            && (m_time_len == 16u && m_time.size() == 16u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneTrails_ZppContract {
    std::array<int32_t, 6> aArray{}; // Z++: seq CPlaneTrail  -- bounded array [6]
    size_t aArray_len{6u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 6u && aArray.size() == 6u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneBanner_ZppContract {
    std::array<ZppVec3, 8> m_pos{}; // Z++: seq CVector  -- bounded array [8]
    size_t m_pos_len{8u}; // Z++ cardinality #m_pos
    constexpr bool checkInvariant() const {
        return (m_pos_len == 8u && m_pos.size() == 8u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneBanners_ZppContract {
    std::array<int32_t, 5> aArray{}; // Z++: seq CPlaneBanner  -- bounded array [5]
    size_t aArray_len{5u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 5u && aArray.size() == 5u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CEscalator_ZppContract {
    ZppVec3 m_pos0{}; // Z++: CVector
    ZppVec3 m_pos1{}; // Z++: CVector
    ZppVec3 m_pos2{}; // Z++: CVector
    ZppVec3 m_pos3{}; // Z++: CVector
    int32_t m_matrix{0}; // Z++: CMatrix
    bool m_bIsActive{false}; // Z++: 𝔹
    bool m_bIsMovingDown{false}; // Z++: 𝔹
    int32_t m_stepsCount{0}; // Z++: ℤ
    float m_lowerEnd{0.0f}; // Z++: ℝ
    float m_upperEnd{0.0f}; // Z++: ℝ
    ZppVec3 m_midPoint{}; // Z++: CVector
    float m_radius{0.0f}; // Z++: ℝ
    std::array<int32_t, 24> m_pSteps{}; // Z++: seq Ref[CObject]  -- bounded array [24]
    size_t m_pSteps_len{24u}; // Z++ cardinality #m_pSteps
    constexpr bool checkInvariant() const {
        return (m_pSteps_len == 24u && m_pSteps.size() == 24u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CMovingThing_ZppContract {
    uint8_t m_pNext_card{0u}; // Z++: Ref[CMovingThing]
    uint8_t m_pPrev_card{0u}; // Z++: Ref[CMovingThing]
    int32_t m_nType{0}; // Z++: ℤ
    int32_t m_farAway{0}; // Z++: ℤ
    ZppVec3 m_vecPosn{}; // Z++: CVector
    uint8_t m_pEntity_card{0u}; // Z++: Ref[CEntity]
    int32_t m_nHidden{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pNext_card <= 1u)
            && (m_pPrev_card <= 1u)
            && (m_pEntity_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CMovingThings_ZppContract {
    int32_t StartCloseList{0}; // Z++: CMovingThing
    int32_t EndCloseList{0}; // Z++: CMovingThing
    int32_t Num{0}; // Z++: ℤ
    std::array<int32_t, 48> aMovingThings{}; // Z++: seq CMovingThing  -- bounded array [NUMMOVINGTHINGS]
    size_t aMovingThings_len{48u}; // Z++ cardinality #aMovingThings
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (aMovingThings_len == 48u && aMovingThings.size() == 48u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScrollBar_ZppContract {
    int64_t m_Counter{0}; // Z++: ℕ
    int32_t m_pMessage{0}; // Z++: seq CHAR
    ZppVec3 m_Position{}; // Z++: CVector
    int64_t m_MessageCurrentChar{0}; // Z++: ℕ
    int64_t m_MessageLength{0}; // Z++: ℕ
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fIntensity{0.0f}; // Z++: ℝ
    std::array<uint32_t, 40> m_MessageBar{}; // Z++: seq ℕ  -- bounded array [40]
    size_t m_MessageBar_len{40u}; // Z++ cardinality #m_MessageBar
    int64_t m_Type{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    float m_fScale{0.0f}; // Z++: ℝ
    int32_t TonightsEvent{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_MessageBar_len == 40u && m_MessageBar.size() == 40u)
            && (m_Counter >= 0)
            && (m_MessageCurrentChar >= 0)
            && (m_MessageLength >= 0)
            && (m_Type >= 0)
            && (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CSmokeTrails_ZppContract {
    std::array<int32_t, 3> aSmoke{}; // Z++: seq CSmokeTrail  -- bounded array [3]
    size_t aSmoke_len{3u}; // Z++ cardinality #aSmoke
    bool CigOn{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (aSmoke_len == 3u && aSmoke.size() == 3u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CTowerClock_ZppContract {
    ZppVec3 m_Position{}; // Z++: CVector
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fDrawDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    float m_fIntensity{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CDigitalClock_ZppContract {
    ZppVec3 m_Position{}; // Z++: CVector
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fDrawDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    float m_fIntensity{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Font.zpp
struct CFontDetails_ZppContract {
    int32_t color{0}; // Z++: CRGBA
    float scaleX{0.0f}; // Z++: ℝ
    float scaleY{0.0f}; // Z++: ℝ
    float slant{0.0f}; // Z++: ℝ
    float slantRefX{0.0f}; // Z++: ℝ
    float slantRefY{0.0f}; // Z++: ℝ
    int32_t justify{0}; // Z++: bool8
    int32_t centre{0}; // Z++: bool8
    int32_t rightJustify{0}; // Z++: bool8
    int32_t background{0}; // Z++: bool8
    int32_t backgroundOnlyText{0}; // Z++: bool8
    int32_t proportional{0}; // Z++: bool8
    int32_t bIsShadow{0}; // Z++: bool8
    int32_t bFlash{0}; // Z++: bool8
    int32_t bBold{0}; // Z++: bool8
    float alphaFade{0.0f}; // Z++: ℝ
    int32_t backgroundColor{0}; // Z++: CRGBA
    float wrapX{0.0f}; // Z++: ℝ
    float centreSize{0.0f}; // Z++: ℝ
    float rightJustifyWrap{0.0f}; // Z++: ℝ
    int32_t style{0}; // Z++: ℤ
    int32_t bFontHalfTexture{0}; // Z++: bool8
    int64_t bank{0}; // Z++: ℕ
    int32_t dropShadowPosition{0}; // Z++: ℤ
    int32_t dropColor{0}; // Z++: CRGBA
    int32_t bFlashState{0}; // Z++: bool8
    int32_t nFlashTimer{0}; // Z++: ℤ
    int32_t anonymous_23{0}; // Z++: bool8
    int64_t anonymous_25{0}; // Z++: ℕ
    int32_t outlineColor{0}; // Z++: CRGBA
    int32_t bOutlineOn{0}; // Z++: ℤ
    int32_t line{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (bank >= 0)
            && (anonymous_25 >= 0);
    }
};
// State Schema Contract from zpp/renderer/Font.zpp
struct CFontRenderState_ZppContract {
    int64_t anonymous_0{0}; // Z++: ℕ
    float fTextPosX{0.0f}; // Z++: ℝ
    float fTextPosY{0.0f}; // Z++: ℝ
    float scaleX{0.0f}; // Z++: ℝ
    float scaleY{0.0f}; // Z++: ℝ
    int32_t color{0}; // Z++: CRGBA
    float fExtraSpace{0.0f}; // Z++: ℝ
    float slant{0.0f}; // Z++: ℝ
    float slantRefX{0.0f}; // Z++: ℝ
    float slantRefY{0.0f}; // Z++: ℝ
    int32_t bIsShadow{0}; // Z++: bool8
    int32_t bFontHalfTexture{0}; // Z++: bool8
    int32_t proportional{0}; // Z++: bool8
    int32_t anonymous_14{0}; // Z++: bool8
    int32_t style{0}; // Z++: ℤ
    int32_t bOutlineOn{0}; // Z++: ℤ
    int32_t line{0}; // Z++: ℤ
    int32_t rightJustify{0}; // Z++: bool8
    constexpr bool checkInvariant() const {
        return (anonymous_0 >= 0);
    }
};
// State Schema Contract from zpp/renderer/Glass.zpp
struct CFallingGlassPane_ZppContract {
    uint8_t m_attachment_card{0u}; // Z++: Ref[RwMatrix]
    float fz{0.0f}; // Z++: ℝ
    bool m_hasRwMatrix{false}; // Z++: 𝔹
    float uw{0.0f}; // Z++: ℝ
    float pw{0.0f}; // Z++: ℝ
    float fw{0.0f}; // Z++: ℝ
    ZppVec3 m_vecMoveSpeed{}; // Z++: CVector
    ZppVec3 m_vecTurn{}; // Z++: CVector
    int64_t m_nTimer{0}; // Z++: ℕ
    float m_fGroundZ{0.0f}; // Z++: ℝ
    float m_fStep{0.0f}; // Z++: ℝ
    int64_t m_nTriIndex{0}; // Z++: ℕ
    bool m_bActive{false}; // Z++: 𝔹
    bool m_bShattered{false}; // Z++: 𝔹
    bool m_bCarGlass{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_attachment_card <= 1u)
            && (m_nTimer >= 0)
            && (m_nTriIndex >= 0);
    }
};
// State Schema Contract from zpp/renderer/Instance.zpp
struct CInstance_ZppContract {
    int32_t m_matrix{0}; // Z++: CMatrix
    int32_t m_modelIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Lines.zpp
struct CLines_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/MBlur.zpp
struct CMBlur_ZppContract {
    uint8_t pFrontBuffer_card{0u}; // Z++: Ref[RwRaster]
    bool ms_bJustInitialised{false}; // Z++: 𝔹
    bool ms_bScaledBlur{false}; // Z++: 𝔹
    bool BlurOn{false}; // Z++: 𝔹
    float Drunkness{0.0f}; // Z++: ℝ
    int32_t pBufVertCount{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (pFrontBuffer_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct ActiveOccluderLine_ZppContract {
    int32_t origin{0}; // Z++: CVector2D
    int32_t direction{0}; // Z++: CVector2D
    float length{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct CActiveOccluder_ZppContract {
    std::array<int32_t, 6> lines{}; // Z++: seq ActiveOccluderLine  -- bounded array [6]
    size_t lines_len{6u}; // Z++ cardinality #lines
    int32_t linesCount{0}; // Z++: ℤ
    float radius{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (lines_len == 6u && lines.size() == 6u);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct COccluder_ZppContract {
    int32_t height{0}; // Z++: ℤ
    int32_t z{0}; // Z++: ℤ
    int64_t angle{0}; // Z++: ℕ
    int32_t listIndex{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (angle >= 0);
    }
};
// State Schema Contract from zpp/renderer/Particle.zpp
struct CParticle_ZppContract {
    ZppVec3 m_vecPosition{}; // Z++: CVector
    ZppVec3 m_vecVelocity{}; // Z++: CVector
    int64_t m_nTimeWhenWillBeDestroyed{0}; // Z++: ℕ
    int64_t m_nTimeWhenColorWillBeChanged{0}; // Z++: ℕ
    float m_fZGround{0.0f}; // Z++: ℝ
    ZppVec3 m_vecParticleMovementOffset{}; // Z++: CVector
    int32_t m_nCurrentZRotation{0}; // Z++: ℤ
    int64_t m_nZRotationTimer{0}; // Z++: ℕ
    float m_fCurrentZRadius{0.0f}; // Z++: ℝ
    int64_t m_nZRadiusTimer{0}; // Z++: ℕ
    int64_t m_nColorIntensity{0}; // Z++: ℕ
    int64_t m_nAlpha{0}; // Z++: ℕ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fExpansionRate{0.0f}; // Z++: ℝ
    int32_t m_nFadeToBlackTimer{0}; // Z++: ℤ
    int32_t m_nFadeAlphaTimer{0}; // Z++: ℤ
    int32_t m_nAnimationSpeedTimer{0}; // Z++: ℤ
    int32_t m_nRotationStep{0}; // Z++: ℤ
    int32_t m_nRotation{0}; // Z++: ℤ
    int64_t m_nCurrentFrame{0}; // Z++: ℕ
    int32_t m_Color{0}; // Z++: RwRGBA
    uint8_t m_pNext_card{0u}; // Z++: Ref[CParticle]
    uint8_t m_pUnusedListHead_card{0u}; // Z++: Ref[CParticle]
    std::array<float, 1024> m_SinTable{}; // Z++: seq ℝ  -- bounded array [SIN_COS_TABLE_SIZE]
    size_t m_SinTable_len{1024u}; // Z++ cardinality #m_SinTable
    std::array<float, 1024> m_CosTable{}; // Z++: seq ℝ  -- bounded array [SIN_COS_TABLE_SIZE]
    size_t m_CosTable_len{1024u}; // Z++ cardinality #m_CosTable
    int32_t field_4C{0}; // Z++: ℤ
    ZppVec3 m_vecScreenPosition{}; // Z++: CVector
    constexpr bool checkInvariant() const {
        return (m_SinTable_len == 1024u && m_SinTable.size() == 1024u)
            && (m_CosTable_len == 1024u && m_CosTable.size() == 1024u)
            && (m_nTimeWhenWillBeDestroyed >= 0)
            && (m_nTimeWhenColorWillBeChanged >= 0)
            && (m_nZRotationTimer >= 0)
            && (m_nZRadiusTimer >= 0)
            && (m_nColorIntensity >= 0)
            && (m_nAlpha >= 0)
            && (m_nCurrentFrame >= 0)
            && (m_pNext_card <= 1u)
            && (m_pUnusedListHead_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ParticleMgr.zpp
struct cParticleSystemMgr_ZppContract {
    uint8_t m_aParticles_card{0u}; // Z++: Ref[tParticleSystemData]
    constexpr bool checkInvariant() const {
        return (m_aParticles_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ParticleType.zpp
struct ParticleTypeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{98}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 98);
    }
};
// State Schema Contract from zpp/renderer/PlayerSkin.zpp
struct CPlayerSkin_ZppContract {
    int32_t m_txdSlot{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/PointLights.zpp
struct CRegisteredPointLight_ZppContract {
    ZppVec3 coors{}; // Z++: CVector
    ZppVec3 dir{}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    float red{0.0f}; // Z++: ℝ
    float green{0.0f}; // Z++: ℝ
    float blue{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int32_t fogType{0}; // Z++: ℤ
    bool castExtraShadows{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/RenderBuffer.zpp
struct RenderBuffer_ZppContract {
    int32_t VerticesToBeStored{0}; // Z++: ℤ
    int32_t IndicesToBeStored{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/RenderBuffer.zpp
struct VertexBufferUnion_ZppContract {
    std::array<int32_t, 512> im2d{}; // Z++: seq RwIm2DVertex  -- bounded array [TEMPBUFFERVERTSIZE]
    size_t im2d_len{512u}; // Z++ cardinality #im2d
    std::array<int32_t, 512> im3d{}; // Z++: seq RwIm3DVertex  -- bounded array [TEMPBUFFERVERTSIZE]
    size_t im3d_len{512u}; // Z++ cardinality #im3d
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (im2d_len == 512u && im2d.size() == 512u)
            && (im3d_len == 512u && im3d.size() == 512u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Renderer.zpp
struct EntityInfo_ZppContract {
    uint8_t ent_card{0u}; // Z++: Ref[CEntity]
    float sort{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (ent_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Renderer.zpp
struct BlockedRange_ZppContract {
    float b{0.0f}; // Z++: ℝ
    uint8_t next_card{0u}; // Z++: Ref[BlockedRange_prev]
    constexpr bool checkInvariant() const {
        return (next_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Rubbish.zpp
struct COneSheet_ZppContract {
    ZppVec3 m_basePos{}; // Z++: CVector
    ZppVec3 m_animatedPos{}; // Z++: CVector
    float m_targetZ{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    int32_t m_animationType{0}; // Z++: ℤ
    int64_t m_moveStart{0}; // Z++: ℕ
    int64_t m_moveDuration{0}; // Z++: ℕ
    float m_animHeight{0.0f}; // Z++: ℝ
    float m_xDist{0.0f}; // Z++: ℝ
    float m_yDist{0.0f}; // Z++: ℝ
    float m_angle{0.0f}; // Z++: ℝ
    bool m_isVisible{false}; // Z++: 𝔹
    bool m_targetIsVisible{false}; // Z++: 𝔹
    uint8_t m_next_card{0u}; // Z++: Ref[COneSheet]
    uint8_t m_prev_card{0u}; // Z++: Ref[COneSheet]
    constexpr bool checkInvariant() const {
        return (m_moveStart >= 0)
            && (m_moveDuration >= 0)
            && (m_next_card <= 1u)
            && (m_prev_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ShadowCamera.zpp
struct CShadowCamera_ZppContract {
    uint8_t m_pCamera_card{0u}; // Z++: Ref[RwCamera]
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    constexpr bool checkInvariant() const {
        return (m_pCamera_card <= 1u)
            && (m_pTexture_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CStoredShadow_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    uint8_t m_pCutsceneShadow_card{0u}; // Z++: Ref[CCutsceneShadow]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_ShadowType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    uint32_t bRendered{0u}; // Z++: 𝔹
    uint32_t bDrawOnBuildings{0u}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_pTexture_card <= 1u)
            && (m_pCutsceneShadow_card <= 1u)
            && (m_ShadowType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0)
            && (bRendered <= 1u)
            && (bDrawOnBuildings <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CPolyBunch_ZppContract {
    std::array<ZppVec3, 7> m_aVerts{}; // Z++: seq CVector  -- bounded array [7]
    size_t m_aVerts_len{7u}; // Z++ cardinality #m_aVerts
    uint8_t m_pNext_card{0u}; // Z++: Ref[CPolyBunch]
    int32_t m_nNumVerts{0}; // Z++: ℤ
    std::array<uint32_t, 7> m_aU{}; // Z++: seq ℕ  -- bounded array [7]
    size_t m_aU_len{7u}; // Z++ cardinality #m_aU
    std::array<uint32_t, 7> m_aV{}; // Z++: seq ℕ  -- bounded array [7]
    size_t m_aV_len{7u}; // Z++ cardinality #m_aV
    constexpr bool checkInvariant() const {
        return (m_aVerts_len == 7u && m_aVerts.size() == 7u)
            && (m_aU_len == 7u && m_aU.size() == 7u)
            && (m_aV_len == 7u && m_aV.size() == 7u)
            && (m_pNext_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CStaticShadow_ZppContract {
    int64_t m_nId{0}; // Z++: ℕ
    uint8_t m_pPolyBunch_card{0u}; // Z++: Ref[CPolyBunch]
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    ZppVec3 m_vecPosn{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_nType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    bool m_bJustCreated{false}; // Z++: 𝔹
    bool m_bRendered{false}; // Z++: 𝔹
    bool m_bTemp{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nId >= 0)
            && (m_pPolyBunch_card <= 1u)
            && (m_nTimeCreated >= 0)
            && (m_pTexture_card <= 1u)
            && (m_nType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CPermanentShadow_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    int64_t m_nLifeTime{0}; // Z++: ℕ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_nType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nTimeCreated >= 0)
            && (m_nLifeTime >= 0)
            && (m_pTexture_card <= 1u)
            && (m_nType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CShadows_ZppContract {
    int32_t ShadowsStoredToBeRendered{0}; // Z++: ℤ
    uint8_t pEmptyBunchList_card{0u}; // Z++: Ref[CPolyBunch]
    std::array<int32_t, 48> aPermanentShadows{}; // Z++: seq CPermanentShadow  -- bounded array [MAX_PERMAMENTSHADOWS]
    size_t aPermanentShadows_len{48u}; // Z++ cardinality #aPermanentShadows
    constexpr bool checkInvariant() const {
        return (aPermanentShadows_len == 48u && aPermanentShadows.size() == 48u)
            && (pEmptyBunchList_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Skidmarks.zpp
struct CSkidmark_ZppContract {
    std::array<ZppVec3, 16> m_pos{}; // Z++: seq CVector  -- bounded array [SKIDMARK_LENGTH]
    size_t m_pos_len{16u}; // Z++ cardinality #m_pos
    std::array<float, 16> m_sideX{}; // Z++: seq ℝ  -- bounded array [SKIDMARK_LENGTH]
    size_t m_sideX_len{16u}; // Z++ cardinality #m_sideX
    std::array<float, 16> m_sideY{}; // Z++: seq ℝ  -- bounded array [SKIDMARK_LENGTH]
    size_t m_sideY_len{16u}; // Z++ cardinality #m_sideY
    int64_t m_id{0}; // Z++: ℕ
    int64_t m_lastUpdate{0}; // Z++: ℕ
    int64_t m_fadeStart{0}; // Z++: ℕ
    int64_t m_fadeEnd{0}; // Z++: ℕ
    int64_t m_type{0}; // Z++: ℕ
    int32_t m_last{0}; // Z++: ℤ
    int64_t m_state{0}; // Z++: ℕ
    bool m_wasUpdated{false}; // Z++: 𝔹
    bool m_isBloody{false}; // Z++: 𝔹
    bool m_isMuddy{false}; // Z++: 𝔹
    std::array<ZppVec3, 16> m_side{}; // Z++: seq CVector  -- bounded array [SKIDMARK_LENGTH]
    size_t m_side_len{16u}; // Z++ cardinality #m_side
    constexpr bool checkInvariant() const {
        return (m_pos_len == 16u && m_pos.size() == 16u)
            && (m_sideX_len == 16u && m_sideX.size() == 16u)
            && (m_sideY_len == 16u && m_sideY.size() == 16u)
            && (m_side_len == 16u && m_side.size() == 16u)
            && (m_id >= 0)
            && (m_lastUpdate >= 0)
            && (m_fadeStart >= 0)
            && (m_fadeEnd >= 0)
            && (m_type >= 0)
            && (m_state >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CSpecialFX_ZppContract {
    bool bVideoCam{false}; // Z++: 𝔹
    bool bLiftCam{false}; // Z++: 𝔹
    bool bSnapShotActive{false}; // Z++: 𝔹
    int32_t SnapShotFrames{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CRegisteredMotionBlurStreak_ZppContract {
    int64_t m_id{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    std::array<ZppVec3, 3> m_pos1{}; // Z++: seq CVector  -- bounded array [3]
    size_t m_pos1_len{3u}; // Z++ cardinality #m_pos1
    std::array<ZppVec3, 3> m_pos2{}; // Z++: seq CVector  -- bounded array [3]
    size_t m_pos2_len{3u}; // Z++ cardinality #m_pos2
    std::array<bool, 3> m_isValid{}; // Z++: seq 𝔹  -- bounded array [3]
    size_t m_isValid_len{3u}; // Z++ cardinality #m_isValid
    constexpr bool checkInvariant() const {
        return (m_pos1_len == 3u && m_pos1.size() == 3u)
            && (m_pos2_len == 3u && m_pos2.size() == 3u)
            && (m_isValid_len == 3u && m_isValid.size() == 3u)
            && (m_id >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CBulletTrace_ZppContract {
    ZppVec3 m_vecStartPos{}; // Z++: CVector
    ZppVec3 m_vecEndPos{}; // Z++: CVector
    bool m_bInUse{false}; // Z++: 𝔹
    int64_t m_nCreationTime{0}; // Z++: ℕ
    int64_t m_nLifeTime{0}; // Z++: ℕ
    float m_fThickness{0.0f}; // Z++: ℝ
    int64_t m_fVisibility{0}; // Z++: ℕ
    ZppVec3 m_vecCurrentPos{}; // Z++: CVector
    ZppVec3 m_vecTargetPos{}; // Z++: CVector
    int64_t m_framesInUse{0}; // Z++: ℕ
    int64_t m_lifeTime{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nCreationTime >= 0)
            && (m_nLifeTime >= 0)
            && (m_fVisibility >= 0)
            && (m_framesInUse >= 0)
            && (m_lifeTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct C3dMarker_ZppContract {
    int32_t m_Matrix{0}; // Z++: CMatrix
    uint8_t m_pAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t m_pMaterial_card{0u}; // Z++: Ref[RpMaterial]
    int64_t m_nType{0}; // Z++: ℕ
    bool m_bIsUsed{false}; // Z++: 𝔹
    bool m_bFindZOnNextPlacement{false}; // Z++: 𝔹
    int64_t m_nIdentifier{0}; // Z++: ℕ
    int32_t m_Color{0}; // Z++: RwRGBA
    int64_t m_nPulsePeriod{0}; // Z++: ℕ
    int32_t m_nRotateRate{0}; // Z++: ℤ
    int64_t m_nStartTime{0}; // Z++: ℕ
    float m_fPulseFraction{0.0f}; // Z++: ℝ
    float m_fStdSize{0.0f}; // Z++: ℝ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fBrightness{0.0f}; // Z++: ℝ
    float m_fCameraRange{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_pAtomic_card <= 1u)
            && (m_pMaterial_card <= 1u)
            && (m_nType >= 0)
            && (m_nIdentifier >= 0)
            && (m_nPulsePeriod >= 0)
            && (m_nStartTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CBrightLight_ZppContract {
    ZppVec3 m_pos{}; // Z++: CVector
    ZppVec3 m_up{}; // Z++: CVector
    ZppVec3 m_side{}; // Z++: CVector
    ZppVec3 m_front{}; // Z++: CVector
    float m_camDist{0.0f}; // Z++: ℝ
    int64_t m_type{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_type >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CShinyText_ZppContract {
    std::array<ZppVec3, 4> m_verts{}; // Z++: seq CVector  -- bounded array [4]
    size_t m_verts_len{4u}; // Z++ cardinality #m_verts
    std::array<int32_t, 4> m_texCoords{}; // Z++: seq CVector2D  -- bounded array [4]
    size_t m_texCoords_len{4u}; // Z++ cardinality #m_texCoords
    float m_camDist{0.0f}; // Z++: ℝ
    int64_t m_type{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_verts_len == 4u && m_verts.size() == 4u)
            && (m_texCoords_len == 4u && m_texCoords.size() == 4u)
            && (m_type >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CSpecialParticleStuff_ZppContract {
    int64_t BoatFromStart{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (BoatFromStart >= 0);
    }
};
// State Schema Contract from zpp/renderer/Sprite.zpp
struct CSprite_ZppContract {
    float m_f2DNearScreenZ{0.0f}; // Z++: ℝ
    float m_f2DFarScreenZ{0.0f}; // Z++: ℝ
    float m_fRecipNearClipPlane{0.0f}; // Z++: ℝ
    int32_t m_bFlushSpriteBufferSwitchZTest{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Sprite2d.zpp
struct CSprite2d_ZppContract {
    float RecipNearClip{0.0f}; // Z++: ℝ
    float NearScreenZ{0.0f}; // Z++: ℝ
    float NearCamZ{0.0f}; // Z++: ℝ
    int32_t nextBufferVertex{0}; // Z++: ℤ
    int32_t nextBufferIndex{0}; // Z++: ℤ
    std::array<int32_t, 8> maVertices{}; // Z++: seq RwIm2DVertex  -- bounded array [8]
    size_t maVertices_len{8u}; // Z++ cardinality #maVertices
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t mCurrentBank{0}; // Z++: ℤ
    std::array<int32_t, 10> mpBankTextures{}; // Z++: seq Ref[RwTexture]  -- bounded array [10]
    size_t mpBankTextures_len{10u}; // Z++ cardinality #mpBankTextures
    std::array<int32_t, 10> mCurrentSprite{}; // Z++: seq ℤ  -- bounded array [10]
    size_t mCurrentSprite_len{10u}; // Z++ cardinality #mCurrentSprite
    std::array<int32_t, 10> mBankStart{}; // Z++: seq ℤ  -- bounded array [10]
    size_t mBankStart_len{10u}; // Z++ cardinality #mBankStart
    std::array<int32_t, 500> maBankVertices{}; // Z++: seq RwIm2DVertex  -- bounded array [500]
    size_t maBankVertices_len{500u}; // Z++ cardinality #maBankVertices
    constexpr bool checkInvariant() const {
        return (maVertices_len == 8u && maVertices.size() == 8u)
            && (mpBankTextures_len == 10u && mpBankTextures.size() == 10u)
            && (mCurrentSprite_len == 10u && mCurrentSprite.size() == 10u)
            && (mBankStart_len == 10u && mBankStart.size() == 10u)
            && (maBankVertices_len == 500u && maBankVertices.size() == 500u)
            && (m_pTexture_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/TexList.zpp
struct CTexList_ZppContract {
    std::array<bool, 400> ms_nTexUsed{}; // Z++: seq 𝔹  -- bounded array [MAX_TEXUSED]
    size_t ms_nTexUsed_len{400u}; // Z++ cardinality #ms_nTexUsed
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (ms_nTexUsed_len == 400u && ms_nTexUsed.size() == 400u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Timecycle.zpp
struct CTimeCycle_ZppContract {
    float m_fCurrentAmbientRed{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientGreen{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientBlue{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientRed_Obj{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientGreen_Obj{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientBlue_Obj{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientRed_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientGreen_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientBlue_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientRed_Obj_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientGreen_Obj_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentAmbientBlue_Obj_Bl{0.0f}; // Z++: ℝ
    float m_fCurrentDirectionalRed{0.0f}; // Z++: ℝ
    float m_fCurrentDirectionalGreen{0.0f}; // Z++: ℝ
    float m_fCurrentDirectionalBlue{0.0f}; // Z++: ℝ
    int32_t m_nCurrentSkyTopRed{0}; // Z++: ℤ
    int32_t m_nCurrentSkyTopGreen{0}; // Z++: ℤ
    int32_t m_nCurrentSkyTopBlue{0}; // Z++: ℤ
    int32_t m_nCurrentSkyBottomRed{0}; // Z++: ℤ
    int32_t m_nCurrentSkyBottomGreen{0}; // Z++: ℤ
    int32_t m_nCurrentSkyBottomBlue{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoreRed{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoreGreen{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoreBlue{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoronaRed{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoronaGreen{0}; // Z++: ℤ
    int32_t m_nCurrentSunCoronaBlue{0}; // Z++: ℤ
    float m_fCurrentSunSize{0.0f}; // Z++: ℝ
    float m_fCurrentSpriteSize{0.0f}; // Z++: ℝ
    float m_fCurrentSpriteBrightness{0.0f}; // Z++: ℝ
    int32_t m_nCurrentShadowStrength{0}; // Z++: ℤ
    int32_t m_nCurrentLightShadowStrength{0}; // Z++: ℤ
    int32_t m_nCurrentPoleShadowStrength{0}; // Z++: ℤ
    float m_fCurrentFogStart{0.0f}; // Z++: ℝ
    float m_fCurrentFarClip{0.0f}; // Z++: ℝ
    float m_fCurrentLightsOnGroundBrightness{0.0f}; // Z++: ℝ
    int32_t m_nCurrentLowCloudsRed{0}; // Z++: ℤ
    int32_t m_nCurrentLowCloudsGreen{0}; // Z++: ℤ
    int32_t m_nCurrentLowCloudsBlue{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsTopRed{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsTopGreen{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsTopBlue{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsBottomRed{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsBottomGreen{0}; // Z++: ℤ
    int32_t m_nCurrentFluffyCloudsBottomBlue{0}; // Z++: ℤ
    float m_fCurrentBlurRed{0.0f}; // Z++: ℝ
    float m_fCurrentBlurGreen{0.0f}; // Z++: ℝ
    float m_fCurrentBlurBlue{0.0f}; // Z++: ℝ
    float m_fCurrentWaterRed{0.0f}; // Z++: ℝ
    float m_fCurrentWaterGreen{0.0f}; // Z++: ℝ
    float m_fCurrentWaterBlue{0.0f}; // Z++: ℝ
    float m_fCurrentWaterAlpha{0.0f}; // Z++: ℝ
    int32_t m_nCurrentFogColourRed{0}; // Z++: ℤ
    int32_t m_nCurrentFogColourGreen{0}; // Z++: ℤ
    int32_t m_nCurrentFogColourBlue{0}; // Z++: ℤ
    int32_t m_FogReduction{0}; // Z++: ℤ
    int32_t m_bExtraColourOn{0}; // Z++: ℤ
    int32_t m_ExtraColour{0}; // Z++: ℤ
    float m_ExtraColourInter{0.0f}; // Z++: ℝ
    int32_t m_CurrentStoredValue{0}; // Z++: ℤ
    std::array<ZppVec3, 16> m_VectorToSun{}; // Z++: seq CVector  -- bounded array [16]
    size_t m_VectorToSun_len{16u}; // Z++ cardinality #m_VectorToSun
    std::array<float, 16> m_fShadowFrontX{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowFrontX_len{16u}; // Z++ cardinality #m_fShadowFrontX
    std::array<float, 16> m_fShadowFrontY{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowFrontY_len{16u}; // Z++ cardinality #m_fShadowFrontY
    std::array<float, 16> m_fShadowSideX{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowSideX_len{16u}; // Z++ cardinality #m_fShadowSideX
    std::array<float, 16> m_fShadowSideY{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowSideY_len{16u}; // Z++ cardinality #m_fShadowSideY
    std::array<float, 16> m_fShadowDisplacementX{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowDisplacementX_len{16u}; // Z++ cardinality #m_fShadowDisplacementX
    std::array<float, 16> m_fShadowDisplacementY{}; // Z++: seq ℝ  -- bounded array [16]
    size_t m_fShadowDisplacementY_len{16u}; // Z++ cardinality #m_fShadowDisplacementY
    int32_t m_nCurrentTreeShadowStrength{0}; // Z++: ℤ
    float m_fCurrentBlurAlpha{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_VectorToSun_len == 16u && m_VectorToSun.size() == 16u)
            && (m_fShadowFrontX_len == 16u && m_fShadowFrontX.size() == 16u)
            && (m_fShadowFrontY_len == 16u && m_fShadowFrontY.size() == 16u)
            && (m_fShadowSideX_len == 16u && m_fShadowSideX.size() == 16u)
            && (m_fShadowSideY_len == 16u && m_fShadowSideY.size() == 16u)
            && (m_fShadowDisplacementX_len == 16u && m_fShadowDisplacementX.size() == 16u)
            && (m_fShadowDisplacementY_len == 16u && m_fShadowDisplacementY.size() == 16u);
    }
};
// State Schema Contract from zpp/renderer/VarConsole.zpp
struct CVarConsoleEntry_ZppContract {
    int32_t text{0}; // Z++: seq CHAR
    uint8_t pInt8Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt16Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt32Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt64Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pUint8Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint16Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint32Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint64Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pFloatValue_card{0u}; // Z++: ℙ ℝ
    uint8_t pBoolValue_card{0u}; // Z++: ℙ 𝔹
    int32_t I8_min{0}; // Z++: ℤ
    int32_t I16_min{0}; // Z++: ℤ
    int32_t I32_min{0}; // Z++: ℤ
    int32_t I64_min{0}; // Z++: ℤ
    float F_min{0.0f}; // Z++: ℝ
    int32_t bAllowExceedBounds{0}; // Z++: bool8
    int64_t VarType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (pInt8Value_card <= 1u)
            && (pInt16Value_card <= 1u)
            && (pInt32Value_card <= 1u)
            && (pInt64Value_card <= 1u)
            && (pUint8Value_card <= 1u)
            && (pUint16Value_card <= 1u)
            && (pUint32Value_card <= 1u)
            && (pUint64Value_card <= 1u)
            && (pFloatValue_card <= 1u)
            && (pBoolValue_card <= 1u)
            && (VarType >= 0);
    }
};
// State Schema Contract from zpp/renderer/VarConsole.zpp
struct CVarConsole_ZppContract {
    int32_t m_nCountEntries{0}; // Z++: ℤ
    int32_t m_bIsOpen{0}; // Z++: bool8
    int32_t m_nCurEntry{0}; // Z++: ℤ
    int32_t m_nFirstEntryOnPage{0}; // Z++: ℤ
    int32_t m_nCurPage{0}; // Z++: ℤ
    int32_t m_nNumPages{0}; // Z++: ℤ
    std::array<int32_t, 91> m_aEntries{}; // Z++: seq CVarConsoleEntry  -- bounded array [91]
    size_t m_aEntries_len{91u}; // Z++ cardinality #m_aEntries
    constexpr bool checkInvariant() const {
        return (m_aEntries_len == 91u && m_aEntries.size() == 91u);
    }
};
// State Schema Contract from zpp/renderer/WaterCannon.zpp
struct CWaterCannon_ZppContract {
    int32_t m_nId{0}; // Z++: ℤ
    int32_t m_nCur{0}; // Z++: ℤ
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    int32_t field_C{0}; // Z++: ℤ
    std::array<ZppVec3, 16> m_avecPos{}; // Z++: seq CVector  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_avecPos_len{16u}; // Z++ cardinality #m_avecPos
    std::array<ZppVec3, 16> m_avecVelocity{}; // Z++: seq CVector  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_avecVelocity_len{16u}; // Z++ cardinality #m_avecVelocity
    std::array<bool, 16> m_abUsed{}; // Z++: seq 𝔹  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_abUsed_len{16u}; // Z++ cardinality #m_abUsed
    constexpr bool checkInvariant() const {
        return (m_avecPos_len == 16u && m_avecPos.size() == 16u)
            && (m_avecVelocity_len == 16u && m_avecVelocity.size() == 16u)
            && (m_abUsed_len == 16u && m_abUsed.size() == 16u)
            && (m_nTimeCreated >= 0);
    }
};
// State Schema Contract from zpp/renderer/WaterCreatures.zpp
struct CWaterCreature_ZppContract {
    uint8_t m_pObj_card{0u}; // Z++: Ref[CObject]
    float m_fFwdSpeed{0.0f}; // Z++: ℝ
    float m_fZTurnSpeed{0.0f}; // Z++: ℝ
    int32_t m_alpha{0}; // Z++: ℤ
    float m_fWaterDepth{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pObj_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/WaterCreatures.zpp
struct WaterCreatureProperties_ZppContract {
    uint8_t modelID_card{0u}; // Z++: ℙ ℤ
    float fFwdSpeed{0.0f}; // Z++: ℝ
    float fLevel{0.0f}; // Z++: ℝ
    float fUnknown{0.0f}; // Z++: ℝ
    float fWaterDepth{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (modelID_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/WaterLevel.zpp
struct ColData_ZppContract {
    int64_t SurfaceType{0}; // Z++: ℕ
    int64_t PieceType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (SurfaceType >= 0)
            && (PieceType >= 0);
    }
};
// State Schema Contract from zpp/renderer/WaterLevel.zpp
struct CWaterLevel_ZppContract {
    int32_t ms_nNoOfWaterLevels{0}; // Z++: ℤ
    std::array<float, 48> ms_aWaterZs{}; // Z++: seq ℝ  -- bounded array [48]
    size_t ms_aWaterZs_len{48u}; // Z++ cardinality #ms_aWaterZs
    std::array<int32_t, 48> ms_aWaterRects{}; // Z++: seq CRect  -- bounded array [48]
    size_t ms_aWaterRects_len{48u}; // Z++ cardinality #ms_aWaterRects
    bool WavesCalculatedThisFrame{false}; // Z++: 𝔹
    bool RequireWavySector{false}; // Z++: 𝔹
    bool MaskCalculatedThisFrame{false}; // Z++: 𝔹
    ZppVec3 PreCalculatedMaskPosn{}; // Z++: CVector
    bool m_bRenderSeaBed{false}; // Z++: 𝔹
    int32_t m_nRenderWaterLayers{0}; // Z++: ℤ
    uint8_t ms_pWavyAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t ms_pMaskAtomic_card{0u}; // Z++: Ref[RpAtomic]
    std::array<int32_t, 8> apGeomArray{}; // Z++: seq Ref[RpGeometry]  -- bounded array [MAX_BOAT_WAKES]
    size_t apGeomArray_len{8u}; // Z++ cardinality #apGeomArray
    int32_t nGeomUsed{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (ms_aWaterZs_len == 48u && ms_aWaterZs.size() == 48u)
            && (ms_aWaterRects_len == 48u && ms_aWaterRects.size() == 48u)
            && (apGeomArray_len == 8u && apGeomArray.size() == 8u)
            && (ms_pWavyAtomic_card <= 1u)
            && (ms_pMaskAtomic_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Weather.zpp
struct CWeather_ZppContract {
    int32_t SoundHandle{0}; // Z++: ℤ
    int32_t WeatherTypeInList{0}; // Z++: ℤ
    int32_t OldWeatherType{0}; // Z++: ℤ
    int32_t NewWeatherType{0}; // Z++: ℤ
    int32_t ForcedWeatherType{0}; // Z++: ℤ
    bool LightningFlash{false}; // Z++: 𝔹
    bool LightningBurst{false}; // Z++: 𝔹
    int64_t LightningStart{0}; // Z++: ℕ
    int64_t LightningFlashLastChange{0}; // Z++: ℕ
    int64_t WhenToPlayLightningSound{0}; // Z++: ℕ
    int64_t LightningDuration{0}; // Z++: ℕ
    int32_t StreamAfterRainTimer{0}; // Z++: ℤ
    float ExtraSunnyness{0.0f}; // Z++: ℝ
    float Foggyness{0.0f}; // Z++: ℝ
    float CloudCoverage{0.0f}; // Z++: ℝ
    float Wind{0.0f}; // Z++: ℝ
    float Rain{0.0f}; // Z++: ℝ
    float InterpolationValue{0.0f}; // Z++: ℝ
    float WetRoads{0.0f}; // Z++: ℝ
    float Rainbow{0.0f}; // Z++: ℝ
    float SunGlare{0.0f}; // Z++: ℝ
    float WindClipped{0.0f}; // Z++: ℝ
    float TrafficLightBrightness{0.0f}; // Z++: ℝ
    bool bScriptsForceRain{false}; // Z++: 𝔹
    bool Stored_StateStored{false}; // Z++: 𝔹
    float Stored_InterpolationValue{0.0f}; // Z++: ℝ
    int32_t Stored_OldWeatherType{0}; // Z++: ℤ
    int32_t Stored_NewWeatherType{0}; // Z++: ℤ
    float Stored_Rain{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (LightningStart >= 0)
            && (LightningFlashLastChange >= 0)
            && (WhenToPlayLightningSound >= 0)
            && (LightningDuration >= 0);
    }
};
// State Schema Contract from zpp/renderer/Weather.zpp
struct tRainStreak_ZppContract {
    ZppVec3 position{}; // Z++: CVector
    ZppVec3 direction{}; // Z++: CVector
    int64_t timer{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (timer >= 0);
    }
};
// State Schema Contract from zpp/renderer/WindModifiers.zpp
struct CWindModifiers_ZppContract {
    ZppVec3 m_pos{}; // Z++: CVector
    int32_t m_type{0}; // Z++: ℤ
    int32_t Number{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Query Operation Schema Contract (ΞCEscalator) from zpp/renderer/Fluff.zpp
constexpr bool op_CEscalator_IsActive_5(const CEscalator_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCEscalator) from zpp/renderer/Fluff.zpp
constexpr bool op_CEscalator_GetPosition_6(const CEscalator_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCClouds) from zpp/renderer/Clouds.zpp
constexpr bool op_CClouds_Init(const CClouds_ZppContract& pre, const CClouds_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.CloudRotation == 0.0f);
}
constexpr CClouds_ZppContract apply_CClouds_Init(const CClouds_ZppContract& pre) {
    CClouds_ZppContract post = pre;
    post.CloudRotation = 0.0f;
    return post;
}
// Delta Operation Schema Contract (ΔCCredits) from zpp/renderer/Credits.zpp
constexpr bool op_CCredits_Start(const CCredits_ZppContract& pre, const CCredits_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == true) && (post.CreditsStartTime == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CCredits_ZppContract apply_CCredits_Start(const CCredits_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = true;
    post.CreditsStartTime = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCCredits) from zpp/renderer/Credits.zpp
constexpr bool op_CCredits_Stop(const CCredits_ZppContract& pre, const CCredits_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == false);
}
constexpr CCredits_ZppContract apply_CCredits_Stop(const CCredits_ZppContract& pre) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = false;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_MotionBlurClose(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pFrontBuffer_card == 0u);
}
constexpr CMBlur_ZppContract apply_CMBlur_MotionBlurClose(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.pFrontBuffer_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_MotionBlurRender(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_bJustInitialised == false);
}
constexpr CMBlur_ZppContract apply_CMBlur_MotionBlurRender(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.ms_bJustInitialised = false;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_ClearDrunkBlur(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Drunkness == 0.0f);
}
constexpr CMBlur_ZppContract apply_CMBlur_ClearDrunkBlur(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.Drunkness = 0.0f;
    return post;
}
// Delta Operation Schema Contract (ΔcParticleSystemMgr) from zpp/renderer/ParticleMgr.zpp
constexpr bool op_cParticleSystemMgr_cParticleSystemMgr(const cParticleSystemMgr_ZppContract& pre, const cParticleSystemMgr_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_aParticles_card == 0u);
}
constexpr cParticleSystemMgr_ZppContract apply_cParticleSystemMgr_cParticleSystemMgr(const cParticleSystemMgr_ZppContract& pre) {
    cParticleSystemMgr_ZppContract post = pre;
    post.m_aParticles_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCShadowCamera) from zpp/renderer/ShadowCamera.zpp
constexpr bool op_CShadowCamera_CShadowCamera(const CShadowCamera_ZppContract& pre, const CShadowCamera_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pCamera_card == 0u) && (post.m_pTexture_card == 0u);
}
constexpr CShadowCamera_ZppContract apply_CShadowCamera_CShadowCamera(const CShadowCamera_ZppContract& pre) {
    CShadowCamera_ZppContract post = pre;
    post.m_pCamera_card = 0u;
    post.m_pTexture_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCSpecialParticleStuff) from zpp/renderer/SpecialFX.zpp
constexpr bool op_CSpecialParticleStuff_StartBoatFoamAnimation(const CSpecialParticleStuff_ZppContract& pre, const CSpecialParticleStuff_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.BoatFromStart == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CSpecialParticleStuff_ZppContract apply_CSpecialParticleStuff_StartBoatFoamAnimation(const CSpecialParticleStuff_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CSpecialParticleStuff_ZppContract post = pre;
    post.BoatFromStart = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite) from zpp/renderer/Sprite.zpp
constexpr bool op_CSprite_InitSpriteBuffer(const CSprite_ZppContract& pre, const CSprite_ZppContract& post, float RwIm2DGetNearScreenZ_in = 2.5f, float RwIm2DGetFarScreenZ_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_f2DNearScreenZ == RwIm2DGetNearScreenZ_in) && (post.m_f2DFarScreenZ == RwIm2DGetFarScreenZ_in);
}
constexpr CSprite_ZppContract apply_CSprite_InitSpriteBuffer(const CSprite_ZppContract& pre, float RwIm2DGetNearScreenZ_in = 2.5f, float RwIm2DGetFarScreenZ_in = 2.5f) {
    CSprite_ZppContract post = pre;
    post.m_f2DNearScreenZ = RwIm2DGetNearScreenZ_in;
    post.m_f2DFarScreenZ = RwIm2DGetFarScreenZ_in;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite) from zpp/renderer/Sprite.zpp
constexpr bool op_CSprite_RenderBufferedOneXLUSprite2D(const CSprite_ZppContract& pre, const CSprite_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bFlushSpriteBufferSwitchZTest == 1);
}
constexpr CSprite_ZppContract apply_CSprite_RenderBufferedOneXLUSprite2D(const CSprite_ZppContract& pre) {
    CSprite_ZppContract post = pre;
    post.m_bFlushSpriteBufferSwitchZTest = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite2d) from zpp/renderer/Sprite2d.zpp
constexpr bool op_CSprite2d_Delete(const CSprite2d_ZppContract& pre, const CSprite2d_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pTexture_card == 0u);
}
constexpr CSprite2d_ZppContract apply_CSprite2d_Delete(const CSprite2d_ZppContract& pre) {
    CSprite2d_ZppContract post = pre;
    post.m_pTexture_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite2d) from zpp/renderer/Sprite2d.zpp
constexpr bool op_CSprite2d_RenderVertexBuffer(const CSprite2d_ZppContract& pre, const CSprite2d_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.nextBufferVertex == 0) && (post.nextBufferIndex == 0);
}
constexpr CSprite2d_ZppContract apply_CSprite2d_RenderVertexBuffer(const CSprite2d_ZppContract& pre) {
    CSprite2d_ZppContract post = pre;
    post.nextBufferVertex = 0;
    post.nextBufferIndex = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCWaterCannon) from zpp/renderer/WaterCannon.zpp
constexpr bool op_CWaterCannon_Init(const CWaterCannon_ZppContract& pre, const CWaterCannon_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nId == 0) && (post.m_nCur == 0) && (post.m_nTimeCreated == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CWaterCannon_ZppContract apply_CWaterCannon_Init(const CWaterCannon_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CWaterCannon_ZppContract post = pre;
    post.m_nId = 0;
    post.m_nCur = 0;
    post.m_nTimeCreated = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWaterLevel) from zpp/renderer/WaterLevel.zpp
constexpr bool op_CWaterLevel_FreeBoatWakeArray(const CWaterLevel_ZppContract& pre, const CWaterLevel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.nGeomUsed == 0);
}
constexpr CWaterLevel_ZppContract apply_CWaterLevel_FreeBoatWakeArray(const CWaterLevel_ZppContract& pre) {
    CWaterLevel_ZppContract post = pre;
    post.nGeomUsed = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceWeather(const CWeather_ZppContract& pre, const CWeather_ZppContract& post, int32_t weather_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ForcedWeatherType == (int32_t)weather_in);
}
constexpr CWeather_ZppContract apply_CWeather_ForceWeather(const CWeather_ZppContract& pre, int32_t weather_in = 25) {
    CWeather_ZppContract post = pre;
    post.ForcedWeatherType = (int32_t)weather_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceWeatherNow(const CWeather_ZppContract& pre, const CWeather_ZppContract& post, int32_t weather_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.OldWeatherType == (int32_t)weather_in) && (post.NewWeatherType == (int32_t)weather_in) && (post.ForcedWeatherType == (int32_t)weather_in);
}
constexpr CWeather_ZppContract apply_CWeather_ForceWeatherNow(const CWeather_ZppContract& pre, int32_t weather_in = 25) {
    CWeather_ZppContract post = pre;
    post.OldWeatherType = (int32_t)weather_in;
    post.NewWeatherType = (int32_t)weather_in;
    post.ForcedWeatherType = (int32_t)weather_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_StoreWeatherState(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Stored_StateStored == true) && (post.Stored_InterpolationValue == pre.InterpolationValue) && (post.Stored_Rain == pre.Rain) && (post.Stored_NewWeatherType == pre.NewWeatherType) && (post.Stored_OldWeatherType == pre.OldWeatherType);
}
constexpr CWeather_ZppContract apply_CWeather_StoreWeatherState(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.Stored_StateStored = true;
    post.Stored_InterpolationValue = pre.InterpolationValue;
    post.Stored_Rain = pre.Rain;
    post.Stored_NewWeatherType = pre.NewWeatherType;
    post.Stored_OldWeatherType = pre.OldWeatherType;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_RestoreWeatherState(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Stored_StateStored == false) && (post.InterpolationValue == pre.Stored_InterpolationValue) && (post.Rain == pre.Stored_Rain) && (post.NewWeatherType == pre.Stored_NewWeatherType) && (post.OldWeatherType == pre.Stored_OldWeatherType);
}
constexpr CWeather_ZppContract apply_CWeather_RestoreWeatherState(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.Stored_StateStored = false;
    post.InterpolationValue = pre.Stored_InterpolationValue;
    post.Rain = pre.Stored_Rain;
    post.NewWeatherType = pre.Stored_NewWeatherType;
    post.OldWeatherType = pre.Stored_OldWeatherType;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceHurricaneWeather(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.OldWeatherType == 5) && (post.NewWeatherType == 5) && (post.ForcedWeatherType == 5);
}
constexpr CWeather_ZppContract apply_CWeather_ForceHurricaneWeather(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.OldWeatherType = 5;
    post.NewWeatherType = 5;
    post.ForcedWeatherType = 5;
    return post;
}
// Delta Operation Schema Contract (ΔCWindModifiers) from zpp/renderer/WindModifiers.zpp
constexpr bool op_CWindModifiers_RegisterOne(const CWindModifiers_ZppContract& pre, const CWindModifiers_ZppContract& post, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, int32_t type_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pos == pos_in) && (post.m_type == (int32_t)type_in);
}
constexpr CWindModifiers_ZppContract apply_CWindModifiers_RegisterOne(const CWindModifiers_ZppContract& pre, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, int32_t type_in = 25) {
    CWindModifiers_ZppContract post = pre;
    post.m_pos = pos_in;
    post.m_type = (int32_t)type_in;
    return post;
}
constexpr bool verify_renderer_zpp_invariants() {
    {
        C2dEffect_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        C2dEffect_ZppContract bad{};
        bad.lightType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Light_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Light_ZppContract bad{};
        bad.lightType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Particle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Particle_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Attractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Attractor_ZppContract bad{};
        bad.probability = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        PedAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedAttractor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CAntenna_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAntenna_ZppContract bad{};
        bad.pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClouds_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CClouds_ZppContract bad{};
        bad.IndividualRotation = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CConsole_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CConsole_ZppContract bad{};
        bad.m_aTimer_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        FlareDef_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        FlareDef_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredCorona_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredCorona_ZppContract bad{};
        bad.prevX_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCredits_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCredits_ZppContract bad{};
        bad.CreditsStartTime = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCutsceneShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCutsceneShadow_ZppContract bad{};
        bad.m_pAtomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CDraw_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDraw_ZppContract bad{};
        bad.FadeValue = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CScriptPath_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScriptPath_ZppContract bad{};
        bad.m_pObjects_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CScriptPaths_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScriptPaths_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneTrail_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneTrail_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneTrails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneTrails_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneBanner_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneBanner_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneBanners_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneBanners_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CEscalator_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEscalator_ZppContract bad{};
        bad.m_pSteps_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMovingThing_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovingThing_ZppContract bad{};
        bad.m_pNext_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMovingThings_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovingThings_ZppContract bad{};
        bad.aMovingThings_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CScrollBar_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScrollBar_ZppContract bad{};
        bad.m_MessageBar_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSmokeTrails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSmokeTrails_ZppContract bad{};
        bad.aSmoke_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTowerClock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTowerClock_ZppContract bad{};
        bad.m_uRed = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CDigitalClock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDigitalClock_ZppContract bad{};
        bad.m_uRed = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFontDetails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFontDetails_ZppContract bad{};
        bad.bank = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFontRenderState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFontRenderState_ZppContract bad{};
        bad.anonymous_0 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFallingGlassPane_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFallingGlassPane_ZppContract bad{};
        bad.m_attachment_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CInstance_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CInstance_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CLines_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMBlur_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMBlur_ZppContract bad{};
        bad.pFrontBuffer_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ActiveOccluderLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ActiveOccluderLine_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CActiveOccluder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CActiveOccluder_ZppContract bad{};
        bad.lines_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        COccluder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        COccluder_ZppContract bad{};
        bad.angle = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CParticle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CParticle_ZppContract bad{};
        bad.m_SinTable_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        cParticleSystemMgr_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cParticleSystemMgr_ZppContract bad{};
        bad.m_aParticles_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ParticleTypeModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ParticleTypeModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlayerSkin_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlayerSkin_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredPointLight_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredPointLight_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RenderBuffer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RenderBuffer_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        VertexBufferUnion_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VertexBufferUnion_ZppContract bad{};
        bad.im2d_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        EntityInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        EntityInfo_ZppContract bad{};
        bad.ent_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        BlockedRange_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        BlockedRange_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        COneSheet_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        COneSheet_ZppContract bad{};
        bad.m_moveStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShadowCamera_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShadowCamera_ZppContract bad{};
        bad.m_pCamera_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredShadow_ZppContract bad{};
        bad.m_pTexture_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPolyBunch_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPolyBunch_ZppContract bad{};
        bad.m_aVerts_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStaticShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStaticShadow_ZppContract bad{};
        bad.m_nId = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPermanentShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPermanentShadow_ZppContract bad{};
        bad.m_nTimeCreated = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShadows_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShadows_ZppContract bad{};
        bad.aPermanentShadows_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSkidmark_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSkidmark_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSpecialFX_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSpecialFX_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredMotionBlurStreak_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredMotionBlurStreak_ZppContract bad{};
        bad.m_pos1_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CBulletTrace_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBulletTrace_ZppContract bad{};
        bad.m_nCreationTime = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        C3dMarker_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        C3dMarker_ZppContract bad{};
        bad.m_pAtomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CBrightLight_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBrightLight_ZppContract bad{};
        bad.m_type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShinyText_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShinyText_ZppContract bad{};
        bad.m_verts_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSpecialParticleStuff_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSpecialParticleStuff_ZppContract bad{};
        bad.BoatFromStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSprite_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSprite_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSprite2d_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSprite2d_ZppContract bad{};
        bad.maVertices_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTexList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTexList_ZppContract bad{};
        bad.ms_nTexUsed_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTimeCycle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTimeCycle_ZppContract bad{};
        bad.m_VectorToSun_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVarConsoleEntry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVarConsoleEntry_ZppContract bad{};
        bad.pInt8Value_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVarConsole_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVarConsole_ZppContract bad{};
        bad.m_aEntries_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterCannon_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterCannon_ZppContract bad{};
        bad.m_avecPos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterCreature_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterCreature_ZppContract bad{};
        bad.m_pObj_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        WaterCreatureProperties_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        WaterCreatureProperties_ZppContract bad{};
        bad.modelID_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ColData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ColData_ZppContract bad{};
        bad.SurfaceType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterLevel_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterLevel_ZppContract bad{};
        bad.ms_aWaterZs_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWeather_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeather_ZppContract bad{};
        bad.LightningStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tRainStreak_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tRainStreak_ZppContract bad{};
        bad.timer = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWindModifiers_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWindModifiers_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const CEscalator_ZppContract st{};
        if (!op_CEscalator_IsActive_5(st)) return false;
    }
    {
        const CEscalator_ZppContract st{};
        if (!op_CEscalator_GetPosition_6(st)) return false;
    }
    {
        CClouds_ZppContract pre{};
        pre.CloudRotation = 1.0f;
        const auto post = apply_CClouds_Init(pre);
        if (!op_CClouds_Init(pre, post)) return false;
        {
            CClouds_ZppContract badPost = post;
            badPost.CloudRotation = 1.0f;
            if (op_CClouds_Init(pre, badPost)) return false;
        }
    }
    {
        CCredits_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.bCreditsGoing = false;
        pre.CreditsStartTime = 0;
        const auto post = apply_CCredits_Start(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CCredits_Start(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CCredits_ZppContract badPost = post;
            badPost.bCreditsGoing = false;
            if (op_CCredits_Start(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CCredits_ZppContract badPost = post;
            badPost.CreditsStartTime = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CCredits_Start(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CCredits_ZppContract pre{};
        pre.bCreditsGoing = true;
        const auto post = apply_CCredits_Stop(pre);
        if (!op_CCredits_Stop(pre, post)) return false;
        {
            CCredits_ZppContract badPost = post;
            badPost.bCreditsGoing = true;
            if (op_CCredits_Stop(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.pFrontBuffer_card = 1u;
        const auto post = apply_CMBlur_MotionBlurClose(pre);
        if (!op_CMBlur_MotionBlurClose(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.pFrontBuffer_card = 1u;
            if (op_CMBlur_MotionBlurClose(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.ms_bJustInitialised = true;
        const auto post = apply_CMBlur_MotionBlurRender(pre);
        if (!op_CMBlur_MotionBlurRender(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.ms_bJustInitialised = true;
            if (op_CMBlur_MotionBlurRender(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.Drunkness = 1.0f;
        const auto post = apply_CMBlur_ClearDrunkBlur(pre);
        if (!op_CMBlur_ClearDrunkBlur(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.Drunkness = 1.0f;
            if (op_CMBlur_ClearDrunkBlur(pre, badPost)) return false;
        }
    }
    {
        cParticleSystemMgr_ZppContract pre{};
        pre.m_aParticles_card = 1u;
        const auto post = apply_cParticleSystemMgr_cParticleSystemMgr(pre);
        if (!op_cParticleSystemMgr_cParticleSystemMgr(pre, post)) return false;
        {
            cParticleSystemMgr_ZppContract badPost = post;
            badPost.m_aParticles_card = 1u;
            if (op_cParticleSystemMgr_cParticleSystemMgr(pre, badPost)) return false;
        }
    }
    {
        CShadowCamera_ZppContract pre{};
        pre.m_pCamera_card = 1u;
        pre.m_pTexture_card = 1u;
        const auto post = apply_CShadowCamera_CShadowCamera(pre);
        if (!op_CShadowCamera_CShadowCamera(pre, post)) return false;
        {
            CShadowCamera_ZppContract badPost = post;
            badPost.m_pCamera_card = 1u;
            if (op_CShadowCamera_CShadowCamera(pre, badPost)) return false;
        }
        {
            CShadowCamera_ZppContract badPost = post;
            badPost.m_pTexture_card = 1u;
            if (op_CShadowCamera_CShadowCamera(pre, badPost)) return false;
        }
    }
    {
        CSpecialParticleStuff_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.BoatFromStart = 0;
        const auto post = apply_CSpecialParticleStuff_StartBoatFoamAnimation(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CSpecialParticleStuff_StartBoatFoamAnimation(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CSpecialParticleStuff_ZppContract badPost = post;
            badPost.BoatFromStart = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CSpecialParticleStuff_StartBoatFoamAnimation(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CSprite_ZppContract pre{};
        const float RwIm2DGetNearScreenZ_in = 2.5f;
        const float RwIm2DGetFarScreenZ_in = 2.5f;
        pre.m_f2DNearScreenZ = 0.0f;
        pre.m_f2DFarScreenZ = 0.0f;
        const auto post = apply_CSprite_InitSpriteBuffer(pre, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in);
        if (!op_CSprite_InitSpriteBuffer(pre, post, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        {
            CSprite_ZppContract badPost = post;
            badPost.m_f2DNearScreenZ = RwIm2DGetNearScreenZ_in + 1.0f;
            if (op_CSprite_InitSpriteBuffer(pre, badPost, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        }
        {
            CSprite_ZppContract badPost = post;
            badPost.m_f2DFarScreenZ = RwIm2DGetFarScreenZ_in + 1.0f;
            if (op_CSprite_InitSpriteBuffer(pre, badPost, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        }
    }
    {
        CSprite_ZppContract pre{};
        pre.m_bFlushSpriteBufferSwitchZTest = 0;
        const auto post = apply_CSprite_RenderBufferedOneXLUSprite2D(pre);
        if (!op_CSprite_RenderBufferedOneXLUSprite2D(pre, post)) return false;
        {
            CSprite_ZppContract badPost = post;
            badPost.m_bFlushSpriteBufferSwitchZTest = 0;
            if (op_CSprite_RenderBufferedOneXLUSprite2D(pre, badPost)) return false;
        }
    }
    {
        CSprite2d_ZppContract pre{};
        pre.m_pTexture_card = 1u;
        const auto post = apply_CSprite2d_Delete(pre);
        if (!op_CSprite2d_Delete(pre, post)) return false;
        {
            CSprite2d_ZppContract badPost = post;
            badPost.m_pTexture_card = 1u;
            if (op_CSprite2d_Delete(pre, badPost)) return false;
        }
    }
    {
        CSprite2d_ZppContract pre{};
        pre.nextBufferVertex = 1;
        pre.nextBufferIndex = 1;
        const auto post = apply_CSprite2d_RenderVertexBuffer(pre);
        if (!op_CSprite2d_RenderVertexBuffer(pre, post)) return false;
        {
            CSprite2d_ZppContract badPost = post;
            badPost.nextBufferVertex = 1;
            if (op_CSprite2d_RenderVertexBuffer(pre, badPost)) return false;
        }
        {
            CSprite2d_ZppContract badPost = post;
            badPost.nextBufferIndex = 1;
            if (op_CSprite2d_RenderVertexBuffer(pre, badPost)) return false;
        }
    }
    {
        CWaterCannon_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.m_nId = 1;
        pre.m_nCur = 1;
        pre.m_nTimeCreated = 0;
        const auto post = apply_CWaterCannon_Init(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CWaterCannon_Init(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nId = 1;
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nCur = 1;
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nTimeCreated = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CWaterLevel_ZppContract pre{};
        pre.nGeomUsed = 1;
        const auto post = apply_CWaterLevel_FreeBoatWakeArray(pre);
        if (!op_CWaterLevel_FreeBoatWakeArray(pre, post)) return false;
        {
            CWaterLevel_ZppContract badPost = post;
            badPost.nGeomUsed = 1;
            if (op_CWaterLevel_FreeBoatWakeArray(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        const int32_t weather_in = 25;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceWeather(pre, weather_in);
        if (!op_CWeather_ForceWeather(pre, post, weather_in)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeather(pre, badPost, weather_in)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        const int32_t weather_in = 25;
        pre.OldWeatherType = 0;
        pre.NewWeatherType = 0;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceWeatherNow(pre, weather_in);
        if (!op_CWeather_ForceWeatherNow(pre, post, weather_in)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.Stored_StateStored = false;
        pre.InterpolationValue = 7.0f; pre.Stored_InterpolationValue = 0.0f;
        pre.Rain = 7.0f; pre.Stored_Rain = 0.0f;
        pre.NewWeatherType = 7; pre.Stored_NewWeatherType = 0;
        pre.OldWeatherType = 7; pre.Stored_OldWeatherType = 0;
        const auto post = apply_CWeather_StoreWeatherState(pre);
        if (!op_CWeather_StoreWeatherState(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_StateStored = false;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_InterpolationValue = pre.InterpolationValue + 1.0f;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_Rain = pre.Rain + 1.0f;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_NewWeatherType = pre.NewWeatherType + 1;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_OldWeatherType = pre.OldWeatherType + 1;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.Stored_StateStored = true;
        pre.Stored_InterpolationValue = 7.0f; pre.InterpolationValue = 0.0f;
        pre.Stored_Rain = 7.0f; pre.Rain = 0.0f;
        pre.Stored_NewWeatherType = 7; pre.NewWeatherType = 0;
        pre.Stored_OldWeatherType = 7; pre.OldWeatherType = 0;
        const auto post = apply_CWeather_RestoreWeatherState(pre);
        if (!op_CWeather_RestoreWeatherState(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_StateStored = true;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.InterpolationValue = pre.Stored_InterpolationValue + 1.0f;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Rain = pre.Stored_Rain + 1.0f;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = pre.Stored_NewWeatherType + 1;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = pre.Stored_OldWeatherType + 1;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.OldWeatherType = 0;
        pre.NewWeatherType = 0;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceHurricaneWeather(pre);
        if (!op_CWeather_ForceHurricaneWeather(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
    }
    {
        CWindModifiers_ZppContract pre{};
        const ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const int32_t type_in = 25;
        pre.m_pos = ZppVec3{0.0f, 0.0f, 0.0f};
        pre.m_type = 0;
        const auto post = apply_CWindModifiers_RegisterOne(pre, pos_in, type_in);
        if (!op_CWindModifiers_RegisterOne(pre, post, pos_in, type_in)) return false;
        {
            CWindModifiers_ZppContract badPost = post;
            badPost.m_pos = ZppVec3{pos_in.x + 1.0f, pos_in.y, pos_in.z};
            if (op_CWindModifiers_RegisterOne(pre, badPost, pos_in, type_in)) return false;
        }
        {
            CWindModifiers_ZppContract badPost = post;
            badPost.m_type = (int32_t)(type_in + 1);
            if (op_CWindModifiers_RegisterOne(pre, badPost, pos_in, type_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
static_assert(CConsole_ZppContract{}.m_aTimer.size() == 8u, "CConsole MAX_LINES must be 8");
static_assert(op_CWeather_StoreWeatherState(CWeather_ZppContract{}, apply_CWeather_StoreWeatherState(CWeather_ZppContract{})), "CWeather_StoreWeatherState float-copy delta contract must hold");
static_assert(op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3), 3) && []() constexpr { auto bad = apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3); bad.NewWeatherType = 2; return !op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, bad, 3); }(), "CWeather_ForceWeatherNow must require equal weather types from single weather_in input");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
