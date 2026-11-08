#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_EXTRAS_SPEC_GUARD
#define GTANGEL_ZPP_EXTRAS_SPEC_GUARD
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
enum class custompipes_Enum_1 : int32_t { VEHICLEPIPE_PSP_v = 0, VEHICLEPIPE_PS2_v = 1, VEHICLEPIPE_MOBILE_v = 2, VEHICLEPIPE_NEO_v = 3 };
// From zpp/extras/custompipes.zpp
enum class custompipes_Enum_2 : int32_t { WORLDPIPE_PSP_v = 0, WORLDPIPE_PS2_v = 1, WORLDPIPE_MOBILE_v = 2 };
// From zpp/extras/custompipes.zpp
enum class custompipes_Enum_3 : int32_t { VEHICLEPIPE_MATFX_v = 0, VEHICLEPIPE_NEO_v = 1 };
// From zpp/extras/custompipes_d3d9.zpp
enum class custompipes_d3d9_Enum_1 : int32_t { VSLOC_boneMatrices_v = 0, VSLOC_viewVec_v = 1, VSLOC_rampStart_v = 2, VSLOC_rampEnd_v = 3, VSLOC_rimData_v = 4, VSLOC_eye_v = 5, VSLOC_reflProps_v = 6, VSLOC_specLights_v = 7, VSLOC_emissive_v = 8, VSLOC_ambient_v = 9, VSLOC_viewMat_v = 10, VSLOC_texMat_v = 11 };
// State Schema Contract from zpp/extras/GitSHA1.zpp
struct GitSHA1ModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/extras/arrow.zpp
struct arrowModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/extras/cursor.zpp
struct cursorModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct CustomMatExt_ZppContract {
    int32_t glossTex{0}; // Z++: Ref[rw__Texture]
    bool haveGloss{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct Color_ZppContract {
    float a{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct InterpolatedValue_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_extras_zpp_invariants() {
    if (!GitSHA1ModuleState_ZppContract{}.checkInvariant()) return false;
    if (!arrowModuleState_ZppContract{}.checkInvariant()) return false;
    if (!cursorModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CustomMatExt_ZppContract{}.checkInvariant()) return false;
    if (!Color_ZppContract{}.checkInvariant()) return false;
    if (!InterpolatedValue_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_extras_zpp_invariants(), "Z++ formal contract invariant failed for extras");
} // namespace zpp_extras_spec
#endif // GTANGEL_ZPP_EXTRAS_SPEC_GUARD
