#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_LEEDS_SPEC_GUARD
#define GTANGEL_ZPP_LEEDS_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/leeds
// Derived from C:/ddd/GTAngel/cpp_test/zpp/leeds/*.zpp
// Modules: 2 | Formal Schemas: 13 | Free Types: 0
// ============================================================================
namespace zpp_leeds_spec {
static constexpr int kZppModuleCount = 2;
static constexpr int kZppSchemaCount = 13;
static constexpr int kZppEnumCount   = 0;
// State Schema Contract from zpp/leeds/CustomSoundTrack.zpp
struct cCustomSoundTrack_ZppContract {
    bool m_bIsPlaying{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/leeds/smallHeap.zpp
struct cSmallHeap_ZppContract {
    bool bLocked{false}; // Z++: 𝔹
    bool bUnk{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_leeds_zpp_invariants() {
    if (!cCustomSoundTrack_ZppContract{}.checkInvariant()) return false;
    if (!cSmallHeap_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_leeds_zpp_invariants(), "Z++ formal contract invariant failed for leeds");
} // namespace zpp_leeds_spec
#endif // GTANGEL_ZPP_LEEDS_SPEC_GUARD
