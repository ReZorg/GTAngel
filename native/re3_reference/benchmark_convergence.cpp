// ═══ GTAngel v13 Extended Q-Learning Convergence Benchmark ═══
// 300 episodes per scenario with per-episode metrics for convergence analysis.
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
    printf("=== GTAngel v13 Extended Q-Learning Convergence Benchmark ===\n\n");

    struct Scenario { const char* name; int pillars; uint32_t seed; };
    Scenario scenarios[] = {
        {"sparse", 6, 1337},
        {"medium", 14, 1337},
        {"dense", 24, 1337},
    };

    std::ofstream csv("nav_convergence.csv");
    csv << "scenario,episode,return,steps,success,collision,mean_contradiction,epsilon,"
        << "cumulative_success_rate,cumulative_collision_rate,moving_avg_return_20\n";

    for (auto& sc : scenarios) {
        printf("--- %s (%d pillars) ---\n", sc.name, sc.pillars);
        GameWorld world;
        world.generate(sc.seed, sc.pillars);
        AvatarSkeleton avatar;
        EgocentricRenderer renderer;
        TrizMapping mapper;
        NavTrainer trainer;

        std::vector<float> returns;
        std::vector<int> steps;
        std::vector<bool> successes;
        std::vector<int> collisions;
        std::vector<float> contras;

        const int EPISODES = 300;
        auto t0 = std::chrono::steady_clock::now();

        for (int ep = 0; ep < EPISODES; ep++) {
            float contra = 0;
            int prevCollisions = trainer.collisions;
            int prevSuccesses = trainer.successes;
            float ret = trainer.episode(world, avatar, &renderer, &mapper, 300, &contra);

            returns.push_back(ret);
            steps.push_back(trainer.stepsPerEpisode.back());
            successes.push_back(trainer.successes > prevSuccesses);
            collisions.push_back(trainer.collisions - prevCollisions);
            contras.push_back(contra);

            // Moving average return (20-episode window)
            float ma = 0;
            int w = std::min(20, (int)returns.size());
            for (int i = (int)returns.size() - w; i < (int)returns.size(); i++) ma += returns[i];
            ma /= w;

            csv << sc.name << "," << ep << "," << ret << "," << steps.back() << ","
                << (successes.back() ? 1 : 0) << "," << collisions.back() << ","
                << contra << "," << trainer.epsilon << ","
                << trainer.successRate() << "," << trainer.collisionRate() << "," << ma << "\n";

            if (ep % 50 == 49) {
                printf("  ep %3d: success=%.1f%% collision=%.4f ret=%.2f MA20=%.2f contra=%.4f eps=%.3f\n",
                    ep + 1, trainer.successRate() * 100, trainer.collisionRate(),
                    ret, ma, contra, trainer.epsilon);
            }
        }

        double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

        // Convergence analysis
        int firstSuccess = -1, firstConverged = -1;
        for (int i = 0; i < EPISODES; i++) {
            if (firstSuccess < 0 && successes[i]) firstSuccess = i;
            // Convergence: 10 successes in last 20 episodes
            if (i >= 19) {
                int recentSuccesses = 0;
                for (int j = i - 19; j <= i; j++) if (successes[j]) recentSuccesses++;
                if (firstConverged < 0 && recentSuccesses >= 10) firstConverged = i;
            }
        }

        // Final 50-episode stats
        float finalSuccess = 0, finalRet = 0, finalSteps = 0, finalContra = 0;
        for (int i = EPISODES - 50; i < EPISODES; i++) {
            finalSuccess += successes[i] ? 1 : 0;
            finalRet += returns[i];
            finalSteps += steps[i];
            finalContra += contras[i];
        }
        finalSuccess /= 50; finalRet /= 50; finalSteps /= 50; finalContra /= 50;

        printf("  CONVERGENCE: firstSuccess=ep%d firstConverged=ep%d\n", firstSuccess, firstConverged);
        printf("  FINAL 50: success=%.0f%% ret=%.2f steps=%.0f contra=%.4f\n",
            finalSuccess * 100, finalRet, finalSteps, finalContra);
        printf("  TIME: %.1fs (%.1f ep/s)\n\n", secs, EPISODES / secs);
    }
    csv.close();
    printf("Convergence data exported to nav_convergence.csv\n");
    return 0;
}
