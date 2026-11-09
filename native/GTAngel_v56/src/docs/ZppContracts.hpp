#pragma once
#ifndef GTANGEL_ZPP_DOCS_SPEC_GUARD
#define GTANGEL_ZPP_DOCS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/docs
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/docs/*.zpp
// Modules: 1 | Formal Schemas: 0 | Free Types: 0
// ============================================================================
namespace zpp_docs_spec {
static constexpr int kZppModuleCount = 1;
static constexpr int kZppSchemaCount = 0;
static constexpr int kZppEnumCount   = 0;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
constexpr bool verify_docs_zpp_invariants() {
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_docs_zpp_invariants(), "Z++ formal contract invariant failed for docs");
} // namespace zpp_docs_spec
#endif // GTANGEL_ZPP_DOCS_SPEC_GUARD
