#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#ifndef _WIN32
static uint64_t GetTickCount64(){return 1000;}
#endif
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
// LW21: baked v25 pose clips (DTEP25L). Clean-room offline bake of the
// user-supplied pose rig; runtime samples per-joint displacements only.
class DtePoseLibrary {
public:
    static const DtePoseLibrary& instance() {
        static DtePoseLibrary lib;
        return lib;
    }
    bool valid() const { return valid_; }
    float duration(int clip) const {
        return (clip>=0 && clip<kClips && fps_[clip]>0.0f) ? (float)frameCount_[clip]/fps_[clip] : 0.0f;
    }
    // phase in [0,1); out receives 16 joint displacements (metres).
    void sample(int clip, float phase, Vec3 out[16]) const {
        for (int i=0;i<16;++i) out[i]={0.0f,0.0f,0.0f};
        if (!valid_ || clip<0 || clip>=kClips || frameCount_[clip]<1) return;
        const float p = phase - std::floor(phase);
        const float f = p * (float)frameCount_[clip];
        const int i0 = (int)f, i1 = (i0+1) % (int)frameCount_[clip];
        const float a = f - (float)i0;
        const Vec3* d0 = &data_[offset_[clip] + (size_t)i0*16];
        const Vec3* d1 = &data_[offset_[clip] + (size_t)i1*16];
        for (int i=0;i<16;++i)
            out[i] = {d0[i].x+(d1[i].x-d0[i].x)*a,
                      d0[i].y+(d1[i].y-d0[i].y)*a,
                      d0[i].z+(d1[i].z-d0[i].z)*a};
    }
private:
    static constexpr int kClips = 8;
    static constexpr uint32_t kMaxFrames = 256;
    bool valid_{false};
    uint32_t frameCount_[kClips]{};
    float fps_[kClips]{};
    size_t offset_[kClips]{};
    std::vector<Vec3> data_;
    static std::string posePath() {
#ifdef _WIN32
        char exe[4096]{};
        const unsigned long n = GetModuleFileNameA(nullptr, exe, sizeof(exe));
        if (n>0 && n<sizeof(exe)-1) {
            std::string v(exe, exe+n);
            const size_t slash = v.find_last_of("\\/");
            if (slash!=std::string::npos)
                return v.substr(0,slash+1)+"assets\\dte_v25_poses.bin";
        }
#endif
        return "assets/dte_v25_poses.bin";
    }
    explicit DtePoseLibrary() {
        std::ifstream s(posePath(), std::ios::binary);
        if (!s) return; // optional cosmetic asset
        s.seekg(0, std::ios::end);
        const std::streamoff n = s.tellg();
        s.seekg(0, std::ios::beg);
        if (n < 16 || n > 1024*1024) return;
        std::vector<uint8_t> b((size_t)n);
        if (!s.read(reinterpret_cast<char*>(b.data()), n)) return;
        static const uint8_t magic[8] = {'D','T','E','P','2','5','L',0};
        if (std::memcmp(b.data(), magic, 8) != 0) return;
        const uint32_t version = b[8]|(b[9]<<8)|(b[10]<<16)|((uint32_t)b[11]<<24);
        const uint32_t clips = b[12]|(b[13]<<8)|(b[14]<<16)|((uint32_t)b[15]<<24);
        if (version != 1 || clips != (uint32_t)kClips) return;
        size_t at = 16;
        for (int c=0;c<kClips;++c) {
            if (at+8 > b.size()) return;
            frameCount_[c] = b[at]|(b[at+1]<<8)|(b[at+2]<<16)|((uint32_t)b[at+3]<<24);
            std::memcpy(&fps_[c], &b[at+4], 4);
            at += 8;
            if (frameCount_[c]==0 || frameCount_[c]>kMaxFrames ||
                !std::isfinite(fps_[c]) || fps_[c]<=0.0f) return;
            const size_t count = (size_t)frameCount_[c]*16;
            if (at + count*12 > b.size()) return;
            offset_[c] = data_.size();
            const size_t base = data_.size();
            data_.resize(base+count);
            std::memcpy(&data_[base], &b[at], count*12);
            at += count*12;
        }
        if (at != b.size()) return;
        for (const auto& v : data_)
            if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z)) {
                data_.clear(); return;
            }
        valid_ = true;
    }
};

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
    float idleTime{0.0f}; // LW21 idle clip phase
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
        const float runBlend=std::clamp((travel-0.55f)/0.35f,0.0f,1.0f)*locomotion;
        const float strideScale=1.0f+0.36f*runBlend;
        const float step=std::sin(walkPhase)*locomotion*strideScale;
        const float bob=(0.032f+0.022f*runBlend)*(1.0f-std::cos(2.0f*walkPhase))*locomotion;
        const float ch=std::cos(heading),sh=std::sin(heading);
        auto R=[&](Vec3 v){return Vec3{v.x*ch+v.z*sh,v.y,-v.x*sh+v.z*ch}+position;};
        auto Rvec=[&](Vec3 v){return Vec3{v.x*ch+v.z*sh,v.y,-v.x*sh+v.z*ch};};
        const float stance=0.12f*load, crouch=0.12f*load;
        const float runLean=0.042f*runBlend;
        joints[Root]=R({0,0.95f+bob-crouch,0});
        joints[Spine]=R({0,1.25f+bob-crouch*0.5f,0.025f*load+runLean*0.6f});
        joints[Neck]=R({0,1.50f+bob-crouch*0.3f,runLean});
        joints[Head]=R({(inspect && !fighting)?0.022f:0.0f,1.70f+bob-crouch*0.2f,runLean*1.15f});
        for (int side=0;side<2;++side) {
            const float s=side==0?-1.0f:1.0f;
            const int shoulder=side==0?ShoulderL:ShoulderR;
            const int elbow=side==0?ElbowL:ElbowR;
            const int hand=side==0?HandL:HandR;
            const int hip=side==0?HipL:HipR;
            const int knee=side==0?KneeL:KneeR;
            const int foot=side==0?FootL:FootR;
            const float footSwing=s*step;
            const float kneeLift=std::max(0.0f,footSwing)*(0.06f+0.08f*runBlend);
            const float lift=std::max(0.0f,footSwing)*(0.12f+0.08f*runBlend);
            joints[shoulder]=R({s*0.22f,1.45f+bob-crouch*0.3f,runLean*0.8f});
            joints[elbow]=R({s*(0.28f+0.11f*load),1.15f+bob+0.04f*runBlend, -footSwing*0.24f+0.18f*load});
            joints[hand]=R({s*(0.30f+0.16f*load),0.85f+bob+0.35f*load+0.10f*runBlend,
                            -footSwing*0.44f+0.32f*load+0.06f*runBlend});
            joints[hip]=R({s*(0.12f+stance),0.95f+bob-crouch,0});
            joints[knee]=R({s*(0.14f+stance),0.50f+kneeLift-crouch*0.7f,
                            footSwing*0.28f+0.09f*load});
            joints[foot]=R({s*(0.15f+stance),0.05f+lift,
                            footSwing*0.46f+0.08f*load});
            wingRoot[side]=R({s*0.19f,1.43f+bob-crouch*0.3f,-0.16f+runLean*0.5f});
            wingBoom[side]=R({s*(0.26f+0.47f*wingDeploy),1.78f+0.26f*wingDeploy+bob,-0.24f});
            wingTip[side]=R({s*(0.30f+1.13f*wingDeploy),1.96f+0.62f*wingDeploy+bob,-0.31f});
            headset[side]=R({s*0.133f,1.72f+bob,runLean*1.15f});
        }
        // LW40: rotate baked v25/v28 GLB clip displacements by (ch, sh) into
        // the avatar's facing heading and transition between Walk24 (clip 0)
        // and Run16 (clip 7) at travel > 0.65 m/s.
        {
            const DtePoseLibrary& pl = DtePoseLibrary::instance();
            if (pl.valid()) {
                Vec3 disp[16];
                if (walkBlend > 0.03f) {
                    const int locoClip = (travel > 0.65f) ? 7 : 0;
                    pl.sample(locoClip, walkPhase*0.15915494f, disp);
                    const float w = 0.85f*walkBlend;
                    for (int i=0;i<COUNT;++i) {
                        const Vec3 rd = Rvec(disp[i]);
                        joints[i] = {joints[i].x+rd.x*w, joints[i].y+rd.y*w, joints[i].z+rd.z*w};
                    }
                } else if (fighting) {
                    int clip=-1; float phase=0.0f;
                    if (motion==Motion::CombatEntry) { clip=4; phase=std::clamp(motionTime/1.5f,0.0f,1.0f); }
                    else if (motion==Motion::Guard) { clip=6; phase=std::clamp(motionTime/1.2f,0.0f,1.0f); }
                    else if (motion==Motion::Recovery) { clip=5; phase=std::clamp(motionTime/0.30f,0.0f,1.0f); }
                    if (clip>=0) {
                        pl.sample(clip, phase, disp);
                        for (int i=0;i<COUNT;++i) {
                            const Vec3 rd = Rvec(disp[i]);
                            joints[i] = {joints[i].x+rd.x*0.9f, joints[i].y+rd.y*0.9f, joints[i].z+rd.z*0.9f};
                        }
                    }
                } else {
                    idleTime += delta;
                    const float idleDur = pl.duration(1);
                    if (inspect)
                        pl.sample(2, idleDur>0.01f ? idleTime/idleDur : 0.0f, disp);
                    else
                        pl.sample(1, idleDur>0.01f ? idleTime/idleDur : 0.0f, disp);
                    for (int i=0;i<COUNT;++i) {
                        const Vec3 rd = Rvec(disp[i]);
                        joints[i] = {joints[i].x+rd.x*0.9f, joints[i].y+rd.y*0.9f, joints[i].z+rd.z*0.9f};
                    }
                }
            }
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
    float ambX{-28.0f}, ambDir{1.0f};
    float busZ{10.0f},  busDir{-1.0f};
    Vec3 collisionFreeEye(Vec3 body, Vec3 desired) const {
        // LW54: Camera & ray pass freely through all world objects (never
        // registering hard collision or snapping on trees, buildings, props,
        // vehicles, or peds). Obfuscation is rectified smoothly per-pixel in
        // RasterEngine::rasterize via continuous alpha blending.
        // Only apply a local centre offset when a synthetic caller places
        // desired directly inside a pillar core at low height (desired.y < 1.5f).
        if (desired.y < 1.5f) {
            for (const auto& pl : pillars) {
                const float dx = desired.x - pl.pos.x, dz = desired.z - pl.pos.z;
                if (dx * dx + dz * dz < (pl.radius + 0.25f) * (pl.radius + 0.25f)) {
                    return desired + (body - desired) * 0.22f + Vec3{0.0f, 0.25f, 0.0f};
                }
            }
        }
        return desired;
    }
    bool pedBlockedByBarrier(Vec3 cand, Vec3 cur, size_t selfIdx) const {
        if (collides(cand)) return true;
        // Streetlamp poles (props)
        for (const auto& lamp : props) {
            const float dNext = len(cand - lamp.pos);
            if (dNext < 0.45f && dNext < len(cur - lamp.pos)) return true;
        }
        // Mutual barrier with Ariadne NPC & parked/driven coupe
        if (len(cand - npcPos) < 0.95f && len(cand - npcPos) < len(cur - npcPos)) return true;
        if (std::fabs(cand.x - vehiclePos.x) < 1.35f && std::fabs(cand.z - vehiclePos.z) < 2.35f &&
            len(cand - vehiclePos) < len(cur - vehiclePos)) return true;
        // Mutual pedestrian-to-pedestrian barrier
        for (size_t j = 0; j < walkers.size(); ++j) {
            if (j == selfIdx) continue;
            const float dNext = len(cand - walkers[j].pos);
            if (dNext < 0.95f && dNext < len(cur - walkers[j].pos)) return true;
        }
        // Mutual pedestrian-to-vehicle barrier against all moving ground traffic
        for (const auto& v : traffic) {
            if (v.orbitR > 0.0f) continue;
            const float halfX = (v.axis == 'x') ? 2.55f : 1.30f;
            const float halfZ = (v.axis == 'z') ? 2.55f : 1.30f;
            if (std::fabs(cand.x - v.pos.x) < halfX && std::fabs(cand.z - v.pos.z) < halfZ &&
                len(cand - v.pos) < len(cur - v.pos)) return true;
        }
        // Mutual pedestrian barrier against Ambulance (z = -1.4f) and 6-Wheel City Bus (x = -1.4f)
        const Vec3 ambPos{ambX, 0.0f, -1.4f};
        if (std::fabs(cand.x - ambPos.x) < 2.55f && std::fabs(cand.z - ambPos.z) < 1.35f &&
            len(cand - ambPos) < len(cur - ambPos)) return true;
        const Vec3 busPos{-1.4f, 0.0f, busZ};
        if (std::fabs(cand.x - busPos.x) < 1.45f && std::fabs(cand.z - busPos.z) < 3.15f &&
            len(cand - busPos) < len(cur - busPos)) return true;
        return false;
    }
    void tickWalkers(float dt) {
        walkerTime += dt;
        const Vec3 ambPos{ambX, 0.0f, -1.4f};
        const Vec3 busPos{-1.4f, 0.0f, busZ};
        for (size_t i = 0; i < walkers.size(); ++i) {
            auto& w = walkers[i];
            // Active mutual separation if encroached by another walker or vehicle
            for (size_t j = 0; j < walkers.size(); ++j) {
                if (i == j) continue;
                const Vec3 diff = w.pos - walkers[j].pos;
                const float d = len(diff);
                if (d > 1e-4f && d < 0.90f) {
                    const Vec3 pushed = w.pos + (diff * ((0.92f - d) / d));
                    if (!collides(pushed)) w.pos = pushed;
                }
            }
            auto pushOutOfVehicle = [&](Vec3 vp, float hx, float hz) {
                const float dx = w.pos.x - vp.x, dz = w.pos.z - vp.z;
                if (std::fabs(dx) < hx && std::fabs(dz) < hz) {
                    Vec3 pushed = w.pos;
                    if (hx - std::fabs(dx) < hz - std::fabs(dz))
                        pushed.x = vp.x + (dx >= 0.0f ? hx + 0.15f : -(hx + 0.15f));
                    else
                        pushed.z = vp.z + (dz >= 0.0f ? hz + 0.15f : -(hz + 0.15f));
                    if (!collides(pushed)) w.pos = pushed;
                }
            };
            for (const auto& v : traffic) {
                if (v.orbitR > 0.0f) continue;
                pushOutOfVehicle(v.pos, (v.axis == 'x') ? 2.45f : 1.25f, (v.axis == 'z') ? 2.45f : 1.25f);
            }
            pushOutOfVehicle(ambPos, 2.45f, 1.30f);
            pushOutOfVehicle(busPos, 1.40f, 3.05f);
            pushOutOfVehicle(vehiclePos, 1.30f, 2.25f);

            w.heading += 0.7f * std::sin(walkerTime * 0.6f + w.phase) * dt
                       + 0.4f * std::sin(walkerTime * 0.23f + w.phase * 2.1f) * dt;
            const Vec3 next = w.pos + Vec3{std::sin(w.heading) * w.speed * dt, 0,
                                           std::cos(w.heading) * w.speed * dt};
            if (pedBlockedByBarrier(next, w.pos, i) ||
                std::fabs(next.x) > worldSize * 0.5f - 1.0f ||
                std::fabs(next.z) > worldSize * 0.5f - 1.0f)
                w.heading += 1.25f;
            else
                w.pos = next;
        }
    }
    // re3 CVehicle lanes with mutual vehicle-to-vehicle, intersection & pedestrian barriers.
    void tickTraffic(float dt) {
        trafficTime += dt;
        struct GroundBox { Vec3 pos; float hx, hz; char axis; };
        auto getBoxes = [&](int skipIdx) {
            std::vector<GroundBox> boxes;
            for (size_t j = 0; j < traffic.size(); ++j) {
                if ((int)j == skipIdx || traffic[j].orbitR > 0.0f) continue;
                const auto& o = traffic[j];
                boxes.push_back({o.pos, (o.axis == 'x') ? 2.1f : 0.95f, (o.axis == 'z') ? 2.1f : 0.95f, o.axis});
            }
            if (skipIdx != 100) boxes.push_back({Vec3{ambX, 0.0f, -1.4f}, 2.1f, 0.95f, 'x'});
            if (skipIdx != 101) boxes.push_back({Vec3{-1.4f, 0.0f, busZ}, 0.95f, 2.55f, 'z'});
            return boxes;
        };
        auto blockedByPedOrCross = [&](Vec3 curP, Vec3 nextP, float hx, float hz, char axis, int selfCode, bool& shouldRebound) {
            shouldRebound = false;
            for (const auto& b : getBoxes(selfCode)) {
                const float curDx  = std::fabs(curP.x - b.pos.x),  curDz  = std::fabs(curP.z - b.pos.z);
                const float nextDx = std::fabs(nextP.x - b.pos.x), nextDz = std::fabs(nextP.z - b.pos.z);
                if (nextDx < (hx + b.hx + 0.35f) && nextDz < (hz + b.hz + 0.35f)) {
                    const float curDist = curDx + curDz, nextDist = nextDx + nextDz;
                    if (nextDist < curDist) {
                        if (b.axis == axis) shouldRebound = true;
                        return true;
                    }
                }
            }
            for (const auto& w : walkers) {
                if (std::fabs(nextP.x - w.pos.x) < (hx + 0.55f) && std::fabs(nextP.z - w.pos.z) < (hz + 0.55f) &&
                    len(nextP - w.pos) < len(curP - w.pos)) {
                    return true;
                }
            }
            return false;
        };

        for (size_t i = 0; i < traffic.size(); ++i) {
            auto& v = traffic[i];
            if (v.orbitR > 0.0f) {
                v.orbitA += v.speed * dt;
                v.pos = {std::cos(v.orbitA) * v.orbitR, v.orbitY, std::sin(v.orbitA) * v.orbitR};
                v.heading = -v.orbitA;
                continue;
            }
            float& along = v.axis == 'x' ? v.pos.x : v.pos.z;
            const float nextAlong = along + v.dir * v.speed * dt;
            const Vec3 nextPos = v.axis == 'x' ? Vec3{nextAlong, v.pos.y, v.pos.z} : Vec3{v.pos.x, v.pos.y, nextAlong};
            const float hx = (v.axis == 'x') ? 2.1f : 0.95f;
            const float hz = (v.axis == 'z') ? 2.1f : 0.95f;
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(v.pos, nextPos, hx, hz, v.axis, (int)i, rebound);
            if (rebound) {
                v.dir = -v.dir;
            } else if (!blocked) {
                along = nextAlong;
                if (along >= v.max) { along = v.max; v.dir = -1; }
                if (along <= v.min) { along = v.min; v.dir = 1; }
            }
            v.heading = v.axis == 'x'
                ? (v.dir > 0 ? 1.5707963f : 4.7123890f)
                : (v.dir > 0 ? 0.0f : 3.14159265f);
        }
        // Step Ambulance (z = -1.4f, axis = 'x') with full mutual barriers
        {
            const Vec3 curAmb{ambX, 0.0f, -1.4f};
            const float nextAmbX = ambX + ambDir * 2.9f * dt;
            const Vec3 nextAmb{nextAmbX, 0.0f, -1.4f};
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(curAmb, nextAmb, 2.1f, 0.95f, 'x', 100, rebound);
            if (rebound) ambDir = -ambDir;
            else if (!blocked) {
                ambX = nextAmbX;
                if (ambX >= 28.0f) { ambX = 28.0f; ambDir = -1.0f; }
                if (ambX <= -28.0f) { ambX = -28.0f; ambDir = 1.0f; }
            }
        }
        // Step 6-Wheel City Bus (x = -1.4f, axis = 'z') with full mutual barriers
        {
            const Vec3 curBus{-1.4f, 0.0f, busZ};
            const float nextBusZ = busZ + busDir * 2.2f * dt;
            const Vec3 nextBus{-1.4f, 0.0f, nextBusZ};
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(curBus, nextBus, 0.95f, 2.55f, 'z', 101, rebound);
            if (rebound) busDir = -busDir;
            else if (!blocked) {
                busZ = nextBusZ;
                if (busZ >= 28.0f) { busZ = 28.0f; busDir = -1.0f; }
                if (busZ <= -28.0f) { busZ = -28.0f; busDir = 1.0f; }
            }
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
namespace v15render {
// Z++ Formal Schema Contracts derived from native/re3_reference/zpp/{vehicles,peds,renderer,animation}/*.zpp
namespace zpp_spec {
enum class eCarNodes : uint8_t {
    CAR_WHEEL_RF = 1, CAR_WHEEL_RM, CAR_WHEEL_RB, CAR_WHEEL_LF, CAR_WHEEL_LM, CAR_WHEEL_LB,
    CAR_BUMP_FRONT, CAR_BUMP_REAR, CAR_WING_RF, CAR_WING_RR, CAR_DOOR_RF, CAR_DOOR_RR,
    CAR_WING_LF, CAR_WING_LR, CAR_DOOR_LF, CAR_DOOR_LR, CAR_BONNET, CAR_BOOT, CAR_WINDSCREEN, NUM_CAR_NODES
};
enum class eEngineStatus : uint8_t { STEAM1 = 100, STEAM2 = 150, SMOKE = 200, ON_FIRE = 225 };
enum class eWheelStatus  : uint8_t { OK = 0, BURST = 1, MISSING = 2 };
enum class eFormation    : uint8_t { UNDEFINED = 0, REAR, REAR_LEFT, REAR_RIGHT, FRONT_LEFT, FRONT_RIGHT, LEFT, RIGHT, FRONT };
struct CDamageManagerSpec {
    float m_fWheelDamageEffect{0.0f};
    uint8_t m_engineStatus{0};
    uint8_t m_wheelStatus[4]{0, 0, 0, 0};
    uint8_t m_doorStatus[6]{0, 0, 0, 0, 0, 0};
    constexpr bool invariant() const { return m_fWheelDamageEffect >= 0.0f && m_fWheelDamageEffect <= 1.0f; }
};
struct CTimeCycleSpec {
    uint8_t m_nCurrentSkyTopBlue{46}, m_nCurrentSkyTopGreen{28}, m_nCurrentSkyTopRed{34};
    uint8_t m_nCurrentLowCloudsBlue{128}, m_nCurrentLowCloudsGreen{86}, m_nCurrentLowCloudsRed{148};
    float m_fCurrentLightsOnGroundBrightness{1.0f};
    constexpr bool invariant() const { return m_fCurrentLightsOnGroundBrightness >= 0.0f; }
};
} // namespace zpp_spec

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
    const float skyB = 46.0f, skyG = 30.0f, skyR = 34.0f;
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
    ObjectVehicle = 8,
    ObjectTraffic = 9,
    ObjectPickup = 10,
    ObjectProp = 11
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
        // LW40: store BOTH proximal (deltaA) and distal (deltaB) joint
        // displacements + rest Y span (baseY0..baseY1) for C0-continuous
        // proximal-to-distal limb interpolation across hips, knees, ankles,
        // shoulders, elbows, and wrists.
        avatar3d::Vec3 deltaA[kTargetCount]{}, deltaB[kTargetCount]{};
        float baseY0[kTargetCount]{}, baseY1[kTargetCount]{};
        for (uint32_t t=0;t<kTargetCount;++t) {
            avatar3d::Vec3 baseA{},baseB{},nowA{},nowB{};
            if (posePair(neutral,static_cast<uint8_t>(t),baseA,baseB) &&
                posePair(avatar,static_cast<uint8_t>(t),nowA,nowB)) {
                deltaA[t]=sub(nowA,add(rotateY(baseA,ch,sh),origin));
                deltaB[t]=sub(nowB,add(rotateY(baseB,ch,sh),origin));
                baseY0[t]=baseA.y; baseY1[t]=baseB.y;
            }
        }
        const float faceClock = avatar.idleTime + avatar.walkPhase * 0.21f + avatar.motionTime;
        const float blinkCycle = std::fmod(std::max(0.0f, faceClock), 3.4f);
        const float blinkW = (blinkCycle < 0.16f) ? std::sin(blinkCycle * (3.14159265f / 0.16f)) : 0.0f;
        const float jawW = 0.25f + 0.25f * std::sin(faceClock * 2.7f) + 0.35f * avatar.orbIntensity;
        for (size_t triangleIndex=0;triangleIndex<triangles_.size();++triangleIndex) {
            const Triangle& triangle=triangles_[triangleIndex];
            const bool wing=triangleIndex>=bodyTriangleCount_ && nativeWings_;
            const avatar3d::Vec3 a = skinVertex(vertices_[triangle.i0], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
            const avatar3d::Vec3 b = skinVertex(vertices_[triangle.i1], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
            const avatar3d::Vec3 c = skinVertex(vertices_[triangle.i2], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
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
    static constexpr uint32_t kMaxTriangles = 32768;
    static constexpr uint32_t kMaxBodyTriangles = 32000;
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

    avatar3d::Vec3 skinVertex(const Vertex& vertex,const avatar3d::Vec3* deltaA,
                               const avatar3d::Vec3* deltaB,const float* baseY0,const float* baseY1,
                               avatar3d::Vec3 origin,float ch,float sh,bool wing,
                               float blinkW=0.0f,float jawW=0.0f,float wingDeploy=0.0f) const {
        if (wing) {
            avatar3d::Vec3 wr = vertex.rest;
            if (wingDeploy > 0.01f) {
                const float span = std::max(0.0f, std::fabs(wr.x) - 0.14f);
                wr.x *= (1.0f + 0.65f * wingDeploy);
                wr.y += 0.32f * wingDeploy * span;
                wr.z -= 0.10f * wingDeploy * span;
            }
            return add(add(rotateY(wr,ch,sh),origin),deltaA[1]);
        }
        avatar3d::Vec3 rest = vertex.rest;
        if (rest.y > 1.46f && rest.z > 0.05f) {
            const float dLx = rest.x - (-0.039f), dLy = rest.y - 1.568f, dLz = rest.z - 0.091f;
            const float dRx = rest.x - ( 0.037f), dRy = rest.y - 1.567f, dRz = rest.z - 0.093f;
            const float rL2 = dLx*dLx + dLy*dLy + dLz*dLz;
            const float rR2 = dRx*dRx + dRy*dRy + dRz*dRz;
            if (blinkW > 0.01f && dLy > -0.002f && (rL2 < 0.00048f || rR2 < 0.00048f)) {
                const float falloff = 1.0f - std::min(rL2, rR2) / 0.00048f;
                rest.y -= 0.0085f * blinkW * falloff;
            }
            const float dMx = rest.x, dMy = rest.y - 1.496f, dMz = rest.z - 0.108f;
            const float rM2 = dMx*dMx + dMy*dMy + dMz*dMz;
            if (jawW > 0.01f && dMy < 0.002f && rM2 < 0.00068f) {
                const float falloff = 1.0f - rM2 / 0.00068f;
                rest.y -= 0.0065f * jawW * falloff;
            }
        }
        avatar3d::Vec3 displacement{};
        for (int influence = 0; influence < 4; ++influence) {
            const uint8_t boneIndex = vertex.bone[influence];
            const float weight = static_cast<float>(vertex.weight[influence]) / 255.0f;
            if (weight <= 0.0f || boneIndex >= bones_.size()) continue;
            const uint8_t t = bones_[boneIndex].target;
            const float span = baseY1[t] - baseY0[t];
            const float s = (std::fabs(span) > 0.02f) ? std::clamp((rest.y - baseY0[t]) / span, 0.0f, 1.0f) : 1.0f;
            const avatar3d::Vec3 segDelta = add(mul(deltaA[t], 1.0f - s), mul(deltaB[t], s));
            displacement = add(displacement, mul(segDelta, weight));
        }
        return add(add(rotateY(rest,ch,sh),origin),displacement);
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
            float sr, sg, sb;
            if (t < 0.45f) {
                const float u = t / 0.45f;
                sr = 58.0f + (34.0f - 58.0f) * u;
                sg = 38.0f + (30.0f - 38.0f) * u;
                sb = 52.0f + (46.0f - 52.0f) * u;
            } else {
                const float u = (t - 0.45f) / 0.55f;
                sr = 34.0f + (14.0f - 34.0f) * u;
                sg = 30.0f + (12.0f - 30.0f) * u;
                sb = 46.0f + (26.0f - 46.0f) * u;
            }
            sky.r = (uint8_t)std::lround(sr);
            sky.g = (uint8_t)std::lround(sg);
            sky.b = (uint8_t)std::lround(sb);
            const uint32_t packed = pack(sky);
            const bool starRow = y < Height * 36 / 100; // upper zenith sky band only
            uint32_t h = (uint32_t)y * 2654435761u + 12345u;
            for (int x = 0; x < Width; ++x) {
                const int index = y * Width + x;
                colour[index] = packed;
                depth[index] = 1e30f;
                objectId[index] = ObjectSky;
                if (starRow) { // LW24: sparse deterministic starfield
                    h = h * 1664525u + 1013904223u;
                    if ((h & 1023u) < 7u) {
                        const uint8_t s = (uint8_t)(150u + ((h >> 8) & 127u));
                        colour[index] = pack({(uint8_t)(205u + ((h >> 16) & 50u)), s, s, 255});
                    }
                }
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
                    case ObjectTraffic: presentation[index] = pack({255, 160, 60, 255}); break;
                    case ObjectPickup: presentation[index] = pack({80, 220, 255, 255}); break;
                    case ObjectProp: presentation[index] = pack({180, 255, 120, 255}); break;
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

        buildSun();
        buildMoon();
        buildGround(world);
        buildRoads(world);
        if (thirdPerson && !world.vehicleOccupied) buildAvatar(avatar);
        buildGoal(world);
        buildNpc(world);
        buildQuestShard(world);
        buildVehicle(world);
        buildWalkers(world);
        buildTraffic(world);
        buildPickups(world);
        buildPillars(world);
        buildBuildings(world);
        buildProps(world);

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
        // LW34: cohesive urban slate plaza with 4m block joints instead of
        // a harsh 1m black/purple checkerboard.
        const uint32_t h = (uint32_t)(gx * 73856093) ^ (uint32_t)(gz * 19349663);
        const int v = (int)((h >> 8) & 7) - 3;
        const bool seam = ((gx & 3) == 0) || ((gz & 3) == 0);
        const int baseB = seam ? 54 : 62;
        const int baseG = seam ? 50 : 57;
        const int baseR = seam ? 48 : 55;
        return Colour{(uint8_t)(baseB + v), (uint8_t)(baseG + v), (uint8_t)(baseR + v), 255};
    }

    void addTriangle(Vec3 a, Vec3 b, Vec3 c, Colour colour, uint16_t objectId) {
        triangles_.push_back({a, b, c, colour, objectId});
    }

    void addQuad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Colour colour, uint16_t objectId) {
        addTriangle(a, b, c, colour, objectId);
        addTriangle(a, c, d, colour, objectId);
    }

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
        // LW52 (Skidmarks.zpp CSkidmarks): stationary world-space braking tire marks on intersection approaches
        for (float laneZ : {-1.4f, 1.4f}) {
            for (float trackOff : {-0.62f, 0.62f}) {
                const float tz = laneZ + trackOff;
                addQuad({-7.5f, 0.017f, tz - 0.06f}, {-4.2f, 0.017f, tz - 0.06f},
                        {-4.2f, 0.017f, tz + 0.06f}, {-7.5f, 0.017f, tz + 0.06f}, Colour{34, 34, 40, 255}, ObjectGround);
                addQuad({ 4.2f, 0.017f, tz - 0.06f}, { 7.5f, 0.017f, tz - 0.06f},
                        { 7.5f, 0.017f, tz + 0.06f}, { 4.2f, 0.017f, tz + 0.06f}, Colour{34, 34, 40, 255}, ObjectGround);
            }
        }
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
    void buildBuildings(const avatar3d::GameWorld& world) {
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
            const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
            // Stepped stone pedestal base (0 .. 0.55m) + glowing energy ring
            const Colour ped = (segment & 1) ? Colour{92, 88, 98, 255} : Colour{78, 74, 84, 255};
            addQuad({world.goal.x + c0*0.58f, 0.0f,  world.goal.z + s0*0.58f},
                    {world.goal.x + c1*0.58f, 0.0f,  world.goal.z + s1*0.58f},
                    {world.goal.x + c1*0.44f, 0.52f, world.goal.z + s1*0.44f},
                    {world.goal.x + c0*0.44f, 0.52f, world.goal.z + s0*0.44f}, ped, ObjectGoal);
            // Faceted crystal spire (0.52m .. 2.85m) + apex pyramid (2.85m .. 3.45m)
            const Colour spire = (segment & 1) ? goal : Colour{(uint8_t)(goal.b*4/5), (uint8_t)(goal.g*4/5), (uint8_t)(goal.r*4/5), 255};
            Vec3 p0{world.goal.x + c0 * 0.28f, 0.52f, world.goal.z + s0 * 0.28f};
            Vec3 p1{world.goal.x + c1 * 0.28f, 0.52f, world.goal.z + s1 * 0.28f};
            Vec3 p2{world.goal.x + c1 * 0.14f, 2.85f, world.goal.z + s1 * 0.14f};
            Vec3 p3{world.goal.x + c0 * 0.14f, 2.85f, world.goal.z + s0 * 0.14f};
            addQuad(p0, p1, p2, p3, spire, ObjectGoal);
            addTriangle({world.goal.x, 3.45f, world.goal.z}, p3, p2, goal, ObjectGoal);
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
        // LW39 Act II landmark 1: Obsidian & Gold Memory Obelisk at (-9.0, 0, -2.4)
        {
            const Vec3 ob{-9.0f, 0.0f, -2.4f};
            const Colour basalt{54, 46, 62, 255}, runeGold{75, 205, 250, 255}, violet{195, 110, 235, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({ob.x + c0*0.52f, 0.0f,  ob.z + s0*0.52f},
                        {ob.x + c1*0.52f, 0.0f,  ob.z + s1*0.52f},
                        {ob.x + c1*0.38f, 0.45f, ob.z + s1*0.38f},
                        {ob.x + c0*0.38f, 0.45f, ob.z + s0*0.38f}, basalt, ObjectQuestItem);
                addQuad({ob.x + c0*0.26f, 0.45f, ob.z + s0*0.26f},
                        {ob.x + c1*0.26f, 0.45f, ob.z + s1*0.26f},
                        {ob.x + c1*0.14f, 2.65f, ob.z + s1*0.14f},
                        {ob.x + c0*0.14f, 2.65f, ob.z + s0*0.14f}, (s & 1) ? basalt : violet, ObjectQuestItem);
                addTriangle({ob.x, 3.25f, ob.z},
                            {ob.x + c0*0.16f, 2.65f, ob.z + s0*0.16f},
                            {ob.x + c1*0.16f, 2.65f, ob.z + s1*0.16f}, runeGold, ObjectQuestItem);
            }
        }
        // LW39 Act II landmark 2: Harmonic Transit Relay Spire at (9.0, 0, 2.4)
        {
            const Vec3 rl{9.0f, 0.0f, 2.4f};
            const Colour steel{88, 96, 112, 255}, harmonic{225, 235, 90, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({rl.x + c0*0.48f, 0.0f,  rl.z + s0*0.48f},
                        {rl.x + c1*0.48f, 0.0f,  rl.z + s1*0.48f},
                        {rl.x + c1*0.22f, 2.75f, rl.z + s1*0.22f},
                        {rl.x + c0*0.22f, 2.75f, rl.z + s0*0.22f}, (s & 1) ? steel : harmonic, ObjectQuestItem);
                addTriangle({rl.x, 3.40f, rl.z},
                            {rl.x + c0*0.24f, 2.75f, rl.z + s0*0.24f},
                            {rl.x + c1*0.24f, 2.75f, rl.z + s1*0.24f}, harmonic, ObjectQuestItem);
            }
        }
        // LW40 Act III landmark 3: Faustian Shadow Node at (2.6, 0, -18.0)
        // & Skyline Resonance Tower at (-18.0, 0, 2.6).
        {
            const Vec3 sn{2.6f, 0.0f, -18.0f};
            const Colour darkCore{44, 34, 72, 255}, ember{75, 85, 235, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({sn.x + c0*0.50f, 0.0f,  sn.z + s0*0.50f},
                        {sn.x + c1*0.50f, 0.0f,  sn.z + s1*0.50f},
                        {sn.x + c1*0.20f, 2.90f, sn.z + s1*0.20f},
                        {sn.x + c0*0.20f, 2.90f, sn.z + s0*0.20f}, (s & 1) ? darkCore : ember, ObjectQuestItem);
                addTriangle({sn.x, 3.55f, sn.z},
                            {sn.x + c0*0.22f, 2.90f, sn.z + s0*0.22f},
                            {sn.x + c1*0.22f, 2.90f, sn.z + s1*0.22f}, ember, ObjectQuestItem);
            }
            const Vec3 st{-18.0f, 0.0f, 2.6f};
            const Colour towerCol{115, 195, 85, 255}, crownCol{185, 245, 130, 255};
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * s / 8.0f, a1 = 6.2831853f * (s + 1) / 8.0f;
                const float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
                addQuad({st.x + c0*0.54f, 0.0f,  st.z + s0*0.54f},
                        {st.x + c1*0.54f, 0.0f,  st.z + s1*0.54f},
                        {st.x + c1*0.22f, 3.20f, st.z + s1*0.22f},
                        {st.x + c0*0.22f, 3.20f, st.z + s0*0.22f}, (s & 1) ? towerCol : crownCol, ObjectQuestItem);
                addTriangle({st.x, 3.85f, st.z},
                            {st.x + c0*0.24f, 3.20f, st.z + s0*0.24f},
                            {st.x + c1*0.24f, 3.20f, st.z + s1*0.24f}, crownCol, ObjectQuestItem);
            }
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

    void addBlobShadow(Vec3 centre, float radius) {
        // LW32: soft dark ellipse grounding each entity.
        static constexpr int Segments = 10;
        const Colour shade{26, 24, 22, 255};
        for (int i = 0; i < Segments; ++i) {
            const float a0 = 6.2831853f * i / Segments, a1 = 6.2831853f * (i + 1) / Segments;
            addTriangle(centre + Vec3{0, 0.013f, 0},
                        centre + Vec3{cosf(a0) * radius, 0.013f, sinf(a0) * radius},
                        centre + Vec3{cosf(a1) * radius, 0.013f, sinf(a1) * radius},
                        shade, ObjectGround);
        }
    }
    void boxLimb(Vec3 a, Vec3 b, float w, float d, Colour colour, uint16_t objectId = ObjectNpc) {
        // LW31: oriented rectangular limb prism between two skeleton joints.
        Vec3 axis = normalize3(b - a);
        if (!std::isfinite(axis.x) || !std::isfinite(axis.y) || !std::isfinite(axis.z)) return;
        Vec3 reference = fabsf(axis.y) < 0.86f ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        Vec3 u = normalize3(cross3(axis, reference));
        Vec3 v = normalize3(cross3(axis, u));
        const float hw = w * 0.5f, hd = d * 0.5f;
        Vec3 p[8];
        for (int i = 0; i < 2; ++i) {
            const Vec3 base = i ? b : a;
            p[i*4+0] = base + u*hw + v*hd;
            p[i*4+1] = base + u*hw - v*hd;
            p[i*4+2] = base - u*hw - v*hd;
            p[i*4+3] = base - u*hw + v*hd;
        }
        for (int s = 0; s < 4; ++s) {
            const int n = (s + 1) & 3;
            addQuad(p[s], p[n], p[4+n], p[4+s], colour, objectId);
        }
    }
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
            const Colour halo{26, 66, 74, 255};
            for (int i = 0; i < 8; ++i) {
                const float a0 = (float)i * 0.7854f, a1 = (float)(i + 1) * 0.7854f;
                const float hr = radius * 1.9f;
                const Vec3 q0 = {c.x + hr * cosf(a0), c.y + hr * sinf(a0), c.z};
                const Vec3 q1 = {c.x + hr * cosf(a1), c.y + hr * sinf(a1), c.z};
                addTriangle(c, q0, q1, halo, ObjectQuestItem);
            }
        for (int i = 0; i < 4; ++i) {
            const int next = (i + 1) % 4;
            addTriangle(top, ring[i], ring[next], shard, ObjectQuestItem);
            addTriangle(bottom, ring[next], ring[i], Colour{34, 154, 225, 255}, ObjectQuestItem);
        }
    }

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
            const float ambDir   = world.ambDir;
            const float ambX     = world.ambX;
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
            const float busDir   = world.busDir;
            const float busZ     = world.busZ;
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
    void buildPickups(const avatar3d::GameWorld& world) {
        for (const auto& pk : world.pickups) {
            if (pk.collected) continue;
            const Vec3 c = pk.pos;
            const float r = 0.22f;
            const float sa = world.trafficTime * 2.0f + pk.phase;
            const Vec3 rx{r * cosf(sa), 0.0f, r * sinf(sa)};
            const Vec3 rz{r * sinf(sa), 0.0f, -r * cosf(sa)};
            const Vec3 top = c + Vec3{0, r*1.4f, 0};
            const Vec3 bot = c + Vec3{0, -r*1.4f, 0};
            addTriangle(top, c + rx, c + rz, Colour{255,220,80,255}, ObjectPickup);
            addTriangle(top, c + rz, c - rx, Colour{255,220,80,255}, ObjectPickup);
            addTriangle(bot, c + rx, c + rz, Colour{255,200,60,255}, ObjectPickup);
            addTriangle(bot, c + rz, c - rx, Colour{255,200,60,255}, ObjectPickup);
            const float glow = 0.45f + 0.1f * sinf(world.trafficTime * 3.0f + pk.phase);
            addTriangle(c + Vec3{glow, 0.02f, 0}, c + Vec3{0, 0.02f, glow}, c + Vec3{-glow, 0.02f, 0}, Colour{120,100,40,255}, ObjectPickup);
            addTriangle(c + Vec3{glow, 0.02f, 0}, c + Vec3{-glow, 0.02f, 0}, c + Vec3{0, 0.02f, -glow}, Colour{120,100,40,255}, ObjectPickup);
        }
    }
    // re3 CDummy props: street lamps (pole + lit head).
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
            // LW52 (Coronas.zpp TYPE_STAR & Timecycle.zpp): 4-point star corona around streetlamp core
            addTriangle(head + Vec3{-0.38f, -0.04f, 0.0f}, head + Vec3{0.38f, -0.04f, 0.0f}, head + Vec3{0.0f, 0.06f, 0.0f}, Colour{195, 242, 255, 255}, ObjectProp);
            addTriangle(head + Vec3{0.0f, -0.04f, -0.38f}, head + Vec3{0.0f, -0.04f, 0.38f}, head + Vec3{0.0f, 0.06f, 0.0f}, Colour{195, 242, 255, 255}, ObjectProp);
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
                // LW54: Smooth per-pixel alpha-blend obfuscation rectification when an
                // obstacle or prop passes between the 3rd-person camera and the avatar/sightline
                if ((oid == ObjectObstacle || oid == ObjectProp) && depth < 4.25f) {
                    const uint16_t prevId = framebuffer.objectId[index];
                    const float cx = (x - Surface::Width * 0.5f) / (Surface::Width * 0.22f);
                    const float cy = (y - Surface::Height * 0.54f) / (Surface::Height * 0.28f);
                    const float r2 = cx * cx + cy * cy;
                    if (prevId == ObjectAvatar || r2 < 1.0f) {
                        const float u = std::clamp(r2, 0.0f, 1.0f);
                        const float alpha = (prevId == ObjectAvatar) ? 0.20f : (0.24f + 0.76f * (u * u * (3.0f - 2.0f * u)));
                        const uint32_t bg = framebuffer.colour[index];
                        const uint8_t bgB = (uint8_t)(bg & 0xFFu);
                        const uint8_t bgG = (uint8_t)((bg >> 8) & 0xFFu);
                        const uint8_t bgR = (uint8_t)((bg >> 16) & 0xFFu);
                        shaded.b = (uint8_t)(shaded.b * alpha + bgB * (1.0f - alpha));
                        shaded.g = (uint8_t)(shaded.g * alpha + bgG * (1.0f - alpha));
                        shaded.r = (uint8_t)(shaded.r * alpha + bgR * (1.0f - alpha));
                        framebuffer.colour[index] = Surface::pack(shaded);
                        if (prevId != ObjectAvatar) {
                            framebuffer.depth[index] = depth;
                            framebuffer.objectId[index] = oid;
                        }
                        wrote = true;
                        continue;
                    }
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
namespace v27learn {
struct OnlineNavigationObserver {
    uint64_t updates{0}, rejected{0}, collisions{0}, windows{0};
    uint32_t stateVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST]{};
    uint32_t actionVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST][avatar3d::NavTrainer::COUNT]{};
    uint32_t windowSteps{0};
    float windowReturn{0}, lastReward{0}, lastQDelta{0};
    static bool valid(const avatar3d::NavTrainer::State& s) {
        return s.bearing>=0 && s.bearing<avatar3d::NavTrainer::BEARINGS &&
               s.dist>=0 && s.dist<avatar3d::NavTrainer::DISTS &&
               s.obst>=0 && s.obst<avatar3d::NavTrainer::OBST;
    }
    static avatar3d::NavTrainer::Action action(float forward,float turn) {
        using N=avatar3d::NavTrainer;
        if(forward<-.01f) return N::BACK;
        if(forward>.01f) return turn<-.01f?N::FWD_L:turn>.01f?N::FWD_R:N::FWD;
        return turn<-.01f?N::TURN_L:turn>.01f?N::TURN_R:N::WAIT;
    }
    bool observe(avatar3d::NavTrainer& q,
                 const v15render::VisualNavEvidence& before,
                 const v15render::VisualNavEvidence& after,
                 float forward,float turn,float reward,bool humanAccepted,
                 bool contact,bool onFoot,bool enabled) {
        if(!enabled) return false;
        if(!humanAccepted || !onFoot || before.sourceSequence==0 ||
           after.sourceSequence<=before.sourceSequence || !valid(before.state) || !valid(after.state) ||
           !std::isfinite(forward) || !std::isfinite(turn) || !std::isfinite(reward) ||
           std::fabs(forward)>1 || std::fabs(turn)>1 || std::fabs(reward)>2 ||
           (std::fabs(forward)<.01f && std::fabs(turn)<.01f)) {
            ++rejected; return false;
        }
        const auto a=action(forward,turn);
        const float old=q.Q[before.state.bearing][before.state.dist][before.state.obst][a];
        q.update(before.state,a,reward,after.state);
        lastQDelta=q.Q[before.state.bearing][before.state.dist][before.state.obst][a]-old;
        ++updates; ++q.stepsTotal; ++windowSteps;
        ++stateVisits[before.state.bearing][before.state.dist][before.state.obst];
        ++actionVisits[before.state.bearing][before.state.dist][before.state.obst][a];
        if(contact) { ++collisions; ++q.collisions; }
        lastReward=reward; windowReturn+=reward;
        // These are fixed observational windows, NOT successful goal episodes.
        if(windowSteps==200) {
            ++windows; ++q.episodes;
            q.returnsPerEpisode.push_back(windowReturn);
            q.stepsPerEpisode.push_back(static_cast<int>(windowSteps));
            if(q.returnsPerEpisode.size()>1000) {
                q.returnsPerEpisode.erase(q.returnsPerEpisode.begin());
                q.stepsPerEpisode.erase(q.stepsPerEpisode.begin());
            }
            windowSteps=0; windowReturn=0;
        }
        return true;
    }
};
} // namespace v27learn
namespace v27dte {
struct ControllerProposal { float forward{0},turn{0},uncertainty{.2f}; bool learned{false}; };
class LocalNavigation {
    std::mt19937 rng_{27001};
    uint32_t escapeTicks_{0};
    uint32_t avoidHoldTicks_{0};
    float avoidTurnDir_{0.46f};
public:
    uint64_t proposals{0},learnedProposals{0},accepted{0},contacts{0};
    void reset(){*this=LocalNavigation{};}
    ControllerProposal propose(const v15render::VisualNavEvidence& v,
            const avatar3d::NavTrainer& q,const v27learn::OnlineNavigationObserver& learning,
            bool contact,int questMode=0,float goalCompass=10.0f) {
        ControllerProposal p;
        if(v.sourceSequence==0 || !v27learn::OnlineNavigationObserver::valid(v.state))return p;
        ++proposals;
        if(contact) {
            ++contacts;
            if (escapeTicks_ == 0)
                avoidTurnDir_ = (v.state.obst < 4) ? 0.48f : -0.48f;
            escapeTicks_ = 10;
        }
        // LW35: decisive non-oscillating unstick + arc around obstacles.
        if(escapeTicks_) {
            --escapeTicks_;
            p.forward = (escapeTicks_ >= 7) ? -0.18f : 0.24f;
            p.turn = avoidTurnDir_;
            return p;
        }
        if(v.obstacleVisible && v.nearestObstacleDepth<2.4f) {
            if (avoidHoldTicks_ == 0) {
                avoidTurnDir_ = (v.state.obst < 4) ? 0.46f : -0.46f;
                avoidHoldTicks_ = 12;
            } else {
                --avoidHoldTicks_;
            }
            p.turn = avoidTurnDir_;
            p.forward = v.nearestObstacleDepth < 1.1f ? 0.14f : 0.25f;
            return p;
        }
        if (avoidHoldTicks_ > 0) --avoidHoldTicks_;
        if (std::fabs(goalCompass) <= 3.1416f) {
            // LW38: authoritative egocentric bearing to the active quest target
            // (Ariadne, Signal Shard, Civic Beacon, or uncollected Data Cell).
            // Never let random ObjectNpc walkers hijack questMode==0 steering!
            // Pivot tightly when >25 deg off-heading so we never orbit past targets.
            p.turn = std::clamp(goalCompass * 0.90f, -0.55f, 0.55f);
            p.forward = std::fabs(goalCompass) > 0.45f ? 0.08f : 0.46f;
        } else if(questMode==1&&v.shardVisible) {
            const float bearing=(v.shardBearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else if(questMode==0&&v.npcVisible) {
            const float bearing=(v.npcBearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else if(v.goalVisible) {
            const float bearing=(v.state.bearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else {
            p.forward=.24f;
            const int b=v.state.bearing;
            const int bl=(b+7)&7, br=(b+1)&7;
            const uint32_t vl=learning.stateVisits[bl][v.state.dist][v.state.obst];
            const uint32_t vr=learning.stateVisits[br][v.state.dist][v.state.obst];
            p.turn=(vl<vr)?-.30f:(vr<vl)?.30f:.28f;
        }
        // LW38: blend visited Q-policy choices with continuous goal steering
        // only when already roughly aligned with the target (|goalCompass|<0.45)
        // so Q-discretization never overrides pivot-to-target alignment.
        if(learning.stateVisits[v.state.bearing][v.state.dist][v.state.obst]>=8) {
            std::uniform_real_distribution<float> unit(0,1);
            if(unit(rng_)>q.epsilon) {
                float best=-1e30f;int chosen=-1;
                for(int a=0;a<avatar3d::NavTrainer::COUNT;++a)
                    if(learning.actionVisits[v.state.bearing][v.state.dist][v.state.obst][a]>0 &&
                       q.Q[v.state.bearing][v.state.dist][v.state.obst][a]>best) {
                        best=q.Q[v.state.bearing][v.state.dist][v.state.obst][a];chosen=a;
                    }
                if(chosen>=0) {
                    using N=avatar3d::NavTrainer;
                    const auto a=static_cast<N::Action>(chosen);
                    const float qFwd=(a==N::FWD||a==N::FWD_L||a==N::FWD_R)?0.46f:0.24f;
                    const float qTurn=(a==N::TURN_L||a==N::FWD_L)?-0.40f:(a==N::TURN_R||a==N::FWD_R)?0.40f:0.0f;
                    const float w = (std::fabs(goalCompass) > 0.45f) ? 0.05f : ((best > 0.0f) ? 0.25f : 0.10f);
                    p.forward = std::clamp((1.0f - w) * p.forward + w * qFwd, 0.08f, 0.48f);
                    p.turn    = std::clamp((1.0f - w) * p.turn    + w * qTurn, -0.55f, 0.55f);
                    p.learned=true;++learnedProposals;
                }
            }
        }
        return p;
    }
};
// Echo persona (superhot-gamer-girl personas/echo.json): resolves the live
// 10-channel endocrine bus into the persona emotion vocabulary used by
// echo.character.json endocrine.event_map / echo.live2d.json expressions.
// Pure classification: it never proposes, vetoes, or actuates anything.
struct EchoPersonaState {
    const char* emotion{"BORED_STOMPING"};   // echo.live2d.json key
    const char* expression{"PLAYFUL_BORED"};// Cubism expression name
    const char* facs{"AU43 AU15"};          // FACS AU set (display contract)
    float spice{0.0f};                      // resolved adaptive spice 0..1
    float confidence{0.0f};                // 1 - margin to nearest boundary
    uint64_t updates{0};
};
class EchoPersona {
public:
    EchoPersonaState resolve(double valence,double arousal,int cognitiveMode) {
        // Thresholds mirror dte::EndocrineSystem::computeCognitiveMode boundaries.
        const bool alarmed = cognitiveMode==3; // Alarm
        const bool resting  = cognitiveMode==2; // Rest
        const bool focused = cognitiveMode==1;  // Exploitation
        const char* emotion; const char* expression; const char* facs; float spice;
        if (alarmed && valence<-0.1) { emotion="GETTING_TILTED"; expression="FRUSTRATED_FOCUS"; facs="AU4 AU7 AU23"; spice=0.40f; }
        else if (valence>0.35 && arousal>0.15) { emotion="VICTORY_ROYALE"; expression="TRIUMPHANT_GRIN"; facs="AU6 AU12 AU25"; spice=0.40f; }
        else if (valence>0.35 && arousal<=0.15) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else if (valence>0.15 && arousal>0.25) { emotion="CLUTCH_MOMENT"; expression="FOCUSED_INTENSE"; facs="AU4 AU5 AU7"; spice=0.40f; }
        else if (valence>0.15 && focused) { emotion="FLOW_STATE"; expression="SERENE_FOCUS"; facs="AU43 AU6"; spice=0.30f; }
        else if (valence>0.15) { emotion="RESONANT_SYNC"; expression="ATTUNED_SMILE"; facs="AU6 AU12 AU45"; spice=0.30f; }
        else if (valence<-0.15 && arousal>0.3) { emotion="SURPRISE_ATTACK"; expression="STARTLED_ALERT"; facs="AU1 AU2 AU5 AU26"; spice=0.40f; }
        else if (valence<-0.05 && !resting) { emotion="BRANCH_DIVERGE"; expression="MISCHIEVOUS_BRANCH"; facs="AU12R AU2 AU5"; spice=0.50f; }
        else if (resting) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else { emotion="BORED_STOMPING"; expression="PLAYFUL_BORED"; facs="AU43 AU15"; spice=0.30f; }
        // Confidence = distance to the nearest decision boundary (saturated).
        const double m1=std::fabs(valence-0.35),m2=std::fabs(valence-0.15),m3=std::fabs(valence+0.05),
                     m4=std::fabs(arousal-0.15),m5=std::fabs(arousal-0.25);
        const double margin=std::min({m1,m2,m3,m4,m5});
        state_.emotion=emotion; state_.expression=expression; state_.facs=facs; state_.spice=spice;
        state_.confidence=(float)std::min(1.0,margin/0.20);
        ++state_.updates;
        return state_;
    }
    const EchoPersonaState& state() const { return state_; }
private:
    EchoPersonaState state_{};
};

} // namespace v27dte
extern "C" {
#include "network.h"
#include "error.h"
#include "gradient_descent.h"
#include "rnd.h"
#include "feedforward.h"
#include "save.h"
#include "load.h"
}


namespace v27gnn {

static constexpr int FeatureSize = 20;
static constexpr int HiddenSize = 16;
static constexpr int OutputSize = 2; // [0]=turn (right+), [1]=forward
static constexpr int MaxSamples = 256; // rolling demonstration buffer
static constexpr int TrainBatch = 16;  // most recent samples per pass
static constexpr int TrainEveryFrames = 20;

class GneuralMotorPolicy {
public:
    GneuralMotorPolicy() = default;
    ~GneuralMotorPolicy() { destroy(); }

    GneuralMotorPolicy(const GneuralMotorPolicy&) = delete;
    GneuralMotorPolicy& operator=(const GneuralMotorPolicy&) = delete;

    // Build the 20->16->2 tanh/linear network with small random weights.
    bool create() {
        if (net_) return true;
        net_ = network_alloc();
        if (!net_) return false;
        network_set_neuron_number(net_, FeatureSize + HiddenSize + OutputSize);
        network_set_layer_number(net_, 3);
        net_->layers[0].neurons = &net_->neurons[0];
        net_->layers[0].num_of_neurons = FeatureSize;
        net_->layers[1].neurons = &net_->neurons[FeatureSize];
        net_->layers[1].num_of_neurons = HiddenSize;
        net_->layers[2].neurons = &net_->neurons[FeatureSize + HiddenSize];
        net_->layers[2].num_of_neurons = OutputSize;
        for (int i = 0; i < FeatureSize; ++i) {
            net_->neurons[i].num_input = 0; // input neurons
        }
        for (int h = 0; h < HiddenSize; ++h) {
            neuron* ne = &net_->neurons[FeatureSize + h];
            network_neuron_set_connection_number(ne, FeatureSize);
            for (int c = 0; c < FeatureSize; ++c) {
                ne->connection[c] = &net_->neurons[c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        for (int o = 0; o < OutputSize; ++o) {
            neuron* ne = &net_->neurons[FeatureSize + HiddenSize + o];
            network_neuron_set_connection_number(ne, HiddenSize);
            for (int c = 0; c < HiddenSize; ++c) {
                ne->connection[c] = &net_->neurons[FeatureSize + c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        return true;
    }

    // Forward pass: features in, (turn, forward) out.
    bool infer(const float* features, float& turnOut, float& forwardOut) const {
        if (!net_) return false;
        for (int i = 0; i < FeatureSize; ++i)
            net_->neurons[i].output = features[i];
        feedforward(net_);
        turnOut = (float)net_->neurons[FeatureSize + HiddenSize].output;
        forwardOut = (float)net_->neurons[FeatureSize + HiddenSize + 1].output;
        return true;
    }

    // Store one demonstration pair in the rolling buffer.
    void remember(const float* features, float turn, float forward) {
        Sample s;
        for (int i = 0; i < FeatureSize; ++i) s.x[i] = features[i];
        s.y[0] = turn;
        s.y[1] = forward;
        if ((int)samples_.size() >= MaxSamples) samples_.erase(samples_.begin());
        samples_.push_back(s);
        ++newSamples_;
    }

    int pendingSamples() const { return newSamples_; }
    int totalSamples() const { return (int)samples_.size(); }
    uint64_t trainingUpdates() const { return trainingUpdates_; }
    uint64_t trainingSteps() const { return trainingSteps_; }
    float lastLoss() const { return lastLoss_; }

    // One online gradient-descent pass over the rolling buffer. Budgeted to
    // a few milliseconds: gamma=0.08, maxiter=8 (empirically stable).
    bool train() {
        if (!net_ || samples_.empty()) return false;
        const int n = (int)samples_.size();
        if (!cfg_) {
            cfg_ = network_config_alloc_default();
            if (!cfg_) return false;
            // alloc_default pre-ensures only 32 neurons x 16 fan-in; our net
            // is 38 neurons with fan-in 20 - grow the point buffers explicitly.
            network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
            cfg_->verbosity = OFF;
            cfg_->optimization_type = GRADIENT_DESCENT;
            cfg_->error_type = L2;
            // gamma set per-pass by the adaptive schedule below
            cfg_->maxiter = 1;
            cfg_->accuracy = 1e-4;
            cfg_->wmin = -1.0;
            cfg_->wmax = 1.0;
            cfg_->nxw = 1000;
        }
        // Train on the most recent TrainBatch samples: a full 256-point
        // pass costs ~600ms (central-difference gradient_descent), while a
        // 16-point single-iteration pass costs ~5ms - safe for the frame
        // loop. Older samples still shape the network across passes.
        const int batch = n < TrainBatch ? n : TrainBatch;
        for (int p = 0; p < batch; ++p) {
            const Sample& s = samples_[n - batch + p];
            for (int i = 0; i < FeatureSize; ++i)
                NET_PX(cfg_, p, i, 0) = s.x[i];
            for (int o = 0; o < OutputSize; ++o)
                NET_PY(cfg_, p, FeatureSize + HiddenSize + o) = s.y[o];
        }
        cfg_->num_points = batch; // training point count (error() reads this)
        // Adaptive learning rate: sqrt-shaped L2 error gives an almost
        // constant gradient magnitude near the optimum, so a fixed gamma
        // oscillates forever. Halve gamma when a pass fails to improve,
        // reset when it does - converges and stays responsive to new data.
        cfg_->gamma = gamma_;
        gradient_descent(net_, cfg_);
        const double after = error(net_, cfg_);
        if (after < lastError_ * 0.999 || newSamples_ > 0) gamma_ = 0.01; // fresh data re-explores
        else gamma_ = (gamma_ > 2e-4) ? gamma_ * 0.5 : 1e-4; // floor keeps learning alive
        lastError_ = after;
        lastLoss_ = (float)after;
        trainingSteps_ += (uint64_t)cfg_->maxiter;
        ++trainingUpdates_;
        newSamples_ = 0;
        return true;
    }

    bool save(const std::string& path) {
        if (!net_ || !cfg_) return false;
        if (cfg_->save_network_file_name) free(cfg_->save_network_file_name);
        cfg_->save_network_file_name = dup_(path.c_str());
        cfg_->save_neural_network = ON;
        network_save(net_, cfg_); // void; exits the process on I/O failure
        cfg_->save_neural_network = OFF;
        return true;
    }

    bool load(const std::string& path) {
        // network_load() exits the process on a missing file; guard first.
        if (!std::filesystem::exists(path)) return false;
        if (!net_ && !create()) return false;
        if (!cfg_ && !(cfg_ = network_config_alloc_default())) return false;
        network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
        cfg_->verbosity = OFF;
        if (cfg_->load_network_file_name) free(cfg_->load_network_file_name);
        cfg_->load_network_file_name = dup_(path.c_str());
        cfg_->load_neural_network = ON;
        network_load(net_, cfg_);
        cfg_->load_neural_network = OFF;
        return true;
    }

private:
    struct Sample {
        float x[FeatureSize];
        float y[OutputSize];
    };

    static char* dup_(const char* s) {
        char* d = (char*)malloc(strlen(s) + 1);
        if (d) strcpy(d, s);
        return d;
    }

    void destroy() {
        if (cfg_) { network_config_free(cfg_); cfg_ = nullptr; }
        if (net_) { network_free(net_); net_ = nullptr; }
    }

    network* net_{nullptr};
    network_config* cfg_{nullptr};
    std::vector<Sample> samples_;
    int newSamples_{0};
    uint64_t trainingUpdates_{0};
    uint64_t trainingSteps_{0};
    float lastLoss_{-1.0f};
    double gamma_{0.01};
    double lastError_{1e9};
};

} // namespace v27gnn
