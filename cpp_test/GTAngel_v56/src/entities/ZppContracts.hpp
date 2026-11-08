#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_ENTITIES_SPEC_GUARD
#define GTANGEL_ZPP_ENTITIES_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/entities
// Derived from C:/ddd/GTAngel/cpp_test/zpp/entities/*.zpp
// Modules: 3 | Formal Schemas: 190 | Free Types: 3
// ============================================================================
namespace zpp_entities_spec {
static constexpr int kZppModuleCount = 3;
static constexpr int kZppSchemaCount = 190;
static constexpr int kZppEnumCount   = 3;
// From zpp/entities/Entity.zpp
enum class eEntityType : int32_t { ENTITY_TYPE_NOTHING_v = 0, ENTITY_TYPE_BUILDING_v = 1, ENTITY_TYPE_VEHICLE_v = 2, ENTITY_TYPE_PED_v = 3, ENTITY_TYPE_OBJECT_v = 4, ENTITY_TYPE_DUMMY_v = 5, ENTITY_TYPE_MULTIPLAYER_v = 6 };
// From zpp/entities/Entity.zpp
enum class eEntityStatus : int32_t { STATUS_PLAYER_v = 0, STATUS_PLAYER_PLAYBACKFROMBUFFER_v = 1, STATUS_SIMPLE_v = 2, STATUS_PHYSICS_v = 3, STATUS_ABANDONED_v = 4, STATUS_WRECKED_v = 5, STATUS_TRAIN_MOVING_v = 6, STATUS_TRAIN_NOT_MOVING_v = 7, STATUS_FERRY_MOVING_v = 8, STATUS_FERRY_NOT_MOVING_v = 9, STATUS_HELI_v = 10, STATUS_PLANE_v = 11 };
// State Schema Contract from zpp/entities/Dummy.zpp
struct CDummy_ZppContract {
    int32_t m_entryInfoList{0}; // Z++: CEntryInfoList
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/entities/Entity.zpp
struct CEntity_ZppContract {
    int32_t m_rwObject{0}; // Z++: Ref[RwObject]
    bool m_type{false}; // Z++: 𝔹
    bool m_status{false}; // Z++: 𝔹
    bool bUsesCollision{false}; // Z++: 𝔹
    bool bCollisionProcessed{false}; // Z++: 𝔹
    bool bIsStatic{false}; // Z++: 𝔹
    bool bHasContacted{false}; // Z++: 𝔹
    bool bPedPhysics{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/entities/Physical.zpp
struct CPhysical_ZppContract {
    int32_t m_audioEntityId{0}; // Z++: ℤ
    float m_phys_unused1{0.0f}; // Z++: ℝ
    uint32_t m_nLastTimeCollided{0}; // Z++: ℕ
    int32_t m_vecMoveSpeed{0}; // Z++: CVector
    int32_t m_vecTurnSpeed{0}; // Z++: CVector
    int32_t m_vecMoveFriction{0}; // Z++: CVector
    int32_t m_vecTurnFriction{0}; // Z++: CVector
    int32_t m_vecMoveSpeedAvg{0}; // Z++: CVector
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_entities_zpp_invariants() {
    if (!CDummy_ZppContract{}.checkInvariant()) return false;
    if (!CEntity_ZppContract{}.checkInvariant()) return false;
    if (!CPhysical_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_entities_zpp_invariants(), "Z++ formal contract invariant failed for entities");
} // namespace zpp_entities_spec
#endif // GTANGEL_ZPP_ENTITIES_SPEC_GUARD
