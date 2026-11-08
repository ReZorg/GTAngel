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
constexpr bool verify_buildings_zpp_invariants() {
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_buildings_zpp_invariants(), "Z++ formal contract invariant failed for buildings");
} // namespace zpp_buildings_spec
#endif // GTANGEL_ZPP_BUILDINGS_SPEC_GUARD
