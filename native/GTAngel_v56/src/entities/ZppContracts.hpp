#pragma once
#ifndef GTANGEL_ZPP_ENTITIES_SPEC_GUARD
#define GTANGEL_ZPP_ENTITIES_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/entities
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/entities/*.zpp
// Modules: 3 | Formal Schemas: 190 | Free Types: 3
// ============================================================================
namespace zpp_entities_spec {
static constexpr int kZppModuleCount = 3;
static constexpr int kZppSchemaCount = 190;
static constexpr int kZppEnumCount   = 3;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/entities/Entity.zpp
enum class eEntityType : int32_t { ENTITY_TYPE_NOTHING_v = 0, ENTITY_TYPE_BUILDING_v = 1, ENTITY_TYPE_VEHICLE_v = 2, ENTITY_TYPE_PED_v = 3, ENTITY_TYPE_OBJECT_v = 4, ENTITY_TYPE_DUMMY_v = 5, ENTITY_TYPE_MULTIPLAYER_v = 6 };
// Free Type from zpp/entities/Entity.zpp
enum class eEntityStatus : int32_t { STATUS_PLAYER_v = 0, STATUS_PLAYER_PLAYBACKFROMBUFFER_v = 1, STATUS_SIMPLE_v = 2, STATUS_PHYSICS_v = 3, STATUS_ABANDONED_v = 4, STATUS_WRECKED_v = 5, STATUS_TRAIN_MOVING_v = 6, STATUS_TRAIN_NOT_MOVING_v = 7, STATUS_FERRY_MOVING_v = 8, STATUS_FERRY_NOT_MOVING_v = 9, STATUS_HELI_v = 10, STATUS_PLANE_v = 11, STATUS_PLAYER_REMOTE_v = 12, STATUS_PLAYER_DISABLED_v = 13, STATUS_GHOST_v = 14 };
// State Schema Contract from zpp/entities/Dummy.zpp
struct CDummy_ZppContract {
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
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/entities/Entity.zpp
struct CEntity_ZppContract {
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
            && (bRenderScorched <= 1u);
    }
};
// State Schema Contract from zpp/entities/Physical.zpp
struct CPhysical_ZppContract {
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
    int32_t m_audioEntityId{0}; // Z++: ℤ
    float m_phys_unused1{0.0f}; // Z++: ℝ
    int64_t m_nLastTimeCollided{0}; // Z++: ℕ
    ZppVec3 m_vecMoveSpeed{}; // Z++: CVector
    ZppVec3 m_vecTurnSpeed{}; // Z++: CVector
    ZppVec3 m_vecMoveFriction{}; // Z++: CVector
    ZppVec3 m_vecTurnFriction{}; // Z++: CVector
    ZppVec3 m_vecMoveSpeedAvg{}; // Z++: CVector
    ZppVec3 m_vecTurnSpeedAvg{}; // Z++: CVector
    float m_fMass{0.0f}; // Z++: ℝ
    float m_fTurnMass{0.0f}; // Z++: ℝ
    float m_fForceMultiplier{0.0f}; // Z++: ℝ
    float m_fAirResistance{0.0f}; // Z++: ℝ
    float m_fElasticity{0.0f}; // Z++: ℝ
    float m_fBuoyancy{0.0f}; // Z++: ℝ
    ZppVec3 m_vecCentreOfMass{}; // Z++: CVector
    int32_t m_entryInfoList{0}; // Z++: CEntryInfoList
    uint8_t m_movingListNode_card{0u}; // Z++: Ref[CPtrNode]
    int32_t m_phys_unused2{0}; // Z++: ℤ
    int64_t m_nStaticFrames{0}; // Z++: ℕ
    int64_t m_nCollisionRecords{0}; // Z++: ℕ
    std::array<int32_t, 6> m_aCollisionRecords{}; // Z++: seq Ref[CEntity]  -- bounded array [PHYSICAL_MAX_COLLISIONRECORDS]
    size_t m_aCollisionRecords_len{6u}; // Z++ cardinality #m_aCollisionRecords
    float m_fDistanceTravelled{0.0f}; // Z++: ℝ
    float m_fDamageImpulse{0.0f}; // Z++: ℝ
    uint8_t m_pDamageEntity_card{0u}; // Z++: Ref[CEntity]
    ZppVec3 m_vecDamageNormal{}; // Z++: CVector
    int32_t m_nDamagePieceType{0}; // Z++: ℤ
    uint32_t bIsHeavy{0u}; // Z++: 𝔹
    uint32_t bAffectedByGravity{0u}; // Z++: 𝔹
    uint32_t bInfiniteMass{0u}; // Z++: 𝔹
    uint32_t m_phy_flagA08{0u}; // Z++: 𝔹
    uint32_t bIsInWater{0u}; // Z++: 𝔹
    uint32_t m_phy_flagA20{0u}; // Z++: 𝔹
    uint32_t bHitByTrain{0u}; // Z++: 𝔹
    uint32_t bSkipLineCol{0u}; // Z++: 𝔹
    uint32_t bIsFrozen{0u}; // Z++: 𝔹
    uint32_t bDontLoadCollision{0u}; // Z++: 𝔹
    uint32_t m_bIsVehicleBeingShifted{0u}; // Z++: 𝔹
    bool bJustCheckCollision{false}; // Z++: 𝔹
    bool phys_lcs_unk1{false}; // Z++: 𝔹
    int64_t m_nSurfaceTouched{0}; // Z++: ℕ
    int32_t m_nZoneLevel{0}; // Z++: ℤ
    int32_t phys_lcs_unk2{0}; // Z++: ℤ
    std::array<int32_t, 2> m_treadable{}; // Z++: seq Ref[CTreadable]  -- bounded array [2]
    size_t m_treadable_len{2u}; // Z++ cardinality #m_treadable
    bool m_phy_flagA10{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_aCollisionRecords_len == 6u && m_aCollisionRecords.size() == 6u)
            && (m_treadable_len == 2u && m_treadable.size() == 2u)
            && (instance_addr >= 0)
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
            && (m_nLastTimeCollided >= 0)
            && (m_movingListNode_card <= 1u)
            && (m_nStaticFrames >= 0)
            && (m_nCollisionRecords >= 0)
            && (m_pDamageEntity_card <= 1u)
            && (bIsHeavy <= 1u)
            && (bAffectedByGravity <= 1u)
            && (bInfiniteMass <= 1u)
            && (m_phy_flagA08 <= 1u)
            && (bIsInWater <= 1u)
            && (m_phy_flagA20 <= 1u)
            && (bHitByTrain <= 1u)
            && (bSkipLineCol <= 1u)
            && (bIsFrozen <= 1u)
            && (bDontLoadCollision <= 1u)
            && (m_bIsVehicleBeingShifted <= 1u);
    }
};
// Query Operation Schema Contract (ΞCEntity) from zpp/entities/Entity.zpp
constexpr bool op_CEntity_GetType(const CEntity_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCEntity) from zpp/entities/Entity.zpp
constexpr bool op_CEntity_GetStatus_3(const CEntity_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCEntity) from zpp/entities/Entity.zpp
constexpr bool op_CEntity_GetIsStatic_6(const CEntity_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCEntity) from zpp/entities/Entity.zpp
constexpr bool op_CEntity_DetachFromRwObject(const CEntity_ZppContract& pre, const CEntity_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_rwObject_card == 0u);
}
constexpr CEntity_ZppContract apply_CEntity_DetachFromRwObject(const CEntity_ZppContract& pre) {
    CEntity_ZppContract post = pre;
    post.m_rwObject_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCPhysical) from zpp/entities/Physical.zpp
constexpr bool op_CPhysical_RemoveFromMovingList(const CPhysical_ZppContract& pre, const CPhysical_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_movingListNode_card == 0u);
}
constexpr CPhysical_ZppContract apply_CPhysical_RemoveFromMovingList(const CPhysical_ZppContract& pre) {
    CPhysical_ZppContract post = pre;
    post.m_movingListNode_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCPhysical) from zpp/entities/Physical.zpp
constexpr bool op_CPhysical_ProcessEntityCollision(const CPhysical_ZppContract& pre, const CPhysical_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bHasHitWall == true);
}
constexpr CPhysical_ZppContract apply_CPhysical_ProcessEntityCollision(const CPhysical_ZppContract& pre) {
    CPhysical_ZppContract post = pre;
    post.bHasHitWall = true;
    return post;
}
constexpr bool verify_entities_zpp_invariants() {
    {
        CDummy_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDummy_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CEntity_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEntity_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPhysical_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPhysical_ZppContract bad{};
        bad.m_aCollisionRecords_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        const CEntity_ZppContract st{};
        if (!op_CEntity_GetType(st)) return false;
    }
    {
        const CEntity_ZppContract st{};
        if (!op_CEntity_GetStatus_3(st)) return false;
    }
    {
        const CEntity_ZppContract st{};
        if (!op_CEntity_GetIsStatic_6(st)) return false;
    }
    {
        CEntity_ZppContract pre{};
        pre.m_rwObject_card = 1u;
        const auto post = apply_CEntity_DetachFromRwObject(pre);
        if (!op_CEntity_DetachFromRwObject(pre, post)) return false;
        {
            CEntity_ZppContract badPost = post;
            badPost.m_rwObject_card = 1u;
            if (op_CEntity_DetachFromRwObject(pre, badPost)) return false;
        }
    }
    {
        CPhysical_ZppContract pre{};
        pre.m_movingListNode_card = 1u;
        const auto post = apply_CPhysical_RemoveFromMovingList(pre);
        if (!op_CPhysical_RemoveFromMovingList(pre, post)) return false;
        {
            CPhysical_ZppContract badPost = post;
            badPost.m_movingListNode_card = 1u;
            if (op_CPhysical_RemoveFromMovingList(pre, badPost)) return false;
        }
    }
    {
        CPhysical_ZppContract pre{};
        pre.bHasHitWall = false;
        const auto post = apply_CPhysical_ProcessEntityCollision(pre);
        if (!op_CPhysical_ProcessEntityCollision(pre, post)) return false;
        {
            CPhysical_ZppContract badPost = post;
            badPost.bHasHitWall = false;
            if (op_CPhysical_ProcessEntityCollision(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_entities_zpp_invariants(), "Z++ formal contract invariant failed for entities");
} // namespace zpp_entities_spec
#endif // GTANGEL_ZPP_ENTITIES_SPEC_GUARD
