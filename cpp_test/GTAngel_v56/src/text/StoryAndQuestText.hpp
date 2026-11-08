namespace v20cns { class WorldEffector; }
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
    // LW16 side quests (not serialized: v20 decode leaves them at defaults).
    bool questRewardClaimed{false};
    uint32_t pickupsCollected{0};
    bool shardGlowSeen{false};
    uint32_t crowdSyncTicks{0};
    bool crowdSyncDone{false};
    // LW39 Act II & Act III storyline progression flags.
    bool obeliskDecrypted{false};
    bool relayAttuned{false};
    bool shadowNodePurified{false};
    bool skylineTowerSynced{false};
    bool sanctuaryAwakened{false};
    uint32_t patrolWaypoint{0};
};

class GameplayLoop {
    friend class v20cns::WorldEffector;
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
        result.turn = left ? -0.68f : (right ? 0.68f : 0.0f);
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
        // LW16 side quests: glow witness + crowd sync progress passively.
        if (!quest.shardGlowSeen && quest.shardVisible &&
            horizontalDistance(player, world.questShard) < 4.0f) {
            quest.shardGlowSeen = true;
            record("Side quest: shard glow witnessed");
        }
        if (!quest.crowdSyncDone) {
            int walkersNear = 0;
            for (const auto& w : world.walkers)
                if (horizontalDistance(player, w.pos) < 3.0f) ++walkersNear;
            if (walkersNear > 0) {
                quest.crowdSyncTicks += 1;
                if (quest.crowdSyncTicks >= 300) {
                    quest.crowdSyncDone = true;
                    record("Side quest: crowd sync achieved");
                }
            }
        }
        // LW40 4-Act storyline triggers once Act I is Complete.
        if (quest.stage == QuestStage::Complete && quest.pickupsCollected >= 4) {
            const avatar3d::Vec3 obeliskPos{-9.0f, 0.0f, -2.4f};
            const avatar3d::Vec3 relayPos{9.0f, 0.0f, 2.4f};
            const avatar3d::Vec3 shadowPos{2.6f, 0.0f, -18.0f};
            const avatar3d::Vec3 towerPos{-18.0f, 0.0f, 2.6f};
            if (!quest.obeliskDecrypted && horizontalDistance(player, obeliskPos) <= 1.85f) {
                quest.obeliskDecrypted = true;
                ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.08f);
                record("Act II: Memory Codex decrypted at Obelisk");
            } else if (quest.obeliskDecrypted && !quest.relayAttuned &&
                       horizontalDistance(player, relayPos) <= 1.85f) {
                quest.relayAttuned = true;
                ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.08f);
                record("Act II: Harmonic Transit Relay attuned");
            } else if (quest.relayAttuned && !quest.shadowNodePurified &&
                       horizontalDistance(player, shadowPos) <= 1.95f) {
                quest.shadowNodePurified = true;
                ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.08f);
                record("Act III: Faustian Shadow Node purified");
            } else if (quest.shadowNodePurified && !quest.skylineTowerSynced &&
                       horizontalDistance(player, towerPos) <= 1.95f) {
                quest.skylineTowerSynced = true;
                ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.08f);
                record("Act III: Skyline Resonance Tower synced");
            } else if (quest.skylineTowerSynced && !quest.sanctuaryAwakened &&
                       horizontalDistance(player, ariadne.position) <= 2.25f) {
                quest.sanctuaryAwakened = true;
                ariadne.disposition = 1.0f;
                record("Act IV complete: Crown of Echoes bestowed");
            }
        }
        syncWorld(world);
    }

    bool nearbyPickup(const avatar3d::Vec3& player, const avatar3d::GameWorld& world) const {
        for (const auto& pk : world.pickups)
            if (!pk.collected && horizontalDistance(player, pk.pos) < 1.2f) return true;
        return false;
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

        if (canRestore(player, world)) return "Beacon repair requires local World Governance Reflex";

        if (canSpeak(player, playerHeading)) {
            quest.dialogueOpen = true;
            if (quest.stage == QuestStage::Dormant) quest.stage = QuestStage::Offered;
            record("Ariadne conversation opened");
            syncWorld(world);
            return quest.lastEvent;
        }

        for (auto& pk : world.pickups) {
            if (!pk.collected && horizontalDistance(player, pk.pos) < 1.2f) {
                pk.collected = true;
                quest.pickupsCollected += 1;
                record(quest.pickupsCollected >= 4
                    ? "Side quest complete: all data cells recovered"
                    : "Data cell recovered");
                syncWorld(world);
                return quest.lastEvent;
            }
        }

        quest.lastEvent = "Nothing within interaction focus";
        return quest.lastEvent;
    }

    // Called only from v20 WorldEffector after a locally minted NICHE_COMMIT.
private:
    bool governedRepairBeacon(avatar3d::GameWorld& world) {
        if (quest.stage != QuestStage::RestoreBeacon || !quest.hasSignalShard || quest.beaconRestored) return false;
        quest.hasSignalShard = false;
        quest.beaconRestored = true;
        quest.stage = QuestStage::ReturnToAriadne;
        ariadne.remembersRestoration = true;
        ariadne.disposition = std::min(1.0f, ariadne.disposition + 0.18f);
        record("Civic beacon restored");
        syncWorld(world);
        return true;
    }
public:
    void publishWorld(avatar3d::GameWorld& world) const { syncWorld(world); }

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

    const char* actTitle() const {
        if (quest.stage != QuestStage::Complete) return "ACT I: ARIADNE & THE BROKEN SIGNAL";
        if (!quest.relayAttuned)        return "ACT II: THE ARCHIVE OF ECHOES";
        if (!quest.skylineTowerSynced)  return "ACT III: COUNTERVAIL OF THE SPIRE";
        return "ACT IV: THE SANCTUARY AWAKENED";
    }

    std::string stageName() const {
        switch (quest.stage) {
            case QuestStage::Dormant: return "ACT I / DISCOVER";
            case QuestStage::Offered: return "ACT I / OFFERED";
            case QuestStage::SeekShard: return "ACT I / SEEK SHARD";
            case QuestStage::RestoreBeacon: return "ACT I / RESTORE";
            case QuestStage::ReturnToAriadne: return "ACT I / RETURN";
            case QuestStage::Complete:
                if (quest.pickupsCollected < 4) return "ACT II / SEEK CELLS";
                if (!quest.obeliskDecrypted)    return "ACT II / OBELISK";
                if (!quest.relayAttuned)        return "ACT II / RELAY";
                if (!quest.shadowNodePurified)  return "ACT III / SHADOW NODE";
                if (!quest.skylineTowerSynced)  return "ACT III / SKYLINE TOWER";
                if (!quest.sanctuaryAwakened)   return "ACT IV / CONSECRATE";
                return "ACT IV / AWAKENED";
        }
        return "UNKNOWN";
    }

    std::string objective() const {
        if (quest.stage == QuestStage::Complete) {
            if (quest.pickupsCollected < 4) return "Recover all 4 lost Data Cells";
            if (!quest.obeliskDecrypted)    return "Decrypt Codex at Memory Obelisk (-9,-2)";
            if (!quest.relayAttuned)        return "Attune Harmonic Transit Relay (9,2)";
            if (!quest.shadowNodePurified)  return "Purify Faustian Shadow Node (3,-18)";
            if (!quest.skylineTowerSynced)  return "Sync Skyline Resonance Tower (-18,3)";
            if (!quest.sanctuaryAwakened)   return "Bestow Crown of Echoes with Ariadne";
            return "Sanctuary awakened: patrol the 6 loci";
        }
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

// GTAngel v20: original, portable local CNS vertical slice. No supplied engine code/assets.
// This module is inserted after v19game in the deterministic generated Win32 source.
#include <optional>
