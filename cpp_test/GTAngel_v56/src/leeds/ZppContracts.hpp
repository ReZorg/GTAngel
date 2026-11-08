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
// State Schema Contract from zpp/leeds/CustomSoundTrack.zpp
struct cCustomSoundTrack_ZppContract {
    bool m_bIsPlaying{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/smallHeap.zpp
struct cSmallHeap_ZppContract {
    bool bLocked{false}; // Z++: 𝔹
    bool bUnk{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Query Operation Schema Contract (ΞcSmallHeap) from zpp/leeds/smallHeap.zpp
constexpr bool op_cSmallHeap_IsLocked_4(const cSmallHeap_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔcSmallHeap) from zpp/leeds/smallHeap.zpp
constexpr bool op_cSmallHeap_cSmallHeap(const cSmallHeap_ZppContract& pre, const cSmallHeap_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bLocked == false) && (post.bUnk == false);
}
constexpr cSmallHeap_ZppContract apply_cSmallHeap_cSmallHeap(const cSmallHeap_ZppContract& pre) {
    cSmallHeap_ZppContract post = pre;
    post.bLocked = false;
    post.bUnk = false;
    return post;
}
constexpr bool verify_leeds_zpp_invariants() {
    {
        cCustomSoundTrack_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cCustomSoundTrack_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cSmallHeap_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cSmallHeap_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const cSmallHeap_ZppContract st{};
        if (!op_cSmallHeap_IsLocked_4(st)) return false;
    }
    {
        cSmallHeap_ZppContract pre{};
        pre.bLocked = true;
        pre.bUnk = true;
        const auto post = apply_cSmallHeap_cSmallHeap(pre);
        if (!op_cSmallHeap_cSmallHeap(pre, post)) return false;
        {
            cSmallHeap_ZppContract badPost = post;
            badPost.bLocked = true;
            if (op_cSmallHeap_cSmallHeap(pre, badPost)) return false;
        }
        {
            cSmallHeap_ZppContract badPost = post;
            badPost.bUnk = true;
            if (op_cSmallHeap_cSmallHeap(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_leeds_zpp_invariants(), "Z++ formal contract invariant failed for leeds");
} // namespace zpp_leeds_spec
#endif // GTANGEL_ZPP_LEEDS_SPEC_GUARD
