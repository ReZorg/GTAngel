#pragma once
#ifndef GTANGEL_ZPP_PEDS_SPEC_GUARD
#define GTANGEL_ZPP_PEDS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/peds
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/peds/*.zpp
// Modules: 17 | Formal Schemas: 673 | Free Types: 31
// ============================================================================
namespace zpp_peds_spec {
static constexpr int kZppModuleCount = 17;
static constexpr int kZppSchemaCount = 673;
static constexpr int kZppEnumCount   = 31;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
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
// State Schema Contract from zpp/peds/DummyPed.zpp
struct CDummyPed_ZppContract {
    int32_t m_matrix{0}; // Z++: CMatrix
    uint8_t m_rwObject_card{0u}; // Z++: Ref[RwObject]
    uint32_t m_type{0u}; // Z++: 𝔹
    uint32_t m_status{0u}; // Z++: 𝔹
    uint32_t bUsesCollision{0u}; // Z++: 𝔹
    uint32_t bCollisionProcessed{0u}; // Z++: 𝔹
    uint32_t bIsStatic{0u}; // Z++: 𝔹
    uint32_t bHasContacted{0u}; // Z++: 𝔹
    uint32_t bPedPhysics{0u}; // Z++: 𝔹
    uint32_t bIsStuck{0u}; // Z++: 𝔹
    uint32_t bIsInSafePosition{0u}; // Z++: 𝔹
    uint32_t bUseCollisionRecords{0u}; // Z++: 𝔹
    uint32_t bWasPostponed{0u}; // Z++: 𝔹
    uint32_t bExplosionProof{0u}; // Z++: 𝔹
    uint32_t bIsVisible{0u}; // Z++: 𝔹
    uint32_t bHasCollided{0u}; // Z++: 𝔹
    uint32_t bRenderScorched{0u}; // Z++: 𝔹
    bool bHasBlip{false}; // Z++: 𝔹
    bool bIsBIGBuilding{false}; // Z++: 𝔹
    bool bStreamBIGBuilding{false}; // Z++: 𝔹
    bool bRenderDamaged{false}; // Z++: 𝔹
    bool bBulletProof{false}; // Z++: 𝔹
    bool bFireProof{false}; // Z++: 𝔹
    bool bCollisionProof{false}; // Z++: 𝔹
    bool bMeleeProof{false}; // Z++: 𝔹
    bool bOnlyDamagedByPlayer{false}; // Z++: 𝔹
    bool bStreamingDontDelete{false}; // Z++: 𝔹
    bool bRemoveFromWorld{false}; // Z++: 𝔹
    bool bHasHitWall{false}; // Z++: 𝔹
    bool bImBeingRendered{false}; // Z++: 𝔹
    bool bTouchingWater{false}; // Z++: 𝔹
    bool bIsSubway{false}; // Z++: 𝔹
    bool bDrawLast{false}; // Z++: 𝔹
    bool bNoBrightHeadLights{false}; // Z++: 𝔹
    bool bDoNotRender{false}; // Z++: 𝔹
    bool bDistanceFade{false}; // Z++: 𝔹
    bool m_flagE1{false}; // Z++: 𝔹
    bool bDontCastShadowsOn{false}; // Z++: 𝔹
    bool bOffscreen{false}; // Z++: 𝔹
    bool bIsStaticWaitingForCollision{false}; // Z++: 𝔹
    bool bDontStream{false}; // Z++: 𝔹
    bool bUnderwater{false}; // Z++: 𝔹
    bool bHasPreRenderEffects{false}; // Z++: 𝔹
    bool bIsTreeModel{false}; // Z++: 𝔹
    bool bIsVehicle{false}; // Z++: 𝔹
    bool bIsPed{false}; // Z++: 𝔹
    bool bMakeVisible{false}; // Z++: 𝔹
    int64_t m_scanCode{0}; // Z++: ℕ
    int64_t m_randomSeed{0}; // Z++: ℕ
    int32_t m_modelIndex{0}; // Z++: ℤ
    int32_t m_level{0}; // Z++: ℤ
    int32_t m_area{0}; // Z++: ℤ
    uint8_t m_pFirstReference_card{0u}; // Z++: Ref[CReference]
    bool bZoneCulled{false}; // Z++: 𝔹
    bool bZoneCulled2{false}; // Z++: 𝔹
    bool m_flagE2{false}; // Z++: 𝔹
    int32_t m_entryInfoList{0}; // Z++: CEntryInfoList
    int32_t pedType{0}; // Z++: ℤ
    int32_t unknown{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_rwObject_card <= 1u)
            && (m_type <= 7u)
            && (m_status <= 31u)
            && (bUsesCollision <= 1u)
            && (bCollisionProcessed <= 1u)
            && (bIsStatic <= 1u)
            && (bHasContacted <= 1u)
            && (bPedPhysics <= 1u)
            && (bIsStuck <= 1u)
            && (bIsInSafePosition <= 1u)
            && (bUseCollisionRecords <= 1u)
            && (bWasPostponed <= 1u)
            && (bExplosionProof <= 1u)
            && (bIsVisible <= 1u)
            && (bHasCollided <= 1u)
            && (bRenderScorched <= 1u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/Gangs.zpp
struct CGangs_ZppContract {
    std::array<bool, 9> GangAttackWithCops{}; // Z++: seq 𝔹  -- bounded array [NUM_GANGS]
    size_t GangAttackWithCops_len{9u}; // Z++ cardinality #GangAttackWithCops
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/Ped.zpp
struct FightMove_ZppContract {
    int32_t animId{0}; // Z++: AnimationId
    float startFireTime{0.0f}; // Z++: ℝ
    float endFireTime{0.0f}; // Z++: ℝ
    float comboFollowOnTime{0.0f}; // Z++: ℝ
    float strikeRadius{0.0f}; // Z++: ℝ
    float extendReachMultiplier{0.0f}; // Z++: ℝ
    int64_t hitLevel{0}; // Z++: ℕ
    int64_t damage{0}; // Z++: ℕ
    int64_t flags{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (hitLevel >= 0)
            && (damage >= 0)
            && (flags >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAI.zpp
struct PedAIModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{7350}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 7350);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CVehicleToEffect_ZppContract {
    uint8_t m_pVehicle_card{0u}; // Z++: Ref[CVehicle]
    std::array<int32_t, 4> m_effects{}; // Z++: seq C2dEffect  -- bounded array [NUM_ATTRACTORS_FOR_ICECREAM_VAN]
    size_t m_effects_len{4u}; // Z++ cardinality #m_effects
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedAtmAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedIceCreamAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedPizzaAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedSeatAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedShelterAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int32_t ms_displacements{0}; // Z++: std__vector_CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedAttractor.zpp
struct CPedStopAttractor_ZppContract {
    uint8_t p2dEffect_card{0u}; // Z++: Ref[C2dEffect]
    uint8_t vApproachingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    uint8_t vWaitingQueue_card{0u}; // Z++: Ref[std__vector_CPed]
    int32_t m_nMaxPedsInAttractor{0}; // Z++: ℤ
    float m_fQueueDistance{0.0f}; // Z++: ℝ
    float m_fTimeInWaitQueue{0.0f}; // Z++: ℝ
    float m_fTimeInApproachingQueue{0.0f}; // Z++: ℝ
    float m_fDistanceToUseAttractor{0.0f}; // Z++: ℝ
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (p2dEffect_card <= 1u)
            && (vApproachingQueue_card <= 1u)
            && (vWaitingQueue_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedChat.zpp
struct PedChatModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{221}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 221);
    }
};
// State Schema Contract from zpp/peds/PedDebug.zpp
struct PedDebugModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{542}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 542);
    }
};
// State Schema Contract from zpp/peds/PedFight.zpp
struct PedFightModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6501}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6501);
    }
};
// State Schema Contract from zpp/peds/PedIK.zpp
struct LimbOrientation_ZppContract {
    float yaw{0.0f}; // Z++: ℝ
    float pitch{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedIK.zpp
struct CPedIK_ZppContract {
    uint8_t m_ped_card{0u}; // Z++: Ref[CPed_Const]
    int32_t m_headOrient{0}; // Z++: LimbOrientation
    int32_t m_torsoOrient{0}; // Z++: LimbOrientation
    int32_t m_upperArmOrient{0}; // Z++: LimbOrientation
    int32_t m_lowerArmOrient{0}; // Z++: LimbOrientation
    int32_t m_flags{0}; // Z++: ℤ
    int32_t ms_torsoInfo{0}; // Z++: LimbMovementInfo
    int32_t ms_headInfo{0}; // Z++: LimbMovementInfo
    int32_t ms_headRestoreInfo{0}; // Z++: LimbMovementInfo
    int32_t ms_upperArmInfo{0}; // Z++: LimbMovementInfo
    int32_t ms_lowerArmInfo{0}; // Z++: LimbMovementInfo
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_ped_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedPlacement.zpp
struct CPedPlacement_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedRoutes.zpp
struct CRouteNode_ZppContract {
    int32_t m_route{0}; // Z++: ℤ
    ZppVec3 m_pos{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedType.zpp
struct CPedType_ZppContract {
    int64_t m_flag{0}; // Z++: ℕ
    float unknown1{0.0f}; // Z++: ℝ
    float unknown2{0.0f}; // Z++: ℝ
    float unknown3{0.0f}; // Z++: ℝ
    float unknown4{0.0f}; // Z++: ℝ
    float unknown5{0.0f}; // Z++: ℝ
    int64_t m_threats{0}; // Z++: ℕ
    int64_t m_avoid{0}; // Z++: ℕ
    std::array<int32_t, 23> ms_apPedType{}; // Z++: seq Ref[CPedType]  -- bounded array [NUM_PEDTYPES]
    size_t ms_apPedType_len{23u}; // Z++ cardinality #ms_apPedType
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_apPedType_len == 23u && ms_apPedType.size() == 23u)
            && (m_flag >= 0)
            && (m_threats >= 0)
            && (m_avoid >= 0);
    }
};
// State Schema Contract from zpp/peds/PlayerPed.zpp
struct animAndGroup_ZppContract {
    int32_t animId{0}; // Z++: AnimationId
    int32_t groupId{0}; // Z++: AssocGroupId
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/Population.zpp
struct RegenerationPoint_ZppContract {
    int32_t srcLevel{0}; // Z++: eLevelName
    int32_t destLevel{0}; // Z++: eLevelName
    float x1{0.0f}; // Z++: ℝ
    float x2{0.0f}; // Z++: ℝ
    float y1{0.0f}; // Z++: ℝ
    float y2{0.0f}; // Z++: ℝ
    float z1{0.0f}; // Z++: ℝ
    float z2{0.0f}; // Z++: ℝ
    ZppVec3 destPosA{}; // Z++: RwV3d
    ZppVec3 destPosB{}; // Z++: RwV3d
    ZppVec3 srcPosA{}; // Z++: RwV3d
    ZppVec3 srcPosB{}; // Z++: RwV3d
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
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
// Query Operation Schema Contract (ΞCPedAtmAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAtmAttractor_GetType(const CPedAtmAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAtmAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAtmAttractor_UpdatePedStateOnDeparture_2(const CPedAtmAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedIceCreamAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedIceCreamAttractor_GetType_2(const CPedIceCreamAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedIceCreamAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedIceCreamAttractor_UpdatePedStateOnDeparture_3(const CPedIceCreamAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedPizzaAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedPizzaAttractor_GetType_2(const CPedPizzaAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedPizzaAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedPizzaAttractor_UpdatePedStateOnDeparture_3(const CPedPizzaAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedSeatAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedSeatAttractor_GetType(const CPedSeatAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedSeatAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedSeatAttractor_UpdatePedStateOnDeparture_2(const CPedSeatAttractor_ZppContract& state) {
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
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_ComputeAttractPos_5(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_ComputeAttractHeading_6(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_GetDisplacement_8(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedStopAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedStopAttractor_GetType(const CPedStopAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedStopAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedStopAttractor_UpdatePedStateOnDeparture_2(const CPedStopAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_ChooseEffect(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_HasThisEffect(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedPizzaAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedPizzaAttractor_UpdatePedStateOnDeparture(const CPedPizzaAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAtmAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAtmAttractor_UpdatePedStateOnDeparture(const CPedAtmAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeDeltaHeading(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeDeltaPos(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractTime(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractPos(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_GetDisplacement(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_ComputeAttractPos(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_ComputeAttractHeading(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedShelterAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedShelterAttractor_ComputeAttractHeading(const CPedShelterAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_IsRegisteredWithPed(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPedAttractor) from zpp/peds/PedAttractor.zpp
constexpr bool op_CPedAttractor_IsInQueue(const CPedAttractor_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCPedType) from zpp/peds/PedType.zpp
constexpr bool op_CPedType_Initialise(const CPedType_ZppContract& pre, const CPedType_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_flag == 1) && (post.unknown1 == 0.0f) && (post.unknown2 == 0.0f) && (post.unknown4 == 0.0f) && (post.unknown5 == 0.0f) && (post.m_threats == 0);
}
constexpr CPedType_ZppContract apply_CPedType_Initialise(const CPedType_ZppContract& pre) {
    CPedType_ZppContract post = pre;
    post.m_flag = 1;
    post.unknown1 = 0.0f;
    post.unknown2 = 0.0f;
    post.unknown4 = 0.0f;
    post.unknown5 = 0.0f;
    post.m_threats = 0;
    return post;
}
constexpr bool verify_peds_zpp_invariants() {
    {
        CDummyPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDummyPed_ZppContract bad{};
        bad.instance_addr = -1;
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
        FightMove_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        FightMove_ZppContract bad{};
        bad.hitLevel = -1;
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
        CPedAtmAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedAtmAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedIceCreamAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedIceCreamAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedPizzaAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedPizzaAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedSeatAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedSeatAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedShelterAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedShelterAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedStopAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedStopAttractor_ZppContract bad{};
        bad.p2dEffect_card = 2u;
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
        CPedIK_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedIK_ZppContract bad{};
        bad.m_ped_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedPlacement_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedPlacement_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRouteNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRouteNode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedType_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedType_ZppContract bad{};
        bad.ms_apPedType_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        animAndGroup_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        animAndGroup_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RegenerationPoint_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RegenerationPoint_ZppContract bad{};
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
        const CPedAtmAttractor_ZppContract st{};
        if (!op_CPedAtmAttractor_GetType(st)) return false;
    }
    {
        const CPedAtmAttractor_ZppContract st{};
        if (!op_CPedAtmAttractor_UpdatePedStateOnDeparture_2(st)) return false;
    }
    {
        const CPedIceCreamAttractor_ZppContract st{};
        if (!op_CPedIceCreamAttractor_GetType_2(st)) return false;
    }
    {
        const CPedIceCreamAttractor_ZppContract st{};
        if (!op_CPedIceCreamAttractor_UpdatePedStateOnDeparture_3(st)) return false;
    }
    {
        const CPedPizzaAttractor_ZppContract st{};
        if (!op_CPedPizzaAttractor_GetType_2(st)) return false;
    }
    {
        const CPedPizzaAttractor_ZppContract st{};
        if (!op_CPedPizzaAttractor_UpdatePedStateOnDeparture_3(st)) return false;
    }
    {
        const CPedSeatAttractor_ZppContract st{};
        if (!op_CPedSeatAttractor_GetType(st)) return false;
    }
    {
        const CPedSeatAttractor_ZppContract st{};
        if (!op_CPedSeatAttractor_UpdatePedStateOnDeparture_2(st)) return false;
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
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_ComputeAttractPos_5(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_ComputeAttractHeading_6(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_GetDisplacement_8(st)) return false;
    }
    {
        const CPedStopAttractor_ZppContract st{};
        if (!op_CPedStopAttractor_GetType(st)) return false;
    }
    {
        const CPedStopAttractor_ZppContract st{};
        if (!op_CPedStopAttractor_UpdatePedStateOnDeparture_2(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_ChooseEffect(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_HasThisEffect(st)) return false;
    }
    {
        const CPedPizzaAttractor_ZppContract st{};
        if (!op_CPedPizzaAttractor_UpdatePedStateOnDeparture(st)) return false;
    }
    {
        const CPedAtmAttractor_ZppContract st{};
        if (!op_CPedAtmAttractor_UpdatePedStateOnDeparture(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeDeltaHeading(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeDeltaPos(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractTime(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractPos(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_GetDisplacement(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_ComputeAttractPos(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_ComputeAttractHeading(st)) return false;
    }
    {
        const CPedShelterAttractor_ZppContract st{};
        if (!op_CPedShelterAttractor_ComputeAttractHeading(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_IsRegisteredWithPed(st)) return false;
    }
    {
        const CPedAttractor_ZppContract st{};
        if (!op_CPedAttractor_IsInQueue(st)) return false;
    }
    {
        CPedType_ZppContract pre{};
        pre.m_flag = 0;
        pre.unknown1 = 1.0f;
        pre.unknown2 = 1.0f;
        pre.unknown4 = 1.0f;
        pre.unknown5 = 1.0f;
        pre.m_threats = 1;
        const auto post = apply_CPedType_Initialise(pre);
        if (!op_CPedType_Initialise(pre, post)) return false;
        {
            CPedType_ZppContract badPost = post;
            badPost.m_flag = 0;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
        {
            CPedType_ZppContract badPost = post;
            badPost.unknown1 = 1.0f;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
        {
            CPedType_ZppContract badPost = post;
            badPost.unknown2 = 1.0f;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
        {
            CPedType_ZppContract badPost = post;
            badPost.unknown4 = 1.0f;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
        {
            CPedType_ZppContract badPost = post;
            badPost.unknown5 = 1.0f;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
        {
            CPedType_ZppContract badPost = post;
            badPost.m_threats = 1;
            if (op_CPedType_Initialise(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_peds_zpp_invariants(), "Z++ formal contract invariant failed for peds");
static_assert(CGangs_ZppContract{}.GangAttackWithCops.size() == 9u, "CGangs NUM_GANGS must be 9");
static_assert([]() constexpr { CPedAtmAttractor_ZppContract bad{}; bad.vWaitingQueue_card = 2u; return !bad.checkInvariant() && !op_CPedAtmAttractor_GetType(bad); }(), "CPedAtmAttractor must inherit CPedAttractor reference cardinality constraints");
} // namespace zpp_peds_spec
#endif // GTANGEL_ZPP_PEDS_SPEC_GUARD
