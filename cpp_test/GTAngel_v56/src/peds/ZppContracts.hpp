#pragma once
#ifndef GTANGEL_ZPP_PEDS_SPEC_GUARD
#define GTANGEL_ZPP_PEDS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
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
enum class EmergencyPedState : int32_t { EMERGENCY_PED_READY_v = 0, EMERGENCY_PED_DETERMINE_NEXT_STATE_v = 1, EMERGENCY_PED_START_CPR_v = 2, EMERGENCY_PED_FLAG_4_v = 4, EMERGENCY_PED_FLAG_8_v = 8, EMERGENCY_PED_FACE_TO_PATIENT_v = 16, EMERGENCY_PED_PERFORM_CPR_v = 32, EMERGENCY_PED_STOP_CPR_v = 64, EMERGENCY_PED_STAND_STILL_v = 128, EMERGENCY_PED_STOP_v = 256 };
// From zpp/peds/Gangs.zpp
enum class Gangs_Enum_1 : int32_t { GANG_MAFIA_v = 0, GANG_TRIAD_v = 1, GANG_DIABLOS_v = 2, GANG_YAKUZA_v = 3, GANG_YARDIE_v = 4, GANG_COLUMB_v = 5, GANG_HOODS_v = 6, GANG_FORELLI_v = 7, GANG_SINDACCO_v = 8, NUM_GANGS_v = 9 };
// From zpp/peds/Gangs.zpp
enum class Gangs_Enum_2 : int32_t { GANG_CUBAN_v = 0, GANG_HAITIAN_v = 1, GANG_STREET_v = 2, GANG_DIAZ_v = 3, GANG_SECURITY_v = 4, GANG_BIKER_v = 5, GANG_PLAYER_v = 6, GANG_GOLFER_v = 7, GANG_9_v = 8, NUM_GANGS_v = 9 };
// State Schema Contract from zpp/peds/CivilianPed.zpp
struct CCivilianPed_ZppContract {
    bool m_bLookForVacantCars{false}; // Z++: 𝔹
    int64_t m_nLookForVacantCarsCounter{0}; // Z++: ℕ
    bool m_bStealCarEvenIfThereIsSomeoneInIt{false}; // Z++: 𝔹
    bool m_bJustStoleACar{false}; // Z++: 𝔹
    std::array<uint32_t, 16> m_nStealWishList{}; // Z++: seq ℕ  -- bounded array [16]
    size_t m_nStealWishList_len{16u}; // Z++ cardinality #m_nStealWishList
    bool m_bAttractorUnk{false}; // Z++: 𝔹
    int32_t m_nAttractorCycleState{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_nLookForVacantCarsCounter >= 0)
            && (m_nStealWishList_len == 16u && m_nStealWishList.size() == 16u);
    }
};
// State Schema Contract from zpp/peds/EmergencyPed.zpp
struct CEmergencyPed_ZppContract {
    uint8_t m_pRevivedPed_card{0u}; // Z++: Ref[CPed]
    int32_t m_nEmergencyPedState{0}; // Z++: EmergencyPedState
    uint8_t m_pAttendedAccident_card{0u}; // Z++: Ref[CAccident]
    uint8_t m_pAttendedFire_card{0u}; // Z++: Ref[CFire]
    bool m_bStartedToCPR{false}; // Z++: 𝔹
    int32_t field_1360{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pRevivedPed_card <= 1u)
            && (m_pAttendedAccident_card <= 1u)
            && (m_pAttendedFire_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedAI.zpp
struct PedAIModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{7350}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 7350);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CVehicleToEffect_ZppContract {
    uint8_t m_pVehicle_card{0u}; // Z++: Ref[CVehicle]
    int32_t m_effects{0}; // Z++: seq C2dEffect  -- bounded array [NUM_ATTRACTORS_FOR_ICECREAM_VAN]
    constexpr bool checkInvariant() const {
        return (m_pVehicle_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedAttractorManager_ZppContract {
    uint8_t vAtmAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    uint8_t vSeatAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    uint8_t vStopAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    uint8_t vPizzaAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    uint8_t vShelterAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    uint8_t vIceCreamAttractors_card{0u}; // Z++: Ref[std__vector_CPedAttractor]
    int32_t vVehicleToEffect{0}; // Z++: std__vector_CVehicleToEffect
    constexpr bool checkInvariant() const {
        return (vAtmAttractors_card <= 1u)
            && (vSeatAttractors_card <= 1u)
            && (vStopAttractors_card <= 1u)
            && (vPizzaAttractors_card <= 1u)
            && (vShelterAttractors_card <= 1u)
            && (vIceCreamAttractors_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u);
    }
};
constexpr bool verify_peds_zpp_invariants() {
    if (!CCivilianPed_ZppContract{}.checkInvariant()) return false;
    if (!CEmergencyPed_ZppContract{}.checkInvariant()) return false;
    if (!PedAIModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CVehicleToEffect_ZppContract{}.checkInvariant()) return false;
    if (!CPedAttractorManager_ZppContract{}.checkInvariant()) return false;
    if (!CPedAttractor_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_peds_zpp_invariants(), "Z++ formal contract invariant failed for peds");
} // namespace zpp_peds_spec
#endif // GTANGEL_ZPP_PEDS_SPEC_GUARD
