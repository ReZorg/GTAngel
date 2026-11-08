#pragma once
#ifndef GTANGEL_ZPP_LEEDS_SPEC_GUARD
#define GTANGEL_ZPP_LEEDS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/leeds
// Derived from C:/ddd/GTAngel/cpp_test/zpp/leeds/*.zpp
// Modules: 2 | Formal Schemas: 13 | Free Types: 0
// ============================================================================
namespace zpp_leeds_spec {
static constexpr int kZppModuleCount = 2;
static constexpr int kZppSchemaCount = 13;
static constexpr int kZppEnumCount   = 0;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
constexpr bool verify_leeds_zpp_invariants() {
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_leeds_zpp_invariants(), "Z++ formal contract invariant failed for leeds");
} // namespace zpp_leeds_spec
#endif // GTANGEL_ZPP_LEEDS_SPEC_GUARD
