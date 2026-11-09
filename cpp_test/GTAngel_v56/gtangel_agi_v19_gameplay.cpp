// GTAngel v19 — Ariadne gameplay vertical slice.
// Local gameplay truth only: no telemetry or external model is required.
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

        if (quest.stage == QuestStage::RestoreBeacon && quest.hasSignalShard &&
            horizontalDistance(player, world.goal) <= 1.55f) {
            quest.hasSignalShard = false;
            quest.beaconRestored = true;
            quest.stage = QuestStage::ReturnToAriadne;
            ariadne.remembersRestoration = true;
            ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.18f);
            record("Civic beacon restored");
            syncWorld(world);
            return quest.lastEvent;
        }

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
