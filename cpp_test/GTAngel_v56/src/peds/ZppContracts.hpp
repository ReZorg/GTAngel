#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_PEDS_SPEC_GUARD
#define GTANGEL_ZPP_PEDS_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/peds
// Derived from C:/ddd/GTAngel/cpp_test/zpp/peds/*.zpp
// Modules: 17 | Formal Schemas: 673 | Free Types: 31
// ============================================================================
namespace zpp_peds_spec {
static constexpr int kZppModuleCount = 17;
static constexpr int kZppSchemaCount = 673;
static constexpr int kZppEnumCount   = 31;
// From zpp/peds/CopPed.zpp
enum class eCopType : int32_t { COP_STREET_v = 0, COP_FBI_v = 1, COP_SWAT_v = 2, COP_HELI_SWAT_v = 3, COP_ARMY_v = 4, COP_MIAMIVICE_v = 5 };
// From zpp/peds/EmergencyPed.zpp
enum class EmergencyPedState : int32_t { EMERGENCY_PED_READY_v = 0, EMERGENCY_PED_DETERMINE_NEXT_STATE_v = 1, EMERGENCY_PED_START_CPR_v = 2, EMERGENCY_PED_FLAG_4_v = 3, EMERGENCY_PED_FLAG_8_v = 4, EMERGENCY_PED_FACE_TO_PATIENT_v = 5, EMERGENCY_PED_PERFORM_CPR_v = 6, EMERGENCY_PED_STOP_CPR_v = 7, EMERGENCY_PED_STAND_STILL_v = 8, EMERGENCY_PED_STOP_v = 9 };
// From zpp/peds/Gangs.zpp
enum class Gangs_Enum_1 : int32_t { GANG_MAFIA_v = 0, GANG_TRIAD_v = 1, GANG_DIABLOS_v = 2, GANG_YAKUZA_v = 3, GANG_YARDIE_v = 4, GANG_COLUMB_v = 5, GANG_HOODS_v = 6, GANG_FORELLI_v = 7, GANG_SINDACCO_v = 8, NUM_GANGS_v = 9 };
// From zpp/peds/Gangs.zpp
enum class Gangs_Enum_2 : int32_t { GANG_CUBAN_v = 0, GANG_HAITIAN_v = 1, GANG_STREET_v = 2, GANG_DIAZ_v = 3, GANG_SECURITY_v = 4, GANG_BIKER_v = 5, GANG_PLAYER_v = 6, GANG_GOLFER_v = 7, GANG_9_v = 8, NUM_GANGS_v = 9 };
// State Schema Contract from zpp/peds/CivilianPed.zpp
struct CCivilianPed_ZppContract {
    bool m_bLookForVacantCars{false}; // Z++: 𝔹
    uint32_t m_nLookForVacantCarsCounter{0}; // Z++: ℕ
    bool m_bStealCarEvenIfThereIsSomeoneInIt{false}; // Z++: 𝔹
    bool m_bJustStoleACar{false}; // Z++: 𝔹
    int32_t m_nStealWishList{0}; // Z++: seq ℕ  -- bounded array [16]
    bool m_bAttractorUnk{false}; // Z++: 𝔹
    int32_t m_nAttractorCycleState{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/peds/CopPed.zpp
struct CCopPed_ZppContract {
    int32_t m_nRoadblockVeh{0}; // Z++: Ref[CVehicle]
    float m_fDistanceToTarget{0.0f}; // Z++: ℝ
    bool m_bIsInPursuit{false}; // Z++: 𝔹
    bool m_bIsDisabledCop{false}; // Z++: 𝔹
    int32_t field_5FE{0}; // Z++: ℤ
    bool m_bBeatingSuspect{false}; // Z++: 𝔹
    bool m_bStopAndShootDisabledZone{false}; // Z++: 𝔹
    bool m_bDragsPlayerFromCar{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/peds/DummyPed.zpp
struct CDummyPed_ZppContract {
    int32_t pedType{0}; // Z++: ℤ
    int32_t unknown{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/peds/EmergencyPed.zpp
struct CEmergencyPed_ZppContract {
    int32_t m_pRevivedPed{0}; // Z++: Ref[CPed]
    int32_t m_nEmergencyPedState{0}; // Z++: EmergencyPedState
    int32_t m_pAttendedAccident{0}; // Z++: Ref[CAccident]
    int32_t m_pAttendedFire{0}; // Z++: Ref[CFire]
    bool m_bStartedToCPR{false}; // Z++: 𝔹
    int32_t field_1360{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/peds/Gangs.zpp
struct CGangInfo_ZppContract {
    int32_t m_nVehicleMI{0}; // Z++: ℤ
    int32_t m_nPedModel1MI{0}; // Z++: ℤ
    int32_t m_nPedModel2MI{0}; // Z++: ℤ
    int32_t m_nPedModelOverride{0}; // Z++: ℤ
    int32_t m_Weapon1{0}; // Z++: ℤ
    int32_t m_Weapon2{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/peds/Gangs.zpp
struct CGangs_ZppContract {
    int32_t GangAttackWithCops{0}; // Z++: seq 𝔹  -- bounded array [NUM_GANGS]
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_peds_zpp_invariants() {
    if (!CCivilianPed_ZppContract{}.checkInvariant()) return false;
    if (!CCopPed_ZppContract{}.checkInvariant()) return false;
    if (!CDummyPed_ZppContract{}.checkInvariant()) return false;
    if (!CEmergencyPed_ZppContract{}.checkInvariant()) return false;
    if (!CGangInfo_ZppContract{}.checkInvariant()) return false;
    if (!CGangs_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_peds_zpp_invariants(), "Z++ formal contract invariant failed for peds");
} // namespace zpp_peds_spec
#endif // GTANGEL_ZPP_PEDS_SPEC_GUARD
