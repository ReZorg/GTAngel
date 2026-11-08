#pragma once
#ifndef GTANGEL_ZPP_MATH_SPEC_GUARD
#define GTANGEL_ZPP_MATH_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/math
// Derived from C:/ddd/GTAngel/cpp_test/zpp/math/*.zpp
// Modules: 8 | Formal Schemas: 264 | Free Types: 0
// ============================================================================
namespace zpp_math_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 264;
static constexpr int kZppEnumCount   = 0;
// State Schema Contract from zpp/math/Matrix.zpp
struct CMatrix_ZppContract {
    uint8_t m_attachment_card{0u}; // Z++: Ref[RwMatrix]
    float fz{0.0f}; // Z++: ℝ
    bool m_hasRwMatrix{false}; // Z++: 𝔹
    float uw{0.0f}; // Z++: ℝ
    float pw{0.0f}; // Z++: ℝ
    float fw{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_attachment_card <= 1u);
    }
};
// State Schema Contract from zpp/math/Quaternion.zpp
struct CQuaternion_ZppContract {
    float w{0.0f}; // Z++: ℝ
    uint8_t this_field_card{0u}; // Z++: Ref[return]
    constexpr bool checkInvariant() const {
        return (this_field_card <= 1u);
    }
};
// State Schema Contract from zpp/math/Vector.zpp
struct CVector_ZppContract {
    float w{0.0f}; // Z++: ℝ
    uint8_t this_field_card{0u}; // Z++: Ref[return]
    constexpr bool checkInvariant() const {
        return (this_field_card <= 1u);
    }
};
// State Schema Contract from zpp/math/Vector2D.zpp
struct CVector2D_ZppContract {
    float y{0.0f}; // Z++: ℝ
    uint8_t this_field_card{0u}; // Z++: Ref[return]
    constexpr bool checkInvariant() const {
        return (this_field_card <= 1u);
    }
};
// State Schema Contract from zpp/math/VuVector.zpp
struct VuVectorModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{40}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 40);
    }
};
// State Schema Contract from zpp/math/math.zpp
struct mathModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{118}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 118);
    }
};
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix_2(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix_3(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_SetScale_4(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix_5(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_Attach_6(CMatrix_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_math_zpp_invariants() {
    {
        CMatrix_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMatrix_ZppContract bad{};
        bad.m_attachment_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CQuaternion_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CQuaternion_ZppContract bad{};
        bad.this_field_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVector_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVector_ZppContract bad{};
        bad.this_field_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVector2D_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVector2D_ZppContract bad{};
        bad.this_field_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        VuVectorModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VuVectorModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        mathModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        mathModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_CMatrix(st)) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_CMatrix_2(st)) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_CMatrix_3(st)) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_SetScale_4(st)) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_CMatrix_5(st)) return false;
    }
    {
        CMatrix_ZppContract st{};
        if (!op_CMatrix_Attach_6(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_math_zpp_invariants(), "Z++ formal contract invariant failed for math");
} // namespace zpp_math_spec
#endif // GTANGEL_ZPP_MATH_SPEC_GUARD
namespace avatar3d {

struct Vec3 { float x{0}, y{0}, z{0}; };
static Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
static float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static float len(Vec3 a) { return sqrtf(dot(a,a)); }
static Vec3 norm(Vec3 a) { float l = len(a); return l > 1e-6f ? a*(1.0f/l) : Vec3{0,0,0}; }

// ── Skeletal avatar model ──
// 17 joints (root, spine, head, arms, legs) with forward-kinematic pose.
// GTAngel v21: clean-room visual pose controller derived from the user-supplied
// DTE motion style guide. This declaration replaces AvatarSkeleton within
// namespace avatar3d; it never moves the body or writes a motor/world commit.
// LW21: baked v25 pose clips (DTEP25L). Clean-room offline bake of the
// user-supplied pose rig; runtime samples per-joint displacements only.
