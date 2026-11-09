#pragma once
#ifndef GTANGEL_ZPP_RW_SPEC_GUARD
#define GTANGEL_ZPP_RW_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/rw
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/rw/*.zpp
// Modules: 12 | Formal Schemas: 395 | Free Types: 4
// ============================================================================
namespace zpp_rw_spec {
static constexpr int kZppModuleCount = 12;
static constexpr int kZppSchemaCount = 395;
static constexpr int kZppEnumCount   = 4;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/rw/MemoryHeap.zpp
enum class MemoryHeap_Enum_1 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_LODS_v = 8, MEMID_STREAM_TEXUTRES_v = 9, MEMID_STREAM_COLLISION_v = 10, MEMID_STREAM_ANIMATION_v = 11, MEMID_TEXTURES_v = 12, MEMID_COLLISION_v = 13, MEMID_PRE_ALLOC_v = 14, MEMID_GAME_PROCESS_v = 15, MEMID_SCRIPT_v = 16, MEMID_CARS_v = 17, MEMID_RENDER_v = 18, MEMID_PED_ATTR_v = 19, NUM_MEMIDS_v = 20, NUM_FIXED_MEMBLOCKS_v = 6 };
// Free Type from zpp/rw/MemoryHeap.zpp
enum class MemoryHeap_Enum_2 : int32_t { MEMID_FREE_v = 0, MEMID_GAME_v = 1, MEMID_WORLD_v = 2, MEMID_ANIMATION_v = 3, MEMID_POOLS_v = 4, MEMID_DEF_MODELS_v = 5, MEMID_STREAM_v = 6, MEMID_STREAM_MODELS_v = 7, MEMID_STREAM_TEXUTRES_v = 8, MEMID_TEXTURES_v = 9, MEMID_COLLISION_v = 10, MEMID_RENDERLIST_v = 11, MEMID_GAME_PROCESS_v = 12, MEMID_SCRIPT_v = 13, MEMID_CARS_v = 14, MEMID_RENDER_v = 15, MEMID_FRONTEND_v = 17, NUM_MEMIDS_v = 18, NUM_FIXED_MEMBLOCKS_v = 6 };
// Free Type from zpp/rw/VisibilityPlugins.zpp
enum class VisibilityPlugins_Enum_1 : int32_t { ID_VISIBILITYATOMIC_v = 0, ID_VISIBILITYCLUMP_v = 1, ID_VISIBILITYFRAME_v = 2 };
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpGeometryList_ZppContract {
    uint8_t geometries_card{0u}; // Z++: Ref[RpGeometry]
    int32_t numGeoms{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (geometries_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpAtomicBinary_ZppContract {
    int32_t frameIndex{0}; // Z++: ℤ
    int32_t geomIndex{0}; // Z++: ℤ
    int32_t flags{0}; // Z++: ℤ
    int32_t unused{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/rw/Lights.zpp
struct LightsModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{492}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 492);
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_size >= 0)
            && (m_next_card <= 1u)
            && (m_prev_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct HeapBlockList_ZppContract {
    int32_t m_first{0}; // Z++: HeapBlockDesc
    int32_t m_last{0}; // Z++: HeapBlockDesc
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_size >= 0)
            && (m_failed >= 0)
            && (m_remaining >= 0);
    }
};
// State Schema Contract from zpp/rw/MemoryHeap.zpp
struct CMemoryHeap_ZppContract {
    uint8_t m_start_card{0u}; // Z++: Ref[HeapBlockDesc]
    uint8_t m_end_card{0u}; // Z++: Ref[HeapBlockDesc]
    int32_t m_freeList{0}; // Z++: HeapBlockList
    std::array<int32_t, 6> m_fixedSize{}; // Z++: seq CommonSize  -- bounded array [NUM_FIXED_MEMBLOCKS]
    size_t m_fixedSize_len{6u}; // Z++ cardinality #m_fixedSize
    int64_t m_totalMemUsed{0}; // Z++: ℕ
    int32_t m_idStack{0}; // Z++: ℤ
    int64_t m_currentMemID{0}; // Z++: ℕ
    uint8_t m_memUsed_card{0u}; // Z++: ℙ ℕ
    int64_t m_totalBlocksUsed{0}; // Z++: ℕ
    uint8_t m_blocksUsed_card{0u}; // Z++: ℙ ℕ
    int64_t m_unkMemId{0}; // Z++: ℕ
    int32_t NUM_FIXED_MEMBLOCKS{0}; // Z++: i
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_fixedSize_len == 6u && m_fixedSize.size() == 6u)
            && (m_start_card <= 1u)
            && (m_end_card <= 1u)
            && (m_totalMemUsed >= 0)
            && (m_currentMemID >= 0)
            && (m_memUsed_card <= 1u)
            && (m_totalBlocksUsed >= 0)
            && (m_blocksUsed_card <= 1u)
            && (m_unkMemId >= 0);
    }
};
// State Schema Contract from zpp/rw/MemoryMgr.zpp
struct MemoryMgrModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{142}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 142);
    }
};
// State Schema Contract from zpp/rw/NodeName.zpp
struct NodeNameModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{81}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 81);
    }
};
// State Schema Contract from zpp/rw/RwHelper.zpp
struct RwHelperModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1035}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1035);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXNothing_ZppContract {
    std::array<int32_t, 5> pad{}; // Z++: seq ℤ  -- bounded array [5]
    size_t pad_len{5u}; // Z++ cardinality #pad
    int32_t effect{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (pad_len == 5u && pad.size() == 5u);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXBump_ZppContract {
    uint8_t bumpFrame_card{0u}; // Z++: Ref[RwFrame]
    uint8_t bumpedTex_card{0u}; // Z++: Ref[RwTexture]
    uint8_t bumpTex_card{0u}; // Z++: Ref[RwTexture]
    float negBumpCoefficient{0.0f}; // Z++: ℝ
    int32_t pad{0}; // Z++: ℤ
    int32_t effect{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (bumpFrame_card <= 1u)
            && (bumpedTex_card <= 1u)
            && (bumpTex_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXEnv_ZppContract {
    uint8_t envFrame_card{0u}; // Z++: Ref[RwFrame]
    uint8_t envTex_card{0u}; // Z++: Ref[RwTexture]
    float envCoeff{0.0f}; // Z++: ℝ
    int32_t envFBalpha{0}; // Z++: ℤ
    int32_t pad{0}; // Z++: ℤ
    int32_t effect{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (envFrame_card <= 1u)
            && (envTex_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXDual_ZppContract {
    uint8_t dualTex_card{0u}; // Z++: Ref[RwTexture]
    int32_t srcBlend{0}; // Z++: ℤ
    int32_t dstBlend{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (dualTex_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFX_ZppContract {
    int32_t b{0}; // Z++: MatFXBump
    int32_t e{0}; // Z++: MatFXEnv
    int32_t d{0}; // Z++: MatFXDual
    int32_t effects{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/rw/RwPS2AlphaTest.zpp
struct RwPS2AlphaTestModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{247}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 247);
    }
};
// State Schema Contract from zpp/rw/TexRead.zpp
struct GPUcaps_ZppContract {
    int64_t version{0}; // Z++: ℕ
    int64_t platform{0}; // Z++: ℕ
    int64_t subplatform{0}; // Z++: ℕ
    int64_t dxtSupport{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (version >= 0)
            && (platform >= 0)
            && (subplatform >= 0)
            && (dxtSupport >= 0);
    }
};
// State Schema Contract from zpp/rw/TexturePools.zpp
struct CTexturePool_ZppContract {
    int32_t Format{0}; // Z++: D3DFORMAT
    int32_t size{0}; // Z++: ℤ
    int64_t levels{0}; // Z++: ℕ
    int32_t texturesMax{0}; // Z++: ℤ
    int32_t texturesUsed{0}; // Z++: ℤ
    int32_t texturesNum{0}; // Z++: ℤ
    uint8_t pTextures_card{0u}; // Z++: Ref[IDirect3DTexture8]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (levels >= 0)
            && (pTextures_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/TexturePools.zpp
struct CPaletteList_ZppContract {
    int32_t Max{0}; // Z++: ℤ
    int32_t Num{0}; // Z++: ℤ
    uint8_t Data_card{0u}; // Z++: ℙ ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (Data_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/TxdStore.zpp
struct CTxdStore_ZppContract {
    uint8_t ms_pTxdPool_card{0u}; // Z++: Ref[CPool_TxdDef_TxdDef]
    uint8_t ms_pStoredTxd_card{0u}; // Z++: Ref[RwTexDictionary]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_pTxdPool_card <= 1u)
            && (ms_pStoredTxd_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/VisibilityPlugins.zpp
struct CVisibilityPlugins_ZppContract {
    uint8_t atomic_card{0u}; // Z++: Ref[RpAtomic]
    float sort{0.0f}; // Z++: ℝ
    int32_t m_alphaList{0}; // Z++: CLinkList_AlphaObjectInfo
    int32_t m_alphaEntityList{0}; // Z++: CLinkList_AlphaObjectInfo
    uint8_t ms_pCameraPosn_card{0u}; // Z++: Ref[RwV3d]
    float ms_cullCompsDist{0.0f}; // Z++: ℝ
    float ms_vehicleLod0Dist{0.0f}; // Z++: ℝ
    float ms_vehicleLod1Dist{0.0f}; // Z++: ℝ
    float ms_vehicleFadeDist{0.0f}; // Z++: ℝ
    float ms_bigVehicleLod0Dist{0.0f}; // Z++: ℝ
    float ms_bigVehicleLod1Dist{0.0f}; // Z++: ℝ
    float ms_pedLodDist{0.0f}; // Z++: ℝ
    float ms_pedFadeDist{0.0f}; // Z++: ℝ
    int64_t flags{0}; // Z++: ℕ
    int32_t ms_atomicPluginOffset{0}; // Z++: ℤ
    int32_t ms_framePluginOffset{0}; // Z++: ℤ
    int32_t alpha{0}; // Z++: ℤ
    int32_t ms_clumpPluginOffset{0}; // Z++: ℤ
    int32_t m_alphaBoatAtomicList{0}; // Z++: CLinkList_AlphaObjectInfo
    int32_t m_alphaUnderwaterEntityList{0}; // Z++: CLinkList_AlphaObjectInfo
    float ms_pedLod1Dist{0.0f}; // Z++: ℝ
    uint8_t modelInfo_card{0u}; // Z++: Ref[union_AtomicExt_CSimpleModelInfo]
    float ms_pedLod0Dist{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (atomic_card <= 1u)
            && (ms_pCameraPosn_card <= 1u)
            && (flags >= 0)
            && (modelInfo_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/VisibilityPlugins.zpp
struct AlphaObjectInfo_ZppContract {
    uint8_t atomic_card{0u}; // Z++: Ref[RpAtomic]
    float sort{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (atomic_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/VisibilityPlugins.zpp
struct AtomicExt_ZppContract {
    int32_t modelId{0}; // Z++: ℤ
    int64_t flags{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (flags >= 0);
    }
};
// State Schema Contract from zpp/rw/VisibilityPlugins.zpp
struct FrameExt_ZppContract {
    int32_t id{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/rw/VisibilityPlugins.zpp
struct ClumpExt_ZppContract {
    int32_t visibilityCB{0}; // Z++: ClumpVisibilityCB
    int32_t alpha{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Delta Operation Schema Contract (ΔCMemoryHeap) from zpp/rw/MemoryHeap.zpp
constexpr bool op_CMemoryHeap_PushMemId(const CMemoryHeap_ZppContract& pre, const CMemoryHeap_ZppContract& post, int64_t id_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_currentMemID == (int64_t)id_in);
}
constexpr CMemoryHeap_ZppContract apply_CMemoryHeap_PushMemId(const CMemoryHeap_ZppContract& pre, int64_t id_in = 25) {
    CMemoryHeap_ZppContract post = pre;
    post.m_currentMemID = (int64_t)id_in;
    return post;
}
// Delta Operation Schema Contract (ΔCommonSize) from zpp/rw/MemoryHeap.zpp
constexpr bool op_CommonSize_Init(const CommonSize_ZppContract& pre, const CommonSize_ZppContract& post, int64_t size_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_size == (int64_t)size_in) && (post.m_failed == 0) && (post.m_remaining == 0);
}
constexpr CommonSize_ZppContract apply_CommonSize_Init(const CommonSize_ZppContract& pre, int64_t size_in = 25) {
    CommonSize_ZppContract post = pre;
    post.m_size = (int64_t)size_in;
    post.m_failed = 0;
    post.m_remaining = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCTexturePool) from zpp/rw/TexturePools.zpp
constexpr bool op_CTexturePool_Release(const CTexturePool_ZppContract& pre, const CTexturePool_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pTextures_card == 0u) && (post.texturesNum == 0) && (post.texturesUsed == 0);
}
constexpr CTexturePool_ZppContract apply_CTexturePool_Release(const CTexturePool_ZppContract& pre) {
    CTexturePool_ZppContract post = pre;
    post.pTextures_card = 0u;
    post.texturesNum = 0;
    post.texturesUsed = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCPaletteList) from zpp/rw/TexturePools.zpp
constexpr bool op_CPaletteList_Free(const CPaletteList_ZppContract& pre, const CPaletteList_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Data_card == 0u) && (post.Num == 0);
}
constexpr CPaletteList_ZppContract apply_CPaletteList_Free(const CPaletteList_ZppContract& pre) {
    CPaletteList_ZppContract post = pre;
    post.Data_card = 0u;
    post.Num = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCTxdStore) from zpp/rw/TxdStore.zpp
constexpr bool op_CTxdStore_PopCurrentTxd(const CTxdStore_ZppContract& pre, const CTxdStore_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_pStoredTxd_card == 0u);
}
constexpr CTxdStore_ZppContract apply_CTxdStore_PopCurrentTxd(const CTxdStore_ZppContract& pre) {
    CTxdStore_ZppContract post = pre;
    post.ms_pStoredTxd_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCVisibilityPlugins) from zpp/rw/VisibilityPlugins.zpp
constexpr bool op_CVisibilityPlugins_SetAtomicId(const CVisibilityPlugins_ZppContract& pre, const CVisibilityPlugins_ZppContract& post, int64_t id_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.flags == (int64_t)id_in);
}
constexpr CVisibilityPlugins_ZppContract apply_CVisibilityPlugins_SetAtomicId(const CVisibilityPlugins_ZppContract& pre, int64_t id_in = 25) {
    CVisibilityPlugins_ZppContract post = pre;
    post.flags = (int64_t)id_in;
    return post;
}
// Delta Operation Schema Contract (ΔCVisibilityPlugins) from zpp/rw/VisibilityPlugins.zpp
constexpr bool op_CVisibilityPlugins_SetClumpAlpha(const CVisibilityPlugins_ZppContract& pre, const CVisibilityPlugins_ZppContract& post, int32_t alpha_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.alpha == (int32_t)alpha_in);
}
constexpr CVisibilityPlugins_ZppContract apply_CVisibilityPlugins_SetClumpAlpha(const CVisibilityPlugins_ZppContract& pre, int32_t alpha_in = 25) {
    CVisibilityPlugins_ZppContract post = pre;
    post.alpha = (int32_t)alpha_in;
    return post;
}
// Delta Operation Schema Contract (ΔCVisibilityPlugins) from zpp/rw/VisibilityPlugins.zpp
constexpr bool op_CVisibilityPlugins_GetClumpDistanceAlpha(const CVisibilityPlugins_ZppContract& pre, const CVisibilityPlugins_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.alpha == 255);
}
constexpr CVisibilityPlugins_ZppContract apply_CVisibilityPlugins_GetClumpDistanceAlpha(const CVisibilityPlugins_ZppContract& pre) {
    CVisibilityPlugins_ZppContract post = pre;
    post.alpha = 255;
    return post;
}
constexpr bool verify_rw_zpp_invariants() {
    {
        rpGeometryList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rpGeometryList_ZppContract bad{};
        bad.geometries_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        rpAtomicBinary_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rpAtomicBinary_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        LightsModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LightsModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        HeapBlockDesc_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        HeapBlockDesc_ZppContract bad{};
        bad.m_size = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        HeapBlockList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        HeapBlockList_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CommonSize_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CommonSize_ZppContract bad{};
        bad.m_size = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMemoryHeap_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMemoryHeap_ZppContract bad{};
        bad.m_fixedSize_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        MemoryMgrModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MemoryMgrModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        NodeNameModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        NodeNameModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RwHelperModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwHelperModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        MatFXNothing_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MatFXNothing_ZppContract bad{};
        bad.pad_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        MatFXBump_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MatFXBump_ZppContract bad{};
        bad.bumpFrame_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        MatFXEnv_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MatFXEnv_ZppContract bad{};
        bad.envFrame_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        MatFXDual_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MatFXDual_ZppContract bad{};
        bad.dualTex_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        MatFX_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MatFX_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RwPS2AlphaTestModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwPS2AlphaTestModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        GPUcaps_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GPUcaps_ZppContract bad{};
        bad.version = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTexturePool_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTexturePool_ZppContract bad{};
        bad.levels = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPaletteList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPaletteList_ZppContract bad{};
        bad.Data_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTxdStore_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTxdStore_ZppContract bad{};
        bad.ms_pTxdPool_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVisibilityPlugins_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVisibilityPlugins_ZppContract bad{};
        bad.atomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        AlphaObjectInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AlphaObjectInfo_ZppContract bad{};
        bad.atomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        AtomicExt_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AtomicExt_ZppContract bad{};
        bad.flags = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        FrameExt_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        FrameExt_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        ClumpExt_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ClumpExt_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMemoryHeap_ZppContract pre{};
        const int64_t id_in = 25;
        pre.m_currentMemID = 0;
        const auto post = apply_CMemoryHeap_PushMemId(pre, id_in);
        if (!op_CMemoryHeap_PushMemId(pre, post, id_in)) return false;
        {
            CMemoryHeap_ZppContract badPost = post;
            badPost.m_currentMemID = (int64_t)(id_in + 1);
            if (op_CMemoryHeap_PushMemId(pre, badPost, id_in)) return false;
        }
    }
    {
        CommonSize_ZppContract pre{};
        const int64_t size_in = 25;
        pre.m_size = 0;
        pre.m_failed = 1;
        pre.m_remaining = 1;
        const auto post = apply_CommonSize_Init(pre, size_in);
        if (!op_CommonSize_Init(pre, post, size_in)) return false;
        {
            CommonSize_ZppContract badPost = post;
            badPost.m_size = (int64_t)(size_in + 1);
            if (op_CommonSize_Init(pre, badPost, size_in)) return false;
        }
        {
            CommonSize_ZppContract badPost = post;
            badPost.m_failed = 1;
            if (op_CommonSize_Init(pre, badPost, size_in)) return false;
        }
        {
            CommonSize_ZppContract badPost = post;
            badPost.m_remaining = 1;
            if (op_CommonSize_Init(pre, badPost, size_in)) return false;
        }
    }
    {
        CTexturePool_ZppContract pre{};
        pre.pTextures_card = 1u;
        pre.texturesNum = 1;
        pre.texturesUsed = 1;
        const auto post = apply_CTexturePool_Release(pre);
        if (!op_CTexturePool_Release(pre, post)) return false;
        {
            CTexturePool_ZppContract badPost = post;
            badPost.pTextures_card = 1u;
            if (op_CTexturePool_Release(pre, badPost)) return false;
        }
        {
            CTexturePool_ZppContract badPost = post;
            badPost.texturesNum = 1;
            if (op_CTexturePool_Release(pre, badPost)) return false;
        }
        {
            CTexturePool_ZppContract badPost = post;
            badPost.texturesUsed = 1;
            if (op_CTexturePool_Release(pre, badPost)) return false;
        }
    }
    {
        CPaletteList_ZppContract pre{};
        pre.Data_card = 1u;
        pre.Num = 1;
        const auto post = apply_CPaletteList_Free(pre);
        if (!op_CPaletteList_Free(pre, post)) return false;
        {
            CPaletteList_ZppContract badPost = post;
            badPost.Data_card = 1u;
            if (op_CPaletteList_Free(pre, badPost)) return false;
        }
        {
            CPaletteList_ZppContract badPost = post;
            badPost.Num = 1;
            if (op_CPaletteList_Free(pre, badPost)) return false;
        }
    }
    {
        CTxdStore_ZppContract pre{};
        pre.ms_pStoredTxd_card = 1u;
        const auto post = apply_CTxdStore_PopCurrentTxd(pre);
        if (!op_CTxdStore_PopCurrentTxd(pre, post)) return false;
        {
            CTxdStore_ZppContract badPost = post;
            badPost.ms_pStoredTxd_card = 1u;
            if (op_CTxdStore_PopCurrentTxd(pre, badPost)) return false;
        }
    }
    {
        CVisibilityPlugins_ZppContract pre{};
        const int64_t id_in = 25;
        pre.flags = 0;
        const auto post = apply_CVisibilityPlugins_SetAtomicId(pre, id_in);
        if (!op_CVisibilityPlugins_SetAtomicId(pre, post, id_in)) return false;
        {
            CVisibilityPlugins_ZppContract badPost = post;
            badPost.flags = (int64_t)(id_in + 1);
            if (op_CVisibilityPlugins_SetAtomicId(pre, badPost, id_in)) return false;
        }
    }
    {
        CVisibilityPlugins_ZppContract pre{};
        const int32_t alpha_in = 25;
        const auto post = apply_CVisibilityPlugins_SetClumpAlpha(pre, alpha_in);
        if (!op_CVisibilityPlugins_SetClumpAlpha(pre, post, alpha_in)) return false;
        {
            CVisibilityPlugins_ZppContract badPost = post;
            badPost.alpha = (int32_t)(alpha_in + 1);
            if (op_CVisibilityPlugins_SetClumpAlpha(pre, badPost, alpha_in)) return false;
        }
    }
    {
        CVisibilityPlugins_ZppContract pre{};
        pre.alpha = 0;
        const auto post = apply_CVisibilityPlugins_GetClumpDistanceAlpha(pre);
        if (!op_CVisibilityPlugins_GetClumpDistanceAlpha(pre, post)) return false;
        {
            CVisibilityPlugins_ZppContract badPost = post;
            badPost.alpha = 0;
            if (op_CVisibilityPlugins_GetClumpDistanceAlpha(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_rw_zpp_invariants(), "Z++ formal contract invariant failed for rw");
} // namespace zpp_rw_spec
#endif // GTANGEL_ZPP_RW_SPEC_GUARD
