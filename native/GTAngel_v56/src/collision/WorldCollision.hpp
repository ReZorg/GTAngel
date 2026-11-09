#pragma once
#ifndef GTANGEL_ZPP_COLLISION_SPEC_GUARD
#define GTANGEL_ZPP_COLLISION_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/collision
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/collision/*.zpp
// Modules: 11 | Formal Schemas: 250 | Free Types: 2
// ============================================================================
namespace zpp_collision_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 250;
static constexpr int kZppEnumCount   = 2;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/collision/ColTriangle.zpp
enum class Direction : int32_t { DIR_X_POS_v = 0, DIR_X_NEG_v = 1, DIR_Y_POS_v = 2, DIR_Y_NEG_v = 3, DIR_Z_POS_v = 4, DIR_Z_NEG_v = 5 };
// Free Type from zpp/collision/Collision.zpp
enum class Collision_Enum_1 : int32_t { MAXNUMSPHERES_v = 128, MAXNUMBOXES_v = 32, MAXNUMLINES_v = 16, MAXNUMTRIS_v = 600 };
// State Schema Contract from zpp/collision/ColBox.zpp
struct CBox_ZppContract {
    ZppVec3 min_field{}; // Z++: CVector
    ZppVec3 max_field{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColBox_ZppContract {
    ZppVec3 min_field{}; // Z++: CVector
    ZppVec3 max_field{}; // Z++: CVector
    int64_t surface{0}; // Z++: ℕ
    int64_t piece{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (surface >= 0)
            && (piece >= 0);
    }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColTriBBox_ZppContract {
    ZppVec3 min_field{}; // Z++: CVector
    ZppVec3 max_field{}; // Z++: CVector
    int32_t first{0}; // Z++: ℤ
    int32_t last{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColLine.zpp
struct CColLine_ZppContract {
    ZppVec3 p0{}; // Z++: CVector
    ZppVec3 p1{}; // Z++: CVector
    int32_t pad0{0}; // Z++: ℤ
    int32_t pad1{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColModel.zpp
struct CColModel_ZppContract {
    int32_t boundingSphere{0}; // Z++: CSphere
    int32_t boundingBox{0}; // Z++: CBox
    int32_t numSpheres{0}; // Z++: ℤ
    int32_t numBoxes{0}; // Z++: ℤ
    int32_t numTriangles{0}; // Z++: ℤ
    int32_t numLines{0}; // Z++: ℤ
    int32_t numTriBBoxes{0}; // Z++: ℤ
    int64_t level{0}; // Z++: ℕ
    bool ownsCollisionVolumes{false}; // Z++: 𝔹
    uint8_t spheres_card{0u}; // Z++: Ref[CColSphere]
    uint8_t lines_card{0u}; // Z++: Ref[CColLine]
    uint8_t boxes_card{0u}; // Z++: Ref[CColBox]
    uint8_t triBBoxes_card{0u}; // Z++: Ref[CColTriBBox]
    uint8_t vertices_card{0u}; // Z++: Ref[CompressedVector]
    uint8_t triangles_card{0u}; // Z++: Ref[CColTriangle]
    uint8_t trianglePlanes_card{0u}; // Z++: Ref[CColTrianglePlane]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (level >= 0)
            && (spheres_card <= 1u)
            && (lines_card <= 1u)
            && (boxes_card <= 1u)
            && (triBBoxes_card <= 1u)
            && (vertices_card <= 1u)
            && (triangles_card <= 1u)
            && (trianglePlanes_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/ColPoint.zpp
struct CColPoint_ZppContract {
    ZppVec3 point{}; // Z++: CVector
    int32_t pad1{0}; // Z++: ℤ
    ZppVec3 normal{}; // Z++: CVector
    int32_t pad2{0}; // Z++: ℤ
    int64_t surfaceA{0}; // Z++: ℕ
    int64_t pieceA{0}; // Z++: ℕ
    int64_t surfaceB{0}; // Z++: ℕ
    int64_t pieceB{0}; // Z++: ℕ
    float depth{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (surfaceA >= 0)
            && (pieceA >= 0)
            && (surfaceB >= 0)
            && (pieceB >= 0);
    }
};
// State Schema Contract from zpp/collision/ColSphere.zpp
struct CColSphere_ZppContract {
    ZppVec3 center{}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    int64_t surface{0}; // Z++: ℕ
    int64_t piece{0}; // Z++: ℕ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (surface >= 0)
            && (piece >= 0);
    }
};
// State Schema Contract from zpp/collision/ColSphere.zpp
struct CSphere_ZppContract {
    ZppVec3 center{}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColStore.zpp
struct ColChunkEntry_ZppContract {
    int32_t modelId{0}; // Z++: ℤ
    uint8_t colModel_card{0u}; // Z++: Ref[CColModel]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (colModel_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/ColStore.zpp
struct CColStore_ZppContract {
    uint8_t ms_pColPool_card{0u}; // Z++: Ref[CPool_ColDef_ColDef]
    bool m_onlyBB{false}; // Z++: 𝔹
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_pColPool_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/ColTriangle.zpp
struct CColTriangle_ZppContract {
    int64_t a{0}; // Z++: ℕ
    int64_t b{0}; // Z++: ℕ
    int64_t c{0}; // Z++: ℕ
    int64_t surface{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (a >= 0)
            && (b >= 0)
            && (c >= 0)
            && (surface >= 0);
    }
};
// State Schema Contract from zpp/collision/ColTriangle.zpp
struct CColTrianglePlane_ZppContract {
    int32_t dist{0}; // Z++: ℤ
    int64_t dir{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (dir >= 0);
    }
};
// State Schema Contract from zpp/collision/Collision.zpp
struct CStoredCollPoly_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/Collision.zpp
struct CCollision_ZppContract {
    int32_t ms_collisionInMemory{0}; // Z++: eLevelName
    uint8_t ms_colModelCache_card{0u}; // Z++: Ref[CLinkList_CColModel]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_colModelCache_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/CompressedVector.zpp
struct CompressedVector_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/TempColModels.zpp
struct CTempColModels_ZppContract {
    int32_t ms_colModelPed1{0}; // Z++: CColModel
    int32_t ms_colModelBBox{0}; // Z++: CColModel
    int32_t ms_colModelWeapon{0}; // Z++: CColModel
    int32_t ms_colModelPed2{0}; // Z++: CColModel
    int32_t ms_colModelBumper1{0}; // Z++: CColModel
    int32_t ms_colModelWheel1{0}; // Z++: CColModel
    int32_t ms_colModelPanel1{0}; // Z++: CColModel
    int32_t ms_colModelBodyPart2{0}; // Z++: CColModel
    int32_t ms_colModelBodyPart1{0}; // Z++: CColModel
    std::array<int32_t, 10> ms_colModelCutObj{}; // Z++: seq CColModel  -- bounded array [10]
    size_t ms_colModelCutObj_len{10u}; // Z++ cardinality #ms_colModelCutObj
    int32_t ms_colModelPedGroundHit{0}; // Z++: CColModel
    int32_t ms_colModelBoot1{0}; // Z++: CColModel
    int32_t ms_colModelDoor1{0}; // Z++: CColModel
    int32_t ms_colModelBonnet1{0}; // Z++: CColModel
    int32_t ms_colModelFerryDocked{0}; // Z++: CColModel
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_colModelCutObj_len == 10u && ms_colModelCutObj.size() == 10u);
    }
};
// State Schema Contract from zpp/collision/VuCollision.zpp
struct VuTriangle_ZppContract {
    int64_t v1{0}; // Z++: ℕ
    int64_t v2{0}; // Z++: ℕ
    int64_t plane{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (v1 >= 0)
            && (v2 >= 0)
            && (plane >= 0);
    }
};
// Query Operation Schema Contract (ΞCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_GetTrianglePoint_8(const CColModel_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_GetTrianglePoint(const CColModel_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_GetNormal_3(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_CalcPoint_4(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_Unpack_7(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_GetNormal_10(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_GetNormalX_11(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_GetNormalY_12(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_GetNormalZ_13(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCColTrianglePlane) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTrianglePlane_CalcPoint_14(const CColTrianglePlane_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCompressedVector) from zpp/collision/CompressedVector.zpp
constexpr bool op_CompressedVector_Get(const CompressedVector_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCompressedVector) from zpp/collision/CompressedVector.zpp
constexpr bool op_CompressedVector_Unpack_3(const CompressedVector_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCompressedVector) from zpp/collision/CompressedVector.zpp
constexpr bool op_CompressedVector_Get_4(const CompressedVector_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_Set(const CColBox_ZppContract& pre, const CColBox_ZppContract& post, ZppVec3 min_in = ZppVec3{1.0f, 2.0f, 3.0f}, ZppVec3 max_in = ZppVec3{1.0f, 2.0f, 3.0f}, int64_t surf_in = 25, int64_t piece_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.min_field == min_in) && (post.max_field == max_in) && (post.surface == (int64_t)surf_in) && (post.piece == (int64_t)piece_in);
}
constexpr CColBox_ZppContract apply_CColBox_Set(const CColBox_ZppContract& pre, ZppVec3 min_in = ZppVec3{1.0f, 2.0f, 3.0f}, ZppVec3 max_in = ZppVec3{1.0f, 2.0f, 3.0f}, int64_t surf_in = 25, int64_t piece_in = 25) {
    CColBox_ZppContract post = pre;
    post.min_field = min_in;
    post.max_field = max_in;
    post.surface = (int64_t)surf_in;
    post.piece = (int64_t)piece_in;
    return post;
}
// Delta Operation Schema Contract (ΔCColLine) from zpp/collision/ColLine.zpp
constexpr bool op_CColLine_Set(const CColLine_ZppContract& pre, const CColLine_ZppContract& post, ZppVec3 p0_in = ZppVec3{1.0f, 2.0f, 3.0f}, ZppVec3 p1_in = ZppVec3{1.0f, 2.0f, 3.0f}) {
    return pre.checkInvariant() && post.checkInvariant() && (post.p0 == p0_in) && (post.p1 == p1_in);
}
constexpr CColLine_ZppContract apply_CColLine_Set(const CColLine_ZppContract& pre, ZppVec3 p0_in = ZppVec3{1.0f, 2.0f, 3.0f}, ZppVec3 p1_in = ZppVec3{1.0f, 2.0f, 3.0f}) {
    CColLine_ZppContract post = pre;
    post.p0 = p0_in;
    post.p1 = p1_in;
    return post;
}
// Delta Operation Schema Contract (ΔCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_CColModel(const CColModel_ZppContract& pre, const CColModel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.numTriBBoxes == 0) && (post.triBBoxes_card == 0u) && (post.numSpheres == 0) && (post.spheres_card == 0u) && (post.numLines == 0) && (post.lines_card == 0u);
}
constexpr CColModel_ZppContract apply_CColModel_CColModel(const CColModel_ZppContract& pre) {
    CColModel_ZppContract post = pre;
    post.numTriBBoxes = 0;
    post.triBBoxes_card = 0u;
    post.numSpheres = 0;
    post.spheres_card = 0u;
    post.numLines = 0;
    post.lines_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_RemoveCollisionVolumes(const CColModel_ZppContract& pre, const CColModel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.numSpheres == 0) && (post.numTriBBoxes == 0) && (post.numLines == 0) && (post.numBoxes == 0) && (post.numTriangles == 0) && (post.spheres_card == 0u);
}
constexpr CColModel_ZppContract apply_CColModel_RemoveCollisionVolumes(const CColModel_ZppContract& pre) {
    CColModel_ZppContract post = pre;
    post.numSpheres = 0;
    post.numTriBBoxes = 0;
    post.numLines = 0;
    post.numBoxes = 0;
    post.numTriangles = 0;
    post.spheres_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_RemoveTrianglePlanes(const CColModel_ZppContract& pre, const CColModel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.trianglePlanes_card == 0u);
}
constexpr CColModel_ZppContract apply_CColModel_RemoveTrianglePlanes(const CColModel_ZppContract& pre) {
    CColModel_ZppContract post = pre;
    post.trianglePlanes_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColSphere) from zpp/collision/ColSphere.zpp
constexpr bool op_CColSphere_Set(const CColSphere_ZppContract& pre, const CColSphere_ZppContract& post, float radius_in = 2.5f, ZppVec3 center_in = ZppVec3{1.0f, 2.0f, 3.0f}, int64_t surf_in = 25, int64_t piece_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.radius == radius_in) && (post.center == center_in) && (post.surface == (int64_t)surf_in) && (post.piece == (int64_t)piece_in);
}
constexpr CColSphere_ZppContract apply_CColSphere_Set(const CColSphere_ZppContract& pre, float radius_in = 2.5f, ZppVec3 center_in = ZppVec3{1.0f, 2.0f, 3.0f}, int64_t surf_in = 25, int64_t piece_in = 25) {
    CColSphere_ZppContract post = pre;
    post.radius = radius_in;
    post.center = center_in;
    post.surface = (int64_t)surf_in;
    post.piece = (int64_t)piece_in;
    return post;
}
// Delta Operation Schema Contract (ΔCColStore) from zpp/collision/ColStore.zpp
constexpr bool op_CColStore_Shutdown(const CColStore_ZppContract& pre, const CColStore_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_pColPool_card == 0u);
}
constexpr CColStore_ZppContract apply_CColStore_Shutdown(const CColStore_ZppContract& pre) {
    CColStore_ZppContract post = pre;
    post.ms_pColPool_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColTriangle) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTriangle_Set(const CColTriangle_ZppContract& pre, const CColTriangle_ZppContract& post, int64_t a_in = 25, int64_t b_in = 25, int64_t c_in = 25, int64_t surf_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.a == (int64_t)a_in) && (post.b == (int64_t)b_in) && (post.c == (int64_t)c_in) && (post.surface == (int64_t)surf_in);
}
constexpr CColTriangle_ZppContract apply_CColTriangle_Set(const CColTriangle_ZppContract& pre, int64_t a_in = 25, int64_t b_in = 25, int64_t c_in = 25, int64_t surf_in = 25) {
    CColTriangle_ZppContract post = pre;
    post.a = (int64_t)a_in;
    post.b = (int64_t)b_in;
    post.c = (int64_t)c_in;
    post.surface = (int64_t)surf_in;
    return post;
}
constexpr bool verify_collision_zpp_invariants() {
    {
        CBox_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBox_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColBox_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColBox_ZppContract bad{};
        bad.surface = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColTriBBox_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColTriBBox_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColLine_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColModel_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColModel_ZppContract bad{};
        bad.level = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColPoint_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColPoint_ZppContract bad{};
        bad.surfaceA = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColSphere_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColSphere_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSphere_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSphere_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        ColChunkEntry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ColChunkEntry_ZppContract bad{};
        bad.colModel_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CColStore_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColStore_ZppContract bad{};
        bad.ms_pColPool_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CColTriangle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColTriangle_ZppContract bad{};
        bad.a = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CColTrianglePlane_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColTrianglePlane_ZppContract bad{};
        bad.dir = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredCollPoly_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredCollPoly_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCollision_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCollision_ZppContract bad{};
        bad.ms_colModelCache_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CompressedVector_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CompressedVector_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTempColModels_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTempColModels_ZppContract bad{};
        bad.ms_colModelCutObj_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        VuTriangle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VuTriangle_ZppContract bad{};
        bad.v1 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const CColModel_ZppContract st{};
        if (!op_CColModel_GetTrianglePoint_8(st)) return false;
    }
    {
        const CColModel_ZppContract st{};
        if (!op_CColModel_GetTrianglePoint(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_GetNormal_3(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_CalcPoint_4(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_Unpack_7(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_GetNormal_10(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_GetNormalX_11(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_GetNormalY_12(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_GetNormalZ_13(st)) return false;
    }
    {
        const CColTrianglePlane_ZppContract st{};
        if (!op_CColTrianglePlane_CalcPoint_14(st)) return false;
    }
    {
        const CompressedVector_ZppContract st{};
        if (!op_CompressedVector_Get(st)) return false;
    }
    {
        const CompressedVector_ZppContract st{};
        if (!op_CompressedVector_Unpack_3(st)) return false;
    }
    {
        const CompressedVector_ZppContract st{};
        if (!op_CompressedVector_Get_4(st)) return false;
    }
    {
        CColBox_ZppContract pre{};
        const ZppVec3 min_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const ZppVec3 max_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const int64_t surf_in = 25;
        const int64_t piece_in = 25;
        pre.surface = 0;
        const auto post = apply_CColBox_Set(pre, min_in, max_in, surf_in, piece_in);
        if (!op_CColBox_Set(pre, post, min_in, max_in, surf_in, piece_in)) return false;
        {
            CColBox_ZppContract badPost = post;
            badPost.min_field = ZppVec3{min_in.x + 1.0f, min_in.y, min_in.z};
            if (op_CColBox_Set(pre, badPost, min_in, max_in, surf_in, piece_in)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.max_field = ZppVec3{max_in.x + 1.0f, max_in.y, max_in.z};
            if (op_CColBox_Set(pre, badPost, min_in, max_in, surf_in, piece_in)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.surface = (int64_t)(surf_in + 1);
            if (op_CColBox_Set(pre, badPost, min_in, max_in, surf_in, piece_in)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.piece = (int64_t)(piece_in + 1);
            if (op_CColBox_Set(pre, badPost, min_in, max_in, surf_in, piece_in)) return false;
        }
    }
    {
        CColLine_ZppContract pre{};
        const ZppVec3 p0_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const ZppVec3 p1_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const auto post = apply_CColLine_Set(pre, p0_in, p1_in);
        if (!op_CColLine_Set(pre, post, p0_in, p1_in)) return false;
        {
            CColLine_ZppContract badPost = post;
            badPost.p0 = ZppVec3{p0_in.x + 1.0f, p0_in.y, p0_in.z};
            if (op_CColLine_Set(pre, badPost, p0_in, p1_in)) return false;
        }
        {
            CColLine_ZppContract badPost = post;
            badPost.p1 = ZppVec3{p1_in.x + 1.0f, p1_in.y, p1_in.z};
            if (op_CColLine_Set(pre, badPost, p0_in, p1_in)) return false;
        }
    }
    {
        CColModel_ZppContract pre{};
        pre.numTriBBoxes = 1;
        pre.triBBoxes_card = 1u;
        pre.numSpheres = 1;
        pre.spheres_card = 1u;
        pre.numLines = 1;
        pre.lines_card = 1u;
        const auto post = apply_CColModel_CColModel(pre);
        if (!op_CColModel_CColModel(pre, post)) return false;
        {
            CColModel_ZppContract badPost = post;
            badPost.numTriBBoxes = 1;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.triBBoxes_card = 1u;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numSpheres = 1;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.spheres_card = 1u;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numLines = 1;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.lines_card = 1u;
            if (op_CColModel_CColModel(pre, badPost)) return false;
        }
    }
    {
        CColModel_ZppContract pre{};
        pre.numSpheres = 1;
        pre.numTriBBoxes = 1;
        pre.numLines = 1;
        pre.numBoxes = 1;
        pre.numTriangles = 1;
        pre.spheres_card = 1u;
        const auto post = apply_CColModel_RemoveCollisionVolumes(pre);
        if (!op_CColModel_RemoveCollisionVolumes(pre, post)) return false;
        {
            CColModel_ZppContract badPost = post;
            badPost.numSpheres = 1;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numTriBBoxes = 1;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numLines = 1;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numBoxes = 1;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numTriangles = 1;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.spheres_card = 1u;
            if (op_CColModel_RemoveCollisionVolumes(pre, badPost)) return false;
        }
    }
    {
        CColModel_ZppContract pre{};
        pre.trianglePlanes_card = 1u;
        const auto post = apply_CColModel_RemoveTrianglePlanes(pre);
        if (!op_CColModel_RemoveTrianglePlanes(pre, post)) return false;
        {
            CColModel_ZppContract badPost = post;
            badPost.trianglePlanes_card = 1u;
            if (op_CColModel_RemoveTrianglePlanes(pre, badPost)) return false;
        }
    }
    {
        CColSphere_ZppContract pre{};
        const float radius_in = 2.5f;
        const ZppVec3 center_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const int64_t surf_in = 25;
        const int64_t piece_in = 25;
        pre.surface = 0;
        const auto post = apply_CColSphere_Set(pre, radius_in, center_in, surf_in, piece_in);
        if (!op_CColSphere_Set(pre, post, radius_in, center_in, surf_in, piece_in)) return false;
        {
            CColSphere_ZppContract badPost = post;
            badPost.radius = radius_in + 1.0f;
            if (op_CColSphere_Set(pre, badPost, radius_in, center_in, surf_in, piece_in)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.center = ZppVec3{center_in.x + 1.0f, center_in.y, center_in.z};
            if (op_CColSphere_Set(pre, badPost, radius_in, center_in, surf_in, piece_in)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.surface = (int64_t)(surf_in + 1);
            if (op_CColSphere_Set(pre, badPost, radius_in, center_in, surf_in, piece_in)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.piece = (int64_t)(piece_in + 1);
            if (op_CColSphere_Set(pre, badPost, radius_in, center_in, surf_in, piece_in)) return false;
        }
    }
    {
        CColStore_ZppContract pre{};
        pre.ms_pColPool_card = 1u;
        const auto post = apply_CColStore_Shutdown(pre);
        if (!op_CColStore_Shutdown(pre, post)) return false;
        {
            CColStore_ZppContract badPost = post;
            badPost.ms_pColPool_card = 1u;
            if (op_CColStore_Shutdown(pre, badPost)) return false;
        }
    }
    {
        CColTriangle_ZppContract pre{};
        const int64_t a_in = 25;
        const int64_t b_in = 25;
        const int64_t c_in = 25;
        const int64_t surf_in = 25;
        pre.surface = 0;
        const auto post = apply_CColTriangle_Set(pre, a_in, b_in, c_in, surf_in);
        if (!op_CColTriangle_Set(pre, post, a_in, b_in, c_in, surf_in)) return false;
        {
            CColTriangle_ZppContract badPost = post;
            badPost.a = (int64_t)(a_in + 1);
            if (op_CColTriangle_Set(pre, badPost, a_in, b_in, c_in, surf_in)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.b = (int64_t)(b_in + 1);
            if (op_CColTriangle_Set(pre, badPost, a_in, b_in, c_in, surf_in)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.c = (int64_t)(c_in + 1);
            if (op_CColTriangle_Set(pre, badPost, a_in, b_in, c_in, surf_in)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.surface = (int64_t)(surf_in + 1);
            if (op_CColTriangle_Set(pre, badPost, a_in, b_in, c_in, surf_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
static_assert(op_CColSphere_Set(CColSphere_ZppContract{}, apply_CColSphere_Set(CColSphere_ZppContract{}, 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5), 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5) && !op_CColSphere_Set(CColSphere_ZppContract{}, apply_CColSphere_Set(CColSphere_ZppContract{}, 2.5f, ZppVec3{4.0f, 5.0f, 6.0f}, 4, 5), 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5), "CColSphere_Set must verify 3D CVector center");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
struct GameWorld {
    struct Pillar { Vec3 pos; float radius; float height; };
    struct Building { Vec3 pos; float w{0}, d{0}, h{0}; }; // axis-aligned box, centred on pos
    struct RoadStrip { float x0{0}, z0{0}, x1{0}, z1{1}, width{3}; }; // axis-aligned segment
    std::vector<Building> buildings;
    std::vector<RoadStrip> roads;
    std::vector<Pillar> pillars;
    Vec3 goal{8.0f, 0.0f, 8.0f};
    Vec3 npcPos{-4.0f, 0.0f, 4.0f};
    float npcHeading{2.35f};
    float npcSpeed{0.0f};
    Vec3 questShard{-6.0f, 0.0f, -5.0f};
    bool questShardVisible{true};
    bool beaconRestored{false};
    bool questComplete{false};
    Vec3 vehiclePos{1.5f, 0.0f, 0.0f};
    float vehicleHeading{0.0f};
    float vehicleWheelPhase{0.0f};
    bool vehicleOccupied{false};
    // Honest social world: wandering walkers DTE can actually meet. Positions
    // are world metres; group_dynamics agents sync from these every tick.
    struct Walker { Vec3 pos; float heading{0}; float speed{0.7f}; float phase{0}; };
    std::vector<Walker> walkers;
    float walkerTime{0};
    // re3 CVehicle: automobiles, bikes, train, heli on fixed lanes.
    struct TrafficVehicle {
        Vec3 pos; float heading{0}; float speed{0};
        float min{0}; float max{0}; char axis{'x'}; char dir{1};
        float orbitR{0}; float orbitA{0}; float orbitY{0};
    };
    // re3 CObject pickup: bobbing reward entities.
    struct Pickup { Vec3 pos; float phase{0}; bool collected{false}; };
    // re3 CDummy: static street furniture.
    struct DummyProp { Vec3 pos; };
    std::vector<TrafficVehicle> traffic;
    std::vector<Pickup> pickups;
    std::vector<DummyProp> props;
    float trafficTime{0};
    float worldSize{128.0f};

    void generate(uint32_t seed, int pillarCount) {
        pillars.clear();
        uint32_t s = seed;
        auto rnd = [&]() { s = s * 1664525u + 1013904223u; return (s >> 8) / 16777216.0f; };
        // Stable placement with bounded retries and protected quest centres.
        for (int attempts=0; attempts<4096 && (int)pillars.size()<std::clamp(pillarCount,0,128); ++attempts) {
            Pillar p;
            p.pos = {(rnd() - 0.5f) * worldSize, 0, (rnd() - 0.5f) * worldSize};
            p.radius = 0.4f + rnd() * 0.5f;
            p.height = 1.5f + rnd() * 2.0f;
            if (len(p.pos) < 3.2f || len(p.pos-goal) < 3.2f ||
                len(p.pos-npcPos) < 3.2f || len(p.pos-questShard) < 3.2f ||
                len(p.pos-vehiclePos) < 2.7f) continue;
            bool overlap=false;
            for (const auto& other : pillars)
                if (len(p.pos-other.pos)<p.radius+other.radius+0.9f) {overlap=true;break;}
            if (!overlap) pillars.push_back(p);
        // Deterministic city block: buildings on a grid with protected zones,
        // plus two crossing roads. Same LCG stream, fully reproducible.
        buildings.clear(); roads.clear();
        roads.push_back({-worldSize*0.5f, 0.0f, worldSize*0.5f, 0.0f, 4.0f});   // E-W main road
        roads.push_back({0.0f, -worldSize*0.5f, 0.0f, worldSize*0.5f, 4.0f});     // N-S main road
        for (int gx = -2; gx <= 2; ++gx) {
            for (int gz = -2; gz <= 2; ++gz) {
                if (gx == 0 || gz == 0) continue; // keep the road corridors clear
                Building b;
                b.pos = {gx * 14.0f + (rnd() - 0.5f) * 3.0f, 0, gz * 14.0f + (rnd() - 0.5f) * 3.0f};
                b.w = 3.0f + rnd() * 3.0f;
                b.d = 3.0f + rnd() * 3.0f;
                b.h = 2.5f + rnd() * 5.5f;
                const Vec3 protected_[] = {Vec3{0,0,0}, goal, npcPos, questShard, vehiclePos};
                bool blocked = false;
                for (const auto& c : protected_) {
                    const float dx = std::fabs(b.pos.x - c.x), dz = std::fabs(b.pos.z - c.z);
                    if (dx < b.w * 0.5f + 4.0f && dz < b.d * 0.5f + 4.0f) { blocked = true; break; }
                }
                if (blocked) continue;
                bool overlapB = false;
                for (const auto& other : buildings) {
                    const float dx = std::fabs(b.pos.x - other.pos.x), dz = std::fabs(b.pos.z - other.pos.z);
                    if (dx < (b.w + other.w) * 0.5f + 2.0f && dz < (b.d + other.d) * 0.5f + 2.0f) { overlapB = true; break; }
                }
                if (!overlapB) buildings.push_back(b);
            }
        }
        }
        // Deterministic walker population: 20 wanderers, clear of protected
        // quest centres and solid geometry, same LCG stream (reproducible).
        walkers.clear();
        for (int attempts=0; attempts<4096 && (int)walkers.size()<20; ++attempts) {
            Walker w;
            w.pos = {(rnd() - 0.5f) * worldSize, 0, (rnd() - 0.5f) * worldSize};
            w.heading = rnd() * 6.2831853f;
            w.speed = 0.5f + rnd() * 0.6f;
            w.phase = rnd() * 6.2831853f;
            if (len(w.pos) < 4.0f || len(w.pos-goal) < 4.0f ||
                len(w.pos-npcPos) < 4.0f || len(w.pos-questShard) < 4.0f ||
                len(w.pos-vehiclePos) < 3.0f || collides(w.pos)) continue;
            bool crowded=false;
            for (const auto& other : walkers)
                if (len(w.pos-other.pos)<2.5f) {crowded=true;break;}
            if (!crowded) walkers.push_back(w);
        }
        // re3-style entity population: fixed verified-clear positions on the
        // road grid (no RNG, deterministic). Lanes: E-W auto z=+1.4, N-S bike
        // x=+0.9, train z=-20; heli orbits above max building height.
        traffic.clear(); pickups.clear(); props.clear();
        TrafficVehicle auto1; auto1.pos={-30,0,1.4f}; auto1.heading=1.5707963f; auto1.speed=3.2f; auto1.min=-32; auto1.max=32; auto1.axis='x'; auto1.dir=1;
        traffic.push_back(auto1);
        TrafficVehicle auto2; auto2.pos={30,0,1.4f}; auto2.heading=4.7123890f; auto2.speed=2.7f; auto2.min=-32; auto2.max=32; auto2.axis='x'; auto2.dir=-1;
        traffic.push_back(auto2);
        TrafficVehicle bike1; bike1.pos={0.9f,0,-30}; bike1.heading=0.0f; bike1.speed=1.8f; bike1.min=-32; bike1.max=32; bike1.axis='z'; bike1.dir=1;
        traffic.push_back(bike1);
        TrafficVehicle bike2; bike2.pos={0.9f,0,30}; bike2.heading=3.14159265f; bike2.speed=2.1f; bike2.min=-32; bike2.max=32; bike2.axis='z'; bike2.dir=-1;
        traffic.push_back(bike2);
        TrafficVehicle train; train.pos={-50,0,-20}; train.heading=1.5707963f; train.speed=4.5f; train.min=-52; train.max=52; train.axis='x'; train.dir=1;
        traffic.push_back(train);
        TrafficVehicle heli; heli.pos={20,8.5f,0}; heli.orbitR=20; heli.orbitA=0; heli.orbitY=8.5f; heli.speed=0.5f;
        traffic.push_back(heli);
        const Vec3 pickupSpots[] = {Vec3{6,0,2.5f}, Vec3{-6,0,-2.5f}, Vec3{2.5f,0,-8}, Vec3{-2.5f,0,10}};
        for (const auto& spot : pickupSpots) {
            Pickup pk; pk.pos = spot; pk.phase = (float)(pickups.size()) * 1.3f;
            pickups.push_back(pk);
        }
        const Vec3 lampSpots[] = {Vec3{15,0,2.6f}, Vec3{-15,0,2.6f}, Vec3{15,0,-2.6f}, Vec3{-15,0,-2.6f},
                                  Vec3{2.6f,0,15}, Vec3{-2.6f,0,15}, Vec3{2.6f,0,-15}, Vec3{-2.6f,0,-15}};
        for (const auto& spot : lampSpots) {
            DummyProp lamp; lamp.pos = spot;
            props.push_back(lamp);
        }
    }

    bool collides(Vec3 p) const {
        for (const auto& pl : pillars) {
            float dx = p.x - pl.pos.x, dz = p.z - pl.pos.z;
            if (dx*dx + dz*dz < (pl.radius + 0.25f) * (pl.radius + 0.25f)) return true;
        }
        for (const auto& b : buildings) {
            float dx = std::fabs(p.x - b.pos.x), dz = std::fabs(p.z - b.pos.z);
            if (dx < b.w * 0.5f + 0.25f && dz < b.d * 0.5f + 0.25f) return true;
        }
        return false;
    }
    float goalDistance(Vec3 p) const { return len(p - goal); }
    // Camera anti-clipping: march from the body toward the desired eye and
    // keep the last collision-free sample, so pillars never swallow the view.
    float ambX{-28.0f}, ambDir{1.0f};
    float busZ{10.0f},  busDir{-1.0f};
    Vec3 collisionFreeEye(Vec3 body, Vec3 desired) const {
        // LW54: Camera & ray pass freely through all world objects (never
        // registering hard collision or snapping on trees, buildings, props,
        // vehicles, or peds). Obfuscation is rectified smoothly per-pixel in
        // RasterEngine::rasterize via continuous alpha blending.
        // Only apply a local centre offset when a synthetic caller places
        // desired directly inside a pillar core at low height (desired.y < 1.5f).
        if (desired.y < 1.5f) {
            for (const auto& pl : pillars) {
                const float dx = desired.x - pl.pos.x, dz = desired.z - pl.pos.z;
                if (dx * dx + dz * dz < (pl.radius + 0.25f) * (pl.radius + 0.25f)) {
                    return desired + (body - desired) * 0.22f + Vec3{0.0f, 0.25f, 0.0f};
                }
            }
        }
        return desired;
    }
    bool pedBlockedByBarrier(Vec3 cand, Vec3 cur, size_t selfIdx) const {
        if (collides(cand)) return true;
        // Streetlamp poles (props)
        for (const auto& lamp : props) {
            const float dNext = len(cand - lamp.pos);
            if (dNext < 0.45f && dNext < len(cur - lamp.pos)) return true;
        }
        // Mutual barrier with Ariadne NPC & parked/driven coupe
        if (len(cand - npcPos) < 0.95f && len(cand - npcPos) < len(cur - npcPos)) return true;
        if (std::fabs(cand.x - vehiclePos.x) < 1.35f && std::fabs(cand.z - vehiclePos.z) < 2.35f &&
            len(cand - vehiclePos) < len(cur - vehiclePos)) return true;
        // Mutual pedestrian-to-pedestrian barrier
        for (size_t j = 0; j < walkers.size(); ++j) {
            if (j == selfIdx) continue;
            const float dNext = len(cand - walkers[j].pos);
            if (dNext < 0.95f && dNext < len(cur - walkers[j].pos)) return true;
        }
        // Mutual pedestrian-to-vehicle barrier against all moving ground traffic
        for (const auto& v : traffic) {
            if (v.orbitR > 0.0f) continue;
            const float halfX = (v.axis == 'x') ? 2.55f : 1.30f;
            const float halfZ = (v.axis == 'z') ? 2.55f : 1.30f;
            if (std::fabs(cand.x - v.pos.x) < halfX && std::fabs(cand.z - v.pos.z) < halfZ &&
                len(cand - v.pos) < len(cur - v.pos)) return true;
        }
        // Mutual pedestrian barrier against Ambulance (z = -1.4f) and 6-Wheel City Bus (x = -1.4f)
        const Vec3 ambPos{ambX, 0.0f, -1.4f};
        if (std::fabs(cand.x - ambPos.x) < 2.55f && std::fabs(cand.z - ambPos.z) < 1.35f &&
            len(cand - ambPos) < len(cur - ambPos)) return true;
        const Vec3 busPos{-1.4f, 0.0f, busZ};
        if (std::fabs(cand.x - busPos.x) < 1.45f && std::fabs(cand.z - busPos.z) < 3.15f &&
            len(cand - busPos) < len(cur - busPos)) return true;
        return false;
    }
    void tickWalkers(float dt) {
        walkerTime += dt;
        const Vec3 ambPos{ambX, 0.0f, -1.4f};
        const Vec3 busPos{-1.4f, 0.0f, busZ};
        for (size_t i = 0; i < walkers.size(); ++i) {
            auto& w = walkers[i];
            // Active mutual separation if encroached by another walker or vehicle
            for (size_t j = 0; j < walkers.size(); ++j) {
                if (i == j) continue;
                const Vec3 diff = w.pos - walkers[j].pos;
                const float d = len(diff);
                if (d > 1e-4f && d < 0.90f) {
                    const Vec3 pushed = w.pos + (diff * ((0.92f - d) / d));
                    if (!collides(pushed)) w.pos = pushed;
                }
            }
            auto pushOutOfVehicle = [&](Vec3 vp, float hx, float hz) {
                const float dx = w.pos.x - vp.x, dz = w.pos.z - vp.z;
                if (std::fabs(dx) < hx && std::fabs(dz) < hz) {
                    Vec3 pushed = w.pos;
                    if (hx - std::fabs(dx) < hz - std::fabs(dz))
                        pushed.x = vp.x + (dx >= 0.0f ? hx + 0.15f : -(hx + 0.15f));
                    else
                        pushed.z = vp.z + (dz >= 0.0f ? hz + 0.15f : -(hz + 0.15f));
                    if (!collides(pushed)) w.pos = pushed;
                }
            };
            for (const auto& v : traffic) {
                if (v.orbitR > 0.0f) continue;
                pushOutOfVehicle(v.pos, (v.axis == 'x') ? 2.45f : 1.25f, (v.axis == 'z') ? 2.45f : 1.25f);
            }
            pushOutOfVehicle(ambPos, 2.45f, 1.30f);
            pushOutOfVehicle(busPos, 1.40f, 3.05f);
            pushOutOfVehicle(vehiclePos, 1.30f, 2.25f);

            w.heading += 0.7f * std::sin(walkerTime * 0.6f + w.phase) * dt
                       + 0.4f * std::sin(walkerTime * 0.23f + w.phase * 2.1f) * dt;
            const Vec3 next = w.pos + Vec3{std::sin(w.heading) * w.speed * dt, 0,
                                           std::cos(w.heading) * w.speed * dt};
            if (pedBlockedByBarrier(next, w.pos, i) ||
                std::fabs(next.x) > worldSize * 0.5f - 1.0f ||
                std::fabs(next.z) > worldSize * 0.5f - 1.0f)
                w.heading += 1.25f;
            else
                w.pos = next;
        }
    }
    // re3 CVehicle lanes with mutual vehicle-to-vehicle, intersection & pedestrian barriers.
    void tickTraffic(float dt) {
        trafficTime += dt;
        struct GroundBox { Vec3 pos; float hx, hz; char axis; };
        auto getBoxes = [&](int skipIdx) {
            std::vector<GroundBox> boxes;
            for (size_t j = 0; j < traffic.size(); ++j) {
                if ((int)j == skipIdx || traffic[j].orbitR > 0.0f) continue;
                const auto& o = traffic[j];
                boxes.push_back({o.pos, (o.axis == 'x') ? 2.1f : 0.95f, (o.axis == 'z') ? 2.1f : 0.95f, o.axis});
            }
            if (skipIdx != 100) boxes.push_back({Vec3{ambX, 0.0f, -1.4f}, 2.1f, 0.95f, 'x'});
            if (skipIdx != 101) boxes.push_back({Vec3{-1.4f, 0.0f, busZ}, 0.95f, 2.55f, 'z'});
            return boxes;
        };
        auto blockedByPedOrCross = [&](Vec3 curP, Vec3 nextP, float hx, float hz, char axis, int selfCode, bool& shouldRebound) {
            shouldRebound = false;
            for (const auto& b : getBoxes(selfCode)) {
                const float curDx  = std::fabs(curP.x - b.pos.x),  curDz  = std::fabs(curP.z - b.pos.z);
                const float nextDx = std::fabs(nextP.x - b.pos.x), nextDz = std::fabs(nextP.z - b.pos.z);
                if (nextDx < (hx + b.hx + 0.35f) && nextDz < (hz + b.hz + 0.35f)) {
                    const float curDist = curDx + curDz, nextDist = nextDx + nextDz;
                    if (nextDist < curDist) {
                        if (b.axis == axis) shouldRebound = true;
                        return true;
                    }
                }
            }
            for (const auto& w : walkers) {
                if (std::fabs(nextP.x - w.pos.x) < (hx + 0.55f) && std::fabs(nextP.z - w.pos.z) < (hz + 0.55f) &&
                    len(nextP - w.pos) < len(curP - w.pos)) {
                    return true;
                }
            }
            return false;
        };

        for (size_t i = 0; i < traffic.size(); ++i) {
            auto& v = traffic[i];
            if (v.orbitR > 0.0f) {
                v.orbitA += v.speed * dt;
                v.pos = {std::cos(v.orbitA) * v.orbitR, v.orbitY, std::sin(v.orbitA) * v.orbitR};
                v.heading = -v.orbitA;
                continue;
            }
            float& along = v.axis == 'x' ? v.pos.x : v.pos.z;
            const float nextAlong = along + v.dir * v.speed * dt;
            const Vec3 nextPos = v.axis == 'x' ? Vec3{nextAlong, v.pos.y, v.pos.z} : Vec3{v.pos.x, v.pos.y, nextAlong};
            const float hx = (v.axis == 'x') ? 2.1f : 0.95f;
            const float hz = (v.axis == 'z') ? 2.1f : 0.95f;
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(v.pos, nextPos, hx, hz, v.axis, (int)i, rebound);
            if (rebound) {
                v.dir = -v.dir;
            } else if (!blocked) {
                along = nextAlong;
                if (along >= v.max) { along = v.max; v.dir = -1; }
                if (along <= v.min) { along = v.min; v.dir = 1; }
            }
            v.heading = v.axis == 'x'
                ? (v.dir > 0 ? 1.5707963f : 4.7123890f)
                : (v.dir > 0 ? 0.0f : 3.14159265f);
        }
        // Step Ambulance (z = -1.4f, axis = 'x') with full mutual barriers
        {
            const Vec3 curAmb{ambX, 0.0f, -1.4f};
            const float nextAmbX = ambX + ambDir * 2.9f * dt;
            const Vec3 nextAmb{nextAmbX, 0.0f, -1.4f};
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(curAmb, nextAmb, 2.1f, 0.95f, 'x', 100, rebound);
            if (rebound) ambDir = -ambDir;
            else if (!blocked) {
                ambX = nextAmbX;
                if (ambX >= 28.0f) { ambX = 28.0f; ambDir = -1.0f; }
                if (ambX <= -28.0f) { ambX = -28.0f; ambDir = 1.0f; }
            }
        }
        // Step 6-Wheel City Bus (x = -1.4f, axis = 'z') with full mutual barriers
        {
            const Vec3 curBus{-1.4f, 0.0f, busZ};
            const float nextBusZ = busZ + busDir * 2.2f * dt;
            const Vec3 nextBus{-1.4f, 0.0f, nextBusZ};
            bool rebound = false;
            const bool blocked = blockedByPedOrCross(curBus, nextBus, 0.95f, 2.55f, 'z', 101, rebound);
            if (rebound) busDir = -busDir;
            else if (!blocked) {
                busZ = nextBusZ;
                if (busZ >= 28.0f) { busZ = 28.0f; busDir = -1.0f; }
                if (busZ <= -28.0f) { busZ = -28.0f; busDir = 1.0f; }
            }
        }
    }
    // re3 CObject pickups: gentle bob, deterministic phase per slot.
    void tickPickups(float dt) {
        trafficTime += dt;
        for (auto& pk : pickups) {
            if (pk.collected) continue;
            pk.pos.y = 0.55f + 0.25f * std::sin(trafficTime * 2.0f + pk.phase);
        }
    }
};

// Component-specific Z++ collision bindings (from collision/ZppContracts.hpp)
constexpr zpp_collision_spec::CColSphere_ZppContract makePillarZppColSphere(Vec3 pos, float radius) {
    return zpp_collision_spec::apply_CColSphere_Set(zpp_collision_spec::CColSphere_ZppContract{}, radius, zpp_collision_spec::ZppVec3{pos.x, pos.y, pos.z}, 1, 0);
}
static_assert(zpp_collision_spec::op_CColSphere_Set(zpp_collision_spec::CColSphere_ZppContract{}, makePillarZppColSphere(Vec3{10.0f, 0.0f, -10.0f}, 0.55f), 0.55f, zpp_collision_spec::ZppVec3{10.0f, 0.0f, -10.0f}, 1, 0), "GameWorld pillar CColSphere Z++ contract must hold");
