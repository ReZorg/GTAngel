// ═══ GTAngel v13 Navigation Training Difficulty Benchmark ═══
// Runs real Q-learning episodes with the 3D avatar, egocentric renderer, and
// framebuffer→TRIZ mapping to measure navigation-learning difficulty.
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>

#include "gtangel_agi_v13_avatar.cpp"

using namespace avatar3d;

int main() {
    printf("=== GTAngel v13 Navigation Training Difficulty Benchmark ===\n\n");

    GameWorld world;
    AvatarSkeleton avatar;
    EgocentricRenderer renderer;
    TrizMapping mapper;
    NavTrainer trainer;

    // Difficulty scenarios: increasing obstacle density.
    struct Scenario { const char* name; int pillars; uint32_t seed; };
    Scenario scenarios[] = {
        {"Sparse  (6 pillars)", 6, 1337},
        {"Medium (14 pillars)", 14, 1337},
        {"Dense  (24 pillars)", 24, 1337},
    };

    std::ofstream csv("nav_difficulty.csv");
    csv << "scenario,episode,return,steps,mean_contradiction,success_rate,collision_rate,epsilon\n";

    for (auto& sc : scenarios) {
        printf("--- Scenario: %s ---\n", sc.name);
        world.generate(sc.seed, sc.pillars);
        trainer = NavTrainer(); // fresh trainer per scenario
        float meanContra = 0;
        auto t0 = std::chrono::steady_clock::now();

        const int EPISODES = 60;
        for (int ep = 0; ep < EPISODES; ep++) {
            float ret = trainer.episode(world, avatar, &renderer, &mapper, 300, &meanContra);
            csv << sc.name << "," << ep << "," << ret << ","
                << trainer.stepsPerEpisode.back() << "," << meanContra << ","
                << trainer.successRate() << "," << trainer.collisionRate() << ","
                << trainer.epsilon << "\n";
            if (ep % 20 == 19) {
                printf("  ep %3d: success=%.0f%% collision=%.3f recentRet=%.2f meanContra=%.3f eps=%.2f\n",
                    ep + 1, trainer.successRate() * 100, trainer.collisionRate(),
                    trainer.recentMeanReturn(), meanContra, trainer.epsilon);
            }
        }
        double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        printf("  FINAL: success=%.0f%% collision=%.3f recentRet=%.2f | %d episodes in %.1fs (%.1f ep/s)\n\n",
            trainer.successRate() * 100, trainer.collisionRate(),
            trainer.recentMeanReturn(), EPISODES, secs, EPISODES / secs);
    }
    csv.close();
    printf("Difficulty data exported to nav_difficulty.csv\n");

    // Frame statistics for the final scenario's last render.
    float lumSum = 0, goalPx = 0, obstPx = 0;
    for (size_t i = 0; i < renderer.frame.size(); i++) {
        lumSum += renderer.frame[i];
        if (renderer.semantic[i] > 0.9f) goalPx++;
        else if (renderer.semantic[i] > 0.5f && renderer.semantic[i] < 0.8f) obstPx++;
    }
    printf("\nFinal frame: meanLum=%.3f goalPx=%.0f obstaclePx=%.0f (of %zu)\n",
        lumSum / renderer.frame.size(), goalPx, obstPx, renderer.frame.size());
    printf("TRIZ map: meanContradiction=%.3f maxContradiction=%.3f at cell (%d,%d)\n",
        mapper.meanContradiction, mapper.maxContradiction, mapper.maxQ, mapper.maxR);
    return 0;
}
