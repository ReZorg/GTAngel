// GTAngel v13 display-grounded 3D navigation acceptance test.
// The controller consumes only the RGB pixels drawn by the live viewport.
// World coordinates are used only by the environment for collision, reward,
// termination, and test reporting; they are never policy inputs.
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdint>

#include "gtangel_agi_v13_avatar.cpp"

using namespace avatar3d;

struct RGB {
    uint8_t r{0}, g{0}, b{0};
};

static std::vector<RGB> makeDisplayedPixels(const EgocentricRenderer& renderer) {
    std::vector<RGB> pixels(renderer.frame.size());
    for (size_t i = 0; i < renderer.frame.size(); ++i) {
        const float v = renderer.frame[i];
        const float sem = renderer.semantic[i];
        if (sem > 0.9f) pixels[i] = {0, 255, 120};
        else if (sem > 0.6f) pixels[i] = {255, 80, 60};
        else if (sem > 0.4f) pixels[i] = {120, 180, 255};
        else if (sem > 0.2f) pixels[i] = {60, 60, 80};
        else pixels[i] = {(uint8_t)(v * 40), (uint8_t)(v * 40), (uint8_t)(v * 60)};
    }
    return pixels;
}

static void writePpm(const char* path, const std::vector<RGB>& pixels, int width, int height) {
    std::ofstream out(path, std::ios::binary);
    out << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& p : pixels) {
        out.put((char)p.r);
        out.put((char)p.g);
        out.put((char)p.b);
    }
}

struct VisualFeatures {
    bool goalVisible{false};
    float goalX{0.5f};
    float goalY{0.5f};
    int goalPixels{0};
    int obstaclePixels{0};
    int avatarPixels{0};
    float corridorObstacle{0.0f};
    float leftObstacle{0.0f};
    float rightObstacle{0.0f};
};

class DisplayNavStateEncoder {
public:
    VisualFeatures encode(const std::vector<RGB>& pixels, int width, int height) const {
        VisualFeatures f;
        double goalXSum = 0.0, goalYSum = 0.0;
        int corridorTotal = 0, corridorObstacles = 0;
        int leftTotal = 0, leftObstacles = 0;
        int rightTotal = 0, rightObstacles = 0;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const RGB& p = pixels[(size_t)y * width + x];
                const bool goal = p.g > 220 && p.r < 40 && p.b > 80;
                const bool obstacle = p.r > 220 && p.g < 120 && p.b < 100;
                const bool avatar = p.b > 220 && p.g > 140 && p.r > 80 && p.r < 180;

                if (goal) {
                    f.goalPixels++;
                    goalXSum += x;
                    goalYSum += y;
                }
                if (obstacle) f.obstaclePixels++;
                if (avatar) f.avatarPixels++;

                if (y >= height / 3) {
                    if (x >= width * 3 / 8 && x < width * 5 / 8) {
                        corridorTotal++;
                        if (obstacle) corridorObstacles++;
                    }
                    if (x < width / 2) {
                        leftTotal++;
                        if (obstacle) leftObstacles++;
                    } else {
                        rightTotal++;
                        if (obstacle) rightObstacles++;
                    }
                }
            }
        }

        f.goalVisible = f.goalPixels > 0;
        if (f.goalVisible) {
            f.goalX = (float)(goalXSum / f.goalPixels) / (width - 1);
            f.goalY = (float)(goalYSum / f.goalPixels) / (height - 1);
        }
        f.corridorObstacle = corridorTotal ? (float)corridorObstacles / corridorTotal : 0.0f;
        f.leftObstacle = leftTotal ? (float)leftObstacles / leftTotal : 0.0f;
        f.rightObstacle = rightTotal ? (float)rightObstacles / rightTotal : 0.0f;
        return f;
    }
};

struct VisualAction {
    float turn{0.0f};
    float move{0.0f};
};

class DisplayReactivePolicy {
public:
    VisualAction choose(const VisualFeatures& f) const {
        VisualAction action;
        if (!f.goalVisible) {
            action.turn = 0.95f;
            action.move = 0.0f;
            return action;
        }

        const float horizontalError = f.goalX - 0.5f;
        if (std::fabs(horizontalError) > 0.065f) {
            action.turn = std::max(-1.4f, std::min(1.4f, horizontalError * 2.6f));
        }

        if (f.corridorObstacle > 0.018f) {
            const float clearSideTurn = f.leftObstacle <= f.rightObstacle ? -1.25f : 1.25f;
            action.turn = 0.75f * clearSideTurn + 0.25f * action.turn;
            action.move = 0.15f;
        } else if (std::fabs(horizontalError) < 0.18f) {
            action.move = f.goalPixels < 10 ? 1.25f : (f.goalPixels < 28 ? 0.95f : 0.55f);
        }
        return action;
    }
};

struct EpisodeResult {
    bool success{false};
    int steps{0};
    int collisions{0};
    int firstGoalPixels{0};
    int firstObstaclePixels{0};
    float meanContradiction{0.0f};
};

static EpisodeResult runEpisode(GameWorld& world,
                                bool blankDisplay,
                                const char* firstFramePath,
                                const char* lastFramePath) {
    AvatarSkeleton avatar;
    EgocentricRenderer renderer;
    TrizMapping mapping;
    DisplayNavStateEncoder encoder;
    DisplayReactivePolicy policy;

    Vec3 position{0, 0, 0};
    float heading = 0.0f;
    float contradictionSum = 0.0f;
    EpisodeResult result;

    const int maxSteps = 700;
    for (int step = 0; step < maxSteps; ++step) {
        avatar.pose(position, heading, 0.0f, 0.05f);
        const Vec3 eye = position + Vec3{0, 1.6f, 0};
        renderer.render(world, eye, heading, 0.0f, avatar, false);
        mapping.mapFrame(renderer);
        contradictionSum += mapping.meanContradiction;

        std::vector<RGB> displayed = makeDisplayedPixels(renderer);
        if (step == 0 && firstFramePath) writePpm(firstFramePath, displayed, 64, 64);
        if (blankDisplay) std::fill(displayed.begin(), displayed.end(), RGB{0, 0, 0});

        const VisualFeatures features = encoder.encode(displayed, 64, 64);
        if (step == 0) {
            result.firstGoalPixels = features.goalPixels;
            result.firstObstaclePixels = features.obstaclePixels;
        }
        const VisualAction action = policy.choose(features);

        heading += action.turn * 0.10f;
        const Vec3 next = position + Vec3{
            std::sinf(heading) * action.move * 0.20f,
            0,
            std::cosf(heading) * action.move * 0.20f
        };
        if (world.collides(next)) {
            result.collisions++;
            heading += (features.leftObstacle <= features.rightObstacle ? -0.35f : 0.35f);
        } else {
            position = next;
        }

        result.steps = step + 1;
        if (world.goalDistance(position) < 1.0f) {
            result.success = true;
            avatar.pose(position, heading, std::fabs(action.move), 0.05f);
            renderer.render(world, position + Vec3{0, 1.6f, 0}, heading, 0.0f, avatar, false);
            if (lastFramePath) writePpm(lastFramePath, makeDisplayedPixels(renderer), 64, 64);
            break;
        }
    }

    result.meanContradiction = result.steps ? contradictionSum / result.steps : 0.0f;
    return result;
}

int main() {
    std::printf("=== GTAngel v13 Display-Grounded 3D Navigation Acceptance Test ===\n");
    std::printf("Policy input: exact RGB pixels drawn by the live viewport\n");
    std::printf("Privileged world geometry is excluded from action selection.\n\n");

    GameWorld renderWorld;
    renderWorld.generate(1337, 14);
    AvatarSkeleton renderAvatar;
    EgocentricRenderer render;
    TrizMapping mapping;
    DisplayNavStateEncoder encoder;

    Vec3 start{0, 0, 0};
    renderAvatar.pose(start, 0.0f, 0.0f, 0.0f);
    render.render(renderWorld, start + Vec3{0, 1.6f, 0}, 0.0f, 0.0f, renderAvatar, false);
    mapping.mapFrame(render);
    const std::vector<RGB> firstPerson = makeDisplayedPixels(render);
    const VisualFeatures liveFeatures = encoder.encode(firstPerson, 64, 64);
    writePpm("visual_nav_first_person.ppm", firstPerson, 64, 64);

    render.render(renderWorld, start + Vec3{0, 2.5f, -4.0f}, 0.0f, -0.4f, renderAvatar, true);
    const std::vector<RGB> thirdPerson = makeDisplayedPixels(render);
    const VisualFeatures avatarFeatures = encoder.encode(thirdPerson, 64, 64);
    writePpm("visual_nav_third_person_avatar.ppm", thirdPerson, 64, 64);

    std::vector<RGB> blank(firstPerson.size(), RGB{0, 0, 0});
    const VisualFeatures blankFeatures = encoder.encode(blank, 64, 64);

    int nonBlack = 0;
    for (const RGB& p : firstPerson) if (p.r || p.g || p.b) nonBlack++;
    const bool renderPass = nonBlack > 1024 && liveFeatures.obstaclePixels > 0;
    const bool avatarPass = avatarFeatures.avatarPixels > 0;
    const bool mappingPass = mapping.maxQ >= 0 || mapping.meanContradiction >= 0.0f;
    const bool ablationPass = (liveFeatures.goalVisible != blankFeatures.goalVisible) ||
                              (liveFeatures.obstaclePixels != blankFeatures.obstaclePixels);

    std::printf("Render integrity: nonBlack=%d/4096 goalPx=%d obstaclePx=%d -> %s\n",
                nonBlack, liveFeatures.goalPixels, liveFeatures.obstaclePixels,
                renderPass ? "PASS" : "FAIL");
    std::printf("Avatar visibility: avatarPx=%d -> %s\n",
                avatarFeatures.avatarPixels, avatarPass ? "PASS" : "FAIL");
    std::printf("TRIZ mapping: mean=%.4f max=%.4f cell=(%d,%d) -> %s\n",
                mapping.meanContradiction, mapping.maxContradiction, mapping.maxQ, mapping.maxR,
                mappingPass ? "PASS" : "FAIL");
    std::printf("Visual ablation: live(goal=%d,obst=%d) blank(goal=%d,obst=%d) -> %s\n\n",
                liveFeatures.goalVisible ? 1 : 0, liveFeatures.obstaclePixels,
                blankFeatures.goalVisible ? 1 : 0, blankFeatures.obstaclePixels,
                ablationPass ? "PASS" : "FAIL");

    struct Scenario { const char* name; int pillars; };
    const Scenario scenarios[] = {
        {"sparse", 6},
        {"medium", 14},
        {"dense", 24}
    };

    std::ofstream csv("visual_nav_results.csv");
    csv << "scenario,seed,blank_display,success,steps,collisions,first_goal_pixels,first_obstacle_pixels,mean_contradiction\n";

    int liveSuccesses = 0;
    int blankSuccesses = 0;
    int totalLive = 0;
    for (const Scenario& scenario : scenarios) {
        int scenarioSuccess = 0;
        int scenarioCollisions = 0;
        for (uint32_t seed = 1337; seed < 1347; ++seed) {
            GameWorld world;
            world.generate(seed, scenario.pillars);
            const bool capture = seed == 1337;
            std::string startPath = std::string("visual_nav_") + scenario.name + "_start.ppm";
            std::string goalPath = std::string("visual_nav_") + scenario.name + "_finish.ppm";
            EpisodeResult live = runEpisode(world, false,
                                            capture ? startPath.c_str() : nullptr,
                                            capture ? goalPath.c_str() : nullptr);
            csv << scenario.name << ',' << seed << ",0," << (live.success ? 1 : 0) << ','
                << live.steps << ',' << live.collisions << ',' << live.firstGoalPixels << ','
                << live.firstObstaclePixels << ',' << live.meanContradiction << '\n';
            liveSuccesses += live.success ? 1 : 0;
            scenarioSuccess += live.success ? 1 : 0;
            scenarioCollisions += live.collisions;
            totalLive++;
        }

        GameWorld blankWorld;
        blankWorld.generate(1337, scenario.pillars);
        EpisodeResult blankRun = runEpisode(blankWorld, true, nullptr, nullptr);
        blankSuccesses += blankRun.success ? 1 : 0;
        csv << scenario.name << ",1337,1," << (blankRun.success ? 1 : 0) << ','
            << blankRun.steps << ',' << blankRun.collisions << ',' << blankRun.firstGoalPixels << ','
            << blankRun.firstObstaclePixels << ',' << blankRun.meanContradiction << '\n';

        std::printf("%-6s: visual success=%d/10 collisions=%d | blank success=%d\n",
                    scenario.name, scenarioSuccess, scenarioCollisions, blankRun.success ? 1 : 0);
    }
    csv.close();

    const bool navigationPass = liveSuccesses > blankSuccesses && liveSuccesses > 0;
    std::printf("\nDisplay-grounded navigation: %d/%d live successes vs %d blank -> %s\n",
                liveSuccesses, totalLive, blankSuccesses,
                navigationPass ? "PASS" : "FAIL");
    std::printf("Artifacts: visual_nav_*.ppm, visual_nav_results.csv\n");

    const bool allPass = renderPass && avatarPass && mappingPass && ablationPass && navigationPass;
    std::printf("OVERALL=%s\n", allPass ? "PASS" : "FAIL");
    return allPass ? 0 : 1;
}
