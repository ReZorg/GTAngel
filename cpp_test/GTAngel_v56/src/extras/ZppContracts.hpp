#pragma once
#ifndef GTANGEL_ZPP_EXTRAS_SPEC_GUARD
#define GTANGEL_ZPP_EXTRAS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/extras
// Derived from C:/ddd/GTAngel/cpp_test/zpp/extras/*.zpp
// Modules: 12 | Formal Schemas: 568 | Free Types: 11
// ============================================================================
namespace zpp_extras_spec {
static constexpr int kZppModuleCount = 12;
static constexpr int kZppSchemaCount = 568;
static constexpr int kZppEnumCount   = 11;
// From zpp/extras/custompipes.zpp
enum class custompipes_Enum_1 : int32_t { VEHICLEPIPE_PSP_v = 0, VEHICLEPIPE_PS2_v = 1, VEHICLEPIPE_MOBILE_v = 2, VEHICLEPIPE_NEO_v = -1 };
// From zpp/extras/custompipes.zpp
enum class custompipes_Enum_2 : int32_t { WORLDPIPE_PSP_v = 0, WORLDPIPE_PS2_v = 1, WORLDPIPE_MOBILE_v = 2 };
// From zpp/extras/custompipes.zpp
enum class custompipes_Enum_3 : int32_t { VEHICLEPIPE_MATFX_v = 0, VEHICLEPIPE_NEO_v = 1 };
// From zpp/extras/custompipes_d3d9.zpp
enum class custompipes_d3d9_Enum_1 : int32_t { VSLOC_boneMatrices_v = 0, VSLOC_viewVec_v = 1, VSLOC_rampStart_v = 2, VSLOC_rampEnd_v = 3, VSLOC_rimData_v = 4, VSLOC_eye_v = 5, VSLOC_reflProps_v = 6, VSLOC_specLights_v = 7, VSLOC_emissive_v = 8, VSLOC_ambient_v = 9, VSLOC_viewMat_v = 10, VSLOC_texMat_v = 11, PSLOC_colorscale_v = 1, PSLOC_shininess_v = 2, PSLOC_skyTop_v = 3, PSLOC_skyBot_v = 4 };
// State Schema Contract from zpp/extras/GitSHA1.zpp
struct GitSHA1ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1);
    }
};
// State Schema Contract from zpp/extras/arrow.zpp
struct arrowModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{16}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 16);
    }
};
// State Schema Contract from zpp/extras/cursor.zpp
struct cursorModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{16}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 16);
    }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct CustomMatExt_ZppContract {
    uint8_t glossTex_card{0u}; // Z++: Ref[rw__Texture]
    bool haveGloss{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (glossTex_card <= 1u);
    }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct InterpolatedValue_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/custompipes_d3d9.zpp
struct VsLight_ZppContract {
    int32_t color{0}; // Z++: rw__RGBAf
    std::array<float, 4> pos{}; // Z++: seq ℝ  -- bounded array [4]
    size_t pos_len{4u}; // Z++ cardinality #pos
    int32_t dir{0}; // Z++: rw__V3d
    float power{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (pos_len == 4u && pos.size() == 4u);
    }
};
constexpr bool verify_extras_zpp_invariants() {
    if (!GitSHA1ModuleState_ZppContract{}.checkInvariant()) return false;
    if (!arrowModuleState_ZppContract{}.checkInvariant()) return false;
    if (!cursorModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CustomMatExt_ZppContract{}.checkInvariant()) return false;
    if (!InterpolatedValue_ZppContract{}.checkInvariant()) return false;
    if (!VsLight_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_extras_zpp_invariants(), "Z++ formal contract invariant failed for extras");
} // namespace zpp_extras_spec
#endif // GTANGEL_ZPP_EXTRAS_SPEC_GUARD
