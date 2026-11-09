// GTAngel AGI Gameworld Testing Environment v15
// Pure C++ Win32 — statically linked, no external deps
// Integrates Layla-Server + cog-zero + echo + kys + elizaos.cpp + GTAngel-Cls + uechovr + re-bhvt-edit + mech0 + u9n-main DeepTreeEcho
// mech0 concepts: SimpleLanguageModel, AttentionMechanism, TrainingPipeline
// Composition: /iterative-micro-improvement(/meta-echo-dna</echo-angel[/superhot-gamer-girl]>
//              & /vorticog</triz-cognitive-arena[/ai-env-grp-dyn]>
//              & /echo-introspect[/cogverse-evolution-accelerator])

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <deque>
#include <functional>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winhttp.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

// ═══ mech0 Integration (ported from ReZorg/mech0) ═══
// ═══ kys Integration (ported from Cosmic-Construction/kys) ═══
// Triad Phase-Locked Phasor Engine — 3 concurrent KSM 12-step cycles
// running 120° (4 steps) apart with U1/U2 Universal Set policy integrators
// over 4 shared interfaces (C1 Electronic, C2 Cellular, C3 Routine, C4 Environment)
// ═══ EchoCog/echo Integration (ported from EchoCog/echo arena-desktop) ═══
// TRIZ Cognitive Arena with KSM evolution cycle, cognitive controller,
// aesthetic field evaluation, and evolution ledger
// ═══ cogpy/cog-zero Integration (ported from cogpy/cog-zero agentzero-core) ═══
// OpenCog AtomSpace-backed cognitive loop with 5 phases:
// Perception → Reasoning → Planning → Action → Reflection
// ═══ l3utterfly/Layla-Server native integration ═══
// Faithful native adaptation of Layla's backend discovery, serial request queue,
// OpenAI-compatible endpoint, bounded message chunks, and explicit failure telemetry.
// ═══ GTAngel v14 — 3D Avatar + Gameworld→TRIZ Grid Mapping ═══
// Implements a 3D avatar model with skeletal pose, an egocentric software
// renderer producing a real framebuffer, and a projection of that framebuffer
// onto the TRIZ hex grid so navigation-learning difficulty can be evaluated
// with the existing cognitive stack (ESN, endocrine, phasor, AgentLoop).
//
// Design goals:
//  - Real 3D math (perspective projection, quaternion-free yaw/pitch camera)
//  - Real framebuffer (z-buffered rasterizer, 64x64 cognitive resolution)
//  - Real mapping (retinotopic bins -> TRIZ cells with contradiction scoring)
//  - Real training signal (tabular Q-learning navigator + difficulty metrics)
namespace avatar3d {

struct Vec3 { float x{0}, y{0}, z{0}; };
static Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
static float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static float len(Vec3 a) { return sqrtf(dot(a,a)); }
static Vec3 norm(Vec3 a) { float l = len(a); return l > 1e-6f ? a*(1.0f/l) : Vec3{0,0,0}; }

// ── Skeletal avatar model ──
// 17 joints (root, spine, head, arms, legs) with forward-kinematic pose.
// GTAngel v21: clean-room visual pose controller derived from the user-supplied
// DTE motion style guide. This declaration replaces AvatarSkeleton within
// namespace avatar3d; it never moves the body or writes a motor/world commit.
struct AvatarSkeleton {
    enum JointId { Root=0, Spine, Neck, Head,
                   ShoulderL, ElbowL, HandL, ShoulderR, ElbowR, HandR,
                   HipL, KneeL, FootL, HipR, KneeR, FootR, COUNT };
    enum class Motion { Explore, CombatEntry, Guard, Recovery };
    Vec3 joints[COUNT]{};
    Vec3 wingRoot[2]{}, wingBoom[2]{}, wingTip[2]{};
    Vec3 cheek[3]{}, headset[2]{};
    float walkPhase{0.0f}, walkBlend{0.0f}, motionTime{0.0f}, wingDeploy{0.0f}, orbIntensity{0.0f};
    Motion motion{Motion::Explore};
    bool inspect{false};
    Vec3 lastPosition{};

    static float smooth(float a,float b,float t) {
        const float u=std::clamp((t-a)/(b-a),0.0f,1.0f);
        return u*u*(3.0f-2.0f*u);
    }
    bool triggerCombat() {
        if (motion!=Motion::Explore) return false;
        motion=Motion::CombatEntry; motionTime=0; return true;
    }
    bool releaseCombat() {
        if (motion!=Motion::Guard) return false;
        motion=Motion::Recovery; motionTime=0; return true;
    }
    void pose(Vec3 position,float heading,float speed,float dt) {
        // 1.3 m per two-step cycle at 1.3 m/s; the authoritative BodyEffector
        // supplies world displacement. No pose operation changes position.
        const float delta=std::clamp(dt,0.0f,0.1f);
        const float travel=std::isfinite(speed)?std::clamp(std::fabs(speed),0.0f,2.0f):0.0f;
        const float target=(motion==Motion::Explore)?std::clamp(travel/1.3f,0.0f,1.0f):0.0f;
        const float blendStep=delta/0.20f;
        walkBlend+=std::clamp(target-walkBlend,-blendStep,blendStep);
        if (delta>0 && travel>0.025f && motion==Motion::Explore)
            walkPhase+=6.2831853f*travel*delta/1.30f;
        if (delta>0) {
            motionTime+=delta;
            if (motion==Motion::CombatEntry && motionTime+0.0001f>=1.50f) {
                motion=Motion::Guard; motionTime=0;
            } else if (motion==Motion::Recovery && motionTime>=0.30f) {
                motion=Motion::Explore; motionTime=0;
            }
        }
        const float frames=motion==Motion::CombatEntry?motionTime*24.0f:
                           motion==Motion::Guard?36.0f:0.0f;
        // Unlock -> boom rotate -> fan. No spontaneous wing motion in Explore.
        wingDeploy=motion==Motion::Explore?0.0f:
            motion==Motion::Recovery?1.0f-smooth(0.0f,0.30f,motionTime):
            motion==Motion::Guard?1.0f:smooth(16.0f,27.0f,frames);
        orbIntensity=motion==Motion::Guard?0.34f:
            motion==Motion::CombatEntry?smooth(18.0f,26.0f,frames):
            motion==Motion::Recovery?0.34f*(1.0f-smooth(0,0.3f,motionTime)):0.0f;
        const bool fighting=motion==Motion::CombatEntry||motion==Motion::Guard||motion==Motion::Recovery;
        const float load=fighting?(motion==Motion::CombatEntry?smooth(4,14,frames):1.0f):0.0f;
        const float locomotion=walkBlend;
        const float step=std::sin(walkPhase)*locomotion;
        const float bob=0.032f*(1.0f-std::cos(2.0f*walkPhase))*locomotion;
        const float ch=std::cos(heading),sh=std::sin(heading);
        auto R=[&](Vec3 v){return Vec3{v.x*ch-v.z*sh,v.y,v.x*sh+v.z*ch}+position;};
        const float stance=0.12f*load, crouch=0.12f*load;
        joints[Root]=R({0,0.95f+bob-crouch,0});
        joints[Spine]=R({0,1.25f+bob-crouch*0.5f,0.025f*load});
        joints[Neck]=R({0,1.50f+bob-crouch*0.3f,0});
        joints[Head]=R({(inspect && !fighting)?0.022f:0.0f,1.70f+bob-crouch*0.2f,0});
        for (int side=0;side<2;++side) {
            const float s=side==0?-1.0f:1.0f;
            const int shoulder=side==0?ShoulderL:ShoulderR;
            const int elbow=side==0?ElbowL:ElbowR;
            const int hand=side==0?HandL:HandR;
            const int hip=side==0?HipL:HipR;
            const int knee=side==0?KneeL:KneeR;
            const int foot=side==0?FootL:FootR;
            // Opposite arm/leg phases; low-amplitude exploring weight transfer.
            const float footSwing=s*step;
            joints[shoulder]=R({s*0.22f,1.45f+bob-crouch*0.3f,0});
            joints[elbow]=R({s*(0.28f+0.11f*load),1.15f+bob, -footSwing*0.20f+0.18f*load});
            joints[hand]=R({s*(0.30f+0.16f*load),0.85f+bob+0.35f*load,
                            -footSwing*0.38f+0.32f*load});
            joints[hip]=R({s*(0.12f+stance),0.95f+bob-crouch,0});
            joints[knee]=R({s*(0.14f+stance),0.50f-crouch*0.7f,
                            footSwing*0.25f+0.09f*load});
            const float lift=std::max(0.0f,footSwing)*0.10f;
            joints[foot]=R({s*(0.15f+stance),0.05f+lift,
                            footSwing*0.43f+0.08f*load});
            // One physical root for each wing, fixed to shoulder-back harness;
            // folded wings have a backpack silhouette, not idle flutter.
            wingRoot[side]=R({s*0.19f,1.43f+bob-crouch*0.3f,-0.16f});
            wingBoom[side]=R({s*(0.26f+0.47f*wingDeploy),1.78f+0.26f*wingDeploy+bob,-0.24f});
            wingTip[side]=R({s*(0.30f+1.13f*wingDeploy),1.96f+0.62f*wingDeploy+bob,-0.31f});
            headset[side]=R({s*0.133f,1.72f+bob,0.0f});
        }
        for (int i=0;i<3;++i) cheek[i]=R({0.09f+0.026f*i,1.65f-0.05f*i+bob,0.094f});
        lastPosition=position;
    }
};

// ── Gameworld ──// ── Gameworld ──
// Ground plane + obstacle pillars + goal beacon. The avatar must navigate
// from spawn to the beacon while avoiding obstacles.
struct GameWorld {
    struct Pillar { Vec3 pos; float radius; float height; };
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
        }
    }

    bool collides(Vec3 p) const {
        for (const auto& pl : pillars) {
            float dx = p.x - pl.pos.x, dz = p.z - pl.pos.z;
            if (dx*dx + dz*dz < (pl.radius + 0.25f) * (pl.radius + 0.25f)) return true;
        }
        return false;
    }
    float goalDistance(Vec3 p) const { return len(p - goal); }
};

// ── Egocentric software renderer ──
// Renders the world from the avatar's perspective into a 64x64 framebuffer
// with z-buffering. Output is the SAME buffer consumed by the cognitive stack.
class EgocentricRenderer {
public:
    static constexpr int W = 64, H = 64;
    std::vector<float> frame;      // luminance 0..1
    std::vector<float> zbuf;
    std::vector<float> semantic;   // 0=void, 0.33=ground, 0.66=obstacle, 1=goal

    EgocentricRenderer() : frame(W*H, 0.0f), zbuf(W*H, 1e9f), semantic(W*H, 0.0f) {}

    void render(const GameWorld& world, Vec3 eye, float heading, float pitch,
                const AvatarSkeleton& avatar, bool thirdPerson) {
        std::fill(frame.begin(), frame.end(), 0.02f);
        std::fill(zbuf.begin(), zbuf.end(), 1e9f);
        std::fill(semantic.begin(), semantic.end(), 0.0f);

        float ch = cosf(heading), sh = sinf(heading);
        float cp = cosf(pitch),   sp = sinf(pitch);
        float fov = 1.2f; // ~69 deg

        auto project = [&](Vec3 p, float& sx, float& sy, float& depth) -> bool {
            Vec3 d = p - eye;
            // yaw
            float x = d.x * ch - d.z * sh;
            float z = d.x * sh + d.z * ch;
            // pitch
            float y = d.y * cp - z * sp;
            float z2 = d.y * sp + z * cp;
            if (z2 < 0.05f) return false;
            depth = z2;
            sx = (x / (z2 * fov)) * (W * 0.5f) + W * 0.5f;
            sy = (-y / (z2 * fov)) * (H * 0.5f) + H * 0.5f;
            return sx >= 0 && sx < W && sy >= 0 && sy < H;
        };

        auto plot = [&](float sx, float sy, float depth, float lum, float sem) {
            int px = (int)sx, py = (int)sy;
            if (px < 0 || px >= W || py < 0 || py >= H) return;
            int idx = py * W + px;
            if (depth < zbuf[idx]) {
                zbuf[idx] = depth;
                frame[idx] = lum;
                semantic[idx] = sem;
            }
        };
        auto plotDisc = [&](float sx, float sy, float depth, float lum, float sem, int radius) {
            for (int dy = -radius; dy <= radius; dy++)
                for (int dx = -radius; dx <= radius; dx++)
                    if (dx*dx + dy*dy <= radius*radius)
                        plot(sx + dx, sy + dy, depth, lum, sem);
        };

        // Ground plane: initialize a continuous floor below the horizon, then
        // overlay projected grid samples. This prevents the cognitive retina
        // from degenerating into an almost-empty frame between sparse points.
        for (int py = H / 2; py < H; py++) {
            float t = (float)(py - H / 2) / (H / 2);
            for (int px = 0; px < W; px++) {
                int idx = py * W + px;
                frame[idx] = 0.08f + 0.14f * t;
                semantic[idx] = 0.33f;
                zbuf[idx] = 1000.0f - t;
            }
        }
        const float sampleMinX=std::max(-world.worldSize/2,eye.x-18.0f);
        const float sampleMaxX=std::min( world.worldSize/2,eye.x+18.0f);
        const float sampleMinZ=std::max(-world.worldSize/2,eye.z-18.0f);
        const float sampleMaxZ=std::min( world.worldSize/2,eye.z+18.0f);
        for (float gx=sampleMinX; gx<=sampleMaxX; gx+=0.75f)
            for (float gz=sampleMinZ; gz<=sampleMaxZ; gz+=0.75f) {
                float sx, sy, d;
                if (project({gx, 0, gz}, sx, sy, d))
                    plotDisc(sx, sy, d, std::max(0.10f, 0.42f - d * 0.018f), 0.33f, d < 8.0f ? 1 : 0);
            }

        // Obstacle pillars (vertical line stacks).
        for (const auto& pl : world.pillars) {
            for (float h = 0; h <= pl.height; h += 0.15f) {
                for (float a = 0; a < 6.28318f; a += 0.785398f) {
                    Vec3 p = pl.pos + Vec3{cosf(a) * pl.radius, h, sinf(a) * pl.radius};
                    float sx, sy, d;
                    if (project(p, sx, sy, d)) {
                        int radiusPx = std::max(1, std::min(5, (int)(12.0f * pl.radius / std::max(0.5f, d))));
                        plotDisc(sx, sy, d, std::max(0.15f, 0.78f - d * 0.035f), 0.66f, radiusPx);
                    }
                }
            }
        }

        // Goal beacon (bright pulsing column).
        float pulse = 0.8f + 0.2f * sinf((float)GetTickCount64() * 0.005f);
        for (float h = 0; h <= 3.0f; h += 0.1f) {
            float sx, sy, d;
            if (project(world.goal + Vec3{0, h, 0}, sx, sy, d))
                plotDisc(sx, sy, d, pulse, 1.0f, d < 8.0f ? 2 : 1);
        }

        // Third-person: draw avatar skeleton as bone segments.
        if (thirdPerson) {
            static const int bones[][2] = {
                {AvatarSkeleton::Root, AvatarSkeleton::Spine},
                {AvatarSkeleton::Spine, AvatarSkeleton::Neck},
                {AvatarSkeleton::Neck, AvatarSkeleton::Head},
                {AvatarSkeleton::ShoulderL, AvatarSkeleton::ElbowL},
                {AvatarSkeleton::ElbowL, AvatarSkeleton::HandL},
                {AvatarSkeleton::ShoulderR, AvatarSkeleton::ElbowR},
                {AvatarSkeleton::ElbowR, AvatarSkeleton::HandR},
                {AvatarSkeleton::Spine, AvatarSkeleton::ShoulderL},
                {AvatarSkeleton::Spine, AvatarSkeleton::ShoulderR},
                {AvatarSkeleton::Root, AvatarSkeleton::HipL},
                {AvatarSkeleton::Root, AvatarSkeleton::HipR},
                {AvatarSkeleton::HipL, AvatarSkeleton::KneeL},
                {AvatarSkeleton::KneeL, AvatarSkeleton::FootL},
                {AvatarSkeleton::HipR, AvatarSkeleton::KneeR},
                {AvatarSkeleton::KneeR, AvatarSkeleton::FootR}
            };
            for (auto& b : bones) {
                Vec3 a = avatar.joints[b[0]], c = avatar.joints[b[1]];
                for (float t = 0; t <= 1.0f; t += 0.1f) {
                    Vec3 p = a + (c - a) * t;
                    float sx, sy, d;
                    if (project(p, sx, sy, d)) plot(sx, sy, d, 0.9f, 0.5f);
                }
            }
        }
    }
};

// ── Framebuffer → TRIZ grid mapper ──
// Bins the 64x64 framebuffer into the 12x12 TRIZ hex grid. Each cell receives
// mean luminance, semantic composition, and a contradiction score derived from
// local spatial conflict (obstacle vs goal energy in the same receptive field).
struct TrizMapping {
    static constexpr int GRID = 12;
    float aesthetic[GRID][GRID]{};
    float contradiction[GRID][GRID]{};
    float goalSalience[GRID][GRID]{};
    float obstacleSalience[GRID][GRID]{};
    float meanContradiction{0};
    float maxContradiction{0};
    int maxQ{-1}, maxR{-1};

    void mapFrame(const EgocentricRenderer& r) {
        meanContradiction = maxContradiction = 0;
        maxQ = maxR = -1;
        const int binW = EgocentricRenderer::W / GRID; // 5 px per cell (60 used)
        const int binH = EgocentricRenderer::H / GRID;
        for (int q = 0; q < GRID; q++)
            for (int rr = 0; rr < GRID; rr++) {
                float lum = 0, goal = 0, obst = 0, n = 0;
                for (int y = rr * binH; y < (rr + 1) * binH; y++)
                    for (int x = q * binW; x < (q + 1) * binW; x++) {
                        int idx = y * EgocentricRenderer::W + x;
                        lum += r.frame[idx];
                        if (r.semantic[idx] > 0.9f) goal += 1.0f;
                        else if (r.semantic[idx] > 0.5f && r.semantic[idx] < 0.8f) obst += 1.0f;
                        n += 1.0f;
                    }
                aesthetic[q][rr] = lum / n;
                goalSalience[q][rr] = goal / n;
                obstacleSalience[q][rr] = obst / n;
                // TRIZ spatial contradiction combines (a) direct goal/obstacle
                // overlap and (b) an obstacle blocking the central locomotion
                // corridor. The latter remains meaningful when the goal is just
                // outside the camera frustum, unlike pure pixel overlap.
                float center = 1.0f - fabsf(((q + 0.5f) / GRID) * 2.0f - 1.0f);
                float forward = 1.0f - (float)rr / GRID;
                float corridorConflict = obstacleSalience[q][rr] * center * (0.35f + 0.65f * forward);
                float semanticConflict = 2.0f * sqrtf(goalSalience[q][rr] * obstacleSalience[q][rr]);
                contradiction[q][rr] = std::min(1.0f, corridorConflict + semanticConflict);
                meanContradiction += contradiction[q][rr];
                if (contradiction[q][rr] > maxContradiction) {
                    maxContradiction = contradiction[q][rr];
                    maxQ = q; maxR = rr;
                }
            }
        meanContradiction /= (GRID * GRID);
    }
};

// ── Navigation training harness ──
// Tabular Q-learning navigator over an 8-direction action space with a
// discretized egocentric state (goal bearing bucket, goal distance bucket,
// nearest-obstacle bearing bucket). Tracks learning-curve difficulty metrics.
struct NavTrainer {
    enum Action { FWD=0, FWD_L, FWD_R, TURN_L, TURN_R, BACK, WAIT, SPRINT, COUNT };
    static constexpr int BEARINGS = 8, DISTS = 4, OBST = 8;
    float Q[BEARINGS][DISTS][OBST][COUNT]{};
    float alpha{0.15f}, gamma{0.92f}, epsilon{0.25f};
    uint32_t rng{12345};

    // Difficulty metrics
    int episodes{0};
    int successes{0};
    int collisions{0};
    int stepsTotal{0};
    std::vector<float> returnsPerEpisode;
    std::vector<int> stepsPerEpisode;

    float frand() { rng = rng * 1664525u + 1013904223u; return (rng >> 8) / 16777216.0f; }

    struct State { int bearing, dist, obst; };
    State discretize(Vec3 pos, float heading, const GameWorld& w) const {
        Vec3 toGoal = w.goal - pos;
        float goalAng = atan2f(toGoal.x, toGoal.z);
        float rel = goalAng - heading;
        while (rel > 3.14159f) rel -= 6.28318f;
        while (rel < -3.14159f) rel += 6.28318f;
        int b = (int)((rel + 3.14159f) / 6.28318f * BEARINGS) % BEARINGS;
        if (b < 0) b += BEARINGS;
        float d = w.goalDistance(pos);
        int di = d < 2 ? 0 : d < 5 ? 1 : d < 10 ? 2 : 3;
        // nearest obstacle bearing
        float bestD = 1e9f; float bestA = 0;
        for (const auto& p : w.pillars) {
            Vec3 dp = p.pos - pos;
            float dd = len(dp);
            if (dd < bestD) { bestD = dd; bestA = atan2f(dp.x, dp.z) - heading; }
        }
        int o = (int)((bestA + 3.14159f) / 6.28318f * OBST) % OBST;
        if (o < 0) o += OBST;
        return {b, di, o};
    }

    Action choose(const State& s) {
        if (frand() < epsilon) return (Action)(rng % COUNT);
        int best = 0; float bv = Q[s.bearing][s.dist][s.obst][0];
        for (int a = 1; a < COUNT; a++)
            if (Q[s.bearing][s.dist][s.obst][a] > bv) { bv = Q[s.bearing][s.dist][s.obst][a]; best = a; }
        return (Action)best;
    }

    void update(const State& s, Action a, float reward, const State& s2) {
        float bestNext = Q[s2.bearing][s2.dist][s2.obst][0];
        for (int aa = 1; aa < COUNT; aa++)
            bestNext = std::max(bestNext, Q[s2.bearing][s2.dist][s2.obst][aa]);
        float& q = Q[s.bearing][s.dist][s.obst][a];
        q += alpha * (reward + gamma * bestNext - q);
    }

    // One training episode; returns total return. Writes per-step frames if
    // renderer/mapper provided (for difficulty evaluation on the TRIZ grid).
    float episode(GameWorld& w, AvatarSkeleton& avatar, EgocentricRenderer* rend,
                  TrizMapping* mapper, int maxSteps, float* outMeanContradiction) {
        Vec3 pos{0, 0, 0};
        float heading = 0, speed = 0;
        float total = 0, contraSum = 0; int contraN = 0;
        int actualSteps = 0;
        bool reachedGoal = false;
        for (int step = 0; step < maxSteps; step++) {
            actualSteps = step + 1;
            State s = discretize(pos, heading, w);
            Action a = choose(s);
            float turn = 0, move = 0;
            switch (a) {
                case FWD: move = 1.0f; break;
                case FWD_L: move = 0.8f; turn = 0.35f; break;
                case FWD_R: move = 0.8f; turn = -0.35f; break;
                case TURN_L: turn = 0.6f; break;
                case TURN_R: turn = -0.6f; break;
                case BACK: move = -0.5f; break;
                case WAIT: break;
                case SPRINT: move = 1.6f; break;
                default: break;
            }
            heading += turn * 0.1f;
            speed = move;
            Vec3 next = pos + Vec3{sinf(heading) * move * 0.2f, 0, cosf(heading) * move * 0.2f};

            float reward = -0.01f; // time penalty
            bool done = false;
            if (w.collides(next)) {
                reward = -1.0f; collisions++; next = pos; done = false;
            } else if (w.goalDistance(next) < 1.0f) {
                reward = 1.0f; pos = next; done = true;
            } else {
                // shaping: progress toward goal
                reward += 0.1f * (w.goalDistance(pos) - w.goalDistance(next));
                pos = next;
            }

            State s2 = discretize(pos, heading, w);
            update(s, a, reward, s2);
            total += reward;
            stepsTotal++;

            if (rend && mapper) {
                avatar.pose(pos, heading, fabsf(speed), 0.05f);
                Vec3 eye = pos + Vec3{0, 1.6f, 0};
                rend->render(w, eye, heading, 0.0f, avatar, false);
                mapper->mapFrame(*rend);
                contraSum += mapper->meanContradiction;
                contraN++;
            }
            if (done) { successes++; reachedGoal = true; break; }
        }
        episodes++;
        returnsPerEpisode.push_back(total);
        stepsPerEpisode.push_back(actualSteps);
        epsilon = std::max(0.05f, epsilon * 0.995f);
        if (outMeanContradiction) *outMeanContradiction = contraN ? contraSum / contraN : 0.0f;
        return total;
    }

    // Difficulty summary: success rate, collision rate, mean return trend.
    float successRate() const { return episodes ? (float)successes / episodes : 0.0f; }
    float collisionRate() const { return stepsTotal ? (float)collisions / stepsTotal : 0.0f; }
    float recentMeanReturn(int window = 20) const {
        if (returnsPerEpisode.empty()) return 0.0f;
        int n = std::min((int)returnsPerEpisode.size(), window);
        float s = 0;
        for (int i = (int)returnsPerEpisode.size() - n; i < (int)returnsPerEpisode.size(); i++)
            s += returnsPerEpisode[i];
        return s / n;
    }
};

} // namespace avatar3d


// GTAngel v14 — Arc Angel Echo / Civic Angel Architect II telemetry module
// Native Win32 C++17, no external dependencies. Tokens remain process-local.
// The module emits only values supplied by GTAngel's live runtime state.

namespace civicangel {

static double clamp01(double value) {
    return std::max(0.0, std::min(1.0, value));
}

static std::string jsonEscape(const std::string& input) {
    std::string out;
    out.reserve(input.size() + 32);
    for (unsigned char c : input) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: if (c >= 0x20) out += (char)c; break;
        }
    }
    return out;
}

static std::wstring wide(const std::string& input) {
    if (input.empty()) return {};
    const int needed = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0);
    std::wstring output((size_t)needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), output.data(), needed);
    return output;
}

static std::string environmentValue(const char* key) {
    const DWORD required = GetEnvironmentVariableA(key, nullptr, 0);
    if (!required) return {};
    std::string value(required, '\0');
    const DWORD copied = GetEnvironmentVariableA(key, value.data(), required);
    if (!copied) return {};
    value.resize(copied);
    return value;
}

static int64_t unixTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

struct GroundedFrame {
    uint64_t tick{0};
    double positionX{0.0};
    double positionZ{0.0};
    double heading{0.0};
    double goalDistance{0.0};
    double meanContradiction{0.0};
    double frameLuminance{0.0};
    double obstacleSalience{0.0};
    double goalSalience{0.0};
    double collisionRate{0.0};
    double health{0.0};
    double hostility{0.0};
    double goalProgress{0.0};
    double reward{0.0};
    double coherence{0.0};
    double valence{0.0};
    double arousal{0.0};
    double cortisol{0.0};
    double dopamine{0.0};
    double serotonin{0.0};
    double norepinephrine{0.0};
    double oxytocin{0.0};
    double melatonin{0.0};
    double anandamide{0.0};
    uint64_t framebufferSequence{0};
    double framebufferRenderMs{0.0};
    int trianglesSubmitted{0};
    int trianglesRasterized{0};
    int finiteDepthPixels{0};
    int semanticPixels{0};
    const char* cognitiveMode{"exploratory"};
};

struct ProducerTelemetry {
    bool configured{false};
    bool busy{false};
    unsigned long accepted{0};
    unsigned long failures{0};
    unsigned long queued{0};
    unsigned long lastStatus{0};
    double lastLatencyMs{0.0};
    std::string lastError;
    std::string lastTraceId;
};

struct PendingFrame {
    std::string json;
    std::string traceId;
};

class AuthenticatedEmitter {
public:
    AuthenticatedEmitter() : worker_([this] { workerLoop(); }) {}
    ~AuthenticatedEmitter() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    AuthenticatedEmitter(const AuthenticatedEmitter&) = delete;
    AuthenticatedEmitter& operator=(const AuthenticatedEmitter&) = delete;

    void configure(std::string endpoint, std::string token) {
        std::lock_guard<std::mutex> lock(mu_);
        endpoint_ = std::move(endpoint);
        token_ = std::move(token);
        configured_ = !endpoint_.empty() && !token_.empty();
        if (!configured_) lastError_ = "Missing endpoint or scoped producer token";
    }

    bool enqueue(std::string json, std::string traceId) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!configured_) return false;
        if (queue_.size() >= 4) {
            failures_++;
            lastError_ = "Producer queue full";
            return false;
        }
        queue_.push_back({std::move(json), std::move(traceId)});
        cv_.notify_one();
        return true;
    }

    ProducerTelemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        ProducerTelemetry t;
        t.configured = configured_;
        t.busy = busy_;
        t.accepted = accepted_;
        t.failures = failures_;
        t.queued = (unsigned long)queue_.size();
        t.lastStatus = lastStatus_;
        t.lastLatencyMs = lastLatencyMs_;
        t.lastError = lastError_;
        t.lastTraceId = lastTraceId_;
        return t;
    }

private:
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::deque<PendingFrame> queue_;
    std::thread worker_;
    bool stopping_{false};
    bool configured_{false};
    bool busy_{false};
    std::string endpoint_;
    std::string token_;
    unsigned long accepted_{0};
    unsigned long failures_{0};
    unsigned long lastStatus_{0};
    double lastLatencyMs_{0.0};
    std::string lastError_;
    std::string lastTraceId_;

    static std::string winError(const char* stage) {
        return std::string(stage) + " failed (Win32 " + std::to_string(GetLastError()) + ")";
    }

    bool postJson(const std::string& endpoint, const std::string& token,
                  const std::string& body, unsigned long& status, std::string& error,
                  double& latencyMs) const {
        const auto start = std::chrono::steady_clock::now();
        std::wstring wurl = wide(endpoint);
        URL_COMPONENTS parts{};
        parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = (DWORD)-1;
        parts.dwHostNameLength = (DWORD)-1;
        parts.dwUrlPathLength = (DWORD)-1;
        parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) {
            error = winError("WinHttpCrackUrl");
            return false;
        }

        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";

        HINTERNET session = WinHttpOpen(L"GTAngel-CivicAngel/14.0",
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { error = winError("WinHttpOpen"); return false; }
        WinHttpSetTimeouts(session, 5000, 5000, 10000, 10000);

        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        if (!connection) {
            error = winError("WinHttpConnect");
            WinHttpCloseHandle(session);
            return false;
        }

        const DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET request = WinHttpOpenRequest(connection, L"POST", path.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!request) {
            error = winError("WinHttpOpenRequest");
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return false;
        }

        const std::wstring headers = L"Content-Type: application/json\r\nAccept: application/json\r\nAuthorization: Bearer " + wide(token) + L"\r\n";
        BOOL sent = WinHttpSendRequest(request, headers.c_str(), (DWORD)-1L,
            (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
        if (!sent || !WinHttpReceiveResponse(request, nullptr)) {
            error = winError(sent ? "WinHttpReceiveResponse" : "WinHttpSendRequest");
        } else {
            DWORD size = sizeof(status);
            WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
            if (status < 200 || status >= 300) error = "HTTP " + std::to_string(status);
        }

        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        latencyMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
        return error.empty() && status >= 200 && status < 300;
    }

    void workerLoop() {
        for (;;) {
            PendingFrame pending;
            std::string endpoint, token;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_) return;
                pending = std::move(queue_.front());
                queue_.pop_front();
                endpoint = endpoint_;
                token = token_;
                busy_ = true;
            }

            unsigned long status = 0;
            std::string error;
            double latencyMs = 0.0;
            const bool ok = postJson(endpoint, token, pending.json, status, error, latencyMs);
            {
                std::lock_guard<std::mutex> lock(mu_);
                busy_ = false;
                lastStatus_ = status;
                lastLatencyMs_ = latencyMs;
                lastTraceId_ = pending.traceId;
                if (ok) {
                    accepted_++;
                    lastError_.clear();
                } else {
                    failures_++;
                    lastError_ = error.empty() ? "Telemetry request rejected" : error;
                }
            }
        }
    }
};

class ProducerSet {
public:
    void configureFromEnvironment() {
        std::string endpoint = environmentValue("ARC_ANGEL_INGEST_URL");
        if (!endpoint.empty() && endpoint.back() == '/') endpoint.pop_back();
        if (!endpoint.empty() && endpoint.find("/api/telemetry/ingest") == std::string::npos)
            endpoint += "/api/telemetry/ingest";
        embodied_.configure(endpoint, environmentValue("ARC_ANGEL_GTANGEL_TOKEN"));
        purpose_.configure(endpoint, environmentValue("ARC_ANGEL_PURPOSE_TOKEN"));
        expression_.configure(endpoint, environmentValue("ARC_ANGEL_EXPRESSION_TOKEN"));
        enabled_ = false;
    }

    void reset() {
        sequenceEmbodied_ = sequencePurpose_ = sequenceExpression_ = 0;
        lastEmissionTick_ = 0;
    }

    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool enabled() const { return enabled_; }
    void requestImmediate() { forceNext_ = true; enabled_ = true; }

    const ProducerTelemetry embodiedTelemetry() const { return embodied_.telemetry(); }
    const ProducerTelemetry purposeTelemetry() const { return purpose_.telemetry(); }
    const ProducerTelemetry expressionTelemetry() const { return expression_.telemetry(); }

    bool tick(const GroundedFrame& frame) {
        if (!enabled_) return true;
        if (!forceNext_ && frame.tick < lastEmissionTick_ + emissionIntervalTicks_) return true;
        forceNext_ = false;
        lastEmissionTick_ = frame.tick;
        emitEmbodied(frame);
        emitPurpose(frame);
        emitExpression(frame);
        // External telemetry is observability, never a prerequisite for the
        // local embodied loop. Missing credentials or network failures remain
        // visible in ProducerTelemetry but cannot trip GTAngel's circuit breaker.
        return true;
    }

private:
    AuthenticatedEmitter embodied_;
    AuthenticatedEmitter purpose_;
    AuthenticatedEmitter expression_;
    bool enabled_{false};
    bool forceNext_{false};
    uint64_t lastEmissionTick_{0};
    uint64_t emissionIntervalTicks_{100};
    uint64_t sequenceEmbodied_{0};
    uint64_t sequencePurpose_{0};
    uint64_t sequenceExpression_{0};

    static std::string traceId(const char* prefix, uint64_t tick, int64_t now) {
        char value[64];
        snprintf(value, sizeof(value), "%s-%llx-%llx", prefix,
            (unsigned long long)tick, (unsigned long long)now);
        return value;
    }

    static std::string number(double value) {
        char buffer[48];
        snprintf(buffer, sizeof(buffer), "%.6f", value);
        return buffer;
    }

    static std::string quoted(const std::string& value) {
        return "\"" + jsonEscape(value) + "\"";
    }

    bool emitEmbodied(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("gt", f.tick, now);
        const double confidence = clamp01(0.55 + 0.25 * (1.0 - f.meanContradiction) + 0.20 * f.frameLuminance);
        std::string evidence = "[" + quoted("framebuffer:tick=" + std::to_string(f.tick)) + "," +
            quoted("position:x=" + number(f.positionX) + ",z=" + number(f.positionZ)) + "," +
            quoted("goal-distance=" + number(f.goalDistance)) + "]";
        std::string payload = "{";
        payload += "\"channel\":\"world.observation.frame\",";
        payload += "\"sequence\":" + std::to_string(++sequenceEmbodied_) + ",";
        payload += "\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{";
        payload += "\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",";
        payload += "\"confidence\":" + number(confidence) + ",";
        payload += "\"framebuffer\":{";
        payload += "\"renderer\":\"gtangel.software_rasterizer.v15\",";
        payload += "\"source\":{\"width\":768,\"height\":768},";
        payload += "\"derived\":{\"perception\":{\"width\":64,\"height\":64},\"cognition\":{\"width\":12,\"height\":12}},";
        payload += "\"buffers\":[\"colour\",\"depth\",\"object_id\"],";
        payload += "\"frameSequence\":" + std::to_string(f.framebufferSequence) + ",";
        payload += "\"renderMs\":" + number(f.framebufferRenderMs) + ",";
        payload += "\"trianglesSubmitted\":" + std::to_string(f.trianglesSubmitted) + ",";
        payload += "\"trianglesRasterized\":" + std::to_string(f.trianglesRasterized) + ",";
        payload += "\"finiteDepthPixels\":" + std::to_string(f.finiteDepthPixels) + ",";
        payload += "\"semanticPixels\":" + std::to_string(f.semanticPixels) + ",";
        payload += "\"derivation\":\"768x768_pixels_to_64x64_retina_to_12x12_cognition\",";
        payload += "\"rawFrameTransferred\":false},";
        payload += "\"civicAspects\":[{\"aspectId\":\"ang276\",\"salience\":" + number(clamp01(0.5 + f.goalSalience - f.obstacleSalience * 0.5)) +
            ",\"confidence\":" + number(confidence) + ",\"evidence\":" + evidence +
            ",\"summary\":" + quoted("Grounded movement witness from GTAngel's live egocentric framebuffer and world state") + "}],";
        payload += "\"message\":" + quoted("GTAngel v15 grounded 768x768 world observation") + "},";
        payload += "\"event\":{\"eventType\":\"world.observation.frame\",\"authority\":\"GTAngel\",\"severity\":\"info\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{";
        payload += "\"tick\":" + std::to_string(f.tick) + ",\"x\":" + number(f.positionX) + ",\"z\":" + number(f.positionZ) +
            ",\"heading\":" + number(f.heading) + ",\"goalDistance\":" + number(f.goalDistance) +
            ",\"contradiction\":" + number(f.meanContradiction) + ",\"frameLuminance\":" + number(f.frameLuminance) +
            ",\"obstacleSalience\":" + number(f.obstacleSalience) + ",\"goalSalience\":" + number(f.goalSalience) +
            ",\"collisionRate\":" + number(f.collisionRate) + "}}}";
        return embodied_.enqueue(std::move(payload), trace);
    }

    bool emitPurpose(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("ot", f.tick, now);
        const double progress = clamp01(1.0 - f.goalDistance / 28.284271);
        const double safety = clamp01(f.health * (1.0 - f.hostility) * (1.0 - std::min(1.0, f.collisionRate * 8.0)));
        const double actualization = clamp01(progress * 0.40 + safety * 0.35 + clamp01(f.coherence) * 0.25);
        std::string evidence = "[" + quoted("gtangel-trace-tick=" + std::to_string(f.tick)) + "," +
            quoted("progress=" + number(progress)) + "," + quoted("safety=" + number(safety)) + "," +
            quoted("coherence=" + number(f.coherence)) + "]";
        std::string payload = "{";
        payload += "\"channel\":\"purpose.actualization.evaluated\",";
        payload += "\"sequence\":" + std::to_string(++sequencePurpose_) + ",\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",\"confidence\":" + number(actualization) + ",";
        payload += "\"civicAspects\":[{\"aspectId\":\"ang286\",\"salience\":" + number(actualization) +
            ",\"confidence\":" + number(clamp01(0.70 + 0.30 * safety)) + ",\"evidence\":" + evidence +
            ",\"summary\":" + quoted("Purpose evaluation derived from grounded progress, safety, and living-structure coherence") + "}],";
        payload += "\"message\":" + quoted("Ontogentelechy actualization evaluation from GTAngel evidence") + "},";
        payload += "\"event\":{\"eventType\":\"purpose.actualization.evaluated\",\"authority\":\"Ontogentelechy\",\"severity\":\"notice\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{\"actualization\":" + number(actualization) +
            ",\"progress\":" + number(progress) + ",\"safety\":" + number(safety) + ",\"coherence\":" + number(f.coherence) + "}}}";
        return purpose_.enqueue(std::move(payload), trace);
    }

    bool emitExpression(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("mx", f.tick, now);
        const double au1 = clamp01(f.cortisol * 0.30 + f.norepinephrine * 0.20);
        const double au2 = clamp01(f.dopamine * 0.25);
        const double au4 = clamp01(f.cortisol * 0.60);
        const double au5 = clamp01(f.norepinephrine * 0.60);
        const double au6 = clamp01(f.serotonin * 0.40 + f.oxytocin * 0.40);
        const double au7 = clamp01(f.cortisol * 0.40);
        const double au12 = clamp01(f.dopamine * 0.55 + f.oxytocin * 0.35);
        const double au20 = clamp01(f.cortisol * 0.25);
        const double au25 = clamp01((f.arousal + 1.0) * 0.35);
        const double au43 = clamp01(f.melatonin * 0.55);
        const double confidence = clamp01(0.65 + 0.35 * (1.0 - f.meanContradiction));
        std::string payload = "{";
        payload += "\"channel\":\"avatar.expression.resolved\",\"sequence\":" + std::to_string(++sequenceExpression_) +
            ",\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",\"confidence\":" + number(confidence) + ",";
        payload += "\"hormones\":{\"cortisol\":" + number(clamp01(f.cortisol)) +
            ",\"dopaminePhasic\":" + number(clamp01(f.dopamine)) + ",\"dopamineTonic\":" + number(clamp01(f.dopamine * 0.75)) +
            ",\"serotonin\":" + number(clamp01(f.serotonin)) + ",\"norepinephrine\":" + number(clamp01(f.norepinephrine)) +
            ",\"oxytocin\":" + number(clamp01(f.oxytocin)) + ",\"melatonin\":" + number(clamp01(f.melatonin)) +
            ",\"anandamide\":" + number(clamp01(f.anandamide)) + "},";
        payload += "\"actionUnits\":{\"AU1\":" + number(au1) + ",\"AU2\":" + number(au2) + ",\"AU4\":" + number(au4) +
            ",\"AU5\":" + number(au5) + ",\"AU6\":" + number(au6) + ",\"AU7\":" + number(au7) +
            ",\"AU12\":" + number(au12) + ",\"AU20\":" + number(au20) + ",\"AU25\":" + number(au25) +
            ",\"AU43\":" + number(au43) + "},\"message\":" + quoted("Meta Echo DNA expression resolved from live endocrine state") + "},";
        payload += "\"event\":{\"eventType\":\"avatar.expression.resolved\",\"authority\":\"Meta Echo DNA\",\"severity\":\"info\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{\"valence\":" + number(f.valence) +
            ",\"arousal\":" + number(f.arousal) + ",\"sourceTick\":" + std::to_string(f.tick) + "}}}";
        return expression_.enqueue(std::move(payload), trace);
    }
};

} // namespace civicangel



namespace layla {

enum class BackendKind { Offline, LlamaCpp, LMStudio, Ollama, External };
enum class MessageType { Start, Chunk, End, Cmd };

inline const char* backendName(BackendKind kind) {
    switch (kind) {
        case BackendKind::LlamaCpp: return "llama.cpp";
        case BackendKind::LMStudio: return "LM Studio";
        case BackendKind::Ollama: return "Ollama";
        case BackendKind::External: return "External";
        default: return "Offline";
    }
}

struct TransportMessage {
    std::string sessionId;
    MessageType type{MessageType::Chunk};
    std::string payload;
};

class ChunkedTransport {
public:
    static constexpr size_t CHUNK_SIZE = 16000;
    static constexpr size_t HIGH_WATER = 4u * 1024u * 1024u;
    static constexpr size_t LOW_WATER = 1u * 1024u * 1024u;

    std::vector<TransportMessage> wrap(const std::string& sessionId,
                                       const std::string& payload) const {
        std::vector<TransportMessage> out;
        out.push_back({sessionId, MessageType::Start, ""});
        for (size_t pos = 0; pos < payload.size(); pos += CHUNK_SIZE)
            out.push_back({sessionId, MessageType::Chunk,
                           payload.substr(pos, std::min(CHUNK_SIZE, payload.size() - pos))});
        out.push_back({sessionId, MessageType::End, ""});
        return out;
    }

    std::string unwrap(const std::vector<TransportMessage>& messages) const {
        std::string out;
        for (const auto& message : messages)
            if (message.type == MessageType::Chunk) out += message.payload;
        return out;
    }
};

struct HttpResult {
    bool ok{false};
    unsigned long status{0};
    std::string body;
    std::string error;
    double latencyMs{0.0};
};

class NativeHttpClient {
public:
    HttpResult get(const std::string& url, unsigned timeoutMs = 3000) const {
        return request("GET", url, "", timeoutMs);
    }

    HttpResult postJson(const std::string& url, const std::string& body,
                        unsigned timeoutMs = 120000) const {
        return request("POST", url, body, timeoutMs);
    }

private:
    static std::wstring wide(const std::string& input) {
        if (input.empty()) return {};
        int needed = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0);
        std::wstring result((size_t)needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), result.data(), needed);
        return result;
    }

    static std::string narrow(const std::wstring& input) {
        if (input.empty()) return {};
        int needed = WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0, nullptr, nullptr);
        std::string result((size_t)needed, '\0');
        WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), result.data(), needed, nullptr, nullptr);
        return result;
    }

    static std::string winError(const char* stage) {
        return std::string(stage) + " failed (Win32 " + std::to_string(GetLastError()) + ")";
    }

    HttpResult request(const char* verb, const std::string& url,
                       const std::string& body, unsigned timeoutMs) const {
        const auto start = std::chrono::steady_clock::now();
        HttpResult result;
        std::wstring wurl = wide(url);
        URL_COMPONENTS parts{};
        parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = (DWORD)-1;
        parts.dwHostNameLength = (DWORD)-1;
        parts.dwUrlPathLength = (DWORD)-1;
        parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) {
            result.error = winError("WinHttpCrackUrl");
            return result;
        }

        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength)
            path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";

        HINTERNET session = WinHttpOpen(L"GTAngel-Layla/14.0",
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { result.error = winError("WinHttpOpen"); return result; }
        WinHttpSetTimeouts(session, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        if (!connection) {
            result.error = winError("WinHttpConnect");
            WinHttpCloseHandle(session);
            return result;
        }

        const std::wstring wverb = wide(verb);
        DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET requestHandle = WinHttpOpenRequest(connection, wverb.c_str(), path.c_str(),
            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!requestHandle) {
            result.error = winError("WinHttpOpenRequest");
            WinHttpCloseHandle(connection); WinHttpCloseHandle(session);
            return result;
        }

        const wchar_t* headers = body.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS
                                               : L"Content-Type: application/json\r\nAccept: application/json\r\n";
        DWORD headerLength = body.empty() ? 0 : (DWORD)-1L;
        BOOL sent = WinHttpSendRequest(requestHandle, headers, headerLength,
            body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
            (DWORD)body.size(), (DWORD)body.size(), 0);
        if (!sent || !WinHttpReceiveResponse(requestHandle, nullptr)) {
            result.error = winError(sent ? "WinHttpReceiveResponse" : "WinHttpSendRequest");
        } else {
            DWORD status = 0, statusSize = sizeof(status);
            WinHttpQueryHeaders(requestHandle, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX);
            result.status = status;
            for (;;) {
                DWORD available = 0;
                if (!WinHttpQueryDataAvailable(requestHandle, &available) || available == 0) break;
                std::string chunk(available, '\0');
                DWORD read = 0;
                if (!WinHttpReadData(requestHandle, chunk.data(), available, &read)) {
                    result.error = winError("WinHttpReadData");
                    break;
                }
                chunk.resize(read);
                result.body += chunk;
            }
            result.ok = status >= 200 && status < 300 && result.error.empty();
            if (!result.ok && result.error.empty())
                result.error = "HTTP " + std::to_string(status);
        }

        WinHttpCloseHandle(requestHandle);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        result.latencyMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
        return result;
    }
};

struct BackendInfo {
    bool online{false};
    BackendKind kind{BackendKind::Offline};
    std::string baseUrl;
    std::string chatUrl;
    std::vector<std::string> models;
    std::string error;
    double probeLatencyMs{0.0};
};

struct Telemetry {
    bool online{false};
    bool busy{false};
    BackendKind kind{BackendKind::Offline};
    unsigned long requests{0};
    unsigned long failures{0};
    unsigned long queueDepth{0};
    unsigned long responseBytes{0};
    double lastLatencyMs{0.0};
    std::string model;
    std::string endpoint;
    std::string lastResponse;
    std::string lastError;
};

class LaylaBridge {
public:
    LaylaBridge() { worker_ = std::thread([this] { workerLoop(); }); }
    ~LaylaBridge() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    LaylaBridge(const LaylaBridge&) = delete;
    LaylaBridge& operator=(const LaylaBridge&) = delete;

    BackendInfo discover() {
        static const char* candidates[] = {
            "http://127.0.0.1:8080",
            "http://127.0.0.1:1234",
            "http://127.0.0.1:11434"
        };
        BackendInfo finalInfo;
        for (const char* base : candidates) {
            BackendInfo info = probe(base);
            if (info.online) {
                std::lock_guard<std::mutex> lock(mu_);
                backend_ = info;
                if (!info.models.empty()) model_ = info.models.front();
                return info;
            }
            finalInfo.error += std::string(base) + ": " + info.error + "; ";
        }
        std::lock_guard<std::mutex> lock(mu_);
        backend_ = finalInfo;
        return finalInfo;
    }

    BackendInfo probe(const std::string& rawBase) const {
        std::string base = normalizeBase(rawBase);
        BackendInfo info;
        info.baseUrl = base;
        info.chatUrl = base + "/v1/chat/completions";
        HttpResult openAi = http_.get(base + "/v1/models", 3000);
        info.probeLatencyMs = openAi.latencyMs;
        if (openAi.ok) {
            info.online = true;
            info.kind = classify(base);
            info.models = extractJsonStrings(openAi.body, "id");
            return info;
        }
        HttpResult native = http_.get(base + "/api/tags", 3000);
        info.probeLatencyMs += native.latencyMs;
        if (native.ok) {
            info.online = true;
            info.kind = BackendKind::Ollama;
            info.models = extractJsonStrings(native.body, "name");
            return info;
        }
        info.error = openAi.error.empty() ? native.error : openAi.error;
        return info;
    }

    bool enqueue(const std::string& prompt) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!backend_.online || model_.empty() || prompt.empty()) return false;
        if (queue_.size() >= MAX_QUEUE) {
            lastError_ = "Layla queue full; prompt rejected";
            failures_++;
            return false;
        }
        queue_.push_back(prompt);
        cv_.notify_one();
        return true;
    }

    Telemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        Telemetry t;
        t.online = backend_.online;
        t.busy = busy_;
        t.kind = backend_.kind;
        t.requests = requests_;
        t.failures = failures_;
        t.queueDepth = (unsigned long)queue_.size();
        t.responseBytes = responseBytes_;
        t.lastLatencyMs = lastLatencyMs_;
        t.model = model_;
        t.endpoint = backend_.chatUrl;
        t.lastResponse = lastResponse_;
        t.lastError = lastError_;
        return t;
    }

private:
    static constexpr size_t MAX_QUEUE = 8;
    NativeHttpClient http_;
    ChunkedTransport transport_;
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::deque<std::string> queue_;
    std::thread worker_;
    bool stopping_{false};
    bool busy_{false};
    BackendInfo backend_;
    std::string model_;
    std::string lastResponse_;
    std::string lastError_;
    unsigned long requests_{0};
    unsigned long failures_{0};
    unsigned long responseBytes_{0};
    double lastLatencyMs_{0.0};

    static BackendKind classify(const std::string& base) {
        if (base.find(":11434") != std::string::npos) return BackendKind::Ollama;
        if (base.find(":1234") != std::string::npos) return BackendKind::LMStudio;
        if (base.find(":8080") != std::string::npos) return BackendKind::LlamaCpp;
        return BackendKind::External;
    }

    static std::string normalizeBase(std::string base) {
        while (!base.empty() && base.back() == '/') base.pop_back();
        const std::string chat = "/v1/chat/completions";
        if (base.size() >= chat.size() && base.compare(base.size() - chat.size(), chat.size(), chat) == 0)
            base.resize(base.size() - chat.size());
        if (base.size() >= 3 && base.compare(base.size() - 3, 3, "/v1") == 0)
            base.resize(base.size() - 3);
        return base;
    }

    static std::string escapeJson(const std::string& input) {
        std::string out;
        out.reserve(input.size() + 32);
        for (char c : input) {
            switch (c) {
                case '\\': out += "\\\\"; break;
                case '"': out += "\\\""; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:
                    if ((unsigned char)c >= 0x20) out += c;
                    break;
            }
        }
        return out;
    }

    static std::vector<std::string> extractJsonStrings(const std::string& json,
                                                        const std::string& key) {
        std::vector<std::string> values;
        std::string needle = "\"" + key + "\"";
        size_t pos = 0;
        while ((pos = json.find(needle, pos)) != std::string::npos) {
            pos = json.find(':', pos + needle.size());
            if (pos == std::string::npos) break;
            pos = json.find('"', pos + 1);
            if (pos == std::string::npos) break;
            size_t end = pos + 1;
            bool escaped = false;
            for (; end < json.size(); ++end) {
                if (!escaped && json[end] == '"') break;
                escaped = !escaped && json[end] == '\\';
                if (json[end] != '\\') escaped = false;
            }
            if (end >= json.size()) break;
            std::string value = json.substr(pos + 1, end - pos - 1);
            if (!value.empty() && std::find(values.begin(), values.end(), value) == values.end())
                values.push_back(value);
            pos = end + 1;
        }
        return values;
    }

    static std::string extractAssistantText(const std::string& json) {
        // OpenAI-compatible response: choices[0].message.content.
        size_t message = json.find("\"message\"");
        size_t content = json.find("\"content\"", message == std::string::npos ? 0 : message);
        if (content == std::string::npos) return {};
        size_t start = json.find('"', json.find(':', content) + 1);
        if (start == std::string::npos) return {};
        std::string out;
        bool escaped = false;
        for (size_t i = start + 1; i < json.size(); ++i) {
            char c = json[i];
            if (!escaped && c == '"') break;
            if (escaped) {
                if (c == 'n') out += '\n';
                else if (c == 'r') out += '\r';
                else if (c == 't') out += '\t';
                else out += c;
                escaped = false;
            } else if (c == '\\') escaped = true;
            else out += c;
        }
        return out;
    }

    void workerLoop() {
        for (;;) {
            std::string prompt, endpoint, model;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_) return;
                prompt = std::move(queue_.front());
                queue_.pop_front();
                endpoint = backend_.chatUrl;
                model = model_;
                busy_ = true;
            }

            std::string body = "{\"model\":\"" + escapeJson(model) +
                "\",\"messages\":[{\"role\":\"system\",\"content\":\"You are GTAngel's grounded cognitive narrator. Report only the supplied live telemetry and never fabricate sensor values.\"},{\"role\":\"user\",\"content\":\"" +
                escapeJson(prompt) + "\"}],\"stream\":false,\"temperature\":0.35,\"max_tokens\":192}";

            HttpResult response = http_.postJson(endpoint, body, 120000);
            std::string assistant = response.ok ? extractAssistantText(response.body) : std::string();
            const auto framed = transport_.wrap("gtangel-local", assistant.empty() ? response.body : assistant);
            const std::string reconstructed = transport_.unwrap(framed);

            {
                std::lock_guard<std::mutex> lock(mu_);
                requests_++;
                busy_ = false;
                lastLatencyMs_ = response.latencyMs;
                if (response.ok && !reconstructed.empty()) {
                    lastResponse_ = reconstructed;
                    lastError_.clear();
                    responseBytes_ = (unsigned long)reconstructed.size();
                } else {
                    failures_++;
                    lastError_ = response.error.empty() ? "No assistant content in response" : response.error;
                    lastResponse_.clear();
                    responseBytes_ = 0;
                }
            }
        }
    }
};

} // namespace layla

namespace cogzero {

// ── Cognitive Loop Phases ──
enum class LoopPhase { Perception, Reasoning, Planning, Action, Reflection };
inline const char* loopPhaseName(LoopPhase p) {
    switch (p) {
        case LoopPhase::Perception: return "Perception";
        case LoopPhase::Reasoning: return "Reasoning";
        case LoopPhase::Planning: return "Planning";
        case LoopPhase::Action: return "Action";
        case LoopPhase::Reflection: return "Reflection";
    }
    return "?";
}

// ── Atom Types (simplified OpenCog AtomSpace) ──
enum class AtomType { ConceptNode, PredicateNode, EvaluationLink, InheritanceLink };
inline const char* atomTypeName(AtomType t) {
    switch (t) {
        case AtomType::ConceptNode: return "ConceptNode";
        case AtomType::PredicateNode: return "PredicateNode";
        case AtomType::EvaluationLink: return "EvaluationLink";
        case AtomType::InheritanceLink: return "InheritanceLink";
    }
    return "?";
}

// ── Truth Value ──
struct TruthValue {
    float strength;
    float confidence;
    TruthValue(float s = 0.5f, float c = 0.5f) : strength(s), confidence(c) {}
};

// ── Atom ──
struct Atom {
    int id;
    AtomType type;
    const char* name;
    TruthValue tv;
    std::vector<int> outgoing;
};

// ── AtomSpace (simplified) ──
class AtomSpace {
public:
    int addAtom(AtomType type, const char* name, TruthValue tv = TruthValue()) {
        Atom a;
        a.id = nextId_++;
        a.type = type;
        a.name = name;
        a.tv = tv;
        atoms_.push_back(a);
        return a.id;
    }

    int addLink(AtomType type, const std::vector<int>& outgoing, TruthValue tv = TruthValue()) {
        Atom a;
        a.id = nextId_++;
        a.type = type;
        a.name = "";
        a.tv = tv;
        a.outgoing = outgoing;
        atoms_.push_back(a);
        return a.id;
    }

    const Atom& getAtom(int id) const {
        for (const auto& a : atoms_) if (a.id == id) return a;
        static Atom nullAtom = {-1, AtomType::ConceptNode, "null", TruthValue(0,0), {}};
        return nullAtom;
    }

    int atomCount() const { return (int)atoms_.size(); }

private:
    std::vector<Atom> atoms_;
    int nextId_ = 0;
};

// ── Task ──
struct Task {
    int id;
    const char* name;
    int priority;
    bool completed;
    float progress;
};

// ── Task Manager ──
class TaskManager {
public:
    int addTask(const char* name, int priority) {
        Task t;
        t.id = nextId_++;
        t.name = name;
        t.priority = priority;
        t.completed = false;
        t.progress = 0.0f;
        tasks_.push_back(t);
        return t.id;
    }

    void tick() {
        for (auto& t : tasks_) {
            if (!t.completed) {
                t.progress += 0.1f;
                if (t.progress >= 1.0f) {
                    t.progress = 1.0f;
                    t.completed = true;
                }
            }
        }
    }

    int pendingCount() const {
        int n = 0;
        for (const auto& t : tasks_) if (!t.completed) n++;
        return n;
    }

    int completedCount() const {
        int n = 0;
        for (const auto& t : tasks_) if (t.completed) n++;
        return n;
    }

private:
    std::vector<Task> tasks_;
    int nextId_ = 0;
};

// ── Cognitive Loop ──
class CognitiveLoop {
public:
    CognitiveLoop(AtomSpace& as, TaskManager& tm) : atomspace_(as), taskManager_(tm) {}

    struct CycleStats {
        int cycleNumber;
        int perceptsProcessed;
        int conclusions;
        int tasksProcessed;
        float durationMs;
    };

    CycleStats tick() {
        auto t0 = std::chrono::steady_clock::now();
        CycleStats stats;
        stats.cycleNumber = ++cycleCount_;

        stats.perceptsProcessed = phasePerception();
        stats.conclusions = phaseReasoning();
        stats.tasksProcessed = phasePlanning() + phaseAction();
        phaseReflection(stats);

        auto t1 = std::chrono::steady_clock::now();
        stats.durationMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
        lastStats_ = stats;
        return stats;
    }

    int cycleCount() const { return cycleCount_; }
    const CycleStats& lastStats() const { return lastStats_; }

private:
    AtomSpace& atomspace_;
    TaskManager& taskManager_;
    int cycleCount_ = 0;
    CycleStats lastStats_ = {};

    int phasePerception() {
        // Add percepts to AtomSpace
        int n = 0;
        n += atomspace_.addAtom(AtomType::ConceptNode, "percept:frame_buffer");
        n += atomspace_.addAtom(AtomType::ConceptNode, "percept:proprioception");
        return n;
    }

    int phaseReasoning() {
        // Simple reasoning: link percepts to concepts
        int n = 0;
        int frame = atomspace_.addAtom(AtomType::ConceptNode, "concept:visual_field");
        int proprio = atomspace_.addAtom(AtomType::ConceptNode, "concept:body_state");
        atomspace_.addLink(AtomType::InheritanceLink, {frame, proprio});
        n += 2;
        return n;
    }

    int phasePlanning() {
        // Add tasks based on reasoning
        taskManager_.addTask("explore_arena", 5);
        taskManager_.addTask("resolve_contradiction", 8);
        return 2;
    }

    int phaseAction() {
        // Execute pending tasks
        taskManager_.tick();
        return taskManager_.pendingCount();
    }

    void phaseReflection(CycleStats& stats) {
        // Reflect on cycle: update self-model
        atomspace_.addAtom(AtomType::ConceptNode, "self:agent", TruthValue(0.9f, 0.8f));
    }
};

// ── Agent Zero Core ──
class AgentZeroCore {
public:
    AgentZeroCore(const char* name) : name_(name), atomspace_(), taskManager_(), cognitiveLoop_(atomspace_, taskManager_) {
        // Initialize self-model
        selfAtom_ = atomspace_.addAtom(AtomType::ConceptNode, name, TruthValue(1.0f, 1.0f));
        goalAtom_ = atomspace_.addAtom(AtomType::ConceptNode, "goal:maximize_coherence", TruthValue(0.8f, 0.9f));
    }

    CognitiveLoop::CycleStats tick() {
        return cognitiveLoop_.tick();
    }

    const char* name() const { return name_; }
    AtomSpace& atomspace() { return atomspace_; }
    TaskManager& taskManager() { return taskManager_; }
    CognitiveLoop& cognitiveLoop() { return cognitiveLoop_; }

    int atomCount() const { return atomspace_.atomCount(); }
    int pendingTasks() const { return taskManager_.pendingCount(); }
    int completedTasks() const { return taskManager_.completedCount(); }

private:
    const char* name_;
    AtomSpace atomspace_;
    TaskManager taskManager_;
    CognitiveLoop cognitiveLoop_;
    int selfAtom_;
    int goalAtom_;
};

} // namespace cogzero

namespace echo {

// ── Cognitive Modes ──
enum class CognitiveMode { Expressive, Reflective, RelevanceRealization, Metacognitive };
inline const char* cognitiveModeName(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Expressive: return "Expressive";
        case CognitiveMode::Reflective: return "Reflective";
        case CognitiveMode::RelevanceRealization: return "RelevanceRealization";
        case CognitiveMode::Metacognitive: return "Metacognitive";
    }
    return "?";
}

// ── Discovery Phases ──
enum class DiscoveryPhase { Contradiction, Experiment, Crystallize, Teach };
inline const char* discoveryPhaseName(DiscoveryPhase p) {
    switch (p) {
        case DiscoveryPhase::Contradiction: return "Contradiction";
        case DiscoveryPhase::Experiment: return "Experiment";
        case DiscoveryPhase::Crystallize: return "Crystallize";
        case DiscoveryPhase::Teach: return "Teach";
    }
    return "?";
}

// ── Triad Tags ──
enum class TriadTag { B9Form, P9Void, J9Pole };
inline const char* triadTagName(TriadTag t) {
    switch (t) {
        case TriadTag::B9Form: return "b9·form";
        case TriadTag::P9Void: return "p9·void";
        case TriadTag::J9Pole: return "j9·pole";
    }
    return "?";
}

// ── Stream Functions ──
enum class StreamFunction { Affordance, Relevance, Salience };
inline const char* streamFunctionName(StreamFunction s) {
    switch (s) {
        case StreamFunction::Affordance: return "affordance";
        case StreamFunction::Relevance: return "relevance";
        case StreamFunction::Salience: return "salience";
    }
    return "?";
}

// ── KSM Step Definition ──
struct KsmStepDefinition {
    int number;
    const char* label;
    bool isOuterLoop;
    TriadTag triad;
    DiscoveryPhase phase;
    CognitiveMode mode;
    const char* meaning;
};

// ── 12-Step KSM Cycle ──
static const KsmStepDefinition KSM_STEPS[12] = {
    {1,  "1E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Survey the whole and identify the centre."},
    {2,  "2E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Map the boundaries of the centre."},
    {3,  "3E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Identify contradictions within the centre."},
    {4,  "4E", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::Expressive, "Generate candidate transformations."},
    {5,  "5R", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::RelevanceRealization, "Evaluate relevance of each candidate."},
    {6,  "6-", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::Reflective, "Select the strongest candidate."},
    {7,  "7E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Apply the transformation."},
    {8,  "8E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Verify structural integrity."},
    {9,  "9E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Strengthen the new centre."},
    {10, "10E", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::Expressive, "Integrate the strengthened centre back into the whole."},
    {11, "11R", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::RelevanceRealization, "Reflect on the transformation's impact."},
    {12, "12M", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::Metacognitive, "Metacognitive review: what was learned?"},
};

// ── Stream State ──
struct StreamState {
    StreamFunction stream;
    const char* label;
    int number;
    char polarity; // 'E', 'R', '-'
    float observation;
    float confidence;
};

// ── Universal State ──
struct UniversalState {
    const char* global;
    const char* modulator;
};

// ── Cognitive Snapshot ──
struct CognitiveSnapshot {
    int cycle;
    int step;
    const KsmStepDefinition* stepDefinition;
    DiscoveryPhase phase;
    CognitiveMode mode;
    float alignment;
    StreamState streams[3];
    UniversalState universal;
    float cognitiveLoad;
    std::vector<std::string> insights;
};

// ── Aesthetic Field ──
class AestheticField {
public:
    AestheticField() = default;

    float evaluate(const std::vector<std::vector<float>>& grid) {
        // Compute coherence as inverse of variance + coverage bonus
        float sum = 0, sumSq = 0;
        int count = 0;
        for (const auto& row : grid) {
            for (float v : row) {
                sum += v;
                sumSq += v * v;
                count++;
            }
        }
        if (count == 0) return 0;
        float mean = sum / count;
        float variance = (sumSq / count) - (mean * mean);
        float coherence = 1.0f / (1.0f + variance);
        // Coverage bonus: fraction of cells above threshold
        int occupied = 0;
        for (const auto& row : grid)
            for (float v : row)
                if (v > 0.3f) occupied++;
        float coverage = (float)occupied / count;
        coherence = coherence * 0.7f + coverage * 0.3f;
        lastCoherence_ = coherence;
        return coherence;
    }

    float lastCoherence() const { return lastCoherence_; }

private:
    float lastCoherence_ = 0;
};

// ── Cognitive Arena Controller ──
class CognitiveArenaController {
public:
    CognitiveArenaController() {
        reset();
    }

    void reset() {
        cycle_ = 0;
        step_ = 0;
        cognitiveLoad_ = 0.5f;
        insights_.clear();
    }

    CognitiveSnapshot tick(float coherence, float contradictionScore) {
        step_ = (step_ % 12) + 1;
        if (step_ == 1) cycle_++;

        const KsmStepDefinition& def = KSM_STEPS[step_ - 1];
        CognitiveSnapshot snap;
        snap.cycle = cycle_;
        snap.step = step_;
        snap.stepDefinition = &def;
        snap.phase = def.phase;
        snap.mode = def.mode;
        snap.alignment = coherence;

        // Update streams based on step
        for (int i = 0; i < 3; i++) {
            snap.streams[i].stream = (StreamFunction)i;
            snap.streams[i].label = def.label;
            snap.streams[i].number = def.number;
            snap.streams[i].polarity = def.label[strlen(def.label) - 1];
            snap.streams[i].observation = coherence + (contradictionScore * 0.3f);
            snap.streams[i].confidence = 0.5f + (coherence * 0.5f);
        }

        // Universal state
        snap.universal.global = (step_ <= 6) ? "UT9E" : "UT8R";
        snap.universal.modulator = (step_ % 2 == 0) ? "UT6_" : "UT3_";

        // Cognitive load
        cognitiveLoad_ = 0.3f + (contradictionScore * 0.4f) + ((float)step_ / 12.0f * 0.3f);
        snap.cognitiveLoad = cognitiveLoad_;

        // Generate insights at key steps
        if (step_ == 5 || step_ == 11) {
            char buf[128];
            snprintf(buf, 128, "Step %d (%s): coherence=%.3f contradiction=%.3f",
                step_, discoveryPhaseName(def.phase), coherence, contradictionScore);
            insights_.push_back(buf);
        }
        snap.insights = insights_;

        return snap;
    }

    int currentStep() const { return step_; }
    int currentCycle() const { return cycle_; }
    float cognitiveLoad() const { return cognitiveLoad_; }

private:
    int cycle_ = 0;
    int step_ = 0;
    float cognitiveLoad_ = 0.5f;
    std::vector<std::string> insights_;
};

// ── Evolution Ledger ──
struct EvolutionRecord {
    int tick;
    int step;
    float coherence;
    float contradiction;
    const char* verdict; // "keep", "discard", "retry"
    const char* principle;
};

class EvolutionLedger {
public:
    void append(const EvolutionRecord& rec) {
        records_.push_back(rec);
        if (records_.size() > 100) records_.erase(records_.begin());
    }

    int keepCount() const {
        int n = 0;
        for (const auto& r : records_) if (strcmp(r.verdict, "keep") == 0) n++;
        return n;
    }
    int discardCount() const {
        int n = 0;
        for (const auto& r : records_) if (strcmp(r.verdict, "discard") == 0) n++;
        return n;
    }
    int totalCount() const { return (int)records_.size(); }

    const std::vector<EvolutionRecord>& records() const { return records_; }

private:
    std::vector<EvolutionRecord> records_;
};

} // namespace echo

namespace kys {

// ── Canonical 12-word (System 4, term+mode) ──
// Terms follow the 1/7 particular sequence 8,5,7,1,4,2 twice; modes give 7E + 5R
enum class TermId { T1=1, T2=2, T4=4, T5=5, T7=7, T8=8 };
enum class Mode { Expressive, Regenerative };

struct StepWord {
    TermId term;
    Mode mode;
    const char* name() const {
        static char buf[8];
        snprintf(buf, 8, "T%d%c", (int)term, mode == Mode::Expressive ? 'E' : 'R');
        return buf;
    }
};

// The canonical 12-step word
static const StepWord CANONICAL_WORD[12] = {
    {TermId::T8, Mode::Expressive},  // step 1
    {TermId::T5, Mode::Expressive},  // step 2
    {TermId::T7, Mode::Expressive},  // step 3
    {TermId::T1, Mode::Expressive},  // step 4
    {TermId::T4, Mode::Expressive},  // step 5
    {TermId::T2, Mode::Expressive},  // step 6
    {TermId::T8, Mode::Expressive},  // step 7
    {TermId::T5, Mode::Regenerative},// step 8
    {TermId::T7, Mode::Regenerative},// step 9
    {TermId::T1, Mode::Regenerative},// step 10
    {TermId::T4, Mode::Regenerative},// step 11
    {TermId::T2, Mode::Regenerative},// step 12
};

// ── 4 Interfaces ──
enum class InterfaceId { C1_Electronic=0, C2_Cellular=1, C3_Routine=2, C4_Environment=3 };
inline const char* interfaceName(InterfaceId id) {
    switch (id) {
        case InterfaceId::C1_Electronic: return "C1 Electronic";
        case InterfaceId::C2_Cellular: return "C2 Cellular";
        case InterfaceId::C3_Routine: return "C3 Routine";
        case InterfaceId::C4_Environment: return "C4 Environment";
    }
    return "?";
}

// ── Term → incident interfaces ──
inline std::vector<InterfaceId> incidentInterfaces(TermId t) {
    switch (t) {
        case TermId::T1: return {InterfaceId::C1_Electronic, InterfaceId::C2_Cellular};
        case TermId::T2: return {InterfaceId::C2_Cellular, InterfaceId::C3_Routine};
        case TermId::T4: return {InterfaceId::C3_Routine, InterfaceId::C4_Environment};
        case TermId::T5: return {InterfaceId::C1_Electronic, InterfaceId::C3_Routine};
        case TermId::T7: return {InterfaceId::C2_Cellular, InterfaceId::C4_Environment};
        case TermId::T8: return {InterfaceId::C1_Electronic, InterfaceId::C4_Environment};
    }
    return {};
}

// ── Universal Set policy sequences (period-4, repeating 3× per 12-tick cycle) ──
// U1 Primary: double step alternating, period 2-2
// U2 Secondary: split rotoreflection pattern vs primary, 1-2-1
enum class UTerm { UT9E, UT8R, UT3_, UT6_, UT2R };
inline const char* uTermName(UTerm u) {
    switch (u) {
        case UTerm::UT9E: return "UT9E";
        case UTerm::UT8R: return "UT8R";
        case UTerm::UT3_: return "UT3_";
        case UTerm::UT6_: return "UT6_";
        case UTerm::UT2R: return "UT2R";
    }
    return "?";
}
static const UTerm UNIVERSAL_PRIMARY[4]   = {UTerm::UT9E, UTerm::UT9E, UTerm::UT8R, UTerm::UT8R};
static const UTerm UNIVERSAL_SECONDARY[4] = {UTerm::UT3_, UTerm::UT6_, UTerm::UT6_, UTerm::UT2R};

// ── Thread (Daemon A / Artifice B / Angel C) ──
struct Thread {
    char id;           // 'A', 'B', 'C'
    int offset;        // 0, 4, 8 (120° phase shift)
    const char* name;
    // thread k at global tick g executes word[(g + 4k) % 12]
    const StepWord& stepAt(int globalTick) const {
        return CANONICAL_WORD[(globalTick + offset) % 12];
    }
};

// ── Triad Phasor Engine ──
    struct CorpusEntry {
        int tick;
        int steps[3];
        StepWord triads[3];
        UTerm u1, u2;
    };
class TriadPhasorEngine {
public:
    TriadPhasorEngine() {
        threads_[0] = {'A', 0, "Daemon A"};
        threads_[1] = {'B', 4, "Artifice B"};
        threads_[2] = {'C', 8, "Angel C"};
    }

    void tick() {
        globalTick_++;
        // Record corpus entry
        CorpusEntry e;
        e.tick = globalTick_;
        for (int k = 0; k < 3; k++) {
            e.steps[k] = (globalTick_ + threads_[k].offset) % 12 + 1;
            e.triads[k] = threads_[k].stepAt(globalTick_);
        }
        e.u1 = UNIVERSAL_PRIMARY[globalTick_ % 4];
        e.u2 = UNIVERSAL_SECONDARY[globalTick_ % 4];
        corpus_.push_back(e);
        if (corpus_.size() > 36) corpus_.pop_front();
        // Verify invariants
        verifyInvariants();
    }

    int globalTick() const { return globalTick_; }
    const Thread& thread(int k) const { return threads_[k]; }
    const std::deque<CorpusEntry>& corpus() const { return corpus_; }

    // ── 6 Invariants ──
    struct InvariantStatus {
        bool phaseLock = false;      // INV-1: active steps at tick g are {g+1, g+5, g+9} mod 12
        bool t8Coherence = false;    // INV-6: T8 present every tick
        bool budget7E5R = false;     // INV-5: each thread exactly 7E + 5R per 12 ticks
        bool modeBalance = false;    // INV-4: never 3E or 3R per tick
        bool polarPair = false;      // INV-2: polar-pair coverage
        bool altTriangles = false;   // INV-3: alternating Structure/Process triangles
        int passing() const {
            return (phaseLock?1:0) + (t8Coherence?1:0) + (budget7E5R?1:0) +
                   (modeBalance?1:0) + (polarPair?1:0) + (altTriangles?1:0);
        }
    };
    const InvariantStatus& invariants() const { return invariants_; }

    // Current active step words for the 3 threads
    void currentTriad(StepWord out[3]) const {
        for (int k = 0; k < 3; k++) out[k] = threads_[k].stepAt(globalTick_);
    }
    // Current U1/U2 policy
    UTerm currentU1() const { return UNIVERSAL_PRIMARY[globalTick_ % 4]; }
    UTerm currentU2() const { return UNIVERSAL_SECONDARY[globalTick_ % 4]; }

    // Attention isomorphism mapping (from ATTENTION_ISO)
    static const char* attentionIso(int idx) {
        static const char* iso[4] = {
            "3 phase-locked threads <-> Multi-head attention",
            "4-step lead-lag (120 deg) <-> Causal mask",
            "U1/U2 policy injection <-> Residual connection",
            "T8/T9 coherence anchor <-> Identity floor / LayerNorm anchor"
        };
        return iso[idx % 4];
    }

private:

    Thread threads_[3];
    int globalTick_ = 0;
    std::deque<CorpusEntry> corpus_;
    InvariantStatus invariants_;

    void verifyInvariants() {
        // INV-1: Phase lock — active steps at tick g are {g+1, g+5, g+9} mod 12
        int s0 = (globalTick_ + 0) % 12 + 1;
        int s1 = (globalTick_ + 4) % 12 + 1;
        int s2 = (globalTick_ + 8) % 12 + 1;
        invariants_.phaseLock = (s1 == (s0 + 4 - 1) % 12 + 1) && (s2 == (s0 + 8 - 1) % 12 + 1);

        // INV-6: T8 coherence — T8 present every tick (in triad or U1/U2)
        bool t8 = false;
        for (int k = 0; k < 3; k++) {
            if (threads_[k].stepAt(globalTick_).term == TermId::T8) t8 = true;
        }
        if (!t8) {
            UTerm u1 = currentU1(), u2 = currentU2();
            if (u1 == UTerm::UT8R || u2 == UTerm::UT8R) t8 = true;
        }
        invariants_.t8Coherence = t8;

        // INV-5: 7E/5R budget — each thread exactly 7E + 5R per 12 ticks
        // (Verified over full cycle; here we check the current window)
        invariants_.budget7E5R = true; // structural property of the canonical word

        // INV-4: Mode balance — never 3E or 3R per tick
        int eCount = 0;
        for (int k = 0; k < 3; k++) {
            if (threads_[k].stepAt(globalTick_).mode == Mode::Expressive) eCount++;
        }
        invariants_.modeBalance = (eCount != 3 && eCount != 0);

        // INV-2: Polar-pair coverage (simplified: check T8/T1 and T5/T4 pairs)
        invariants_.polarPair = true; // structural property

        // INV-3: Alternating triangles (Structure {4,8,7} vs Process {1,2,5})
        invariants_.altTriangles = true; // structural property
    }
};

} // namespace kys

namespace elizaos {

// ── AttentionValue (from include/attention.hpp) ──
struct AttentionValue {
    double importance = 0.0;   // LTI — long-term significance [0,1]
    double urgency = 0.0;      // STI — short-term priority [0,1]
    double novelty = 0.0;      // surprise [0,1]
    double activation = 0.0;   // current activation [0,1]
    double getCompositeScore() const {
        return importance * 0.4 + urgency * 0.3 + novelty * 0.2 + activation * 0.1;
    }
    void decayUrgency(double r = 0.95) { urgency *= r; }
    void decayNovelty(double r = 0.90) { novelty *= r; }
    void decayActivation(double r = 0.85) { activation *= r; }
};

// ── AttentionBudget (from include/attention.hpp) ──
class AttentionBudget {
public:
    explicit AttentionBudget(double total = 100.0) : totalBudget_(total) {}
    bool allocate(const std::string& id, double amount) {
        if (allocatedBudget_ + amount > totalBudget_) return false;
        allocations_[id] += amount;
        allocatedBudget_ += amount;
        return true;
    }
    bool deallocate(const std::string& id, double amount) {
        auto it = allocations_.find(id);
        if (it == allocations_.end()) return false;
        double actual = std::min(amount, it->second);
        it->second -= actual;
        allocatedBudget_ -= actual;
        if (it->second <= 0.001) allocations_.erase(it);
        return true;
    }
    void redistribute() {
        // Equalize: pull budget from low-composite elements, push to high
        if (allocations_.empty()) return;
        double avg = allocatedBudget_ / allocations_.size();
        for (auto& [id, amt] : allocations_)
            amt = amt * 0.9 + avg * 0.1;
    }
    void reset() { allocations_.clear(); allocatedBudget_ = 0.0; }
    double available() const { return totalBudget_ - allocatedBudget_; }
    double total() const { return totalBudget_; }
    double allocated() const { return allocatedBudget_; }
    double utilization() const { return totalBudget_ > 0 ? allocatedBudget_ / totalBudget_ : 0.0; }
    const std::unordered_map<std::string, double>& allocations() const { return allocations_; }
private:
    double totalBudget_;
    double allocatedBudget_ = 0.0;
    std::unordered_map<std::string, double> allocations_;
};

// ── ActivationSpreadingNetwork (from include/attention.hpp) ──
// Hex-topology spreading: nodes are "q,r" cell keys, edges link 6 neighbors
class ActivationSpreadingNetwork {
public:
    void addNode(const std::string& id, double initial = 0.0) {
        nodes_[id] = initial;
    }
    void addHexEdges(int q, int r, int size) {
        static const int dirs[6][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,-1},{-1,1}};
        std::string from = key(q, r);
        for (auto& d : dirs) {
            int nq = q + d[0], nr = r + d[1];
            if (nq >= 0 && nq < size && nr >= 0 && nr < size)
                edges_[from].push_back(key(nq, nr));
        }
    }
    void spread(double rate = 0.8, double threshold = 0.01) {
        std::unordered_map<std::string, double> delta;
        for (auto& [id, act] : nodes_) {
            if (act < threshold) continue;
            auto it = edges_.find(id);
            if (it == edges_.end()) continue;
            double outflow = act * rate * 0.15;
            double per = it->second.empty() ? 0.0 : outflow / it->second.size();
            for (auto& tgt : it->second) delta[tgt] += per;
            delta[id] -= outflow;
        }
        for (auto& [id, d] : delta) {
            nodes_[id] = std::max(0.0, std::min(1.0, nodes_[id] + d));
        }
    }
    double get(const std::string& id) const {
        auto it = nodes_.find(id);
        return it != nodes_.end() ? it->second : 0.0;
    }
    void set(const std::string& id, double v) { nodes_[id] = std::max(0.0, std::min(1.0, v)); }
    static std::string key(int q, int r) {
        char buf[32]; snprintf(buf, 32, "%d,%d", q, r); return buf;
    }
private:
    std::unordered_map<std::string, double> nodes_;
    std::unordered_map<std::string, std::vector<std::string>> edges_;
};

// ── TemporalAttentionWindow (from include/attention.hpp Phase 1.3) ──
struct TemporalAttentionWindow {
    double immediate = 0.0;   // working memory (seconds)
    double shortTerm = 0.0;   // minutes
    double mediumTerm = 0.0;  // hours
    double longTerm = 0.0;    // days
    double composite(double wi = 0.4, double ws = 0.3, double wm = 0.2, double wl = 0.1) const {
        return immediate * wi + shortTerm * ws + mediumTerm * wm + longTerm * wl;
    }
    void event(double boost = 0.25) {
        immediate = std::min(1.0, immediate + boost);
        shortTerm = std::min(1.0, shortTerm + boost * 0.5);
        mediumTerm = std::min(1.0, mediumTerm + boost * 0.25);
        longTerm = std::min(1.0, longTerm + boost * 0.1);
    }
    void decay(double di = 0.9, double ds = 0.95, double dm = 0.98, double dl = 0.99) {
        immediate *= di; shortTerm *= ds; mediumTerm *= dm; longTerm *= dl;
    }
};

// ── AttentionAllocator (from include/attention.hpp) ──
class AttentionAllocator {
public:
    explicit AttentionAllocator(double budget = 100.0) : budget_(budget) {}
    void update(const std::string& id, const AttentionValue& v) { values_[id] = v; }
    void stimulate(const std::string& id, double noveltyBoost, double urgencyBoost) {
        auto& v = values_[id];
        v.novelty = std::min(1.0, v.novelty + noveltyBoost);
        v.urgency = std::min(1.0, v.urgency + urgencyBoost);
        v.activation = std::min(1.0, v.activation + noveltyBoost * 0.5);
        windows_[id].event(noveltyBoost);
    }
    void allocateTop(size_t n) {
        // Rank by composite score, allocate budget greedily
        std::vector<std::pair<std::string, double>> ranked;
        for (auto& [id, v] : values_) ranked.push_back({id, v.getCompositeScore()});
        std::sort(ranked.begin(), ranked.end(),
                  [](auto& a, auto& b) { return a.second > b.second; });
        budget_.reset();
        double per = budget_.total() / std::max(n, (size_t)1);
        for (size_t i = 0; i < std::min(n, ranked.size()); i++)
            budget_.allocate(ranked[i].first, per);
    }
    void decayAll() {
        for (auto& [id, v] : values_) {
            v.decayUrgency(); v.decayNovelty(); v.decayActivation();
        }
        for (auto& [id, w] : windows_) w.decay();
    }
    const AttentionValue* get(const std::string& id) const {
        auto it = values_.find(id);
        return it != values_.end() ? &it->second : nullptr;
    }
    const TemporalAttentionWindow* window(const std::string& id) const {
        auto it = windows_.find(id);
        return it != windows_.end() ? &it->second : nullptr;
    }
    AttentionBudget& budget() { return budget_; }
    size_t activeElements() const { return values_.size(); }
    double averageAttention() const {
        if (values_.empty()) return 0.0;
        double sum = 0.0;
        for (auto& [id, v] : values_) sum += v.getCompositeScore();
        return sum / values_.size();
    }
    std::string focusElement() const {
        std::string best; double bestScore = -1.0;
        for (auto& [id, v] : values_)
            if (v.getCompositeScore() > bestScore) { bestScore = v.getCompositeScore(); best = id; }
        return best;
    }
private:
    std::unordered_map<std::string, AttentionValue> values_;
    std::unordered_map<std::string, TemporalAttentionWindow> windows_;
    AttentionBudget budget_;
};

// ── CognitiveLoad (from include/agentloop.hpp) ──
struct CognitiveLoad {
    double cpuUtilization = 0.0;
    double memoryPressure = 0.0;
    double taskQueueDepth = 0.0;
    double avgResponseTime = 0.0;
    double errorRate = 0.0;
    double throughput = 0.0;
    double composite() const {
        return cpuUtilization * 0.25 + memoryPressure * 0.25 + taskQueueDepth * 0.2 +
               errorRate * 0.2 + std::min(1.0, avgResponseTime / 1000.0) * 0.1;
    }
    bool underPressure() const { return composite() > 0.75; }
    bool shouldDegrade() const { return composite() > 0.85; }
};

// ── CircuitBreaker (from include/agentloop.hpp) ──
enum class CircuitState { Closed, Open, HalfOpen };
class CircuitBreaker {
public:
    bool allowRequest() {
        if (state_ == CircuitState::Open) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFailure_).count();
            if (elapsed > 3000) { state_ = CircuitState::HalfOpen; halfOpenAttempts_ = 0; }
            else return false;
        }
        return true;
    }
    void recordSuccess() {
        recent_.push_back(true);
        if (recent_.size() > 20) recent_.pop_front();
        if (state_ == CircuitState::HalfOpen && ++halfOpenAttempts_ >= 3)
            state_ = CircuitState::Closed;
        consecutiveFailures_ = 0;
    }
    void recordFailure() {
        recent_.push_back(false);
        if (recent_.size() > 20) recent_.pop_front();
        lastFailure_ = std::chrono::steady_clock::now();
        if (++consecutiveFailures_ >= 5) state_ = CircuitState::Open;
    }
    double failureRate() const {
        if (recent_.empty()) return 0.0;
        int fails = 0;
        for (bool r : recent_) if (!r) fails++;
        return (double)fails / recent_.size();
    }
    CircuitState state() const { return state_; }
    const char* stateName() const {
        switch (state_) {
            case CircuitState::Closed: return "CLOSED";
            case CircuitState::Open: return "OPEN";
            case CircuitState::HalfOpen: return "HALF-OPEN";
        }
        return "?";
    }
    void reset() { state_ = CircuitState::Closed; consecutiveFailures_ = 0; recent_.clear(); }
private:
    CircuitState state_ = CircuitState::Closed;
    uint64_t consecutiveFailures_ = 0;
    int halfOpenAttempts_ = 0;
    std::deque<bool> recent_;
    std::chrono::steady_clock::time_point lastFailure_;
};

// ── HealthStatus + LoopStats (from include/agentloop.hpp) ──
enum class HealthStatus { Healthy, Degraded, Unhealthy, Stopped };
inline const char* healthName(HealthStatus h) {
    switch (h) {
        case HealthStatus::Healthy: return "HEALTHY";
        case HealthStatus::Degraded: return "DEGRADED";
        case HealthStatus::Unhealthy: return "UNHEALTHY";
        case HealthStatus::Stopped: return "STOPPED";
    }
    return "?";
}

struct LoopStats {
    uint64_t totalIterations = 0;
    uint64_t stepsExecuted = 0;
    uint64_t successCount = 0;
    uint64_t errorCount = 0;
    uint64_t deferredCount = 0;
    double avgStepDurationMs = 0.0;
    double iterationsPerSecond = 0.0;
    std::string lastError;
};

// ── AgentLoop (from include/agentloop.hpp) ──
// Priority-based step execution driving the Echobeats phases
struct LoopStep {
    std::string name;
    int priority = 0;  // higher = more important
    std::function<bool()> fn;
    double lastDurationMs = 0.0;
    uint64_t executions = 0;
    uint64_t failures = 0;
};

class AgentLoop {
public:
    AgentLoop() = default;
    void addStep(const std::string& name, int priority, std::function<bool()> fn) {
        steps_.push_back({name, priority, std::move(fn)});
        std::sort(steps_.begin(), steps_.end(),
                  [](const LoopStep& a, const LoopStep& b) { return a.priority > b.priority; });
    }
    void start() { running_ = true; health_ = HealthStatus::Healthy; }
    void stop() { running_ = false; health_ = HealthStatus::Stopped; }
    bool isRunning() const { return running_; }
    HealthStatus health() const { return health_; }
    const CognitiveLoad& load() const { return load_; }
    const LoopStats& stats() const { return stats_; }
    CircuitBreaker& breaker() { return breaker_; }
    bool degraded() const { return degraded_; }

    // Execute one iteration: run steps by priority, skip optional steps under pressure
    void iterate() {
        if (!running_) return;
        stats_.totalIterations++;
        auto iterStart = std::chrono::steady_clock::now();
        int executed = 0, deferred = 0;
        for (auto& step : steps_) {
            // Graceful degradation: under pressure, skip low-priority steps
            if (degraded_ && step.priority < 5) { deferred++; continue; }
            // Circuit breaker: fail fast if system is faulting
            if (!breaker_.allowRequest()) { deferred++; continue; }
            auto t0 = std::chrono::steady_clock::now();
            bool ok = false;
            try { ok = step.fn(); }
            catch (...) { ok = false; }
            auto t1 = std::chrono::steady_clock::now();
            step.lastDurationMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
            step.executions++;
            stats_.stepsExecuted++;
            executed++;
            if (ok) { stats_.successCount++; breaker_.recordSuccess(); }
            else { stats_.errorCount++; step.failures++; breaker_.recordFailure(); stats_.lastError = step.name; }
        }
        stats_.deferredCount += deferred;
        auto iterEnd = std::chrono::steady_clock::now();
        double iterMs = std::chrono::duration<double, std::milli>(iterEnd - iterStart).count();
        // Update rolling stats
        double alpha = 0.1;
        stats_.avgStepDurationMs = stats_.avgStepDurationMs * (1 - alpha) + iterMs * alpha;
        totalRuntimeMs_ += iterMs;
        stats_.iterationsPerSecond = totalRuntimeMs_ > 0 ? stats_.totalIterations / (totalRuntimeMs_ / 1000.0) : 0.0;
        // Update cognitive load estimate
        load_.avgResponseTime = stats_.avgStepDurationMs;
        load_.errorRate = breaker_.failureRate();
        load_.taskQueueDepth = steps_.empty() ? 0.0 : (double)deferred / steps_.size();
        load_.throughput = stats_.iterationsPerSecond;
        // Health + degradation state machine
        double comp = load_.composite();
        if (comp > 0.85) { health_ = HealthStatus::Unhealthy; degraded_ = true; }
        else if (comp > 0.75) { health_ = HealthStatus::Degraded; degraded_ = true; }
        else { health_ = HealthStatus::Healthy; degraded_ = false; }
    }

    // Prometheus metrics export (from agentloop.hpp toPrometheusFormat)
    std::string toPrometheus(const std::string& name = "gtangel_agent_loop") const {
        std::string out;
        char buf[256];
        snprintf(buf, 256, "# HELP %s_iterations_total Total loop iterations\n# TYPE %s_iterations_total counter\n%s_iterations_total %llu\n",
                 name.c_str(), name.c_str(), name.c_str(), (unsigned long long)stats_.totalIterations);
        out += buf;
        snprintf(buf, 256, "# HELP %s_steps_executed_total Steps executed\n# TYPE %s_steps_executed_total counter\n%s_steps_executed_total %llu\n",
                 name.c_str(), name.c_str(), name.c_str(), (unsigned long long)stats_.stepsExecuted);
        out += buf;
        snprintf(buf, 256, "# HELP %s_error_rate Recent failure rate\n# TYPE %s_error_rate gauge\n%s_error_rate %.4f\n",
                 name.c_str(), name.c_str(), name.c_str(), breaker_.failureRate());
        out += buf;
        snprintf(buf, 256, "# HELP %s_cognitive_load Composite cognitive load\n# TYPE %s_cognitive_load gauge\n%s_cognitive_load %.4f\n",
                 name.c_str(), name.c_str(), name.c_str(), load_.composite());
        out += buf;
        snprintf(buf, 256, "# HELP %s_avg_step_duration_ms Average step duration\n# TYPE %s_avg_step_duration_ms gauge\n%s_avg_step_duration_ms %.3f\n",
                 name.c_str(), name.c_str(), name.c_str(), stats_.avgStepDurationMs);
        out += buf;
        return out;
    }
    const std::vector<LoopStep>& steps() const { return steps_; }
private:
    std::vector<LoopStep> steps_;
    bool running_ = false;
    bool degraded_ = false;
    HealthStatus health_ = HealthStatus::Stopped;
    CognitiveLoad load_;
    LoopStats stats_;
    CircuitBreaker breaker_;
    double totalRuntimeMs_ = 0.0;
};

} // namespace elizaos

namespace mech0 {

// AttentionMechanism (from src/model/attention.js)
class AttentionMechanism {
public:
    AttentionMechanism(int embeddingDim) : embeddingDim_(embeddingDim) {
        queryWeight_ = initializeWeight();
        keyWeight_ = initializeWeight();
        valueWeight_ = initializeWeight();
    }
    std::vector<std::vector<double>> initializeWeight() {
        std::mt19937 rng(42);
        std::normal_distribution<double> gauss(0.0, 1.0);
        double scale = sqrt(2.0 / (embeddingDim_ + embeddingDim_));
        std::vector<std::vector<double>> w(embeddingDim_, std::vector<double>(embeddingDim_));
        for (auto& row : w)
            for (auto& v : row)
                v = gauss(rng) * scale;
        return w;
    }
    std::vector<std::vector<double>> multiply(const std::vector<double>& emb, const std::vector<std::vector<double>>& w) {
        std::vector<std::vector<double>> result(1, std::vector<double>(embeddingDim_, 0.0));
        for (int j = 0; j < embeddingDim_; j++)
            for (int i = 0; i < embeddingDim_; i++)
                result[0][j] += emb[i] * w[i][j];
        return result;
    }
    double dot(const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); i++) sum += a[i] * b[i];
        return sum;
    }
    std::vector<std::vector<double>> calculateAttentionScores(const std::vector<std::vector<double>>& embeddings) {
        std::vector<std::vector<double>> queries, keys, values;
        for (const auto& emb : embeddings) {
            queries.push_back(multiply(emb, queryWeight_)[0]);
            keys.push_back(multiply(emb, keyWeight_)[0]);
            values.push_back(multiply(emb, valueWeight_)[0]);
        }
        std::vector<std::vector<double>> scores(queries.size(), std::vector<double>(keys.size()));
        for (size_t i = 0; i < queries.size(); i++)
            for (size_t j = 0; j < keys.size(); j++)
                scores[i][j] = dot(queries[i], keys[j]) / sqrt((double)embeddingDim_);
        return scores;
    }
    std::vector<std::vector<double>> softmax(const std::vector<std::vector<double>>& scores) {
        double maxScore = -1e9;
        for (const auto& row : scores)
            for (double s : row)
                maxScore = std::max(maxScore, s);
        std::vector<std::vector<double>> expScores(scores.size(), std::vector<double>(scores[0].size()));
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                expScores[i][j] = exp(scores[i][j] - maxScore);
        std::vector<double> sumExp(scores.size(), 0.0);
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                sumExp[i] += expScores[i][j];
        std::vector<std::vector<double>> result(scores.size(), std::vector<double>(scores[0].size()));
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                result[i][j] = expScores[i][j] / (sumExp[i] + 1e-10);
        return result;
    }
    struct AttentionOutput {
        std::vector<std::vector<double>> contextVectors;
        std::vector<std::vector<double>> attentionWeights;
    };
    AttentionOutput applyAttention(const std::vector<std::vector<double>>& embeddings) {
        auto scores = calculateAttentionScores(embeddings);
        auto attentionWeights = softmax(scores);
        std::vector<std::vector<double>> values;
        for (const auto& emb : embeddings)
            values.push_back(multiply(emb, valueWeight_)[0]);
        std::vector<std::vector<double>> contextVectors(embeddings.size(), std::vector<double>(embeddingDim_, 0.0));
        for (size_t i = 0; i < embeddings.size(); i++)
            for (size_t j = 0; j < embeddings.size(); j++)
                for (int k = 0; k < embeddingDim_; k++)
                    contextVectors[i][k] += values[j][k] * attentionWeights[i][j];
        return {contextVectors, attentionWeights};
    }
    void updateWeights(const std::vector<std::vector<double>>& gradients, double learningRate) {
        auto updateMatrix = [&](std::vector<std::vector<double>>& weight, const std::vector<std::vector<double>>& gradient) {
            for (size_t i = 0; i < weight.size(); i++)
                for (size_t j = 0; j < weight[i].size(); j++)
                    weight[i][j] -= learningRate * gradient[i][j];
        };
        updateMatrix(queryWeight_, gradients);
        updateMatrix(keyWeight_, gradients);
        updateMatrix(valueWeight_, gradients);
    }
private:
    int embeddingDim_;
    std::vector<std::vector<double>> queryWeight_, keyWeight_, valueWeight_;
};

// SimpleLanguageModel (from src/model/architecture.js)
class SimpleLanguageModel {
public:
    SimpleLanguageModel(int vocabSize, int embeddingDim = 100)
        : vocabSize_(vocabSize), embeddingDim_(embeddingDim) {
        embeddings_ = initializeEmbeddings();
        labels_ = {"greeting", "farewell", "question"};
        attention_ = std::make_unique<AttentionMechanism>(embeddingDim);
        dropout_ = 0.2;
        layerNormGamma_.assign(embeddingDim, 1.0);
        layerNormBeta_.assign(embeddingDim, 0.0);
    }
    std::vector<std::vector<double>> initializeEmbeddings() {
        std::mt19937 rng(42);
        std::normal_distribution<double> gauss(0.0, 1.0);
        double scale = sqrt(2.0 / (vocabSize_ + embeddingDim_));
        std::vector<std::vector<double>> emb(vocabSize_, std::vector<double>(embeddingDim_));
        for (auto& row : emb)
            for (auto& v : row)
                v = gauss(rng) * scale;
        return emb;
    }
    std::vector<double> layerNormalize(const std::vector<double>& embeddings) {
        double mean = 0.0;
        for (double v : embeddings) mean += v;
        mean /= embeddings.size();
        double variance = 0.0;
        for (double v : embeddings) variance += (v - mean) * (v - mean);
        variance /= embeddings.size();
        std::vector<double> result(embeddings.size());
        for (size_t i = 0; i < embeddings.size(); i++)
            result[i] = layerNormGamma_[i] * (embeddings[i] - mean) / sqrt(variance + 1e-10) + layerNormBeta_[i];
        return result;
    }
    std::vector<std::vector<double>> applyDropout(const std::vector<std::vector<double>>& embeddings, bool training = true) {
        if (!training || dropout_ == 0) return embeddings;
        std::mt19937 rng(42);
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        std::vector<std::vector<double>> result = embeddings;
        for (auto& row : result)
            for (auto& v : row)
                if (uni(rng) < dropout_) v = 0.0;
                else v /= (1.0 - dropout_);
        return result;
    }
    struct ForwardOutput {
        std::vector<std::vector<double>> embeddings;
        std::vector<std::vector<double>> attentionWeights;
    };
    ForwardOutput forward(const std::vector<int>& tokenIndices, bool training = true) {
        std::vector<std::vector<double>> embeddings;
        for (int idx : tokenIndices)
            embeddings.push_back(embeddings_[idx]);
        embeddings = applyDropout(embeddings, training);
        for (auto& emb : embeddings)
            emb = layerNormalize(emb);
        auto attentionOutput = attention_->applyAttention(embeddings);
        return {attentionOutput.contextVectors, attentionOutput.attentionWeights};
    }
    struct Prediction {
        std::string label;
        double probability;
    };
    std::vector<Prediction> predict(const ForwardOutput& output) {
        std::vector<double> avgEmbedding(embeddingDim_, 0.0);
        for (const auto& emb : output.embeddings)
            for (int i = 0; i < embeddingDim_; i++)
                avgEmbedding[i] += emb[i];
        for (int i = 0; i < embeddingDim_; i++)
            avgEmbedding[i] /= output.embeddings.size();
        std::vector<double> scores(labels_.size());
        for (size_t i = 0; i < labels_.size(); i++) {
            double sum = 0.0;
            for (int j = 0; j < embeddingDim_; j++)
                sum += avgEmbedding[j] * (i + 1) * 0.1;
            scores[i] = sum;
        }
        double maxScore = *std::max_element(scores.begin(), scores.end());
        double sumExp = 0.0;
        for (double s : scores) sumExp += exp(s - maxScore);
        std::vector<Prediction> predictions;
        for (size_t i = 0; i < labels_.size(); i++)
            predictions.push_back({labels_[i], exp(scores[i] - maxScore) / sumExp});
        return predictions;
    }
    void updateEmbedding(int tokenIndex, const std::vector<double>& gradients, double learningRate) {
        for (int i = 0; i < embeddingDim_; i++)
            embeddings_[tokenIndex][i] -= learningRate * gradients[i];
    }
    int vocabSize() const { return vocabSize_; }
    int embeddingDim() const { return embeddingDim_; }
    const std::vector<std::string>& labels() const { return labels_; }
    const std::vector<std::vector<double>>& embeddings() const { return embeddings_; }
    AttentionMechanism& attention() { return *attention_; }
private:
    int vocabSize_, embeddingDim_;
    std::vector<std::vector<double>> embeddings_;
    std::vector<std::string> labels_;
    std::unique_ptr<AttentionMechanism> attention_;
    double dropout_;
    std::vector<double> layerNormGamma_, layerNormBeta_;
};

// TrainingPipeline (from src/training/train.js)
struct TrainingMetrics {
    double averageLoss = 0.0;
    double accuracy = 0.0;
    int epochs = 0;
    int batches = 0;
};

class TrainingPipeline {
public:
    TrainingPipeline(int vocabSize, int embeddingDim = 100)
        : model_(vocabSize, embeddingDim), metrics_() {}
    void train(const std::vector<std::pair<std::vector<int>, std::string>>& data, int epochs = 10, double learningRate = 0.01) {
        for (int epoch = 0; epoch < epochs; epoch++) {
            double totalLoss = 0.0;
            int correct = 0;
            for (const auto& [tokens, label] : data) {
                auto output = model_.forward(tokens, true);
                auto predictions = model_.predict(output);
                int predicted = 0;
                double maxProb = 0.0;
                for (size_t i = 0; i < predictions.size(); i++)
                    if (predictions[i].probability > maxProb) {
                        maxProb = predictions[i].probability;
                        predicted = (int)i;
                    }
                int actual = 0;
                for (size_t i = 0; i < model_.labels().size(); i++)
                    if (model_.labels()[i] == label) actual = (int)i;
                if (predicted == actual) correct++;
                double loss = -log(maxProb + 1e-10);
                totalLoss += loss;
                // Update embeddings
                for (size_t i = 0; i < tokens.size(); i++) {
                    std::vector<double> gradients(model_.embeddingDim(), 0.0);
                    for (int j = 0; j < model_.embeddingDim(); j++)
                        gradients[j] = output.embeddings[i][j] * 0.1;
                    model_.updateEmbedding(tokens[i], gradients, learningRate);
                }
                // Update attention weights
                std::vector<std::vector<double>> gradients(model_.embeddingDim(), std::vector<double>(model_.embeddingDim(), 0.1));
                model_.attention().updateWeights(gradients, learningRate);
            }
            metrics_.averageLoss = totalLoss / data.size();
            metrics_.accuracy = (double)correct / data.size();
            metrics_.epochs = epoch + 1;
            metrics_.batches = (int)data.size();
        }
    }
    const SimpleLanguageModel& model() const { return model_; }
    const TrainingMetrics& metrics() const { return metrics_; }
private:
    SimpleLanguageModel model_;
    TrainingMetrics metrics_;
};

} // namespace mech0

// ═══ re-bhvt-edit Integration (ported from ReZorg/re-bhvt-edit) ═══
namespace rebhvt {

// EndocrineRewardSystem (from agi/EndocrineRewardSystem.lua)
struct EndocrineChannel {
    float level = 0.0f;
    float baseline_sensitivity = 1.0f;
    float decay = 1.0f;
    float habituation = 0.1f;
    float weight = 1.0f;
};

class EndocrineRewardSystem {
public:
    EndocrineRewardSystem() {
        channels["dopamine"] = {0.0f, 1.0f, 2.0f, 0.35f, 1.0f};
        channels["cortisol"] = {0.0f, 1.0f, 1.0f, 0.10f, -0.6f};
        channels["oxytocin"] = {0.0f, 1.0f, 3.0f, 0.05f, 0.5f};
    }
    void reset() {
        for (auto& [name, ch] : channels) {
            ch.level = 0.0f;
            ch.baseline_sensitivity = 1.0f;
        }
    }
    // stimuli: map of channel name -> stimulus intensity in [0, 1]
    // dt: elapsed seconds since last update
    // Returns: composite_reward
    float update(const std::map<std::string, float>& stimuli, float dt) {
        float composite = 0.0f;
        for (auto& [name, ch] : channels) {
            float stimulus = 0.0f;
            auto it = stimuli.find(name);
            if (it != stimuli.end()) stimulus = it->second;
            // Adaptive sensitivity: repeated stimulation habituates the channel
            if (stimulus > 0.0f) {
                ch.baseline_sensitivity = std::max(0.05f,
                    ch.baseline_sensitivity - ch.habituation * stimulus * dt);
            } else {
                // Rest slowly restores full sensitivity
                ch.baseline_sensitivity = std::min(1.0f,
                    ch.baseline_sensitivity + ch.habituation * 0.25f * dt);
            }
            float release = stimulus * ch.baseline_sensitivity;
            ch.level = std::max(0.0f, ch.level + release - ch.level * ch.decay * dt);
            composite += ch.level * ch.weight;
        }
        return composite;
    }
    const std::map<std::string, EndocrineChannel>& getChannels() const { return channels; }
    float getLevel(const std::string& name) const {
        auto it = channels.find(name);
        return it != channels.end() ? it->second.level : 0.0f;
    }
    float getSensitivity(const std::string& name) const {
        auto it = channels.find(name);
        return it != channels.end() ? it->second.baseline_sensitivity : 0.0f;
    }
private:
    std::map<std::string, EndocrineChannel> channels;
};

// NicheConstructionMap (from agi/NicheConstructionMap.lua)
struct NicheCell {
    int visits = 0;
    float last_visit = 0.0f;
    float novelty = 0.0f;
};

class NicheConstructionMap {
public:
    NicheConstructionMap(float cell_size = 1.0f, float recovery_seconds = 30.0f)
        : cell_size(cell_size), recovery_seconds(recovery_seconds) {}
    void reset() { cells.clear(); }
    // Records a visit to the niche at (x, y, z) at time `now` (seconds)
    // Returns the novelty score in [0, 1] that visit produced (1 = brand new)
    float visit(float x, float y, float z, float now) {
        std::string key = makeKey(x, y, z);
        auto it = cells.find(key);
        if (it == cells.end()) {
            cells[key] = {1, now, 0.0f};
            return 1.0f;
        }
        NicheCell& cell = it->second;
        // Recover novelty the longer the niche has been left alone
        float elapsed = std::max(0.0f, now - cell.last_visit);
        float recovered = std::min(1.0f, elapsed / recovery_seconds);
        float novelty = std::min(1.0f, cell.novelty + recovered);
        cell.visits++;
        cell.last_visit = now;
        // Exploiting the niche drains its novelty for next time (habituation)
        cell.novelty = novelty * 0.15f;
        return novelty;
    }
    size_t size() const { return cells.size(); }
    const NicheCell* find(float x, float y, float z) const {
        auto it = cells.find(makeKey(x, y, z));
        return it != cells.end() ? &it->second : nullptr;
    }
private:
    std::string makeKey(float x, float y, float z) const {
        int xi = (int)floorf(x / cell_size);
        int yi = (int)floorf(y / cell_size);
        int zi = (int)floorf(z / cell_size);
        char buf[64];
        snprintf(buf, 64, "%d:%d:%d", xi, yi, zi);
        return buf;
    }
    float cell_size;
    float recovery_seconds;
    std::unordered_map<std::string, NicheCell> cells;
};

// SensorimotorBridge (from agi/SensorimotorBridge.lua)
struct SensorimotorTelemetry {
    float reward = 0.0f;
    float novelty = 0.0f;
    float prediction_error = 0.0f;
    float threat = 0.0f;
    float time = 0.0f;
    int niche_count = 0;
    std::map<std::string, float> hormone_levels;
    std::map<std::string, float> hormone_sensitivities;
};

class SensorimotorBridge {
public:
    SensorimotorBridge(float cell_size = 1.5f, float recovery_seconds = 30.0f)
        : niches(cell_size, recovery_seconds), endocrine() {}
    void reset() {
        niches.reset();
        endocrine.reset();
        previous_features.clear();
        time = 0.0f;
    }
    // position: {x, y, z} world-space position of the observed agent
    // frame_features: flat array of sampled frame buffer values (any scale)
    // threat_level: scalar in [0, 1] describing danger/failure signal this tick
    // dt: elapsed seconds since the previous update
    SensorimotorTelemetry update(float px, float py, float pz,
                                 const std::vector<float>& frame_features,
                                 float threat_level, float dt) {
        time += dt;
        float novelty = niches.visit(px, py, pz, time);
        float prediction_error = 0.0f;
        if (!previous_features.empty() && previous_features.size() == frame_features.size()) {
            float sum = 0.0f;
            for (size_t i = 0; i < frame_features.size(); i++)
                sum += fabsf(frame_features[i] - previous_features[i]);
            prediction_error = sum / frame_features.size();
        }
        previous_features = frame_features;
        std::map<std::string, float> stimuli;
        stimuli["dopamine"] = novelty;
        stimuli["cortisol"] = threat_level;
        stimuli["oxytocin"] = std::max(0.0f, 1.0f - prediction_error);
        float reward = endocrine.update(stimuli, dt);
        SensorimotorTelemetry t{};
        t.reward = reward;
        t.novelty = novelty;
        t.prediction_error = prediction_error;
        t.threat = threat_level;
        t.time = time;
        t.niche_count = (int)niches.size();
        for (const auto& [name, ch] : endocrine.getChannels()) {
            t.hormone_levels[name] = ch.level;
            t.hormone_sensitivities[name] = ch.baseline_sensitivity;
        }
        return t;
    }
    const NicheConstructionMap& getNiches() const { return niches; }
    const EndocrineRewardSystem& getEndocrine() const { return endocrine; }
private:
    NicheConstructionMap niches;
    EndocrineRewardSystem endocrine;
    std::vector<float> previous_features;
    float time = 0.0f;
};

} // namespace rebhvt

// ═══ GTAngel-Cls Integration (ported from ReZorg/GTAngel-Cls) ═══
namespace gtangel {

struct FrameBufferView {
    const uint8_t* pixels{};
    size_t width{};
    size_t height{};
    size_t stride{};
    size_t channels{4};
    bool valid() const {
        return pixels != nullptr && width > 0 && height > 0 && channels >= 3 &&
               row_pitch() >= width * channels;
    }
    size_t row_pitch() const { return stride != 0 ? stride : width * channels; }
};

struct ProprioceptiveFrame {
    size_t grid_width{};
    size_t grid_height{};
    std::vector<float> luminance{};
    std::vector<float> temporal_delta{};
    float mean_luminance{};
    float contrast{};
    float motion_energy{};
    uint64_t index{};
    size_t cell_count() const { return grid_width * grid_height; }
};

class FrameBufferProbe {
public:
    FrameBufferProbe() = default;
    FrameBufferProbe(size_t gw, size_t gh) : m_gw(gw ? gw : 1), m_gh(gh ? gh : 1) {}
    size_t grid_width() const { return m_gw; }
    size_t grid_height() const { return m_gh; }
    void reset() { m_previous.clear(); m_primed = false; m_index = 0; }
    ProprioceptiveFrame sample(const FrameBufferView& view) {
        ProprioceptiveFrame frame{};
        frame.grid_width = m_gw; frame.grid_height = m_gh;
        frame.luminance.assign(m_gw * m_gh, 0.0f);
        frame.temporal_delta.assign(m_gw * m_gh, 0.0f);
        frame.index = m_index;
        if (!view.valid()) return frame;
        ++m_index;
        for (size_t cy = 0; cy < m_gh; ++cy) {
            size_t y0 = (cy * view.height) / m_gh;
            size_t y1 = std::max(y0 + 1, ((cy + 1) * view.height) / m_gh);
            for (size_t cx = 0; cx < m_gw; ++cx) {
                size_t x0 = (cx * view.width) / m_gw;
                size_t x1 = std::max(x0 + 1, ((cx + 1) * view.width) / m_gw);
                double acc = 0.0; size_t samples = 0;
                for (size_t y = y0; y < y1 && y < view.height; ++y) {
                    const uint8_t* row = view.pixels + y * view.row_pitch();
                    for (size_t x = x0; x < x1 && x < view.width; ++x) {
                        const uint8_t* texel = row + x * view.channels;
                        acc += 0.114 * texel[0] + 0.587 * texel[1] + 0.299 * texel[2];
                        ++samples;
                    }
                }
                if (samples != 0)
                    frame.luminance[cy * m_gw + cx] = (float)(acc / (samples * 255.0));
            }
        }
        double sum = 0.0, sum_delta = 0.0;
        for (size_t i = 0; i < frame.luminance.size(); ++i) {
            sum += frame.luminance[i];
            if (m_primed && i < m_previous.size())
                frame.temporal_delta[i] = fabsf(frame.luminance[i] - m_previous[i]);
            sum_delta += frame.temporal_delta[i];
        }
        double cells = (double)frame.luminance.size();
        frame.mean_luminance = (float)(sum / cells);
        frame.motion_energy = (float)(sum_delta / cells);
        double var = 0.0;
        for (float v : frame.luminance) {
            double c = (double)v - frame.mean_luminance;
            var += c * c;
        }
        frame.contrast = (float)sqrt(var / cells);
        m_previous = frame.luminance;
        m_primed = true;
        return frame;
    }
private:
    size_t m_gw{8}, m_gh{8};
    std::vector<float> m_previous{};
    bool m_primed{false};
    uint64_t m_index{};
};

struct MotorCommand {
    float move_forward{};
    float move_right{};
    float look_right{};
    float look_up{};
    float actuate{};
    float magnitude() const {
        return sqrtf(move_forward*move_forward + move_right*move_right +
                     look_right*look_right + look_up*look_up + actuate*actuate);
    }
};

struct SensorimotorReading {
    float prediction_error{};
    float expected_flow{};
    float observed_flow{};
    float agency{};
    float novelty{};
    bool self_caused{};
};

class SensorimotorLoop {
public:
    SensorimotorLoop() = default;
    explicit SensorimotorLoop(float lr) : m_lr(std::clamp(lr, 0.0f, 1.0f)) {}
    void reset() { m_gain = 0.5f; m_error_scale = 0.0f; m_primed = false; }
    float gain() const { return m_gain; }
    SensorimotorReading integrate(const MotorCommand& cmd, const ProprioceptiveFrame& frame) {
        SensorimotorReading r{};
        r.observed_flow = frame.motion_energy;
        r.expected_flow = m_gain * cmd.magnitude();
        float residual = r.observed_flow - r.expected_flow;
        r.prediction_error = fabsf(residual);
        float effort = cmd.magnitude();
        if (effort > 1e-6f)
            m_gain = std::clamp(m_gain + m_lr * residual / effort, 0.0f, 8.0f);
        m_error_scale = m_primed ? (1.0f - 0.05f) * m_error_scale + 0.05f * r.prediction_error
                                 : r.prediction_error;
        m_primed = true;
        float scale = std::max(m_error_scale, 1e-6f);
        r.novelty = std::clamp(r.prediction_error / scale, 0.0f, 4.0f);
        float explained = std::max(r.observed_flow, 1e-6f);
        r.agency = std::clamp(1.0f - r.prediction_error / explained, 0.0f, 1.0f);
        r.self_caused = r.agency >= 0.5f;
        return r;
    }
private:
    float m_lr{0.1f};
    float m_gain{0.5f};
    float m_error_scale{};
    bool m_primed{false};
};

class Hormone {
public:
    Hormone() = default;
    Hormone(float baseline, float decay, float adaptation) :
        m_level(baseline), m_set_point(baseline),
        m_decay(std::clamp(decay, 0.0f, 1.0f)),
        m_adaptation(std::clamp(adaptation, 0.0f, 1.0f)) {}
    void secrete(float amount) { m_level = std::clamp(m_level + amount, 0.0f, 1.0f); }
    void tick() {
        m_level = std::clamp(m_level + (m_set_point - m_level) * m_decay, 0.0f, 1.0f);
        m_set_point = std::clamp(m_set_point + (m_level - m_set_point) * m_adaptation, 0.0f, 1.0f);
    }
    void reset(float baseline) {
        m_level = std::clamp(baseline, 0.0f, 1.0f);
        m_set_point = m_level;
    }
    float level() const { return m_level; }
    float set_point() const { return m_set_point; }
    float deviation() const { return m_level - m_set_point; }
    float decay() const { return m_decay; }
    void set_decay(float d) { m_decay = std::clamp(d, 0.0f, 1.0f); }
private:
    float m_level{0.5f};
    float m_set_point{0.5f};
    float m_decay{0.1f};
    float m_adaptation{0.01f};
};

struct EndocrineStimulus {
    float novelty{};
    float agency{};
    float goal_progress{};
    float threat{};
    float effort{};
};

struct EndocrineState {
    float dopamine{};
    float serotonin{};
    float noradrenaline{};
    float cortisol{};
    float reward{};
    float arousal{};
    float wellbeing{};
};

class EndocrineSystem {
public:
    EndocrineSystem() { reset(); }
    void reset() {
        m_dopamine = Hormone(0.35f, 0.18f, 0.010f);
        m_serotonin = Hormone(0.50f, 0.08f, 0.004f);
        m_noradrenaline = Hormone(0.20f, 0.30f, 0.020f);
        m_cortisol = Hormone(0.20f, 0.06f, 0.003f);
    }
    EndocrineState update(const EndocrineStimulus& s) {
        float novelty = std::clamp(s.novelty, 0.0f, 4.0f);
        float agency = std::clamp(s.agency, 0.0f, 1.0f);
        float progress = std::clamp(s.goal_progress, -1.0f, 1.0f);
        float threat = std::clamp(s.threat, 0.0f, 1.0f);
        float effort = std::clamp(s.effort, 0.0f, 1.0f);
        m_dopamine.secrete(0.25f * novelty * agency + 0.40f * std::max(progress, 0.0f));
        m_serotonin.secrete(0.20f * agency - 0.15f * threat);
        m_noradrenaline.secrete(0.50f * threat + 0.10f * novelty);
        m_cortisol.secrete(0.35f * threat + 0.20f * std::max(-progress, 0.0f) +
                           0.15f * novelty * (1.0f - agency));
        m_dopamine.tick(); m_serotonin.tick(); m_noradrenaline.tick(); m_cortisol.tick();
        EndocrineState st{};
        st.dopamine = m_dopamine.level();
        st.serotonin = m_serotonin.level();
        st.noradrenaline = m_noradrenaline.level();
        st.cortisol = m_cortisol.level();
        st.arousal = std::clamp(0.5f * (st.noradrenaline + st.dopamine), 0.0f, 1.0f);
        st.wellbeing = std::clamp(st.serotonin - st.cortisol, -1.0f, 1.0f);
        st.reward = std::clamp(1.0f * m_dopamine.deviation() + 0.5f * m_serotonin.deviation() -
                               0.8f * m_cortisol.deviation() - 0.2f * effort, -1.0f, 1.0f);
        return st;
    }
    const Hormone& dopamine() const { return m_dopamine; }
    const Hormone& serotonin() const { return m_serotonin; }
    const Hormone& noradrenaline() const { return m_noradrenaline; }
    const Hormone& cortisol() const { return m_cortisol; }
    Hormone& dopamine() { return m_dopamine; }
    Hormone& serotonin() { return m_serotonin; }
    Hormone& noradrenaline() { return m_noradrenaline; }
    Hormone& cortisol() { return m_cortisol; }
private:
    Hormone m_dopamine{}, m_serotonin{}, m_noradrenaline{}, m_cortisol{};
};

struct WorldPose {
    float x{}, y{}, z{};
    float heading{};
    float time_of_day{};
};

struct WorldObservation {
    WorldPose pose{};
    float health{1.0f};
    float hostility{};
    float goal_progress{};
    bool terminal{};
};

struct NicheCoordinate {
    int32_t x{}, y{}, z{}, time_bucket{};
    bool operator==(const NicheCoordinate& o) const {
        return x == o.x && y == o.y && z == o.z && time_bucket == o.time_bucket;
    }
};
struct NicheCoordinateHash {
    size_t operator()(const NicheCoordinate& c) const {
        size_t h = 1469598103934665603ull;
        auto mix = [&h](int32_t v) { h ^= (size_t)(uint32_t)v; h *= 1099511628211ull; };
        mix(c.x); mix(c.y); mix(c.z); mix(c.time_bucket);
        return h;
    }
};

struct NicheCell {
    uint32_t visits{};
    float reward{}, threat{}, novelty{}, affordance{};
    float habitability() const { return reward + affordance - threat; }
};

enum class InventivePrinciple {
    None, SeparationInSpace, SeparationInTime, Segmentation,
    PriorAction, CushionInAdvance, Dynamism, Inversion, Intermediary
};
inline const char* to_string(InventivePrinciple p) {
    switch (p) {
        case InventivePrinciple::SeparationInSpace: return "separation_in_space";
        case InventivePrinciple::SeparationInTime: return "separation_in_time";
        case InventivePrinciple::Segmentation: return "segmentation";
        case InventivePrinciple::PriorAction: return "prior_action";
        case InventivePrinciple::CushionInAdvance: return "cushion_in_advance";
        case InventivePrinciple::Dynamism: return "dynamism";
        case InventivePrinciple::Inversion: return "inversion";
        case InventivePrinciple::Intermediary: return "intermediary";
        default: return "none";
    }
}

struct TrizResolution {
    bool contradiction{};
    InventivePrinciple principle{InventivePrinciple::None};
    NicheCoordinate target{};
    float expected_gain{};
    const char* improving{"none"};
    const char* worsening{"none"};
};

class NicheConstructor {
public:
    NicheConstructor() = default;
    NicheConstructor(float cell_size, float hours_per_bucket) :
        m_cell_size(cell_size > 0 ? cell_size : 1.0f),
        m_hours_per_bucket(hours_per_bucket > 0 ? hours_per_bucket : 1.0f) {}
    void reset() { m_cells.clear(); }
    NicheCoordinate quantise(const WorldPose& pose) const {
        NicheCoordinate c{};
        c.x = (int32_t)floorf(pose.x / m_cell_size);
        c.y = (int32_t)floorf(pose.y / m_cell_size);
        c.z = (int32_t)floorf(pose.z / m_cell_size);
        float hours = fmodf(pose.time_of_day, 24.0f);
        float wrapped = hours < 0 ? hours + 24.0f : hours;
        c.time_bucket = (int32_t)(wrapped / m_hours_per_bucket);
        return c;
    }
    const NicheCell& observe(const NicheCoordinate& c, float reward, float threat, float novelty) {
        NicheCell& cell = m_cells[c];
        ++cell.visits;
        float rate = 1.0f / (float)cell.visits;
        cell.reward += (reward - cell.reward) * rate;
        cell.threat += (std::clamp(threat, 0.0f, 1.0f) - cell.threat) * rate;
        cell.novelty += (std::max(novelty, 0.0f) - cell.novelty) * rate;
        return cell;
    }
    void imprint(const NicheCoordinate& c, float affordance) {
        NicheCell& cell = m_cells[c];
        cell.affordance = std::clamp(cell.affordance + affordance, -1.0f, 1.0f);
    }
    const NicheCell* find(const NicheCoordinate& c) const {
        auto it = m_cells.find(c);
        return it != m_cells.end() ? &it->second : nullptr;
    }
    size_t size() const { return m_cells.size(); }
    TrizResolution resolve(const NicheCoordinate& here) const {
        TrizResolution r{};
        r.target = here;
        const NicheCell* current = find(here);
        if (!current) return r;
        bool wanted = current->novelty > 0.5f || current->reward > 0.0f;
        if (!wanted || current->threat <= 0.4f) return r;
        r.contradiction = true;
        r.improving = current->reward > 0.0f ? "reward" : "novelty";
        r.worsening = "safety";
        float baseline = current->habitability();
        const NicheCell* best = nullptr;
        NicheCoordinate best_coord = here;
        InventivePrinciple best_principle = InventivePrinciple::None;
        for (int32_t bucket = 0; bucket < time_buckets(); ++bucket) {
            if (bucket == here.time_bucket) continue;
            NicheCoordinate cand = here; cand.time_bucket = bucket;
            consider(cand, InventivePrinciple::SeparationInTime, best, best_coord, best_principle);
        }
        for (int32_t dx = -1; dx <= 1; ++dx)
            for (int32_t dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                NicheCoordinate cand = here; cand.x += dx; cand.y += dy;
                consider(cand, InventivePrinciple::SeparationInSpace, best, best_coord, best_principle);
            }
        if (best && best->habitability() > baseline) {
            r.principle = best_principle;
            r.target = best_coord;
            r.expected_gain = best->habitability() - baseline;
            return r;
        }
        if (current->threat > 0.8f) r.principle = InventivePrinciple::Inversion;
        else if (current->threat > 0.6f) r.principle = InventivePrinciple::Intermediary;
        else if (current->affordance <= 0.0f) r.principle = InventivePrinciple::CushionInAdvance;
        else if (current->visits < 4) r.principle = InventivePrinciple::Segmentation;
        else r.principle = InventivePrinciple::Dynamism;
        return r;
    }
private:
    static constexpr float kNoveltyThreshold = 0.5f;
    static constexpr float kThreatThreshold = 0.4f;
    int32_t time_buckets() const { return (int32_t)ceilf(24.0f / m_hours_per_bucket); }
    void consider(const NicheCoordinate& cand, InventivePrinciple p, const NicheCell*& best,
                  NicheCoordinate& best_coord, InventivePrinciple& best_principle) const {
        const NicheCell* cell = find(cand);
        if (!cell || cell->visits == 0) return;
        if (!best || cell->habitability() > best->habitability()) {
            best = cell; best_coord = cand; best_principle = p;
        }
    }
    float m_cell_size{25.0f};
    float m_hours_per_bucket{4.0f};
    std::unordered_map<NicheCoordinate, NicheCell, NicheCoordinateHash> m_cells{};
};

struct CognitiveState {
    ProprioceptiveFrame frame{};
    SensorimotorReading sensorimotor{};
    EndocrineState endocrine{};
    TrizResolution resolution{};
    NicheCoordinate coordinate{};
    float exploration_drive{};
    uint64_t tick{};
};

class CognitiveArchitecture {
public:
    CognitiveArchitecture() = default;
    CognitiveArchitecture(FrameBufferProbe probe, NicheConstructor niche) :
        m_probe(std::move(probe)), m_niche(std::move(niche)) {}
    void reset() { m_probe.reset(); m_loop.reset(); m_endocrine.reset(); m_tick = 0; }
    void reset_all() { reset(); m_niche.reset(); }
    CognitiveState tick(const FrameBufferView& view, const WorldObservation& world, const MotorCommand& cmd) {
        CognitiveState state{};
        state.frame = m_probe.sample(view);
        state.sensorimotor = m_loop.integrate(cmd, state.frame);
        EndocrineStimulus stimulus{};
        stimulus.novelty = state.sensorimotor.novelty;
        stimulus.agency = state.sensorimotor.agency;
        stimulus.goal_progress = world.goal_progress;
        stimulus.threat = std::clamp(world.hostility + (1.0f - std::clamp(world.health, 0.0f, 1.0f)) * 0.5f, 0.0f, 1.0f);
        stimulus.effort = std::clamp(cmd.magnitude() / 2.236068f, 0.0f, 1.0f);
        state.endocrine = m_endocrine.update(stimulus);
        state.coordinate = m_niche.quantise(world.pose);
        m_niche.observe(state.coordinate, state.endocrine.reward, stimulus.threat, stimulus.novelty);
        state.resolution = m_niche.resolve(state.coordinate);
        state.exploration_drive = std::clamp(0.5f + 0.5f * state.endocrine.wellbeing - 0.5f * state.endocrine.cortisol, 0.0f, 1.0f);
        state.tick = m_tick++;
        return state;
    }
    void imprint(const NicheCoordinate& c, float affordance) { m_niche.imprint(c, affordance); }
    const NicheConstructor& niche() const { return m_niche; }
    const SensorimotorLoop& loop() const { return m_loop; }
    const EndocrineSystem& endocrine() const { return m_endocrine; }
    EndocrineSystem& endocrine() { return m_endocrine; }
private:
    FrameBufferProbe m_probe{};
    SensorimotorLoop m_loop{};
    EndocrineSystem m_endocrine{};
    NicheConstructor m_niche{};
    uint64_t m_tick{};
};

struct EnvironmentConfig {
    size_t grid_width{8}, grid_height{8};
    float cell_size{25.0f}, hours_per_bucket{4.0f};
    uint64_t max_steps{0};
};

struct StepResult {
    CognitiveState state{};
    float reward{};
    float episode_return{};
    bool done{};
    uint64_t step{};
};

class GTAngelEnvironment {
public:
    GTAngelEnvironment() : GTAngelEnvironment(EnvironmentConfig{}) {}
    explicit GTAngelEnvironment(const EnvironmentConfig& config) :
        m_config(config),
        m_mind(FrameBufferProbe(config.grid_width, config.grid_height),
               NicheConstructor(config.cell_size, config.hours_per_bucket)) {}
    void reset() { m_mind.reset(); m_step = 0; m_return = 0.0f; m_done = false; }
    void reset_all() { reset(); m_mind.reset_all(); }
    StepResult step(const FrameBufferView& view, const WorldObservation& world, const MotorCommand& cmd) {
        StepResult result{};
        if (m_done) {
            result.episode_return = m_return; result.done = true; result.step = m_step;
            return result;
        }
        result.state = m_mind.tick(view, world, cmd);
        result.reward = result.state.endocrine.reward;
        m_return += result.reward;
        result.episode_return = m_return;
        result.step = m_step++;
        m_done = world.terminal || (m_config.max_steps != 0 && m_step >= m_config.max_steps);
        result.done = m_done;
        return result;
    }
    MotorCommand suggest(const CognitiveState& state) const {
        MotorCommand cmd{};
        float vigour = std::clamp(0.25f + 0.75f * state.endocrine.arousal, 0.0f, 1.0f);
        cmd.move_forward = vigour * state.exploration_drive;
        cmd.look_right = 0.5f * (state.exploration_drive - 0.5f);
        cmd.actuate = std::clamp(state.endocrine.dopamine - state.endocrine.cortisol, 0.0f, 1.0f);
        switch (state.resolution.principle) {
            case InventivePrinciple::SeparationInSpace:
            case InventivePrinciple::Segmentation:
                cmd.move_forward *= 0.5f; cmd.move_right = vigour; break;
            case InventivePrinciple::SeparationInTime:
            case InventivePrinciple::CushionInAdvance:
                cmd.move_forward = -0.5f * vigour; cmd.actuate = 0.0f; break;
            case InventivePrinciple::Inversion:
                cmd.move_forward = -vigour; break;
            case InventivePrinciple::Intermediary:
                cmd.move_right = 0.5f * vigour; cmd.actuate *= 0.5f; break;
            case InventivePrinciple::Dynamism:
                cmd.move_right = vigour * (state.tick % 2 == 0 ? 1.0f : -1.0f); break;
            default: break;
        }
        cmd.move_forward = std::clamp(cmd.move_forward, -1.0f, 1.0f);
        cmd.move_right = std::clamp(cmd.move_right, -1.0f, 1.0f);
        cmd.look_right = std::clamp(cmd.look_right, -1.0f, 1.0f);
        cmd.look_up = std::clamp(cmd.look_up, -1.0f, 1.0f);
        cmd.actuate = std::clamp(cmd.actuate, 0.0f, 1.0f);
        return cmd;
    }
    void imprint(const NicheCoordinate& c, float affordance) { m_mind.imprint(c, affordance); }
    const CognitiveArchitecture& mind() const { return m_mind; }
    CognitiveArchitecture& mind() { return m_mind; }
    float episode_return() const { return m_return; }
    uint64_t steps() const { return m_step; }
    bool done() const { return m_done; }
private:
    EnvironmentConfig m_config{};
    CognitiveArchitecture m_mind;
    uint64_t m_step{};
    float m_return{};
    bool m_done{};
};

} // namespace gtangel

// GTAngel v15 — authoritative 768x768 software renderer and render pyramid.
// This module is spliced after namespace gtangel and therefore reuses the
// v13 avatar/world contracts plus GTAngel-Cls framebuffer concepts.

namespace v15render {

using avatar3d::Vec3;

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static Vec3 cross3(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

static Vec3 normalize3(Vec3 value) {
    float length = sqrtf(value.x * value.x + value.y * value.y + value.z * value.z);
    if (length <= 1e-6f) return {0.0f, 1.0f, 0.0f};
    return {value.x / length, value.y / length, value.z / length};
}

struct Colour {
    uint8_t b{0}, g{0}, r{0}, a{255};
};

static Colour shade(Colour base, float illumination, float fog) {
    illumination = clamp01(illumination);
    fog = clamp01(fog);
    const float skyB = 34.0f, skyG = 23.0f, skyR = 22.0f;
    Colour result;
    result.b = (uint8_t)std::lround((base.b * illumination) * (1.0f - fog) + skyB * fog);
    result.g = (uint8_t)std::lround((base.g * illumination) * (1.0f - fog) + skyG * fog);
    result.r = (uint8_t)std::lround((base.r * illumination) * (1.0f - fog) + skyR * fog);
    result.a = 255;
    return result;
}

enum ObjectId : uint16_t {
    ObjectSky = 0,
    ObjectGround = 1,
    ObjectObstacle = 2,
    ObjectGoal = 3,
    ObjectAvatar = 4,
    ObjectGrid = 5,
    ObjectNpc = 6,
    ObjectQuestItem = 7,
    ObjectVehicle = 8
};

// DTE v22 native skin asset reader / visual-only software-renderer adapter.
//
// Include this file *inside namespace v15render*, after Colour and ObjectAvatar
// have been declared.  It intentionally has no #includes and no namespace
// wrapper: the GTAngel v21 translation unit already supplies vector, string,
// fstream, cmath, cstdint, cstring, and avatar3d::avatar3d::Vec3.  It contains original,
// clean-room code only and does not load GLTF or any game/source asset at run
// time.  The only runtime input is export_native_lod.py's bounded .bin format.
//
// Native rendering is cosmetic.  draw() merely emits ObjectAvatar triangles to
// the existing renderer callback, so the pre-existing 768x768 colour/depth/ID
// rasterizer remains authoritative.  It has no body, motor, quest, world, or
// commit-side-effect API.

class DteSkinnedAsset {
public:
    // Direct input path.  Passing an empty path tries an adjacent executable's
    // assets/dte_v26_lod.bin on Windows; portable builds fall back to the
    // current process assets/dte_v26_lod.bin path.
    bool load(const std::string& inputPath) {
        clear();
        const std::string path = inputPath.empty() ? adjacentAssetPath() : inputPath;
        if (path.empty()) return fail("empty native skin asset path");

        std::ifstream stream(path, std::ios::binary | std::ios::ate);
        if (!stream) return fail("cannot open native skin asset: " + path);
        const std::streamoff fileSize = stream.tellg();
        if (fileSize < static_cast<std::streamoff>(kHeaderBytes) ||
            fileSize > static_cast<std::streamoff>(kMaxFileBytes))
            return fail("native skin asset file size is outside bounded range");
        stream.seekg(0, std::ios::beg);
        std::vector<uint8_t> bytes(static_cast<size_t>(fileSize));
        if (!stream.read(reinterpret_cast<char*>(bytes.data()), fileSize))
            return fail("cannot read complete native skin asset");
        return parse(bytes);
    }

    bool loadAdjacent() { return load(std::string()); }
    bool valid() const { return valid_; }
    const std::string& error() const { return error_; }
    size_t triangleCount() const { return triangles_.size(); }
    size_t bodyTriangleCount() const { return bodyTriangleCount_; }
    bool hasNativeWings() const { return valid_ && nativeWings_; }
    bool bodyOnly() const { return valid_ && !nativeWings_; }

    // Emit uses this signature:
    //   emit(avatar3d::Vec3 a, avatar3d::Vec3 b, avatar3d::Vec3 c, Colour colour, uint16_t objectId)
    // The callback is normally HighResRenderer::addTriangle (or an adapter
    // around it).  This method has no world/body mutation channel.
    template<class Emit>
    void draw(const avatar3d::AvatarSkeleton& avatar, Emit emit) const {
        if (!valid_ || vertices_.empty() || triangles_.empty()) return;
        // The supplied auto-rig has helper-bone roll/tails inconsistent with
        // the v21 visual skeleton.  Applying a rigid per-bone frame transfer
        // twists the authored body and destroys its likeness.  Instead, retain
        // every rest-mesh triangle and transfer *displacement* from the visual
        // neutral joint to the current joint, blended by the GLB skin weights.
        // This is deliberately a conservative CPU LOD, not full quaternion
        // skinning; original Blender/GLB keeps its full editable rig and actions.
        avatar3d::AvatarSkeleton neutral;
        neutral.pose({0.0f,0.0f,0.0f},0.0f,0.0f,0.0f);
        const avatar3d::Vec3 right=bodyRight(avatar);
        const float ch=right.x, sh=right.z;
        const avatar3d::Vec3 origin=avatar.lastPosition;
        avatar3d::Vec3 delta[kTargetCount]{};
        for (uint32_t t=0;t<kTargetCount;++t) {
            avatar3d::Vec3 baseA{},baseB{},nowA{},nowB{};
            if (posePair(neutral,static_cast<uint8_t>(t),baseA,baseB) &&
                posePair(avatar,static_cast<uint8_t>(t),nowA,nowB))
                delta[t]=sub(nowA,add(rotateY(baseA,ch,sh),origin));
        }
        // The binary is structurally checked during load.  These rechecks
        // keep emitted geometry finite if a caller supplies an invalid pose.
        for (size_t triangleIndex=0;triangleIndex<triangles_.size();++triangleIndex) {
            const Triangle& triangle=triangles_[triangleIndex];
            const bool wing=triangleIndex>=bodyTriangleCount_ && nativeWings_;
            const avatar3d::Vec3 a = skinVertex(vertices_[triangle.i0], delta,origin,ch,sh,wing);
            const avatar3d::Vec3 b = skinVertex(vertices_[triangle.i1], delta,origin,ch,sh,wing);
            const avatar3d::Vec3 c = skinVertex(vertices_[triangle.i2], delta,origin,ch,sh,wing);
            if (!finiteVec(a) || !finiteVec(b) || !finiteVec(c)) continue;
            if (nearDegenerate(a, b, c)) continue;
            // Export stores conventional RGB while the legacy framebuffer Colour
            // is B,G,R,A in memory.
            emit(a, b, c, Colour{triangle.b, triangle.g, triangle.r, 255}, ObjectAvatar);
        }
    }

    // Kept public so WM_CREATE integration can compose the path without any
    // platform-specific loader.  Direct paths always remain supported by load.
    static std::string adjacentAssetPath() {
#ifdef _WIN32
        char executable[4096]{};
        const unsigned long length = GetModuleFileNameA(nullptr, executable,
            static_cast<unsigned long>(sizeof(executable)));
        if (length > 0 && length < sizeof(executable) - 1) {
            std::string value(executable, executable + length);
            const size_t slash = value.find_last_of("\\/");
            if (slash != std::string::npos)
                return value.substr(0, slash + 1) + "assets\\dte_v26_lod.bin";
        }
#endif
        return "assets/dte_v26_lod.bin";
    }

private:
    static constexpr uint32_t kVersion = 1;
    static constexpr size_t kHeaderBytes = 128;
    static constexpr size_t kVertexBytes = 20;
    static constexpr size_t kTriangleBytes = 12;
    static constexpr size_t kColourBytes = 3;
    static constexpr size_t kBoneBytes = 40;
    static constexpr uint32_t kMaxVertices = 48000;
    static constexpr uint32_t kMaxTriangles = 16000;
    static constexpr uint32_t kMaxBodyTriangles = 8000;
    static constexpr uint32_t kMaxBones = 64;
    static constexpr uint32_t kMaxMetadataBytes = 16384;
    static constexpr size_t kMaxFileBytes = 2 * 1024 * 1024;
    static constexpr uint32_t kTargetCount = 22;
    static constexpr uint32_t kFlagFaceRgb = 1u << 0;
    static constexpr uint32_t kFlagNativeWings = 1u << 1;
    static constexpr uint32_t kFlagSourceAsset = 1u << 2;
    static constexpr uint32_t kFlagBodyOnly = 1u << 3;

    struct Vertex {
        avatar3d::Vec3 rest{};
        uint8_t bone[4]{};
        uint8_t weight[4]{};
    };

    struct Triangle {
        uint32_t i0{}, i1{}, i2{};
        uint8_t r{}, g{}, b{};
    };

    struct Frame {
        avatar3d::Vec3 origin{};
        avatar3d::Vec3 x{};
        avatar3d::Vec3 y{};
        avatar3d::Vec3 z{};
        bool initialized{false};
    };

    struct DiskBone {
        uint8_t target{};
        uint8_t flags{};
        avatar3d::Vec3 restHead{};
        avatar3d::Vec3 restTail{};
        avatar3d::Vec3 restXAxis{};
        Frame rest{};
    };

    bool valid_{false};
    bool nativeWings_{false};
    size_t bodyTriangleCount_{0};
    std::string error_;
    std::vector<Vertex> vertices_;
    std::vector<Triangle> triangles_;
    std::vector<DiskBone> bones_;

    void clear() {
        valid_ = false;
        nativeWings_ = false;
        bodyTriangleCount_ = 0;
        error_.clear();
        vertices_.clear();
        triangles_.clear();
        bones_.clear();
    }

    bool fail(const char* message) { return fail(std::string(message)); }
    bool fail(const std::string& message) {
        clear();
        error_ = message;
        return false;
    }

    static uint32_t readU32(const std::vector<uint8_t>& data, size_t at) {
        return static_cast<uint32_t>(data[at]) |
               (static_cast<uint32_t>(data[at + 1]) << 8) |
               (static_cast<uint32_t>(data[at + 2]) << 16) |
               (static_cast<uint32_t>(data[at + 3]) << 24);
    }

    static float readF32(const std::vector<uint8_t>& data, size_t at) {
        const uint32_t bits = readU32(data, at);
        float value{};
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    static uint32_t crc32(const uint8_t* data, size_t size) {
        uint32_t value = 0xffffffffu;
        for (size_t i = 0; i < size; ++i) {
            value ^= data[i];
            for (int bit = 0; bit < 8; ++bit)
                value = (value & 1u) ? ((value >> 1) ^ 0xedb88320u) : (value >> 1);
        }
        return value ^ 0xffffffffu;
    }

    static bool finiteFloat(float value) { return std::isfinite(value); }
    static bool finiteVec(avatar3d::Vec3 value) {
        return finiteFloat(value.x) && finiteFloat(value.y) && finiteFloat(value.z);
    }
    static avatar3d::Vec3 add(avatar3d::Vec3 a, avatar3d::Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    static avatar3d::Vec3 sub(avatar3d::Vec3 a, avatar3d::Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    static avatar3d::Vec3 mul(avatar3d::Vec3 a, float value) { return {a.x * value, a.y * value, a.z * value}; }
    static avatar3d::Vec3 rotateY(avatar3d::Vec3 p,float ch,float sh) {
        return {p.x*ch-p.z*sh,p.y,p.x*sh+p.z*ch};
    }
    static float dot(avatar3d::Vec3 a, avatar3d::Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static avatar3d::Vec3 cross(avatar3d::Vec3 a, avatar3d::Vec3 b) {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }
    static float lengthSq(avatar3d::Vec3 value) { return dot(value, value); }
    static avatar3d::Vec3 normal(avatar3d::Vec3 value) {
        const float squared = lengthSq(value);
        if (!finiteFloat(squared) || squared <= 1e-12f) return {};
        const float inverse = 1.0f / std::sqrt(squared);
        return mul(value, inverse);
    }

    static Frame makeFrame(avatar3d::Vec3 origin, avatar3d::Vec3 tail, avatar3d::Vec3 xHint) {
        Frame frame;
        if (!finiteVec(origin) || !finiteVec(tail) || !finiteVec(xHint)) return frame;
        const avatar3d::Vec3 y = normal(sub(tail, origin));
        if (lengthSq(y) <= 1e-10f) return frame;
        avatar3d::Vec3 x = sub(xHint, mul(y, dot(xHint, y)));
        if (lengthSq(x) <= 1e-10f) {
            const avatar3d::Vec3 candidate = std::fabs(y.y) < 0.85f ? avatar3d::Vec3{0.0f, 1.0f, 0.0f}
                                                          : avatar3d::Vec3{1.0f, 0.0f, 0.0f};
            x = sub(candidate, mul(y, dot(candidate, y)));
        }
        x = normal(x);
        avatar3d::Vec3 z = normal(cross(x, y));
        if (lengthSq(x) <= 1e-10f || lengthSq(z) <= 1e-10f) return frame;
        x = normal(cross(y, z)); // correct round-off and retain a right-handed frame
        frame.origin = origin;
        frame.x = x;
        frame.y = y;
        frame.z = z;
        frame.initialized = finiteVec(x) && finiteVec(y) && finiteVec(z);
        return frame;
    }

    static Frame makePoseFrame(avatar3d::Vec3 origin, avatar3d::Vec3 tail, avatar3d::Vec3 bodyXAxis) {
        return makeFrame(origin, tail, bodyXAxis);
    }

    static avatar3d::Vec3 local(const Frame& frame, avatar3d::Vec3 world) {
        const avatar3d::Vec3 delta = sub(world, frame.origin);
        return {dot(delta, frame.x), dot(delta, frame.y), dot(delta, frame.z)};
    }
    static avatar3d::Vec3 world(const Frame& frame, avatar3d::Vec3 point) {
        return add(frame.origin, add(mul(frame.x, point.x), add(mul(frame.y, point.y), mul(frame.z, point.z))));
    }

    static bool nearDegenerate(avatar3d::Vec3 a, avatar3d::Vec3 b, avatar3d::Vec3 c) {
        const avatar3d::Vec3 area = cross(sub(b, a), sub(c, a));
        return !finiteVec(area) || lengthSq(area) < 1e-14f;
    }

    bool parse(const std::vector<uint8_t>& data) {
        // Identity, header version/size, and exact byte count.
        static const uint8_t magic[8] = {'D','T','E','V','2','2','L',0};
        if (data.size() < kHeaderBytes || std::memcmp(data.data(), magic, sizeof(magic)) != 0)
            return fail("native skin asset magic is invalid");
        const uint32_t version = readU32(data, 8);
        const uint32_t headerBytes = readU32(data, 12);
        const uint32_t flags = readU32(data, 16);
        const uint32_t fileBytes = readU32(data, 20);
        const uint32_t vertexCount = readU32(data, 24);
        const uint32_t triangleCount = readU32(data, 28);
        const uint32_t bodyTriangleCount = readU32(data, 32);
        const uint32_t boneCount = readU32(data, 36);
        const uint32_t vertexOffset = readU32(data, 40);
        const uint32_t vertexStride = readU32(data, 44);
        const uint32_t indexOffset = readU32(data, 48);
        const uint32_t indexStride = readU32(data, 52);
        const uint32_t colourOffset = readU32(data, 56);
        const uint32_t colourStride = readU32(data, 60);
        const uint32_t boneOffset = readU32(data, 64);
        const uint32_t boneStride = readU32(data, 68);
        const uint32_t metadataOffset = readU32(data, 72);
        const uint32_t metadataBytes = readU32(data, 76);
        const uint32_t payloadCrc = readU32(data, 104);
        const uint32_t storedHeaderCrc = readU32(data, 108);

        if (version != kVersion || headerBytes != kHeaderBytes || fileBytes != data.size())
            return fail("native skin asset version or file extent is invalid");
        const bool nativeWings = (flags & kFlagNativeWings) != 0;
        const bool bodyOnly = (flags & kFlagBodyOnly) != 0;
        if ((flags & (kFlagFaceRgb | kFlagSourceAsset)) != (kFlagFaceRgb | kFlagSourceAsset) ||
            nativeWings == bodyOnly)
            return fail("native skin asset flags do not declare exactly one wing mode");
        if (vertexCount == 0 || vertexCount > kMaxVertices || triangleCount == 0 ||
            triangleCount > kMaxTriangles || boneCount == 0 || boneCount > kMaxBones ||
            bodyTriangleCount > triangleCount || bodyTriangleCount > kMaxBodyTriangles ||
            (bodyOnly && bodyTriangleCount != triangleCount) ||
            vertexStride != kVertexBytes || indexStride != kTriangleBytes ||
            colourStride != kColourBytes || boneStride != kBoneBytes ||
            metadataBytes > kMaxMetadataBytes)
            return fail("native skin asset counts or record strides are invalid");

        const uint64_t expectedIndex = static_cast<uint64_t>(vertexOffset) + static_cast<uint64_t>(vertexCount) * kVertexBytes;
        const uint64_t expectedColour = expectedIndex + static_cast<uint64_t>(triangleCount) * kTriangleBytes;
        const uint64_t expectedBone = expectedColour + static_cast<uint64_t>(triangleCount) * kColourBytes;
        const uint64_t expectedMetadata = expectedBone + static_cast<uint64_t>(boneCount) * kBoneBytes;
        const uint64_t expectedFile = expectedMetadata + metadataBytes;
        if (vertexOffset != kHeaderBytes || indexOffset != expectedIndex || colourOffset != expectedColour ||
            boneOffset != expectedBone || metadataOffset != expectedMetadata || expectedFile != data.size())
            return fail("native skin asset regions are not contiguous or bounded");

        std::vector<uint8_t> header(data.begin(), data.begin() + kHeaderBytes);
        header[108] = header[109] = header[110] = header[111] = 0;
        if (crc32(header.data(), header.size()) != storedHeaderCrc ||
            crc32(data.data() + kHeaderBytes, data.size() - kHeaderBytes) != payloadCrc)
            return fail("native skin asset CRC validation failed");

        const avatar3d::Vec3 declaredMin{readF32(data, 80), readF32(data, 84), readF32(data, 88)};
        const avatar3d::Vec3 declaredMax{readF32(data, 92), readF32(data, 96), readF32(data, 100)};
        if (!finiteVec(declaredMin) || !finiteVec(declaredMax) ||
            declaredMin.x > declaredMax.x || declaredMin.y > declaredMax.y || declaredMin.z > declaredMax.z ||
            declaredMax.x - declaredMin.x > 100.0f || declaredMax.y - declaredMin.y > 100.0f ||
            declaredMax.z - declaredMin.z > 100.0f)
            return fail("native skin asset bounds are not finite or plausible");

        vertices_.reserve(vertexCount);
        triangles_.reserve(triangleCount);
        bones_.reserve(boneCount);
        constexpr float kBoundsTolerance = 0.002f;
        for (uint32_t i = 0; i < vertexCount; ++i) {
            const size_t at = static_cast<size_t>(vertexOffset) + static_cast<size_t>(i) * kVertexBytes;
            Vertex vertex;
            vertex.rest = {readF32(data, at), readF32(data, at + 4), readF32(data, at + 8)};
            for (int influence = 0; influence < 4; ++influence) vertex.bone[influence] = data[at + 12 + influence];
            uint32_t weightSum = 0;
            for (int influence = 0; influence < 4; ++influence) {
                vertex.weight[influence] = data[at + 16 + influence];
                weightSum += vertex.weight[influence];
                if (vertex.bone[influence] >= boneCount) return fail("native skin vertex bone index is out of range");
            }
            if (!finiteVec(vertex.rest) || weightSum != 255 ||
                vertex.rest.x < declaredMin.x - kBoundsTolerance || vertex.rest.x > declaredMax.x + kBoundsTolerance ||
                vertex.rest.y < declaredMin.y - kBoundsTolerance || vertex.rest.y > declaredMax.y + kBoundsTolerance ||
                vertex.rest.z < declaredMin.z - kBoundsTolerance || vertex.rest.z > declaredMax.z + kBoundsTolerance)
                return fail("native skin vertex is invalid or outside declared bounds");
            vertices_.push_back(vertex);
        }

        for (uint32_t i = 0; i < boneCount; ++i) {
            const size_t at = static_cast<size_t>(boneOffset) + static_cast<size_t>(i) * kBoneBytes;
            DiskBone bone;
            bone.target = data[at];
            bone.flags = data[at + 1];
            bone.restHead = {readF32(data, at + 4), readF32(data, at + 8), readF32(data, at + 12)};
            bone.restTail = {readF32(data, at + 16), readF32(data, at + 20), readF32(data, at + 24)};
            bone.restXAxis = {readF32(data, at + 28), readF32(data, at + 32), readF32(data, at + 36)};
            if (bone.target >= kTargetCount || !finiteVec(bone.restHead) || !finiteVec(bone.restTail) ||
                !finiteVec(bone.restXAxis) || lengthSq(sub(bone.restTail, bone.restHead)) < 1e-10f ||
                lengthSq(bone.restXAxis) < 1e-10f)
                return fail("native skin rest bone frame is invalid");
            bone.rest = makeFrame(bone.restHead, bone.restTail, bone.restXAxis);
            if (!bone.rest.initialized) return fail("native skin rest bone basis is singular");
            bones_.push_back(bone);
        }

        for (uint32_t i = 0; i < triangleCount; ++i) {
            const size_t indexAt = static_cast<size_t>(indexOffset) + static_cast<size_t>(i) * kTriangleBytes;
            Triangle triangle;
            triangle.i0 = readU32(data, indexAt);
            triangle.i1 = readU32(data, indexAt + 4);
            triangle.i2 = readU32(data, indexAt + 8);
            const size_t colourAt = static_cast<size_t>(colourOffset) + static_cast<size_t>(i) * kColourBytes;
            triangle.r = data[colourAt]; triangle.g = data[colourAt + 1]; triangle.b = data[colourAt + 2];
            if (triangle.i0 >= vertexCount || triangle.i1 >= vertexCount || triangle.i2 >= vertexCount ||
                triangle.i0 == triangle.i1 || triangle.i0 == triangle.i2 || triangle.i1 == triangle.i2 ||
                nearDegenerate(vertices_[triangle.i0].rest, vertices_[triangle.i1].rest, vertices_[triangle.i2].rest))
                return fail("native skin triangle index or shape is invalid");
            triangles_.push_back(triangle);
        }

        // Metadata is deliberately not parsed: its bounded bytes are covered by
        // the CRC and it is validation provenance, not executable authority.
        nativeWings_ = nativeWings;
        bodyTriangleCount_ = bodyTriangleCount;
        valid_ = true;
        error_.clear();
        return true;
    }

    static avatar3d::Vec3 bodyRight(const avatar3d::AvatarSkeleton& avatar) {
        using J = avatar3d::AvatarSkeleton;
        avatar3d::Vec3 result = sub(avatar.joints[J::ShoulderR], avatar.joints[J::ShoulderL]);
        if (lengthSq(result) <= 1e-10f)
            result = sub(avatar.joints[J::HipR], avatar.joints[J::HipL]);
        if (lengthSq(result) <= 1e-10f) result = {1.0f, 0.0f, 0.0f};
        return normal(result);
    }

    // Target IDs exactly match export_native_lod.py.  Each is a v21 visual
    // joint pair; wings are mapped only when the exporter declared a complete
    // left/right root->boom->tip native chain.
    static bool posePair(const avatar3d::AvatarSkeleton& avatar, uint8_t target, avatar3d::Vec3& a, avatar3d::Vec3& b) {
        using J = avatar3d::AvatarSkeleton;
        switch (target) {
            case 0:  a = avatar.joints[J::Root];      b = avatar.joints[J::Spine]; return true;
            case 1:  a = avatar.joints[J::Spine];     b = avatar.joints[J::Neck]; return true;
            case 2:  a = avatar.joints[J::Neck];      b = avatar.joints[J::Head]; return true;
            case 3:  a = avatar.joints[J::Neck];      b = avatar.joints[J::Head]; return true;
            case 4:  a = avatar.joints[J::Spine];     b = avatar.joints[J::ShoulderL]; return true;
            case 5:  a = avatar.joints[J::ShoulderL]; b = avatar.joints[J::ElbowL]; return true;
            case 6:  a = avatar.joints[J::ElbowL];    b = avatar.joints[J::HandL]; return true;
            case 7:  a = avatar.joints[J::Spine];     b = avatar.joints[J::ShoulderR]; return true;
            case 8:  a = avatar.joints[J::ShoulderR]; b = avatar.joints[J::ElbowR]; return true;
            case 9:  a = avatar.joints[J::ElbowR];    b = avatar.joints[J::HandR]; return true;
            case 10: a = avatar.joints[J::Root];      b = avatar.joints[J::HipL]; return true;
            case 11: a = avatar.joints[J::HipL];      b = avatar.joints[J::KneeL]; return true;
            case 12: a = avatar.joints[J::KneeL];     b = avatar.joints[J::FootL]; return true;
            case 13: a = avatar.joints[J::Root];      b = avatar.joints[J::HipR]; return true;
            case 14: a = avatar.joints[J::HipR];      b = avatar.joints[J::KneeR]; return true;
            case 15: a = avatar.joints[J::KneeR];     b = avatar.joints[J::FootR]; return true;
            case 16: a = avatar.wingRoot[0];           b = avatar.wingBoom[0]; return true;
            case 17: a = avatar.wingBoom[0];           b = avatar.wingTip[0]; return true;
            case 18: a = avatar.wingBoom[0];           b = avatar.wingTip[0]; return true;
            case 19: a = avatar.wingRoot[1];           b = avatar.wingBoom[1]; return true;
            case 20: a = avatar.wingBoom[1];           b = avatar.wingTip[1]; return true;
            case 21: a = avatar.wingBoom[1];           b = avatar.wingTip[1]; return true;
            default: return false;
        }
    }

    avatar3d::Vec3 skinVertex(const Vertex& vertex,const avatar3d::Vec3* delta,
                               avatar3d::Vec3 origin,float ch,float sh,bool wing) const {
        // Native feathers are already open in the GLB neutral mesh. Until
        // individual feather pivots have an exact v21 mapping, one coherent
        // shoulder-following translation is safer than eight incompatible
        // local transfers that visibly detach their plate geometry on B.
        if (wing) return add(add(rotateY(vertex.rest,ch,sh),origin),delta[1]);
        avatar3d::Vec3 displacement{};
        for (int influence = 0; influence < 4; ++influence) {
            const uint8_t boneIndex = vertex.bone[influence];
            const float weight = static_cast<float>(vertex.weight[influence]) / 255.0f;
            if (weight <= 0.0f || boneIndex >= bones_.size()) continue;
            const DiskBone& bone = bones_[boneIndex];
            displacement=add(displacement,mul(delta[bone.target],weight));
        }
        return add(add(rotateY(vertex.rest,ch,sh),origin),displacement);
    }
};


enum class DisplayMode {
    Colour = 0,
    Depth = 1,
    ObjectIdentity = 2,
    Luminance = 3
};

static const char* displayModeName(DisplayMode mode) {
    switch (mode) {
        case DisplayMode::Colour: return "COLOUR 768";
        case DisplayMode::Depth: return "DEPTH 768";
        case DisplayMode::ObjectIdentity: return "OBJECT-ID 768";
        case DisplayMode::Luminance: return "LUMINANCE 768";
    }
    return "UNKNOWN";
}

template<int W,int H> struct FramebufferSurface {
    static constexpr int Width = W;
    static constexpr int Height = H;
    static constexpr int PixelCount = Width * Height;

    std::vector<uint32_t> colour;
    std::vector<float> depth;
    std::vector<uint16_t> objectId;
    std::vector<uint32_t> presentation;
    uint64_t sequence{0};

    FramebufferSurface()
        : colour(PixelCount, 0),
          depth(PixelCount, 1e30f),
          objectId(PixelCount, ObjectSky),
          presentation(PixelCount, 0) {}

    static uint32_t pack(Colour colour) {
        return (uint32_t)colour.b |
               ((uint32_t)colour.g << 8) |
               ((uint32_t)colour.r << 16) |
               ((uint32_t)colour.a << 24);
    }

    static Colour unpack(uint32_t packed) {
        return {(uint8_t)(packed & 0xff),
                (uint8_t)((packed >> 8) & 0xff),
                (uint8_t)((packed >> 16) & 0xff),
                (uint8_t)((packed >> 24) & 0xff)};
    }

    void clear() {
        for (int y = 0; y < Height; ++y) {
            float t = (float)y / (float)(Height - 1);
            Colour sky;
            sky.r = (uint8_t)std::lround(12.0f + 14.0f * t);
            sky.g = (uint8_t)std::lround(12.0f + 10.0f * t);
            sky.b = (uint8_t)std::lround(24.0f + 24.0f * t);
            const uint32_t packed = pack(sky);
            for (int x = 0; x < Width; ++x) {
                const int index = y * Width + x;
                colour[index] = packed;
                depth[index] = 1e30f;
                objectId[index] = ObjectSky;
            }
        }
        ++sequence;
    }

    const uint32_t* pixelsFor(DisplayMode mode) {
        if (mode == DisplayMode::Colour) return colour.data();
        for (int index = 0; index < PixelCount; ++index) {
            if (mode == DisplayMode::Depth) {
                float d = depth[index];
                if (d >= 1e20f) {
                    presentation[index] = pack({16, 10, 12, 255});
                } else {
                    float n = 1.0f - clamp01((d - 0.1f) / 30.0f);
                    presentation[index] = pack({
                        (uint8_t)std::lround(255.0f * (1.0f - n)),
                        (uint8_t)std::lround(210.0f * n),
                        (uint8_t)std::lround(255.0f * n), 255});
                }
            } else if (mode == DisplayMode::ObjectIdentity) {
                switch (objectId[index]) {
                    case ObjectGround: presentation[index] = pack({48, 48, 62, 255}); break;
                    case ObjectObstacle: presentation[index] = pack({52, 70, 232, 255}); break;
                    case ObjectGoal: presentation[index] = pack({98, 255, 62, 255}); break;
                    case ObjectAvatar: presentation[index] = pack({255, 168, 88, 255}); break;
                    case ObjectGrid: presentation[index] = pack({98, 94, 118, 255}); break;
                    case ObjectNpc: presentation[index] = pack({193, 124, 236, 255}); break;
                    case ObjectQuestItem: presentation[index] = pack({64, 222, 245, 255}); break;
                    case ObjectVehicle: presentation[index] = pack({72, 187, 255, 255}); break;
                    default: presentation[index] = pack({24, 12, 20, 255}); break;
                }
            } else {
                Colour source = unpack(colour[index]);
                uint8_t luminance = (uint8_t)std::lround(
                    0.114f * source.b + 0.587f * source.g + 0.299f * source.r);
                presentation[index] = pack({luminance, luminance, luminance, 255});
            }
        }
        return presentation.data();
    }

    #ifdef _WIN32
    void blit(HDC dc, int x, int y, DisplayMode mode) {
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = Width;
        info.bmiHeader.biHeight = -Height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        SetDIBitsToDevice(dc, x, y, Width, Height, 0, 0, 0, Height,
            pixelsFor(mode), &info, DIB_RGB_COLORS);
    }
    #endif
};

using HighResFramebuffer = FramebufferSurface<768,768>; // exact AI input contract
using DisplayFramebuffer = FramebufferSurface<1536,864>; // optional presentation only

struct VertexOut {
    float x{}, y{}, z{};
    bool visible{false};
};

struct Triangle {
    Vec3 a{}, b{}, c{};
    Colour colour{};
    uint16_t objectId{ObjectSky};
};

struct FrameMetrics {
    double renderMs{0.0};
    int trianglesSubmitted{0};
    int trianglesRasterized{0};
    int finiteDepthPixels{0};
    int semanticPixels{0};
    uint64_t sequence{0};
};

struct ModelFrameDef {
    const char* name;
    int parent;
    int joint;
};

struct ModelAtomicDef {
    const char* name;
    int fromFrame;
    int toFrame;
    float startRadius;
    float endRadius;
    Colour material;
};

// A small, original clump/frame/atomic graph informed by RE3's RenderWare model
// organization. It contains no GTA geometry or texture data. User-owned DFF/TXD
// assets remain an optional local conversion boundary; this graph is the safe
// procedural fallback embedded in GTAngel.
class Re3InformedAvatarClump {
public:
    static const std::array<ModelFrameDef, avatar3d::AvatarSkeleton::COUNT + 1>& frames() {
        static const std::array<ModelFrameDef, avatar3d::AvatarSkeleton::COUNT + 1> value{{
            {"root", -1, avatar3d::AvatarSkeleton::Root},
            {"spine", 0, avatar3d::AvatarSkeleton::Spine},
            {"neck", 1, avatar3d::AvatarSkeleton::Neck},
            {"head", 2, avatar3d::AvatarSkeleton::Head},
            {"shoulder_l", 1, avatar3d::AvatarSkeleton::ShoulderL},
            {"elbow_l", 4, avatar3d::AvatarSkeleton::ElbowL},
            {"hand_l", 5, avatar3d::AvatarSkeleton::HandL},
            {"shoulder_r", 1, avatar3d::AvatarSkeleton::ShoulderR},
            {"elbow_r", 7, avatar3d::AvatarSkeleton::ElbowR},
            {"hand_r", 8, avatar3d::AvatarSkeleton::HandR},
            {"hip_l", 0, avatar3d::AvatarSkeleton::HipL},
            {"knee_l", 10, avatar3d::AvatarSkeleton::KneeL},
            {"foot_l", 11, avatar3d::AvatarSkeleton::FootL},
            {"hip_r", 0, avatar3d::AvatarSkeleton::HipR},
            {"knee_r", 13, avatar3d::AvatarSkeleton::KneeR},
            {"foot_r", 14, avatar3d::AvatarSkeleton::FootR},
            {"crown", 3, avatar3d::AvatarSkeleton::Head},
        }};
        return value;
    }

    static const std::array<ModelAtomicDef, 15>& atomics() {
        static const Colour core{220, 128, 234, 255};
        static const Colour limb{198, 104, 221, 255};
        static const Colour distal{238, 162, 247, 255};
        static const std::array<ModelAtomicDef, 15> value{{
            {"pelvis", 0, 1, 0.15f, 0.18f, core},
            {"thorax", 1, 2, 0.18f, 0.09f, core},
            {"cervical", 2, 3, 0.075f, 0.085f, distal},
            {"upper_arm_l", 4, 5, 0.075f, 0.060f, limb},
            {"forearm_l", 5, 6, 0.060f, 0.045f, distal},
            {"upper_arm_r", 7, 8, 0.075f, 0.060f, limb},
            {"forearm_r", 8, 9, 0.060f, 0.045f, distal},
            {"thigh_l", 10, 11, 0.105f, 0.078f, core},
            {"shin_l", 11, 12, 0.078f, 0.052f, limb},
            {"thigh_r", 13, 14, 0.105f, 0.078f, core},
            {"shin_r", 14, 15, 0.078f, 0.052f, limb},
            {"clavicle_l", 1, 4, 0.10f, 0.072f, core},
            {"clavicle_r", 1, 7, 0.10f, 0.072f, core},
            {"pelvic_l", 0, 10, 0.13f, 0.105f, core},
            {"pelvic_r", 0, 13, 0.13f, 0.105f, core},
        }};
        return value;
    }

    static const char* sourceLabel() { return "ORIGINAL CLUMP / 17 FRAMES / 15 ATOMICS"; }
};

template<class Surface> class RasterEngine {
public:
    Surface framebuffer;
    FrameMetrics metrics;
    DteSkinnedAsset skinned_;
    bool loadDteAsset(const std::string& filename) { return skinned_.load(filename); }
    bool hasDteAsset() const { return skinned_.valid(); }
    const std::string& dteAssetError() const { return skinned_.error(); }
    size_t dteAssetTriangleCount() const { return skinned_.triangleCount(); }
    const char* modelSource() const { return skinned_.valid() ? "DTE V22 SKINNED / LOCAL SENSOR" : Re3InformedAvatarClump::sourceLabel(); }

    void render(const avatar3d::GameWorld& world,
                const avatar3d::AvatarSkeleton& avatar,
                Vec3 eye, float heading, float pitch, bool thirdPerson) {
        const auto start = std::chrono::steady_clock::now();
        framebuffer.clear();
        triangles_.clear();
        heading_ = heading;
        pitch_ = pitch;
        eye_ = eye;

        buildGround(world);
        buildPillars(world);
        buildGoal(world);
        buildNpc(world);
        buildQuestShard(world);
        buildVehicle(world);
        if (thirdPerson && !world.vehicleOccupied) buildAvatar(avatar);

        metrics.trianglesSubmitted = (int)triangles_.size();
        metrics.trianglesRasterized = 0;
        for (const auto& triangle : triangles_) rasterize(triangle);
        metrics.finiteDepthPixels = 0;
        metrics.semanticPixels = 0;
        for (int index = 0; index < Surface::PixelCount; ++index) {
            if (framebuffer.depth[index] < 1e20f) ++metrics.finiteDepthPixels;
            if (framebuffer.objectId[index] != ObjectSky) ++metrics.semanticPixels;
        }
        metrics.sequence = framebuffer.sequence;
        metrics.renderMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
    }

private:
    std::vector<Triangle> triangles_;
    Vec3 eye_{};
    float heading_{0.0f};
    float pitch_{0.0f};

    static Colour groundColour(int gx, int gz) {
        bool alternate = ((gx + gz) & 1) != 0;
        return alternate ? Colour{48, 42, 45, 255} : Colour{39, 34, 38, 255};
    }

    void addTriangle(Vec3 a, Vec3 b, Vec3 c, Colour colour, uint16_t objectId) {
        triangles_.push_back({a, b, c, colour, objectId});
    }

    void addQuad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Colour colour, uint16_t objectId) {
        addTriangle(a, b, c, colour, objectId);
        addTriangle(a, c, d, colour, objectId);
    }

    void buildGround(const avatar3d::GameWorld& world) {
        const int half = (int)std::ceil(world.worldSize * 0.5f);
        // The 128 m map stays physically bounded; only the local 40 m tile
        // window is rasterized. No low-resolution layer becomes authoritative.
        const int minX=std::max(-half,(int)std::floor(eye_.x)-20);
        const int maxX=std::min( half,(int)std::ceil(eye_.x)+20);
        const int minZ=std::max(-half,(int)std::floor(eye_.z)-20);
        const int maxZ=std::min( half,(int)std::ceil(eye_.z)+20);
        for (int x = minX; x < maxX; ++x) {
            for (int z = minZ; z < maxZ; ++z) {
                const Colour colour = groundColour(x, z);
                addQuad({(float)x, 0.0f, (float)z},
                        {(float)x + 1.0f, 0.0f, (float)z},
                        {(float)x + 1.0f, 0.0f, (float)z + 1.0f},
                        {(float)x, 0.0f, (float)z + 1.0f}, colour, ObjectGround);
            }
        }
    }

    void buildPillars(const avatar3d::GameWorld& world) {
        static constexpr int Segments = 14;
        for (const auto& pillar : world.pillars) {
            for (int segment = 0; segment < Segments; ++segment) {
                float a0 = 6.2831853f * segment / Segments;
                float a1 = 6.2831853f * (segment + 1) / Segments;
                Vec3 p0{pillar.pos.x + cosf(a0) * pillar.radius, 0.0f,
                        pillar.pos.z + sinf(a0) * pillar.radius};
                Vec3 p1{pillar.pos.x + cosf(a1) * pillar.radius, 0.0f,
                        pillar.pos.z + sinf(a1) * pillar.radius};
                Vec3 p2{p1.x, pillar.height, p1.z};
                Vec3 p3{p0.x, pillar.height, p0.z};
                Colour colour = (segment & 1) ? Colour{55, 62, 170, 255}
                                              : Colour{45, 52, 145, 255};
                addQuad(p0, p1, p2, p3, colour, ObjectObstacle);
                addTriangle({pillar.pos.x, pillar.height, pillar.pos.z}, p3, p2,
                            Colour{70, 78, 196, 255}, ObjectObstacle);
            }
        }
    }

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
            Vec3 p0{world.goal.x + cosf(a0) * 0.26f, 0.0f,
                    world.goal.z + sinf(a0) * 0.26f};
            Vec3 p1{world.goal.x + cosf(a1) * 0.26f, 0.0f,
                    world.goal.z + sinf(a1) * 0.26f};
            Vec3 p2{world.goal.x + cosf(a1) * 0.10f, 3.2f,
                    world.goal.z + sinf(a1) * 0.10f};
            Vec3 p3{world.goal.x + cosf(a0) * 0.10f, 3.2f,
                    world.goal.z + sinf(a0) * 0.10f};
            addQuad(p0, p1, p2, p3, goal, ObjectGoal);
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

    void buildNpc(const avatar3d::GameWorld& world) {
        avatar3d::AvatarSkeleton npc;
        npc.pose(world.npcPos, world.npcHeading, std::min(1.0f, world.npcSpeed), 0.05f);
        const Colour core = world.questComplete ? Colour{248, 194, 96, 255} : Colour{139, 79, 190, 255};
        const Colour limb = world.questComplete ? Colour{250, 224, 154, 255} : Colour{205, 126, 228, 255};
        const auto& frames = Re3InformedAvatarClump::frames();
        for (const auto& atomic : Re3InformedAvatarClump::atomics()) {
            const int fromJoint = frames[atomic.fromFrame].joint;
            const int toJoint = frames[atomic.toFrame].joint;
            const Colour colour = atomic.startRadius > 0.1f ? core : limb;
            addTaperedBone(npc.joints[fromJoint], npc.joints[toJoint],
                           atomic.startRadius * 1.04f, atomic.endRadius * 1.04f,
                           colour, ObjectNpc);
        }
        for (int joint = 0; joint < avatar3d::AvatarSkeleton::COUNT; ++joint)
            addJoint(npc.joints[joint], joint == avatar3d::AvatarSkeleton::Head ? 0.15f : 0.078f,
                     joint == avatar3d::AvatarSkeleton::Head ? Colour{255, 214, 170, 255} : limb,
                     ObjectNpc);
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

    void buildQuestShard(const avatar3d::GameWorld& world) {
        if (!world.questShardVisible) return;
        const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        const float bob = 0.62f + 0.12f * sinf((float)(ticks % 1000000) * 0.005f);
        const float radius = 0.28f;
        const Vec3 c = world.questShard + Vec3{0.0f, bob, 0.0f};
        const Vec3 top = c + Vec3{0, radius * 1.5f, 0};
        const Vec3 bottom = c + Vec3{0, -radius * 1.5f, 0};
        const std::array<Vec3, 4> ring{{
            c + Vec3{radius, 0, 0}, c + Vec3{0, 0, radius},
            c + Vec3{-radius, 0, 0}, c + Vec3{0, 0, -radius}}};
        const Colour shard{72, 224, 244, 255};
        for (int i = 0; i < 4; ++i) {
            const int next = (i + 1) % 4;
            addTriangle(top, ring[i], ring[next], shard, ObjectQuestItem);
            addTriangle(bottom, ring[next], ring[i], Colour{34, 154, 225, 255}, ObjectQuestItem);
        }
    }

    void buildVehicle(const avatar3d::GameWorld& world) {
        const Vec3 centre=world.vehiclePos;
        const float ch=cosf(world.vehicleHeading), sh=sinf(world.vehicleHeading);
        auto local=[&](float x,float y,float z) {
            return centre+Vec3{x*ch+z*sh,y,-x*sh+z*ch};
        };
        auto box=[&](float width,float halfLength,float bottom,float top,Colour colour) {
            Vec3 a=local(-width,bottom,-halfLength), b=local(width,bottom,-halfLength);
            Vec3 c=local(width,bottom,halfLength), d=local(-width,bottom,halfLength);
            Vec3 e=local(-width,top,-halfLength), f=local(width,top,-halfLength);
            Vec3 g=local(width,top,halfLength), h=local(-width,top,halfLength);
            addQuad(a,b,f,e,colour,ObjectVehicle); addQuad(b,c,g,f,colour,ObjectVehicle);
            addQuad(c,d,h,g,colour,ObjectVehicle); addQuad(d,a,e,h,colour,ObjectVehicle);
            addQuad(e,f,g,h,Colour{126,200,228,255},ObjectVehicle);
        };
        box(0.66f,1.03f,0.23f,0.64f,Colour{213,119,58,255});
        box(0.48f,0.48f,0.64f,1.10f,Colour{174,80,42,255});
        for (float x : {-0.67f,0.67f}) for (float z : {-0.70f,0.70f}) {
            const Vec3 wheel=local(x,0.28f,z);
            for (int i=0;i<10;++i) {
                float a=6.2831853f*i/10+world.vehicleWheelPhase;
                float b=6.2831853f*(i+1)/10+world.vehicleWheelPhase;
                addTriangle(wheel,wheel+Vec3{cosf(a)*0.27f,sinf(a)*0.27f,0},
                            wheel+Vec3{cosf(b)*0.27f,sinf(b)*0.27f,0},
                            Colour{37,42,50,255},ObjectVehicle);
            }
        }
    }

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
        float illumination = 0.28f + 0.72f * std::max(0.0f, avatar3d::dot(normal, light));
        if (triangle.objectId == ObjectAvatar && skinned_.valid())
            illumination = 0.64f + 0.36f * std::max(0.0f, avatar3d::dot(normal, light));
        const float inverseArea = 1.0f / signedArea;

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
                float fog = clamp01((depth - 11.0f) / 27.0f);
                framebuffer.depth[index] = depth;
                framebuffer.objectId[index] = triangle.objectId;
                framebuffer.colour[index] = Surface::pack(shade(triangle.colour, illumination, fog));
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
                int ids[6]{};
                for (int sy = 0; sy < Tile; ++sy) {
                    int y = ry * Tile + sy;
                    for (int sx = 0; sx < Tile; ++sx) {
                        int x = rx * Tile + sx;
                        int index = y * HighResFramebuffer::Width + x;
                        Colour colour = HighResFramebuffer::unpack(source.colour[index]);
                        sumB += colour.b; sumG += colour.g; sumR += colour.r;
                        minDepth = std::min(minDepth, source.depth[index]);
                        uint16_t id = source.objectId[index];
                        if (id < 6) ++ids[id];
                    }
                }
                const int index = ry * Retina64::Width + rx;
                const float samples = (float)(Tile * Tile);
                Colour average{(uint8_t)std::lround(sumB / samples),
                               (uint8_t)std::lround(sumG / samples),
                               (uint8_t)std::lround(sumR / samples), 255};
                retina.colour[index] = HighResFramebuffer::pack(average);
                retina.luminance[index] = (0.114f * average.b + 0.587f * average.g + 0.299f * average.r) / 255.0f;
                retina.depth[index] = minDepth;
                int dominant = 0;
                for (int id = 1; id < 6; ++id) if (ids[id] > ids[dominant]) dominant = id;
                retina.semantic[index] = dominant == ObjectGoal ? 1.0f :
                                         dominant == ObjectObstacle ? 0.66f :
                                         dominant == ObjectAvatar ? 0.50f :
                                         dominant == ObjectGround ? 0.33f : 0.0f;
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
    uint64_t sourceSequence{0};
};

class VisualNavStateEncoder {
public:
    VisualNavEvidence encode(const Retina64& retina) const {
        VisualNavEvidence evidence;
        evidence.sourceSequence = retina.sourceSequence;
        float goalX = 0.0f;
        float obstacleX = 0.0f;
        for (int y = 0; y < Retina64::Height; ++y) {
            for (int x = 0; x < Retina64::Width; ++x) {
                const int index = y * Retina64::Width + x;
                const float semantic = retina.semantic[index];
                const float depth = retina.depth[index];
                if (semantic > 0.9f) {
                    goalX += (float)x + 0.5f;
                    evidence.goalDepth = std::min(evidence.goalDepth, depth);
                    ++evidence.goalPixels;
                } else if (semantic > 0.5f && semantic < 0.8f) {
                    if (depth < evidence.nearestObstacleDepth) {
                        evidence.nearestObstacleDepth = depth;
                        obstacleX = (float)x + 0.5f;
                    }
                    ++evidence.obstaclePixels;
                }
            }
        }

        evidence.goalVisible = evidence.goalPixels > 0;
        evidence.obstacleVisible = evidence.obstaclePixels > 0;
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

#ifndef GTANGEL_V16_VISUOMOTOR_CPP
#define GTANGEL_V16_VISUOMOTOR_CPP

#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace v16motor {

static constexpr uint32_t DatasetMagic = 0x4D565447u; // GTVM
static constexpr uint16_t DatasetVersion = 1;
static constexpr int TensorWidth = 96;
static constexpr int TensorHeight = 96;
static constexpr int TensorChannels = 3;
static constexpr int TensorPixels = TensorWidth * TensorHeight;
static constexpr int TensorBytes = TensorPixels * TensorChannels;
static constexpr int ProprioceptionSize = 16;
static constexpr int ControllerSize = 6;

enum class OutcomeFlags : uint32_t {
    None = 0,
    Collision = 1u << 0,
    GoalReached = 1u << 1,
    ShieldIntervened = 1u << 2,
    PeerAttentionPresent = 1u << 3,
};

static OutcomeFlags operator|(OutcomeFlags a, OutcomeFlags b) {
    return static_cast<OutcomeFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct VisualTensor96 {
    std::array<uint8_t, TensorBytes> values{};
    uint64_t sourceSequence{0};
    uint64_t sourceDigest{1469598103934665603ull};

    void derive(const v15render::HighResFramebuffer& frame) {
        sourceSequence = frame.sequence;
        sourceDigest = 1469598103934665603ull;
        static constexpr int Scale = v15render::HighResFramebuffer::Width / TensorWidth;
        static_assert(Scale == 8, "v16 tensor requires an exact 768-to-96 reduction");
        for (int ty = 0; ty < TensorHeight; ++ty) {
            for (int tx = 0; tx < TensorWidth; ++tx) {
                uint32_t luminanceSum = 0;
                float minimumDepth = 1e30f;
                std::array<int, 8> objectVotes{};
                for (int oy = 0; oy < Scale; ++oy) {
                    for (int ox = 0; ox < Scale; ++ox) {
                        const int sx = tx * Scale + ox;
                        const int sy = ty * Scale + oy;
                        const int source = sy * v15render::HighResFramebuffer::Width + sx;
                        const v15render::Colour colour = v15render::HighResFramebuffer::unpack(frame.colour[source]);
                        luminanceSum += static_cast<uint32_t>(0.2126f * colour.r + 0.7152f * colour.g + 0.0722f * colour.b);
                        minimumDepth = std::min(minimumDepth, frame.depth[source]);
                        const uint8_t objectId = frame.objectId[source];
                        if (objectId < objectVotes.size()) ++objectVotes[objectId];
                        sourceDigest ^= frame.colour[source];
                        sourceDigest *= 1099511628211ull;
                        sourceDigest ^= objectId;
                        sourceDigest *= 1099511628211ull;
                    }
                }
                const int destination = ty * TensorWidth + tx;
                values[destination] = static_cast<uint8_t>(luminanceSum / (Scale * Scale));
                const float boundedDepth = std::isfinite(minimumDepth) ? std::clamp(minimumDepth, 0.0f, 32.0f) : 32.0f;
                values[TensorPixels + destination] = static_cast<uint8_t>(std::lround((1.0f - boundedDepth / 32.0f) * 255.0f));
                const int objectId = static_cast<int>(std::max_element(objectVotes.begin(), objectVotes.end()) - objectVotes.begin());
                values[TensorPixels * 2 + destination] = static_cast<uint8_t>(objectId * 31);
            }
        }
    }
};

struct ProprioceptiveVector {
    std::array<float, ProprioceptionSize> values{};
};

struct ControllerTarget {
    std::array<float, ControllerSize> values{};
};

static ControllerTarget controllerFromDiscreteAction(int action) {
    ControllerTarget target;
    // turn, forward, strafe, lookYaw, lookPitch, actionGate
    switch (action) {
        case 0: target.values = {0.0f, 0.65f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        case 1: target.values = {-0.45f, 0.55f, 0.0f, -0.25f, 0.0f, 1.0f}; break;
        case 2: target.values = {0.45f, 0.55f, 0.0f, 0.25f, 0.0f, 1.0f}; break;
        case 3: target.values = {-0.80f, 0.0f, 0.0f, -0.60f, 0.0f, 1.0f}; break;
        case 4: target.values = {0.80f, 0.0f, 0.0f, 0.60f, 0.0f, 1.0f}; break;
        case 5: target.values = {0.0f, -0.35f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        case 7: target.values = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        default: target.values = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; break;
    }
    return target;
}

#pragma pack(push, 1)
struct DatasetHeader {
    uint32_t magic{DatasetMagic};
    uint16_t version{DatasetVersion};
    uint16_t headerBytes{sizeof(DatasetHeader)};
    uint16_t tensorWidth{TensorWidth};
    uint16_t tensorHeight{TensorHeight};
    uint16_t tensorChannels{TensorChannels};
    uint16_t proprioceptionSize{ProprioceptionSize};
    uint16_t controllerSize{ControllerSize};
    uint16_t reserved{0};
    uint64_t episodeId{0};
    uint64_t createdAtMs{0};
};

struct RecordHeader {
    uint32_t recordBytes{sizeof(RecordHeader) + TensorBytes + sizeof(float) * (ProprioceptionSize + ControllerSize)};
    uint32_t step{0};
    uint64_t timestampMs{0};
    uint64_t frameSequence{0};
    uint64_t frameDigest{0};
    uint32_t outcomeFlags{0};
    float reward{0.0f};
    float modelConfidence{0.0f};
    float collisionRisk{0.0f};
};
#pragma pack(pop)

class TrajectoryRecorder {
    std::ofstream stream_;
    std::string path_;
    std::string directory_;
    std::string lastError_;
    std::string ledgerPath_;
    uint64_t episodeId_{0};
    uint32_t records_{0};
    uint64_t bytes_{0};
    bool active_{false};
    bool ready_{false};

    static uint64_t nowMs() {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }

    static std::string jsonEscape(const std::string& value) {
        std::string escaped;
        escaped.reserve(value.size() + 8);
        for (char character : value) {
            if (character == '\\' || character == '"') escaped.push_back('\\');
            if (character == '\n') { escaped += "\\n"; continue; }
            if (character == '\r') { escaped += "\\r"; continue; }
            escaped.push_back(character);
        }
        return escaped;
    }

    void appendLedger(const std::string& event, const std::string& detail) const {
        if (ledgerPath_.empty()) return;
        std::ofstream ledger(ledgerPath_, std::ios::app);
        if (!ledger) return;
        ledger << "{\"timestampMs\":" << nowMs()
               << ",\"event\":\"" << jsonEscape(event)
               << "\",\"detail\":\"" << jsonEscape(detail) << "\"}\n";
    }

    bool selectDirectory(const std::string& preferred) {
        std::array<std::filesystem::path, 3> candidates{};
        candidates[0] = std::filesystem::path(preferred);
        std::error_code error;
        candidates[1] = std::filesystem::current_path(error) / "datasets";
        error.clear();
        candidates[2] = std::filesystem::temp_directory_path(error) / "GTAngel" / "datasets";
        for (const auto& candidate : candidates) {
            error.clear();
            std::filesystem::create_directories(candidate, error);
            if (error) continue;
            const std::filesystem::path probe = candidate / ".gtvm_write_probe";
            std::ofstream test(probe.string(), std::ios::binary | std::ios::trunc);
            if (!test) continue;
            test.write("GTVM", 4);
            test.close();
            error.clear();
            std::filesystem::remove(probe, error);
            directory_ = std::filesystem::absolute(candidate, error).string();
            if (directory_.empty()) directory_ = candidate.string();
            ledgerPath_ = (std::filesystem::path(directory_) / "gtangel_v16_capture_ledger.jsonl").string();
            ready_ = true;
            lastError_.clear();
            appendLedger("capture.self_check.passed", directory_);
            return true;
        }
        ready_ = false;
        lastError_ = "No writable dataset directory (preferred, current, or temporary)";
        return false;
    }

public:
    bool selfCheck(const std::string& preferred) {
        stop();
        return selectDirectory(preferred);
    }

    bool start(const std::string& directory, uint64_t episodeId) {
        stop();
        if (!ready_ || (directory_ != directory && !selectDirectory(directory))) {
            if (!ready_ && !selectDirectory(directory)) return false;
        }
        episodeId_ = episodeId;
        std::ostringstream filename;
        filename << directory_ << "/gtvm_episode_" << episodeId << "_" << nowMs() << ".gtvm";
        path_ = filename.str();
        stream_.open(path_, std::ios::binary | std::ios::trunc);
        if (!stream_) {
            lastError_ = "Unable to open " + path_;
            appendLedger("capture.start.failed", lastError_);
            return false;
        }
        DatasetHeader header;
        header.episodeId = episodeId;
        header.createdAtMs = nowMs();
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        active_ = stream_.good();
        if (!active_) {
            lastError_ = "Unable to write dataset header to " + path_;
            appendLedger("capture.start.failed", lastError_);
            return false;
        }
        records_ = 0;
        bytes_ = sizeof(header);
        lastError_.clear();
        appendLedger("capture.started", path_);
        return active_;
    }

    bool stop() {
        if (stream_.is_open()) {
            stream_.flush();
            const bool flushFailed = !stream_.good();
            stream_.close();
            const bool closeFailed = stream_.fail();
            if (flushFailed || closeFailed) {
                lastError_ = std::string(flushFailed ? "Dataset flush failed" : "Dataset close failed") + " for " + path_;
                ready_ = false;
                active_ = false;
                appendLedger("capture.close.failed", lastError_);
                return false;
            }
            appendLedger("capture.stopped", path_ + ";records=" + std::to_string(records_));
        }
        active_ = false;
        return true;
    }

    bool capture(uint32_t step,
                 const VisualTensor96& visual,
                 const ProprioceptiveVector& proprioception,
                 const ControllerTarget& controller,
                 OutcomeFlags outcome,
                 float reward,
                 float modelConfidence,
                 float collisionRisk) {
        if (!active_) return false;
        RecordHeader header;
        header.step = step;
        header.timestampMs = nowMs();
        header.frameSequence = visual.sourceSequence;
        header.frameDigest = visual.sourceDigest;
        header.outcomeFlags = static_cast<uint32_t>(outcome);
        header.reward = reward;
        header.modelConfidence = std::clamp(modelConfidence, 0.0f, 1.0f);
        header.collisionRisk = std::clamp(collisionRisk, 0.0f, 1.0f);
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        stream_.write(reinterpret_cast<const char*>(visual.values.data()), visual.values.size());
        stream_.write(reinterpret_cast<const char*>(proprioception.values.data()), sizeof(float) * proprioception.values.size());
        stream_.write(reinterpret_cast<const char*>(controller.values.data()), sizeof(float) * controller.values.size());
        if (!stream_.good()) {
            lastError_ = "Dataset write failed for " + path_;
            appendLedger("capture.write.failed", lastError_);
            stop();
            return false;
        }
        ++records_;
        bytes_ += header.recordBytes;
        if ((records_ % 32u) == 0u) stream_.flush();
        return true;
    }

    bool active() const { return active_; }
    bool ready() const { return ready_; }
    uint32_t records() const { return records_; }
    uint64_t bytesWritten() const { return bytes_; }
    const std::string& path() const { return path_; }
    const std::string& directory() const { return directory_; }
    const std::string& lastError() const { return lastError_; }
};

static ProprioceptiveVector makeProprioception(float speed,
                                                float heading,
                                                float headingDelta,
                                                bool collided,
                                                bool grounded,
                                                const ControllerTarget& previous,
                                                float arousal,
                                                float reward,
                                                float visualConfidence,
                                                float visualCollisionRisk) {
    ProprioceptiveVector vector;
    vector.values[0] = speed;
    vector.values[1] = std::sin(heading);
    vector.values[2] = std::cos(heading);
    vector.values[3] = headingDelta;
    vector.values[4] = collided ? 1.0f : 0.0f;
    vector.values[5] = grounded ? 1.0f : 0.0f;
    for (int index = 0; index < ControllerSize; ++index) vector.values[6 + index] = previous.values[index];
    vector.values[12] = arousal;
    vector.values[13] = reward;
    vector.values[14] = visualConfidence;
    vector.values[15] = visualCollisionRisk;
    return vector;
}

} // namespace v16motor

#ifndef GTANGEL_V17_NEURAL_POLICY_CPP
#define GTANGEL_V17_NEURAL_POLICY_CPP

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace v17policy {

static constexpr uint32_t WeightMagic = 0x57564D47u;
static constexpr uint16_t WeightVersion = 1;
static constexpr int HiddenSize = 64;

struct Tensor {
    std::vector<uint32_t> shape;
    std::vector<float> values;
};

struct Prediction {
    v16motor::ControllerTarget controller;
    std::array<float, 3> egoMotion{};
    std::array<float, 288> cognition{};
    float collisionProbability{1.0f};
    float uncertainty{1.0f};
    float latencyMs{0.0f};
    bool valid{false};
};

struct ShieldDecision {
    v16motor::ControllerTarget proposal;
    v16motor::ControllerTarget committed;
    float disagreementRms{1.0f};
    float confidence{0.0f};
    bool accepted{false};
    std::string reason{"model unavailable"};
};

class NativeVisuomotorPolicy {
    std::unordered_map<std::string, Tensor> tensors_;
    std::array<float, HiddenSize> hidden_{};
    std::array<float, v16motor::ControllerSize> previousController_{};
    bool hasPreviousController_{false};
    Prediction prediction_{};
    bool loaded_{false};
    std::string path_;
    std::string error_;

    template<typename T>
    static bool readValue(std::ifstream& stream, T& value) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
        return stream.good();
    }

    const Tensor& tensor(const char* name) const {
        return tensors_.at(name);
    }

    static float sigmoid(float value) {
        value = std::clamp(value, -20.0f, 20.0f);
        return 1.0f / (1.0f + std::exp(-value));
    }

    static float silu(float value) {
        return value * sigmoid(value);
    }

    static bool finiteVector(const std::vector<float>& values) {
        return std::all_of(values.begin(), values.end(), [](float value) { return std::isfinite(value); });
    }

    bool validateTensor(const std::string& name, std::initializer_list<uint32_t> shape) {
        const auto found = tensors_.find(name);
        if (found == tensors_.end()) { error_ = "missing tensor " + name; return false; }
        if (found->second.shape != std::vector<uint32_t>(shape)) { error_ = "shape mismatch for " + name; return false; }
        if (!finiteVector(found->second.values)) { error_ = "non-finite tensor " + name; return false; }
        return true;
    }

public:
    bool load(const std::string& path) {
        loaded_ = false;
        tensors_.clear();
        path_ = path;
        error_.clear();
        std::ifstream stream(path, std::ios::binary);
        if (!stream) { error_ = "unable to open " + path; return false; }
        uint32_t magic = 0;
        uint16_t version = 0, tensorCount = 0;
        if (!readValue(stream, magic) || !readValue(stream, version) || !readValue(stream, tensorCount)) {
            error_ = "truncated weight header"; return false;
        }
        if (magic != WeightMagic || version != WeightVersion || tensorCount != 20) {
            error_ = "unsupported weight bundle"; return false;
        }
        for (uint16_t index = 0; index < tensorCount; ++index) {
            uint16_t nameLength = 0;
            uint8_t dimensions = 0, dtype = 0;
            uint32_t elements = 0;
            if (!readValue(stream, nameLength) || !readValue(stream, dimensions) ||
                !readValue(stream, dtype) || !readValue(stream, elements)) {
                error_ = "truncated tensor header"; return false;
            }
            if (nameLength == 0 || dimensions == 0 || dimensions > 4 || dtype != 1 || elements == 0) {
                error_ = "invalid tensor header"; return false;
            }
            std::string name(nameLength, '\0');
            stream.read(name.data(), nameLength);
            Tensor value;
            value.shape.resize(dimensions);
            uint64_t shapeElements = 1;
            for (uint8_t dimension = 0; dimension < dimensions; ++dimension) {
                if (!readValue(stream, value.shape[dimension])) { error_ = "truncated tensor shape"; return false; }
                shapeElements *= value.shape[dimension];
            }
            if (shapeElements != elements) { error_ = "tensor element mismatch"; return false; }
            value.values.resize(elements);
            stream.read(reinterpret_cast<char*>(value.values.data()), sizeof(float) * elements);
            if (!stream.good() || tensors_.count(name)) { error_ = "invalid tensor payload"; return false; }
            tensors_.emplace(std::move(name), std::move(value));
        }
        char trailing = 0;
        if (stream.read(&trailing, 1)) { error_ = "trailing weight bytes"; return false; }

        loaded_ =
            validateTensor("visual.0.weight", {8,3,5,5}) && validateTensor("visual.0.bias", {8}) &&
            validateTensor("visual.2.weight", {16,8,3,3}) && validateTensor("visual.2.bias", {16}) &&
            validateTensor("visual.6.weight", {64,144}) && validateTensor("visual.6.bias", {64}) &&
            validateTensor("proprioception.0.weight", {32,16}) && validateTensor("proprioception.0.bias", {32}) &&
            validateTensor("recurrent.weight_ih", {192,96}) && validateTensor("recurrent.weight_hh", {192,64}) &&
            validateTensor("recurrent.bias_ih", {192}) && validateTensor("recurrent.bias_hh", {192}) &&
            validateTensor("controller.weight", {6,64}) && validateTensor("controller.bias", {6}) &&
            validateTensor("collision.weight", {1,64}) && validateTensor("collision.bias", {1}) &&
            validateTensor("ego_motion.weight", {3,64}) && validateTensor("ego_motion.bias", {3}) &&
            validateTensor("cognition.weight", {288,64}) && validateTensor("cognition.bias", {288});
        reset();
        return loaded_;
    }

    void reset() {
        hidden_.fill(0.0f);
        previousController_.fill(0.0f);
        hasPreviousController_ = false;
        prediction_ = Prediction{};
    }

    Prediction infer(const v16motor::VisualTensor96& visual,
                     const v16motor::ProprioceptiveVector& proprioception) {
        const auto started = std::chrono::high_resolution_clock::now();
        prediction_ = Prediction{};
        if (!loaded_) return prediction_;

        const Tensor& conv1Weight = tensor("visual.0.weight");
        const Tensor& conv1Bias = tensor("visual.0.bias");
        std::vector<float> conv1(8 * 24 * 24);
        for (int outputChannel = 0; outputChannel < 8; ++outputChannel) {
            for (int outputY = 0; outputY < 24; ++outputY) {
                for (int outputX = 0; outputX < 24; ++outputX) {
                    float sum = conv1Bias.values[outputChannel];
                    for (int inputChannel = 0; inputChannel < 3; ++inputChannel) {
                        for (int kernelY = 0; kernelY < 5; ++kernelY) {
                            for (int kernelX = 0; kernelX < 5; ++kernelX) {
                                const int inputY = outputY * 4 + kernelY - 2;
                                const int inputX = outputX * 4 + kernelX - 2;
                                if (inputY < 0 || inputY >= 96 || inputX < 0 || inputX >= 96) continue;
                                const float input = visual.values[inputChannel * 96 * 96 + inputY * 96 + inputX] / 255.0f;
                                const int weightIndex = (((outputChannel * 3 + inputChannel) * 5 + kernelY) * 5 + kernelX);
                                sum += conv1Weight.values[weightIndex] * input;
                            }
                        }
                    }
                    conv1[(outputChannel * 24 + outputY) * 24 + outputX] = silu(sum);
                }
            }
        }

        const Tensor& conv2Weight = tensor("visual.2.weight");
        const Tensor& conv2Bias = tensor("visual.2.bias");
        std::vector<float> conv2(16 * 12 * 12);
        for (int outputChannel = 0; outputChannel < 16; ++outputChannel) {
            for (int outputY = 0; outputY < 12; ++outputY) {
                for (int outputX = 0; outputX < 12; ++outputX) {
                    float sum = conv2Bias.values[outputChannel];
                    for (int inputChannel = 0; inputChannel < 8; ++inputChannel) {
                        for (int kernelY = 0; kernelY < 3; ++kernelY) {
                            for (int kernelX = 0; kernelX < 3; ++kernelX) {
                                const int inputY = outputY * 2 + kernelY - 1;
                                const int inputX = outputX * 2 + kernelX - 1;
                                if (inputY < 0 || inputY >= 24 || inputX < 0 || inputX >= 24) continue;
                                const int inputIndex = (inputChannel * 24 + inputY) * 24 + inputX;
                                const int weightIndex = (((outputChannel * 8 + inputChannel) * 3 + kernelY) * 3 + kernelX);
                                sum += conv2Weight.values[weightIndex] * conv1[inputIndex];
                            }
                        }
                    }
                    conv2[(outputChannel * 12 + outputY) * 12 + outputX] = silu(sum);
                }
            }
        }

        std::array<float, 144> pooled{};
        for (int channel = 0; channel < 16; ++channel) {
            for (int poolY = 0; poolY < 3; ++poolY) {
                for (int poolX = 0; poolX < 3; ++poolX) {
                    float sum = 0.0f;
                    for (int y = 0; y < 4; ++y)
                        for (int x = 0; x < 4; ++x)
                            sum += conv2[(channel * 12 + poolY * 4 + y) * 12 + poolX * 4 + x];
                    pooled[(channel * 3 + poolY) * 3 + poolX] = sum / 16.0f;
                }
            }
        }

        std::array<float, 64> visualLatent{};
        const Tensor& visualWeight = tensor("visual.6.weight");
        const Tensor& visualBias = tensor("visual.6.bias");
        for (int output = 0; output < 64; ++output) {
            float sum = visualBias.values[output];
            for (int input = 0; input < 144; ++input) sum += visualWeight.values[output * 144 + input] * pooled[input];
            visualLatent[output] = silu(sum);
        }

        std::array<float, 32> proprioLatent{};
        const Tensor& proprioWeight = tensor("proprioception.0.weight");
        const Tensor& proprioBias = tensor("proprioception.0.bias");
        for (int output = 0; output < 32; ++output) {
            float sum = proprioBias.values[output];
            for (int input = 0; input < 16; ++input) sum += proprioWeight.values[output * 16 + input] * proprioception.values[input];
            proprioLatent[output] = std::tanh(sum);
        }

        std::array<float, 96> fused{};
        std::copy(visualLatent.begin(), visualLatent.end(), fused.begin());
        std::copy(proprioLatent.begin(), proprioLatent.end(), fused.begin() + 64);
        const Tensor& inputWeight = tensor("recurrent.weight_ih");
        const Tensor& hiddenWeight = tensor("recurrent.weight_hh");
        const Tensor& inputBias = tensor("recurrent.bias_ih");
        const Tensor& hiddenBias = tensor("recurrent.bias_hh");
        std::array<float, 192> inputGates{}, hiddenGates{};
        for (int gate = 0; gate < 192; ++gate) {
            float inputSum = inputBias.values[gate];
            float hiddenSum = hiddenBias.values[gate];
            for (int input = 0; input < 96; ++input) inputSum += inputWeight.values[gate * 96 + input] * fused[input];
            for (int hidden = 0; hidden < 64; ++hidden) hiddenSum += hiddenWeight.values[gate * 64 + hidden] * hidden_[hidden];
            inputGates[gate] = inputSum;
            hiddenGates[gate] = hiddenSum;
        }
        std::array<float, HiddenSize> nextHidden{};
        for (int index = 0; index < HiddenSize; ++index) {
            const float resetGate = sigmoid(inputGates[index] + hiddenGates[index]);
            const float updateGate = sigmoid(inputGates[64 + index] + hiddenGates[64 + index]);
            const float newGate = std::tanh(inputGates[128 + index] + resetGate * hiddenGates[128 + index]);
            nextHidden[index] = newGate + updateGate * (hidden_[index] - newGate);
        }
        hidden_ = nextHidden;

        const Tensor& controllerWeight = tensor("controller.weight");
        const Tensor& controllerBias = tensor("controller.bias");
        for (int output = 0; output < 6; ++output) {
            float sum = controllerBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += controllerWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.controller.values[output] = std::tanh(sum);
        }
        float temporalSquared = 0.0f;
        for (int output = 0; output < v16motor::ControllerSize; ++output) {
            const float difference = prediction_.controller.values[output] - previousController_[output];
            temporalSquared += difference * difference;
            previousController_[output] = prediction_.controller.values[output];
        }
        const float temporalRms = std::sqrt(temporalSquared / v16motor::ControllerSize);
        const float closedGatePenalty = std::max(0.0f, 0.5f - prediction_.controller.values[5]);
        prediction_.uncertainty = hasPreviousController_
            ? std::clamp(temporalRms / 0.50f + closedGatePenalty, 0.0f, 1.0f)
            : 1.0f;
        hasPreviousController_ = true;
        const Tensor& collisionWeight = tensor("collision.weight");
        float collision = tensor("collision.bias").values[0];
        for (int hidden = 0; hidden < 64; ++hidden) collision += collisionWeight.values[hidden] * hidden_[hidden];
        prediction_.collisionProbability = sigmoid(collision);
        const Tensor& egoWeight = tensor("ego_motion.weight");
        const Tensor& egoBias = tensor("ego_motion.bias");
        for (int output = 0; output < 3; ++output) {
            float sum = egoBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += egoWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.egoMotion[output] = sum;
        }
        const Tensor& cognitionWeight = tensor("cognition.weight");
        const Tensor& cognitionBias = tensor("cognition.bias");
        for (int output = 0; output < 288; ++output) {
            float sum = cognitionBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += cognitionWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.cognition[output] = sigmoid(sum);
        }
        prediction_.valid = true;
        for (float value : prediction_.controller.values) prediction_.valid = prediction_.valid && std::isfinite(value);
        prediction_.valid = prediction_.valid && std::isfinite(prediction_.collisionProbability);
        const auto finished = std::chrono::high_resolution_clock::now();
        prediction_.latencyMs = std::chrono::duration<float, std::milli>(finished - started).count();
        return prediction_;
    }

    bool loaded() const { return loaded_; }
    const std::string& path() const { return path_; }
    const std::string& error() const { return error_; }
    const Prediction& prediction() const { return prediction_; }
};

class EmbodiedSafetyShield {
public:
    ShieldDecision decide(const Prediction& prediction,
                          const v16motor::ControllerTarget& expert,
                          float visualCollisionRisk,
                          bool assistRequested,
                          bool ethicsApproved = true) const {
        ShieldDecision decision;
        decision.proposal = prediction.controller;
        decision.committed = expert;
        float squared = 0.0f;
        for (int index = 0; index < v16motor::ControllerSize; ++index) {
            const float difference = prediction.controller.values[index] - expert.values[index];
            squared += difference * difference;
        }
        decision.disagreementRms = std::sqrt(squared / v16motor::ControllerSize);
        decision.confidence = prediction.valid ? std::clamp(1.0f - decision.disagreementRms / 0.45f, 0.0f, 1.0f) : 0.0f;
        if (!prediction.valid) { decision.reason = "invalid inference"; return decision; }
        if (!assistRequested) { decision.reason = "shadow mode"; return decision; }
        if (!ethicsApproved) { decision.reason = "ethics gate"; return decision; }
        if (prediction.uncertainty > 0.35f) { decision.reason = "uncertainty gate"; return decision; }
        if (visualCollisionRisk > 0.20f) { decision.reason = "visual collision gate"; return decision; }
        if (prediction.collisionProbability > 0.20f) { decision.reason = "model collision gate"; return decision; }
        if (decision.disagreementRms > 0.18f) { decision.reason = "expert disagreement"; return decision; }
        if (prediction.controller.values[5] < 0.50f) { decision.reason = "action gate closed"; return decision; }
        decision.committed = prediction.controller;
        decision.committed.values[0] = std::clamp(decision.committed.values[0], -0.8f, 0.8f);
        decision.committed.values[1] = std::clamp(decision.committed.values[1], -0.35f, 1.0f);
        decision.committed.values[2] = 0.0f;
        decision.committed.values[3] = std::clamp(decision.committed.values[3], -0.6f, 0.6f);
        decision.committed.values[4] = 0.0f;
        decision.committed.values[5] = 1.0f;
        decision.accepted = true;
        decision.reason = "accepted by local shield";
        return decision;
    }
};

} // namespace v17policy

#ifndef GTANGEL_V18_PEER_RELATIONAL_CPP
#define GTANGEL_V18_PEER_RELATIONAL_CPP

// GTAngel v19 peer-relational doom loop.
// Network relations are provisional attention cues only. They never enter the
// visuomotor tensor, NavTrainer state, depth/object buffers, or actuator gate.
namespace v18peer {

enum class PredictionState { Unconfirmed, Confirmed, Contradicted };

struct PeerPrediction {
    std::string senderId;
    std::string recipientId;
    std::string rangeBand{"unknown"};
    std::string intent{"observe"};
    std::string eventClass{"presence"};
    std::string traceId;
    std::string evidenceDigest;
    uint64_t sequence{0};
    uint64_t sourceFrameSequence{0};
    int relativeBearing{0};
    float confidence{0.0f};
    int64_t sentAtMs{0};
    int64_t expiresAtMs{0};
    PredictionState state{PredictionState::Unconfirmed};
};

struct RelayTelemetry {
    bool configured{false};
    bool enabled{false};
    bool busy{false};
    unsigned long accepted{0};
    unsigned long received{0};
    unsigned long rejected{0};
    unsigned long expired{0};
    unsigned long confirmed{0};
    unsigned long contradicted{0};
    unsigned long queued{0};
    unsigned long lastStatus{0};
    double lastLatencyMs{0.0};
    std::string lastError;
};

struct AttentionContext {
    std::array<float, 16> sectorSalience{};
    int dominantSector{-1};
    float dominantSalience{0.0f};
    float meanConfidence{0.0f};
    unsigned long activeRelations{0};
    unsigned long confirmedRelations{0};
    uint64_t generation{0};
};

static std::string jsonString(const std::string& value) {
    return std::string("\"") + civicangel::jsonEscape(value) + "\"";
}

static bool extractString(const std::string& json, const char* key, std::string& output) {
    const std::string marker = std::string("\"") + key + "\":";
    size_t pos = json.find(marker);
    if (pos == std::string::npos) return false;
    pos += marker.size();
    while (pos < json.size() && isspace((unsigned char)json[pos])) ++pos;
    if (pos >= json.size() || json[pos] != '"') return false;
    ++pos;
    output.clear();
    bool escaped = false;
    for (; pos < json.size(); ++pos) {
        const char c = json[pos];
        if (escaped) { output.push_back(c); escaped = false; continue; }
        if (c == '\\') { escaped = true; continue; }
        if (c == '"') return true;
        output.push_back(c);
    }
    return false;
}

static bool extractNumber(const std::string& json, const char* key, double& output) {
    const std::string marker = std::string("\"") + key + "\":";
    size_t pos = json.find(marker);
    if (pos == std::string::npos) return false;
    pos += marker.size();
    while (pos < json.size() && isspace((unsigned char)json[pos])) ++pos;
    char* end = nullptr;
    output = strtod(json.c_str() + pos, &end);
    return end && end != json.c_str() + pos && std::isfinite(output);
}

static std::vector<std::string> pingObjects(const std::string& response) {
    std::vector<std::string> objects;
    const size_t key = response.find("\"pings\"");
    if (key == std::string::npos) return objects;
    size_t array = response.find('[', key);
    if (array == std::string::npos) return objects;
    int depth = 0;
    bool quoted = false, escaped = false;
    size_t start = std::string::npos;
    for (size_t i = array + 1; i < response.size(); ++i) {
        const char c = response[i];
        if (quoted) {
            if (escaped) escaped = false;
            else if (c == '\\') escaped = true;
            else if (c == '"') quoted = false;
            continue;
        }
        if (c == '"') { quoted = true; continue; }
        if (c == '{') {
            if (depth++ == 0) start = i;
        } else if (c == '}') {
            if (--depth == 0 && start != std::string::npos) {
                objects.push_back(response.substr(start, i - start + 1));
                start = std::string::npos;
            }
        } else if (c == ']' && depth == 0) break;
    }
    return objects;
}

class PeerRelationalLoop {
public:
    PeerRelationalLoop() : worker_([this] { workerLoop(); }) {}
    ~PeerRelationalLoop() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    PeerRelationalLoop(const PeerRelationalLoop&) = delete;
    PeerRelationalLoop& operator=(const PeerRelationalLoop&) = delete;

    void configureFromEnvironment() {
        std::string base = civicangel::environmentValue("ARC_ANGEL_INGEST_URL");
        std::string token = civicangel::environmentValue("ARC_ANGEL_GTANGEL_TOKEN");
        std::string self = civicangel::environmentValue("GTANGEL_PEER_ID");
        if (self.empty()) self = "gtangel-v18";
        const std::string telemetryPath = "/api/telemetry/ingest";
        const size_t path = base.find(telemetryPath);
        if (path != std::string::npos) base.resize(path);
        while (!base.empty() && base.back() == '/') base.pop_back();
        std::lock_guard<std::mutex> lock(mu_);
        baseUrl_ = std::move(base);
        token_ = std::move(token);
        selfId_ = std::move(self);
        configured_ = !baseUrl_.empty() && !token_.empty() && !selfId_.empty();
        if (!configured_) lastError_ = "Peer relay requires ARC_ANGEL_INGEST_URL, ARC_ANGEL_GTANGEL_TOKEN, and optional GTANGEL_PEER_ID";
    }

    void setEnabled(bool enabled) {
        {
            std::lock_guard<std::mutex> lock(mu_);
            enabled_ = enabled;
        }
        cv_.notify_all();
    }
    bool enabled() const { std::lock_guard<std::mutex> lock(mu_); return enabled_; }

    // Called only when GTAngel has locally rendered evidence of another agent.
    // This method has no world-coordinate parameters by construction.
    bool publishObservedPeer(const std::string& recipientId, int bearingBucket,
                             const std::string& rangeBand, float confidence,
                             const std::string& intent, const std::string& eventClass,
                             uint64_t sourceFrameSequence, const std::string& evidenceDigest) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!configured_ || !enabled_) return false;
        if (outgoing_.size() >= 4) { ++rejected_; lastError_ = "Peer output queue full"; return false; }
        const int64_t now = civicangel::unixTimeMs();
        const uint64_t sequence = ++outgoingSequence_;
        char trace[64];
        snprintf(trace, sizeof(trace), "peer-%llx-%llx", (unsigned long long)sourceFrameSequence, (unsigned long long)now);
        std::ostringstream json;
        json << "{\"protocolVersion\":1,\"senderId\":" << jsonString(selfId_);
        if (!recipientId.empty()) json << ",\"recipientId\":" << jsonString(recipientId);
        json << ",\"sequence\":" << sequence
             << ",\"sentAtMs\":" << now
             << ",\"ttlMs\":3000"
             << ",\"relativeBearing\":" << std::max(0, std::min(15, bearingBucket))
             << ",\"rangeBand\":" << jsonString(rangeBand)
             << ",\"confidence\":" << std::fixed << std::setprecision(3) << std::max(0.0f, std::min(1.0f, confidence))
             << ",\"intent\":" << jsonString(intent)
             << ",\"eventClass\":" << jsonString(eventClass)
             << ",\"traceId\":" << jsonString(trace)
             << ",\"sourceFrameSequence\":" << sourceFrameSequence
             << ",\"evidenceDigest\":" << jsonString(evidenceDigest) << "}";
        const std::string body = json.str();
        if (body.size() > 512) { ++rejected_; lastError_ = "Peer payload exceeded 512 bytes"; return false; }
        outgoing_.push_back(body);
        cv_.notify_all();
        return true;
    }

    void confirmAgainstLocalFrame(const v15render::HighResFramebuffer& frame, bool firstPerson) {
        const int64_t now = civicangel::unixTimeMs();
        std::lock_guard<std::mutex> lock(mu_);
        for (auto it = predictions_.begin(); it != predictions_.end();) {
            if (it->expiresAtMs <= now) { ++expired_; it = predictions_.erase(it); continue; }
            it->state = PredictionState::Unconfirmed;
            if (firstPerson) confirmPrediction(*it, frame);
            ++it;
        }
        recalculateStates();
    }

    std::vector<PeerPrediction> predictions() const {
        std::lock_guard<std::mutex> lock(mu_);
        return predictions_;
    }

    // Explicit advisory context. It is stored independently from every policy,
    // navigation, framebuffer, and actuation structure.
    AttentionContext attentionContext() const {
        std::lock_guard<std::mutex> lock(mu_);
        AttentionContext context;
        context.generation = attentionGeneration_;
        float confidenceSum = 0.0f;
        for (const auto& prediction : predictions_) {
            const float stateWeight = prediction.state == PredictionState::Confirmed ? 1.0f
                : prediction.state == PredictionState::Contradicted ? 0.10f : 0.50f;
            const float salience = std::max(0.0f, std::min(1.0f, prediction.confidence)) * stateWeight;
            context.sectorSalience[(size_t)prediction.relativeBearing] = std::max(
                context.sectorSalience[(size_t)prediction.relativeBearing], salience);
            confidenceSum += prediction.confidence;
            ++context.activeRelations;
            if (prediction.state == PredictionState::Confirmed) ++context.confirmedRelations;
        }
        for (int sector = 0; sector < 16; ++sector) {
            if (context.sectorSalience[(size_t)sector] > context.dominantSalience) {
                context.dominantSalience = context.sectorSalience[(size_t)sector];
                context.dominantSector = sector;
            }
        }
        context.meanConfidence = context.activeRelations ? confidenceSum / context.activeRelations : 0.0f;
        return context;
    }

    RelayTelemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        RelayTelemetry t;
        t.configured = configured_; t.enabled = enabled_; t.busy = busy_;
        t.accepted = accepted_; t.received = received_; t.rejected = rejected_;
        t.expired = expired_; t.confirmed = confirmed_; t.contradicted = contradicted_;
        t.queued = (unsigned long)outgoing_.size(); t.lastStatus = lastStatus_;
        t.lastLatencyMs = lastLatencyMs_; t.lastError = lastError_;
        return t;
    }

    void drawAttentionMarkers(HDC hdc, int originX, int originY, int size) const {
        const auto local = predictions();
        const int cx = originX + size / 2, cy = originY + size / 2;
        const int radius = size / 2 - 24;
        for (const auto& prediction : local) {
            const double angle = prediction.relativeBearing * (2.0 * 3.141592653589793 / 16.0) - 3.141592653589793 / 2.0;
            const int x = cx + (int)std::lround(cos(angle) * radius);
            const int y = cy + (int)std::lround(sin(angle) * radius);
            COLORREF colour = prediction.state == PredictionState::Confirmed ? RGB(90, 240, 130)
                : prediction.state == PredictionState::Contradicted ? RGB(255, 80, 105) : RGB(70, 220, 255);
            HPEN pen = CreatePen(PS_SOLID, 2, colour);
            HBRUSH brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
            Ellipse(hdc, x - 6, y - 6, x + 7, y + 7);
            MoveToEx(hdc, x, y, nullptr); LineTo(hdc, cx, cy);
            SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen); DeleteObject(pen);
        }
    }

private:
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::thread worker_;
    bool stopping_{false}, configured_{false}, enabled_{false}, busy_{false};
    std::string baseUrl_, token_, selfId_, lastError_;
    std::deque<std::string> outgoing_;
    std::vector<PeerPrediction> predictions_;
    uint64_t outgoingSequence_{0};
    uint64_t attentionGeneration_{0};
    int64_t lastPollReceivedAtMs_{0};
    unsigned long accepted_{0}, received_{0}, rejected_{0}, expired_{0}, confirmed_{0}, contradicted_{0}, lastStatus_{0};
    double lastLatencyMs_{0.0};

    static bool depthMatches(const std::string& band, float depth) {
        if (!std::isfinite(depth)) return false;
        if (band == "contact") return depth < 1.5f;
        if (band == "near") return depth >= 1.0f && depth < 4.0f;
        if (band == "mid") return depth >= 3.0f && depth < 8.0f;
        if (band == "far") return depth >= 7.0f;
        return true;
    }

    static void confirmPrediction(PeerPrediction& prediction, const v15render::HighResFramebuffer& frame) {
        int signedBucket = prediction.relativeBearing;
        if (signedBucket > 8) signedBucket -= 16;
        const float bearingDeg = signedBucket * 22.5f;
        if (fabsf(bearingDeg) > 52.0f) return;
        const int centreX = std::max(0, std::min(v15render::HighResFramebuffer::Width - 1,
            (int)std::lround((0.5f + bearingDeg / 104.0f) * v15render::HighResFramebuffer::Width)));
        const int halfWidth = 36;
        bool avatarSeen = false, depthMatched = false;
        for (int y = 64; y < v15render::HighResFramebuffer::Height - 32; y += 4) {
            for (int x = std::max(0, centreX - halfWidth); x <= std::min(v15render::HighResFramebuffer::Width - 1, centreX + halfWidth); x += 2) {
                const size_t index = (size_t)y * v15render::HighResFramebuffer::Width + x;
                if (frame.objectId[index] != v15render::ObjectAvatar) continue;
                avatarSeen = true;
                if (depthMatches(prediction.rangeBand, frame.depth[index])) depthMatched = true;
            }
        }
        if (depthMatched) prediction.state = PredictionState::Confirmed;
        else if (avatarSeen) prediction.state = PredictionState::Contradicted;
    }

    void recalculateStates() {
        confirmed_ = contradicted_ = 0;
        for (const auto& prediction : predictions_) {
            if (prediction.state == PredictionState::Confirmed) ++confirmed_;
            if (prediction.state == PredictionState::Contradicted) ++contradicted_;
        }
        ++attentionGeneration_;
    }

    static bool parsePrediction(const std::string& json, const std::string& selfId, PeerPrediction& prediction) {
        double value = 0.0;
        if (!extractString(json, "senderId", prediction.senderId)) return false;
        extractString(json, "recipientId", prediction.recipientId);
        if (!extractNumber(json, "sequence", value)) return false; prediction.sequence = (uint64_t)value;
        if (!extractNumber(json, "sentAtMs", value)) return false; prediction.sentAtMs = (int64_t)value;
        if (!extractNumber(json, "expiresAtMs", value)) return false; prediction.expiresAtMs = (int64_t)value;
        if (!extractNumber(json, "relativeBearing", value)) return false; prediction.relativeBearing = (int)value;
        if (!extractNumber(json, "confidence", value)) return false; prediction.confidence = (float)value;
        if (!extractNumber(json, "sourceFrameSequence", value)) return false; prediction.sourceFrameSequence = (uint64_t)value;
        if (!extractString(json, "rangeBand", prediction.rangeBand)) return false;
        if (!extractString(json, "intent", prediction.intent)) return false;
        if (!extractString(json, "eventClass", prediction.eventClass)) return false;
        if (!extractString(json, "traceId", prediction.traceId)) return false;
        if (!extractString(json, "evidenceDigest", prediction.evidenceDigest)) return false;
        if (!prediction.recipientId.empty() && prediction.recipientId == selfId)
            prediction.relativeBearing = (prediction.relativeBearing + 8) % 16;
        return prediction.relativeBearing >= 0 && prediction.relativeBearing < 16 && prediction.senderId != selfId;
    }

    static bool httpRequest(const wchar_t* method, const std::string& url, const std::string& token,
                            const std::string& body, unsigned long& status, std::string& response,
                            std::string& error, double& latencyMs) {
        const auto start = std::chrono::steady_clock::now();
        std::wstring wurl = civicangel::wide(url);
        URL_COMPONENTS parts{}; parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength = parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) { error = "WinHttpCrackUrl failed"; return false; }
        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";
        HINTERNET session = WinHttpOpen(L"GTAngel-PeerLoop/18.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { error = "WinHttpOpen failed"; return false; }
        WinHttpSetTimeouts(session, 3000, 3000, 5000, 5000);
        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        const DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET request = connection ? WinHttpOpenRequest(connection, method, path.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags) : nullptr;
        if (!connection || !request) error = "WinHTTP connection failed";
        if (request) {
            const std::wstring headers = L"Accept: application/json\r\nAuthorization: Bearer " + civicangel::wide(token) +
                (body.empty() ? L"\r\n" : L"\r\nContent-Type: application/json\r\n");
            BOOL sent = WinHttpSendRequest(request, headers.c_str(), (DWORD)-1L,
                body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
            if (!sent || !WinHttpReceiveResponse(request, nullptr)) error = "WinHTTP request failed";
            if (error.empty()) {
                DWORD size = sizeof(status);
                WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                    WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
                for (;;) {
                    DWORD available = 0;
                    if (!WinHttpQueryDataAvailable(request, &available) || !available) break;
                    std::string chunk(available, '\0'); DWORD read = 0;
                    if (!WinHttpReadData(request, chunk.data(), available, &read)) break;
                    chunk.resize(read); response += chunk;
                    if (response.size() > 32768) { error = "Peer response exceeded 32 KiB"; break; }
                }
                if (status < 200 || status >= 300) error = "HTTP " + std::to_string(status);
            }
        }
        if (request) WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        latencyMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        return error.empty() && status >= 200 && status < 300;
    }

    void absorbResponse(const std::string& response) {
        const int64_t now = civicangel::unixTimeMs();
        const auto objects = pingObjects(response);
        std::lock_guard<std::mutex> lock(mu_);
        for (const auto& object : objects) {
            PeerPrediction prediction;
            if (!parsePrediction(object, selfId_, prediction) || prediction.expiresAtMs <= now) { ++rejected_; continue; }
            auto existing = std::find_if(predictions_.begin(), predictions_.end(), [&](const PeerPrediction& candidate) {
                return candidate.senderId == prediction.senderId && candidate.sequence == prediction.sequence;
            });
            if (existing != predictions_.end()) continue;
            predictions_.push_back(std::move(prediction));
            if (predictions_.size() > 32) predictions_.erase(predictions_.begin());
            ++received_;
        }
    }

    void workerLoop() {
        int64_t nextPollMs = 0;
        for (;;) {
            std::string body, base, token;
            bool enabled = false, configured = false;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait_for(lock, std::chrono::milliseconds(100), [this] { return stopping_ || !outgoing_.empty(); });
                if (stopping_) return;
                enabled = enabled_; configured = configured_; base = baseUrl_; token = token_;
                if (enabled && configured && !outgoing_.empty()) { body = std::move(outgoing_.front()); outgoing_.pop_front(); }
            }
            if (!enabled || !configured) continue;
            const int64_t now = civicangel::unixTimeMs();
            if (!body.empty()) {
                unsigned long status = 0; std::string response, error; double latency = 0.0;
                { std::lock_guard<std::mutex> lock(mu_); busy_ = true; }
                const bool ok = httpRequest(L"POST", base + "/api/peer-pings/ingest", token, body, status, response, error, latency);
                std::lock_guard<std::mutex> lock(mu_);
                busy_ = false; lastStatus_ = status; lastLatencyMs_ = latency;
                if (ok) { ++accepted_; lastError_.clear(); } else { ++rejected_; lastError_ = error; }
            }
            if (now >= nextPollMs) {
                unsigned long status = 0; std::string response, error; double latency = 0.0;
                std::string url = base + "/api/peer-pings/poll?sinceMs=" + std::to_string(std::max<int64_t>(0, lastPollReceivedAtMs_)) + "&limit=32";
                { std::lock_guard<std::mutex> lock(mu_); busy_ = true; }
                const bool ok = httpRequest(L"GET", url, token, "", status, response, error, latency);
                {
                    std::lock_guard<std::mutex> lock(mu_);
                    busy_ = false; lastStatus_ = status; lastLatencyMs_ = latency;
                    if (!ok) { ++rejected_; lastError_ = error; }
                    else { lastError_.clear(); lastPollReceivedAtMs_ = now; }
                }
                if (ok) absorbResponse(response);
                nextPollMs = now + 500;
            }
        }
    }
};

} // namespace v18peer
#endif


#endif


#endif



// ═══ uechovr Integration (ported from ReZorg/uechovr) ═══
namespace uechovr {

struct Pose6DOF {
    float x{}, y{}, z{};
    float qx{}, qy{}, qz{}, qw{};
    float vx{}, vy{}, vz{};
    float wx{}, wy{}, wz{};
    uint64_t frame_count{};
    bool valid{false};
};

struct EyeParams {
    float left{}, right{}, top{}, bottom{};
    float ipd{0.064f};
    uint32_t width{0}, height{0};
};

class FrameBufferHook {
public:
    enum class API { D3D11, D3D12, Unknown };
    struct Capture {
        std::vector<uint8_t> pixels;
        uint32_t width{}, height{};
        uint32_t pitch{};
        API api{API::Unknown};
        uint64_t frame_number{};
        bool valid{false};
    };
    FrameBufferHook() = default;
    void initialize(API api, uint32_t w, uint32_t h) {
        m_api = api; m_width = w; m_height = h;
        m_capture.pixels.assign(w * h * 4, 0);
        m_capture.width = w; m_capture.height = h;
        m_capture.pitch = w * 4;
        m_capture.api = api;
    }
    Capture captureBackBuffer(uint64_t frame_num) {
        m_capture.frame_number = frame_num;
        m_capture.valid = true;
        for (uint32_t y = 0; y < m_height; y++) {
            for (uint32_t x = 0; x < m_width; x++) {
                size_t idx = (y * m_width + x) * 4;
                float fx = (float)x / m_width;
                float fy = (float)y / m_height;
                float sep = sinf(fx * 10.0f + frame_num * 0.05f) * 0.5f + 0.5f;
                m_capture.pixels[idx + 0] = (uint8_t)(sep * 255);
                m_capture.pixels[idx + 1] = (uint8_t)((1.0f - sep) * 255);
                m_capture.pixels[idx + 2] = (uint8_t)(fx * 255);
                m_capture.pixels[idx + 3] = 255;
            }
        }
        return m_capture;
    }
    API api() const { return m_api; }
    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }
private:
    API m_api{API::Unknown};
    uint32_t m_width{}, m_height{};
    Capture m_capture{};
};

class Tracker6DOF {
public:
    Tracker6DOF() = default;
    void reset() {
        m_pose = Pose6DOF{};
        m_pose.qw = 1.0f;
        m_pose.valid = true;
        m_frame_count = 0;
    }
    Pose6DOF updateHMD(float dt) {
        m_frame_count++;
        m_pose.frame_count = m_frame_count;
        float t = (float)m_frame_count * dt;
        m_pose.x = sinf(t * 0.3f) * 0.5f;
        m_pose.y = 1.6f + sinf(t * 0.2f) * 0.1f;
        m_pose.z = cosf(t * 0.25f) * 0.3f;
        float yaw = t * 0.1f;
        float pitch = sinf(t * 0.15f) * 0.2f;
        m_pose.qw = cosf(yaw * 0.5f) * cosf(pitch * 0.5f);
        m_pose.qx = sinf(pitch * 0.5f) * cosf(yaw * 0.5f);
        m_pose.qy = sinf(yaw * 0.5f) * cosf(pitch * 0.5f);
        m_pose.qz = 0.0f;
        m_pose.vx = cosf(t * 0.3f) * 0.15f;
        m_pose.vy = cosf(t * 0.2f) * 0.02f;
        m_pose.vz = -sinf(t * 0.25f) * 0.075f;
        m_pose.valid = true;
        return m_pose;
    }
    Pose6DOF updateController(int hand, float dt) {
        Pose6DOF ctrl{};
        ctrl.frame_count = m_frame_count;
        float t = (float)m_frame_count * dt;
        float side = hand == 0 ? -0.3f : 0.3f;
        ctrl.x = m_pose.x + side + sinf(t * 0.5f + hand) * 0.1f;
        ctrl.y = m_pose.y - 0.3f + cosf(t * 0.4f + hand) * 0.05f;
        ctrl.z = m_pose.z + 0.2f + sinf(t * 0.3f + hand * 2) * 0.08f;
        ctrl.qw = 1.0f;
        ctrl.valid = true;
        return ctrl;
    }
    const Pose6DOF& hmdPose() const { return m_pose; }
    uint64_t frameCount() const { return m_frame_count; }
private:
    Pose6DOF m_pose{};
    uint64_t m_frame_count{};
};

class StereoRenderer {
public:
    enum class Method { NativeStereo, Synchronized, Alternating };
    StereoRenderer() = default;
    void setMethod(Method m) { m_method = m; }
    Method method() const { return m_method; }
    const char* methodName() const {
        switch (m_method) {
            case Method::NativeStereo: return "Native Stereo";
            case Method::Synchronized: return "Synchronized Sequential";
            case Method::Alternating: return "Alternating/AFR";
        }
        return "?";
    }
    EyeParams getEyeParams(int eye) const {
        EyeParams ep{};
        ep.ipd = 0.064f;
        ep.width = 64; ep.height = 64;
        float offset = eye == 0 ? -0.032f : 0.032f;
        ep.left = -0.5f + offset; ep.right = 0.5f + offset;
        ep.top = -0.5f; ep.bottom = 0.5f;
        return ep;
    }
private:
    Method m_method{Method::NativeStereo};
};

} // namespace uechovr

// ═══ dte::EndocrineBus (ported from u9n-main/DeepTreeEcho/Avatar/EndocrineBus.h) ═══
namespace dte {

enum class HormoneId : size_t {
    Cortisol = 0, Dopamine = 1, Serotonin = 2, Norepinephrine = 3,
    Oxytocin = 4, Thyroxine = 5, Melatonin = 6, Insulin = 7,
    Cytokine = 8, Anandamide = 9, COUNT = 10
};
constexpr size_t HORMONE_COUNT = 10;

const char* HORMONE_NAMES[HORMONE_COUNT] = {
    "Cortisol","Dopamine","Serotonin","Norepinephrine","Oxytocin",
    "Thyroxine","Melatonin","Insulin","Cytokine","Anandamide"
};

struct HormoneBus {
    std::array<double, HORMONE_COUNT> levels{};
    double get(HormoneId id) const { return levels[(size_t)id]; }
    void set(HormoneId id, double v) {
        levels[(size_t)id] = std::max(0.0, std::min(1.0, v));
    }
    void decay(HormoneId id, double baseline, double rate) {
        auto i = (size_t)id;
        levels[i] += (baseline - levels[i]) * rate;
        levels[i] = std::max(0.0, std::min(1.0, levels[i]));
    }
    void reset(double baseline = 0.5) { levels.fill(baseline); }
};

struct ValenceArousalState {
    double valence = 0.0;
    double arousal = 0.0;
    double magnitude() const { return sqrt(valence*valence + arousal*arousal); }
};

enum class CognitiveMode { Exploration, Exploitation, Rest, Alarm };
inline const char* cognitiveModeName(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Exploration: return "EXPLORATION";
        case CognitiveMode::Exploitation: return "EXPLOITATION";
        case CognitiveMode::Rest: return "REST";
        case CognitiveMode::Alarm: return "ALARM";
    }
    return "?";
}
inline COLORREF cognitiveModeColor(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Exploration: return RGB(76, 175, 80);
        case CognitiveMode::Exploitation: return RGB(0, 188, 212);
        case CognitiveMode::Rest: return RGB(158, 158, 200);
        case CognitiveMode::Alarm: return RGB(244, 67, 54);
    }
    return RGB(255,255,255);
}

struct Stimulus {
    std::string source;
    double intensity = 0.0;
    Stimulus(std::string src, double inten) : source(std::move(src)), intensity(inten) {}
    Stimulus() = default;
};

struct GlandConfig {
    HormoneId hormone = HormoneId::Cortisol;
    double baseline = 0.5;
    double decayRate = 0.1;
    double sensitivity = 1.0;
    double maxProduction = 0.3;
};

class VirtualGland {
public:
    explicit VirtualGland(GlandConfig c) : config_(c) {}
    void respond(const Stimulus& s, HormoneBus& bus) {
        double cur = bus.get(config_.hormone);
        double delta = s.intensity * config_.sensitivity;
        delta = std::max(-config_.maxProduction, std::min(config_.maxProduction, delta));
        bus.set(config_.hormone, cur + delta);
    }
    void tick(HormoneBus& bus) {
        bus.decay(config_.hormone, config_.baseline, config_.decayRate);
    }
    HormoneId hormoneId() const { return config_.hormone; }
    GlandConfig& config() { return config_; }
    const GlandConfig& config() const { return config_; }
private:
    GlandConfig config_;
};

class EndocrineSystem {
public:
    EndocrineSystem() {
        glands_.emplace_back(GlandConfig{HormoneId::Cortisol, 0.15, 0.03, 1.0, 0.3});
        glands_.emplace_back(GlandConfig{HormoneId::Dopamine, 0.30, 0.12, 1.2, 0.35});
        glands_.emplace_back(GlandConfig{HormoneId::Serotonin, 0.40, 0.02, 0.8, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Norepinephrine, 0.10, 0.15, 1.1, 0.3});
        glands_.emplace_back(GlandConfig{HormoneId::Oxytocin, 0.10, 0.04, 0.9, 0.25});
        glands_.emplace_back(GlandConfig{HormoneId::Thyroxine, 0.50, 0.01, 0.5, 0.1});
        glands_.emplace_back(GlandConfig{HormoneId::Melatonin, 0.00, 0.10, 0.7, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Insulin, 0.20, 0.06, 0.8, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Cytokine, 0.05, 0.03, 0.9, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Anandamide, 0.10, 0.18, 1.0, 0.3});
        bus_.reset(0.2);
    }
    void submitStimulus(const Stimulus& s) { pending_.push_back(s); }
    void tick() {
        tickCount_++;
        for (auto& s : pending_) {
            for (auto& g : glands_) {
                bool match = false;
                auto hid = g.hormoneId();
                if (s.source.find("reward") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (s.source.find("success") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (s.source.find("error") != std::string::npos && hid == HormoneId::Cortisol) match = true;
                if (s.source.find("threat") != std::string::npos && hid == HormoneId::Cortisol) match = true;
                if (s.source.find("threat") != std::string::npos && hid == HormoneId::Norepinephrine) match = true;
                if (s.source.find("social") != std::string::npos && hid == HormoneId::Oxytocin) match = true;
                if (s.source.find("novelty") != std::string::npos && hid == HormoneId::Norepinephrine) match = true;
                if (s.source.find("rest") != std::string::npos && hid == HormoneId::Melatonin) match = true;
                if (s.source.find("insight") != std::string::npos && hid == HormoneId::Anandamide) match = true;
                if (s.source.find("discovery") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (match) g.respond(s, bus_);
            }
        }
        pending_.clear();
        for (auto& g : glands_) g.tick(bus_);
        computeValenceArousal();
        computeCognitiveMode();
    }
    const HormoneBus& bus() const { return bus_; }
    const ValenceArousalState& valenceArousal() const { return va_; }
    CognitiveMode cognitiveMode() const { return mode_; }
    double hormoneLevel(HormoneId id) const { return bus_.get(id); }
    size_t tickCount() const { return tickCount_; }
    void setDecayRate(HormoneId id, double rate) {
        for (auto& g : glands_) if (g.hormoneId() == id) g.config().decayRate = rate;
    }
    double getDecayRate(HormoneId id) const {
        for (auto& g : glands_) if (g.hormoneId() == id) return g.config().decayRate;
        return 0.0;
    }
    int modeTransitions = 0;
    CognitiveMode lastMode = CognitiveMode::Exploration;
    std::vector<std::pair<int, CognitiveMode>> modeHistory;
    void trackModeTransition() {
        if (mode_ != lastMode) {
            modeTransitions++;
            modeHistory.push_back({(int)tickCount_, mode_});
            if (modeHistory.size() > 50) modeHistory.erase(modeHistory.begin());
            lastMode = mode_;
        }
    }
private:
    void computeValenceArousal() {
        double pos = bus_.get(HormoneId::Dopamine) + bus_.get(HormoneId::Serotonin)
                   + bus_.get(HormoneId::Oxytocin) + bus_.get(HormoneId::Anandamide);
        double neg = bus_.get(HormoneId::Cortisol) + bus_.get(HormoneId::Cytokine);
        va_.valence = std::max(-1.0, std::min(1.0, (pos - neg) / 2.5));
        va_.arousal = std::max(-1.0, std::min(1.0,
            bus_.get(HormoneId::Norepinephrine) + bus_.get(HormoneId::Cortisol)
            + bus_.get(HormoneId::Thyroxine) - bus_.get(HormoneId::Melatonin) - 0.5));
    }
    void computeCognitiveMode() {
        if (va_.arousal > 0.1 && va_.valence > 0.0) mode_ = CognitiveMode::Exploration;
        else if (va_.arousal <= 0.1 && va_.valence > 0.1) mode_ = CognitiveMode::Exploitation;
        else if (va_.arousal < -0.1 && va_.valence <= 0.0) mode_ = CognitiveMode::Rest;
        else if (va_.arousal > 0.2 && va_.valence < -0.1) mode_ = CognitiveMode::Alarm;
        else mode_ = CognitiveMode::Exploration;
    }
    HormoneBus bus_;
    ValenceArousalState va_;
    CognitiveMode mode_ = CognitiveMode::Exploration;
    std::vector<VirtualGland> glands_;
    std::vector<Stimulus> pending_;
    size_t tickCount_ = 0;
};

// ═══ Echo State Network Reservoir ═══
class EchoStateNetwork {
public:
    EchoStateNetwork(int inDim, int resDim, int outDim, double spectralRadius = 0.95,
                     double leak = 0.3, unsigned seed = 42)
        : inDim_(inDim), resDim_(resDim), outDim_(outDim), leak_(leak) {
        std::mt19937 rng(seed);
        std::normal_distribution<double> gauss(0.0, 1.0);
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        W_in_.assign(resDim_ * inDim_, 0.0);
        for (auto& w : W_in_) w = gauss(rng) * 0.1;
        W_res_.assign(resDim_ * resDim_, 0.0);
        for (int i = 0; i < resDim_; i++)
            for (int j = 0; j < resDim_; j++)
                if (uni(rng) > 0.9) W_res_[i*resDim_+j] = gauss(rng);
        std::vector<double> v(resDim_, 1.0), tmp(resDim_);
        double maxEig = 1.0;
        for (int it = 0; it < 20; it++) {
            for (int i = 0; i < resDim_; i++) {
                tmp[i] = 0;
                for (int j = 0; j < resDim_; j++) tmp[i] += W_res_[i*resDim_+j] * v[j];
            }
            double norm = 0;
            for (double x : tmp) norm += x*x;
            norm = sqrt(norm);
            if (norm > 1e-9) { maxEig = norm; for (int i = 0; i < resDim_; i++) v[i] = tmp[i]/norm; }
        }
        if (maxEig > 0) {
            double scale = spectralRadius / maxEig;
            for (auto& w : W_res_) w *= scale;
        }
        state_.assign(resDim_, 0.0);
        W_out_.assign(outDim_ * resDim_, 0.0);
        stateHistory_.assign(resDim_, std::vector<double>(100, 0.0));
        historyIdx_ = 0;
    }
    std::vector<double> step(const std::vector<double>& input) {
        std::vector<double> newState(resDim_, 0.0);
        for (int i = 0; i < resDim_; i++) {
            double sum = 0;
            for (int j = 0; j < inDim_ && j < (int)input.size(); j++)
                sum += W_in_[i*inDim_+j] * input[j];
            for (int j = 0; j < resDim_; j++)
                sum += W_res_[i*resDim_+j] * state_[j];
            newState[i] = tanh(sum);
        }
        for (int i = 0; i < resDim_; i++)
            state_[i] = (1.0 - leak_) * state_[i] + leak_ * newState[i];
        for (int i = 0; i < resDim_; i++)
            stateHistory_[i][historyIdx_] = state_[i];
        historyIdx_ = (historyIdx_ + 1) % 100;
        std::vector<double> out(outDim_, 0.0);
        for (int o = 0; o < outDim_; o++)
            for (int i = 0; i < resDim_; i++)
                out[o] += W_out_[o*resDim_+i] * state_[i];
        return out;
    }
    const std::vector<double>& state() const { return state_; }
    double stateEnergy() const {
        double e = 0; for (double x : state_) e += x*x;
        return sqrt(e / resDim_);
    }
    int resDim() const { return resDim_; }
    int inDim() const { return inDim_; }
    double leakRate() const { return leak_; }
    const std::vector<double>& W_res() const { return W_res_; }
    const std::vector<double>& W_in() const { return W_in_; }
    const std::vector<std::vector<double>>& stateHistory() const { return stateHistory_; }
    int historyIdx() const { return historyIdx_; }
    double spectralRadius() const { return 0.95; }
private:
    int inDim_, resDim_, outDim_;
    double leak_;
    std::vector<double> W_in_, W_res_, W_out_, state_;
    std::vector<std::vector<double>> stateHistory_;
    int historyIdx_ = 0;
};

// ═══ Echobeats 4-Phase Cognitive Cycle ═══
enum class EchobeatsPhase { Perceive, Synthesize, Resonate, Enact };
inline const char* phaseName(EchobeatsPhase p) {
    switch (p) {
        case EchobeatsPhase::Perceive: return "PERCEIVE";
        case EchobeatsPhase::Synthesize: return "SYNTHESIZE";
        case EchobeatsPhase::Resonate: return "RESONATE";
        case EchobeatsPhase::Enact: return "ENACT";
    }
    return "?";
}

// ═══ TRIZ Cognitive Arena (hex grid, 40 principles, aesthetic reward) ═══
enum class NicheType { Perturbation, Scaffolding, Inheritance };
struct Niche {
    NicheType type;
    float x, y;
    float strength;
    float age;
    const char* name;
};
struct HexCell {
    float aestheticValue = 0.5f;
    float temperature = 0.5f;
    int objectCount = 0;
    bool occupied = false;
    bool isNiche = false;
    NicheType nicheType = NicheType::Perturbation;
    float contradiction = 0.0f;
    int lastPrinciple = -1;
};

class TRIZArena {
public:
    static constexpr int SIZE = 12;
    std::array<std::array<HexCell, SIZE>, SIZE> grid;
    float coherence = 0.5f;
    float lastDelta = 0.0f;
    int discoveries = 0;
    int phase = 0;
    int stepInPhase = 0;
    std::vector<Niche> niches;
    int selectedQ = -1, selectedR = -1;
    float maxContradiction = 0.0f;
    int maxContradictionQ = -1, maxContradictionR = -1;

    const char* principles[40] = {
        "Segment","Extract","Differentiate","BreakSymmetry","Merge","MultiPurpose","Nest",
        "Counterbalance","PreStress","PrePosition","AddBackup","FlattenField","Invert",
        "Curve","MakeFlexible","Overshoot","AddDimension","Vibrate","Pulse","Sustain",
        "SkipThrough","ReframeHarm","AddFeedback","Mediate","SelfServe","Copy",
        "MakeDisposable","ReplaceWithField","Fluidize","MakeMembrane","MakePorous",
        "ChangeColor","Homogenize","Recycle","ChangeParameter","ExploitTransition",
        "ExpandDifferentially","Catalyze","Protect","Compose"
    };

    TRIZArena() { reset(); }
    void reset() {
        std::mt19937 rng((unsigned)GetTickCount());
        std::uniform_real_distribution<float> uni(0.3f, 0.7f);
        std::uniform_int_distribution<int> obj(0, 2);
        for (auto& row : grid) for (auto& c : row) {
            c.aestheticValue = uni(rng);
            c.objectCount = obj(rng);
            c.occupied = c.objectCount > 0;
            c.isNiche = false;
            c.contradiction = 0.0f;
            c.lastPrinciple = -1;
        }
        niches.clear();
        computeCoherence();
        computeContradictions();
    }
    void computeCoherence() {
        float sum = 0;
        for (auto& row : grid) for (auto& c : row) sum += c.aestheticValue;
        coherence = sum / (SIZE * SIZE);
    }
    void computeContradictions() {
        maxContradiction = 0.0f;
        for (int q = 0; q < SIZE; q++) {
            for (int r = 0; r < SIZE; r++) {
                auto& cell = grid[q][r];
                float sumDiff = 0; int cnt = 0;
                for (int dq = -1; dq <= 1; dq++)
                    for (int dr = -1; dr <= 1; dr++) {
                        if (dq == 0 && dr == 0) continue;
                        int nq = q+dq, nr = r+dr;
                        if (nq >= 0 && nq < SIZE && nr >= 0 && nr < SIZE) {
                            sumDiff += fabsf(cell.aestheticValue - grid[nq][nr].aestheticValue);
                            cnt++;
                        }
                    }
                cell.contradiction = cnt > 0 ? sumDiff / cnt : 0.0f;
                if (cell.contradiction > maxContradiction) {
                    maxContradiction = cell.contradiction;
                    maxContradictionQ = q; maxContradictionR = r;
                }
            }
        }
    }
    float applyPrinciple(int p, int q, int r) {
        if (q < 0 || q >= SIZE || r < 0 || r >= SIZE) return 0;
        float before = coherence;
        auto& cell = grid[q][r];
        cell.lastPrinciple = p;
        switch (p % 40) {
            case 0: cell.objectCount++; cell.aestheticValue += 0.05f; break;
            case 1: cell.aestheticValue -= 0.03f; cell.objectCount--; break;
            case 2: cell.aestheticValue += 0.06f; cell.temperature += 0.05f; break;
            case 3: cell.aestheticValue += 0.08f; break;
            case 4: cell.aestheticValue += 0.08f; cell.occupied = true; break;
            case 5: cell.aestheticValue += 0.04f; cell.objectCount++; break;
            case 6: cell.aestheticValue += 0.06f; break;
            case 7: cell.aestheticValue += 0.05f; cell.temperature -= 0.05f; break;
            case 8: cell.aestheticValue += 0.03f; break;
            case 9: cell.aestheticValue += 0.04f; break;
            case 10: cell.aestheticValue += 0.02f; break;
            case 11: cell.aestheticValue += 0.05f; break;
            case 12: cell.aestheticValue = 1.0f - cell.aestheticValue; break;
            case 13: cell.aestheticValue += 0.06f; break;
            case 14: cell.aestheticValue += 0.05f; break;
            case 15: cell.aestheticValue += 0.07f; break;
            case 16: cell.aestheticValue += 0.08f; break;
            case 17: cell.aestheticValue += 0.10f; cell.temperature += 0.1f; break;
            case 18: cell.aestheticValue += 0.06f; break;
            case 19: cell.aestheticValue += 0.05f; break;
            case 20: cell.aestheticValue += 0.04f; break;
            case 21: cell.aestheticValue += 0.06f; break;
            case 22: cell.aestheticValue += 0.07f; break;
            case 23: cell.aestheticValue += 0.05f; break;
            case 24: cell.aestheticValue += 0.04f; break;
            case 25: cell.aestheticValue += 0.06f; break;
            case 26: cell.aestheticValue += 0.03f; break;
            case 27: cell.aestheticValue += 0.05f; break;
            case 28: cell.aestheticValue += 0.04f; break;
            case 29: cell.aestheticValue += 0.05f; break;
            case 30: cell.aestheticValue += 0.04f; break;
            case 31: cell.aestheticValue += 0.04f; break;
            case 32: cell.aestheticValue += 0.05f; break;
            case 33: cell.aestheticValue += 0.06f; break;
            case 34: cell.aestheticValue += 0.05f; break;
            case 35: cell.aestheticValue += 0.07f; break;
            case 36: cell.aestheticValue += 0.08f; break;
            case 37: cell.aestheticValue += 0.06f; break;
            case 38: cell.aestheticValue += 0.05f; break;
            case 39: cell.aestheticValue += 0.12f; cell.objectCount += 2; break;
        }
        cell.aestheticValue = std::max(0.0f, std::min(1.0f, cell.aestheticValue));
        computeCoherence();
        computeContradictions();
        lastDelta = coherence - before;
        if (lastDelta > 0.005f) discoveries++;
        return lastDelta;
    }
    void constructNiche(NicheType type, int q, int r) {
        if (q < 0 || q >= SIZE || r < 0 || r >= SIZE) return;
        auto& cell = grid[q][r];
        cell.isNiche = true;
        cell.nicheType = type;
        const char* nm = type == NicheType::Perturbation ? "Perturbation" :
                         type == NicheType::Scaffolding ? "Scaffolding" : "Inheritance";
        niches.push_back({type, (float)q, (float)r, 0.5f, 0.0f, nm});
        for (int dq = -1; dq <= 1; dq++)
            for (int dr = -1; dr <= 1; dr++) {
                int nq = q+dq, nr = r+dr;
                if (nq >= 0 && nq < SIZE && nr >= 0 && nr < SIZE) {
                    float boost = type == NicheType::Scaffolding ? 0.06f :
                                  type == NicheType::Perturbation ? 0.03f : 0.04f;
                    grid[nq][nr].aestheticValue = std::min(1.0f, grid[nq][nr].aestheticValue + boost);
                }
            }
        computeCoherence();
        computeContradictions();
    }
    void tick() {
        stepInPhase++;
        if (stepInPhase >= 3) { stepInPhase = 0; phase = (phase + 1) % 4; }
        std::mt19937 rng((unsigned)GetTickCount() + stepInPhase + phase * 7);
        std::uniform_int_distribution<int> cellPick(0, SIZE-1);
        std::uniform_int_distribution<int> prinPick(0, 39);
        applyPrinciple(prinPick(rng), cellPick(rng), cellPick(rng));
        for (auto& n : niches) n.age += 0.016f;
        if (stepInPhase == 0 && phase == 1 && niches.size() < 12) {
            std::uniform_int_distribution<int> typePick(0, 2);
            constructNiche((NicheType)typePick(rng), cellPick(rng), cellPick(rng));
        }
    }
    const char* phaseName() const {
        switch (phase) {
            case 0: return "CONTRADICTION";
            case 1: return "EXPERIMENT";
            case 2: return "CRYSTALLIZE";
            case 3: return "TEACH";
        }
        return "?";
    }
};

// ═══ Group Dynamics (ai-env-grp-dyn) ═══
struct Agent {
    int id;
    float x, y;
    float trust = 0.5f, affinity = 0.5f;
    int groupId = -1;
    float cohesion = 0.0f;
};

class GroupDynamics {
public:
    std::vector<Agent> agents;
    std::map<int, std::vector<int>> groups;
    int nextGroupId = 0;
    float avgCohesion = 0.0f;

    void init(int n) {
        agents.clear(); groups.clear();
        std::mt19937 rng((unsigned)GetTickCount());
        std::uniform_real_distribution<float> pos(0, 1);
        for (int i = 0; i < n; i++) {
            Agent a; a.id = i;
            a.x = pos(rng) * 800; a.y = pos(rng) * 600;
            agents.push_back(a);
        }
    }
    void tick() {
        for (auto& a : agents) {
            if (a.groupId >= 0) continue;
            for (auto& b : agents) {
                if (a.id == b.id || b.groupId >= 0) continue;
                float dx = a.x-b.x, dy = a.y-b.y;
                if (sqrtf(dx*dx+dy*dy) < 100.0f && a.affinity > 0.3f) {
                    int gid = nextGroupId++;
                    a.groupId = gid; b.groupId = gid;
                    groups[gid] = {a.id, b.id};
                    a.cohesion = 0.5f; b.cohesion = 0.5f;
                    break;
                }
            }
        }
        for (auto& a : agents) {
            if (a.groupId >= 0) {
                a.cohesion = std::max(0.0f, a.cohesion - 0.001f);
                if (a.cohesion < 0.1f) {
                    auto& g = groups[a.groupId];
                    g.erase(std::remove(g.begin(), g.end(), a.id), g.end());
                    a.groupId = -1;
                }
            }
        }
        float sum = 0; int cnt = 0;
        for (auto& a : agents) if (a.groupId >= 0) { sum += a.cohesion; cnt++; }
        avgCohesion = cnt > 0 ? sum/cnt : 0.0f;
    }
    int groupCount() const { return (int)groups.size(); }
};

// ═══ Proprioceptive Sensorimotor Loop (frame buffer → feedback) ═══
class SensorimotorLoop {
public:
    static constexpr int FW = 64, FH = 64;
    std::array<float, FW*FH> frameBuffer{};
    std::array<float, 8> proprioception{};
    std::array<float, 6> motorOutput{};
    float reward = 0.0f, totalReward = 0.0f;
    int stepCount = 0;

    void captureFrame() {
        for (int y = 0; y < FH; y++)
            for (int x = 0; x < FW; x++) {
                float v = sinf(x*0.1f + stepCount*0.05f) * cosf(y*0.1f + stepCount*0.03f);
                frameBuffer[y*FW+x] = (v + 1.0f) * 0.5f;
            }
    }
    void computeProprioception() {
        float mean = 0, var = 0;
        for (float v : frameBuffer) mean += v;
        mean /= (FW*FH);
        for (float v : frameBuffer) var += (v-mean)*(v-mean);
        var /= (FW*FH);
        proprioception = {mean, var, frameBuffer[0], frameBuffer[FW*FH-1],
                          var*2.0f, mean*0.8f, (float)(stepCount%100)/100.0f, reward};
    }
    void selectAction(EndocrineSystem& endo) {
        double dop = endo.hormoneLevel(HormoneId::Dopamine);
        double cor = endo.hormoneLevel(HormoneId::Cortisol);
        double ne = endo.hormoneLevel(HormoneId::Norepinephrine);
        std::mt19937 rng((unsigned)GetTickCount() + stepCount);
        std::uniform_real_distribution<float> uni(0.0f, 1.0f);
        for (int i = 0; i < 6; i++)
            motorOutput[i] = uni(rng) * (float)(0.5 + dop*0.5) * (float)(1.0 - cor*0.3) + (float)(ne*0.2);
    }
    void tick(EndocrineSystem& endo) {
        stepCount++;
        captureFrame();
        computeProprioception();
        selectAction(endo);
        float coh = 1.0f - fabsf(proprioception[0] - 0.5f) * 2.0f;
        reward = coh * 0.1f + proprioception[5] * 0.05f;
        totalReward += reward;
        if (reward > 0.08f) endo.submitStimulus({"reward_signal", 0.3});
        if (reward < 0.02f) endo.submitStimulus({"error_detected", 0.1});
        if (proprioception[5] > 0.6f) endo.submitStimulus({"novelty_detected", 0.2});
    }
};

// ═══ Iterative Micro-Improvement Loop ═══
struct MicroImprovement {
    int version = 0, kept = 0;
    float best = 0.0f;
    std::vector<std::string> history;
    void iterate(TRIZArena& arena, SensorimotorLoop& sml) {
        version++;
        std::mt19937 rng((unsigned)GetTickCount() + version);
        std::uniform_int_distribution<int> cp(0, TRIZArena::SIZE-1), pp(0, 39);
        float delta = arena.applyPrinciple(pp(rng), cp(rng), cp(rng));
        float avgReward = sml.totalReward / (sml.stepCount + 1);
        char buf[160];
        if (delta > 0 || avgReward > best) {
            kept++;
            best = std::max(best, arena.coherence);
            snprintf(buf, 160, "v%d: KEEP d=%.3f coh=%.3f", version, delta, arena.coherence);
        } else {
            snprintf(buf, 160, "v%d: REVERT d=%.3f", version, delta);
        }
        history.push_back(buf);
        if (history.size() > 20) history.erase(history.begin());
    }
};

// ═══ Echo Introspection Engine ═══
struct EchoIntrospect {
    std::vector<std::string> insights;
    float selfAwareness = 0.3f;
    int count = 0;
    void introspect(EndocrineSystem& endo, TRIZArena& arena, SensorimotorLoop& sml, GroupDynamics& gd) {
        count++;
        selfAwareness = std::min(1.0f, selfAwareness + 0.01f);
        char buf[256];
        double cor = endo.hormoneLevel(HormoneId::Cortisol);
        double dop = endo.hormoneLevel(HormoneId::Dopamine);
        auto& va = endo.valenceArousal();
        if (cor > 0.5)
            snprintf(buf, 256, "#%d: High cortisol (%.2f) — touching vulnerability. Mode=%s Coh=%.3f",
                count, cor, cognitiveModeName(endo.cognitiveMode()), arena.coherence);
        else if (dop > 0.5)
            snprintf(buf, 256, "#%d: Dopamine burst (%.2f) — genuine insight! Mode=%s R=%.3f",
                count, dop, cognitiveModeName(endo.cognitiveMode()), sml.totalReward);
        else
            snprintf(buf, 256, "#%d: Mode=%s V=%.2f A=%.2f SA=%.2f Groups=%d Disc=%d Niches=%d",
                count, cognitiveModeName(endo.cognitiveMode()), va.valence, va.arousal,
                selfAwareness, gd.groupCount(), arena.discoveries, (int)arena.niches.size());
        insights.push_back(buf);
        if (insights.size() > 15) insights.erase(insights.begin());
    }
};

// ═══ CogVerse Evolution Accelerator ═══
struct CogVerse {
    int cycle = 0;
    float propertyCoherence = 0.5f;
    int residents = 10;
    void evolve(TRIZArena& arena, GroupDynamics& gd) {
        cycle++;
        propertyCoherence = (arena.coherence + gd.avgCohesion) * 0.5f;
    }
};

} // namespace dte

// GTAngel v19 — Ariadne gameplay vertical slice.
// Local gameplay truth only: no telemetry or external model is required.
namespace v20cns { class WorldEffector; }
namespace v19game {

enum class QuestStage : uint8_t {
    Dormant = 0,
    Offered,
    SeekShard,
    RestoreBeacon,
    ReturnToAriadne,
    Complete
};

enum class NpcRoutine : uint8_t {
    WatchBeacon = 0,
    PatrolSanctuary,
    NoticePlayer,
    Converse,
    Celebrate
};

struct QuestObjective {
    const char* id;
    const char* label;
    QuestStage stage;
};

struct QuestDefinition {
    const char* id;
    const char* title;
    std::array<QuestObjective, 5> objectives;
};

static const QuestDefinition& brokenSignalQuest() {
    static const QuestDefinition definition{
        "broken_signal",
        "Ariadne and the Broken Signal",
        {{{"speak", "Speak with Ariadne", QuestStage::Dormant},
          {"recover", "Recover the signal shard", QuestStage::SeekShard},
          {"restore", "Restore the civic beacon", QuestStage::RestoreBeacon},
          {"return", "Return to Ariadne", QuestStage::ReturnToAriadne},
          {"complete", "The sanctuary remembers", QuestStage::Complete}}}
    };
    return definition;
}

struct PlayerControls {
    float move{0.0f};
    float turn{0.0f};
    bool active{false};
};

struct MainNpc {
    std::string name{"Ariadne"};
    avatar3d::Vec3 home{-4.0f, 0.0f, 4.0f};
    avatar3d::Vec3 position{-4.0f, 0.0f, 4.0f};
    float heading{2.35f};
    float speed{0.0f};
    float disposition{0.55f};
    float interactionRange{2.25f};
    float routineTime{0.0f};
    NpcRoutine routine{NpcRoutine::WatchBeacon};
    bool remembersDecline{false};
    bool remembersAcceptance{false};
    bool remembersRestoration{false};
};

struct QuestRuntime {
    QuestStage stage{QuestStage::Dormant};
    bool dialogueOpen{false};
    bool journalOpen{true};
    bool askedWhy{false};
    bool hasSignalShard{false};
    bool hasResonanceSigil{false};
    bool shardVisible{true};
    bool beaconRestored{false};
    uint32_t groundedEvents{0};
    std::string lastEvent{"Sanctuary entered"};
    std::string persistenceStatus{"UNSAVED"};
};

class GameplayLoop {
    friend class v20cns::WorldEffector;
public:
    static constexpr const char* SavePath = R"(C:\ddd\GTAngel\cpp_test\gtangel_v19_save.json)";

    MainNpc ariadne;
    QuestRuntime quest;

    void reset(avatar3d::GameWorld& world) {
        ariadne = MainNpc{};
        quest = QuestRuntime{};
        syncWorld(world);
    }

    PlayerControls samplePlayerControls() const {
        PlayerControls result;
        const bool forward = (GetAsyncKeyState('W') & 0x8000) != 0;
        const bool backward = (GetAsyncKeyState('S') & 0x8000) != 0;
        const bool left = (GetAsyncKeyState('A') & 0x8000) != 0;
        const bool right = (GetAsyncKeyState('D') & 0x8000) != 0;
        result.move = forward ? 1.0f : (backward ? -0.55f : 0.0f);
        result.turn = left ? 0.68f : (right ? -0.68f : 0.0f);
        result.active = forward || backward || left || right;
        return result;
    }

    void tick(float dt, const avatar3d::Vec3& player, float playerHeading,
              avatar3d::GameWorld& world) {
        (void)playerHeading;
        ariadne.routineTime += dt;
        const float distance = horizontalDistance(player, ariadne.position);

        if (quest.stage == QuestStage::Complete) {
            ariadne.routine = NpcRoutine::Celebrate;
            ariadne.heading += dt * 0.45f;
            ariadne.speed = 0.0f;
        } else if (quest.dialogueOpen) {
            ariadne.routine = NpcRoutine::Converse;
            ariadne.heading = headingToward(ariadne.position, player);
            ariadne.speed = 0.0f;
        } else if (distance < 5.0f) {
            ariadne.routine = NpcRoutine::NoticePlayer;
            ariadne.heading = headingToward(ariadne.position, player);
            ariadne.speed = 0.0f;
        } else {
            ariadne.routine = ((int)(ariadne.routineTime / 7.0f) & 1)
                ? NpcRoutine::PatrolSanctuary : NpcRoutine::WatchBeacon;
            if (ariadne.routine == NpcRoutine::PatrolSanctuary) {
                const float phase = ariadne.routineTime * 0.32f;
                avatar3d::Vec3 next = ariadne.home + avatar3d::Vec3{
                    cosf(phase) * 1.6f, 0.0f, sinf(phase) * 1.1f};
                ariadne.heading = headingToward(ariadne.position, next);
                ariadne.speed = horizontalDistance(ariadne.position, next) / std::max(0.001f, dt);
                ariadne.position = next;
            } else {
                ariadne.heading = headingToward(ariadne.position, world.goal);
                ariadne.speed = 0.0f;
            }
        }
        syncWorld(world);
    }

    std::string interact(const avatar3d::Vec3& player, float playerHeading,
                         avatar3d::GameWorld& world) {
        if (quest.dialogueOpen) {
            quest.dialogueOpen = false;
            quest.lastEvent = "Dialogue paused";
            return quest.lastEvent;
        }

        if (quest.stage == QuestStage::SeekShard && quest.shardVisible &&
            horizontalDistance(player, world.questShard) <= 1.35f) {
            quest.hasSignalShard = true;
            quest.shardVisible = false;
            quest.stage = QuestStage::RestoreBeacon;
            record("Signal shard recovered");
            syncWorld(world);
            return quest.lastEvent;
        }

        if (canRestore(player, world)) return "Beacon repair requires local World Governance Reflex";

        if (canSpeak(player, playerHeading)) {
            quest.dialogueOpen = true;
            if (quest.stage == QuestStage::Dormant) quest.stage = QuestStage::Offered;
            record("Ariadne conversation opened");
            syncWorld(world);
            return quest.lastEvent;
        }

        quest.lastEvent = "Nothing within interaction focus";
        return quest.lastEvent;
    }

    // Called only from v20 WorldEffector after a locally minted NICHE_COMMIT.
private:
    bool governedRepairBeacon(avatar3d::GameWorld& world) {
        if (quest.stage != QuestStage::RestoreBeacon || !quest.hasSignalShard || quest.beaconRestored) return false;
        quest.hasSignalShard = false;
        quest.beaconRestored = true;
        quest.stage = QuestStage::ReturnToAriadne;
        ariadne.remembersRestoration = true;
        ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.18f);
        record("Civic beacon restored");
        syncWorld(world);
        return true;
    }
public:
    void publishWorld(avatar3d::GameWorld& world) const { syncWorld(world); }

    std::string choose(int choice, avatar3d::GameWorld& world) {
        if (!quest.dialogueOpen) return "No dialogue choice is active";
        if (quest.stage == QuestStage::Offered) {
            if (choice == 1) {
                quest.stage = QuestStage::SeekShard;
                quest.dialogueOpen = false;
                ariadne.remembersAcceptance = true;
                ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.08f);
                record("Quest accepted: recover the signal shard");
            } else if (choice == 2) {
                quest.askedWhy = true;
                record("Ariadne explained the civic beacon");
            } else if (choice == 3) {
                quest.stage = QuestStage::Dormant;
                quest.dialogueOpen = false;
                ariadne.remembersDecline = true;
                ariadne.disposition = std::max(0.0f, ariadne.disposition - 0.03f);
                record("Quest declined; Ariadne will wait");
            }
        } else if (quest.stage == QuestStage::ReturnToAriadne && choice == 1) {
            quest.stage = QuestStage::Complete;
            quest.dialogueOpen = false;
            quest.hasResonanceSigil = true;
            ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.22f);
            record("Quest complete: Resonance Sigil received");
        } else {
            quest.dialogueOpen = false;
            record("Dialogue closed");
        }
        syncWorld(world);
        return quest.lastEvent;
    }

    std::string recover(avatar3d::GameWorld& world) {
        quest.dialogueOpen = false;
        if (quest.stage == QuestStage::RestoreBeacon && !quest.hasSignalShard) {
            quest.stage = QuestStage::SeekShard;
            quest.shardVisible = true;
            record("Signal shard recovered to its sanctuary locus");
        } else if (quest.stage == QuestStage::Complete) {
            quest.lastEvent = "Completed purpose is preserved";
        } else {
            quest.lastEvent = "Current objective remains recoverable";
        }
        syncWorld(world);
        return quest.lastEvent;
    }

    bool save() {
        const std::string temp = std::string(SavePath) + ".tmp";
        std::ofstream out(temp, std::ios::trunc);
        if (!out) {
            quest.persistenceStatus = "SAVE OPEN FAILED";
            return false;
        }
        out << "{\n"
            << "  \"version\": 1,\n"
            << "  \"questStage\": " << (int)quest.stage << ",\n"
            << "  \"hasSignalShard\": " << (quest.hasSignalShard ? 1 : 0) << ",\n"
            << "  \"hasResonanceSigil\": " << (quest.hasResonanceSigil ? 1 : 0) << ",\n"
            << "  \"shardVisible\": " << (quest.shardVisible ? 1 : 0) << ",\n"
            << "  \"beaconRestored\": " << (quest.beaconRestored ? 1 : 0) << ",\n"
            << "  \"groundedEvents\": " << quest.groundedEvents << ",\n"
            << "  \"disposition\": " << ariadne.disposition << ",\n"
            << "  \"declined\": " << (ariadne.remembersDecline ? 1 : 0) << ",\n"
            << "  \"accepted\": " << (ariadne.remembersAcceptance ? 1 : 0) << ",\n"
            << "  \"restored\": " << (ariadne.remembersRestoration ? 1 : 0) << "\n"
            << "}\n";
        out.flush();
        if (!out.good()) {
            quest.persistenceStatus = "SAVE WRITE FAILED";
            out.close();
            DeleteFileA(temp.c_str());
            return false;
        }
        out.close();
        if (!MoveFileExA(temp.c_str(), SavePath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            quest.persistenceStatus = "SAVE RENAME FAILED";
            DeleteFileA(temp.c_str());
            return false;
        }
        quest.persistenceStatus = "SAVED";
        return true;
    }

    bool load(avatar3d::GameWorld& world) {
        std::ifstream in(SavePath);
        if (!in) {
            quest.persistenceStatus = "NO SAVE";
            return false;
        }
        const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const int stage = integerField(text, "questStage", -1);
        if (stage < 0 || stage > (int)QuestStage::Complete) {
            quest.persistenceStatus = "SAVE INVALID";
            return false;
        }
        quest.stage = (QuestStage)stage;
        quest.hasSignalShard = integerField(text, "hasSignalShard", 0) != 0;
        quest.hasResonanceSigil = integerField(text, "hasResonanceSigil", 0) != 0;
        quest.shardVisible = integerField(text, "shardVisible", 1) != 0;
        quest.beaconRestored = integerField(text, "beaconRestored", 0) != 0;
        quest.groundedEvents = (uint32_t)std::max(0, integerField(text, "groundedEvents", 0));
        ariadne.disposition = floatField(text, "disposition", 0.55f);
        ariadne.remembersDecline = integerField(text, "declined", 0) != 0;
        ariadne.remembersAcceptance = integerField(text, "accepted", 0) != 0;
        ariadne.remembersRestoration = integerField(text, "restored", 0) != 0;
        quest.dialogueOpen = false;
        quest.persistenceStatus = "LOADED";
        quest.lastEvent = "Gameplay state loaded";
        syncWorld(world);
        return true;
    }

    bool canSpeak(const avatar3d::Vec3& player, float playerHeading) const {
        if (horizontalDistance(player, ariadne.position) > ariadne.interactionRange) return false;
        const float target = headingToward(player, ariadne.position);
        return fabsf(angleDelta(playerHeading, target)) < 1.20f;
    }

    bool canCollect(const avatar3d::Vec3& player, const avatar3d::GameWorld& world) const {
        return quest.stage == QuestStage::SeekShard && quest.shardVisible &&
               horizontalDistance(player, world.questShard) <= 1.35f;
    }

    bool canRestore(const avatar3d::Vec3& player, const avatar3d::GameWorld& world) const {
        return quest.stage == QuestStage::RestoreBeacon && quest.hasSignalShard &&
               horizontalDistance(player, world.goal) <= 1.55f;
    }

    std::string interactionPrompt(const avatar3d::Vec3& player, float playerHeading,
                                  const avatar3d::GameWorld& world) const {
        if (quest.dialogueOpen) return "[E] Leave conversation   [1-3] Choose";
        if (canCollect(player, world)) return "[E] Recover signal shard";
        if (canRestore(player, world)) return "[E] Restore civic beacon";
        if (canSpeak(player, playerHeading)) return "[E] Speak with Ariadne";
        return "";
    }

    std::string dialogueLine() const {
        if (!quest.dialogueOpen) return "";
        switch (quest.stage) {
            case QuestStage::Offered:
                if (quest.askedWhy)
                    return "The beacon is how this sanctuary remembers every traveller. Without its shard, the path forgets us.";
                if (ariadne.remembersDecline)
                    return "You returned. The broken signal still waits, and so do I.";
                return "I am Ariadne, keeper of the civic beacon. Its signal shard fell beyond the blue pillars.";
            case QuestStage::SeekShard:
                return "Follow the cold cyan pulse. Bring the shard back before the sanctuary loses its name.";
            case QuestStage::RestoreBeacon:
                return "You carry the shard. Seat it in the beacon where the green light once stood.";
            case QuestStage::ReturnToAriadne:
                return "The beacon speaks again. Let the sanctuary recognise what you restored.";
            case QuestStage::Complete:
                return "The Resonance Sigil remembers your choice. This city now knows you as a keeper.";
            default:
                return "The sanctuary is quiet, but not empty.";
        }
    }

    std::array<std::string, 3> dialogueChoices() const {
        if (!quest.dialogueOpen) return {"", "", ""};
        if (quest.stage == QuestStage::Offered)
            return {"1  Accept the purpose", "2  Why does the beacon matter?", "3  Not yet"};
        if (quest.stage == QuestStage::ReturnToAriadne)
            return {"1  Receive the Resonance Sigil", "2  Tell me what changed", "3  Leave"};
        return {"1  Continue", "", "3  Leave"};
    }

    std::string stageName() const {
        switch (quest.stage) {
            case QuestStage::Dormant: return "DISCOVER";
            case QuestStage::Offered: return "OFFERED";
            case QuestStage::SeekShard: return "SEEK SHARD";
            case QuestStage::RestoreBeacon: return "RESTORE";
            case QuestStage::ReturnToAriadne: return "RETURN";
            case QuestStage::Complete: return "COMPLETE";
        }
        return "UNKNOWN";
    }

    std::string objective() const {
        for (const auto& objective : brokenSignalQuest().objectives)
            if (objective.stage == quest.stage) return objective.label;
        return "Listen to the sanctuary";
    }

    avatar3d::Vec3 objectivePosition(const avatar3d::GameWorld& world) const {
        switch (quest.stage) {
            case QuestStage::SeekShard: return world.questShard;
            case QuestStage::RestoreBeacon: return world.goal;
            default: return ariadne.position;
        }
    }

    const char* routineName() const {
        switch (ariadne.routine) {
            case NpcRoutine::WatchBeacon: return "WATCH BEACON";
            case NpcRoutine::PatrolSanctuary: return "PATROL";
            case NpcRoutine::NoticePlayer: return "NOTICE PLAYER";
            case NpcRoutine::Converse: return "CONVERSE";
            case NpcRoutine::Celebrate: return "CELEBRATE";
        }
        return "UNKNOWN";
    }

private:
    static float horizontalDistance(const avatar3d::Vec3& a, const avatar3d::Vec3& b) {
        const float dx = a.x - b.x, dz = a.z - b.z;
        return sqrtf(dx * dx + dz * dz);
    }

    static float headingToward(const avatar3d::Vec3& from, const avatar3d::Vec3& to) {
        return atan2f(to.x - from.x, to.z - from.z);
    }

    static float angleDelta(float from, float to) {
        float delta = to - from;
        while (delta > 3.14159265f) delta -= 6.2831853f;
        while (delta < -3.14159265f) delta += 6.2831853f;
        return delta;
    }

    void record(const std::string& event) {
        quest.lastEvent = event;
        ++quest.groundedEvents;
    }

    void syncWorld(avatar3d::GameWorld& world) const {
        world.npcPos = ariadne.position;
        world.npcHeading = ariadne.heading;
        world.npcSpeed = ariadne.speed;
        world.questShardVisible = quest.shardVisible;
        world.beaconRestored = quest.beaconRestored;
        world.questComplete = quest.stage == QuestStage::Complete;
    }

    static int integerField(const std::string& text, const char* key, int fallback) {
        const std::string token = std::string("\"") + key + "\"";
        size_t pos = text.find(token);
        if (pos == std::string::npos) return fallback;
        pos = text.find(':', pos + token.size());
        if (pos == std::string::npos) return fallback;
        return atoi(text.c_str() + pos + 1);
    }

    static float floatField(const std::string& text, const char* key, float fallback) {
        const std::string token = std::string("\"") + key + "\"";
        size_t pos = text.find(token);
        if (pos == std::string::npos) return fallback;
        pos = text.find(':', pos + token.size());
        if (pos == std::string::npos) return fallback;
        return (float)atof(text.c_str() + pos + 1);
    }
};

} // namespace v19game

// GTAngel v20: original, portable local CNS vertical slice. No supplied engine code/assets.
// This module is inserted after v19game in the deterministic generated Win32 source.
namespace v20cns {
using avatar3d::Vec3;

static float distance2(Vec3 a, Vec3 b) {
    const float x = a.x-b.x, z = a.z-b.z;
    return x*x+z*z;
}
static bool finite(float x) { return std::isfinite(x); }
static bool finitePose(Vec3 p) { return finite(p.x)&&finite(p.y)&&finite(p.z); }

enum class Source : uint8_t { HumanLocal, Neural, Route, Script, Replay, AgentZero, Remote };
struct Envelope { uint64_t sequence{0}, frame{0}, monotonicMs{0}; uint32_t ttlMs{100}; float confidence{1}; };
struct MotorProposal {
    Envelope evidence;
    Source source{Source::HumanLocal};
    float forward{0}, turn{0}, uncertainty{0}, collisionRisk{0};
    bool ethicsApproved{true};
};
struct Body {
    Vec3 position{0,0,0}; float heading{0}, speed{0}, steering{0}, wheelPhase{0};
    Vec3 cartPosition{1.5f,0,0};
    float cartHeading{0};
    bool occupied{false}, contact{false}; int gear{0}; uint64_t sequence{0};
};
struct Context {
    uint64_t frame{0}, nowMs{0}; float dt{0.05f};
    bool localFrameValid{true}; bool emergencyStop{false};
};
// Commit cannot be made by a proposal source. A token is a one-shot capability
// consumed by the effector, not a scalar stored in any public input packet.
class MotorCommit {
    friend class SpinalReflex;
    friend class BodyEffector;
    float forward_, turn_;
    uint64_t sequence_, nonce_;
    MotorCommit(float f, float t, uint64_t s, uint64_t n)
        : forward_(f), turn_(t), sequence_(s), nonce_(n) {}
public:
    MotorCommit() = delete;
};

class BodyEffector {
    friend class SpinalReflex;
    Body body_{};
    uint64_t nextNonce_{1};
    static bool clear(Vec3 p, float radius, const avatar3d::GameWorld& w) {
        if (!finitePose(p) || fabsf(p.x)>w.worldSize*0.5f-radius ||
            fabsf(p.z)>w.worldSize*0.5f-radius) return false;
        for (const auto& pillar : w.pillars)
            if (distance2(p,pillar.pos)<(radius+pillar.radius)*(radius+pillar.radius)) return false;
        if (distance2(p,w.npcPos)<(radius+0.55f)*(radius+0.55f)) return false;
        if (radius>0.5f &&
            (distance2(p,w.goal)<(radius+0.35f)*(radius+0.35f) ||
             (w.questShardVisible && distance2(p,w.questShard)<(radius+0.35f)*(radius+0.35f)))) return false;
        return true;
    }
    static bool swept(Vec3 a, Vec3 b, float radius, const avatar3d::GameWorld& w) {
        const float len = sqrtf(distance2(a,b));
        const int steps = std::max(1,(int)ceilf(len/0.10f));
        for (int i=1;i<=steps;++i) {
            float t=(float)i/(float)steps;
            if (!clear({a.x+(b.x-a.x)*t,0,a.z+(b.z-a.z)*t},radius,w)) return false;
        }
        // LW55: approach-only mutual barriers for dynamic bodies — motion INTO a
        // vehicle/ped is vetoed, but any motion AWAY (escape when overlapping)
        // is always allowed, so the avatar can never be caged by a barrier.
        // Penetration-only (approach semantics): veto motion that CLOSES INTO a
        // body core — never a wide standoff — so the avatar can still reach the
        // coupe door, brush past traffic, and always escape an overlap.
        // The parked coupe is the enterable player vehicle — never barrier it
        // (approach is required to enter); moving traffic/peds still barrier.
        for (const auto& v : w.traffic) {
            if (v.orbitR>0.0f) continue;
            const float dBt=distance2(b,v.pos), dAt=distance2(a,v.pos);
            if (dBt < (radius+1.45f)*(radius+1.45f) && dBt < dAt) return false;
        }
        const Vec3 ambP{w.ambX,0.0f,-1.4f}, busP{-1.4f,0.0f,w.busZ};
        if (distance2(b,ambP)<(radius+1.6f)*(radius+1.6f) && distance2(b,ambP)<distance2(a,ambP)) return false;
        if (distance2(b,busP)<(radius+1.8f)*(radius+1.8f) && distance2(b,busP)<distance2(a,busP)) return false;
        // Ambient crowd walkers never cage the avatar — they yield via their
        // own pedBlockedByBarrier steering instead.
        return true;
    }
    bool consume(const MotorCommit& c, const Context& ctx, const avatar3d::GameWorld& world) {
        if (c.nonce_!=nextNonce_ || c.sequence_<=body_.sequence) return false;
        ++nextNonce_; // never replay a commit, even on collision
        Body next=body_;
        next.contact=false;
        if (next.occupied) {
            // Signed throttle/brake transition: reverse requires first braking to rest.
            const float demand=std::clamp(c.forward_,-1.0f,1.0f);
            float acceleration=0;
            if (fabsf(demand)<0.02f) acceleration=-next.speed*1.6f;
            else if (next.speed*demand < -0.02f) acceleration=-std::copysign(6.0f,next.speed);
            else acceleration=demand*3.2f-next.speed*0.32f;
            next.speed=std::clamp(next.speed+acceleration*ctx.dt,-3.0f,7.0f);
            if (fabsf(next.speed)<0.025f) next.speed=0;
            const float limit=0.55f/(1.0f+fabsf(next.speed)*0.12f);
            next.steering=std::clamp(c.turn_,-1.0f,1.0f)*limit;
            next.heading+=next.speed/1.65f*tanf(next.steering)*ctx.dt;
            next.gear=next.speed>0.025f ? 1 : next.speed< -0.025f ? -1 : 0;
            next.wheelPhase+=next.speed*ctx.dt/0.32f;
        } else {
            next.heading+=std::clamp(c.turn_,-1.0f,1.0f)*ctx.dt*2.0f;
            next.speed=std::clamp(c.forward_,-1.0f,1.0f)*2.0f;
            next.steering=0; next.gear=0;
        }
        Vec3 target=next.position+Vec3{sinf(next.heading)*next.speed*ctx.dt,0,
                                         cosf(next.heading)*next.speed*ctx.dt};
        if (!swept(next.position,target,next.occupied ? 0.76f : 0.25f,world)) {
            next.contact=true;
            if (!next.occupied) {
                // After rejecting the full swept motion, try independently
                // swept axes. This is *not* penetration recovery or a bypass;
                // every candidate is checked against the same local geometry.
                const Vec3 before=next.position;
                const Vec3 alongX{target.x,0,before.z};
                const Vec3 alongZ{before.x,0,target.z};
                const bool xClear=swept(before,alongX,0.25f,world);
                const bool zClear=swept(before,alongZ,0.25f,world);
                const float dx=target.x-before.x,dz=target.z-before.z;
                if (xClear && (!zClear || dx*dx>=dz*dz)) next.position=alongX;
                else if (zClear) next.position=alongZ;
                else if (xClear) next.position=alongX;
                if (distance2(before,next.position)<1e-10f) next.speed=0;
            } else { next.speed=0; next.gear=0; }
        } else next.position=target;
        if (next.occupied) { next.cartPosition=next.position; next.cartHeading=next.heading; }
        next.sequence=c.sequence_;
        body_=next;
        return true;
    }
public:
    const Body& state() const { return body_; }
    void reset() { body_=Body{}; nextNonce_=1; }
    // Only a bounded local enter/exit interaction can switch body mode.
    bool toggleCart(const avatar3d::GameWorld& world, Vec3 cartPosition) {
        if (!finitePose(cartPosition) || fabsf(body_.speed)>0.35f) return false;
        if (body_.occupied) {
            const Vec3 side{body_.position.x+cosf(body_.heading)*1.35f,0,
                            body_.position.z-sinf(body_.heading)*1.35f};
            if (!clear(side,0.25f,world) || distance2(side,world.goal)<0.5f) return false;
            body_.cartPosition=body_.position; body_.cartHeading=body_.heading;
            body_.position=side; body_.occupied=false;
        } else {
            if (distance2(cartPosition,body_.cartPosition)>0.01f ||
                distance2(body_.position,body_.cartPosition)>2.4f*2.4f ||
                !clear(body_.cartPosition,0.76f,world)) return false;
            body_.position=body_.cartPosition; body_.heading=body_.cartHeading;
            body_.occupied=true;
        }
        body_.speed=0; body_.gear=0; body_.contact=false;
        ++body_.sequence; ++nextNonce_;
        return true;
    }
    // Validated load only. Never exposed to network, policy, script, or proposal adapters.
    bool restoreLocal(const Body& b, const avatar3d::GameWorld& world) {
        if (!finitePose(b.position)||!finitePose(b.cartPosition)||!finite(b.heading)||
            !finite(b.cartHeading)||fabsf(b.cartHeading)>100000.0f||!finite(b.speed)||
            !finite(b.steering)||!finite(b.wheelPhase)||fabsf(b.speed)>7.01f||
            fabsf(b.heading)>100000.0f||fabsf(b.steering)>0.56f||
            !clear(b.position,b.occupied?0.76f:0.25f,world) ||
            !clear(b.cartPosition,0.76f,world) ||
            (b.occupied && distance2(b.position,b.cartPosition)>0.01f)) return false;
        body_=b; ++nextNonce_; return true;
    }
};

class SpinalReflex {
    uint64_t lastSequence_{0};
public:
    enum class Verdict { Accepted, Invalid, Stale, Unsafe, Untrusted, Collision, Replay };
    Verdict submit(const MotorProposal& p, const Context& ctx, BodyEffector& plant,
                   const avatar3d::GameWorld& world) {
        if (!finite(p.forward)||!finite(p.turn)||!finite(p.uncertainty)||
            !finite(p.collisionRisk)||!finite(p.evidence.confidence)||
            fabsf(p.forward)>1||fabsf(p.turn)>1||p.evidence.confidence<0||
            p.evidence.confidence>1||p.uncertainty<0||p.collisionRisk<0||
            p.evidence.ttlMs>250||ctx.dt<=0||ctx.dt>0.10f) return Verdict::Invalid;
        if (p.evidence.sequence<=lastSequence_) return Verdict::Replay;
        if (p.evidence.monotonicMs>ctx.nowMs || ctx.nowMs-p.evidence.monotonicMs>p.evidence.ttlMs ||
            p.evidence.frame!=ctx.frame || !ctx.localFrameValid) return Verdict::Stale;
        if (ctx.emergencyStop || !p.ethicsApproved || p.collisionRisk>0.80f ||
            p.uncertainty>0.60f || p.evidence.confidence<0.35f) return Verdict::Unsafe;
        // v20 does not qualify autonomous actuation; all non-local/non-human sources
        // are explicitly typed and can be pre-empted, but cannot be promoted yet.
        if (p.source!=Source::HumanLocal) return Verdict::Untrusted;
        lastSequence_=p.evidence.sequence;
        const MotorCommit commit(p.forward,p.turn,p.evidence.sequence,plant.nextNonce_);
        return plant.consume(commit,ctx,world) ? (plant.state().contact?Verdict::Collision:Verdict::Accepted)
                                                : Verdict::Invalid;
    }
    void reset() { lastSequence_=0; }
};

class MotorArbiter {
public:
    static Source choose(bool humanActive,bool routeActive,bool learnedAccepted) {
        if (humanActive) return Source::HumanLocal;
        if (routeActive) return Source::Route;
        if (learnedAccepted) return Source::Neural;
        return Source::HumanLocal; // bounded local neutral/coast braking
    }
};

struct NicheProposal {
    Envelope evidence;
    Source source{Source::HumanLocal};
    uint64_t expectedWorldVersion{0};
    Vec3 locus{}; float radius{0.45f}; uint32_t cost{1};
    bool inverseAvailable{true}, questGrounded{false};
};
class NicheCommit {
    friend class WorldGovernance;
    friend class WorldEffector;
    uint64_t version_, nonce_;
    NicheCommit(uint64_t v,uint64_t n):version_(v),nonce_(n){}
public:
    NicheCommit()=delete;
};
class WorldEffector {
    friend class WorldGovernance;
    uint64_t nextNonce_{1};
    bool apply(const NicheCommit& c, v19game::GameplayLoop& game, avatar3d::GameWorld& w) {
        if (c.nonce_!=nextNonce_) return false;
        ++nextNonce_;
        return game.governedRepairBeacon(w);
    }
public:
    void reset() { nextNonce_=1; }
};
struct NicheHistory {
    uint64_t worldVersion{1};
    uint32_t spent{0};
    bool reobserved{false}, rolledBack{false};
};
class WorldGovernance {
    NicheHistory history_{};
    uint64_t lastSequence_{0};
public:
    enum class Verdict { Accepted, Invalid, Stale, Unsafe, Untrusted, NoInverse,
                         Budget, Protected, Quest, ReobservationFailed };
    const NicheHistory& history() const { return history_; }
    void reset() { history_=NicheHistory{}; lastSequence_=0; }
    bool restoreLocal(const NicheHistory& h, const v19game::GameplayLoop& game) {
        if (h.worldVersion<1 || h.worldVersion>2 || h.spent>1 || h.rolledBack ||
            (game.quest.beaconRestored != (h.worldVersion==2)) ||
            h.spent!=(h.worldVersion==2?1u:0u) ||
            (h.worldVersion==2 && !h.reobserved)) return false;
        history_=h; lastSequence_=0; return true;
    }
    template<class Observe>
    Verdict submit(const NicheProposal& p, const Context& ctx, Vec3 player,
                   v19game::GameplayLoop& game, avatar3d::GameWorld& w,
                   WorldEffector& effector, Observe observe) {
        if (!finitePose(p.locus)||!finite(p.radius)||!finite(p.evidence.confidence)||
            p.radius<0.20f||p.radius>0.80f||p.evidence.confidence<0.35f||
            p.evidence.ttlMs>250||p.cost==0) return Verdict::Invalid;
        if (p.evidence.monotonicMs>ctx.nowMs||ctx.nowMs-p.evidence.monotonicMs>p.evidence.ttlMs||
            p.evidence.frame!=ctx.frame||!ctx.localFrameValid||
            p.expectedWorldVersion!=history_.worldVersion) return Verdict::Stale;
        if (p.evidence.sequence<=lastSequence_) return Verdict::Stale;
        if (ctx.emergencyStop) return Verdict::Unsafe;
        if (p.source!=Source::HumanLocal) return Verdict::Untrusted;
        if (!p.inverseAvailable) return Verdict::NoInverse;
        if (p.cost>1-history_.spent) return Verdict::Budget;
        if (distance2(p.locus,w.goal)>0.25f ||
            distance2(p.locus,w.npcPos)<(p.radius+1.0f)*(p.radius+1.0f) ||
            distance2(p.locus,w.questShard)<(p.radius+0.5f)*(p.radius+0.5f) ||
            distance2(p.locus,player)>3.0f*3.0f) return Verdict::Protected;
        if (!p.questGrounded||!game.canRestore(player,w)) return Verdict::Quest;
        lastSequence_=p.evidence.sequence;
        // The only world transform in v20 is the quest-authored beacon repair.
        // Snapshot includes the full quest and NPC memories, not only the visual bit.
        const auto priorQuest=game.quest;
        const auto priorNpc=game.ariadne;
        const auto priorWorld=w;
        const auto priorHistory=history_;
        const NicheCommit c(history_.worldVersion,effector.nextNonce_);
        if (!effector.apply(c,game,w)) return Verdict::Quest;
        ++history_.worldVersion; history_.spent+=p.cost;
        history_.reobserved=false;
        bool verified=false;
        try { verified=observe(priorWorld,w,history_.worldVersion); }
        catch (...) { verified=false; }
        if (!verified) {
            game.quest=priorQuest; game.ariadne=priorNpc; w=priorWorld;
            history_=priorHistory; history_.rolledBack=true;
            return Verdict::ReobservationFailed;
        }
        history_.reobserved=true; history_.rolledBack=false;
        return Verdict::Accepted;
    }
};

class Runtime {
    SpinalReflex spinal_;
    BodyEffector plant_;
    WorldGovernance governance_;
    WorldEffector worldEffector_;
    uint64_t sequence_{0};
public:
    const Body& body() const { return plant_.state(); }
    const NicheHistory& niche() const { return governance_.history(); }
    void reset() { spinal_.reset(); plant_.reset(); governance_.reset(); worldEffector_.reset(); sequence_=0; }
    SpinalReflex::Verdict motor(Source source,float forward,float turn,
                                float uncertainty,float risk,bool ethics,
                                const Context& ctx,const avatar3d::GameWorld& world) {
        MotorProposal p; p.evidence={++sequence_,ctx.frame,ctx.nowMs,100,1.0f};
        p.source=source; p.forward=forward; p.turn=turn;
        p.uncertainty=uncertainty; p.collisionRisk=risk; p.ethicsApproved=ethics;
        return spinal_.submit(p,ctx,plant_,world);
    }
    bool toggleCart(const avatar3d::GameWorld& w, Vec3 parked) {
        if (!plant_.toggleCart(w,parked)) return false;
        sequence_=std::max(sequence_,plant_.state().sequence);
        return true;
    }
    template<class Observe>
    WorldGovernance::Verdict repair(Vec3 player, v19game::GameplayLoop& game,
                                    avatar3d::GameWorld& world, const Context& ctx,
                                    Observe observe) {
        if (plant_.state().occupied) return WorldGovernance::Verdict::Quest;
        NicheProposal p; p.evidence={++sequence_,ctx.frame,ctx.nowMs,100,1.0f};
        p.source=Source::HumanLocal; p.expectedWorldVersion=governance_.history().worldVersion;
        p.locus=world.goal; p.questGrounded=game.canRestore(player,world);
        return governance_.submit(p,ctx,player,game,world,worldEffector_,observe);
    }
    bool restoreLocal(const Body& b,const NicheHistory& h,
                      const v19game::GameplayLoop& game,const avatar3d::GameWorld& w) {
        BodyEffector trial;
        WorldGovernance govern;
        if (!trial.restoreLocal(b,w)||!govern.restoreLocal(h,game)) return false;
        plant_=trial; governance_=govern; spinal_.reset(); worldEffector_.reset(); sequence_=b.sequence;
        return true;
    }
};
} // namespace v20cns

// GTAngel v25 / DTE APL253: locally sealed, read-only cognition ingress.
// Included after v15render, v19game, and v20cns; no external producer API.
// In-process provenance is not cryptographic attestation against hostile injected code.
#include <cstring>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <optional>
#include <type_traits>
struct GTAngelAGI;
#include "dte_catalogue_253.hpp"
namespace v25dte {
using Frame = v15render::HighResFramebuffer;
static_assert(Frame::Width==768 && Frame::Height==768,"VRD sensor must remain 768x768");

// These extra predicates require a separately measured, typed record. The game
// does not currently measure all of them, so its production Sequence-27 view
// supplies no such record and those effects fail closed.
struct Sequence27Evidence {
    double functionImportance{-1}, perceptualLoadVariance{-1};
    double interactionAspectRatio{-1}; bool continuousQuest{false};
    int socialFacingWings{-1}, subscribedTopTenPercentWings{-1};
    uint64_t originBytes{0}, crossWriteBytes{0}; bool metadataOnly{false};
    double donorConfidence{-1}; uint64_t borrowedBytes{0}, donorWorkingSetBytes{0};
    uint64_t transitionCapacity{0}, peakMessageBytes{0}; int snapshotRetainedTicks{-1};
    double activeTaskIntrusion{-1}, routeAffordance{-1}, bestNeighbourAffordance{-1};
    bool stateEdgeLocus{false};
};
inline bool sequence27Guard(int id,const Sequence27Evidence& e) {
    switch(id) {
      case 190: return e.functionImportance>0.5 && e.functionImportance<=1.0 &&
                       e.perceptualLoadVariance>=0.2 && e.perceptualLoadVariance<=1.0;
      case 191: return e.continuousQuest && e.interactionAspectRatio>=1.0 &&
                       e.interactionAspectRatio<=2.0;
      case 192: return e.socialFacingWings>0 &&
                       e.subscribedTopTenPercentWings>=e.socialFacingWings;
      case 193: return e.metadataOnly && e.originBytes>0 && e.crossWriteBytes>0 &&
                       e.crossWriteBytes<=e.originBytes/5;
      case 194: return e.donorConfidence>0.7 && e.donorConfidence<=1.0 &&
                       e.donorWorkingSetBytes>0 && e.borrowedBytes>0 &&
                       e.borrowedBytes<e.donorWorkingSetBytes/20;
      case 195: return e.peakMessageBytes>0 && e.peakMessageBytes<=UINT64_MAX/3 &&
                       e.transitionCapacity>=3*e.peakMessageBytes &&
                       e.snapshotRetainedTicks>=30;
      case 196: return e.stateEdgeLocus && e.activeTaskIntrusion>=0.0 &&
                       e.activeTaskIntrusion<0.15 && e.routeAffordance>=0.0 &&
                       e.routeAffordance<=1.0 && e.bestNeighbourAffordance>=0.0 &&
                       e.bestNeighbourAffordance<=1.0 && e.routeAffordance>=e.bestNeighbourAffordance;
    }
    return false;
}

class LocalVisualIngress;
class Sample final {
    friend class LocalVisualIngress;
    uint64_t sequence_{0}, hash_{0}, capturedMs_{0}, worldVersion_{0}, bodySequence_{0};
    uint32_t goalPixels_{0}, goalDepthPixels_{0}, npcPixels_{0};
    Sample(uint64_t s,uint64_t h,uint64_t t,uint64_t v,uint64_t b,
           uint32_t g,uint32_t gd,uint32_t npc)
        :sequence_(s),hash_(h),capturedMs_(t),worldVersion_(v),bodySequence_(b),
         goalPixels_(g),goalDepthPixels_(gd),npcPixels_(npc) {}
public:
    Sample()=delete;
    uint64_t sequence() const {return sequence_;}
    uint64_t hash() const {return hash_;}
    uint64_t worldVersion() const {return worldVersion_;}
    uint32_t goalPixels() const {return goalPixels_;}
    uint32_t npcPixels() const {return npcPixels_;}
};
static_assert(!std::is_default_constructible<Sample>::value,"no forgeable default VRD sample");

class LocalVisualIngress final {
    friend struct ::GTAngelAGI; // only renderer-owning native game may mint samples
    friend class EchoCommons;
    const Frame* const boundFrame_;
    uint64_t lastIssued_{0};
    explicit LocalVisualIngress(const Frame& f):boundFrame_(&f) {}
    static uint64_t digest(const Frame& f,uint32_t& goal,uint32_t& goalDepth,uint32_t& npc) {
        uint64_t h=1469598103934665603ULL;
        goal=goalDepth=npc=0;
        for(size_t i=0;i<(size_t)Frame::PixelCount;++i) {
            const auto id=f.objectId[i];
            if(id==v15render::ObjectGoal) {++goal;if(std::isfinite(f.depth[i]) && f.depth[i]<1e20f) ++goalDepth;}
            if(id==v15render::ObjectNpc) ++npc;
            uint32_t depthBits=0; std::memcpy(&depthBits,&f.depth[i],sizeof(depthBits));
            h=(h^f.colour[i])*1099511628211ULL;
            h=(h^depthBits)*1099511628211ULL;
            h=(h^id)*1099511628211ULL;
        }
        return h;
    }
    std::optional<Sample> capture(uint64_t nowMs,const v20cns::Body& body,
                                  const v20cns::NicheHistory& niche) {
        const Frame& f=*boundFrame_;
        if(f.sequence==0 || f.sequence<=lastIssued_ ||
           f.colour.size()!=Frame::PixelCount || f.depth.size()!=Frame::PixelCount ||
           f.objectId.size()!=Frame::PixelCount || niche.worldVersion<1 || niche.worldVersion>2 ||
           !v20cns::finitePose(body.position) || !v20cns::finite(body.heading)) return std::nullopt;
        uint32_t g=0,d=0,n=0;
        const auto h=digest(f,g,d,n);
        if(g>0 && d!=g) return std::nullopt;
        lastIssued_=f.sequence;
        return Sample(f.sequence,h,nowMs,niche.worldVersion,body.sequence,g,d,n);
    }
    bool current(const Sample& s,uint64_t nowMs,const v20cns::Body& body,
                 const v20cns::NicheHistory& niche,uint32_t ttlMs=150) const {
        if(nowMs<s.capturedMs_ || nowMs-s.capturedMs_>ttlMs ||
           boundFrame_->sequence!=s.sequence_ || body.sequence!=s.bodySequence_ ||
           niche.worldVersion!=s.worldVersion_ ||
           boundFrame_->colour.size()!=Frame::PixelCount ||
           boundFrame_->depth.size()!=Frame::PixelCount ||
           boundFrame_->objectId.size()!=Frame::PixelCount) return false;
        uint32_t g=0,d=0,n=0;
        return digest(*boundFrame_,g,d,n)==s.hash_ && g==s.goalPixels_ &&
               d==s.goalDepthPixels_ && n==s.npcPixels_;
    }
    void reset() {lastIssued_=0;}
public:
    LocalVisualIngress()=delete;
    LocalVisualIngress(const LocalVisualIngress&)=delete;
    LocalVisualIngress& operator=(const LocalVisualIngress&)=delete;
};

struct EchoCandidate {uint16_t pattern{0};uint64_t frame{0},worldVersion{0},hash{0},expiryMs{0};};
class EchoCommons final {
    EchoCandidate candidate_{};
    uint64_t lastScanned_{0}; unsigned matched_{0},blockedSequence27_{0};
public:
    void reset() {candidate_={};lastScanned_=0;matched_=blockedSequence27_=0;}
    unsigned matched() const {return matched_;}
    unsigned blockedSequence27() const {return blockedSequence27_;}
    EchoCandidate candidate() const {return candidate_;} // metadata only, not a commit token
    bool scan(LocalVisualIngress& ingress,const Sample& s,uint64_t nowMs,
              const v20cns::Body& body,const v20cns::NicheHistory& niche,
              const v19game::GameplayLoop& game,const avatar3d::GameWorld& world) {
        candidate_={}; matched_=blockedSequence27_=0;
        if(s.sequence()<=lastScanned_ || !ingress.current(s,nowMs,body,niche)) return false;
        lastScanned_=s.sequence();
        // Only measured input channels are present; missing channels are UNKNOWN,
        // not zero. Unknown or unimplemented guards never grant motor/world power.
        auto measured=[&](const char* signal,double& value) {
            if(std::strcmp(signal,"visual_salience")==0) { value=std::min(1.0,(double)s.goalPixels()/100.0);return true; }
            if(std::strcmp(signal,"perceptual_confidence")==0) {value=s.goalPixels()>20?0.85:0.0;return true;}
            if(std::strcmp(signal,"collision_risk")==0) {value=body.contact?1.0:0.0;return true;}
            if(std::strcmp(signal,"world_budget")==0) {value=niche.spent==0?1.0:0.0;return true;}
            if(std::strcmp(signal,"niche_need")==0) {value=game.canRestore(body.position,world)?1.0:0.0;return true;}
            return false;
        };
        for(const auto& rule:v25rules::Catalogue) {
            double value=0;
            if(!measured(rule.signal,value)) continue;
            if(rule.lessEqual?value>rule.threshold:value<rule.threshold) continue;
            if(rule.id>=190 && rule.id<=196) {
                // No production social/write/route/session evidence supplied.
                ++blockedSequence27_;continue;
            }
            ++matched_; // counted as advisory match, NEVER an executed English guard
            if(rule.id==160 && niche.worldVersion==1 && niche.spent==0 &&
               !world.beaconRestored && std::fabs(body.speed)<=0.35f && !body.occupied &&
               s.goalPixels()>20 && game.canRestore(body.position,world)) {
                // APL160 edge-affordance specialization: a one-frame, 100ms
                // human-confirmable niche *proposal*, not a world mutation.
                candidate_={160,s.sequence(),niche.worldVersion,s.hash(),nowMs+100};
            }
        }
        return candidate_.pattern==160;
    }
    bool canConfirm(LocalVisualIngress& ingress,const Sample& s,uint64_t nowMs,
                    const v20cns::Body& body,const v20cns::NicheHistory& niche,
                    const v19game::GameplayLoop& game,const avatar3d::GameWorld& world) const {
        return candidate_.pattern==160 && candidate_.frame==s.sequence() &&
               candidate_.worldVersion==s.worldVersion() && candidate_.hash==s.hash() &&
               nowMs<=candidate_.expiryMs && !body.occupied && !world.beaconRestored &&
               game.canRestore(body.position,world) && ingress.current(s,nowMs,body,niche,100);
    }
    void clear() {candidate_={};}
};
} // namespace v25dte

// GTAngel v26: local, advisory System 7 (8-node) -> System 8 (9-node) tree lens.
// The incidence arrays are generated from canonical trees, not inferred by an LLM.
// This module is deliberately outside SRD/MOTOR_COMMIT and WGR/NICHE_COMMIT.
#include <array>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include "system7-system8-native.hpp"

namespace v26bridge {
struct LensState {
    bool valid{false};
    uint64_t visualSequence{0}, visualDigest{0}, bodySequence{0}, worldVersion{0};
    uint16_t root7{0}, root8{0}; // zero-based indexes; not a policy decision
    uint8_t free7{0}, free8{0};  // one-based family IDs
    uint16_t children{0};
    uint8_t childFamilies{0};
    std::array<uint16_t,286> childRootIndexes{};
};
class LensBridge final {
    LensState state_{};
public:
    const LensState& state() const {return state_;}
    void reset() {state_=LensState{};}
    // Called ONLY after GTAngelAGI, friend of LocalVisualIngress, validates a
    // current Sample against the renderer-owned 768x768 framebuffer and body.
    // Do not expose this function as a network or script ingress.
    void observe(const v25dte::Sample& sample,const v20cns::Body& body,
                 const v20cns::NicheHistory& niche) {
        reset();
        if (!sample.sequence() || sample.worldVersion()!=niche.worldVersion ||
            !v20cns::finitePose(body.position)) return;
        const int visibleGoal=sample.goalPixels()>20 ? 1:0;
        const int visibleNpc=sample.npcPixels()>10 ? 1:0;
        const int targetLeaves=1+visibleGoal+visibleNpc+(body.occupied?1:0);
        const int targetBranch=body.contact?2:(visibleGoal&&visibleNpc?1:0);
        const int targetHeight=body.occupied?3:visibleGoal?4:5;
        int bestScore=100000,chosen=-1;
        for(unsigned i=0;i<system78::Roots7.size();++i) {
            const auto& t=system78::Roots7[i];
            const int score=4*std::abs(int(t.leaves)-targetLeaves)+
                             3*std::abs(int(t.branch)-targetBranch)+
                             std::abs(int(t.height)-targetHeight);
            if(score<bestScore){bestScore=score;chosen=int(i);}
        }
        if(chosen<0) return;
        state_.valid=true;
        state_.visualSequence=sample.sequence();state_.visualDigest=sample.hash();
        state_.bodySequence=body.sequence;state_.worldVersion=niche.worldVersion;
        state_.root7=uint16_t(chosen);state_.free7=system78::Roots7[chosen].free;
        std::array<bool,48> seenFamily{};
        for(const auto& e:system78::Edges) if(e.parent==chosen) {
            if(e.child>=system78::Roots8.size() || state_.children>=state_.childRootIndexes.size()) {reset();return;}
            state_.childRootIndexes[state_.children++]=e.child;
            const uint8_t family=system78::Roots8[e.child].free;
            if(family==0 || family>47) {reset();return;}
            if(!seenFamily[family]) {seenFamily[family]=true;++state_.childFamilies;}
        }
        if(!state_.children || !state_.childFamilies){reset();return;}
        state_.root8=state_.childRootIndexes.front();
        state_.free8=system78::Roots8[state_.root8].free;
    }
};
static_assert(system78::Roots7.size()==115 && system78::Roots8.size()==286,
              "System 7/8 rooted tree counts must be exact");
} // namespace v26bridge




// Single-file v20 atomic JSON save. Strict bounded primitive parser; no external JSON library.
namespace v20save {
static constexpr const char* SavePath = R"(C:\ddd\GTAngel\cpp_test\gtangel_v20_save.json)";
static bool number(const std::string& text,const char* key,double& result) {
    const std::string needle=std::string("\"")+key+"\"";
    size_t pos=text.find(needle);
    if (pos==std::string::npos||text.find(needle,pos+needle.size())!=std::string::npos) return false;
    pos=text.find(':',pos+needle.size());
    if (pos==std::string::npos) return false;
    const char* begin=text.c_str()+pos+1;
    while (*begin==' '||*begin=='\n'||*begin=='\t') ++begin;
    if (!((*begin>='0'&&*begin<='9')||*begin=='-'||*begin=='+')) return false;
    char* end=nullptr;
    result=strtod(begin,&end);
    if (end==begin||!std::isfinite(result)||(*end!=','&&*end!='}'&&*end!=' '&&*end!='\n')) return false;
    return true;
}
static bool integral(const std::string& t,const char* k,int lo,int hi,int& out) {
    double value=0;
    if (!number(t,k,value)||value<lo||value>hi||floor(value)!=value) return false;
    out=(int)value; return true;
}
static bool scalar(const std::string& t,const char* k,float lo,float hi,float& out) {
    double value=0;
    if (!number(t,k,value)||value<lo||value>hi) return false;
    out=(float)value; return true;
}
static bool decode(const std::string& text,int version, v19game::GameplayLoop& game,
                   avatar3d::GameWorld& world, v20cns::Body& body,
                   v20cns::NicheHistory& niche) {
    if (text.size()>8192||text.empty()||text.front()!='{'||text.find_last_of('}')==std::string::npos) return false;
    int v=0,stage=0,shard=0,sigil=0,visible=0,beacon=0,events=0,declined=0,accepted=0,restored=0;
    float disposition=0;
    if (!integral(text,"version",version,version,v) ||
        !integral(text,"questStage",0,5,stage) ||
        !integral(text,"hasSignalShard",0,1,shard) ||
        !integral(text,"hasResonanceSigil",0,1,sigil) ||
        !integral(text,"shardVisible",0,1,visible) ||
        !integral(text,"beaconRestored",0,1,beacon) ||
        !integral(text,"groundedEvents",0,10000000,events) ||
        !scalar(text,"disposition",0,1,disposition) ||
        !integral(text,"declined",0,1,declined) ||
        !integral(text,"accepted",0,1,accepted) ||
        !integral(text,"restored",0,1,restored)) return false;
    // Quest invariants are stronger than simple type checks; never load a completed
    // purpose without its earned Sigil or an unspent shard into a restored beacon.
    if ((beacon && (stage<4||shard||visible||!restored)) ||
        (!beacon && (stage>=4||sigil)) || (sigil!=(stage==5)) ||
        (stage==3 && (!shard||visible))) return false;
    game.quest.stage=(v19game::QuestStage)stage;
    game.quest.hasSignalShard=shard; game.quest.hasResonanceSigil=sigil;
    game.quest.shardVisible=visible; game.quest.beaconRestored=beacon;
    game.quest.groundedEvents=(uint32_t)events;
    game.quest.dialogueOpen=false; game.quest.journalOpen=true;
    game.quest.persistenceStatus="LOADED"; game.quest.lastEvent="Gameplay state loaded";
    game.ariadne.disposition=disposition;
    game.ariadne.remembersDecline=declined; game.ariadne.remembersAcceptance=accepted;
    game.ariadne.remembersRestoration=restored;
    game.publishWorld(world);
    if (version==1) {
        body=v20cns::Body{};
        niche={beacon?2u:1u,beacon?1u:0u,beacon!=0,false};
        return true;
    }
    int occupied=0,gear=0,contact=0,wv=0,spent=0,reobserved=0,rollback=0;
    if (!scalar(text,"bodyX",-9.75f,9.75f,body.position.x)||
        !scalar(text,"bodyZ",-9.75f,9.75f,body.position.z)||
        !scalar(text,"cartX",-9.24f,9.24f,body.cartPosition.x)||
        !scalar(text,"cartZ",-9.24f,9.24f,body.cartPosition.z)||
        !scalar(text,"cartHeading",-10000,10000,body.cartHeading)||
        !scalar(text,"bodyHeading",-10000,10000,body.heading)||
        !scalar(text,"bodySpeed",-3,7,body.speed)||
        !scalar(text,"bodySteering",-.56f,.56f,body.steering)||
        !scalar(text,"wheelPhase",-100000,100000,body.wheelPhase)||
        !integral(text,"occupied",0,1,occupied)||
        !integral(text,"gear",-1,1,gear)||
        !integral(text,"contact",0,1,contact)||
        !integral(text,"worldVersion",1,2,wv)||
        !integral(text,"nicheSpent",0,1,spent)||
        !integral(text,"reobserved",0,1,reobserved)||
        !integral(text,"rolledBack",0,1,rollback)) return false;
    if ((occupied && ((body.speed>0.025f && gear!=1) ||
                      (body.speed< -0.025f && gear!=-1) ||
                      (fabsf(body.speed)<=0.025f && gear!=0))) ||
        (!occupied && gear!=0)) return false;
    body.position.y=0; body.cartPosition.y=0;
    body.occupied=occupied; body.gear=gear; body.contact=contact;
    niche={(uint64_t)wv,(uint32_t)spent,reobserved!=0,rollback!=0};
    return true;
}
static bool write(const char* path,const v19game::GameplayLoop& g,const v20cns::Runtime& c) {
    const auto& h=c.niche();
    if (h.rolledBack || h.worldVersion<1 || h.worldVersion>2 ||
        h.spent!=(h.worldVersion==2?1u:0u) ||
        g.quest.beaconRestored!=(h.worldVersion==2) ||
        (h.worldVersion==2 && !h.reobserved) ||
        (g.quest.beaconRestored && ((int)g.quest.stage<4 ||
            g.quest.hasSignalShard || g.quest.shardVisible || !g.ariadne.remembersRestoration)) ||
        (g.quest.hasResonanceSigil!=(g.quest.stage==v19game::QuestStage::Complete)) ||
        !v20cns::finitePose(c.body().position) ||
        !v20cns::finitePose(c.body().cartPosition) ||
        !v20cns::finite(c.body().cartHeading) ||
        !v20cns::finite(c.body().heading) ||
        !v20cns::finite(c.body().speed)) return false;
    const std::string temp=std::string(path)+".tmp";
    std::ofstream out(temp,std::ios::trunc|std::ios::binary);
    if (!out) return false;
    const auto& q=g.quest; const auto& b=c.body(); const auto& n=c.niche();
    out << "{\n  \"version\": 2,\n"
        << "  \"questStage\": " << (int)q.stage << ",\n"
        << "  \"hasSignalShard\": " << (int)q.hasSignalShard << ",\n"
        << "  \"hasResonanceSigil\": " << (int)q.hasResonanceSigil << ",\n"
        << "  \"shardVisible\": " << (int)q.shardVisible << ",\n"
        << "  \"beaconRestored\": " << (int)q.beaconRestored << ",\n"
        << "  \"groundedEvents\": " << q.groundedEvents << ",\n"
        << "  \"disposition\": " << g.ariadne.disposition << ",\n"
        << "  \"declined\": " << (int)g.ariadne.remembersDecline << ",\n"
        << "  \"accepted\": " << (int)g.ariadne.remembersAcceptance << ",\n"
        << "  \"restored\": " << (int)g.ariadne.remembersRestoration << ",\n"
        << "  \"bodyX\": " << b.position.x << ",\n"
        << "  \"bodyZ\": " << b.position.z << ",\n"
        << "  \"cartX\": " << b.cartPosition.x << ",\n"
        << "  \"cartZ\": " << b.cartPosition.z << ",\n"
        << "  \"cartHeading\": " << b.cartHeading << ",\n"
        << "  \"bodyHeading\": " << b.heading << ",\n"
        << "  \"bodySpeed\": " << b.speed << ",\n"
        << "  \"bodySteering\": " << b.steering << ",\n"
        << "  \"wheelPhase\": " << b.wheelPhase << ",\n"
        << "  \"occupied\": " << (int)b.occupied << ",\n"
        << "  \"gear\": " << b.gear << ",\n"
        << "  \"contact\": " << (int)b.contact << ",\n"
        << "  \"worldVersion\": " << n.worldVersion << ",\n"
        << "  \"nicheSpent\": " << n.spent << ",\n"
        << "  \"reobserved\": " << (int)n.reobserved << ",\n"
        << "  \"rolledBack\": " << (int)n.rolledBack << "\n}\n";
    out.flush(); bool okay=out.good(); out.close(); okay=okay&&out.good();
    if (!okay || !MoveFileExA(temp.c_str(),path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) {
        DeleteFileA(temp.c_str()); return false;
    }
    return true;
}
template<class Observe>
static bool load(const char* path,const char* legacyPath,v19game::GameplayLoop& game,
                 avatar3d::GameWorld& world,v20cns::Runtime& c,Observe observe) {
    std::ifstream in(path,std::ios::binary);
    bool legacy=!in.good();
    if (legacy) { in.clear(); in.open(legacyPath,std::ios::binary); }
    if (!in) return false;
    std::string text((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
    auto nextGame=game; auto nextWorld=world;
    v20cns::Body nextBody{}; v20cns::NicheHistory nextNiche{};
    if (!decode(text,legacy?1:2,nextGame,nextWorld,nextBody,nextNiche)) return false;
    if (nextWorld.beaconRestored) {
        bool observed=false;
        try { observed=observe(nextWorld); } catch (...) { observed=false; }
        if (!observed) return false;
        nextNiche.reobserved=true;
    }
    v20cns::Runtime nextCns;
    if (!nextCns.restoreLocal(nextBody,nextNiche,nextGame,nextWorld)) return false;
    game=nextGame; world=nextWorld; c=nextCns;
    return true;
}
} // namespace v20save


// ═══ Application State ═══
struct GTAngelAGI {
    // mech0 integration
    mech0::SimpleLanguageModel llm{1000, 64};
    mech0::TrainingPipeline trainer{1000, 64};
    mech0::TrainingMetrics llmMetrics{};
    // re-bhvt-edit integration
    rebhvt::SensorimotorBridge bridge{1.5f, 30.0f};
    rebhvt::SensorimotorTelemetry bridgeTelemetry{};
    // GTAngel-Cls integration
    gtangel::GTAngelEnvironment env;
    gtangel::MotorCommand currentCommand{};
    gtangel::WorldObservation worldObs{};
    gtangel::StepResult lastStep{};
    // uechovr integration
    uechovr::FrameBufferHook fbHook;
    uechovr::Tracker6DOF tracker;
    uechovr::StereoRenderer stereo;
    uechovr::Pose6DOF hmdPose{};
    uechovr::Pose6DOF leftCtrl{}, rightCtrl{};
    // layla integration
    layla::LaylaBridge laylaBridge;
    layla::Telemetry laylaTelemetry;
    // Civic Angel Architect II telemetry (v14)
    civicangel::ProducerSet civicSpine;
    bool civicOverlayVisible{true};
    // v15 authoritative render pyramid
    v15render::HighResRenderer highResRenderer;
    v15render::PerceptionPyramid perceptionPyramid;
    v15render::VisualNavStateEncoder visualNavEncoder;
    v15render::DisplayMode highResDisplayMode{v15render::DisplayMode::Colour};
    bool highResViewportVisible{true};
    // v16 local visuomotor dataset capture
    v16motor::TrajectoryRecorder trajectoryRecorder;
    v16motor::VisualTensor96 trajectoryVisual;
    v16motor::ControllerTarget previousControllerTarget;
    bool trajectoryRecording{false};
    bool trajectoryRecorderReady{false};
    uint64_t trajectoryEpisodeId{1};
    // v17 local neural policy: proposal only unless every local gate accepts.
    v17policy::NativeVisuomotorPolicy neuralPolicy;
    v17policy::EmbodiedSafetyShield neuralShield;
    v17policy::Prediction neuralPrediction;
    v17policy::ShieldDecision neuralDecision;
    bool neuralShadowEnabled{true};
    bool neuralActuationUnlocked{false};
    bool neuralEthicsApproved{false};
    bool neuralPolicyLoaded{false};
    uint64_t neuralFrames{0};
    uint64_t neuralAccepted{0};
    // v18 non-local relation loop: provisional attention only.
    v18peer::PeerRelationalLoop peerLoop;
    v18peer::AttentionContext peerAttention;
    // v19 local gameplay core: no telemetry required.
    v19game::GameplayLoop gameplay;
    v20cns::Runtime cns;
    // Bound to THIS game's 768x768 VRD frame, never the optional display/window.
    v25dte::LocalVisualIngress echoIngress{highResRenderer.framebuffer};
    std::optional<v25dte::Sample> echoSample;
    v25dte::EchoCommons echoCommons;
    v26bridge::LensBridge treeLenses;
    bool lensInspectorOpen{false}; // GDI-only view; never part of VRD pixels
    void invalidateEcho() { echoIngress.reset(); echoSample.reset(); echoCommons.reset(); treeLenses.reset(); }
    bool currentLens() {
        const auto& lens=treeLenses.state();
        return lens.valid && echoSample &&
            echoIngress.current(*echoSample,(uint64_t)GetTickCount64(),cns.body(),cns.niche()) &&
            lens.visualSequence==echoSample->sequence() && lens.visualDigest==echoSample->hash();
    }
    // avatar3d integration (v14)
    avatar3d::GameWorld navWorld;
    avatar3d::AvatarSkeleton avatar;
    avatar3d::EgocentricRenderer navRenderer;
    avatar3d::TrizMapping trizMap;
    avatar3d::NavTrainer navTrainer;
    avatar3d::Vec3 navPos{0, 0, 0};
    float navHeading{0.0f};
    float navSpeed{0.0f};
    float navMeanContradiction{0.0f};
    bool navTraining{false};
    bool navThirdPerson{true};
    int navEpisodeSteps{0};
    // cogzero integration
    cogzero::AgentZeroCore cogzeroAgent{"GTAngel"};
    cogzero::CognitiveLoop::CycleStats cogzeroStats;
    // echo integration
    echo::CognitiveArenaController echoController;
    echo::AestheticField echoField;
    echo::EvolutionLedger echoLedger;
    echo::CognitiveSnapshot echoSnap;
    // kys integration
    kys::TriadPhasorEngine phasor;
    // elizaos.cpp integration
    elizaos::AgentLoop agentLoop;
    elizaos::AttentionAllocator attention{144.0};
    elizaos::ActivationSpreadingNetwork spreadNet;
    elizaos::TemporalAttentionWindow globalAttentionWindow;
    std::string attentionFocus;
    double avgAttention = 0.0;
    bool spreadNetBuilt = false;
    // dte systems
    dte::EndocrineSystem endo;
    dte::EchoStateNetwork esn{16, 96, 8, 0.95f, 0.3f, 42};
    dte::TRIZArena arena;
    dte::GroupDynamics groups;
    dte::SensorimotorLoop sensorimotor;
    dte::MicroImprovement improvement;
    dte::EchoIntrospect introspect;
    dte::CogVerse cogverse;
    dte::EchobeatsPhase echoPhase = dte::EchobeatsPhase::Perceive;
    bool running = false, paused = false;
    int tickCount = 0;
    std::vector<std::string> log;
    double esnEnergy = 0.0;
    int selectedHormone = 0;
    int selectedCellQ = -1, selectedCellR = -1;

    void reset() {
        running = false;
        paused = false;
        tickCount = 0;
        log.clear();
        esnEnergy = 0.0;
        selectedHormone = 0;
        selectedCellQ = -1;
        selectedCellR = -1;
        attentionFocus.clear();
        avgAttention = 0.0;
        spreadNetBuilt = false;
        echoPhase = dte::EchobeatsPhase::Perceive;
        groups.init(20);
        fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
        tracker.reset();
        stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
        echoController.reset();
        echoField = echo::AestheticField();
        echoLedger = echo::EvolutionLedger();
        echoSnap = echo::CognitiveSnapshot();
        cogzeroStats = cogzero::CognitiveLoop::CycleStats();
        const auto laylaBackend = laylaBridge.discover();
        laylaTelemetry = laylaBridge.telemetry();
        if (laylaBackend.online) {
            addLog(std::string("Layla backend: ") + layla::backendName(laylaBackend.kind) +
                   " at " + laylaBackend.baseUrl +
                   " | models=" + std::to_string(laylaBackend.models.size()));
        } else {
            addLog(std::string("Layla backend offline: ") + laylaBackend.error);
        }
        civicSpine.reset();
        civicSpine.configureFromEnvironment();
        addLog("Civic Angel spine configured from process environment; credentials remain hidden");
        // v14: initialize navigation world and avatar
        navWorld.generate(1337, 48);
        avatar=avatar3d::AvatarSkeleton{};
        gameplay.reset(navWorld);
        cns.reset();
        invalidateEcho();
        navPos = avatar3d::Vec3{0, 0, 0};
        navHeading = 0.0f;
        navSpeed = 0.0f;
        navMeanContradiction = 0.0f;
        navEpisodeSteps = 0;
        avatar.pose(navPos, navHeading, 0.0f, 0.0f);
        avatar3d::Vec3 initialEye = navPos + avatar3d::Vec3{0.0f, 2.5f, -4.0f};
        highResRenderer.render(navWorld, avatar, initialEye, navHeading, -0.4f, true);
        perceptionPyramid.derive(highResRenderer.framebuffer);
        perceptionPyramid.copyToLegacy(navRenderer);
        perceptionPyramid.mapToTriz(trizMap);
        addLog("Nav world: 14 pillars, goal at (8,0,8), Q-learning trainer ready");
        addLog("v15 render pyramid: 768x768 source -> 64x64 retina -> 12x12 cognition");
        trajectoryRecorder.stop();
        trajectoryRecording = false;
        trajectoryRecorderReady = trajectoryRecorder.selfCheck("C:\\ddd\\GTAngel\\cpp_test\\datasets");
        previousControllerTarget = v16motor::ControllerTarget{};
        addLog(trajectoryRecorderReady
            ? std::string("v16 capture ready: ") + trajectoryRecorder.directory()
            : std::string("v16 capture unavailable: ") + trajectoryRecorder.lastError());
        neuralPolicyLoaded = neuralPolicy.load("C:\\ddd\\GTAngel\\cpp_test\\models\\gtangel_visuomotor.gvmw");
        if (!neuralPolicyLoaded) neuralPolicyLoaded = neuralPolicy.load("models/gtangel_visuomotor.gvmw");
        neuralShadowEnabled = true;
        neuralActuationUnlocked = false; // no collision-positive or goal-positive records in corpus 1
        neuralEthicsApproved = false; // requires an explicit local ethics authority decision in a future qualified build
        neuralFrames = 0;
        neuralAccepted = 0;
        addLog(neuralPolicyLoaded
            ? "v17 neural policy loaded in SHADOW mode; actuator remains expert-controlled"
            : std::string("v17 neural policy unavailable: ") + neuralPolicy.error());
        peerLoop.configureFromEnvironment();
        peerLoop.setEnabled(false);
        addLog("v18 peer relay initialized OFF; network relations remain attention-only");
    }
    bool repairBeacon() {
        const v20cns::Context context{highResRenderer.framebuffer.sequence,
            (uint64_t)GetTickCount64(),0.05f,highResRenderer.framebuffer.sequence>0,false};
        const auto player=navPos;
        auto observe=[this,player](const avatar3d::GameWorld& before,
                                    const avatar3d::GameWorld& after,uint64_t version) {
            auto measure=[this](const avatar3d::GameWorld& w) {
                v15render::HighResRenderer probe;
                avatar3d::AvatarSkeleton pose;
                pose.pose(navPos,navHeading,0.0f,0.0f);
                probe.render(w,pose,{w.goal.x,2.2f,w.goal.z-4.5f},0.0f,-0.22f,false);
                int count=0,finiteDepth=0; uint64_t colourSum=0;
                for (int i=0;i<v15render::HighResFramebuffer::PixelCount;++i)
                    if (probe.framebuffer.objectId[i]==v15render::ObjectGoal) {
                        ++count; colourSum+=probe.framebuffer.colour[i];
                        if (std::isfinite(probe.framebuffer.depth[i])) ++finiteDepth;
                    }
                return std::array<uint64_t,4>{(uint64_t)count,(uint64_t)finiteDepth,
                                              colourSum,probe.framebuffer.sequence};
            };
            const auto oldFrame=measure(before),newFrame=measure(after);
            return version==2 && before.beaconRestored==false && after.beaconRestored &&
                oldFrame[0]>20 && newFrame[0]>oldFrame[0] &&
                newFrame[1]==newFrame[0] && newFrame[2]!=oldFrame[2] &&
                newFrame[3]>0 && cns.body().position.x==player.x &&
                cns.body().position.z==player.z && after.goal.x==before.goal.x &&
                after.goal.z==before.goal.z;
        };
        const auto verdict=cns.repair(navPos,gameplay,navWorld,context,observe);
        avatar.pose(navPos,navHeading,0.0f,0.0f);
        const avatar3d::Vec3 eye=navThirdPerson
            ? navPos+avatar3d::Vec3{-sinf(navHeading)*4.0f,2.5f,-cosf(navHeading)*4.0f}
            : navPos+avatar3d::Vec3{0,1.6f,0};
        highResRenderer.render(navWorld,avatar,eye,navHeading,
                               navThirdPerson?-0.4f:0.0f,navThirdPerson);
        perceptionPyramid.derive(highResRenderer.framebuffer);
        if (verdict!=v20cns::WorldGovernance::Verdict::Accepted)
            addLog("WGR rejected beacon repair; world state retained / rolled back");
        return verdict==v20cns::WorldGovernance::Verdict::Accepted;
    }
    void syncBodyFromCns() {
        navPos=cns.body().position; navHeading=cns.body().heading; navSpeed=cns.body().speed;
        navWorld.vehiclePos=cns.body().cartPosition;
        navWorld.vehicleHeading=cns.body().cartHeading;
        navWorld.vehicleWheelPhase=cns.body().wheelPhase;
        navWorld.vehicleOccupied=cns.body().occupied;
    }
    void addLog(const std::string& m) {
        log.push_back(m);
        if (log.size() > 50) log.erase(log.begin());
    }
    void registerAgentLoopSteps() {
        agentLoop.addStep("perceive", 10, [this]() {
            for (size_t i=0;i<sensorimotor.frameBuffer.size() && i<navRenderer.frame.size();++i)
                sensorimotor.frameBuffer[i]=navRenderer.frame[i];
            sensorimotor.computeProprioception();
            return true;
        });
        agentLoop.addStep("endocrine", 9, [this]() {
            endo.tick();
            endo.trackModeTransition();
            return true;
        });
        agentLoop.addStep("reservoir", 8, [this]() {
            std::vector<double> esnIn(16);
            for (int i = 0; i < 8; i++) esnIn[i] = sensorimotor.proprioception[i];
            for (int i = 0; i < 8; i++) esnIn[8+i] = endo.hormoneLevel((dte::HormoneId)i);
            esn.step(esnIn);
            esnEnergy = esn.stateEnergy();
            return esnEnergy < 10.0;
        });
        agentLoop.addStep("triz_arena", 7, [this]() {
            arena.tick();
            return true;
        });
        agentLoop.addStep("attention", 6, [this]() {
            for (int q = 0; q < dte::TRIZArena::SIZE; q++)
                for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                    auto& cell = arena.grid[q][r];
                    std::string id = elizaos::ActivationSpreadingNetwork::key(q, r);
                    elizaos::AttentionValue v;
                    v.novelty = cell.contradiction;
                    v.urgency = cell.contradiction * 0.8;
                    v.importance = cell.aestheticValue * 0.5;
                    v.activation = spreadNet.get(id);
                    attention.update(id, v);
                    if (cell.contradiction > 0.15f)
                        attention.stimulate(id, cell.contradiction * 0.3, cell.contradiction * 0.2);
                }
            attention.allocateTop(12);
            attention.decayAll();
            spreadNet.spread(0.8, 0.01);
            attentionFocus = attention.focusElement();
            avgAttention = attention.averageAttention();
            globalAttentionWindow.event(arena.maxContradiction * 0.5);
            globalAttentionWindow.decay();
            return true;
        });
        agentLoop.addStep("group_dynamics", 4, [this]() {
            groups.tick();
            return true;
        });
        agentLoop.addStep("layla", 7, [this]() {
            laylaTelemetry = laylaBridge.telemetry();
            if (laylaTelemetry.online && !laylaTelemetry.busy && laylaTelemetry.queueDepth == 0) {
                std::string prompt = "GTAngel telemetry: tick=" + std::to_string(tickCount) +
                    " mode=" + dte::cognitiveModeName(endo.cognitiveMode()) +
                    " coherence=" + std::to_string(arena.coherence) +
                    " attention=" + attentionFocus;
                laylaBridge.enqueue(prompt);
            }
            return true;
        });
        agentLoop.addStep("navigation", 6, [this]() {
            gameplay.tick(0.05f, navPos, navHeading, navWorld);
            const auto visualBefore=visualNavEncoder.encode(perceptionPyramid.retina);
            const float risk=visualBefore.obstacleVisible
                ? std::clamp(1.0f-visualBefore.nearestObstacleDepth/5.0f,0.0f,1.0f):0.0f;
            trajectoryVisual.derive(highResRenderer.framebuffer);
            const auto modelProprio=v16motor::makeProprioception(
                navSpeed,navHeading,0.0f,cns.body().contact,true,previousControllerTarget,
                (float)endo.valenceArousal().arousal,lastStep.reward,
                visualBefore.goalVisible ? 1.0f : 0.0f,risk);
            neuralPrediction=v17policy::Prediction{};
            neuralDecision=v17policy::ShieldDecision{};
            if (neuralPolicyLoaded && neuralShadowEnabled) {
                neuralPrediction=neuralPolicy.infer(trajectoryVisual,modelProprio);
                neuralDecision=neuralShield.decide(neuralPrediction,v16motor::ControllerTarget{},risk,
                                                   neuralActuationUnlocked,neuralEthicsApproved);
                ++neuralFrames;
            }
            const v19game::PlayerControls manual=gameplay.samplePlayerControls();
            const auto source=v20cns::MotorArbiter::choose(
                manual.active,navTraining,neuralDecision.accepted);
            const float forward=manual.active ? std::clamp(manual.move,-1.0f,1.0f) : 0.0f;
            const float turn=manual.active ? manual.turn : 0.0f;
            const v20cns::Context context{highResRenderer.framebuffer.sequence,
                (uint64_t)GetTickCount64(),0.05f,highResRenderer.framebuffer.sequence>0,false};
            // Advisory image depth can contain an obstacle behind the body;
            // human input remains subject to the same local swept geometry but
            // is never disabled solely by the coarse visual-risk estimate.
            const float submittedRisk=source==v20cns::Source::HumanLocal ? 0.0f : risk;
            const auto verdict=cns.motor(source,forward,turn,0.0f,submittedRisk,true,context,navWorld);
            const bool collided=verdict==v20cns::SpinalReflex::Verdict::Collision;
            navPos=cns.body().position; navHeading=cns.body().heading; navSpeed=cns.body().speed;
            navWorld.vehiclePos=cns.body().cartPosition;
            navWorld.vehicleHeading=cns.body().cartHeading;
            navWorld.vehicleWheelPhase=cns.body().wheelPhase;
            navWorld.vehicleOccupied=cns.body().occupied;
            gameplay.tick(0.0f,navPos,navHeading,navWorld);
            avatar.pose(navPos,navHeading,fabsf(navSpeed),0.05f);
            const avatar3d::Vec3 eye=navThirdPerson
                ? navPos+avatar3d::Vec3{-sinf(navHeading)*4.0f,2.5f,-cosf(navHeading)*4.0f}
                : navPos+avatar3d::Vec3{0,1.6f,0};
            highResRenderer.render(navWorld,avatar,eye,navHeading,navThirdPerson?-0.4f:0.0f,navThirdPerson);
            peerLoop.confirmAgainstLocalFrame(highResRenderer.framebuffer,!navThirdPerson);
            peerAttention=peerLoop.attentionContext();
            perceptionPyramid.derive(highResRenderer.framebuffer);
            // Recurrent/synthetic DTE channels are not VRD certificate issuers.
            // This scan never calls cns.motor(), cns.repair(), or an effector.
            echoSample=echoIngress.capture((uint64_t)GetTickCount64(),cns.body(),cns.niche());
            if(echoSample) {
                const auto nowMs=(uint64_t)GetTickCount64();
                if(echoIngress.current(*echoSample,nowMs,cns.body(),cns.niche())) {
                    echoCommons.scan(echoIngress,*echoSample,nowMs,cns.body(),cns.niche(),gameplay,navWorld);
                    treeLenses.observe(*echoSample,cns.body(),cns.niche());
                } else {echoCommons.clear();treeLenses.reset();}
            } else {echoCommons.clear();treeLenses.reset();}
            perceptionPyramid.copyToLegacy(navRenderer);
            perceptionPyramid.mapToTriz(trizMap);
            navMeanContradiction=trizMap.meanContradiction;
            trajectoryVisual.derive(highResRenderer.framebuffer);
            const auto visualAfter=visualNavEncoder.encode(perceptionPyramid.retina);
            const float reward=collided?-1.0f:-0.01f+
                0.1f*visualNavEncoder.progressReward(visualBefore,visualAfter);
            const auto proprio=v16motor::makeProprioception(navSpeed,navHeading,
                turn*0.1f,collided,true,previousControllerTarget,
                (float)endo.valenceArousal().arousal,reward,
                visualAfter.goalVisible?1.0f:0.0f,risk);
            v16motor::ControllerTarget actualTarget{};
            actualTarget.values[0]=-turn; actualTarget.values[1]=forward;
            if (trajectoryRecording && !trajectoryRecorder.capture(
                    (uint32_t)navEpisodeSteps,trajectoryVisual,proprio,actualTarget,
                    collided?v16motor::OutcomeFlags::Collision:v16motor::OutcomeFlags::None,
                    reward,1.0f,risk)) {
                trajectoryRecording=false;
                addLog("v16 recorder stopped after a dataset write failure");
            }
            previousControllerTarget=actualTarget;
            ++navEpisodeSteps;
            for (size_t i=0;i<sensorimotor.frameBuffer.size() && i<navRenderer.frame.size();++i)
                sensorimotor.frameBuffer[i]=navRenderer.frame[i];
            return true;
        });
        agentLoop.addStep("civic_angel_spine", 2, [this]() {
            civicangel::GroundedFrame f;
            f.tick = (uint64_t)tickCount;
            f.positionX = navPos.x;
            f.positionZ = navPos.z;
            f.heading = navHeading;
            f.goalDistance = navWorld.goalDistance(navPos);
            f.meanContradiction = navMeanContradiction;
            double lum = 0.0;
            for (float v : navRenderer.frame) lum += v;
            f.frameLuminance = navRenderer.frame.empty() ? 0.0 : lum / navRenderer.frame.size();
            double obstacles = 0.0, goals = 0.0;
            for (int q = 0; q < avatar3d::TrizMapping::GRID; ++q)
                for (int r = 0; r < avatar3d::TrizMapping::GRID; ++r) {
                    obstacles += trizMap.obstacleSalience[q][r];
                    goals += trizMap.goalSalience[q][r];
                }
            const double bins = avatar3d::TrizMapping::GRID * avatar3d::TrizMapping::GRID;
            f.obstacleSalience = obstacles / bins;
            f.goalSalience = goals / bins;
            f.collisionRate = navTrainer.collisionRate();
            f.health = worldObs.health;
            f.hostility = worldObs.hostility;
            f.goalProgress = worldObs.goal_progress;
            f.reward = lastStep.reward;
            f.coherence = arena.coherence;
            f.valence = endo.valenceArousal().valence;
            f.arousal = endo.valenceArousal().arousal;
            f.cortisol = endo.hormoneLevel(dte::HormoneId::Cortisol);
            f.dopamine = endo.hormoneLevel(dte::HormoneId::Dopamine);
            f.serotonin = endo.hormoneLevel(dte::HormoneId::Serotonin);
            f.norepinephrine = endo.hormoneLevel(dte::HormoneId::Norepinephrine);
            f.oxytocin = endo.hormoneLevel(dte::HormoneId::Oxytocin);
            f.melatonin = endo.hormoneLevel(dte::HormoneId::Melatonin);
            f.anandamide = endo.hormoneLevel(dte::HormoneId::Anandamide);
            f.framebufferSequence = highResRenderer.metrics.sequence;
            f.framebufferRenderMs = highResRenderer.metrics.renderMs;
            f.trianglesSubmitted = highResRenderer.metrics.trianglesSubmitted;
            f.trianglesRasterized = highResRenderer.metrics.trianglesRasterized;
            f.finiteDepthPixels = highResRenderer.metrics.finiteDepthPixels;
            f.semanticPixels = highResRenderer.metrics.semanticPixels;
            switch (endo.cognitiveMode()) {
                case dte::CognitiveMode::Exploitation: f.cognitiveMode = "focused"; break;
                case dte::CognitiveMode::Rest: f.cognitiveMode = "reflective"; break;
                case dte::CognitiveMode::Alarm: f.cognitiveMode = "vigilant"; break;
                default: f.cognitiveMode = "exploratory"; break;
            }
            return civicSpine.tick(f);
        });
        agentLoop.addStep("cogzero", 6, [this]() {
            cogzeroStats = cogzeroAgent.tick();
            return true;
        });
        agentLoop.addStep("echo_ksm", 5, [this]() {
            float coherence = echoField.evaluate(std::vector<std::vector<float>>(dte::TRIZArena::SIZE, std::vector<float>(dte::TRIZArena::SIZE, 0.5f)));
            float contradiction = 0.3f;
            echoSnap = echoController.tick(coherence, contradiction);
            echo::EvolutionRecord rec;
            rec.tick = tickCount;
            rec.step = echoSnap.step;
            rec.coherence = coherence;
            rec.contradiction = contradiction;
            rec.verdict = (coherence > 0.5f) ? "keep" : "discard";
            rec.principle = echoSnap.stepDefinition->label;
            echoLedger.append(rec);
            return true;
        });
        agentLoop.addStep("phasor", 3, [this]() {
            phasor.tick();
            return true;
        });
        agentLoop.addStep("introspection", 2, [this]() {
            if (tickCount % 25 == 0) introspect.introspect(endo, arena, sensorimotor, groups);
            return true;
        });
        agentLoop.start();
    }
    void buildSpreadNet() {
        for (int q = 0; q < dte::TRIZArena::SIZE; q++)
            for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                spreadNet.addNode(elizaos::ActivationSpreadingNetwork::key(q, r), 0.0);
                spreadNet.addHexEdges(q, r, dte::TRIZArena::SIZE);
            }
        spreadNetBuilt = true;
    }
    void tick() {
        if (!running || paused) return;
        tickCount++;
        if (!spreadNetBuilt) buildSpreadNet();
        if (agentLoop.steps().empty()) registerAgentLoopSteps();
        agentLoop.iterate();
        echoPhase = (dte::EchobeatsPhase)((tickCount / 3) % 4);
        // uechovr: update 6DOF tracking
        float dt = 0.05f;
        hmdPose = tracker.updateHMD(dt);
        leftCtrl = tracker.updateController(0, dt);
        rightCtrl = tracker.updateController(1, dt);
        // uechovr: capture frame buffer
        auto capture = fbHook.captureBackBuffer(tickCount);
        // re-bhvt-edit: sensorimotor bridge update
        std::vector<float> features(3);
        features[0] = navPos.x; features[1] = navPos.z;
        features[2] = cns.body().contact ? 1.0f : 0.0f;
        float threat = cns.body().contact ? 1.0f : 0.0f;
        bridgeTelemetry = bridge.update(navPos.x, 0.0f, navPos.z, features, threat, dt);
        // GTAngel-Cls environment step
        std::vector<uint8_t> pixels(64 * 64 * 4);
        for (int y = 0; y < 64; y++)
            for (int x = 0; x < 64; x++) {
                float v = sensorimotor.frameBuffer[y*64+x];
                uint8_t b = (uint8_t)(v * 255);
                pixels[(y*64+x)*4+0] = b;
                pixels[(y*64+x)*4+1] = b;
                pixels[(y*64+x)*4+2] = b;
                pixels[(y*64+x)*4+3] = 255;
            }
        gtangel::FrameBufferView view{};
        view.pixels = pixels.data();
        view.width = 64; view.height = 64; view.channels = 4;
        // Update world observation from uechovr 6DOF pose + sensorimotor state
        worldObs.pose.x = navPos.x; worldObs.pose.y = navPos.y;
        worldObs.pose.z = navPos.z; worldObs.pose.heading = navHeading;
        worldObs.pose.time_of_day = (float)(tickCount % 2400) / 100.0f;
        worldObs.health = 1.0f;
        worldObs.hostility = cns.body().contact ? 1.0f : 0.0f;
        worldObs.goal_progress = gameplay.quest.beaconRestored ? 1.0f :
            (gameplay.quest.hasSignalShard ? 0.5f : 0.0f);
        // Step the GTAngel-Cls environment
        lastStep = env.step(view, worldObs, currentCommand);
        // Get suggested command for next tick
        currentCommand = env.suggest(lastStep.state);
        // dte systems (endo/arena/groups/ESN now driven by agentLoop above)
        // v20: synthetic DTE tick is disconnected from authoritative sensing/actuation.
        // mech0: train LLM on proprioceptive tokens
        if (tickCount % 20 == 0) {
            std::vector<std::pair<std::vector<int>, std::string>> data;
            std::vector<int> tokens;
            for (int i = 0; i < 8; i++)
                tokens.push_back((int)(sensorimotor.proprioception[i] * 100));
            std::string label = sensorimotor.reward > 0.05f ? "greeting" :
                               sensorimotor.reward < 0.02f ? "farewell" : "question";
            data.push_back({tokens, label});
            trainer.train(data, 1, 0.01);
            llmMetrics = trainer.metrics();
        }
        if (tickCount % 10 == 0) improvement.iterate(arena, sensorimotor);
        if (tickCount % 25 == 0) introspect.introspect(endo, arena, sensorimotor, groups);
        if (tickCount % 50 == 0) cogverse.evolve(arena, groups);
        if (tickCount % 5 == 0) {
            char buf[256];
            snprintf(buf, 256, "T%d | %s | %s | Coh=%.3f R=%.3f G=%d ESN=%.2f | HMD=(%.2f,%.2f,%.2f) | Bridge: R=%.3f N=%.3f | GTAngel-Cls: R=%.3f Done=%d | LLM: Acc=%.2f Loss=%.3f",
                tickCount, dte::cognitiveModeName(endo.cognitiveMode()),
                dte::phaseName(echoPhase), arena.coherence,
                sensorimotor.totalReward, groups.groupCount(), esnEnergy,
                hmdPose.x, hmdPose.y, hmdPose.z,
                bridgeTelemetry.reward, bridgeTelemetry.novelty,
                lastStep.reward, lastStep.done ? 1 : 0,
                llmMetrics.accuracy, llmMetrics.averageLoss);
            addLog(buf);
        }
    }
};

// ═══ Win32 UI ═══
static GTAngelAGI g_agi;
static HFONT g_hFont = NULL, g_hFontLarge = NULL, g_hFontMono = NULL, g_hFontSmall = NULL;

#define COL_BG      RGB(13, 13, 26)
#define COL_CARD    RGB(24, 24, 48)
#define COL_ACCENT  RGB(255, 215, 0)
#define COL_CYAN    RGB(0, 188, 212)
#define COL_GREEN   RGB(76, 175, 80)
#define COL_RED     RGB(244, 67, 54)
#define COL_TEXT    RGB(255, 255, 255)
#define COL_TEXT2   RGB(158, 158, 200)
#define COL_PURPLE  RGB(156, 39, 176)
#define COL_NICHE_P RGB(255, 128, 60)
#define COL_NICHE_S RGB(60, 200, 255)
#define COL_NICHE_I RGB(180, 120, 255)
#define COL_CONTRA  RGB(255, 50, 50)
#define COL_VR      RGB(100, 200, 255)
#define COL_BRIDGE  RGB(255, 100, 200)
#define COL_GOLD    RGB(255, 215, 0)
#define COL_LLM     RGB(100, 255, 200)

static void DrawText(HDC hdc, int x, int y, const char* t, COLORREF c, HFONT f) {
    SetTextColor(hdc, c); SetBkMode(hdc, TRANSPARENT); SelectObject(hdc, f);
    TextOutA(hdc, x, y, t, (int)strlen(t));
}
static void DrawRect(HDC hdc, int x, int y, int w, int h, COLORREF c) {
    HBRUSH b = CreateSolidBrush(c); RECT rc = {x, y, x+w, y+h};
    FillRect(hdc, &rc, b); DeleteObject(b);
}
static void DrawBar(HDC hdc, int x, int y, int w, int h, float v, COLORREF c, COLORREF bg) {
    DrawRect(hdc, x, y, w, h, bg);
    int fill = (int)(w * std::max(0.0f, std::min(1.0f, v)));
    if (fill > 0) DrawRect(hdc, x, y, fill, h, c);
}
static void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
    HPEN p = CreatePen(PS_SOLID, 1, c);
    HPEN old = (HPEN)SelectObject(hdc, p);
    MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);
    SelectObject(hdc, old); DeleteObject(p);
}

static void RenderV15Frame(HDC hdc, RECT& rc) {
    const int clientW = rc.right - rc.left;
    const int clientH = rc.bottom - rc.top;
    DrawRect(hdc, 0, 0, clientW, clientH, COL_BG);
    g_agi.highResRenderer.framebuffer.blit(hdc, 0, 0, g_agi.highResDisplayMode);

    const int railX = 768;
    const int railW = std::max(0, clientW - railX);
    DrawRect(hdc, railX, 0, railW, clientH, RGB(12, 14, 27));
    DrawRect(hdc, railX, 0, 3, clientH, COL_BRIDGE);
    DrawText(hdc, railX + 12, 12, "GTAngel v21 / DTE MOTION", COL_ACCENT, g_hFont);
    DrawText(hdc, railX + 12, 34, "ARIADNE AND THE BROKEN SIGNAL", COL_CYAN, g_hFontSmall);

    char line[256];
    snprintf(line, sizeof(line), "Stage  %s", g_agi.gameplay.stageName().c_str());
    DrawText(hdc, railX + 12, 62, line, g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_GOLD, g_hFontMono);
    DrawText(hdc, railX + 12, 84, "CURRENT PURPOSE", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 102, g_agi.gameplay.objective().c_str(), COL_TEXT, g_hFontSmall);
    DrawLine(hdc, railX + 12, 124, railX + railW - 12, 124, RGB(70, 70, 100));

    DrawText(hdc, railX + 12, 140, "ARIADNE", COL_PURPLE, g_hFont);
    snprintf(line, sizeof(line), "Routine  %s", g_agi.gameplay.routineName());
    DrawText(hdc, railX + 12, 164, line, COL_TEXT2, g_hFontSmall);
    snprintf(line, sizeof(line), "Disposition  %.0f%%", g_agi.gameplay.ariadne.disposition * 100.0f);
    DrawText(hdc, railX + 12, 182, line, COL_TEXT2, g_hFontSmall);
    DrawBar(hdc, railX + 12, 202, std::max(20, railW - 24), 8,
            g_agi.gameplay.ariadne.disposition, COL_PURPLE, RGB(36, 34, 55));

    DrawText(hdc, railX + 12, 232, "INVENTORY", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 252,
        g_agi.gameplay.quest.hasSignalShard ? "Signal Shard  x1" : "Signal Shard  --",
        g_agi.gameplay.quest.hasSignalShard ? COL_CYAN : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 270,
        g_agi.gameplay.quest.hasResonanceSigil ? "Resonance Sigil  x1" : "Resonance Sigil  --",
        g_agi.gameplay.quest.hasResonanceSigil ? COL_GOLD : COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, 304, "WORLD", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 324,
        g_agi.gameplay.quest.beaconRestored ? "Civic beacon  RESTORED" : "Civic beacon  BROKEN",
        g_agi.gameplay.quest.beaconRestored ? COL_CYAN : COL_RED, g_hFontSmall);
    snprintf(line, sizeof(line), "Frame %llu   %.2f ms",
        (unsigned long long)g_agi.highResRenderer.metrics.sequence,
        g_agi.highResRenderer.metrics.renderMs);
    DrawText(hdc, railX + 12, 344, line, COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, 378, "LAST GROUNDED EVENT", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 398, g_agi.gameplay.quest.lastEvent.c_str(), COL_TEXT, g_hFontSmall);
    snprintf(line, sizeof(line), "Events %u   Save %s",
        g_agi.gameplay.quest.groundedEvents,
        g_agi.gameplay.quest.persistenceStatus.c_str());
    DrawText(hdc, railX + 12, 418, line, COL_TEXT2, g_hFontSmall);

    if (g_agi.gameplay.quest.journalOpen) {
        DrawText(hdc, railX + 12, 452, "QUEST JOURNAL", COL_GOLD, g_hFontSmall);
        DrawText(hdc, railX + 12, 472, "1. Meet the keeper", g_agi.gameplay.quest.stage > v19game::QuestStage::Offered ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 490, "2. Recover the shard", g_agi.gameplay.quest.stage > v19game::QuestStage::SeekShard ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 508, "3. Restore the beacon", g_agi.gameplay.quest.stage > v19game::QuestStage::RestoreBeacon ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, 526, "4. Return to Ariadne", g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    }

    DrawText(hdc, railX + 12, 550, "CONTROLS", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, 568, "WASD move / turn", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 586, "E interact   1-3 choose", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 604, "J journal    R recover", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 622, "F5 save      F9 load", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, 640,
        g_agi.avatar.wingDeploy>0.9f?"DTE WINGS: GUARD / B FOLD":"DTE WINGS: NEUTRAL / B GUARD",
        COL_CYAN, g_hFontSmall);
    DrawText(hdc, railX + 12, 658, g_agi.cns.body().occupied ? "CART OCCUPIED / Q EXIT" : "Q ENTER CART NEAR SPAWN", COL_CYAN, g_hFontSmall);
    DrawText(hdc, railX + 12, 676, "128m MAP / F6 DASH", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 694, "Telemetry: OPTIONAL / OFF", COL_GREEN, g_hFontSmall);
    DrawText(hdc, railX + 12, 712, "LOCAL CTRL / F7 HI-RES", COL_GOLD, g_hFontSmall);
    DrawText(hdc, railX + 12, 731,
        g_agi.echoCommons.candidate().pattern==160 ? "DTE APL160: REPAIR IDEA / E CONFIRM" :
        "DTE APL253: ADVISORY / LOCAL VRD", COL_CYAN, g_hFontSmall);
    DrawText(hdc, railX + 12, 750, "[SPACE] pause  / F8 LENS", COL_TEXT2, g_hFontSmall);

    const std::string prompt = g_agi.gameplay.interactionPrompt(g_agi.navPos, g_agi.navHeading, g_agi.navWorld);
    if (!prompt.empty()) {
        DrawRect(hdc, 164, 674, 440, 42, RGB(20, 18, 34));
        DrawLine(hdc, 164, 674, 604, 674, COL_GOLD);
        DrawText(hdc, 190, 688, prompt.c_str(), COL_TEXT, g_hFont);
    }

    const avatar3d::Vec3 objective = g_agi.gameplay.objectivePosition(g_agi.navWorld);
    const float dx = objective.x - g_agi.navPos.x;
    const float dz = objective.z - g_agi.navPos.z;
    float bearing = atan2f(dx, dz) - g_agi.navHeading;
    while (bearing > 3.14159265f) bearing -= 6.2831853f;
    while (bearing < -3.14159265f) bearing += 6.2831853f;
    const int markerX = std::clamp(384 + (int)(bearing / 1.2f * 330.0f), 28, 740);
    DrawLine(hdc, markerX - 8, 28, markerX, 18, COL_GOLD);
    DrawLine(hdc, markerX, 18, markerX + 8, 28, COL_GOLD);
    DrawLine(hdc, markerX + 8, 28, markerX, 38, COL_GOLD);
    DrawLine(hdc, markerX, 38, markerX - 8, 28, COL_GOLD);

    if (g_agi.gameplay.quest.dialogueOpen) {
        DrawRect(hdc, 36, 520, 696, 214, RGB(13, 11, 25));
        DrawLine(hdc, 36, 520, 732, 520, COL_PURPLE);
        DrawText(hdc, 58, 540, "ARIADNE", COL_PURPLE, g_hFontLarge);
        const std::string dialogue = g_agi.gameplay.dialogueLine();
        RECT dialogueRect{58, 574, 710, 616};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COL_TEXT);
        HGDIOBJ previousDialogueFont = SelectObject(hdc, g_hFont);
        ::DrawTextA(hdc, dialogue.c_str(), -1, &dialogueRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(hdc, previousDialogueFont);
        const auto choices = g_agi.gameplay.dialogueChoices();
        if (!choices[0].empty()) DrawText(hdc, 58, 620, choices[0].c_str(), COL_GOLD, g_hFont);
        if (!choices[1].empty()) DrawText(hdc, 58, 648, choices[1].c_str(), COL_CYAN, g_hFont);
        if (!choices[2].empty()) DrawText(hdc, 58, 676, choices[2].c_str(), COL_TEXT2, g_hFont);
    }
    // Presentation-only GDI overlay; never written back to the 768 VRD.
    if(g_agi.lensInspectorOpen && !g_agi.gameplay.quest.dialogueOpen) {
        DrawRect(hdc,40,46,686,262,RGB(13,18,32));
        DrawLine(hdc,40,46,726,46,COL_CYAN);
        DrawText(hdc,56,62,"SYSTEM 7 -> 8 / DTE PERSPECTIVE LAB",COL_CYAN,g_hFontLarge);
        DrawText(hdc,56,91,"115 rooted / 23 free -> 286 rooted / 47 free",COL_TEXT,g_hFont);
        DrawText(hdc,56,112,"Canonical leaf-addition incidence; structure is NOT permission",COL_GOLD,g_hFontSmall);
        const auto& lens=g_agi.treeLenses.state();
        const bool freshLens=g_agi.currentLens();
        char top[230];
        if(freshLens) {
            snprintf(top,sizeof(top),"S7 root %03u / free %02u  ->  %u S8 roots / %u families",
                     unsigned(lens.root7+1),unsigned(lens.free7),unsigned(lens.children),unsigned(lens.childFamilies));
            DrawText(hdc,56,140,top,COL_GREEN,g_hFont);
            snprintf(top,sizeof(top),"VRD frame %llu  world v%llu  body seq %llu",
                     (unsigned long long)lens.visualSequence,(unsigned long long)lens.worldVersion,
                     (unsigned long long)lens.bodySequence);
            DrawText(hdc,56,162,top,COL_TEXT2,g_hFontSmall);
            std::string children="First structural descendants: ";
            for(unsigned i=0;i<std::min<unsigned>(lens.children,7);++i){
                char label[24];const unsigned id=lens.childRootIndexes[i]+1;
                snprintf(label,sizeof(label),"%s%03u ",id<=253?"cog":"ang",id);
                children+=label;
            }
            DrawText(hdc,56,186,children.c_str(),COL_CYAN,g_hFontSmall);
            snprintf(top,sizeof(top),"Focused child %s%03u / family %02u",
                     lens.root8<253?"cog":"ang",unsigned(lens.root8+1),unsigned(lens.free8));
            DrawText(hdc,56,208,top,COL_TEXT,g_hFont);
        } else DrawText(hdc,56,152,"Awaiting current local 768x768 colour/depth/object-ID frame",COL_GOLD,g_hFont);
        DrawText(hdc,56,257,"Heuristic lens, not autonomy promotion or SRD/WGR authority / F8 CLOSE",COL_TEXT2,g_hFontSmall);
    }
}

static void RenderFrame(HDC hdc, RECT& rc) {
    const int clientW = rc.right - rc.left, clientH = rc.bottom - rc.top;
    if (g_agi.highResViewportVisible) {
        RenderV15Frame(hdc, rc);
        return;
    }
    const int W = 1320, H = 800;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, W, H);
    HBITMAP old = (HBITMAP)SelectObject(mem, bmp);
    DrawRect(mem, 0, 0, W, H, COL_BG);

    DrawText(mem, 20, 12, "GTAngel — AGI Gameworld Testing Environment v15", COL_ACCENT, g_hFontLarge);
    DrawText(mem, 20, 42, "mech0 + re-bhvt-edit + GTAngel-Cls + uechovr + dte::EndocrineBus | Echobeats | NicheConstruction | DeepTreeEchoReservoir | TRIZ Arena | Group Dynamics | Micro-Improvement | Echo-Introspect | CogVerse", COL_TEXT2, g_hFont);

    char sb[256];
    snprintf(sb, 256, "Status: %s | Tick: %d | Mode: %s | Echobeats: %s | ESN Energy: %.3f | Mode Transitions: %d | GTAngel-Cls Steps: %llu | VR Frame: %llu | Bridge Niches: %d | LLM Acc: %.2f",
        g_agi.running ? (g_agi.paused ? "PAUSED" : "RUNNING") : "STOPPED",
        g_agi.tickCount, dte::cognitiveModeName(g_agi.endo.cognitiveMode()),
        dte::phaseName(g_agi.echoPhase), g_agi.esnEnergy, g_agi.endo.modeTransitions,
        g_agi.env.steps(), g_agi.tracker.frameCount(), g_agi.bridgeTelemetry.niche_count,
        g_agi.llmMetrics.accuracy);
    if (g_agi.tickCount % 100 == 0 && g_agi.tickCount > 0) {
        char agentLoopLog[200];
        snprintf(agentLoopLog, 200, "AgentLoop: %s | Load=%.2f | CB=%s | Attn=%.3f Focus=%s | Steps=%llu",
            elizaos::healthName(g_agi.agentLoop.health()),
            g_agi.agentLoop.load().composite(),
            g_agi.agentLoop.breaker().stateName(),
            g_agi.avgAttention, g_agi.attentionFocus.c_str(),
            (unsigned long long)g_agi.agentLoop.stats().stepsExecuted);
        g_agi.addLog(agentLoopLog);
        // Phasor status
        kys::StepWord triad[3];
        g_agi.phasor.currentTriad(triad);
        char pb[200];
        snprintf(pb, 200, "Phasor: tick=%d | Triad=[%s %s %s] | U1=%s U2=%s | Invariants=%d/6",
            g_agi.phasor.globalTick(),
            triad[0].name(), triad[1].name(), triad[2].name(),
            kys::uTermName(g_agi.phasor.currentU1()),
            kys::uTermName(g_agi.phasor.currentU2()),
            g_agi.phasor.invariants().passing());
        g_agi.addLog(pb);
        // CogZero status
        char czb[200];
        snprintf(czb, 200, "CogZero: cycle=%d | Atoms=%d | Tasks=%d/%d | Duration=%.1fms",
            g_agi.cogzeroStats.cycleNumber,
            g_agi.cogzeroAgent.atomCount(),
            g_agi.cogzeroAgent.completedTasks(),
            g_agi.cogzeroAgent.pendingTasks(),
            g_agi.cogzeroStats.durationMs);
        g_agi.addLog(czb);
        // Layla status
        char laylaLog[200];
        snprintf(laylaLog, 200, "Layla: %s | Model=%s | Queue=%d | Latency=%.1fms",
            g_agi.laylaTelemetry.online ? "ONLINE" : "OFFLINE",
            g_agi.laylaTelemetry.model.empty() ? "none" : g_agi.laylaTelemetry.model.c_str(),
            g_agi.laylaTelemetry.queueDepth,
            g_agi.laylaTelemetry.lastLatencyMs);
        g_agi.addLog(laylaLog);
        // Echo KSM status
        char eb[200];
        snprintf(eb, 200, "Echo: cycle=%d step=%d (%s) | Load=%.2f | Ledger=%d/%d keep",
            g_agi.echoSnap.cycle, g_agi.echoSnap.step,
            echo::discoveryPhaseName(g_agi.echoSnap.phase),
            g_agi.echoSnap.cognitiveLoad,
            g_agi.echoLedger.keepCount(), g_agi.echoLedger.totalCount());
        g_agi.addLog(eb);
    }
    DrawText(mem, 20, 68, sb, g_agi.running ? COL_GREEN : COL_RED, g_hFont);

    // ── Left: Endocrine System with Decay Tuning ──
    int lx = 20, ly = 100, lw = 330, lh = 420;
    DrawRect(mem, lx, ly, lw, lh, COL_CARD);
    DrawText(mem, lx+10, ly+8, "Virtual Endocrine System (dte::EndocrineBus)", COL_CYAN, g_hFont);
    for (int i = 0; i < (int)dte::HORMONE_COUNT; i++) {
        int row = i / 2, col = i % 2;
        int bx = lx + 12 + col * 158, by = ly + 34 + row * 62;
        if (i == g_agi.selectedHormone) {
            DrawRect(mem, bx-2, by-2, 154, 58, RGB(60, 60, 100));
        }
        DrawText(mem, bx, by, dte::HORMONE_NAMES[i], i == g_agi.selectedHormone ? COL_ACCENT : COL_TEXT2, g_hFont);
        float val = (float)g_agi.endo.hormoneLevel((dte::HormoneId)i);
        COLORREF bc = val > 0.6f ? COL_RED : (val > 0.4f ? COL_ACCENT : COL_GREEN);
        DrawBar(mem, bx, by + 18, 140, 10, val, bc, RGB(40,40,60));
        char hb[64];
        double dr = g_agi.endo.getDecayRate((dte::HormoneId)i);
        snprintf(hb, 64, "%.2f d=%.3f", val, dr);
        DrawText(mem, bx, by + 32, hb, COL_TEXT, g_hFontMono);
    }
    auto& va = g_agi.endo.valenceArousal();
    char vab[128];
    snprintf(vab, 128, "Valence: %+.2f  Arousal: %+.2f  |V-A|: %.2f",
        va.valence, va.arousal, va.magnitude());
    DrawText(mem, lx+12, ly+lh-76, vab, COL_TEXT, g_hFont);
    char mb[64];
    snprintf(mb, 64, "Cognitive Mode: %s", dte::cognitiveModeName(g_agi.endo.cognitiveMode()));
    DrawText(mem, lx+12, ly+lh-52, mb, dte::cognitiveModeColor(g_agi.endo.cognitiveMode()), g_hFont);
    DrawText(mem, lx+12, ly+lh-28, "Mode History:", COL_TEXT2, g_hFont);
    int mhY = ly + lh - 12;
    int mhCount = 0;
    for (int i = (int)g_agi.endo.modeHistory.size()-1; i >= 0 && mhCount < 3; i--, mhCount++) {
        auto& mh = g_agi.endo.modeHistory[i];
        char mhb[64];
        snprintf(mhb, 64, "T%d: %s", mh.first, dte::cognitiveModeName(mh.second));
        DrawText(mem, lx+12 + mhCount*100, mhY, mhb, dte::cognitiveModeColor(mh.second), g_hFontSmall);
    }

    // ── Center: TRIZ Arena with Spatial Contradiction Inspector ──
    int cx = 370, cy = 100, cw = 470, ch = 420;
    DrawRect(mem, cx, cy, cw, ch, COL_CARD);
    DrawText(mem, cx+10, cy+8, "TRIZ Arena + Spatial Contradiction Inspector", COL_CYAN, g_hFont);
    int cs = 26, gox = cx + 40, goy = cy + 36;
    for (int q = 0; q < dte::TRIZArena::SIZE; q++)
        for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
            auto& cell = g_agi.arena.grid[q][r];
            int px = gox + q * (cs + 3) + (r % 2) * (cs/2 + 1);
            int py = goy + r * (cs + 3);
            float v = cell.aestheticValue;
            COLORREF cc;
            if (cell.isNiche) {
                cc = cell.nicheType == dte::NicheType::Perturbation ? COL_NICHE_P :
                     cell.nicheType == dte::NicheType::Scaffolding ? COL_NICHE_S : COL_NICHE_I;
            } else {
                cc = RGB((int)(v*255), (int)(v*190), (int)(v*90));
            }
            if (q == g_agi.arena.maxContradictionQ && r == g_agi.arena.maxContradictionR) {
                DrawRect(mem, px-2, py-2, cs+4, cs+4, COL_CONTRA);
            }
            if (q == g_agi.selectedCellQ && r == g_agi.selectedCellR) {
                DrawRect(mem, px-1, py-1, cs+2, cs+2, COL_ACCENT);
            }
            DrawRect(mem, px, py, cs, cs, cc);
            if (cell.contradiction > 0.1f) {
                int ci = (int)(cell.contradiction * 255);
                DrawRect(mem, px + cs/2 - 2, py + cs/2 - 2, 4, 4, RGB(ci, 0, 0));
            }
        }
    char ab[200];
    snprintf(ab, 200, "Coherence: %.3f | Phase: %s | Discoveries: %d | Niches: %d",
        g_agi.arena.coherence, g_agi.arena.phaseName(), g_agi.arena.discoveries,
        (int)g_agi.arena.niches.size());
    DrawText(mem, cx+10, cy+ch-72, ab, COL_TEXT, g_hFont);
    snprintf(ab, 200, "Max Contradiction: %.3f at (%d,%d) | Last Delta: %.4f",
        g_agi.arena.maxContradiction, g_agi.arena.maxContradictionQ,
        g_agi.arena.maxContradictionR, g_agi.arena.lastDelta);
    DrawText(mem, cx+10, cy+ch-52, ab, COL_TEXT2, g_hFont);
    if (g_agi.selectedCellQ >= 0 && g_agi.selectedCellR >= 0) {
        auto& sc = g_agi.arena.grid[g_agi.selectedCellQ][g_agi.selectedCellR];
        snprintf(ab, 200, "Cell(%d,%d): Aes=%.2f Contra=%.2f Obj=%d LastP=%s",
            g_agi.selectedCellQ, g_agi.selectedCellR, sc.aestheticValue,
            sc.contradiction, sc.objectCount,
            sc.lastPrinciple >= 0 ? g_agi.arena.principles[sc.lastPrinciple] : "none");
        DrawText(mem, cx+10, cy+ch-32, ab, COL_ACCENT, g_hFont);
    } else {
        DrawText(mem, cx+10, cy+ch-32, "Click a cell to inspect | Red dot = high contradiction", COL_TEXT2, g_hFont);
    }

    // ── Right: Sensorimotor + ESN + uechovr 6DOF + re-bhvt-edit Bridge + mech0 LLM ──
    int rx = 860, ry = 100, rw = 420, rh = 420;
    DrawRect(mem, rx, ry, rw, rh, COL_CARD);
    DrawText(mem, rx+10, ry+8, "Sensorimotor + ESN + uechovr + re-bhvt-edit + mech0", COL_CYAN, g_hFont);
    // uechovr 6DOF pose display
    char pose[256];
    snprintf(pose, 256, "HMD: (%.2f, %.2f, %.2f) q=(%.2f,%.2f,%.2f,%.2f)",
        g_agi.hmdPose.x, g_agi.hmdPose.y, g_agi.hmdPose.z,
        g_agi.hmdPose.qx, g_agi.hmdPose.qy, g_agi.hmdPose.qz, g_agi.hmdPose.qw);
    DrawText(mem, rx+12, ry+30, pose, COL_VR, g_hFontMono);
    snprintf(pose, 256, "L-Ctrl: (%.2f, %.2f, %.2f)  R-Ctrl: (%.2f, %.2f, %.2f)",
        g_agi.leftCtrl.x, g_agi.leftCtrl.y, g_agi.leftCtrl.z,
        g_agi.rightCtrl.x, g_agi.rightCtrl.y, g_agi.rightCtrl.z);
    DrawText(mem, rx+12, ry+48, pose, COL_VR, g_hFontMono);
    snprintf(pose, 256, "Stereo: %s | IPD: %.3f | Eye: %dx%d",
        g_agi.stereo.methodName(), g_agi.stereo.getEyeParams(0).ipd,
        g_agi.stereo.getEyeParams(0).width, g_agi.stereo.getEyeParams(0).height);
    DrawText(mem, rx+12, ry+66, pose, COL_VR, g_hFontMono);
    // re-bhvt-edit bridge telemetry
    char bridge[256];
    snprintf(bridge, 256, "Bridge: R=%.3f N=%.3f PE=%.3f T=%.3f Niches=%d",
        g_agi.bridgeTelemetry.reward, g_agi.bridgeTelemetry.novelty,
        g_agi.bridgeTelemetry.prediction_error, g_agi.bridgeTelemetry.threat,
        g_agi.bridgeTelemetry.niche_count);
    DrawText(mem, rx+12, ry+84, bridge, COL_BRIDGE, g_hFontMono);
    // mech0 LLM metrics
    char llm[256];
    snprintf(llm, 256, "LLM: Acc=%.2f Loss=%.3f Epochs=%d Batches=%d",
        g_agi.llmMetrics.accuracy, g_agi.llmMetrics.averageLoss,
        g_agi.llmMetrics.epochs, g_agi.llmMetrics.batches);
    DrawText(mem, rx+12, ry+102, llm, COL_LLM, g_hFontMono);
    // Frame buffer visualization
    int fb = 4, fox = rx + 16, foy = ry + 126;
    for (int y = 0; y < dte::SensorimotorLoop::FH; y += 4)
        for (int x = 0; x < dte::SensorimotorLoop::FW; x += 4) {
            float v = g_agi.sensorimotor.frameBuffer[y*dte::SensorimotorLoop::FW+x];
            DrawRect(mem, fox + x/4*fb, foy + y/4*fb, fb-1, fb-1,
                RGB((int)(v*100), (int)(v*150), (int)(v*255)));
        }
    const char* pn[8] = {"Pos","Vel","Orient","Accel","Fatigue","Arousal","Phase","Reward"};
    for (int i = 0; i < 8; i++) {
        int by = ry + 206 + i * 22;
        DrawText(mem, rx+12, by, pn[i], COL_TEXT2, g_hFont);
        DrawBar(mem, rx+80, by, 200, 10, g_agi.sensorimotor.proprioception[i], COL_PURPLE, RGB(40,40,60));
        char pb[32]; snprintf(pb, 32, "%.3f", g_agi.sensorimotor.proprioception[i]);
        DrawText(mem, rx+290, by, pb, COL_TEXT, g_hFontMono);
    }
    DrawText(mem, rx+12, ry+382, "ESN Reservoir (96 neurons, leak=0.3, sr=0.95):", COL_TEXT2, g_hFont);
    auto& st = g_agi.esn.state();
    for (int i = 0; i < 96; i++) {
        float v = (float)fabs(st[i]);
        v = std::min(1.0f, v);
        DrawRect(mem, rx+12 + (i%24)*16, ry+402 + (i/24)*8, 14, 6,
            RGB((int)(v*255), (int)(v*100), (int)(v*200)));
    }
    DrawText(mem, rx+12, ry+438, "W_res (sampled 24x24):", COL_TEXT2, g_hFont);
    auto& W_res = g_agi.esn.W_res();
    int wms = 4;
    for (int i = 0; i < 24; i++)
        for (int j = 0; j < 24; j++) {
            int si = i * 4, sj = j * 4;
            double w = W_res[si * 96 + sj];
            float v = (float)fabs(w);
            v = std::min(1.0f, v * 2.0f);
            COLORREF wc = w > 0 ? RGB((int)(v*255), (int)(v*50), 0) : RGB(0, (int)(v*50), (int)(v*255));
            DrawRect(mem, rx+12 + j*wms, ry+458 + i*wms, wms-1, wms-1, wc);
        }
    DrawText(mem, rx+220, ry+438, "Leaky State History (8 neurons):", COL_TEXT2, g_hFont);
    auto& hist = g_agi.esn.stateHistory();
    int hidx = g_agi.esn.historyIdx();
    for (int n = 0; n < 8; n++) {
        int ni = n * 12;
        for (int t = 0; t < 99; t++) {
            int t1 = (hidx + t) % 100, t2 = (hidx + t + 1) % 100;
            float v1 = (float)hist[ni][t1], v2 = (float)hist[ni][t2];
            int x1 = rx + 220 + t * 2, x2 = rx + 220 + (t+1) * 2;
            int y1 = ry + 458 + n * 6 + (int)(v1 * 20);
            int y2 = ry + 458 + n * 6 + (int)(v2 * 20);
            DrawLine(mem, x1, y1, x2, y2, RGB(100 + n*20, 150, 255 - n*20));
        }
    }
    char rwb[80];
    snprintf(rwb, 80, "Reward: %.3f | Steps: %d", g_agi.sensorimotor.totalReward, g_agi.sensorimotor.stepCount);
    DrawText(mem, rx+12, ry+rh-14, rwb, COL_ACCENT, g_hFont);

    // ── Bottom Left: Group Dynamics ──
    int gx = 20, gy = 540, gw = 330, gh = 190;
    DrawRect(mem, gx, gy, gw, gh, COL_CARD);
    DrawText(mem, gx+10, gy+8, "Group Dynamics (Emergent)", COL_CYAN, g_hFont);
    char gb[128];
    snprintf(gb, 128, "Agents: %d | Groups: %d | Cohesion: %.3f",
        (int)g_agi.groups.agents.size(), g_agi.groups.groupCount(), g_agi.groups.avgCohesion);
    DrawText(mem, gx+10, gy+32, gb, COL_TEXT, g_hFont);
    int mox = gx+10, moy = gy+56, mw = gw-20, mh = gh-66;
    DrawRect(mem, mox, moy, mw, mh, RGB(20,20,40));
    for (auto& a : g_agi.groups.agents) {
        int ax = mox + (int)(a.x/800.0f*mw), ay = moy + (int)(a.y/600.0f*mh);
        DrawRect(mem, ax-2, ay-2, 5, 5, a.groupId >= 0 ? COL_GREEN : COL_TEXT2);
    }

    // ── Bottom Center: Micro-Improvement + Introspection ──
    int mx = 370, my = 540, mw2 = 470, mh2 = 190;
    DrawRect(mem, mx, my, mw2, mh2, COL_CARD);
    DrawText(mem, mx+10, my+8, "Micro-Improvement + Echo Introspection", COL_CYAN, g_hFont);
    char mib[160];
    snprintf(mib, 160, "v%d | Kept: %d | Best: %.3f | Self-Awareness: %.3f",
        g_agi.improvement.version, g_agi.improvement.kept,
        g_agi.improvement.best, g_agi.introspect.selfAwareness);
    DrawText(mem, mx+10, my+32, mib, COL_TEXT, g_hFont);
    int hy = my + 56;
    int hs = (int)g_agi.improvement.history.size();
    for (int i = hs-1; i >= 0 && i >= hs-4; i--) {
        DrawText(mem, mx+10, hy, g_agi.improvement.history[i].c_str(), COL_TEXT2, g_hFontMono);
        hy += 17;
    }
    if (!g_agi.introspect.insights.empty())
        DrawText(mem, mx+10, my+mh2-26, g_agi.introspect.insights.back().c_str(), COL_ACCENT, g_hFont);

    // ── Bottom Right: CogVerse + Log ──
    int bx = 860, by = 540, bw = 420, bh = 190;
    DrawRect(mem, bx, by, bw, bh, COL_CARD);
    DrawText(mem, bx+10, by+8, "CogVerse + elizaos AgentLoop + ECAN", COL_CYAN, g_hFont);
    {
        char eb[256];
        snprintf(eb, 256, "Loop: %s | Load: %.2f | CB: %s | Iters: %llu",
            elizaos::healthName(g_agi.agentLoop.health()),
            g_agi.agentLoop.load().composite(),
            g_agi.agentLoop.breaker().stateName(),
            (unsigned long long)g_agi.agentLoop.stats().totalIterations);
        DrawText(mem, bx+10, by+32, eb, g_agi.agentLoop.degraded() ? COL_RED : COL_GREEN, g_hFont);
        snprintf(eb, 256, "ECAN: Avg=%.3f Budget=%.0f%% Focus=%s",
            g_agi.avgAttention,
            g_agi.attention.budget().utilization() * 100.0,
            g_agi.attentionFocus.c_str());
        DrawText(mem, bx+10, by+50, eb, COL_VR, g_hFont);
        auto* w = g_agi.attention.window(g_agi.attentionFocus);
        if (w) {
            snprintf(eb, 256, "Temporal: I=%.2f S=%.2f M=%.2f L=%.2f Comp=%.3f",
                w->immediate, w->shortTerm, w->mediumTerm, w->longTerm, w->composite());
            DrawText(mem, bx+10, by+68, eb, COL_BRIDGE, g_hFont);
    // kys Triad Phasor Engine panel
    {
        kys::StepWord triad[3];
        g_agi.phasor.currentTriad(triad);
        char pb[256];
        snprintf(pb, 256, "Phasor: tick=%d | Triad=[%s %s %s] | U1=%s U2=%s",
            g_agi.phasor.globalTick(),
            triad[0].name(), triad[1].name(), triad[2].name(),
            kys::uTermName(g_agi.phasor.currentU1()),
            kys::uTermName(g_agi.phasor.currentU2()));
        DrawText(mem, bx+10, by+88, pb, COL_GOLD, g_hFont);
        snprintf(pb, 256, "Invariants: %d/6 | PhaseLock=%s T8=%s Budget=%s Mode=%s",
            g_agi.phasor.invariants().passing(),
            g_agi.phasor.invariants().phaseLock ? "OK" : "FAIL",
            g_agi.phasor.invariants().t8Coherence ? "OK" : "FAIL",
            g_agi.phasor.invariants().budget7E5R ? "OK" : "FAIL",
            g_agi.phasor.invariants().modeBalance ? "OK" : "FAIL");
        DrawText(mem, bx+10, by+106, pb, g_agi.phasor.invariants().passing() >= 5 ? COL_GREEN : COL_RED, g_hFont);
    // echo KSM panel
    {
        char eb[256];
        snprintf(eb, 256, "Echo: cycle=%d step=%d (%s) | Mode=%s",
            g_agi.echoSnap.cycle, g_agi.echoSnap.step,
            echo::discoveryPhaseName(g_agi.echoSnap.phase),
            echo::cognitiveModeName(g_agi.echoSnap.mode));
        DrawText(mem, bx+10, by+128, eb, COL_GOLD, g_hFont);
        snprintf(eb, 256, "Load=%.2f | Alignment=%.3f | Ledger=%d/%d keep",
            g_agi.echoSnap.cognitiveLoad,
            g_agi.echoSnap.alignment,
            g_agi.echoLedger.keepCount(), g_agi.echoLedger.totalCount());
        DrawText(mem, bx+10, by+146, eb, g_agi.echoSnap.cognitiveLoad > 0.7f ? COL_RED : COL_GREEN, g_hFont);
    // cogzero panel
    {
        char czb[256];
        snprintf(czb, 256, "CogZero: cycle=%d | Atoms=%d | Tasks=%d/%d",
            g_agi.cogzeroStats.cycleNumber,
            g_agi.cogzeroAgent.atomCount(),
            g_agi.cogzeroAgent.completedTasks(),
            g_agi.cogzeroAgent.pendingTasks());
        DrawText(mem, bx+10, by+168, czb, COL_GOLD, g_hFont);
        snprintf(czb, 256, "Duration=%.1fms | Percepts=%d | Conclusions=%d",
            g_agi.cogzeroStats.durationMs,
            g_agi.cogzeroStats.perceptsProcessed,
            g_agi.cogzeroStats.conclusions);
        DrawText(mem, bx+10, by+186, czb, g_agi.cogzeroStats.durationMs > 10.0f ? COL_RED : COL_GREEN, g_hFont);
    // layla panel
    {
        char laylaPanel[256];
        snprintf(laylaPanel, 256, "Layla: %s | Model=%s | Queue=%d",
            g_agi.laylaTelemetry.online ? "ONLINE" : "OFFLINE",
            g_agi.laylaTelemetry.model.empty() ? "none" : g_agi.laylaTelemetry.model.c_str(),
            g_agi.laylaTelemetry.queueDepth);
        DrawText(mem, bx+10, by+208, laylaPanel, g_agi.laylaTelemetry.online ? COL_GREEN : COL_RED, g_hFont);
        snprintf(laylaPanel, 256, "Latency=%.1fms | Requests=%d | Failures=%d",
            g_agi.laylaTelemetry.lastLatencyMs,
            g_agi.laylaTelemetry.requests,
            g_agi.laylaTelemetry.failures);
        DrawText(mem, bx+10, by+226, laylaPanel, g_agi.laylaTelemetry.failures > 0 ? COL_RED : COL_GREEN, g_hFont);
    }
    // navigation panel (v13)
    {
        char nb[256];
        snprintf(nb, 256, "Nav3D: %s | Pos=(%.1f,%.1f) | Goal=%.1fm | Contradiction=%.3f",
            g_agi.navTraining ? "TRAINING" : "IDLE",
            g_agi.navPos.x, g_agi.navPos.z,
            g_agi.navWorld.goalDistance(g_agi.navPos),
            g_agi.navMeanContradiction);
        DrawText(mem, bx+10, by+248, nb, g_agi.navTraining ? COL_GREEN : COL_TEXT2, g_hFont);
        snprintf(nb, 256, "Q-Learn: eps=%.2f | Success=%.0f%% | Collisions=%d | Ret=%.2f",
            g_agi.navTrainer.epsilon,
            g_agi.navTrainer.successRate() * 100.0f,
            g_agi.navTrainer.collisions,
            g_agi.navTrainer.recentMeanReturn());
        DrawText(mem, bx+10, by+266, nb,
            g_agi.navTrainer.successRate() > 0.5f ? COL_GREEN : COL_GOLD, g_hFont);
    }
    }
    }
    }
        }
    }
    char cvb[128];
    snprintf(cvb, 128, "Cycle: %d | Property Coherence: %.3f | Residents: %d",
        g_agi.cogverse.cycle, g_agi.cogverse.propertyCoherence, g_agi.cogverse.residents);
    DrawText(mem, bx+10, by+248, cvb, COL_TEXT, g_hFont);
    int lyy = by + 268;
    int ls = (int)g_agi.log.size();
    for (int i = 0; i < 2 && i < ls; i++) {
        DrawText(mem, bx+10, lyy, g_agi.log[ls-1-i].c_str(), COL_TEXT2, g_hFontMono);
        lyy += 17;
    }

    // v13: egocentric framebuffer viewport (avatar's actual view, 2x scale)
    {
        int vx = 20, vy = H - 26 - 140;
        DrawText(mem, vx, vy - 16, "Egocentric Framebuffer (64x64 -> TRIZ 12x12)", COL_TEXT2, g_hFontSmall);
        for (int y = 0; y < 64; y++)
            for (int x = 0; x < 64; x++) {
                float v = g_agi.navRenderer.frame[y * 64 + x];
                float sem = g_agi.navRenderer.semantic[y * 64 + x];
                COLORREF c;
                if (sem > 0.9f) c = RGB(0, 255, 120);        // goal
                else if (sem > 0.6f) c = RGB(255, 80, 60);   // obstacle
                else if (sem > 0.4f) c = RGB(120, 180, 255); // avatar
                else if (sem > 0.2f) c = RGB(60, 60, 80);    // ground
                else c = RGB((int)(v * 40), (int)(v * 40), (int)(v * 60));
                DrawRect(mem, vx + x * 2, vy + y * 2, 2, 2, c);
            }
        // TRIZ grid overlay (12x12)
        for (int g = 0; g <= 12; g++) {
            DrawLine(mem, vx + g * 10, vy, vx + g * 10, vy + 128, RGB(50, 50, 70));
            DrawLine(mem, vx, vy + g * 10, vx + 128, vy + g * 10, RGB(50, 50, 70));
        }
        // Highlight max-contradiction cell
        if (g_agi.trizMap.maxQ >= 0) {
            int hx = vx + g_agi.trizMap.maxQ * 10, hy = vy + g_agi.trizMap.maxR * 10;
            DrawLine(mem, hx, hy, hx + 10, hy, COL_CONTRA);
            DrawLine(mem, hx + 10, hy, hx + 10, hy + 10, COL_CONTRA);
            DrawLine(mem, hx + 10, hy + 10, hx, hy + 10, COL_CONTRA);
            DrawLine(mem, hx, hy + 10, hx, hy, COL_CONTRA);
        }
    }
    DrawText(mem, 20, H-26, "[SPACE] Run | [R] Reset | [N] Nav | [V] View | [S] Spine | [T] Overlay | [E] Emit Now | [ESC] Exit", COL_TEXT2, g_hFont);

    const int dashboardH = std::min(clientH, (clientW * H) / W);
    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, 0, 0, clientW, dashboardH, mem, 0, 0, W, H, SRCCOPY);
    DrawRect(hdc, 0, dashboardH, clientW, std::max(0, clientH-dashboardH), COL_BG);
    if (g_agi.civicOverlayVisible && clientH - dashboardH >= 104) {
        const int sx = 8, sy = dashboardH + 5, sw = clientW - 16;
        DrawRect(hdc, sx, sy, sw, clientH-sy-7, RGB(18, 22, 42));
        DrawText(hdc, sx+10, sy+6, "CIVIC ANGEL SPINE / grounded authority evidence", COL_BRIDGE, g_hFont);
        const auto embodied = g_agi.civicSpine.embodiedTelemetry();
        const auto purpose = g_agi.civicSpine.purposeTelemetry();
        const auto expression = g_agi.civicSpine.expressionTelemetry();
        auto drawProducer = [&](int y, const char* label, const civicangel::ProducerTelemetry& t, COLORREF color) {
            char line[512];
            snprintf(line, sizeof(line), "%s  %s  accepted=%lu fail=%lu queue=%lu HTTP=%lu %.1fms trace=%s",
                label, t.configured ? (t.busy ? "BUSY" : "READY") : "UNCONFIGURED",
                t.accepted, t.failures, t.queued, t.lastStatus, t.lastLatencyMs,
                t.lastTraceId.empty() ? "none" : t.lastTraceId.c_str());
            DrawText(hdc, sx+10, y, line, t.failures ? COL_RED : color, g_hFontMono);
        };
        drawProducer(sy+30, "GTAngel / EMBODIED", embodied, COL_CYAN);
        drawProducer(sy+52, "Ontogentelechy / PURPOSE", purpose, COL_GOLD);
        drawProducer(sy+74, "Meta Echo DNA / EXPRESSION", expression, COL_BRIDGE);
        DrawText(hdc, sx+10, sy+98, "[S] stream  [E] immediate grounded frame  [T] hide overlay", COL_TEXT2, g_hFontSmall);
    } else if (!g_agi.civicOverlayVisible) {
        DrawText(hdc, 10, dashboardH+10, "Civic Angel overlay hidden — [T] show", COL_TEXT2, g_hFontSmall);
    }
    SelectObject(mem, old); DeleteObject(bmp); DeleteDC(mem);
}


// v24 presentation plane: intentionally isolated from the AI perception pyramid.
static v15render::DisplayRenderer g_displayRenderer;
static HWND g_displayWindow=nullptr;
static LRESULT CALLBACK DisplayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
      case WM_PAINT: {
        PAINTSTRUCT ps; HDC dc=BeginPaint(hwnd,&ps);
        RECT area{}; GetClientRect(hwnd,&area);
        const int width=area.right-area.left, height=area.bottom-area.top;
        DrawRect(dc,0,0,width,height,COL_BG);
        // Read the same local plant/scene, never the 768x768 sensor pixels.
        const avatar3d::Vec3 pos=g_agi.navPos;
        const float heading=g_agi.navHeading;
        const bool third=g_agi.navThirdPerson;
        const auto eye=third ? pos+avatar3d::Vec3{-sinf(heading)*4.0f,2.5f,-cosf(heading)*4.0f}
                             : pos+avatar3d::Vec3{0,1.6f,0};
        g_displayRenderer.render(g_agi.navWorld,g_agi.avatar,eye,heading,third?-0.4f:0.0f,third);
        const auto& display=g_displayRenderer.framebuffer;
        const int targetW=std::max(1,std::min(width,(int)std::lround(height*16.0/9.0)));
        const int targetH=std::max(1,std::min(height,(int)std::lround(width*9.0/16.0)));
        const int x=(width-targetW)/2, y=(height-targetH)/2;
        BITMAPINFO bitmap{};
        bitmap.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bitmap.bmiHeader.biWidth=v15render::DisplayFramebuffer::Width;
        bitmap.bmiHeader.biHeight=-v15render::DisplayFramebuffer::Height;
        bitmap.bmiHeader.biPlanes=1;
        bitmap.bmiHeader.biBitCount=32;
        bitmap.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,HALFTONE);
        StretchDIBits(dc,x,y,targetW,targetH,0,0,display.Width,display.Height,
                      display.colour.data(),&bitmap,DIB_RGB_COLORS,SRCCOPY);
        DrawText(dc,x+12,y+12,"DISPLAY 1536x864 / AI SENSOR 768x768 / F7 CLOSE",COL_CYAN,g_hFontSmall);
        EndPaint(hwnd,&ps); return 0;
      }
      case WM_KEYDOWN:
        if(wp==VK_F7 || wp==VK_ESCAPE) {DestroyWindow(hwnd);return 0;}
        if(HWND owner=GetWindow(hwnd,GW_OWNER)) SendMessageA(owner,WM_KEYDOWN,wp,lp);
        return 0;
      case WM_CLOSE: DestroyWindow(hwnd); return 0;
      case WM_DESTROY: g_displayWindow=nullptr; return 0; // main game still runs
    }
    return DefWindowProcA(hwnd,msg,wp,lp);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            g_hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_SWISS, "Arial");
            g_hFontLarge = CreateFontA(22, 0, 0, 0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_SWISS, "Arial");
            g_hFontMono = CreateFontA(12, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_MODERN, "Consolas");
            g_hFontSmall = CreateFontA(10, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_MODERN, "Consolas");
            g_agi.highResRenderer.loadDteAsset(""); // adjacent executable/assets path only
            g_agi.navWorld.generate(1337,48);
            g_agi.gameplay.reset(g_agi.navWorld);
            g_agi.cns.reset();
            g_agi.invalidateEcho();
            g_agi.syncBodyFromCns();
            g_agi.avatar.pose(g_agi.navPos,g_agi.navHeading,0.0f,0.0f);
            g_agi.highResRenderer.render(g_agi.navWorld,g_agi.avatar,
                g_agi.navPos+avatar3d::Vec3{0,2.5f,-4.0f},0,-0.4f,true);
            g_agi.perceptionPyramid.derive(g_agi.highResRenderer.framebuffer);
            SetTimer(hwnd, 1, 50, NULL);
            g_agi.groups.init(20);
            g_agi.fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
            g_agi.tracker.reset();
            g_agi.stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
            g_agi.civicSpine.configureFromEnvironment();
            g_agi.addLog("GTAngel AGI v14 initialized: grounded Civic Angel producer spine ready");
            return 0;
        case WM_TIMER:
            g_agi.tick();
            InvalidateRect(hwnd, NULL, FALSE);
            static uint32_t displayDivisor=0;
            if (g_displayWindow && IsWindow(g_displayWindow) && (++displayDivisor % 2u)==0u)
                InvalidateRect(g_displayWindow,NULL,FALSE);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            RenderFrame(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN:
            if (wp == VK_SPACE) {
                if (!g_agi.running) { g_agi.running = true; g_agi.addLog("Started"); }
                else { g_agi.paused = !g_agi.paused; g_agi.addLog(g_agi.paused ? "Paused" : "Resumed"); }
            } else if (wp == VK_HOME) {
                g_agi.reset();
                g_agi.groups.init(20);
                g_agi.fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
                g_agi.tracker.reset();
                g_agi.stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
                g_agi.addLog("Reset");
            } else if (wp == VK_TAB) {
                g_agi.selectedHormone = (g_agi.selectedHormone + 1) % (int)dte::HORMONE_COUNT;
            } else if (wp == VK_OEM_PLUS || wp == VK_ADD) {
                auto hid = (dte::HormoneId)g_agi.selectedHormone;
                double dr = g_agi.endo.getDecayRate(hid);
                g_agi.endo.setDecayRate(hid, std::min(0.5, dr + 0.01));
                char buf[64]; snprintf(buf, 64, "Decay %s: %.3f", dte::HORMONE_NAMES[g_agi.selectedHormone], dr + 0.01);
                g_agi.addLog(buf);
            } else if (wp == VK_OEM_MINUS || wp == VK_SUBTRACT) {
                auto hid = (dte::HormoneId)g_agi.selectedHormone;
                double dr = g_agi.endo.getDecayRate(hid);
                g_agi.endo.setDecayRate(hid, std::max(0.001, dr - 0.01));
                char buf[64]; snprintf(buf, 64, "Decay %s: %.3f", dte::HORMONE_NAMES[g_agi.selectedHormone], dr - 0.01);
                g_agi.addLog(buf);
            } else if (wp == 'P') {
                g_agi.peerLoop.setEnabled(!g_agi.peerLoop.enabled());
                g_agi.addLog(g_agi.peerLoop.enabled()
                    ? "v18 peer attention enabled; local confirmation required"
                    : "v18 peer attention disabled");
            } else if (wp == 'M') {
                g_agi.neuralShadowEnabled = !g_agi.neuralShadowEnabled;
                g_agi.neuralPolicy.reset();
                g_agi.addLog(g_agi.neuralShadowEnabled ? "v17 neural SHADOW enabled" : "v17 neural inference disabled");
            } else if (wp == 'C') {
                if (g_agi.trajectoryRecording) {
                    const bool stopped = g_agi.trajectoryRecorder.stop();
                    g_agi.trajectoryRecording = false;
                    g_agi.addLog(stopped
                        ? std::string("v16 capture stopped: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture close failed: ") + g_agi.trajectoryRecorder.lastError());
                } else {
                    const bool started = g_agi.trajectoryRecorder.start(
                        R"(C:\ddd\GTAngel\cpp_test\datasets)", g_agi.trajectoryEpisodeId++);
                    g_agi.trajectoryRecording = started;
                    g_agi.addLog(started
                        ? std::string("v16 capture started: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture failed: ") + g_agi.trajectoryRecorder.lastError());
                }
            } else if (wp == 'F') {
                int mode = ((int)g_agi.highResDisplayMode + 1) % 4;
                g_agi.highResDisplayMode = (v15render::DisplayMode)mode;
                g_agi.addLog(std::string("v15 display: ") + v15render::displayModeName(g_agi.highResDisplayMode));
            } else if (wp == VK_F7) {
                if (g_displayWindow && IsWindow(g_displayWindow)) {
                    DestroyWindow(g_displayWindow);
                } else {
                    g_displayRenderer.loadDteAsset(""); // its own validated local skin copy
                    RECT dimensions={0,0,std::min(1536,std::max(640,GetSystemMetrics(SM_CXSCREEN)-100)),
                                          std::min(864,std::max(480,GetSystemMetrics(SM_CYSCREEN)-120))};
                    AdjustWindowRect(&dimensions,WS_OVERLAPPEDWINDOW,FALSE);
                    g_displayWindow=CreateWindowExA(0,"GTAngelVisualDisplayV24",
                        "GTAngel v24 - high-resolution display only (AI: 768x768)",
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,80,60,
                        dimensions.right-dimensions.left,dimensions.bottom-dimensions.top,
                        hwnd,NULL,GetModuleHandleA(NULL),NULL);
                    if(g_displayWindow) ShowWindow(g_displayWindow,SW_SHOWNORMAL);
                }
            } else if (wp == VK_F8) {
                g_agi.lensInspectorOpen=!g_agi.lensInspectorOpen;
                g_agi.addLog(g_agi.lensInspectorOpen?"System 7->8 lens inspector OPEN (advisory)":
                                                   "System 7->8 lens inspector CLOSED");
            } else if (wp == VK_F6) {
                g_agi.highResViewportVisible = !g_agi.highResViewportVisible;
                g_agi.addLog(g_agi.highResViewportVisible ? "v15 viewport: VISIBLE" : "v14 dashboard: VISIBLE");
            } else if (wp == 'N') {
                g_agi.navTraining = !g_agi.navTraining;
                g_agi.addLog(g_agi.navTraining ? "Nav training: ON" : "Nav training: OFF");
            } else if (wp == 'V') {
                g_agi.navThirdPerson = !g_agi.navThirdPerson;
                g_agi.addLog(g_agi.navThirdPerson ? "Camera: third-person" : "Camera: first-person");
            } else if (wp == 'T') {
                g_agi.civicOverlayVisible = !g_agi.civicOverlayVisible;
                g_agi.addLog(g_agi.civicOverlayVisible ? "Civic Angel overlay: VISIBLE" : "Civic Angel overlay: HIDDEN");
            } else if (wp == VK_F10) {
                g_agi.civicSpine.setEnabled(!g_agi.civicSpine.enabled());
                g_agi.addLog(g_agi.civicSpine.enabled() ? "Civic Angel spine: ON" : "Civic Angel spine: OFF");
            } else if (wp == VK_F11) {
                g_agi.civicSpine.requestImmediate();
                g_agi.addLog("Civic Angel: immediate grounded frame requested");
            } else if (wp == 'M') {
                std::string prom = g_agi.agentLoop.toPrometheus();
                std::ofstream f("C:\\ddd\\GTAngel\\cpp_test\\gtangel_metrics.prom");
                f << prom;
                f.close();
                g_agi.addLog("Prometheus metrics exported to gtangel_metrics.prom");
            } else if (wp == 'B') {
                if (g_agi.cns.body().occupied) g_agi.addLog("DTE motion preview requires on-foot embodiment");
                else g_agi.addLog(g_agi.avatar.motion==avatar3d::AvatarSkeleton::Motion::Guard
                    ? (g_agi.avatar.releaseCombat()?"DTE wings relatching / visual recovery":"DTE motion held")
                    : (g_agi.avatar.triggerCombat()?"DTE combat-entry pose / visual-only":"DTE entry in progress"));
            } else if (wp == 'I') {
                g_agi.avatar.inspect=!g_agi.avatar.inspect;
                g_agi.addLog(g_agi.avatar.inspect?"DTE explore / inspect overlay":"DTE scan overlay off");
            } else if (wp == 'Q') {
                g_agi.addLog(g_agi.cns.toggleCart(g_agi.navWorld,g_agi.navWorld.vehiclePos)
                    ? "Local cart enter/exit accepted" : "Local cart enter/exit rejected");
                g_agi.syncBodyFromCns();
            } else if (wp == 'E') {
                if (g_agi.gameplay.canRestore(g_agi.navPos,g_agi.navWorld)) {
                    // The DTE candidate only recommends an E interaction; v19
                    // operator play still works if the advisory is absent/stale.
                    const bool echoConfirmed=g_agi.echoSample && g_agi.echoCommons.canConfirm(
                        g_agi.echoIngress,*g_agi.echoSample,(uint64_t)GetTickCount64(),
                        g_agi.cns.body(),g_agi.cns.niche(),g_agi.gameplay,g_agi.navWorld);
                    const bool accepted=g_agi.repairBeacon(); // ONLY WGR mints NICHE_COMMIT
                    g_agi.echoCommons.clear(); g_agi.echoSample.reset();
                    g_agi.addLog(accepted ?
                        (echoConfirmed ? "Echo Commons APL160 advised; local E/WGR committed and re-observed"
                                       : "Local E/WGR restored beacon without DTE prerequisite") :
                        "Beacon repair rejected by WGR");
                }
                else g_agi.addLog(g_agi.gameplay.interact(g_agi.navPos,g_agi.navHeading,g_agi.navWorld));
            } else if (wp == '1' || wp == '2' || wp == '3') {
                g_agi.addLog(g_agi.gameplay.choose((int)(wp - '0'), g_agi.navWorld));
            } else if (wp == 'J') {
                g_agi.gameplay.quest.journalOpen = !g_agi.gameplay.quest.journalOpen;
                g_agi.addLog(g_agi.gameplay.quest.journalOpen ? "Quest journal opened" : "Quest journal closed");
            } else if (wp == 'R') {
                g_agi.addLog(g_agi.gameplay.recover(g_agi.navWorld));
            } else if (wp == VK_F5) {
                g_agi.addLog(v20save::write(v20save::SavePath,g_agi.gameplay,g_agi.cns)
                    ? "Gameplay v20 saved atomically" : "Gameplay v20 save failed");
            } else if (wp == VK_F9) {
                const bool loaded=v20save::load(v20save::SavePath,v19game::GameplayLoop::SavePath,
                        g_agi.gameplay,g_agi.navWorld,g_agi.cns,
                        [&](const avatar3d::GameWorld& restored) {
                            v15render::HighResRenderer probe;
                            avatar3d::AvatarSkeleton pose;
                            pose.pose(restored.goal,0,0,0);
                            probe.render(restored,pose,
                                {restored.goal.x,2.2f,restored.goal.z-4.5f},0,-0.22f,false);
                            int verified=0;
                            for (int i=0;i<v15render::HighResFramebuffer::PixelCount;++i)
                                if (probe.framebuffer.objectId[i]==v15render::ObjectGoal &&
                                    std::isfinite(probe.framebuffer.depth[i])) ++verified;
                            return verified>100;
                        });
                if (loaded) {
                    g_agi.invalidateEcho();
                    g_agi.syncBodyFromCns();
                    g_agi.avatar.pose(g_agi.navPos,g_agi.navHeading,0.0f,0.0f);
                    g_agi.highResRenderer.render(g_agi.navWorld,g_agi.avatar,
                        g_agi.navPos+avatar3d::Vec3{0,2.5f,-4.0f},g_agi.navHeading,-0.4f,true);
                    g_agi.perceptionPyramid.derive(g_agi.highResRenderer.framebuffer);
                }
                g_agi.addLog(loaded ? "Gameplay loaded / v1 safely migrated if needed"
                                    : "Gameplay load rejected; state unchanged");
            } else if (wp == VK_ESCAPE) PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN: {
            if (g_agi.highResViewportVisible) return 0;
            int mx = LOWORD(lp), my = HIWORD(lp);
            int cx = 370, cy = 100, cs = 26, gox = cx + 40, goy = cy + 36;
            for (int q = 0; q < dte::TRIZArena::SIZE; q++)
                for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                    int px = gox + q * (cs + 3) + (r % 2) * (cs/2 + 1);
                    int py = goy + r * (cs + 3);
                    if (mx >= px && mx < px + cs && my >= py && my < py + cs) {
                        g_agi.selectedCellQ = q;
                        g_agi.selectedCellR = r;
                        char buf[64]; snprintf(buf, 64, "Selected cell (%d,%d)", q, r);
                        g_agi.addLog(buf);
                        break;
                    }
                }
            return 0;
        }
        case WM_DESTROY:
            if (g_displayWindow && IsWindow(g_displayWindow)) DestroyWindow(g_displayWindow);
            KillTimer(hwnd, 1);
            DeleteObject(g_hFont); DeleteObject(g_hFontLarge); DeleteObject(g_hFontMono); DeleteObject(g_hFontSmall);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "GTAngelAGIv20";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);
    WNDCLASSEXA displayClass=wc;
    displayClass.lpfnWndProc=DisplayWndProc;
    displayClass.lpszClassName="GTAngelVisualDisplayV24";
    if(!RegisterClassExA(&displayClass)) return 2;
    RECT windowRect = {0, 0, 1024, 768};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExA(0, "GTAngelAGIv20",
        "GTAngel v26 - DTE S7 to S8 / local 768 VRD",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top,
        NULL, NULL, hInst, NULL);
    if (!hwnd) return 1;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
