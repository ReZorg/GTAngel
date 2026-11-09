#pragma once
#ifndef GTANGEL_ZPP_LEEDS_SPEC_GUARD
#define GTANGEL_ZPP_LEEDS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/leeds
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/leeds/*.zpp
// Modules: 6 | Formal Schemas: 106 | Free Types: 0
// ============================================================================
namespace zpp_leeds_spec {
static constexpr int kZppModuleCount = 6;
static constexpr int kZppSchemaCount = 106;
static constexpr int kZppEnumCount   = 0;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// State Schema Contract from zpp/leeds/CustomSoundTrack.zpp
struct cCustomSoundTrack_ZppContract {
    uint8_t m_pSingleton_card{0u}; // Z++: Ref[cCustomSoundTrack]
    bool m_bIsPlaying{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pSingleton_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/memoryManager.zpp
struct cMemoryManager_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/memoryManager.zpp
struct cMainMemoryManager_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_pInstance_card{0u}; // Z++: Ref[cMainMemoryManager]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_pInstance_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/base/memoryManager.zpp
struct cMemoryBlock_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/relocatableChunk.zpp
struct cRelocatableChunk_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/relocatableChunk.zpp
struct cRelocatableChunkClassInfo_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/relocatableChunk.zpp
struct cRelocatableChunkWriter_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/leeds/base/sList.zpp
struct cSList_ZppContract {
    int32_t item{0}; // Z++: T
    uint8_t first_card{0u}; // Z++: Ref[tSItem]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (first_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/base/sList.zpp
struct tSItem_ZppContract {
    uint8_t next_card{0u}; // Z++: Ref[tSItem]
    int32_t item{0}; // Z++: T
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (next_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/base/singletonManager.zpp
struct cSingletonManager_ZppContract {
    uint8_t head_card{0u}; // Z++: Ref[cSingletonBase]
    uint8_t tail_card{0u}; // Z++: Ref[cSingletonBase]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (head_card <= 1u)
            && (tail_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/base/singletonManager.zpp
struct cSingletonBase_ZppContract {
    uint8_t next_card{0u}; // Z++: Ref[cSingletonBase]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (next_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/base/singletonManager.zpp
struct cSingleton_ZppContract {
    uint8_t next_card{0u}; // Z++: Ref[cSingletonBase]
    uint8_t mspInstance_card{0u}; // Z++: Ref[T]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (next_card <= 1u)
            && (mspInstance_card <= 1u);
    }
};
// State Schema Contract from zpp/leeds/smallHeap.zpp
struct cSmallHeap_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    bool bLocked{false}; // Z++: 𝔹
    bool bUnk{false}; // Z++: 𝔹
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
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
        bad.m_pSingleton_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cMemoryManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cMemoryManager_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cMainMemoryManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cMainMemoryManager_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cMemoryBlock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cMemoryBlock_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cRelocatableChunk_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cRelocatableChunk_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cRelocatableChunkClassInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cRelocatableChunkClassInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cRelocatableChunkWriter_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cRelocatableChunkWriter_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cSList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cSList_ZppContract bad{};
        bad.first_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        tSItem_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tSItem_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cSingletonManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cSingletonManager_ZppContract bad{};
        bad.head_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cSingletonBase_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cSingletonBase_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cSingleton_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cSingleton_ZppContract bad{};
        bad.next_card = 2u;
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
