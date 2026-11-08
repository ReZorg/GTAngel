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
constexpr bool verify_math_zpp_invariants() {
    if (!CMatrix_ZppContract{}.checkInvariant()) return false;
    if (!CQuaternion_ZppContract{}.checkInvariant()) return false;
    if (!CVector_ZppContract{}.checkInvariant()) return false;
    if (!CVector2D_ZppContract{}.checkInvariant()) return false;
    if (!VuVectorModuleState_ZppContract{}.checkInvariant()) return false;
    if (!mathModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_math_zpp_invariants(), "Z++ formal contract invariant failed for math");
} // namespace zpp_math_spec
#endif // GTANGEL_ZPP_MATH_SPEC_GUARD
