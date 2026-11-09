#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using SHORT = short;
static constexpr int MOVEFILE_REPLACE_EXISTING = 1;
static constexpr int MOVEFILE_WRITE_THROUGH = 8;
static SHORT GetAsyncKeyState(int) { return 0; }
static int DeleteFileA(const char* path) { return std::remove(path) == 0; }
static int MoveFileExA(const char* from, const char* to, int) {
    std::remove(to);
    return std::rename(from, to) == 0;
}

namespace avatar3d {
struct Vec3 {
    float x{0}, y{0}, z{0};
    Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
};
struct GameWorld {
    Vec3 goal{8.0f, 0.0f, 8.0f};
    Vec3 npcPos{-4.0f, 0.0f, 4.0f};
    float npcHeading{2.35f};
    float npcSpeed{0.0f};
    Vec3 questShard{-6.0f, 0.0f, -5.0f};
    bool questShardVisible{true};
    bool beaconRestored{false};
    bool questComplete{false};
};
}

#include "gtangel_agi_v19_gameplay.cpp"

static int failures = 0;
static void check(bool condition, const char* label) {
    if (!condition) {
        std::cerr << "FAIL: " << label << "\n";
        ++failures;
    }
}

int main() {
    avatar3d::GameWorld world;
    v19game::GameplayLoop gameplay;
    gameplay.reset(world);

    const avatar3d::Vec3 nearAriadne{-4.0f, 0.0f, 2.5f};
    check(gameplay.canSpeak(nearAriadne, 0.0f), "local range and facing permit Ariadne interaction");
    gameplay.interact(nearAriadne, 0.0f, world);
    check(gameplay.quest.stage == v19game::QuestStage::Offered, "speaking grounds the offer stage");
    check(gameplay.quest.dialogueOpen, "dialogue opens only through local interaction");

    gameplay.choose(2, world);
    check(gameplay.quest.askedWhy, "why choice updates conversation memory");
    check(gameplay.dialogueLine().find("remembers") != std::string::npos,
          "state-aware dialogue explains the beacon purpose");

    gameplay.choose(1, world);
    check(gameplay.quest.stage == v19game::QuestStage::SeekShard, "accept choice activates shard objective");
    check(gameplay.ariadne.remembersAcceptance, "Ariadne remembers acceptance");

    gameplay.interact(world.questShard, 0.0f, world);
    check(gameplay.quest.stage == v19game::QuestStage::RestoreBeacon, "local shard trigger advances objective");
    check(gameplay.quest.hasSignalShard && !world.questShardVisible, "shard enters inventory and leaves world");

    gameplay.interact(world.goal, 0.0f, world);
    check(gameplay.quest.stage == v19game::QuestStage::ReturnToAriadne, "beacon trigger advances return objective");
    check(world.beaconRestored && !gameplay.quest.hasSignalShard, "beacon restoration consumes shard and changes world");

    gameplay.interact(nearAriadne, 0.0f, world);
    gameplay.choose(1, world);
    check(gameplay.quest.stage == v19game::QuestStage::Complete, "return dialogue completes quest");
    check(gameplay.quest.hasResonanceSigil && world.questComplete, "completion grants reward and persists world consequence");

    check(gameplay.save(), "atomic gameplay save succeeds");
    gameplay.quest = v19game::QuestRuntime{};
    check(gameplay.load(world), "gameplay load succeeds");
    check(gameplay.quest.stage == v19game::QuestStage::Complete,
          "loaded quest preserves completion stage");
    check(gameplay.quest.hasResonanceSigil && world.beaconRestored,
          "loaded state preserves reward and restored beacon");
    DeleteFileA(v19game::GameplayLoop::SavePath);

    check(gameplay.recover(world).find("preserved") != std::string::npos,
          "recovery never erases completed purpose");
    check(gameplay.quest.groundedEvents >= 4, "quest progression is event-grounded");

    std::cout << "{\"ok\":" << (failures == 0 ? "true" : "false")
              << ",\"checks\":17,\"failures\":" << failures
              << ",\"stage\":\"" << gameplay.stageName() << "\"}\n";
    return failures == 0 ? 0 : 1;
}
