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
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/buildings/Solid.zpp
struct CSolid_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/buildings/Treadable.zpp
struct CTreadable_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
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
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_buildings_zpp_invariants(), "Z++ formal contract invariant failed for buildings");
} // namespace zpp_buildings_spec
#endif // GTANGEL_ZPP_BUILDINGS_SPEC_GUARD
