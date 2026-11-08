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
// Free Type from zpp/peds/CopPed.zpp
enum class eCopType : int32_t { COP_STREET_v = 0, COP_FBI_v = 1, COP_SWAT_v = 2, COP_HELI_SWAT_v = 3, COP_ARMY_v = 4, COP_MIAMIVICE_v = 5 };
// Free Type from zpp/peds/EmergencyPed.zpp
enum class EmergencyPedState : int32_t { EMERGENCY_PED_READY_v = 0, EMERGENCY_PED_DETERMINE_NEXT_STATE_v = 1, EMERGENCY_PED_START_CPR_v = 2, EMERGENCY_PED_FLAG_4_v = 4, EMERGENCY_PED_FLAG_8_v = 8, EMERGENCY_PED_FACE_TO_PATIENT_v = 16, EMERGENCY_PED_PERFORM_CPR_v = 32, EMERGENCY_PED_STOP_CPR_v = 64, EMERGENCY_PED_STAND_STILL_v = 128, EMERGENCY_PED_STOP_v = 256 };
// Free Type from zpp/peds/Gangs.zpp
enum class Gangs_Enum_1 : int32_t { GANG_MAFIA_v = 0, GANG_TRIAD_v = 1, GANG_DIABLOS_v = 2, GANG_YAKUZA_v = 3, GANG_YARDIE_v = 4, GANG_COLUMB_v = 5, GANG_HOODS_v = 6, GANG_FORELLI_v = 7, GANG_SINDACCO_v = 8, NUM_GANGS_v = 9 };
// Free Type from zpp/peds/Gangs.zpp
enum class Gangs_Enum_2 : int32_t { GANG_CUBAN_v = 0, GANG_HAITIAN_v = 1, GANG_STREET_v = 2, GANG_DIAZ_v = 3, GANG_SECURITY_v = 4, GANG_BIKER_v = 5, GANG_PLAYER_v = 6, GANG_GOLFER_v = 7, GANG_9_v = 8, NUM_GANGS_v = 9 };
// Free Type from zpp/peds/Gangs.zpp
enum class Gangs_Enum_3 : int32_t { GANG_MAFIA_v = 0, GANG_TRIAD_v = 1, GANG_DIABLOS_v = 2, GANG_YAKUZA_v = 3, GANG_YARDIE_v = 4, GANG_COLUMB_v = 5, GANG_HOODS_v = 6, GANG_7_v = 7, GANG_8_v = 8, NUM_GANGS_v = 9 };
// Free Type from zpp/peds/Ped.zpp
enum class Ped_Enum_1 : int32_t { ATTACK_IN_PROGRESS_v = 0, CANT_ATTACK_v = 1, WATCH_UNTIL_HE_DISAPPEARS_v = 2 };
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
        return (m_nStealWishList_len == 16u && m_nStealWishList.size() == 16u)
            && (m_nLookForVacantCarsCounter >= 0);
    }
};
// State Schema Contract from zpp/peds/DummyPed.zpp
struct CDummyPed_ZppContract {
    int32_t pedType{0}; // Z++: ℤ
    int32_t unknown{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
// State Schema Contract from zpp/peds/Gangs.zpp
struct CGangInfo_ZppContract {
    int32_t m_nVehicleMI{0}; // Z++: ℤ
    int32_t m_nPedModel1MI{0}; // Z++: ℤ
    int32_t m_nPedModel2MI{0}; // Z++: ℤ
    int32_t m_nPedModelOverride{0}; // Z++: ℤ
    int32_t m_Weapon1{0}; // Z++: ℤ
    int32_t m_Weapon2{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/Gangs.zpp
struct CGangs_ZppContract {
    std::array<bool, 9> GangAttackWithCops{}; // Z++: seq 𝔹  -- bounded array [NUM_GANGS]
    size_t GangAttackWithCops_len{9u}; // Z++ cardinality #GangAttackWithCops
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (GangAttackWithCops_len == 9u && GangAttackWithCops.size() == 9u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/Ped.zpp
struct PedAudioData_ZppContract {
    int32_t m_nFixedDelayTime{0}; // Z++: ℤ
    int32_t m_nOverrideFixedDelayTime{0}; // Z++: ℤ
    int32_t m_nOverrideMaxRandomDelayTime{0}; // Z++: ℤ
    int32_t m_nMaxRandomDelayTime{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
    std::array<int32_t, 4> m_effects{}; // Z++: seq C2dEffect  -- bounded array [NUM_ATTRACTORS_FOR_ICECREAM_VAN]
    size_t m_effects_len{4u}; // Z++ cardinality #m_effects
    constexpr bool checkInvariant() const {
        return (m_effects_len == 4u && m_effects.size() == 4u)
            && (m_pVehicle_card <= 1u);
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
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedShelterAttractor_ZppContract {
    int32_t ms_displacements{0}; // Z++: std__vector_CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedChat.zpp
struct PedChatModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{221}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 221);
    }
};
// State Schema Contract from zpp/peds/PedDebug.zpp
struct PedDebugModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{542}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 542);
    }
};
// State Schema Contract from zpp/peds/PedFight.zpp
struct PedFightModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6501}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6501);
    }
};
// State Schema Contract from zpp/peds/PedIK.zpp
struct LimbOrientation_ZppContract {
    float yaw{0.0f}; // Z++: ℝ
    float pitch{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedIK.zpp
struct LimbMovementInfo_ZppContract {
    float maxYaw{0.0f}; // Z++: ℝ
    float minYaw{0.0f}; // Z++: ℝ
    float yawD{0.0f}; // Z++: ℝ
    float maxPitch{0.0f}; // Z++: ℝ
    float minPitch{0.0f}; // Z++: ℝ
    float pitchD{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_ChooseEffect_2(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_GetVehicle_6(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_HasThisEffect_7(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_GetEffect_8(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetType_3(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_UpdatePedStateOnDeparture_4(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_IsAtHeadOfQueue_5(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractPos_6(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractHeading_7(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_IsRegisteredWithPed_9(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeDeltaHeading_11(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeDeltaPos_12(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractTime_13(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetNoOfRegisteredPeds_14(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeFreeSlot_15(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_IsInQueue_16(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetEffect_20(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetEffectPos_21(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetMaxPedsInAttractor_22(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetDistanceToCountSeekDone_23(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_GetAcceptableHeading_24(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_GetType(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_UpdatePedStateOnDeparture_3(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_IsAtHeadOfQueue_4(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
constexpr bool verify_peds_zpp_invariants() {
    {
        CCivilianPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCivilianPed_ZppContract bad{};
        bad.m_nStealWishList_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CDummyPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDummyPed_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CEmergencyPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEmergencyPed_ZppContract bad{};
        bad.m_pRevivedPed_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CGangInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CGangInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CGangs_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CGangs_ZppContract bad{};
        bad.GangAttackWithCops_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        PedAudioData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedAudioData_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        PedAIModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedAIModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CVehicleToEffect_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVehicleToEffect_ZppContract bad{};
        bad.m_effects_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedAttractorManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedAttractorManager_ZppContract bad{};
        bad.vAtmAttractors_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedShelterAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedShelterAttractor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        PedChatModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedChatModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        PedDebugModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedDebugModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        PedFightModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedFightModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        LimbOrientation_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LimbOrientation_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        LimbMovementInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LimbMovementInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_ChooseEffect_2(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_GetVehicle_6(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_HasThisEffect_7(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_GetEffect_8(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetType_3(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_UpdatePedStateOnDeparture_4(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_IsAtHeadOfQueue_5(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractPos_6(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractHeading_7(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_IsRegisteredWithPed_9(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeDeltaHeading_11(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeDeltaPos_12(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractTime_13(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetNoOfRegisteredPeds_14(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeFreeSlot_15(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_IsInQueue_16(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetEffect_20(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetEffectPos_21(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetMaxPedsInAttractor_22(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetDistanceToCountSeekDone_23(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_GetAcceptableHeading_24(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_GetType(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_UpdatePedStateOnDeparture_3(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_IsAtHeadOfQueue_4(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_peds_zpp_invariants(), "Z++ formal contract invariant failed for peds");
static_assert(CGangs_ZppContract{}.GangAttackWithCops.size() == 9u, "CGangs NUM_GANGS must be 9");
} // namespace zpp_peds_spec
#endif // GTANGEL_ZPP_PEDS_SPEC_GUARD
