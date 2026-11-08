#pragma once
#ifndef GTANGEL_ZPP_CONTROL_SPEC_GUARD
#define GTANGEL_ZPP_CONTROL_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/control
// Derived from C:/ddd/GTAngel/cpp_test/zpp/control/*.zpp
// Modules: 34 | Formal Schemas: 1230 | Free Types: 66
// ============================================================================
namespace zpp_control_spec {
static constexpr int kZppModuleCount = 34;
static constexpr int kZppSchemaCount = 1230;
static constexpr int kZppEnumCount   = 66;
// From zpp/control/AutoPilot.zpp
enum class eCarMission : int32_t { MISSION_NONE_v = 0, MISSION_CRUISE_v = 1, MISSION_RAMPLAYER_FARAWAY_v = 2, MISSION_RAMPLAYER_CLOSE_v = 3, MISSION_BLOCKPLAYER_FARAWAY_v = 4, MISSION_BLOCKPLAYER_CLOSE_v = 5, MISSION_BLOCKPLAYER_HANDBRAKESTOP_v = 6, MISSION_WAITFORDELETION_v = 7, MISSION_GOTOCOORDS_v = 8, MISSION_GOTOCOORDS_STRAIGHT_v = 9, MISSION_EMERGENCYVEHICLE_STOP_v = 10, MISSION_STOP_FOREVER_v = 11, MISSION_GOTOCOORDS_ACCURATE_v = 12, MISSION_GOTO_COORDS_STRAIGHT_ACCURATE_v = 13, MISSION_GOTOCOORDS_ASTHECROWSWIMS_v = 14, MISSION_RAMCAR_FARAWAY_v = 15, MISSION_RAMCAR_CLOSE_v = 16, MISSION_BLOCKCAR_FARAWAY_v = 17, MISSION_BLOCKCAR_CLOSE_v = 18, MISSION_BLOCKCAR_HANDBRAKESTOP_v = 19, MISSION_HELI_FLYTOCOORS_v = 20, MISSION_ATTACKPLAYER_v = 21, MISSION_PLANE_FLYTOCOORS_v = 22, MISSION_HELI_LAND_v = 23, MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1_v = 24, MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2_v = 25, MISSION_BLOCKPLAYER_FORWARDANDBACK_v = 26 };
// From zpp/control/AutoPilot.zpp
enum class eCarTempAction : int32_t { TEMPACT_NONE_v = 0, TEMPACT_WAIT_v = 1, TEMPACT_REVERSE_v = 2, TEMPACT_HANDBRAKETURNLEFT_v = 3, TEMPACT_HANDBRAKETURNRIGHT_v = 4, TEMPACT_HANDBRAKESTRAIGHT_v = 5, TEMPACT_TURNLEFT_v = 6, TEMPACT_TURNRIGHT_v = 7, TEMPACT_GOFORWARD_v = 8, TEMPACT_SWERVELEFT_v = 9, TEMPACT_SWERVERIGHT_v = 10 };
// From zpp/control/AutoPilot.zpp
enum class eCarDrivingStyle : int32_t { DRIVINGSTYLE_STOP_FOR_CARS_v = 0, DRIVINGSTYLE_SLOW_DOWN_FOR_CARS_v = 1, DRIVINGSTYLE_AVOID_CARS_v = 2, DRIVINGSTYLE_PLOUGH_THROUGH_v = 3, DRIVINGSTYLE_STOP_FOR_CARS_IGNORE_LIGHTS_v = 4 };
// From zpp/control/Bridge.zpp
enum class bridgeStates : int32_t { STATE_BRIDGE_LOCKED_v = 0, STATE_LIFT_PART_IS_UP_v = 1, STATE_LIFT_PART_MOVING_DOWN_v = 2, STATE_LIFT_PART_IS_DOWN_v = 3, STATE_LIFT_PART_ABOUT_TO_MOVE_UP_v = 4, STATE_LIFT_PART_MOVING_UP_v = 5, STATE_BRIDGE_ALWAYS_UNLOCKED_v = 6 };
// State Schema Contract from zpp/control/Bridge.zpp
struct CBridge_ZppContract {
    uint8_t pWeight_card{0u}; // Z++: Ref[CEntity_pLiftRoad__pLiftPart]
    int32_t OldState{0}; // Z++: ℤ
    float DefaultZLiftWeight{0.0f}; // Z++: ℝ
    float OldLift{0.0f}; // Z++: ℝ
    int64_t TimeOfBridgeBecomingOperational{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (pWeight_card <= 1u)
            && (TimeOfBridgeBecomingOperational >= 0);
    }
};
// State Schema Contract from zpp/control/CarAI.zpp
struct CCarAI_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Curves.zpp
struct CCurves_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPedPathNode_ZppContract {
    bool bBlockade{false}; // Z++: 𝔹
    int64_t nodeIdX{0}; // Z++: ℕ
    int64_t nodeIdY{0}; // Z++: ℕ
    int32_t id{0}; // Z++: ℤ
    uint8_t prev_card{0u}; // Z++: Ref[CPedPathNode]
    uint8_t next_card{0u}; // Z++: Ref[CPedPathNode]
    constexpr bool checkInvariant() const {
        return (nodeIdX >= 0)
            && (nodeIdY >= 0)
            && (prev_card <= 1u)
            && (next_card <= 1u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPedPath_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CTempNode_ZppContract {
    int32_t pos{0}; // Z++: CVector
    int32_t dirX{0}; // Z++: ℤ
    int32_t dirY{0}; // Z++: ℤ
    int32_t link1{0}; // Z++: ℤ
    int32_t link2{0}; // Z++: ℤ
    int32_t numLeftLanes{0}; // Z++: ℤ
    int32_t numRightLanes{0}; // Z++: ℤ
    int64_t width{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (width >= 0);
    }
};
constexpr bool verify_control_zpp_invariants() {
    if (!CBridge_ZppContract{}.checkInvariant()) return false;
    if (!CCarAI_ZppContract{}.checkInvariant()) return false;
    if (!CCurves_ZppContract{}.checkInvariant()) return false;
    if (!CPedPathNode_ZppContract{}.checkInvariant()) return false;
    if (!CPedPath_ZppContract{}.checkInvariant()) return false;
    if (!CTempNode_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_control_zpp_invariants(), "Z++ formal contract invariant failed for control");
} // namespace zpp_control_spec
#endif // GTANGEL_ZPP_CONTROL_SPEC_GUARD
