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
// State Schema Contract from zpp/peds/CopPed.zpp
struct CCopPed_ZppContract {
    uint8_t m_nRoadblockVeh_card{0u}; // Z++: Ref[CVehicle]
    float m_fDistanceToTarget{0.0f}; // Z++: ℝ
    bool m_bIsInPursuit{false}; // Z++: 𝔹
    bool m_bIsDisabledCop{false}; // Z++: 𝔹
    int32_t field_5FE{0}; // Z++: ℤ
    bool m_bBeatingSuspect{false}; // Z++: 𝔹
    bool m_bStopAndShootDisabledZone{false}; // Z++: 𝔹
    bool m_bDragsPlayerFromCar{false}; // Z++: 𝔹
    bool m_bZoneDisabled{false}; // Z++: 𝔹
    float m_fAbseilPos{0.0f}; // Z++: ℝ
    int32_t m_nCopType{0}; // Z++: eCopType
    bool m_bThrowsSpikeTrap{false}; // Z++: 𝔹
    uint8_t m_pRopeEntity_card{0u}; // Z++: Ref[CEntity]
    int64_t m_nRopeID{0}; // Z++: ℕ
    int64_t m_nHassleTimer{0}; // Z++: ℕ
    int64_t field_61C{0}; // Z++: ℕ
    int32_t field_624{0}; // Z++: ℤ
    int32_t field_628{0}; // Z++: ℤ
    int32_t m_nRoadblockNode{0}; // Z++: ℤ
    int32_t field_1350{0}; // Z++: ℤ
    int32_t field_1364{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_nRoadblockVeh_card <= 1u)
            && (m_pRopeEntity_card <= 1u)
            && (m_nRopeID >= 0)
            && (m_nHassleTimer >= 0)
            && (field_61C >= 0);
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
    constexpr bool checkInvariant() const {
        return (hitLevel >= 0)
            && (damage >= 0)
            && (flags >= 0);
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
    float m_fAcceptableHeading{0.0f}; // Z++: ℝ
    float m_fMaxPositionDisplacement{0.0f}; // Z++: ℝ
    float m_fMaxHeadingDisplacement{0.0f}; // Z++: ℝ
    ZppVec3 vecEffectPos{}; // Z++: CVector
    ZppVec3 vecQueueDir{}; // Z++: CVector
    ZppVec3 vecUseDir{}; // Z++: CVector
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
    constexpr bool checkInvariant() const {
        return (m_ped_card <= 1u);
    }
};
// State Schema Contract from zpp/peds/PedPlacement.zpp
struct CPedPlacement_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PedRoutes.zpp
struct CRouteNode_ZppContract {
    int32_t m_route{0}; // Z++: ℤ
    ZppVec3 m_pos{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/peds/PlayerPed.zpp
struct CPlayerPed_ZppContract {
    uint8_t m_pWanted_card{0u}; // Z++: Ref[CWanted]
    uint8_t m_pArrestingCop_card{0u}; // Z++: Ref[CCopPed]
    float m_fMoveSpeed{0.0f}; // Z++: ℝ
    float m_fCurrentStamina{0.0f}; // Z++: ℝ
    float m_fMaxStamina{0.0f}; // Z++: ℝ
    float m_fStaminaProgress{0.0f}; // Z++: ℝ
    int32_t m_nSelectedWepSlot{0}; // Z++: ℤ
    bool m_bSpeedTimerFlag{false}; // Z++: 𝔹
    int64_t m_nEvadeAmount{0}; // Z++: ℕ
    int64_t m_nSpeedTimer{0}; // Z++: ℕ
    int64_t m_nHitAnimDelayTimer{0}; // Z++: ℕ
    float m_fAttackButtonCounter{0.0f}; // Z++: ℝ
    bool m_bHaveTargetSelected{false}; // Z++: 𝔹
    uint8_t m_pEvadingFrom_card{0u}; // Z++: Ref[CEntity]
    std::array<int32_t, 4> m_nTargettableObjects{}; // Z++: seq ℤ  -- bounded array [4]
    size_t m_nTargettableObjects_len{4u}; // Z++ cardinality #m_nTargettableObjects
    int64_t m_nAdrenalineTime{0}; // Z++: ℕ
    int64_t m_nDrunkenness{0}; // Z++: ℕ
    int64_t m_nFadeDrunkenness{0}; // Z++: ℕ
    int64_t m_nDrunkCountdown{0}; // Z++: ℕ
    bool m_bAdrenalineActive{false}; // Z++: 𝔹
    bool m_bHasLockOnTarget{false}; // Z++: 𝔹
    bool m_bCanBeDamaged{false}; // Z++: 𝔹
    bool m_bNoPosForMeleeAttack{false}; // Z++: 𝔹
    bool unk1{false}; // Z++: 𝔹
    std::array<ZppVec3, 6> m_vecSafePos{}; // Z++: seq CVector  -- bounded array [6]
    size_t m_vecSafePos_len{6u}; // Z++ cardinality #m_vecSafePos
    std::array<int32_t, 6> m_pPedAtSafePos{}; // Z++: seq Ref[CPed]  -- bounded array [6]
    size_t m_pPedAtSafePos_len{6u}; // Z++ cardinality #m_pPedAtSafePos
    std::array<int32_t, 6> m_pMeleeList{}; // Z++: seq Ref[CPed]  -- bounded array [6]
    size_t m_pMeleeList_len{6u}; // Z++ cardinality #m_pMeleeList
    int32_t m_nAttackDirToCheck{0}; // Z++: ℤ
    float m_fWalkAngle{0.0f}; // Z++: ℝ
    float m_fFPSMoveHeading{0.0f}; // Z++: ℝ
    uint8_t m_pMinigunTopAtomic_card{0u}; // Z++: Ref[RpAtomic]
    float m_fGunSpinSpeed{0.0f}; // Z++: ℝ
    float m_fGunSpinAngle{0.0f}; // Z++: ℝ
    int32_t m_nPadDownPressedInMilliseconds{0}; // Z++: ℤ
    int32_t m_nLastBusFareCollected{0}; // Z++: ℤ
    ZppVec3 m_cachedCamSource{}; // Z++: CVector
    ZppVec3 m_cachedCamFront{}; // Z++: CVector
    ZppVec3 m_cachedCamUp{}; // Z++: CVector
    int32_t field_1367{0}; // Z++: ℤ
    int32_t field_1413{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_nTargettableObjects_len == 4u && m_nTargettableObjects.size() == 4u)
            && (m_vecSafePos_len == 6u && m_vecSafePos.size() == 6u)
            && (m_pPedAtSafePos_len == 6u && m_pPedAtSafePos.size() == 6u)
            && (m_pMeleeList_len == 6u && m_pMeleeList.size() == 6u)
            && (m_pWanted_card <= 1u)
            && (m_pArrestingCop_card <= 1u)
            && (m_nEvadeAmount >= 0)
            && (m_nSpeedTimer >= 0)
            && (m_nHitAnimDelayTimer >= 0)
            && (m_pEvadingFrom_card <= 1u)
            && (m_nAdrenalineTime >= 0)
            && (m_nDrunkenness >= 0)
            && (m_nFadeDrunkenness >= 0)
            && (m_nDrunkCountdown >= 0)
            && (m_pMinigunTopAtomic_card <= 1u);
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
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_ChooseEffect(const CVehicleToEffect_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVehicleToEffect) from zpp/peds/PedAttractor.zpp
constexpr bool op_CVehicleToEffect_HasThisEffect(const CVehicleToEffect_ZppContract& state) {
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
// Delta Operation Schema Contract (ΔCPlayerPed) from zpp/peds/PlayerPed.zpp
constexpr bool op_CPlayerPed_ClearAdrenaline(const CPlayerPed_ZppContract& pre, const CPlayerPed_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nAdrenalineTime == 0);
}
constexpr CPlayerPed_ZppContract apply_CPlayerPed_ClearAdrenaline(const CPlayerPed_ZppContract& pre) {
    CPlayerPed_ZppContract post = pre;
    post.m_nAdrenalineTime = 0;
    return post;
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
        CCopPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCopPed_ZppContract bad{};
        bad.m_nRoadblockVeh_card = 2u;
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
        CPlayerPed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlayerPed_ZppContract bad{};
        bad.m_nTargettableObjects_len = 0u;
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
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_ChooseEffect(st)) return false;
    }
    {
        const CVehicleToEffect_ZppContract st{};
        if (!op_CVehicleToEffect_HasThisEffect(st)) return false;
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
    {
        CPlayerPed_ZppContract pre{};
        pre.m_nAdrenalineTime = 1;
        const auto post = apply_CPlayerPed_ClearAdrenaline(pre);
        if (!op_CPlayerPed_ClearAdrenaline(pre, post)) return false;
        {
            CPlayerPed_ZppContract badPost = post;
            badPost.m_nAdrenalineTime = 1;
            if (op_CPlayerPed_ClearAdrenaline(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_peds_zpp_invariants(), "Z++ formal contract invariant failed for peds");
static_assert(CGangs_ZppContract{}.GangAttackWithCops.size() == 9u, "CGangs NUM_GANGS must be 9");
} // namespace zpp_peds_spec
#endif // GTANGEL_ZPP_PEDS_SPEC_GUARD
