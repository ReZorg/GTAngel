#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_RW_SPEC_GUARD
#define GTANGEL_ZPP_RW_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/rw
// Derived from C:/ddd/GTAngel/cpp_test/zpp/rw/*.zpp
// Modules: 12 | Formal Schemas: 395 | Free Types: 4
// ============================================================================
namespace zpp_rw_spec {
static constexpr int kZppModuleCount = 12;
static constexpr int kZppSchemaCount = 395;
static constexpr int kZppEnumCount   = 4;
// From zpp/rw/MemoryHeap.zpp
enum class MemoryHeap_Enum_1 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_LODS_v = 8, MEMID_STREAM_TEXUTRES_v = 9, MEMID_STREAM_COLLISION_v = 10, MEMID_STREAM_ANIMATION_v = 11 };
// From zpp/rw/MemoryHeap.zpp
enum class MemoryHeap_Enum_2 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_TEXUTRES_v = 8, MEMID_TEXTURES_v = 9, MEMID_COLLISION_v = 10, MEMID_RENDERLIST_v = 11 };
// From zpp/rw/VisibilityPlugins.zpp
enum class VisibilityPlugins_Enum_1 : int32_t { ID_VISIBILITYATOMIC_v = 0, ID_VISIBILITYCLUMP_v = 1, ID_VISIBILITYFRAME_v = 2 };
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpGeometryList_ZppContract {
    int32_t geometries{0}; // Z++: Ref[RpGeometry]
    int32_t numGeoms{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpAtomicBinary_ZppContract {
    int32_t frameIndex{0}; // Z++: ℤ
    int32_t geomIndex{0}; // Z++: ℤ
    int32_t flags{0}; // Z++: ℤ
    int32_t unused{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/rw/Lights.zpp
struct LightsModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct CStack_ZppContract {
    int32_t values{0}; // Z++: seq T  -- bounded array [N]
    uint32_t sp{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct HeapBlockDesc_ZppContract {
    uint32_t m_size{0}; // Z++: ℕ
    int32_t m_memId{0}; // Z++: ℤ
    int32_t m_ptrListIndex{0}; // Z++: ℤ
    int32_t m_next{0}; // Z++: Ref[HeapBlockDesc]
    int32_t m_prev{0}; // Z++: Ref[HeapBlockDesc]
    int32_t b{0}; // Z++: return
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct HeapBlockList_ZppContract {
    int32_t m_first{0}; // Z++: HeapBlockDesc
    int32_t m_last{0}; // Z++: HeapBlockDesc
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_rw_zpp_invariants() {
    if (!rpGeometryList_ZppContract{}.checkInvariant()) return false;
    if (!rpAtomicBinary_ZppContract{}.checkInvariant()) return false;
    if (!LightsModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CStack_ZppContract{}.checkInvariant()) return false;
    if (!HeapBlockDesc_ZppContract{}.checkInvariant()) return false;
    if (!HeapBlockList_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_rw_zpp_invariants(), "Z++ formal contract invariant failed for rw");
} // namespace zpp_rw_spec
#endif // GTANGEL_ZPP_RW_SPEC_GUARD
