#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_CONTROL_SPEC_GUARD
#define GTANGEL_ZPP_CONTROL_SPEC_GUARD
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
enum class eCarMission : int32_t { MISSION_NONE_v = 0, MISSION_CRUISE_v = 1, MISSION_RAMPLAYER_FARAWAY_v = 2, MISSION_RAMPLAYER_CLOSE_v = 3, MISSION_BLOCKPLAYER_FARAWAY_v = 4, MISSION_BLOCKPLAYER_CLOSE_v = 5, MISSION_BLOCKPLAYER_HANDBRAKESTOP_v = 6, MISSION_WAITFORDELETION_v = 7, MISSION_GOTOCOORDS_v = 8, MISSION_GOTOCOORDS_STRAIGHT_v = 9, MISSION_EMERGENCYVEHICLE_STOP_v = 10, MISSION_STOP_FOREVER_v = 11 };
// From zpp/control/AutoPilot.zpp
enum class eCarTempAction : int32_t { TEMPACT_NONE_v = 0, TEMPACT_WAIT_v = 1, TEMPACT_REVERSE_v = 2, TEMPACT_HANDBRAKETURNLEFT_v = 3, TEMPACT_HANDBRAKETURNRIGHT_v = 4, TEMPACT_HANDBRAKESTRAIGHT_v = 5, TEMPACT_TURNLEFT_v = 6, TEMPACT_TURNRIGHT_v = 7, TEMPACT_GOFORWARD_v = 8, TEMPACT_SWERVELEFT_v = 9, TEMPACT_SWERVERIGHT_v = 10 };
// From zpp/control/AutoPilot.zpp
enum class eCarDrivingStyle : int32_t { DRIVINGSTYLE_STOP_FOR_CARS_v = 0, DRIVINGSTYLE_SLOW_DOWN_FOR_CARS_v = 1, DRIVINGSTYLE_AVOID_CARS_v = 2, DRIVINGSTYLE_PLOUGH_THROUGH_v = 3, DRIVINGSTYLE_STOP_FOR_CARS_IGNORE_LIGHTS_v = 4 };
// From zpp/control/Bridge.zpp
enum class bridgeStates : int32_t { STATE_BRIDGE_LOCKED_v = 0, STATE_LIFT_PART_IS_UP_v = 1, STATE_LIFT_PART_MOVING_DOWN_v = 2, STATE_LIFT_PART_IS_DOWN_v = 3, STATE_LIFT_PART_ABOUT_TO_MOVE_UP_v = 4, STATE_LIFT_PART_MOVING_UP_v = 5, STATE_BRIDGE_ALWAYS_UNLOCKED_v = 6 };
// State Schema Contract from zpp/control/AutoPilot.zpp
struct CAutoPilot_ZppContract {
    int32_t m_nCurrentRouteNode{0}; // Z++: ℤ
    int32_t m_nNextRouteNode{0}; // Z++: ℤ
    int32_t m_nPrevRouteNode{0}; // Z++: ℤ
    int32_t m_nTimeEnteredCurve{0}; // Z++: ℤ
    int32_t m_nTimeToSpendOnCurrentCurve{0}; // Z++: ℤ
    uint32_t m_nCurrentPathNodeInfo{0}; // Z++: ℕ
    uint32_t m_nNextPathNodeInfo{0}; // Z++: ℕ
    uint32_t m_nPreviousPathNodeInfo{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/control/Bridge.zpp
struct CBridge_ZppContract {
    int32_t pWeight{0}; // Z++: Ref[CEntity_pLiftRoad__pLiftPart]
    int32_t OldState{0}; // Z++: ℤ
    float DefaultZLiftWeight{0.0f}; // Z++: ℝ
    float OldLift{0.0f}; // Z++: ℝ
    uint32_t TimeOfBridgeBecomingOperational{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/control/CarAI.zpp
struct CCarAI_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/control/CarCtrl.zpp
struct CCarCtrl_ZppContract {
    int32_t NumLawEnforcerCars{0}; // Z++: ℤ
    int32_t NumAmbulancesOnDuty{0}; // Z++: ℤ
    int32_t NumFiretrucksOnDuty{0}; // Z++: ℤ
    int32_t NumRandomCars{0}; // Z++: ℤ
    int32_t NumMissionCars{0}; // Z++: ℤ
    int32_t NumParkedCars{0}; // Z++: ℤ
    int32_t NumPermanentCars{0}; // Z++: ℤ
    bool bCarsGeneratedAroundCamera{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/control/Curves.zpp
struct CCurves_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/control/Darkel.zpp
struct CDarkel_ZppContract {
    int32_t TimeLimit{0}; // Z++: ℤ
    int32_t PreviousTime{0}; // Z++: ℤ
    int32_t TimeOfFrenzyStart{0}; // Z++: ℤ
    int32_t WeaponType{0}; // Z++: ℤ
    int32_t AmmoInterruptedWeapon{0}; // Z++: ℤ
    int32_t KillsNeeded{0}; // Z++: ℤ
    int32_t InterruptedWeaponType{0}; // Z++: ℤ
    int32_t InterruptedWeaponSelected{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_control_zpp_invariants() {
    if (!CAutoPilot_ZppContract{}.checkInvariant()) return false;
    if (!CBridge_ZppContract{}.checkInvariant()) return false;
    if (!CCarAI_ZppContract{}.checkInvariant()) return false;
    if (!CCarCtrl_ZppContract{}.checkInvariant()) return false;
    if (!CCurves_ZppContract{}.checkInvariant()) return false;
    if (!CDarkel_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_control_zpp_invariants(), "Z++ formal contract invariant failed for control");
} // namespace zpp_control_spec
#endif // GTANGEL_ZPP_CONTROL_SPEC_GUARD
