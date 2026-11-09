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
        float swing = std::sin(walkPhase) * std::min(1.0f, speed * 2.0f);
        float bob = std::fabs(std::cos(walkPhase)) * 0.05f * std::min(1.0f, speed * 2.0f);
        float ch = std::cos(heading), sh = std::sin(heading);
        auto R = [&](Vec3 v) { return Vec3{v.x*ch - v.z*sh, v.y, v.x*sh + v.z*ch} + position; };
        joints[Root]      = R({0, 0.95f + bob, 0});
        joints[Spine]     = R({0, 1.25f + bob, 0});
        joints[Neck]      = R({0, 1.50f + bob, 0});
        joints[Head]      = R({0, 1.70f + bob, 0});
        joints[ShoulderL] = R({-0.22f, 1.45f + bob, 0});
        joints[ElbowL]    = R({-0.28f, 1.15f + bob, swing * 0.25f});
        joints[HandL]     = R({-0.30f, 0.85f + bob, swing * 0.45f});
        joints[ShoulderR] = R({0.22f, 1.45f + bob, 0});
        joints[ElbowR]    = R({0.28f, 1.15f + bob, -swing * 0.25f});
        joints[HandR]     = R({0.30f, 0.85f + bob, -swing * 0.45f});
        joints[HipL]      = R({-0.12f, 0.95f + bob, 0});
        joints[KneeL]     = R({-0.14f, 0.50f + bob, -swing * 0.30f});
        joints[FootL]     = R({-0.15f, 0.05f, -swing * 0.55f});
        joints[HipR]      = R({0.12f, 0.95f + bob, 0});
        joints[KneeR]     = R({0.14f, 0.50f + bob, swing * 0.30f});
        joints[FootR]     = R({0.15f, 0.05f, swing * 0.55f});
    }
};

struct GameWorld {
    struct Pillar { Vec3 pos; float radius; float height; };
    std::vector<Pillar> pillars;
    Vec3 goal{8.0f, 0.0f, 8.0f};
    float worldSize{20.0f};
    void generate(uint32_t seed, int count) {
        pillars.clear();
        uint32_t s = seed;
        auto rnd = [&]() { s = s * 1664525u + 1013904223u; return (s >> 8) / 16777216.0f; };
        for (int i = 0; i < count; ++i) {
            Pillar p{{(rnd()-0.5f)*worldSize, 0.0f, (rnd()-0.5f)*worldSize},
                     0.4f+rnd()*0.5f, 1.5f+rnd()*2.0f};
            if (len(p.pos) < 3.0f || len(p.pos-goal) < 3.0f) { --i; continue; }
            pillars.push_back(p);
        }
    }
};

struct EgocentricRenderer {
    std::vector<float> frame, zbuf, semantic;
};

struct TrizMapping {
    static constexpr int GRID = 12;
    float aesthetic[GRID][GRID]{};
    float contradiction[GRID][GRID]{};
    float goalSalience[GRID][GRID]{};
    float obstacleSalience[GRID][GRID]{};
    float meanContradiction{0.0f}, maxContradiction{0.0f};
    int maxQ{-1}, maxR{-1};
};

} // namespace avatar3d

#include "gtangel_agi_v15_hires_renderer.cpp"

int main() {
    avatar3d::GameWorld world;
    avatar3d::AvatarSkeleton avatar;
    avatar3d::EgocentricRenderer legacy;
    avatar3d::TrizMapping triz;
    v15render::HighResRenderer renderer;
    v15render::PerceptionPyramid pyramid;
    world.generate(1337, 14);

    constexpr int frames = 90;
    double totalMs = 0.0;
    double minMs = std::numeric_limits<double>::max();
    double maxMs = 0.0;
    int rasterized = 0;
    int finiteDepth = 0;
    int semanticPixels = 0;

    for (int frame = 0; frame < frames; ++frame) {
        float heading = frame * 0.017f;
        avatar3d::Vec3 position{std::sin(heading) * 1.4f, 0.0f, std::cos(heading) * 1.4f};
        avatar.pose(position, heading, 1.0f, 0.05f);
        avatar3d::Vec3 eye = position + avatar3d::Vec3{-std::sin(heading) * 4.0f, 2.5f,
                                                        -std::cos(heading) * 4.0f};
        renderer.render(world, avatar, eye, heading, -0.4f, true);
        pyramid.derive(renderer.framebuffer);
        pyramid.copyToLegacy(legacy);
        pyramid.mapToTriz(triz);
        totalMs += renderer.metrics.renderMs;
        minMs = std::min(minMs, renderer.metrics.renderMs);
        maxMs = std::max(maxMs, renderer.metrics.renderMs);
        rasterized += renderer.metrics.trianglesRasterized;
    }

    for (int i = 0; i < v15render::HighResFramebuffer::PixelCount; ++i) {
        if (renderer.framebuffer.depth[i] < 1e20f) ++finiteDepth;
        if (renderer.framebuffer.objectId[i] != v15render::ObjectSky) ++semanticPixels;
    }

    bool valid = true;
    valid = valid && renderer.framebuffer.colour.size() == 768u * 768u;
    valid = valid && renderer.framebuffer.depth.size() == 768u * 768u;
    valid = valid && renderer.framebuffer.objectId.size() == 768u * 768u;
    valid = valid && legacy.frame.size() == 64u * 64u;
    valid = valid && legacy.zbuf.size() == 64u * 64u;
    valid = valid && legacy.semantic.size() == 64u * 64u;
    valid = valid && pyramid.retina.sourceSequence == renderer.framebuffer.sequence;
    valid = valid && finiteDepth > 0 && semanticPixels > 0;
    valid = valid && renderer.metrics.trianglesSubmitted > 0;
    valid = valid && renderer.metrics.trianglesRasterized > 0;
    valid = valid && triz.maxQ >= 0 && triz.maxR >= 0;

    const double averageMs = totalMs / frames;
    const double framesPerSecond = averageMs > 0.0 ? 1000.0 / averageMs : 0.0;
    auto writeTarget = [&](const char* filename, v15render::DisplayMode mode) {
        std::ofstream image(filename, std::ios::binary);
        image << "P6\n768 768\n255\n";
        const uint32_t* pixels = renderer.framebuffer.pixelsFor(mode);
        for (int index = 0; index < v15render::HighResFramebuffer::PixelCount; ++index) {
            v15render::Colour colour = v15render::HighResFramebuffer::unpack(pixels[index]);
            image.put((char)colour.r);
            image.put((char)colour.g);
            image.put((char)colour.b);
        }
        image.close();
    };
    writeTarget("v15_frame.ppm", v15render::DisplayMode::Colour);
    writeTarget("v15_depth.ppm", v15render::DisplayMode::Depth);
    writeTarget("v15_object_id.ppm", v15render::DisplayMode::ObjectIdentity);
    writeTarget("v15_luminance.ppm", v15render::DisplayMode::Luminance);

    std::ofstream report("v15_benchmark.json");
    report << "{\n"
           << "  \"valid\": " << (valid ? "true" : "false") << ",\n"
           << "  \"frames\": " << frames << ",\n"
           << "  \"source_resolution\": \"768x768\",\n"
           << "  \"retina_resolution\": \"64x64\",\n"
           << "  \"cognitive_resolution\": \"12x12\",\n"
           << "  \"average_render_ms\": " << averageMs << ",\n"
           << "  \"minimum_render_ms\": " << minMs << ",\n"
           << "  \"maximum_render_ms\": " << maxMs << ",\n"
           << "  \"estimated_fps\": " << framesPerSecond << ",\n"
           << "  \"triangles_submitted\": " << renderer.metrics.trianglesSubmitted << ",\n"
           << "  \"average_triangles_rasterized\": " << (double)rasterized / frames << ",\n"
           << "  \"finite_depth_pixels\": " << finiteDepth << ",\n"
           << "  \"semantic_pixels\": " << semanticPixels << ",\n"
           << "  \"mean_luminance\": " << pyramid.retina.meanLuminance << ",\n"
           << "  \"mean_motion\": " << pyramid.retina.meanMotion << ",\n"
           << "  \"mean_edge_energy\": " << pyramid.retina.meanEdge << ",\n"
           << "  \"mean_contradiction\": " << triz.meanContradiction << ",\n"
           << "  \"frame_artifacts\": [\"v15_frame.ppm\", \"v15_depth.ppm\", \"v15_object_id.ppm\", \"v15_luminance.ppm\"]\n"
           << "}\n";
    report.close();

    std::cout << "GTAngel v15 render-pyramid benchmark\n"
              << "valid=" << (valid ? "true" : "false")
              << " average_ms=" << averageMs
              << " estimated_fps=" << framesPerSecond
              << " triangles=" << renderer.metrics.trianglesSubmitted
              << " finite_depth=" << finiteDepth
              << " semantic_pixels=" << semanticPixels << "\n";
    return valid ? 0 : 1;
}
