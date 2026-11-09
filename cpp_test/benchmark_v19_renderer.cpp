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

namespace avatar3d {
struct Vec3 { float x{0}, y{0}, z{0}; };
static Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
static float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static float len(Vec3 a) { return std::sqrt(dot(a, a)); }

struct AvatarSkeleton {
    enum JointId { Root=0, Spine, Neck, Head,
                   ShoulderL, ElbowL, HandL, ShoulderR, ElbowR, HandR,
                   HipL, KneeL, FootL, HipR, KneeR, FootR, COUNT };
    Vec3 joints[COUNT]{};
    float walkPhase{0.0f};
    void pose(Vec3 position, float heading, float speed, float dt) {
        walkPhase += speed * dt * 6.0f;
        const float swing = std::sin(walkPhase) * std::min(1.0f, speed * 2.0f);
        const float bob = std::fabs(std::cos(walkPhase)) * 0.05f * std::min(1.0f, speed * 2.0f);
        const float ch = std::cos(heading), sh = std::sin(heading);
        auto R = [&](Vec3 v) { return Vec3{v.x*ch - v.z*sh, v.y, v.x*sh + v.z*ch} + position; };
        joints[Root]=R({0,.95f+bob,0}); joints[Spine]=R({0,1.25f+bob,0});
        joints[Neck]=R({0,1.50f+bob,0}); joints[Head]=R({0,1.70f+bob,0});
        joints[ShoulderL]=R({-.22f,1.45f+bob,0}); joints[ElbowL]=R({-.28f,1.15f+bob,swing*.25f});
        joints[HandL]=R({-.30f,.85f+bob,swing*.45f}); joints[ShoulderR]=R({.22f,1.45f+bob,0});
        joints[ElbowR]=R({.28f,1.15f+bob,-swing*.25f}); joints[HandR]=R({.30f,.85f+bob,-swing*.45f});
        joints[HipL]=R({-.12f,.95f+bob,0}); joints[KneeL]=R({-.14f,.50f+bob,-swing*.30f});
        joints[FootL]=R({-.15f,.05f,-swing*.55f}); joints[HipR]=R({.12f,.95f+bob,0});
        joints[KneeR]=R({.14f,.50f+bob,swing*.30f}); joints[FootR]=R({.15f,.05f,swing*.55f});
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
    float worldSize{20.0f};
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

#include "gtangel_agi_v19_hires_renderer.cpp"

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
    writeTarget("v19_broken_colour.ppm", renderer.framebuffer, v15render::DisplayMode::Colour);
    writeTarget("v19_broken_object_id.ppm", renderer.framebuffer, v15render::DisplayMode::ObjectIdentity);

    world.questShardVisible=false; world.beaconRestored=true; world.questComplete=true;
    renderer.render(world, player, {0,2.25f,-4.2f}, 0.0f, -0.22f, true);
    pyramid.derive(renderer.framebuffer);
    const int npcPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectNpc);
    const int shardPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectQuestItem);
    const int goalPixelsComplete = countObject(renderer.framebuffer, v15render::ObjectGoal);
    writeTarget("v19_restored_colour.ppm", renderer.framebuffer, v15render::DisplayMode::Colour);
    writeTarget("v19_restored_depth.ppm", renderer.framebuffer, v15render::DisplayMode::Depth);
    writeTarget("v19_restored_object_id.ppm", renderer.framebuffer, v15render::DisplayMode::ObjectIdentity);

    const bool valid = renderer.framebuffer.colour.size()==768u*768u &&
        npcPixelsBroken>40 && shardPixelsBroken>10 && goalPixelsBroken>10 &&
        npcPixelsComplete>40 && shardPixelsComplete==0 && goalPixelsComplete>goalPixelsBroken &&
        pyramid.retina.sourceSequence==renderer.framebuffer.sequence;
    const double averageMs=totalMs/frames;
    std::ofstream report("v19_render_benchmark.json");
    report << "{\n"
           << "  \"valid\": " << (valid?"true":"false") << ",\n"
           << "  \"frames\": " << frames << ",\n"
           << "  \"source_resolution\": \"768x768\",\n"
           << "  \"client_canvas\": \"1024x768\",\n"
           << "  \"average_render_ms\": " << averageMs << ",\n"
           << "  \"estimated_fps\": " << 1000.0/averageMs << ",\n"
           << "  \"npc_pixels_broken\": " << npcPixelsBroken << ",\n"
           << "  \"shard_pixels_broken\": " << shardPixelsBroken << ",\n"
           << "  \"goal_pixels_broken\": " << goalPixelsBroken << ",\n"
           << "  \"npc_pixels_complete\": " << npcPixelsComplete << ",\n"
           << "  \"shard_pixels_complete\": " << shardPixelsComplete << ",\n"
           << "  \"goal_pixels_complete\": " << goalPixelsComplete << "\n"
           << "}\n";
    std::cout << "{\"valid\":" << (valid?"true":"false")
              << ",\"averageMs\":" << averageMs
              << ",\"npcPixels\":" << npcPixelsBroken
              << ",\"shardPixels\":" << shardPixelsBroken
              << ",\"restoredGoalPixels\":" << goalPixelsComplete << "}\n";
    return valid?0:1;
}
