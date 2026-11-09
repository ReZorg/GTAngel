#include "verify_v56_components.hpp"
// LW19 visual verification: dump the real 768x768 renderer framebuffer for
// close-up views of each traffic exemplar (car x2, bike, train, heli, street).
static void dumpPpm(const v15render::HighResRenderer& r, const char* path) {
    FILE* f = std::fopen(path, "wb");
    if (!f) { std::cerr << "cannot open " << path << "\n"; return; }
    const auto& fb = r.framebuffer;
    const int W = 768, H = (int)fb.colour.size() / W;
    std::fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (size_t i = 0; i < fb.colour.size(); ++i) {
        const uint32_t p = fb.colour[i];
        const unsigned char rgb[3] = {
            (unsigned char)((p >> 16) & 0xff),
            (unsigned char)((p >> 8) & 0xff),
            (unsigned char)(p & 0xff)};
        std::fwrite(rgb, 1, 3, f);
    }
    std::fclose(f);
    std::cout << "wrote " << path << "\n";
}
int main() {
    avatar3d::GameWorld world;
    world.generate(1337, 48);
    for (size_t i = 0; i < world.traffic.size(); ++i)
        std::cout << "traffic[" << i << "] pos=(" << world.traffic[i].pos.x << ","
                  << world.traffic[i].pos.y << "," << world.traffic[i].pos.z
                  << ") heading=" << world.traffic[i].heading
                  << " speed=" << world.traffic[i].speed
                  << " orbitR=" << world.traffic[i].orbitR << "\n";
    for (size_t i = 0; i < world.pillars.size() && i < 6; ++i)
        std::cout << "pillar[" << i << "] pos=(" << world.pillars[i].pos.x << ","
                  << world.pillars[i].pos.z << ") r=" << world.pillars[i].radius
                  << " h=" << world.pillars[i].height << "\n";
    avatar3d::AvatarSkeleton avatar;
    avatar.pose({0,0,0}, 0, 0, 0);
    // LW21: advance a walk cycle so baked v25 clip displacements are mid-stride.
    for (int i = 0; i < 22; ++i) avatar.pose({0,0,0}, 0.0f, 1.9f, 1.0f/60.0f);
    v15render::HighResRenderer render;
    if (!render.loadDteAsset("assets/dte_v26_lod.bin"))
        std::cerr << "WARN: 2K skin asset not loaded\n";
    struct View { const char* name; avatar3d::Vec3 eye; float heading; float pitch; bool third; };
    const View views[] = {
        {"view_avatar.ppm",{ 2.6f, 1.3f, 2.8f}, -2.4f, -0.06f, true},   // LW28 avatar (thirdPerson)
        {"view_car1.ppm",  {-24.0f, 1.6f, 1.4f}, -1.5707963f, 0.0f, false}, // look -x at auto1
        {"view_car2.ppm",  { 24.0f, 1.6f, 1.4f},  1.5707963f, 0.0f, false}, // look +x at auto2
        {"view_bike.ppm",  {  0.9f, 1.5f,-25.0f}, 3.14159265f, 0.0f, false}, // look -z at bike1
        {"view_train.ppm", {-44.0f, 1.8f,-20.0f}, -1.5707963f, 0.0f, false}, // look -x at train
        {"view_heli.ppm",  { 27.0f, 9.4f,  0.0f}, -1.5707963f, 0.0f, false}, // look -x at heli (LW20 alt fix)
        {"view_pillar.ppm",{ -8.0f, 1.6f, -6.0f}, -0.78f, 0.0f, false},      // near a pillar base
        {"view_street.ppm",{  0.0f, 8.0f,-34.0f}, 0.0f, -0.12f, false},      // street overview +z
    };
    for (const auto& v : views) {
        render.render(world, avatar, v.eye, v.heading, v.pitch, v.third);
        dumpPpm(render, v.name);
        std::cout << v.name << " tris=" << render.metrics.trianglesSubmitted
                  << " rasterized=" << render.metrics.trianglesRasterized << "\n";
        {
            const auto& fb = render.framebuffer;
            std::map<uint32_t,int> hist;
            int n = 0;
            for (int i = 0; i < (int)fb.colour.size(); ++i)
                if (fb.objectId[i] == 9) { ++n; ++hist[fb.colour[i] & 0xffffff]; }
            std::cout << "  traffic pixels=" << n << "\n";
            std::map<int,int> allIds;
            for (int i = 0; i < (int)fb.colour.size(); ++i) ++allIds[fb.objectId[i]];
            std::cout << "  id hist:";
            for (const auto& kv : allIds) std::cout << " " << kv.first << ":" << kv.second;
            std::cout << "\n";
            int avatarPx = 0;
            for (int i = 0; i < (int)fb.colour.size(); ++i)
                if (fb.objectId[i] == 4) ++avatarPx; // ObjectAvatar semantic band
            std::cout << "  avatar pixels(id=4)=" << avatarPx << "\n";
            int shown = 0;
            for (const auto& kv : hist) {
                if (kv.second < 20 || shown >= 12) continue;
                std::cout << "    rgb=" << ((kv.first>>16)&0xff) << ","
                          << ((kv.first>>8)&0xff) << "," << (kv.first&0xff)
                          << " count=" << kv.second << "\n";
                ++shown;
            }
        }
    }
    return 0;
}
