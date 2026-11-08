#pragma once
#ifndef GTANGEL_ZPP_COLLISION_SPEC_GUARD
#define GTANGEL_ZPP_COLLISION_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/collision
// Derived from C:/ddd/GTAngel/cpp_test/zpp/collision/*.zpp
// Modules: 11 | Formal Schemas: 250 | Free Types: 2
// ============================================================================
namespace zpp_collision_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 250;
static constexpr int kZppEnumCount   = 2;
// Free Type from zpp/collision/ColTriangle.zpp
enum class Direction : int32_t { DIR_X_POS_v = 0, DIR_X_NEG_v = 1, DIR_Y_POS_v = 2, DIR_Y_NEG_v = 3, DIR_Z_POS_v = 4, DIR_Z_NEG_v = 5 };
// Free Type from zpp/collision/Collision.zpp
enum class Collision_Enum_1 : int32_t { MAXNUMSPHERES_v = 128, MAXNUMBOXES_v = 32, MAXNUMLINES_v = 16, MAXNUMTRIS_v = 600 };
// State Schema Contract from zpp/collision/ColBox.zpp
struct CBox_ZppContract {
    int32_t min_field{0}; // Z++: CVector
    int32_t max_field{0}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColBox_ZppContract {
    int32_t min_field{0}; // Z++: CVector
    int32_t max_field{0}; // Z++: CVector
    int64_t surface{0}; // Z++: ℕ
    int64_t piece{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (surface >= 0)
            && (piece >= 0);
    }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColTriBBox_ZppContract {
    int32_t first{0}; // Z++: ℤ
    int32_t last{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColLine.zpp
struct CColLine_ZppContract {
    int32_t p0{0}; // Z++: CVector
    int32_t p1{0}; // Z++: CVector
    int32_t pad0{0}; // Z++: ℤ
    int32_t pad1{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
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
    int32_t point{0}; // Z++: CVector
    int32_t pad1{0}; // Z++: ℤ
    int32_t normal{0}; // Z++: CVector
    int32_t pad2{0}; // Z++: ℤ
    int64_t surfaceA{0}; // Z++: ℕ
    int64_t pieceA{0}; // Z++: ℕ
    int64_t surfaceB{0}; // Z++: ℕ
    int64_t pieceB{0}; // Z++: ℕ
    float depth{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (surfaceA >= 0)
            && (pieceA >= 0)
            && (surfaceB >= 0)
            && (pieceB >= 0);
    }
};
// State Schema Contract from zpp/collision/ColSphere.zpp
struct CColSphere_ZppContract {
    int64_t surface{0}; // Z++: ℕ
    int64_t piece{0}; // Z++: ℕ
    int32_t center{0}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (surface >= 0)
            && (piece >= 0);
    }
};
// State Schema Contract from zpp/collision/ColSphere.zpp
struct CSphere_ZppContract {
    int32_t center{0}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/ColStore.zpp
struct ColChunkEntry_ZppContract {
    int32_t modelId{0}; // Z++: ℤ
    uint8_t colModel_card{0u}; // Z++: Ref[CColModel]
    constexpr bool checkInvariant() const {
        return (colModel_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/ColStore.zpp
struct CColStore_ZppContract {
    uint8_t ms_pColPool_card{0u}; // Z++: Ref[CPool_ColDef_ColDef]
    bool m_onlyBB{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (ms_pColPool_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/ColTriangle.zpp
struct CColTriangle_ZppContract {
    int64_t a{0}; // Z++: ℕ
    int64_t b{0}; // Z++: ℕ
    int64_t c{0}; // Z++: ℕ
    int64_t surface{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (dir >= 0);
    }
};
// State Schema Contract from zpp/collision/Collision.zpp
struct CStoredCollPoly_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/collision/Collision.zpp
struct CCollision_ZppContract {
    int32_t ms_collisionInMemory{0}; // Z++: eLevelName
    uint8_t ms_colModelCache_card{0u}; // Z++: Ref[CLinkList_CColModel]
    constexpr bool checkInvariant() const {
        return (ms_colModelCache_card <= 1u);
    }
};
// State Schema Contract from zpp/collision/CompressedVector.zpp
struct CompressedVector_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (ms_colModelCutObj_len == 10u && ms_colModelCutObj.size() == 10u);
    }
};
// State Schema Contract from zpp/collision/VuCollision.zpp
struct VuTriangle_ZppContract {
    int64_t v1{0}; // Z++: ℕ
    int64_t v2{0}; // Z++: ℕ
    int64_t plane{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
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
constexpr bool op_CColBox_Set(const CColBox_ZppContract& pre, const CColBox_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.min_field == 1) && (post.max_field == 1) && (post.surface == 1) && (post.piece == 1);
}
constexpr CColBox_ZppContract apply_CColBox_Set(const CColBox_ZppContract& pre) {
    CColBox_ZppContract post = pre;
    post.min_field = 1;
    post.max_field = 1;
    post.surface = 1;
    post.piece = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_operator(const CColBox_ZppContract& pre, const CColBox_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.min_field == 1) && (post.max_field == 1) && (post.surface == 1) && (post.piece == 1);
}
constexpr CColBox_ZppContract apply_CColBox_operator(const CColBox_ZppContract& pre) {
    CColBox_ZppContract post = pre;
    post.min_field = 1;
    post.max_field = 1;
    post.surface = 1;
    post.piece = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCColLine) from zpp/collision/ColLine.zpp
constexpr bool op_CColLine_Set(const CColLine_ZppContract& pre, const CColLine_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.p0 == 1) && (post.p1 == 1);
}
constexpr CColLine_ZppContract apply_CColLine_Set(const CColLine_ZppContract& pre) {
    CColLine_ZppContract post = pre;
    post.p0 = 1;
    post.p1 = 1;
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
// Delta Operation Schema Contract (ΔCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_operator(const CColModel_ZppContract& pre, const CColModel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.boundingSphere == 1) && (post.boundingBox == 1) && (post.numTriBBoxes == 0) && (post.triBBoxes_card == 0u);
}
constexpr CColModel_ZppContract apply_CColModel_operator(const CColModel_ZppContract& pre) {
    CColModel_ZppContract post = pre;
    post.boundingSphere = 1;
    post.boundingBox = 1;
    post.numTriBBoxes = 0;
    post.triBBoxes_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColModel) from zpp/collision/ColModel.zpp
constexpr bool op_CColModel_operator_2(const CColModel_ZppContract& pre, const CColModel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.boundingSphere == 1) && (post.boundingBox == 1) && (post.numSpheres == 0) && (post.spheres_card == 0u);
}
constexpr CColModel_ZppContract apply_CColModel_operator_2(const CColModel_ZppContract& pre) {
    CColModel_ZppContract post = pre;
    post.boundingSphere = 1;
    post.boundingBox = 1;
    post.numSpheres = 0;
    post.spheres_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCColPoint) from zpp/collision/ColPoint.zpp
constexpr bool op_CColPoint_operator(const CColPoint_ZppContract& pre, const CColPoint_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.point == 1) && (post.normal == 1) && (post.surfaceA == 1) && (post.pieceA == 1) && (post.surfaceB == 1) && (post.pieceB == 1);
}
constexpr CColPoint_ZppContract apply_CColPoint_operator(const CColPoint_ZppContract& pre) {
    CColPoint_ZppContract post = pre;
    post.point = 1;
    post.normal = 1;
    post.surfaceA = 1;
    post.pieceA = 1;
    post.surfaceB = 1;
    post.pieceB = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCColSphere) from zpp/collision/ColSphere.zpp
constexpr bool op_CColSphere_Set(const CColSphere_ZppContract& pre, const CColSphere_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.radius == 1.0f) && (post.center == 1) && (post.surface == 1) && (post.piece == 1);
}
constexpr CColSphere_ZppContract apply_CColSphere_Set(const CColSphere_ZppContract& pre) {
    CColSphere_ZppContract post = pre;
    post.radius = 1.0f;
    post.center = 1;
    post.surface = 1;
    post.piece = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCColStore) from zpp/collision/ColStore.zpp
constexpr bool op_CColStore_Initialise(const CColStore_ZppContract& pre, const CColStore_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_pColPool_card == 1u);
}
constexpr CColStore_ZppContract apply_CColStore_Initialise(const CColStore_ZppContract& pre) {
    CColStore_ZppContract post = pre;
    post.ms_pColPool_card = 1u;
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
// Delta Operation Schema Contract (ΔCColStore) from zpp/collision/ColStore.zpp
constexpr bool op_CColStore_Load(const CColStore_ZppContract& pre, const CColStore_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_pColPool_card == 1u) && (post.m_onlyBB == true);
}
constexpr CColStore_ZppContract apply_CColStore_Load(const CColStore_ZppContract& pre) {
    CColStore_ZppContract post = pre;
    post.ms_pColPool_card = 1u;
    post.m_onlyBB = true;
    return post;
}
// Delta Operation Schema Contract (ΔCColTriangle) from zpp/collision/ColTriangle.zpp
constexpr bool op_CColTriangle_Set(const CColTriangle_ZppContract& pre, const CColTriangle_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.a == 1) && (post.b == 1) && (post.c == 1) && (post.surface == 1);
}
constexpr CColTriangle_ZppContract apply_CColTriangle_Set(const CColTriangle_ZppContract& pre) {
    CColTriangle_ZppContract post = pre;
    post.a = 1;
    post.b = 1;
    post.c = 1;
    post.surface = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCCollision) from zpp/collision/Collision.zpp
constexpr bool op_CCollision_Init(const CCollision_ZppContract& pre, const CCollision_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_collisionInMemory == 1);
}
constexpr CCollision_ZppContract apply_CCollision_Init(const CCollision_ZppContract& pre) {
    CCollision_ZppContract post = pre;
    post.ms_collisionInMemory = 1;
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
        bad.surface = -1;
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
        pre.surface = 0;
        const auto post = apply_CColBox_Set(pre);
        if (!op_CColBox_Set(pre, post)) return false;
        {
            CColBox_ZppContract badPost = post;
            badPost.min_field = 0;
            if (op_CColBox_Set(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.max_field = 0;
            if (op_CColBox_Set(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.surface = 0;
            if (op_CColBox_Set(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.piece = 0;
            if (op_CColBox_Set(pre, badPost)) return false;
        }
    }
    {
        CColBox_ZppContract pre{};
        pre.min_field = 0;
        pre.max_field = 0;
        pre.surface = 0;
        pre.piece = 0;
        const auto post = apply_CColBox_operator(pre);
        if (!op_CColBox_operator(pre, post)) return false;
        {
            CColBox_ZppContract badPost = post;
            badPost.min_field = 0;
            if (op_CColBox_operator(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.max_field = 0;
            if (op_CColBox_operator(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.surface = 0;
            if (op_CColBox_operator(pre, badPost)) return false;
        }
        {
            CColBox_ZppContract badPost = post;
            badPost.piece = 0;
            if (op_CColBox_operator(pre, badPost)) return false;
        }
    }
    {
        CColLine_ZppContract pre{};
        const auto post = apply_CColLine_Set(pre);
        if (!op_CColLine_Set(pre, post)) return false;
        {
            CColLine_ZppContract badPost = post;
            badPost.p0 = 0;
            if (op_CColLine_Set(pre, badPost)) return false;
        }
        {
            CColLine_ZppContract badPost = post;
            badPost.p1 = 0;
            if (op_CColLine_Set(pre, badPost)) return false;
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
        CColModel_ZppContract pre{};
        pre.boundingSphere = 0;
        pre.boundingBox = 0;
        pre.numTriBBoxes = 1;
        pre.triBBoxes_card = 1u;
        const auto post = apply_CColModel_operator(pre);
        if (!op_CColModel_operator(pre, post)) return false;
        {
            CColModel_ZppContract badPost = post;
            badPost.boundingSphere = 0;
            if (op_CColModel_operator(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.boundingBox = 0;
            if (op_CColModel_operator(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numTriBBoxes = 1;
            if (op_CColModel_operator(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.triBBoxes_card = 1u;
            if (op_CColModel_operator(pre, badPost)) return false;
        }
    }
    {
        CColModel_ZppContract pre{};
        pre.boundingSphere = 0;
        pre.boundingBox = 0;
        pre.numSpheres = 1;
        pre.spheres_card = 1u;
        const auto post = apply_CColModel_operator_2(pre);
        if (!op_CColModel_operator_2(pre, post)) return false;
        {
            CColModel_ZppContract badPost = post;
            badPost.boundingSphere = 0;
            if (op_CColModel_operator_2(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.boundingBox = 0;
            if (op_CColModel_operator_2(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.numSpheres = 1;
            if (op_CColModel_operator_2(pre, badPost)) return false;
        }
        {
            CColModel_ZppContract badPost = post;
            badPost.spheres_card = 1u;
            if (op_CColModel_operator_2(pre, badPost)) return false;
        }
    }
    {
        CColPoint_ZppContract pre{};
        pre.point = 0;
        pre.normal = 0;
        pre.surfaceA = 0;
        pre.pieceA = 0;
        pre.surfaceB = 0;
        pre.pieceB = 0;
        const auto post = apply_CColPoint_operator(pre);
        if (!op_CColPoint_operator(pre, post)) return false;
        {
            CColPoint_ZppContract badPost = post;
            badPost.point = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
        {
            CColPoint_ZppContract badPost = post;
            badPost.normal = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
        {
            CColPoint_ZppContract badPost = post;
            badPost.surfaceA = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
        {
            CColPoint_ZppContract badPost = post;
            badPost.pieceA = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
        {
            CColPoint_ZppContract badPost = post;
            badPost.surfaceB = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
        {
            CColPoint_ZppContract badPost = post;
            badPost.pieceB = 0;
            if (op_CColPoint_operator(pre, badPost)) return false;
        }
    }
    {
        CColSphere_ZppContract pre{};
        pre.radius = 0.0f;
        pre.surface = 0;
        const auto post = apply_CColSphere_Set(pre);
        if (!op_CColSphere_Set(pre, post)) return false;
        {
            CColSphere_ZppContract badPost = post;
            badPost.radius = 0.0f;
            if (op_CColSphere_Set(pre, badPost)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.center = 0;
            if (op_CColSphere_Set(pre, badPost)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.surface = 0;
            if (op_CColSphere_Set(pre, badPost)) return false;
        }
        {
            CColSphere_ZppContract badPost = post;
            badPost.piece = 0;
            if (op_CColSphere_Set(pre, badPost)) return false;
        }
    }
    {
        CColStore_ZppContract pre{};
        pre.ms_pColPool_card = 0u;
        const auto post = apply_CColStore_Initialise(pre);
        if (!op_CColStore_Initialise(pre, post)) return false;
        {
            CColStore_ZppContract badPost = post;
            badPost.ms_pColPool_card = 0u;
            if (op_CColStore_Initialise(pre, badPost)) return false;
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
        CColStore_ZppContract pre{};
        pre.ms_pColPool_card = 0u;
        pre.m_onlyBB = false;
        const auto post = apply_CColStore_Load(pre);
        if (!op_CColStore_Load(pre, post)) return false;
        {
            CColStore_ZppContract badPost = post;
            badPost.ms_pColPool_card = 0u;
            if (op_CColStore_Load(pre, badPost)) return false;
        }
        {
            CColStore_ZppContract badPost = post;
            badPost.m_onlyBB = false;
            if (op_CColStore_Load(pre, badPost)) return false;
        }
    }
    {
        CColTriangle_ZppContract pre{};
        pre.surface = 0;
        const auto post = apply_CColTriangle_Set(pre);
        if (!op_CColTriangle_Set(pre, post)) return false;
        {
            CColTriangle_ZppContract badPost = post;
            badPost.a = 0;
            if (op_CColTriangle_Set(pre, badPost)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.b = 0;
            if (op_CColTriangle_Set(pre, badPost)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.c = 0;
            if (op_CColTriangle_Set(pre, badPost)) return false;
        }
        {
            CColTriangle_ZppContract badPost = post;
            badPost.surface = 0;
            if (op_CColTriangle_Set(pre, badPost)) return false;
        }
    }
    {
        CCollision_ZppContract pre{};
        pre.ms_collisionInMemory = 0;
        const auto post = apply_CCollision_Init(pre);
        if (!op_CCollision_Init(pre, post)) return false;
        {
            CCollision_ZppContract badPost = post;
            badPost.ms_collisionInMemory = 0;
            if (op_CCollision_Init(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
