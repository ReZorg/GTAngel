#pragma once
#ifndef GTANGEL_ZPP_BUILDINGS_SPEC_GUARD
#define GTANGEL_ZPP_BUILDINGS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/buildings
// Derived from C:/ddd/GTAngel/cpp_test/zpp/buildings/*.zpp
// Modules: 3 | Formal Schemas: 18 | Free Types: 0
// ============================================================================
namespace zpp_buildings_spec {
static constexpr int kZppModuleCount = 3;
static constexpr int kZppSchemaCount = 18;
static constexpr int kZppEnumCount   = 0;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// State Schema Contract from zpp/buildings/Building.zpp
struct CBuilding_ZppContract {
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
            && (bRenderScorched <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/buildings/Solid.zpp
struct CSolid_ZppContract {
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
            && (bRenderScorched <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/buildings/Treadable.zpp
struct CTreadable_ZppContract {
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
            && (bRenderScorched <= 1u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// Delta Operation Schema Contract (ΔCBuilding) from zpp/buildings/Building.zpp
constexpr bool op_CBuilding_ReplaceWithNewModel(const CBuilding_ZppContract& pre, const CBuilding_ZppContract& post, int32_t id_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_modelIndex == (int32_t)id_in);
}
constexpr CBuilding_ZppContract apply_CBuilding_ReplaceWithNewModel(const CBuilding_ZppContract& pre, int32_t id_in = 25) {
    CBuilding_ZppContract post = pre;
    post.m_modelIndex = (int32_t)id_in;
    return post;
}
constexpr bool verify_buildings_zpp_invariants() {
    {
        CBuilding_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBuilding_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSolid_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSolid_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTreadable_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTreadable_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CBuilding_ZppContract pre{};
        const int32_t id_in = 25;
        pre.m_modelIndex = 0;
        const auto post = apply_CBuilding_ReplaceWithNewModel(pre, id_in);
        if (!op_CBuilding_ReplaceWithNewModel(pre, post, id_in)) return false;
        {
            CBuilding_ZppContract badPost = post;
            badPost.m_modelIndex = (int32_t)(id_in + 1);
            if (op_CBuilding_ReplaceWithNewModel(pre, badPost, id_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_buildings_zpp_invariants(), "Z++ formal contract invariant failed for buildings");
} // namespace zpp_buildings_spec
#endif // GTANGEL_ZPP_BUILDINGS_SPEC_GUARD
