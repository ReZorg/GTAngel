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
// State Schema Contract from zpp/rw/ClumpRead.zpp
struct rpAtomicBinary_ZppContract {
    int32_t frameIndex{0}; // Z++: ℤ
    int32_t geomIndex{0}; // Z++: ℤ
    int32_t flags{0}; // Z++: ℤ
    int32_t unused{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
struct HeapBlockList_ZppContract {
    int32_t m_first{0}; // Z++: HeapBlockDesc
    int32_t m_last{0}; // Z++: HeapBlockDesc
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
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
// State Schema Contract from zpp/rw/NodeName.zpp
struct NodeNameModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{81}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 81);
    }
};
// State Schema Contract from zpp/rw/RwHelper.zpp
struct RwHelperModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1035}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1035);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXNothing_ZppContract {
    std::array<int32_t, 5> pad{}; // Z++: seq ℤ  -- bounded array [5]
    size_t pad_len{5u}; // Z++ cardinality #pad
    int32_t effect{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (envFrame_card <= 1u)
            && (envTex_card <= 1u);
    }
};
// State Schema Contract from zpp/rw/RwMatFX.zpp
struct MatFXDual_ZppContract {
    uint8_t dualTex_card{0u}; // Z++: Ref[RwTexture]
    int32_t srcBlend{0}; // Z++: ℤ
    int32_t dstBlend{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/rw/RwPS2AlphaTest.zpp
struct RwPS2AlphaTestModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{247}; // Z++: ℕ
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (version >= 0)
            && (platform >= 0)
            && (subplatform >= 0)
            && (dxtSupport >= 0);
    }
};
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

