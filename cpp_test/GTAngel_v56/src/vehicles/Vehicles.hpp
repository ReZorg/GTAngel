    // ── Z++ Formal Spec Contract (vehicles): 15 modules, 700 schemas, 54 enums (Automobile.zpp, Bike.zpp, Boat.zpp, CarGen.zpp, Cranes.zpp, DamageManager.zpp, Door.zpp, Ferry.zpp) ──
    static constexpr int zpp_vehicles_modules() { return 15; }
    static constexpr int zpp_vehicles_schemas() { return 700; }
    void drawSculptedCoupe(Vec3 centre, float ch, float sh, float wheelPhase,
                          Colour body, Colour cab, uint16_t id) {
        // LW34: true 3D fastback coupe — sloped front hood, angled windshield,
        // cabin roof, sloped rear fastback glass, rear trunk + spoiler wing,
        // front/rear bumpers at actual z=+1.15/-1.15, side windows, and 3D
        // cylindrical wheels oriented in the local Y-Z wheel plane.
        auto local = [&](float x, float y, float z) {
            return centre + Vec3{x*ch + z*sh, y, -x*sh + z*ch};
        };
        auto boxZ = [&](float w0, float w1, float y0, float y1, float z0, float z1, Colour col, Colour topCol) {
            Vec3 a = local(-w0, y0, z0), b = local( w0, y0, z0);
            Vec3 c = local( w1, y0, z1), d = local(-w1, y0, z1);
            Vec3 e = local(-w0, y1, z0), f = local( w0, y1, z0);
            Vec3 g = local( w1, y1, z1), h = local(-w1, y1, z1);
            addQuad(a, b, f, e, col, id); addQuad(b, c, g, f, col, id);
            addQuad(c, d, h, g, col, id); addQuad(d, a, e, h, col, id);
            addQuad(e, f, g, h, topCol, id);
        };
        const Colour glass{220, 185, 115, 255}, darkTrim{46, 48, 56, 255};
        const Colour hoodTop{(uint8_t)std::min(255, body.b + 22), (uint8_t)std::min(255, body.g + 22), (uint8_t)std::min(255, body.r + 22), 255};
        // LW37: 3D frustum body & tumblehome cabin (no vertical cabin box walls!)
        auto frustum = [&](float wb, float zb0, float zb1, float yb,
                           float wt, float zt0, float zt1, float yt,
                           Colour sideCol, Colour frontCol, Colour backCol, Colour topCol) {
            Vec3 b0 = local(-wb, yb, zb0), b1 = local(wb, yb, zb0);
            Vec3 b2 = local( wb, yb, zb1), b3 = local(-wb, yb, zb1);
            Vec3 t0 = local(-wt, yt, zt0), t1 = local(wt, yt, zt0);
            Vec3 t2 = local( wt, yt, zt1), t3 = local(-wt, yt, zt1);
            addQuad(b0, b1, t1, t0, backCol, id);
            addQuad(b2, b3, t3, t2, frontCol, id);
            addQuad(b1, b2, t2, t1, sideCol, id);
            addQuad(b3, b0, t0, t3, sideCol, id);
            addQuad(t0, t1, t2, t3, topCol, id);
        };
        // LW44: 3-zone sculpted sports coupe — sloped aerodynamic front nose
        // (+0.38..+1.16, tapering down to y=0.46), sculpted mid-body doors
        // (-0.64..+0.38, y=0.20..0.64), and tapered rear haunches (-1.16..-0.64)
        // so there is no flat step in front of the windshield!
        frustum(0.68f, -0.64f,  0.38f, 0.20f,
                0.64f, -0.60f,  0.38f, 0.64f, body, body, body, hoodTop);
        frustum(0.66f,  0.38f,  1.16f, 0.20f,
                0.56f,  0.38f,  1.12f, 0.52f, body, darkTrim, body, hoodTop);
        addQuad(local(-0.62f, 0.64f, 0.38f), local(0.62f, 0.64f, 0.38f),
                local( 0.56f, 0.48f, 1.12f), local(-0.56f, 0.48f, 1.12f), hoodTop, id);
        frustum(0.68f, -1.16f, -0.64f, 0.22f,
                0.62f, -1.10f, -0.60f, 0.62f, body, body, darkTrim, hoodTop);
        // True 3D tumblehome greenhouse: base (-0.76..+0.44, w=0.58) slopes
        // smoothly inward to roof (-0.32..+0.12, w=0.44) on all 4 sides!
        frustum(0.58f, -0.76f,  0.44f, 0.63f,
                0.44f, -0.32f,  0.12f, 1.05f, cab, glass, glass, hoodTop);
        // Projected warm headlight beams on the road ahead (+z) + red taillight glow (-z)
        for (float sx : {-0.40f, 0.40f}) {
            addTriangle(local(sx, 0.019f, 1.18f),
                        local(sx - 0.36f, 0.019f, 2.85f),
                        local(sx + 0.36f, 0.019f, 2.85f),
                        Colour{72, 96, 118, 255}, ObjectGround);
        }
        addQuad(local(-0.52f, 0.019f, -1.75f), local(0.52f, 0.019f, -1.75f),
                local( 0.52f, 0.019f, -1.16f), local(-0.52f, 0.019f, -1.16f),
                Colour{38, 42, 112, 255}, ObjectGround);
        // LW46: subtle cyan/amber neon chassis underglow + hood air-intake louvres
        addQuad(local(-0.46f, 0.020f, -0.78f), local(0.46f, 0.020f, -0.78f),
                local( 0.46f, 0.020f,  0.78f), local(-0.46f, 0.020f,  0.78f),
                Colour{110, 88, 44, 255}, ObjectGround);
        addQuad(local(-0.26f, 0.57f, 0.62f), local(0.26f, 0.57f, 0.62f),
                local( 0.24f, 0.53f, 0.84f), local(-0.24f, 0.53f, 0.84f), darkTrim, id);
        // Angled side-window glass inset + A/B/C pillars, wing mirrors, amber indicators & brake calipers
        for (float s : {-1.0f, 1.0f}) {
            addQuad(local(s*0.565f, 0.67f, -0.62f), local(s*0.565f, 0.67f, 0.42f),
                    local(s*0.465f, 1.01f,  0.12f), local(s*0.465f, 1.01f, -0.30f), glass, id);
            // B-pillar, A-pillar frame & C-pillar buttress
            addQuad(local(s*0.572f, 0.66f, -0.14f), local(s*0.572f, 0.66f, -0.04f),
                    local(s*0.462f, 1.02f, -0.06f), local(s*0.462f, 1.02f, -0.14f), cab, id);
            addTaperedBone(local(s*0.56f, 0.64f, 0.43f), local(s*0.44f, 1.05f, 0.12f), 0.032f, 0.026f, cab, id);
            addTaperedBone(local(s*0.56f, 0.64f, -0.74f), local(s*0.44f, 1.05f, -0.31f), 0.038f, 0.030f, cab, id);
            // Sculpted side mirror stalk + housing
            addTaperedBone(local(s*0.56f, 0.66f, 0.30f), local(s*0.68f, 0.72f, 0.26f), 0.025f, 0.038f, cab, id);
            // Amber corner turn-signal lenses (front +z and rear -z)
            addQuad(local(s*0.54f, 0.36f, 1.155f), local(s*0.62f, 0.36f, 1.155f),
                    local(s*0.62f, 0.46f, 1.155f), local(s*0.54f, 0.46f, 1.155f), Colour{45, 175, 255, 255}, id);
            addQuad(local(s*0.52f, 0.42f, -1.165f), local(s*0.62f, 0.42f, -1.165f),
                    local(s*0.62f, 0.52f, -1.165f), local(s*0.52f, 0.52f, -1.165f), Colour{45, 175, 255, 255}, id);
            // Crimson brake calipers + CAR_WING_LF/RF/LR/RR flared 3D fender arches + CAR_DOOR_LF/RF shut-lines
            for (float wz : {-0.72f, 0.72f}) {
                addJoint(local(s*0.72f, 0.34f, wz + 0.09f), 0.055f, Colour{48, 52, 225, 255}, id);
                addTaperedBone(local(s*0.69f, 0.48f, wz - 0.26f), local(s*0.71f, 0.56f, wz), 0.036f, 0.042f, hoodTop, id);
                addTaperedBone(local(s*0.71f, 0.56f, wz), local(s*0.69f, 0.48f, wz + 0.26f), 0.042f, 0.036f, hoodTop, id);
            }
            // Door vertical shut-line seams (CAR_DOOR_LF / CAR_DOOR_RF) + chrome beltline molding
            addTaperedBone(local(s*0.682f, 0.22f,  0.34f), local(s*0.585f, 0.65f,  0.34f), 0.012f, 0.012f, darkTrim, id);
            addTaperedBone(local(s*0.682f, 0.22f, -0.22f), local(s*0.585f, 0.65f, -0.22f), 0.012f, 0.012f, darkTrim, id);
            addTaperedBone(local(s*0.580f, 0.655f, -0.62f), local(s*0.580f, 0.655f, 0.42f), 0.014f, 0.014f, Colour{210, 218, 228, 255}, id);
        }
        // CAR_BONNET raised two-tone power-dome stripe (m_currentColour2) + CAR_WINDSCREEN wiper blades
        addQuad(local(-0.16f, 0.648f, 0.38f), local(0.16f, 0.648f, 0.38f),
                local( 0.12f, 0.492f, 1.11f), local(-0.12f, 0.492f, 1.11f), cab, id);
        addTaperedBone(local(-0.34f, 0.65f, 0.42f), local(-0.04f, 0.72f, 0.34f), 0.014f, 0.010f, darkTrim, id);
        addTaperedBone(local( 0.02f, 0.65f, 0.42f), local( 0.32f, 0.72f, 0.34f), 0.014f, 0.010f, darkTrim, id);
        // CVehicle::SetUpDriver & SetupPassenger (example_vehicle.cpp:1788): seated pDriver + steering wheel + dashboard
        addTaperedBone(local(-0.22f, 0.56f, -0.18f), local(-0.22f, 0.80f, -0.14f), 0.11f, 0.09f, darkTrim, id);
        addJoint(local(-0.22f, 0.88f, -0.12f), 0.082f, Colour{168, 182, 218, 255}, id);
        addJoint(local(-0.22f, 0.72f,  0.14f), 0.068f, Colour{32, 34, 42, 255}, id);
        addTaperedBone(local( 0.22f, 0.56f, -0.18f), local( 0.22f, 0.78f, -0.14f), 0.10f, 0.08f, darkTrim, id);
        // CVehicle::DoSunGlare (example_vehicle.cpp:2222): 4-point star specular glare corona on roof/windshield crown
        {
            const Vec3 gl = local(0.26f, 1.06f, 0.10f);
            addTriangle(gl + Vec3{-0.14f, 0.01f, 0.0f}, gl + Vec3{0.14f, 0.01f, 0.0f}, gl + Vec3{0.0f, 0.01f, 0.04f}, Colour{235, 250, 255, 255}, id);
            addTriangle(gl + Vec3{0.0f, 0.01f, -0.14f}, gl + Vec3{0.0f, 0.01f, 0.14f}, gl + Vec3{0.04f, 0.01f, 0.0f}, Colour{235, 250, 255, 255}, id);
        }
        // Front radiator grille + 3 chrome horizontal grille bars + front foglamps + rear fastback louvres
        addQuad(local(-0.32f, 0.28f, 1.165f), local(0.32f, 0.28f, 1.165f),
                local( 0.30f, 0.45f, 1.135f), local(-0.30f, 0.45f, 1.135f), darkTrim, id);
        for (float gy : {0.32f, 0.37f, 0.42f})
            addTaperedBone(local(-0.28f, gy, 1.168f), local(0.28f, gy, 1.168f), 0.010f, 0.010f, Colour{195, 205, 220, 255}, id);
        for (float fx : {-0.46f, 0.46f})
            addJoint(local(fx, 0.25f, 1.205f), 0.036f, Colour{185, 240, 255, 255}, id);
        for (int lv = 0; lv < 3; ++lv) {
            const float lz = -0.68f + lv * 0.12f, ly = 0.72f + lv * 0.09f;
            addQuad(local(-0.42f, ly, lz), local(0.42f, ly, lz),
                    local( 0.40f, ly + 0.03f, lz - 0.05f), local(-0.40f, ly + 0.03f, lz - 0.05f), darkTrim, id);
        }
        addQuad(local(-0.20f, 0.25f, -1.21f), local(0.20f, 0.25f, -1.21f),
                local( 0.20f, 0.36f, -1.21f), local(-0.20f, 0.36f, -1.21f), Colour{225, 215, 185, 255}, id);
        for (float ex : {-0.38f, 0.38f})
            addTaperedBone(local(ex, 0.21f, -1.24f), local(ex, 0.21f, -1.12f), 0.042f, 0.042f, Colour{185, 195, 210, 255}, id);
        // Side-skirt rocker panels, wheel-arch cutouts, and chrome door handles
        for (float s : {-1.0f, 1.0f}) {
            const float fx = s * 0.685f;
            addQuad(local(fx, 0.20f, -0.44f), local(fx, 0.20f, 0.44f),
                    local(fx, 0.27f, 0.44f),  local(fx, 0.27f, -0.44f), darkTrim, id);
            addQuad(local(fx, 0.52f, -0.08f), local(fx, 0.52f, 0.06f),
                    local(fx, 0.56f, 0.06f),  local(fx, 0.56f, -0.08f), Colour{210, 218, 228, 255}, id);
            for (float wz : {-0.72f, 0.72f}) {
                addQuad(local(fx, 0.20f, wz - 0.30f), local(fx, 0.20f, wz + 0.30f),
                        local(fx, 0.55f, wz + 0.22f), local(fx, 0.55f, wz - 0.22f), Colour{22, 24, 30, 255}, id);
            }
        }
        // Front splitter bumper (z = +1.08 .. +1.20) & rear diffuser bumper (z = -1.20 .. -1.08)
        boxZ(0.68f, 0.64f, 0.18f, 0.38f,  1.06f,  1.20f, darkTrim, darkTrim);
        boxZ(0.72f, 0.70f, 0.18f, 0.40f, -1.20f, -1.06f, darkTrim, darkTrim);
        // Rear spoiler wing (z = -1.12 .. -0.96, y = 0.76) + vertical twin pylons & aero endplates
        boxZ(0.64f, 0.64f, 0.74f, 0.80f, -1.12f, -0.96f, cab, hoodTop);
        for (float sx : {-0.36f, 0.36f})
            addTaperedBone(local(sx, 0.62f, -1.04f), local(sx, 0.75f, -1.06f), 0.024f, 0.020f, darkTrim, id);
        for (float sx : {-0.65f, 0.65f})
            addQuad(local(sx, 0.70f, -1.15f), local(sx, 0.70f, -0.92f),
                    local(sx, 0.84f, -0.94f), local(sx, 0.84f, -1.15f), cab, id);
        // LW52 (Antennas.zpp from Automobile.zpp): rear quarter whip antenna + 3D rear diffuser mudflaps
        {
            const float sway = 0.06f * sinf(wheelPhase * 0.5f);
            addTaperedBone(local(-0.56f, 0.62f, -0.88f), local(-0.56f + sway, 1.32f, -1.02f), 0.012f, 0.006f, Colour{195, 205, 220, 255}, id);
            addJoint(local(-0.56f + sway, 1.32f, -1.02f), 0.020f, Colour{65, 75, 250, 255}, id);
            for (float tx : {-0.64f, 0.64f}) {
                addQuad(local(tx - 0.06f, 0.15f, -1.16f), local(tx + 0.06f, 0.15f, -1.16f),
                        local(tx + 0.06f, 0.28f, -1.12f), local(tx - 0.06f, 0.28f, -1.12f),
                        darkTrim, id);
            }
        }
        // Headlights (+z) and full-width taillight bar (-z)
        for (float x : {-0.48f, 0.34f}) {
            addQuad(local(x, 0.38f, 1.15f), local(x + 0.16f, 0.38f, 1.15f),
                    local(x + 0.16f, 0.50f, 1.15f), local(x, 0.50f, 1.15f),
                    Colour{195, 245, 255, 255}, id);
        }
        addQuad(local(-0.58f, 0.42f, -1.15f), local(0.58f, 0.42f, -1.15f),
                local( 0.58f, 0.54f, -1.15f), local(-0.58f, 0.54f, -1.15f),
                Colour{55, 65, 250, 255}, id);
        // 3D wheels in the local Y-Z plane (with tire width & alloy spokes)
        for (float x : {-0.70f, 0.70f}) for (float z : {-0.72f, 0.72f}) {
            const float ox = x + (x > 0 ? 0.04f : -0.04f);
            for (int i = 0; i < 10; ++i) {
                const float a = 6.2831853f * i / 10.0f + wheelPhase;
                const float b = 6.2831853f * (i + 1) / 10.0f + wheelPhase;
                const Vec3 p0 = local(ox, 0.28f + sinf(a) * 0.26f, z + cosf(a) * 0.26f);
                const Vec3 p1 = local(ox, 0.28f + sinf(b) * 0.26f, z + cosf(b) * 0.26f);
                const Vec3 q0 = local(x,  0.28f + sinf(a) * 0.26f, z + cosf(a) * 0.26f);
                const Vec3 q1 = local(x,  0.28f + sinf(b) * 0.26f, z + cosf(b) * 0.26f);
                addTriangle(local(ox, 0.28f, z), p0, p1, Colour{34, 38, 44, 255}, id);
                addQuad(q0, q1, p1, p0, Colour{28, 32, 38, 255}, id);
            }
            for (int i = 0; i < 5; ++i) {
                const float a = 6.2831853f * i / 5.0f + wheelPhase;
                addTriangle(local(ox + (x > 0 ? 0.01f : -0.01f), 0.28f, z),
                            local(ox + (x > 0 ? 0.01f : -0.01f), 0.28f + sinf(a) * 0.15f, z + cosf(a) * 0.15f),
                            local(ox + (x > 0 ? 0.01f : -0.01f), 0.28f + sinf(a + 0.35f) * 0.15f, z + cosf(a + 0.35f) * 0.15f),
                            Colour{175, 185, 200, 255}, id);
            }
        }
    }

    void buildVehicle(const avatar3d::GameWorld& world) {
        addBlobShadow(world.vehiclePos, 0.68f);
        const float ch = cosf(world.vehicleHeading), sh = sinf(world.vehicleHeading);
        drawSculptedCoupe(world.vehiclePos, ch, sh,
                          world.vehicleWheelPhase, Colour{225, 130, 55, 255}, Colour{175, 85, 38, 255}, ObjectVehicle);
        // LW50: CVehicle::OpenDoor(DOOR_FRONT_LEFT) (example_vehicle.h:76-84, 299)
        // When DTE is near the parked coupe, swing open DOOR_FRONT_LEFT & illuminate the cockpit HUD!
        const float dx = eye_.x - world.vehiclePos.x, dz = eye_.z - world.vehiclePos.z;
        const float dist = sqrtf(dx*dx + dz*dz);
        if (dist < 7.5f && !world.vehicleOccupied) {
            const float openRatio = std::clamp((7.5f - dist) / 4.0f, 0.0f, 1.0f);
            auto vLoc = [&](float lx, float ly, float lz) {
                return world.vehiclePos + Vec3{lx*ch + lz*sh, ly, -lx*sh + lz*ch};
            };
            const float outX = -0.68f - 0.42f * openRatio;
            const float upY  =  0.24f + 0.48f * openRatio;
            addQuad(vLoc(-0.68f, 0.24f, 0.36f), vLoc(outX, upY, -0.22f),
                    vLoc(outX, upY + 0.38f, -0.22f), vLoc(-0.68f, 0.64f, 0.36f),
                    Colour{235, 145, 70, 255}, ObjectVehicle);
            addJoint(vLoc(-0.18f, 0.74f, 0.22f), 0.065f, Colour{245, 225, 95, 255}, ObjectVehicle);
        }
    }

    // re3 CVehicle family: lane traffic rendered as compact vehicles with
    // spinning wheels, train as linked boxes, heli with rotor disc.
    void buildTraffic(const avatar3d::GameWorld& world) {
        int trafficIndex = 0;
        for (const auto& v : world.traffic) {
            addBlobShadow(v.pos, v.orbitR > 0.0f ? 0.68f : 0.55f); // LW32: ground traffic (heli casts wide)
            const int vIndex = trafficIndex++;
            const Vec3 centre = v.pos;
            const float ch = cosf(v.heading), sh = sinf(v.heading);
            auto local = [&](float x, float y, float z) {
                return centre + Vec3{x*ch+z*sh, y, -x*sh+z*ch};
            };
            auto box = [&](float width, float halfLength, float bottom, float top, Colour colour, uint16_t id) {
                Vec3 a=local(-width,bottom,-halfLength), b=local(width,bottom,-halfLength);
                Vec3 c=local(width,bottom,halfLength), d=local(-width,bottom,halfLength);
                Vec3 e=local(-width,top,-halfLength), f=local(width,top,-halfLength);
                Vec3 g=local(width,top,halfLength), h=local(-width,top,halfLength);
                addQuad(a,b,f,e,colour,id); addQuad(b,c,g,f,colour,id);
                addQuad(c,d,h,g,colour,id); addQuad(d,a,e,h,colour,id);
                addQuad(e,f,g,h,colour,id);
            };
            auto boxZ = [&](float w0, float w1, float y0, float y1, float z0, float z1, Colour col, uint16_t id) {
                Vec3 a=local(-w0,y0,z0), b=local(w0,y0,z0), c=local(w1,y0,z1), d=local(-w1,y0,z1);
                Vec3 e=local(-w0,y1,z0), f=local(w0,y1,z0), g=local(w1,y1,z1), h=local(-w1,y1,z1);
                addQuad(a,b,f,e,col,id); addQuad(b,c,g,f,col,id);
                addQuad(c,d,h,g,col,id); addQuad(d,a,e,h,col,id);
                addQuad(e,f,g,h,col,id);
            };
            if (v.orbitR > 0.0f) {
                // LW36 directional CHeli: cabin (-0.55..+0.75), tapered nose
                // canopy (+0.45..+1.15), rearward tail boom (-2.15..-0.55),
                // and rear vertical tail fin (-2.15..-1.75).
                boxZ(0.52f, 0.48f, 0.12f, 0.96f, -0.55f, 0.55f, Colour{58, 62, 188, 255}, ObjectTraffic);
                boxZ(0.46f, 0.28f, 0.22f, 0.84f,  0.55f, 1.12f, Colour{215, 190, 115, 255}, ObjectTraffic);
                boxZ(0.10f, 0.18f, 0.58f, 0.82f, -2.15f, -0.55f, Colour{44, 48, 138, 255}, ObjectTraffic);
                boxZ(0.05f, 0.06f, 0.78f, 1.48f, -2.15f, -1.75f, Colour{58, 62, 188, 255}, ObjectTraffic);
                for (float sx : {-0.44f, 0.44f}) {
                    addTaperedBone(local(sx, -0.16f, -0.75f), local(sx, -0.16f, 0.85f), 0.035f, 0.035f, Colour{90,96,108,255}, ObjectTraffic);
                    for (float sz : {-0.40f, 0.40f})
                        addTaperedBone(local(sx, -0.16f, sz), local(sx*0.75f, 0.14f, sz), 0.028f, 0.028f, Colour{90,96,108,255}, ObjectTraffic);
                }
                const Vec3 rotor = centre + Vec3{0, 1.12f, 0};
                addJoint(rotor, 0.12f, Colour{90,96,108,255}, ObjectTraffic);
                const float spin = world.trafficTime * 18.0f;
                for (int i = 0; i < 4; ++i) {
                    const float a = 6.2831853f * i / 4 + spin;
                    const Vec3 tip = rotor + Vec3{cosf(a) * 1.85f, 0, sinf(a) * 1.85f};
                    addTriangle(rotor + Vec3{-sinf(a)*0.07f, 0.02f, cosf(a)*0.07f},
                                rotor + Vec3{ sinf(a)*0.07f, 0.02f,-cosf(a)*0.07f},
                                tip, Colour{140,148,162,255}, ObjectTraffic);
                }
                // LW48: helicopter port/starboard navigation strobes + ground searchlight pool
                addJoint(local(-0.54f, 0.42f, 0.15f), 0.075f, Colour{60, 65, 255, 255}, ObjectTraffic);
                addJoint(local( 0.54f, 0.42f, 0.15f), 0.075f, Colour{85, 245, 95, 255}, ObjectTraffic);
                for (int si = 0; si < 10; ++si) {
                    const float sa0 = 6.2831853f * si / 10.0f, sa1 = 6.2831853f * (si + 1) / 10.0f;
                    const Vec3 sp{centre.x + sh * 2.5f, 0.020f, centre.z + ch * 2.5f};
                    addTriangle(sp, {sp.x + cosf(sa0)*1.35f, 0.020f, sp.z + sinf(sa0)*1.35f},
                                    {sp.x + cosf(sa1)*1.35f, 0.020f, sp.z + sinf(sa1)*1.35f},
                                Colour{95, 115, 130, 255}, ObjectGround);
                }
                // LW49: CVehicle::HeliDustGenerate (example_vehicle.cpp:2098) swirling tarmac rotor-wash dust ring
                for (int di = 0; di < 16; ++di) {
                    const float da = 6.2831853f * di / 16.0f + world.trafficTime * 2.4f;
                    const float rIn = 1.95f, rOut = 2.45f;
                    const Vec3 d0{centre.x + cosf(da)*rIn,  0.021f, centre.z + sinf(da)*rIn};
                    const Vec3 d1{centre.x + cosf(da)*rOut, 0.021f, centre.z + sinf(da)*rOut};
                    const Vec3 d2{centre.x + cosf(da + 0.22f)*rOut, 0.021f, centre.z + sinf(da + 0.22f)*rOut};
                    addTriangle(d0, d1, d2, Colour{88, 94, 104, 255}, ObjectGround);
                }
                const Vec3 tailRotor = local(0.10f, 1.18f, -1.98f);
                const float tspin = world.trafficTime * 30.0f;
                for (int i = 0; i < 3; ++i) {
                    const float a = 6.2831853f * i / 3 + tspin;
                    const Vec3 tip = local(0.10f, 1.18f + sinf(a) * 0.34f, -1.98f + cosf(a) * 0.34f);
                    addTriangle(tailRotor, tip, local(0.12f, 1.18f, -1.95f), Colour{140,148,162,255}, ObjectTraffic);
                }
                continue;
            }
            const bool isTrain = v.speed > 4.0f && v.orbitR == 0.0f && v.pos.z < -10.0f;
            if (isTrain) {
                // LW43 streamlined bullet-metro CTrain: tapered nose cone,
                // cyan livery stripe, panoramic window bays, roof pantograph.
                boxZ(0.80f, 0.78f, 0.28f, 1.58f, -2.20f, 1.85f, Colour{108, 114, 126, 255}, ObjectTraffic);
                boxZ(0.78f, 0.46f, 0.28f, 1.18f,  1.85f, 2.65f, Colour{122, 128, 142, 255}, ObjectTraffic);
                boxZ(0.82f, 0.82f, 0.72f, 0.86f, -2.18f, 1.82f, Colour{235, 215, 75, 255}, ObjectTraffic);
                boxZ(0.83f, 0.81f, 0.98f, 1.42f, -1.92f, 1.72f, Colour{215, 185, 115, 255}, ObjectTraffic);
                boxZ(0.68f, 0.68f, 1.58f, 1.74f, -2.05f, 1.70f, Colour{72, 76, 86, 255}, ObjectTraffic);
                addTaperedBone(local(0, 1.74f, -0.4f), local(0, 2.08f, -0.1f), 0.04f, 0.03f, Colour{160,170,185,255}, ObjectTraffic);
                addTaperedBone(local(-0.35f, 2.08f, -0.1f), local(0.35f, 2.08f, -0.1f), 0.03f, 0.03f, Colour{160,170,185,255}, ObjectTraffic);
                {
                    const Vec3 lampT = local(0, 0.85f, 2.24f);
                    addTriangle(lampT, lampT + Vec3{0.16f,0.12f,0}, lampT + Vec3{-0.16f,0.12f,0},
                                Colour{180,244,255,255}, ObjectTraffic);
                }
                // Dark gangway connector between engine and carriage.
                addQuad(local(-0.5f,0.6f,-2.4f), local(0.5f,0.6f,-2.4f),
                        local(0.5f,1.5f,-2.9f), local(-0.5f,1.5f,-2.9f),
                        Colour{40,42,48,255}, ObjectTraffic);
                addQuad(local(-0.5f,1.5f,-2.4f), local(0.5f,1.5f,-2.4f),
                        local(0.5f,0.6f,-2.9f), local(-0.5f,0.6f,-2.9f),
                        Colour{40,42,48,255}, ObjectTraffic);
                const Vec3 back = centre + Vec3{-sinf(v.heading) * 5.2f, 0, -cosf(v.heading) * 5.2f};
                const float ch2 = cosf(v.heading), sh2 = sinf(v.heading);
                auto local2 = [&](float x, float y, float z) {
                    return back + Vec3{x*ch2+z*sh2, y, -x*sh2+z*ch2};
                };
                Vec3 a=local2(-0.8f,0.3f,-2.2f), b=local2(0.8f,0.3f,-2.2f);
                Vec3 c=local2(0.8f,0.3f,2.2f), d=local2(-0.8f,0.3f,2.2f);
                Vec3 e=local2(-0.8f,1.6f,-2.2f), f=local2(0.8f,1.6f,-2.2f);
                Vec3 g2=local2(0.8f,1.6f,2.2f), h2=local2(-0.8f,1.6f,2.2f);
                addQuad(a,b,f,e,Colour{80,84,94,255},ObjectTraffic); addQuad(b,c,g2,f,Colour{80,84,94,255},ObjectTraffic);
                addQuad(c,d,h2,g2,Colour{80,84,94,255},ObjectTraffic); addQuad(d,a,e,h2,Colour{80,84,94,255},ObjectTraffic);
                addQuad(e,f,g2,h2,Colour{120,124,134,255},ObjectTraffic);
                // Carriage glass band on both flanks + roof cap.
                addQuad(local2(-0.82f,1.0f,-1.9f), local2(-0.82f,1.0f,1.9f),
                        local2(-0.82f,1.45f,1.9f), local2(-0.82f,1.45f,-1.9f),
                        Colour{110,170,200,255}, ObjectTraffic);
                addQuad(local2(0.82f,1.0f,-1.9f), local2(0.82f,1.0f,1.9f),
                        local2(0.82f,1.45f,1.9f), local2(0.82f,1.45f,-1.9f),
                        Colour{110,170,200,255}, ObjectTraffic);
                addQuad(local2(-0.7f,1.6f,-2.1f), local2(0.7f,1.6f,-2.1f),
                        local2(0.7f,1.74f,2.1f), local2(-0.7f,1.74f,2.1f),
                        Colour{70,74,84,255}, ObjectTraffic);
                continue;
            }
            const bool isBike = v.speed < 2.5f && v.orbitR == 0.0f;
            if (isBike) {
                // LW36 sculpted sport motorcycle + articulated rider + 3D Y-Z wheels.
                const Colour bikeCol = (vIndex & 1) ? Colour{70, 185, 95, 255} : Colour{225, 95, 185, 255};
                const Colour darkMetal{48, 52, 60, 255}, jacket{58, 64, 78, 255};
                // Sculpted chassis, fuel tank, and front fairing
                addTaperedBone(local(0, 0.42f, -0.48f), local(0, 0.72f, 0.36f), 0.13f, 0.16f, bikeCol, ObjectTraffic);
                addTaperedBone(local(0, 0.72f, 0.36f), local(0, 0.88f, 0.62f), 0.14f, 0.08f, bikeCol, ObjectTraffic);
                addTaperedBone(local(0, 0.54f, -0.15f), local(0, 0.78f, -0.62f), 0.12f, 0.06f, bikeCol, ObjectTraffic);
                // Front forks + handlebar + LED headlamp
                for (float sx : {-0.08f, 0.08f})
                    addTaperedBone(local(sx, 0.28f, 0.58f), local(sx, 0.92f, 0.34f), 0.025f, 0.028f, darkMetal, ObjectTraffic);
                addTaperedBone(local(-0.28f, 0.94f, 0.32f), local(0.28f, 0.94f, 0.32f), 0.022f, 0.022f, darkMetal, ObjectTraffic);
                addJoint(local(0, 0.80f, 0.64f), 0.065f, Colour{195, 245, 255, 255}, ObjectTraffic);
                // LW48: sport-bike tinted windscreen bubble + chrome exhaust + headlight cone + taillight
                addTaperedBone(local(0, 0.86f, 0.52f), local(0, 1.04f, 0.28f), 0.095f, 0.075f, Colour{215, 185, 115, 255}, ObjectTraffic);
                addTaperedBone(local(0.12f, 0.34f, -0.10f), local(0.14f, 0.46f, -0.58f), 0.038f, 0.045f, Colour{185, 195, 210, 255}, ObjectTraffic);
                addJoint(local(0, 0.76f, -0.64f), 0.048f, Colour{55, 65, 250, 255}, ObjectTraffic);
                addTriangle(local(0, 0.019f, 0.72f), local(-0.34f, 0.019f, 2.25f), local(0.34f, 0.019f, 2.25f),
                            Colour{72, 96, 118, 255}, ObjectGround);
                // Anatomical rider: hips, forward-leaning jacket torso, arms to bars, legs, helmet + visor
                const Vec3 rHip = local(0, 0.72f, -0.18f), rSh = local(0, 1.18f, 0.08f), rHead = local(0, 1.38f, 0.16f);
                addTaperedBone(rHip, rSh, 0.13f, 0.14f, jacket, ObjectTraffic);
                for (float sx : {-1.0f, 1.0f}) {
                    addTaperedBone(local(sx*0.14f, 1.14f, 0.08f), local(sx*0.24f, 0.94f, 0.32f), 0.045f, 0.035f, jacket, ObjectTraffic);
                    addTaperedBone(local(sx*0.11f, 0.70f, -0.16f), local(sx*0.14f, 0.42f, -0.05f), 0.055f, 0.042f, darkMetal, ObjectTraffic);
                }
                addJoint(rHead, 0.125f, Colour{55, 70, 225, 255}, ObjectTraffic);
                addJoint(local(0, 1.38f, 0.24f), 0.075f, Colour{220, 195, 110, 255}, ObjectTraffic);
                // 3D wheels in the local Y-Z wheel plane
                const float wPhase = world.trafficTime * (v.dir > 0 ? 7.0f : -7.0f);
                for (float z : {-0.56f, 0.58f}) {
                    for (int i = 0; i < 10; ++i) {
                        const float a = 6.2831853f * i / 10.0f + wPhase;
                        const float b = 6.2831853f * (i + 1) / 10.0f + wPhase;
                        const Vec3 p0 = local( 0.05f, 0.28f + sinf(a)*0.26f, z + cosf(a)*0.26f);
                        const Vec3 p1 = local( 0.05f, 0.28f + sinf(b)*0.26f, z + cosf(b)*0.26f);
                        const Vec3 q0 = local(-0.05f, 0.28f + sinf(a)*0.26f, z + cosf(a)*0.26f);
                        const Vec3 q1 = local(-0.05f, 0.28f + sinf(b)*0.26f, z + cosf(b)*0.26f);
                        addTriangle(local(0.05f, 0.28f, z), p0, p1, Colour{34, 38, 44, 255}, ObjectTraffic);
                        addTriangle(local(-0.05f, 0.28f, z), q1, q0, Colour{34, 38, 44, 255}, ObjectTraffic);
                        addQuad(q0, q1, p1, p0, Colour{28, 32, 38, 255}, ObjectTraffic);
                    }
                }
                continue;
            }
            // Exemplar traffic sedan: player-vehicle silhouette + stable livery.
            static const Colour liveries[5] = {
                {213,119,58,255}, {58,110,170,255}, {60,60,150,255},
                {90,130,96,255}, {60,180,220,255}};
            static const Colour cabins[5] = {
                {174,80,42,255}, {44,84,130,255}, {46,46,110,255},
                {66,96,70,255}, {40,130,160,255}};
            const bool isPolice = (vIndex == 0);
            const bool isTaxi   = (vIndex == 1);
            const Colour body = isPolice ? Colour{230, 232, 238, 255}
                              : (isTaxi  ? Colour{42, 195, 248, 255} : liveries[vIndex % 5]);
            const Colour cab  = isPolice ? Colour{34, 38, 48, 255}
                              : (isTaxi  ? Colour{32, 155, 215, 255} : cabins[vIndex % 5]);
            drawSculptedCoupe(centre, ch, sh, world.trafficTime * (v.dir > 0 ? 8.0f : -8.0f),
                              body, cab, ObjectTraffic);
            if (isPolice) {
                // CVehicle::UsesSiren (MI_POLICE): roof lightbar + alternating red/blue siren strobes
                const bool sSwap = (((int)(world.trafficTime * 7.5f)) & 1) != 0;
                const Colour sL = sSwap ? Colour{55, 55, 255, 255} : Colour{255, 130, 45, 255};
                const Colour sR = sSwap ? Colour{255, 130, 45, 255} : Colour{55, 55, 255, 255};
                boxZ(0.36f, 0.36f, 1.05f, 1.15f, -0.18f, -0.02f, Colour{40, 44, 52, 255}, ObjectTraffic);
                addJoint(local(-0.24f, 1.13f, -0.10f), 0.075f, sL, ObjectTraffic);
                addJoint(local( 0.24f, 1.13f, -0.10f), 0.075f, sR, ObjectTraffic);
                addQuad(local(-1.35f, 0.020f, -0.95f), local(0.0f, 0.020f, -0.95f),
                        local( 0.0f, 0.020f,  0.95f), local(-1.35f, 0.020f,  0.95f),
                        sSwap ? Colour{48, 48, 128, 255} : Colour{128, 72, 38, 255}, ObjectGround);
            } else if (isTaxi) {
                // CVehicle::IsTaxi (MI_TAXI): illuminated roof TAXI sign (m_aExtras[0]) + checker stripe
                boxZ(0.22f, 0.20f, 1.05f, 1.18f, -0.18f, 0.02f, Colour{95, 232, 255, 255}, ObjectTraffic);
            }
        }
        // LW50: opposite-lane CVehicle::bIsAmbulanceOnDuty (MI_AMBULAN, z = -1.4f)
        // and CVehicle::bIsBus (6-wheel CAR_WHEEL_RF/RM/RB metropolitan coach, x = -1.4f)
        {
            const float ambCycle = fmodf(world.trafficTime * 2.9f, 112.0f);
            const float ambDir   = ambCycle < 56.0f ? 1.0f : -1.0f;
            const float ambX     = ambCycle < 56.0f ? (-28.0f + ambCycle) : (28.0f - (ambCycle - 56.0f));
            const Vec3  ambPos{ambX, 0.0f, -1.4f};
            const float ach = 0.0f, ash = ambDir; // heading +pi/2 or -pi/2
            auto aLoc = [&](float x, float y, float z) { return ambPos + Vec3{x*ach + z*ash, y, -x*ash + z*ach}; };
            auto aBox = [&](float w0, float w1, float y0, float y1, float z0, float z1, Colour col) {
                Vec3 a=aLoc(-w0,y0,z0), b=aLoc(w0,y0,z0), c=aLoc(w1,y0,z1), d=aLoc(-w1,y0,z1);
                Vec3 e=aLoc(-w0,y1,z0), f=aLoc(w0,y1,z0), g=aLoc(w1,y1,z1), h=aLoc(-w1,y1,z1);
                addQuad(a,b,f,e,col,ObjectTraffic); addQuad(b,c,g,f,col,ObjectTraffic);
                addQuad(c,d,h,g,col,ObjectTraffic); addQuad(d,a,e,h,col,ObjectTraffic);
                addQuad(e,f,g,h,col,ObjectTraffic);
            };
            addBlobShadow(ambPos, 0.66f);
            // High-roof white & crimson Emergency Ambulance Van (bIsAmbulanceOnDuty / bIsVan)
            aBox(0.72f, 0.72f, 0.22f, 1.38f, -1.32f, 0.42f, Colour{235, 238, 244, 255});
            aBox(0.68f, 0.60f, 0.22f, 0.88f,  0.42f, 1.24f, Colour{235, 238, 244, 255});
            aBox(0.73f, 0.73f, 0.56f, 0.74f, -1.30f, 1.20f, Colour{52, 58, 232, 255}); // crimson emergency stripe
            aBox(0.62f, 0.54f, 0.72f, 1.18f,  0.38f, 0.96f, Colour{215, 185, 115, 255}); // cab windshield
            // Red cross emblem on ambulance flanks + flashing red/white siren bar
            for (float s : {-0.735f, 0.735f}) {
                addQuad(aLoc(s, 0.86f, -0.58f), aLoc(s, 0.86f, -0.30f), aLoc(s, 1.22f, -0.30f), aLoc(s, 1.22f, -0.58f), Colour{52, 58, 232, 255}, ObjectTraffic);
            }
            const bool aFlash = (((int)(world.trafficTime * 8.0f)) & 1) != 0;
            addJoint(aLoc(-0.28f, 1.44f, 0.32f), 0.08f, aFlash ? Colour{55, 65, 255, 255} : Colour{240, 245, 255, 255}, ObjectTraffic);
            addJoint(aLoc( 0.28f, 1.44f, 0.32f), 0.08f, aFlash ? Colour{240, 245, 255, 255} : Colour{55, 65, 255, 255}, ObjectTraffic);
            // LW51: Ambulance front/rear bumpers, headlights + road beams, taillights, and 4 spinning 3D wheels
            aBox(0.74f, 0.72f, 0.18f, 0.34f,  1.18f,  1.30f, Colour{46, 48, 56, 255});
            aBox(0.74f, 0.74f, 0.18f, 0.34f, -1.38f, -1.26f, Colour{46, 48, 56, 255});
            for (float hx : {-0.48f, 0.34f}) {
                addQuad(aLoc(hx, 0.38f, 1.26f), aLoc(hx + 0.16f, 0.38f, 1.26f),
                        aLoc(hx + 0.16f, 0.50f, 1.26f), aLoc(hx, 0.50f, 1.26f), Colour{195, 245, 255, 255}, ObjectTraffic);
                addTriangle(aLoc(hx + 0.08f, 0.019f, 1.28f), aLoc(hx - 0.28f, 0.019f, 2.85f),
                            aLoc(hx + 0.44f, 0.019f, 2.85f), Colour{72, 96, 118, 255}, ObjectGround);
            }
            addQuad(aLoc(-0.62f, 0.40f, -1.34f), aLoc(0.62f, 0.40f, -1.34f),
                    aLoc( 0.62f, 0.52f, -1.34f), aLoc(-0.62f, 0.52f, -1.34f), Colour{55, 65, 250, 255}, ObjectTraffic);
            const float aWheelPhase = world.trafficTime * (ambDir > 0 ? 7.5f : -7.5f);
            for (float wx : {-0.74f, 0.74f}) for (float wz : {-0.82f, 0.78f}) {
                for (int wi = 0; wi < 8; ++wi) {
                    const float wa = 6.2831853f * wi / 8.0f + aWheelPhase;
                    const float wb = 6.2831853f * (wi + 1) / 8.0f + aWheelPhase;
                    addTriangle(aLoc(wx, 0.28f, wz),
                                aLoc(wx, 0.28f + sinf(wa)*0.27f, wz + cosf(wa)*0.27f),
                                aLoc(wx, 0.28f + sinf(wb)*0.27f, wz + cosf(wb)*0.27f),
                                (wi & 1) ? Colour{34, 38, 44, 255} : Colour{165, 175, 190, 255}, ObjectTraffic);
                }
            }

            // Metropolitan 6-Wheel City Bus (bIsBus / CAR_WHEEL_RF, RM, RB, LF, LM, LB) on N-S lane x = -1.4f
            const float busCycle = fmodf(world.trafficTime * 2.2f + 18.0f, 112.0f);
            const float busDir   = busCycle < 56.0f ? -1.0f : 1.0f;
            const float busZ     = busCycle < 56.0f ? (28.0f - busCycle) : (-28.0f + (busCycle - 56.0f));
            const Vec3  busPos{-1.4f, 0.0f, busZ};
            const float bch = busDir, bsh = 0.0f;
            auto bLoc = [&](float x, float y, float z) { return busPos + Vec3{x*bch + z*bsh, y, -x*bsh + z*bch}; };
            auto bBox = [&](float w0, float w1, float y0, float y1, float z0, float z1, Colour col) {
                Vec3 a=bLoc(-w0,y0,z0), b=bLoc(w0,y0,z0), c=bLoc(w1,y0,z1), d=bLoc(-w1,y0,z1);
                Vec3 e=bLoc(-w0,y1,z0), f=bLoc(w0,y1,z0), g=bLoc(w1,y1,z1), h=bLoc(-w1,y1,z1);
                addQuad(a,b,f,e,col,ObjectTraffic); addQuad(b,c,g,f,col,ObjectTraffic);
                addQuad(c,d,h,g,col,ObjectTraffic); addQuad(d,a,e,h,col,ObjectTraffic);
                addQuad(e,f,g,h,col,ObjectTraffic);
            };
            addBlobShadow(busPos, 0.78f);
            bBox(0.76f, 0.76f, 0.24f, 1.62f, -1.85f, 1.85f, Colour{195, 128, 52, 255});
            bBox(0.785f, 0.785f, 0.60f, 0.76f, -1.84f, 1.84f, Colour{85, 215, 245, 255});  // gold-cyan transit stripe
            bBox(0.68f, 0.68f, 1.62f, 1.74f, -1.55f, 1.45f, Colour{215, 220, 230, 255});  // rooftop AC pod
            // LW51: 6-wheel City Bus (CAR_WHEEL_RF, RM, RB, LF, LM, LB) + bumpers + headlights + road beams
            bBox(0.80f, 0.78f, 0.18f, 0.36f,  1.80f,  1.92f, Colour{46, 48, 56, 255});
            bBox(0.80f, 0.80f, 0.18f, 0.36f, -1.92f, -1.80f, Colour{46, 48, 56, 255});
            for (float hx : {-0.56f, 0.40f}) {
                addQuad(bLoc(hx, 0.40f, 1.88f), bLoc(hx + 0.16f, 0.40f, 1.88f),
                        bLoc(hx + 0.16f, 0.52f, 1.88f), bLoc(hx, 0.52f, 1.88f), Colour{195, 245, 255, 255}, ObjectTraffic);
                addTriangle(bLoc(hx + 0.08f, 0.019f, 1.90f), bLoc(hx - 0.30f, 0.019f, 3.45f),
                            bLoc(hx + 0.46f, 0.019f, 3.45f), Colour{72, 96, 118, 255}, ObjectGround);
            }
            addQuad(bLoc(-0.68f, 0.42f, -1.88f), bLoc(0.68f, 0.42f, -1.88f),
                    bLoc( 0.68f, 0.54f, -1.88f), bLoc(-0.68f, 0.54f, -1.88f), Colour{55, 65, 250, 255}, ObjectTraffic);
            const float bWheelPhase = world.trafficTime * (busDir > 0 ? 6.5f : -6.5f);
            for (float wx : {-0.80f, 0.80f}) for (float wz : {-1.28f, -0.62f, 1.18f}) {
                for (int wi = 0; wi < 8; ++wi) {
                    const float wa = 6.2831853f * wi / 8.0f + bWheelPhase;
                    const float wb = 6.2831853f * (wi + 1) / 8.0f + bWheelPhase;
                    addTriangle(bLoc(wx, 0.28f, wz),
                                bLoc(wx, 0.28f + sinf(wa)*0.28f, wz + cosf(wa)*0.28f),
                                bLoc(wx, 0.28f + sinf(wb)*0.28f, wz + cosf(wb)*0.28f),
                                (wi & 1) ? Colour{34, 38, 44, 255} : Colour{165, 175, 190, 255}, ObjectTraffic);
                }
            }
            // Front panoramic windshield + rear window + illuminated route display
            addQuad(bLoc(-0.68f, 0.82f, 1.865f), bLoc(0.68f, 0.82f, 1.865f),
                    bLoc( 0.64f, 1.38f, 1.865f), bLoc(-0.64f, 1.38f, 1.865f), Colour{215, 190, 120, 255}, ObjectTraffic);
            addQuad(bLoc(-0.48f, 1.42f, 1.870f), bLoc(0.48f, 1.42f, 1.870f),
                    bLoc( 0.48f, 1.58f, 1.870f), bLoc(-0.48f, 1.58f, 1.870f), Colour{95, 235, 255, 255}, ObjectTraffic);
            // 5 individual passenger window bays on left & right flanks + folding glass boarding door
            for (float sx : {-0.785f, 0.785f}) {
                for (int wb = 0; wb < 5; ++wb) {
                    const float wz0 = -1.62f + wb * 0.64f, wz1 = wz0 + 0.48f;
                    addQuad(bLoc(sx, 0.86f, wz0), bLoc(sx, 0.86f, wz1),
                            bLoc(sx, 1.38f, wz1), bLoc(sx, 1.38f, wz0), Colour{215, 190, 120, 255}, ObjectTraffic);
                }
                addQuad(bLoc(sx, 0.28f, 1.32f), bLoc(sx, 0.28f, 1.72f),
                        bLoc(sx, 1.36f, 1.72f), bLoc(sx, 1.36f, 1.32f), Colour{48, 56, 68, 255}, ObjectTraffic);
            }
        }
    }
    // re3 CObject pickups: bobbing diamonds with a soft glow ring.
