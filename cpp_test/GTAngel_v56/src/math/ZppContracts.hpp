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
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
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
// State Schema Contract from zpp/math/Matrix.zpp
struct CCompressedMatrixNotAligned_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_rightX{0}; // Z++: ℤ
    int32_t m_rightY{0}; // Z++: ℤ
    int32_t m_rightZ{0}; // Z++: ℤ
    int32_t m_upX{0}; // Z++: ℤ
    int32_t m_upY{0}; // Z++: ℤ
    int32_t m_upZ{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/math/Matrix.zpp
struct CCompressedMatrix_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_rightX{0}; // Z++: ℤ
    int32_t m_rightY{0}; // Z++: ℤ
    int32_t m_rightZ{0}; // Z++: ℤ
    int32_t m_upX{0}; // Z++: ℤ
    int32_t m_upY{0}; // Z++: ℤ
    int32_t m_upZ{0}; // Z++: ℤ
    int32_t _alignment{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
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
// State Schema Contract from zpp/math/Rect.zpp
struct CRect_ZppContract {
    float left{0.0f}; // Z++: ℝ
    float bottom{0.0f}; // Z++: ℝ
    float right{0.0f}; // Z++: ℝ
    float top{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
// State Schema Contract from zpp/math/maths.zpp
struct mathsModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{19}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 19);
    }
};
// Query Operation Schema Contract (ΞCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_GetPosition_18(const CMatrix_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_GetRight_19(const CMatrix_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_GetForward_20(const CMatrix_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_GetUp_21(const CMatrix_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCQuaternion) from zpp/math/Quaternion.zpp
constexpr bool op_CQuaternion_MagnitudeSqr_2(const CQuaternion_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCQuaternion) from zpp/math/Quaternion.zpp
constexpr bool op_CQuaternion_operator_10(const CQuaternion_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_Magnitude_2(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_MagnitudeSqr_3(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_operator_12(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_operator_13(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_operator_14(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCVector2D) from zpp/math/Vector2D.zpp
constexpr bool op_CVector2D_operator_15(const CVector2D_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix(const CMatrix_ZppContract& pre, const CMatrix_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_attachment_card == 0u) && (post.m_hasRwMatrix == false);
}
constexpr CMatrix_ZppContract apply_CMatrix_CMatrix(const CMatrix_ZppContract& pre) {
    CMatrix_ZppContract post = pre;
    post.m_attachment_card = 0u;
    post.m_hasRwMatrix = false;
    return post;
}
// Delta Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_CMatrix_3(const CMatrix_ZppContract& pre, const CMatrix_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_attachment_card == 0u);
}
constexpr CMatrix_ZppContract apply_CMatrix_CMatrix_3(const CMatrix_ZppContract& pre) {
    CMatrix_ZppContract post = pre;
    post.m_attachment_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCMatrix) from zpp/math/Matrix.zpp
constexpr bool op_CMatrix_Detach(const CMatrix_ZppContract& pre, const CMatrix_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_attachment_card == 0u);
}
constexpr CMatrix_ZppContract apply_CMatrix_Detach(const CMatrix_ZppContract& pre) {
    CMatrix_ZppContract post = pre;
    post.m_attachment_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCRect) from zpp/math/Rect.zpp
constexpr bool op_CRect_CRect_2(const CRect_ZppContract& pre, const CRect_ZppContract& post, float l_in = 2.5f, float t_in = 2.5f, float r_in = 2.5f, float b_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.left == l_in) && (post.top == t_in) && (post.right == r_in) && (post.bottom == b_in);
}
constexpr CRect_ZppContract apply_CRect_CRect_2(const CRect_ZppContract& pre, float l_in = 2.5f, float t_in = 2.5f, float r_in = 2.5f, float b_in = 2.5f) {
    CRect_ZppContract post = pre;
    post.left = l_in;
    post.top = t_in;
    post.right = r_in;
    post.bottom = b_in;
    return post;
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
        CCompressedMatrixNotAligned_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCompressedMatrixNotAligned_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCompressedMatrix_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCompressedMatrix_ZppContract bad{};
        bad.instance_addr = -1;
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
        CRect_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRect_ZppContract bad{};
        bad.instance_addr = -1;
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
        mathsModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        mathsModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        const CMatrix_ZppContract st{};
        if (!op_CMatrix_GetPosition_18(st)) return false;
    }
    {
        const CMatrix_ZppContract st{};
        if (!op_CMatrix_GetRight_19(st)) return false;
    }
    {
        const CMatrix_ZppContract st{};
        if (!op_CMatrix_GetForward_20(st)) return false;
    }
    {
        const CMatrix_ZppContract st{};
        if (!op_CMatrix_GetUp_21(st)) return false;
    }
    {
        const CQuaternion_ZppContract st{};
        if (!op_CQuaternion_MagnitudeSqr_2(st)) return false;
    }
    {
        const CQuaternion_ZppContract st{};
        if (!op_CQuaternion_operator_10(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_Magnitude_2(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_MagnitudeSqr_3(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_operator_12(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_operator_13(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_operator_14(st)) return false;
    }
    {
        const CVector2D_ZppContract st{};
        if (!op_CVector2D_operator_15(st)) return false;
    }
    {
        CMatrix_ZppContract pre{};
        pre.m_attachment_card = 1u;
        pre.m_hasRwMatrix = true;
        const auto post = apply_CMatrix_CMatrix(pre);
        if (!op_CMatrix_CMatrix(pre, post)) return false;
        {
            CMatrix_ZppContract badPost = post;
            badPost.m_attachment_card = 1u;
            if (op_CMatrix_CMatrix(pre, badPost)) return false;
        }
        {
            CMatrix_ZppContract badPost = post;
            badPost.m_hasRwMatrix = true;
            if (op_CMatrix_CMatrix(pre, badPost)) return false;
        }
    }
    {
        CMatrix_ZppContract pre{};
        pre.m_attachment_card = 1u;
        const auto post = apply_CMatrix_CMatrix_3(pre);
        if (!op_CMatrix_CMatrix_3(pre, post)) return false;
        {
            CMatrix_ZppContract badPost = post;
            badPost.m_attachment_card = 1u;
            if (op_CMatrix_CMatrix_3(pre, badPost)) return false;
        }
    }
    {
        CMatrix_ZppContract pre{};
        pre.m_attachment_card = 1u;
        const auto post = apply_CMatrix_Detach(pre);
        if (!op_CMatrix_Detach(pre, post)) return false;
        {
            CMatrix_ZppContract badPost = post;
            badPost.m_attachment_card = 1u;
            if (op_CMatrix_Detach(pre, badPost)) return false;
        }
    }
    {
        CRect_ZppContract pre{};
        const float l_in = 2.5f;
        const float t_in = 2.5f;
        const float r_in = 2.5f;
        const float b_in = 2.5f;
        pre.left = 0.0f;
        pre.top = 0.0f;
        pre.right = 0.0f;
        pre.bottom = 0.0f;
        const auto post = apply_CRect_CRect_2(pre, l_in, t_in, r_in, b_in);
        if (!op_CRect_CRect_2(pre, post, l_in, t_in, r_in, b_in)) return false;
        {
            CRect_ZppContract badPost = post;
            badPost.left = l_in + 1.0f;
            if (op_CRect_CRect_2(pre, badPost, l_in, t_in, r_in, b_in)) return false;
        }
        {
            CRect_ZppContract badPost = post;
            badPost.top = t_in + 1.0f;
            if (op_CRect_CRect_2(pre, badPost, l_in, t_in, r_in, b_in)) return false;
        }
        {
            CRect_ZppContract badPost = post;
            badPost.right = r_in + 1.0f;
            if (op_CRect_CRect_2(pre, badPost, l_in, t_in, r_in, b_in)) return false;
        }
        {
            CRect_ZppContract badPost = post;
            badPost.bottom = b_in + 1.0f;
            if (op_CRect_CRect_2(pre, badPost, l_in, t_in, r_in, b_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_math_zpp_invariants(), "Z++ formal contract invariant failed for math");
static_assert(op_CVector2D_Magnitude_2(CVector2D_ZppContract{}), "CVector2D_Magnitude_2 query contract must hold");
} // namespace zpp_math_spec
#endif // GTANGEL_ZPP_MATH_SPEC_GUARD
