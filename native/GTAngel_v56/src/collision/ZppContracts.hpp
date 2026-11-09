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
