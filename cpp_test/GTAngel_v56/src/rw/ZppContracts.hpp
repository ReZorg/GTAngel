#pragma once
#ifndef GTANGEL_ZPP_RW_SPEC_GUARD
#define GTANGEL_ZPP_RW_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
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
enum class MemoryHeap_Enum_1 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_LODS_v = 8, MEMID_STREAM_TEXUTRES_v = 9, MEMID_STREAM_COLLISION_v = 10, MEMID_STREAM_ANIMATION_v = 11, MEMID_TEXTURES_v = 12, MEMID_COLLISION_v = 13, MEMID_PRE_ALLOC_v = 14, MEMID_GAME_PROCESS_v = 15, MEMID_SCRIPT_v = 16, MEMID_CARS_v = 17, MEMID_RENDER_v = 18, MEMID_PED_ATTR_v = 19, NUM_MEMIDS_v = 20, NUM_FIXED_MEMBLOCKS_v = 6 };
// From zpp/rw/MemoryHeap.zpp
enum class MemoryHeap_Enum_2 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_TEXUTRES_v = 8, MEMID_TEXTURES_v = 9, MEMID_COLLISION_v = 10, MEMID_RENDERLIST_v = 11, MEMID_GAME_PROCESS_v = 12, MEMID_SCRIPT_v = 13, MEMID_CARS_v = 14, MEMID_RENDER_v = 15, MEMID_FRONTEND_v = 17, NUM_MEMIDS_v = 18, NUM_FIXED_MEMBLOCKS_v = 6 };
// From zpp/rw/VisibilityPlugins.zpp
enum class VisibilityPlugins_Enum_1 : int32_t { ID_VISIBILITYATOMIC_v = 0, ID_VISIBILITYCLUMP_v = 1, ID_VISIBILITYFRAME_v = 2 };
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpGeometryList_ZppContract {
    uint8_t geometries_card{0u}; // Z++: Ref[RpGeometry]
    int32_t numGeoms{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (geometries_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/Lights.zpp
struct LightsModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{492}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 492);
    }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct CStack_ZppContract {
    int32_t values{0}; // Z++: seq T  -- bounded array [N]
    int64_t sp{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (sp >= 0);
    }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct HeapBlockDesc_ZppContract {
    int64_t m_size{0}; // Z++: ℕ
    int32_t m_memId{0}; // Z++: ℤ
    int32_t m_ptrListIndex{0}; // Z++: ℤ
    uint8_t m_next_card{0u}; // Z++: Ref[HeapBlockDesc]
    uint8_t m_prev_card{0u}; // Z++: Ref[HeapBlockDesc]
    int32_t b{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (m_size >= 0)
            && (m_next_card <= 1u)
            && (m_prev_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct CommonSize_ZppContract {
    int32_t m_freeList{0}; // Z++: HeapBlockList
    int64_t m_size{0}; // Z++: ℕ
    int64_t m_failed{0}; // Z++: ℕ
    int64_t m_remaining{0}; // Z++: ℕ
    int32_t nil{0}; // Z++: return
    int32_t block{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (m_size >= 0)
            && (m_failed >= 0)
            && (m_remaining >= 0);
    }
};
// State Schema Contract from zpp/rw/MemoryMgr.zpp
struct MemoryMgrModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{142}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 142);
    }
};
constexpr bool verify_rw_zpp_invariants() {
    if (!rpGeometryList_ZppContract{}.checkInvariant()) return false;
    if (!LightsModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CStack_ZppContract{}.checkInvariant()) return false;
    if (!HeapBlockDesc_ZppContract{}.checkInvariant()) return false;
    if (!CommonSize_ZppContract{}.checkInvariant()) return false;
    if (!MemoryMgrModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_rw_zpp_invariants(), "Z++ formal contract invariant failed for rw");
} // namespace zpp_rw_spec
#endif // GTANGEL_ZPP_RW_SPEC_GUARD
