#pragma once
#ifndef GTANGEL_ZPP_ENTITIES_SPEC_GUARD
#define GTANGEL_ZPP_ENTITIES_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/entities
// Derived from C:/ddd/GTAngel/cpp_test/zpp/entities/*.zpp
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
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
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
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_entities_zpp_invariants(), "Z++ formal contract invariant failed for entities");
} // namespace zpp_entities_spec
#endif // GTANGEL_ZPP_ENTITIES_SPEC_GUARD
