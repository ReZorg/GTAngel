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
// Delta Operation Schema Contract (ΔCColStore) from zpp/collision/ColStore.zpp
constexpr bool op_CColStore_Shutdown(const CColStore_ZppContract& pre, const CColStore_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_pColPool_card == 0u);
}
constexpr CColStore_ZppContract apply_CColStore_Shutdown(const CColStore_ZppContract& pre) {
    CColStore_ZppContract post = pre;
    post.ms_pColPool_card = 0u;
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
        VuTriangle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VuTriangle_ZppContract bad{};
        bad.v1 = -1;
        if (bad.checkInvariant()) return false;
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
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
