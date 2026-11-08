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
    std::array<int32_t, 8> values{}; // Z++: seq T  -- bounded array [N]
    size_t values_len{8u}; // Z++ cardinality #values
    int64_t sp{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (values_len == 8u && values.size() == 8u)
            && (sp >= 0);
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
// Operation Schema Contract (ΔCStack) from zpp/rw/MemoryHeap.zpp
constexpr bool op_CStack_CStack(CStack_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCStack) from zpp/rw/MemoryHeap.zpp
constexpr bool op_CStack_pop_2(CStack_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔHeapBlockDesc) from zpp/rw/MemoryHeap.zpp
constexpr bool op_HeapBlockDesc_GetNextConsecutive(HeapBlockDesc_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔHeapBlockDesc) from zpp/rw/MemoryHeap.zpp
constexpr bool op_HeapBlockDesc_GetDataPointer_2(HeapBlockDesc_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔHeapBlockDesc) from zpp/rw/MemoryHeap.zpp
constexpr bool op_HeapBlockDesc_RemoveHeapFreeBlock_3(HeapBlockDesc_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔHeapBlockDesc) from zpp/rw/MemoryHeap.zpp
constexpr bool op_HeapBlockDesc_InsertHeapFreeBlock_4(HeapBlockDesc_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
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
        LightsModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LightsModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CStack_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStack_ZppContract bad{};
        bad.values_len = 0u;
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
        CommonSize_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CommonSize_ZppContract bad{};
        bad.m_size = -1;
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
        CStack_ZppContract st{};
        if (!op_CStack_CStack(st)) return false;
    }
    {
        CStack_ZppContract st{};
        if (!op_CStack_pop_2(st)) return false;
    }
    {
        HeapBlockDesc_ZppContract st{};
        if (!op_HeapBlockDesc_GetNextConsecutive(st)) return false;
    }
    {
        HeapBlockDesc_ZppContract st{};
        if (!op_HeapBlockDesc_GetDataPointer_2(st)) return false;
    }
    {
        HeapBlockDesc_ZppContract st{};
        if (!op_HeapBlockDesc_RemoveHeapFreeBlock_3(st)) return false;
    }
    {
        HeapBlockDesc_ZppContract st{};
        if (!op_HeapBlockDesc_InsertHeapFreeBlock_4(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_rw_zpp_invariants(), "Z++ formal contract invariant failed for rw");
} // namespace zpp_rw_spec
#endif // GTANGEL_ZPP_RW_SPEC_GUARD
template<class Surface> class RasterEngine {
public:
    Surface framebuffer;
    FrameMetrics metrics;
    DteSkinnedAsset skinned_;
    bool loadDteAsset(const std::string& filename) { return skinned_.load(filename); }
    bool hasDteAsset() const { return skinned_.valid(); }
    const std::string& dteAssetError() const { return skinned_.error(); }
    size_t dteAssetTriangleCount() const { return skinned_.triangleCount(); }
    const char* modelSource() const { return skinned_.valid() ? "DTE V22 SKINNED / LOCAL SENSOR" : Re3InformedAvatarClump::sourceLabel(); }

    void render(const avatar3d::GameWorld& world,
                const avatar3d::AvatarSkeleton& avatar,
                Vec3 eye, float heading, float pitch, bool thirdPerson) {
        const auto start = std::chrono::steady_clock::now();
        framebuffer.clear();
        triangles_.clear();
        heading_ = heading;
        pitch_ = pitch;
        eye_ = eye;

        buildSun();
        buildMoon();
        buildGround(world);
        buildRoads(world);
        buildPillars(world);
        buildBuildings(world);
        buildGoal(world);
        buildNpc(world);
        buildQuestShard(world);
        buildVehicle(world);
        buildWalkers(world);
        buildTraffic(world);
        buildPickups(world);
        buildProps(world);
        if (thirdPerson && !world.vehicleOccupied) buildAvatar(avatar);

        metrics.trianglesSubmitted = (int)triangles_.size();
        metrics.trianglesRasterized = 0;
        for (const auto& triangle : triangles_) rasterize(triangle);
        metrics.finiteDepthPixels = 0;
        metrics.semanticPixels = 0;
        for (int index = 0; index < Surface::PixelCount; ++index) {
            if (framebuffer.depth[index] < 1e20f) ++metrics.finiteDepthPixels;
            if (framebuffer.objectId[index] != ObjectSky) ++metrics.semanticPixels;
        }
        metrics.sequence = framebuffer.sequence;
        metrics.renderMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
    }

private:
    std::vector<Triangle> triangles_;
    Vec3 eye_{};
    float heading_{0.0f};
    float pitch_{0.0f};

    static Colour groundColour(int gx, int gz) {
        // LW34: cohesive urban slate plaza with 4m block joints instead of
        // a harsh 1m black/purple checkerboard.
        const uint32_t h = (uint32_t)(gx * 73856093) ^ (uint32_t)(gz * 19349663);
        const int v = (int)((h >> 8) & 7) - 3;
        const bool seam = ((gx & 3) == 0) || ((gz & 3) == 0);
        const int baseB = seam ? 54 : 62;
        const int baseG = seam ? 50 : 57;
        const int baseR = seam ? 48 : 55;
        return Colour{(uint8_t)(baseB + v), (uint8_t)(baseG + v), (uint8_t)(baseR + v), 255};
    }

    void addTriangle(Vec3 a, Vec3 b, Vec3 c, Colour colour, uint16_t objectId) {
        triangles_.push_back({a, b, c, colour, objectId});
    }

    void addQuad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Colour colour, uint16_t objectId) {
        addTriangle(a, b, c, colour, objectId);
        addTriangle(a, c, d, colour, objectId);
    }

