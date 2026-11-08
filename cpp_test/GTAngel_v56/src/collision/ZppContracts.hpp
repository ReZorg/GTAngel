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
struct CColBox_ZppContract {
    int32_t min{0}; // Z++: CVector
    int32_t max{0}; // Z++: CVector
    int64_t surface{0}; // Z++: ℕ
    int64_t piece{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (surface >= 0)
            && (piece >= 0);
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
// Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_Set(CColBox_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_GetSize_2(CColBox_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_operator_3(CColBox_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_Set_4(CColBox_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCColBox) from zpp/collision/ColBox.zpp
constexpr bool op_CColBox_operator(CColBox_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCColPoint) from zpp/collision/ColPoint.zpp
constexpr bool op_CColPoint_GetNormal(CColPoint_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_collision_zpp_invariants() {
    {
        CColBox_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CColBox_ZppContract bad{};
        bad.surface = -1;
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
        CColBox_ZppContract st{};
        if (!op_CColBox_Set(st)) return false;
    }
    {
        CColBox_ZppContract st{};
        if (!op_CColBox_GetSize_2(st)) return false;
    }
    {
        CColBox_ZppContract st{};
        if (!op_CColBox_operator_3(st)) return false;
    }
    {
        CColBox_ZppContract st{};
        if (!op_CColBox_Set_4(st)) return false;
    }
    {
        CColBox_ZppContract st{};
        if (!op_CColBox_operator(st)) return false;
    }
    {
        CColPoint_ZppContract st{};
        if (!op_CColPoint_GetNormal(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
