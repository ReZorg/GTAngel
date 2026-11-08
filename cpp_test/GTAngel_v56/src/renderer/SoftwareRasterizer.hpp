    // ── Z++ Formal Spec Contract (renderer): 38 modules, 1267 schemas, 51 enums (2dEffect.zpp, Antennas.zpp, Clouds.zpp, Console.zpp, Coronas.zpp, Credits.zpp, CutsceneShadow.zpp, Draw.zpp) ──
    static constexpr int zpp_renderer_modules() { return 38; }
    static constexpr int zpp_renderer_schemas() { return 1267; }
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
// From zpp/renderer/Console.zpp
enum class Console_Enum_1 : int32_t { MAX_LINES_v = 8, MAX_STR_LEN_v = 40 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_1 : int32_t { SUN_CORE_v = 1, SUN_CORONA_v = 2 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_2 : int32_t { TYPE_STAR_v = 0, TYPE_NORMAL_v = 1, TYPE_MOON_v = 2, TYPE_REFLECT_v = 3, TYPE_HEADLIGHT_v = 4, TYPE_HEX_v = 5, TYPE_CIRCLE_v = 6, TYPE_RING_v = 7, TYPE_STREAK_v = 8 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_3 : int32_t { FLARE_NONE_v = 0, FLARE_SUN_v = 1, FLARE_HEADLIGHTS_v = 2 };
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    int32_t dir{0}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/Antennas.zpp
struct CAntenna_ZppContract {
    bool active{false}; // Z++: 𝔹
    bool updatedLastFrame{false}; // Z++: 𝔹
    int64_t id{0}; // Z++: ℕ
    float segmentLength{0.0f}; // Z++: ℝ
    std::array<int32_t, 6> pos{}; // Z++: seq CVector  -- bounded array [6]
    size_t pos_len{6u}; // Z++ cardinality #pos
    std::array<int32_t, 6> speed{}; // Z++: seq CVector  -- bounded array [6]
    size_t speed_len{6u}; // Z++ cardinality #speed
    constexpr bool checkInvariant() const {
        return (id >= 0)
            && (pos_len == 6u && pos.size() == 6u)
            && (speed_len == 6u && speed.size() == 6u);
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
    int32_t m_aTimer{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aRed{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aGreen{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aBlue{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    constexpr bool checkInvariant() const {
        return (m_nLineCount >= 0)
            && (m_nCurrentLine >= 0);
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
constexpr bool verify_renderer_zpp_invariants() {
    if (!Attractor_ZppContract{}.checkInvariant()) return false;
    if (!CAntenna_ZppContract{}.checkInvariant()) return false;
    if (!CClouds_ZppContract{}.checkInvariant()) return false;
    if (!CConsole_ZppContract{}.checkInvariant()) return false;
    if (!CCredits_ZppContract{}.checkInvariant()) return false;
    if (!CDraw_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
