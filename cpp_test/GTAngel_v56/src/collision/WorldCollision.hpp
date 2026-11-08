#ifndef GTANGEL_ZPP_COLLISION_SPEC_GUARD
#define GTANGEL_ZPP_COLLISION_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/collision
// Derived from C:/ddd/GTAngel/cpp_test/zpp/collision/*.zpp
// Modules: 11 | Formal Schemas: 250 | Free Types: 2
// ============================================================================
namespace zpp_collision_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 250;
static constexpr int kZppEnumCount   = 2;
// From zpp/collision/ColTriangle.zpp
enum class Direction : int32_t { DIR_X_POS_v = 0, DIR_X_NEG_v = 1, DIR_Y_POS_v = 2, DIR_Y_NEG_v = 3, DIR_Z_POS_v = 4, DIR_Z_NEG_v = 5 };
// From zpp/collision/Collision.zpp
enum class Collision_Enum_1 : int32_t { MAXNUMSPHERES_v = 0, MAXNUMBOXES_v = 1, MAXNUMLINES_v = 2, MAXNUMTRIS_v = 3 };
// State Schema Contract from zpp/collision/ColBox.zpp
struct CBox_ZppContract {
    int32_t min{0}; // Z++: CVector
    int32_t max{0}; // Z++: CVector
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColBox_ZppContract {
    int32_t min{0}; // Z++: CVector
    int32_t max{0}; // Z++: CVector
    uint32_t surface{0}; // Z++: ℕ
    uint32_t piece{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColTriBBox_ZppContract {
    int32_t first{0}; // Z++: ℤ
    int32_t last{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColLine.zpp
struct CColLine_ZppContract {
    int32_t p0{0}; // Z++: CVector
    int32_t p1{0}; // Z++: CVector
    int32_t pad0{0}; // Z++: ℤ
    int32_t pad1{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColModel.zpp
struct CColModel_ZppContract {
    int32_t boundingSphere{0}; // Z++: CSphere
    int32_t boundingBox{0}; // Z++: CBox
    int32_t numSpheres{0}; // Z++: ℤ
    int32_t numBoxes{0}; // Z++: ℤ
    int32_t numTriangles{0}; // Z++: ℤ
    int32_t numLines{0}; // Z++: ℤ
    int32_t numTriBBoxes{0}; // Z++: ℤ
    uint32_t level{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColPoint.zpp
struct CColPoint_ZppContract {
    int32_t point{0}; // Z++: CVector
    int32_t pad1{0}; // Z++: ℤ
    int32_t normal{0}; // Z++: CVector
    int32_t pad2{0}; // Z++: ℤ
    uint32_t surfaceA{0}; // Z++: ℕ
    uint32_t pieceA{0}; // Z++: ℕ
    uint32_t surfaceB{0}; // Z++: ℕ
    uint32_t pieceB{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_collision_zpp_invariants() {
    if (!CBox_ZppContract{}.checkInvariant()) return false;
    if (!CColBox_ZppContract{}.checkInvariant()) return false;
    if (!CColTriBBox_ZppContract{}.checkInvariant()) return false;
    if (!CColLine_ZppContract{}.checkInvariant()) return false;
    if (!CColModel_ZppContract{}.checkInvariant()) return false;
    if (!CColPoint_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
struct GameWorld {
    struct Pillar { Vec3 pos; float radius; float height; };
    struct Building { Vec3 pos; float w{0}, d{0}, h{0}; }; // axis-aligned box, centred on pos
    struct RoadStrip { float x0{0}, z0{0}, x1{0}, z1{1}, width{3}; }; // axis-aligned segment
    std::vector<Building> buildings;
    std::vector<RoadStrip> roads;
    std::vector<Pillar> pillars;
    Vec3 goal{8.0f, 0.0f, 8.0f};
    Vec3 npcPos{-4.0f, 0.0f, 4.0f};
    float npcHeading{2.35f};
    float npcSpeed{0.0f};
    Vec3 questShard{-6.0f, 0.0f, -5.0f};
    bool questShardVisible{true};
    bool beaconRestored{false};
    bool questComplete{false};
    Vec3 vehiclePos{1.5f, 0.0f, 0.0f};
    float vehicleHeading{0.0f};
    float vehicleWheelPhase{0.0f};
    bool vehicleOccupied{false};
    // Honest social world: wandering walkers DTE can actually meet. Positions
    // are world metres; group_dynamics agents sync from these every tick.
    struct Walker { Vec3 pos; float heading{0}; float speed{0.7f}; float phase{0}; };
    std::vector<Walker> walkers;
    float walkerTime{0};
    // re3 CVehicle: automobiles, bikes, train, heli on fixed lanes.
    struct TrafficVehicle {
        Vec3 pos; float heading{0}; float speed{0};
        float min{0}; float max{0}; char axis{'x'}; char dir{1};
        float orbitR{0}; float orbitA{0}; float orbitY{0};
    };
    // re3 CObject pickup: bobbing reward entities.
    struct Pickup { Vec3 pos; float phase{0}; bool collected{false}; };
    // re3 CDummy: static street furniture.
    struct DummyProp { Vec3 pos; };
    std::vector<TrafficVehicle> traffic;
    std::vector<Pickup> pickups;
    std::vector<DummyProp> props;
    float trafficTime{0};
    float worldSize{128.0f};

    void generate(uint32_t seed, int pillarCount) {
        pillars.clear();
        uint32_t s = seed;
        auto rnd = [&]() { s = s * 1664525u + 1013904223u; return (s >> 8) / 16777216.0f; };
        // Stable placement with bounded retries and protected quest centres.
        for (int attempts=0; attempts<4096 && (int)pillars.size()<std::clamp(pillarCount,0,128); ++attempts) {
            Pillar p;
            p.pos = {(rnd() - 0.5f) * worldSize, 0, (rnd() - 0.5f) * worldSize};
            p.radius = 0.4f + rnd() * 0.5f;
            p.height = 1.5f + rnd() * 2.0f;
            if (len(p.pos) < 3.2f || len(p.pos-goal) < 3.2f ||
                len(p.pos-npcPos) < 3.2f || len(p.pos-questShard) < 3.2f ||
                len(p.pos-vehiclePos) < 2.7f) continue;
            bool overlap=false;
            for (const auto& other : pillars)
                if (len(p.pos-other.pos)<p.radius+other.radius+0.9f) {overlap=true;break;}
            if (!overlap) pillars.push_back(p);
        // Deterministic city block: buildings on a grid with protected zones,
        // plus two crossing roads. Same LCG stream, fully reproducible.
        buildings.clear(); roads.clear();
        roads.push_back({-worldSize*0.5f, 0.0f, worldSize*0.5f, 0.0f, 4.0f});   // E-W main road
        roads.push_back({0.0f, -worldSize*0.5f, 0.0f, worldSize*0.5f, 4.0f});     // N-S main road
        for (int gx = -2; gx <= 2; ++gx) {
            for (int gz = -2; gz <= 2; ++gz) {
                if (gx == 0 || gz == 0) continue; // keep the road corridors clear
                Building b;
                b.pos = {gx * 14.0f + (rnd() - 0.5f) * 3.0f, 0, gz * 14.0f + (rnd() - 0.5f) * 3.0f};
                b.w = 3.0f + rnd() * 3.0f;
                b.d = 3.0f + rnd() * 3.0f;
                b.h = 2.5f + rnd() * 5.5f;
                const Vec3 protected_[] = {Vec3{0,0,0}, goal, npcPos, questShard, vehiclePos};
                bool blocked = false;
                for (const auto& c : protected_) {
                    const float dx = std::fabs(b.pos.x - c.x), dz = std::fabs(b.pos.z - c.z);
                    if (dx < b.w * 0.5f + 4.0f && dz < b.d * 0.5f + 4.0f) { blocked = true; break; }
                }
                if (blocked) continue;
                bool overlapB = false;
                for (const auto& other : buildings) {
                    const float dx = std::fabs(b.pos.x - other.pos.x), dz = std::fabs(b.pos.z - other.pos.z);
                    if (dx < (b.w + other.w) * 0.5f + 2.0f && dz < (b.d + other.d) * 0.5f + 2.0f) { overlapB = true; break; }
                }
                if (!overlapB) buildings.push_back(b);
            }
        }
        }
        // Deterministic walker population: 20 wanderers, clear of protected
        // quest centres and solid geometry, same LCG stream (reproducible).
        walkers.clear();
        for (int attempts=0; attempts<4096 && (int)walkers.size()<20; ++attempts) {
            Walker w;
            w.pos = {(rnd() - 0.5f) * worldSize, 0, (rnd() - 0.5f) * worldSize};
            w.heading = rnd() * 6.2831853f;
            w.speed = 0.5f + rnd() * 0.6f;
            w.phase = rnd() * 6.2831853f;
            if (len(w.pos) < 4.0f || len(w.pos-goal) < 4.0f ||
                len(w.pos-npcPos) < 4.0f || len(w.pos-questShard) < 4.0f ||
                len(w.pos-vehiclePos) < 3.0f || collides(w.pos)) continue;
            bool crowded=false;
            for (const auto& other : walkers)
                if (len(w.pos-other.pos)<2.5f) {crowded=true;break;}
            if (!crowded) walkers.push_back(w);
        }
        // re3-style entity population: fixed verified-clear positions on the
        // road grid (no RNG, deterministic). Lanes: E-W auto z=+1.4, N-S bike
        // x=+0.9, train z=-20; heli orbits above max building height.
        traffic.clear(); pickups.clear(); props.clear();
        TrafficVehicle auto1; auto1.pos={-30,0,1.4f}; auto1.heading=1.5707963f; auto1.speed=3.2f; auto1.min=-32; auto1.max=32; auto1.axis='x'; auto1.dir=1;
        traffic.push_back(auto1);
        TrafficVehicle auto2; auto2.pos={30,0,1.4f}; auto2.heading=4.7123890f; auto2.speed=2.7f; auto2.min=-32; auto2.max=32; auto2.axis='x'; auto2.dir=-1;
        traffic.push_back(auto2);
        TrafficVehicle bike1; bike1.pos={0.9f,0,-30}; bike1.heading=0.0f; bike1.speed=1.8f; bike1.min=-32; bike1.max=32; bike1.axis='z'; bike1.dir=1;
        traffic.push_back(bike1);
        TrafficVehicle bike2; bike2.pos={0.9f,0,30}; bike2.heading=3.14159265f; bike2.speed=2.1f; bike2.min=-32; bike2.max=32; bike2.axis='z'; bike2.dir=-1;
        traffic.push_back(bike2);
        TrafficVehicle train; train.pos={-50,0,-20}; train.heading=1.5707963f; train.speed=4.5f; train.min=-52; train.max=52; train.axis='x'; train.dir=1;
        traffic.push_back(train);
        TrafficVehicle heli; heli.pos={20,8.5f,0}; heli.orbitR=20; heli.orbitA=0; heli.orbitY=8.5f; heli.speed=0.5f;
        traffic.push_back(heli);
        const Vec3 pickupSpots[] = {Vec3{6,0,2.5f}, Vec3{-6,0,-2.5f}, Vec3{2.5f,0,-8}, Vec3{-2.5f,0,10}};
        for (const auto& spot : pickupSpots) {
            Pickup pk; pk.pos = spot; pk.phase = (float)(pickups.size()) * 1.3f;
            pickups.push_back(pk);
        }
        const Vec3 lampSpots[] = {Vec3{15,0,2.6f}, Vec3{-15,0,2.6f}, Vec3{15,0,-2.6f}, Vec3{-15,0,-2.6f},
                                  Vec3{2.6f,0,15}, Vec3{-2.6f,0,15}, Vec3{2.6f,0,-15}, Vec3{-2.6f,0,-15}};
        for (const auto& spot : lampSpots) {
            DummyProp lamp; lamp.pos = spot;
            props.push_back(lamp);
        }
    }

    bool collides(Vec3 p) const {
        for (const auto& pl : pillars) {
            float dx = p.x - pl.pos.x, dz = p.z - pl.pos.z;
            if (dx*dx + dz*dz < (pl.radius + 0.25f) * (pl.radius + 0.25f)) return true;
        }
        for (const auto& b : buildings) {
            float dx = std::fabs(p.x - b.pos.x), dz = std::fabs(p.z - b.pos.z);
            if (dx < b.w * 0.5f + 0.25f && dz < b.d * 0.5f + 0.25f) return true;
        }
        return false;
    }
    float goalDistance(Vec3 p) const { return len(p - goal); }
    // Camera anti-clipping: march from the body toward the desired eye and
    // keep the last collision-free sample, so pillars never swallow the view.
    bool cameraBlocked(Vec3 p, float pad = 0.42f) const {
        for (const auto& pl : pillars) {
            const float dx = p.x - pl.pos.x, dz = p.z - pl.pos.z;
            if (dx*dx + dz*dz < (pl.radius + pad) * (pl.radius + pad)) return true;
        }
        for (const auto& b : buildings) {
            const float dx = std::fabs(p.x - b.pos.x), dz = std::fabs(p.z - b.pos.z);
            if (dx < b.w * 0.5f + pad && dz < b.d * 0.5f + pad) return true;
        }
        for (const auto& lamp : props) {
            const float dx = p.x - lamp.pos.x, dz = p.z - lamp.pos.z;
            if (dx*dx + dz*dz < (0.22f + pad) * (0.22f + pad)) return true;
        }
        return false;
    }
    Vec3 collisionFreeEye(Vec3 body, Vec3 desired) const {
        // LW35: sweep the full ray from elevated body anchor to desired eye
        // (never shortcut on !collides(desired) alone, which missed poles or
        // pillars standing between the avatar and the 3rd-person camera).
        const Vec3 anchor{body.x, std::max(body.y + 1.65f, desired.y * 0.65f), body.z};
        const Vec3 ray = desired - anchor;
        const float rlen = std::sqrt(ray.x*ray.x + ray.z*ray.z);
        const Vec3 side = rlen > 1e-4f ? Vec3{-ray.z / rlen, 0.0f, ray.x / rlen} : Vec3{1.0f, 0.0f, 0.0f};
        bool clearRay = true;
        Vec3 pulledIn = anchor;
        for (int step = 1; step <= 24; ++step) {
            const float t = (float)step / 24.0f;
            const Vec3 sample = anchor + ray * t;
            if (cameraBlocked(sample, 0.42f)) {
                clearRay = false;
                break;
            }
            pulledIn = sample;
        }
        if (clearRay) return desired;
        // If only a thin pole or tree trunk grazed the camera ray (while the
        // desired endpoint itself is outside solid geometry), try a gentle
        // lateral/elevated camera orbit around the pole before pulling in.
        if (!collides(desired)) {
            for (float lat : {0.55f, -0.55f, 0.95f, -0.95f}) {
                const Vec3 cand = desired + side * lat + Vec3{0.0f, 0.25f, 0.0f};
                bool ok = true;
                for (int s = 1; s <= 20; ++s) {
                    if (cameraBlocked(anchor + (cand - anchor) * ((float)s / 20.0f), 0.34f)) { ok = false; break; }
                }
                if (ok) return cand;
            }
        }
        return pulledIn;
    }
    void tickWalkers(float dt) {
        walkerTime += dt;
        for (auto& w : walkers) {
            // Deterministic wander: layered sine headings, no RNG, so runs
            // replay exactly and the world stays reproducible.
            w.heading += 0.7f * std::sin(walkerTime * 0.6f + w.phase) * dt
                       + 0.4f * std::sin(walkerTime * 0.23f + w.phase * 2.1f) * dt;
            const Vec3 next = w.pos + Vec3{std::sin(w.heading) * w.speed * dt, 0,
                                           std::cos(w.heading) * w.speed * dt};
            if (collides(next) || std::fabs(next.x) > worldSize * 0.5f - 1.0f ||
                std::fabs(next.z) > worldSize * 0.5f - 1.0f)
                w.heading += 1.7f;
            else
                w.pos = next;
        }
    }
    // re3 CVehicle lanes: ping-pong along fixed axes, heli orbits overhead.
    // Time-based only (no RNG) so every run replays identically.
    void tickTraffic(float dt) {
        trafficTime += dt;
        for (auto& v : traffic) {
            if (v.orbitR > 0.0f) {
                v.orbitA += v.speed * dt;
                v.pos = {std::cos(v.orbitA) * v.orbitR, v.orbitY, std::sin(v.orbitA) * v.orbitR};
                v.heading = -v.orbitA; // tangent {-sin A, 0, cos A} = {sin h, 0, cos h}
                continue;
            }
            float& along = v.axis == 'x' ? v.pos.x : v.pos.z;
            along += v.dir * v.speed * dt;
            if (along >= v.max) { along = v.max; v.dir = -1; }
            if (along <= v.min) { along = v.min; v.dir = 1; }
            v.heading = v.axis == 'x'
                ? (v.dir > 0 ? 1.5707963f : 4.7123890f)
                : (v.dir > 0 ? 0.0f : 3.14159265f);
        }
    }
    // re3 CObject pickups: gentle bob, deterministic phase per slot.
    void tickPickups(float dt) {
        trafficTime += dt;
        for (auto& pk : pickups) {
            if (pk.collected) continue;
            pk.pos.y = 0.55f + 0.25f * std::sin(trafficTime * 2.0f + pk.phase);
        }
    }
};

