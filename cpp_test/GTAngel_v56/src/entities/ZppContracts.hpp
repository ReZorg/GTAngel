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
// Free Type from zpp/entities/Entity.zpp
enum class eEntityType : int32_t { ENTITY_TYPE_NOTHING_v = 0, ENTITY_TYPE_BUILDING_v = 1, ENTITY_TYPE_VEHICLE_v = 2, ENTITY_TYPE_PED_v = 3, ENTITY_TYPE_OBJECT_v = 4, ENTITY_TYPE_DUMMY_v = 5, ENTITY_TYPE_MULTIPLAYER_v = 6 };
// Free Type from zpp/entities/Entity.zpp
enum class eEntityStatus : int32_t { STATUS_PLAYER_v = 0, STATUS_PLAYER_PLAYBACKFROMBUFFER_v = 1, STATUS_SIMPLE_v = 2, STATUS_PHYSICS_v = 3, STATUS_ABANDONED_v = 4, STATUS_WRECKED_v = 5, STATUS_TRAIN_MOVING_v = 6, STATUS_TRAIN_NOT_MOVING_v = 7, STATUS_FERRY_MOVING_v = 8, STATUS_FERRY_NOT_MOVING_v = 9, STATUS_HELI_v = 10, STATUS_PLANE_v = 11, STATUS_PLAYER_REMOTE_v = 12, STATUS_PLAYER_DISABLED_v = 13, STATUS_GHOST_v = 14 };
// State Schema Contract from zpp/entities/Dummy.zpp
struct CDummy_ZppContract {
    int32_t m_entryInfoList{0}; // Z++: CEntryInfoList
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
constexpr bool verify_entities_zpp_invariants() {
    {
        CDummy_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDummy_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_entities_zpp_invariants(), "Z++ formal contract invariant failed for entities");
} // namespace zpp_entities_spec
#endif // GTANGEL_ZPP_ENTITIES_SPEC_GUARD
