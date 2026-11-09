#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
using SHORT=short;
static constexpr int MOVEFILE_REPLACE_EXISTING=1, MOVEFILE_WRITE_THROUGH=8;
static SHORT GetAsyncKeyState(int) { return 0; }
static int DeleteFileA(const char* p) { return std::remove(p)==0; }
static int MoveFileExA(const char* a,const char* b,int) { std::remove(b); return std::rename(a,b)==0; }

namespace avatar3d {
struct Vec3 { float x{0}, y{0}, z{0}; };
static Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
static float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static float len(Vec3 a) { return std::sqrt(dot(a, a)); }

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

struct GameWorld {
    struct Pillar { Vec3 pos; float radius; float height; };
    std::vector<Pillar> pillars;
    Vec3 goal{2.5f, 0.0f, 7.0f};
    Vec3 npcPos{0.0f, 0.0f, 5.0f};
    float npcHeading{3.14159265f};
    float npcSpeed{0.0f};
    Vec3 questShard{-2.2f, 0.0f, 5.2f};
    bool questShardVisible{true};
    bool beaconRestored{false};
    bool questComplete{false};
    Vec3 vehiclePos{0.8f, 0.0f, 2.2f};
    float vehicleHeading{0.0f};
    float vehicleWheelPhase{0.0f};
    bool vehicleOccupied{false};
    float worldSize{128.0f};
    void generate(uint32_t seed, int count) {
        pillars.clear(); uint32_t s = seed;
        auto rnd = [&]() { s=s*1664525u+1013904223u; return (s>>8)/16777216.0f; };
        for (int i=0;i<count;++i) {
            Pillar p{{(rnd()-.5f)*worldSize,0,(rnd()-.5f)*worldSize},.4f+rnd()*.5f,1.5f+rnd()*2.0f};
            if (len(p.pos)<3.0f || len(p.pos-goal)<3.0f || len(p.pos-npcPos)<2.0f) { --i; continue; }
            pillars.push_back(p);
        }
    }
};
struct EgocentricRenderer { std::vector<float> frame,zbuf,semantic; };
struct TrizMapping { static constexpr int GRID=12; float aesthetic[GRID][GRID]{}, contradiction[GRID][GRID]{}, goalSalience[GRID][GRID]{}, obstacleSalience[GRID][GRID]{}; float meanContradiction{0},maxContradiction{0}; int maxQ{-1},maxR{-1}; };
struct NavTrainer { static constexpr int BEARINGS=8,DISTS=4,OBST=8; struct State{int bearing,dist,obst;}; };
}

#include <cstring>
#include "gtangel_agi_v24_renderer.hpp"
#include "gtangel_agi_v20_gameplay.hpp"
#include "gtangel_agi_v20_cns.hpp"

static void writeTarget(const char* filename, v15render::HighResFramebuffer& framebuffer,
                        v15render::DisplayMode mode) {
    std::ofstream image(filename, std::ios::binary);
    image << "P6\n768 768\n255\n";
    const uint32_t* pixels = framebuffer.pixelsFor(mode);
    for (int index=0; index<v15render::HighResFramebuffer::PixelCount; ++index) {
        const auto colour = v15render::HighResFramebuffer::unpack(pixels[index]);
        image.put((char)colour.r); image.put((char)colour.g); image.put((char)colour.b);
    }
}

static int countObject(const v15render::HighResFramebuffer& framebuffer, uint16_t id) {
    return (int)std::count(framebuffer.objectId.begin(), framebuffer.objectId.end(), id);
}

int main() {
    avatar3d::GameWorld world; world.generate(1337, 8);
    avatar3d::AvatarSkeleton player; player.pose({0,0,0},0,0,0.05f);
    v15render::HighResRenderer renderer;
    v15render::PerceptionPyramid pyramid;
    constexpr int frames=60;
    double totalMs=0.0;
    for (int i=0;i<frames;++i) {
        player.pose({0,0,0},0,0,0.05f);
        renderer.render(world, player, {0,2.25f,-4.2f}, 0.0f, -0.22f, true);
        pyramid.derive(renderer.framebuffer);
        totalMs += renderer.metrics.renderMs;
    }
    const int npcPixelsBroken = countObject(renderer.framebuffer, v15render::ObjectNpc);
    const int shardPixelsBroken = countObject(renderer.framebuffer, v15render::ObjectQuestItem);
    const int goalPixelsBroken = countObject(renderer.framebuffer, v15render::ObjectGoal);
    const int vehiclePixelsBroken = countObject(renderer.framebuffer, v15render::ObjectVehicle);
    const auto vehicleIt=std::find(renderer.framebuffer.objectId.begin(),renderer.framebuffer.objectId.end(),
                                   v15render::ObjectVehicle);
    const uint32_t identity=vehicleIt==renderer.framebuffer.objectId.end()?0:
        renderer.framebuffer.pixelsFor(v15render::DisplayMode::ObjectIdentity)[
            vehicleIt-renderer.framebuffer.objectId.begin()];
    writeTarget("v20_broken_colour.ppm", renderer.framebuffer, v15render::DisplayMode::Colour);
    writeTarget("v20_broken_object_id.ppm", renderer.framebuffer, v15render::DisplayMode::ObjectIdentity);

    world.questShardVisible=false; world.beaconRestored=true; world.questComplete=true;
    renderer.render(world, player, {0,2.25f,-4.2f}, 0.0f, -0.22f, true);
    pyramid.derive(renderer.framebuffer);
    const int npcPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectNpc);
    const int shardPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectQuestItem);
    const int goalPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectGoal);
    writeTarget("v20_restored_colour.ppm", renderer.framebuffer, v15render::DisplayMode::Colour);
    writeTarget("v20_restored_depth.ppm", renderer.framebuffer, v15render::DisplayMode::Depth);
    writeTarget("v20_restored_object_id.ppm", renderer.framebuffer, v15render::DisplayMode::ObjectIdentity);

    avatar3d::GameWorld questWorld; questWorld.generate(1337,8);
    v19game::GameplayLoop quest; quest.reset(questWorld);
    quest.interact({-4,0,2.5f},0,questWorld); quest.choose(1,questWorld);
    quest.interact(questWorld.questShard,0,questWorld);
    const int questStageBefore=(int)quest.quest.stage;
    v20cns::Runtime cns;
    bool frameConsequence=false;
    auto measured=[&](const avatar3d::GameWorld& oldWorld,
                      const avatar3d::GameWorld& newWorld,uint64_t version) {
        auto evidence=[&](const avatar3d::GameWorld& w) {
            v15render::HighResRenderer scope;
            scope.render(w,player,{w.goal.x,2.2f,w.goal.z-4.5f},0.0f,-0.22f,false);
            uint64_t colour=0; int semantic=0,depth=0;
            for (int i=0;i<v15render::HighResFramebuffer::PixelCount;++i)
                if (scope.framebuffer.objectId[i]==v15render::ObjectGoal) {
                    ++semantic; colour+=scope.framebuffer.colour[i];
                    if (std::isfinite(scope.framebuffer.depth[i])) ++depth;
                }
            return std::array<uint64_t,3>{(uint64_t)semantic,(uint64_t)depth,colour};
        };
        auto a=evidence(oldWorld),b=evidence(newWorld);
        frameConsequence=!oldWorld.beaconRestored && newWorld.beaconRestored &&
            a[0]>20 && b[0]>a[0] && b[1]==b[0] && a[2]!=b[2] && version==2;
        return frameConsequence;
    };
    const auto receipt=cns.repair(questWorld.goal,quest,questWorld,
        {renderer.framebuffer.sequence,1000,0.05f,true,false},measured);

    const bool valid = renderer.framebuffer.colour.size()==768u*768u &&
        npcPixelsBroken>40 && shardPixelsBroken>10 && goalPixelsBroken>10 &&
        vehiclePixelsBroken>30 && identity!=v15render::HighResFramebuffer::pack({24,12,20,255}) &&
        npcPixelsComplete>40 && shardPixelsComplete==0 && goalPixelsComplete>goalPixelsBroken &&
        pyramid.retina.sourceSequence==renderer.framebuffer.sequence &&
        receipt==v20cns::WorldGovernance::Verdict::Accepted && frameConsequence &&
        cns.niche().worldVersion==2 && cns.niche().reobserved;
    const double averageMs=totalMs/frames;
    std::ofstream report("v20_render_benchmark.json");
    report << "{\n"
           << "  \"valid\": " << (valid?"true":"false") << ",\n"
           << "  \"frames\": " << frames << ",\n"
           << "  \"source_resolution\": \"768x768\",\n"
           << "  \"client_canvas\": \"1024x768\",\n"
           << "  \"average_render_ms\": " << averageMs << ",\n"
           << "  \"estimated_fps\": " << 1000.0/averageMs << ",\n"
           << "  \"npc_pixels_broken\": " << npcPixelsBroken << ",\n"
           << "  \"vehicle_pixels_broken\": " << vehiclePixelsBroken << ",\n"
           << "  \"niche_frame_reobserved\": " << (frameConsequence?"true":"false") << ",\n"
           << "  \"shard_pixels_broken\": " << shardPixelsBroken << ",\n"
           << "  \"goal_pixels_broken\": " << goalPixelsBroken << ",\n"
           << "  \"npc_pixels_complete\": " << npcPixelsComplete << ",\n"
           << "  \"shard_pixels_complete\": " << shardPixelsComplete << ",\n"
           << "  \"goal_pixels_complete\": " << goalPixelsComplete << "\n"
           << "}\n";
    std::cout << "{\"valid\":" << (valid?"true":"false")
              << ",\"averageMs\":" << averageMs
              << ",\"repairReceipt\":" << (int)receipt
              << ",\"questStageBefore\":" << questStageBefore
              << ",\"frameConsequence\":" << (frameConsequence?"true":"false")
              << ",\"npcPixels\":" << npcPixelsBroken
              << ",\"vehiclePixels\":" << vehiclePixelsBroken
              << ",\"shardPixels\":" << shardPixelsBroken
              << ",\"restoredGoalPixels\":" << goalPixelsComplete << "}\n";
    return valid?0:1;
}
