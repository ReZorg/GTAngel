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
// Free Type from zpp/extras/custompipes.zpp
enum class custompipes_Enum_1 : int32_t { VEHICLEPIPE_PSP_v = 0, VEHICLEPIPE_PS2_v = 1, VEHICLEPIPE_MOBILE_v = 2, VEHICLEPIPE_NEO_v = -1 };
// Free Type from zpp/extras/custompipes.zpp
enum class custompipes_Enum_2 : int32_t { WORLDPIPE_PSP_v = 0, WORLDPIPE_PS2_v = 1, WORLDPIPE_MOBILE_v = 2 };
// Free Type from zpp/extras/custompipes.zpp
enum class custompipes_Enum_3 : int32_t { VEHICLEPIPE_MATFX_v = 0, VEHICLEPIPE_NEO_v = 1 };
// Free Type from zpp/extras/custompipes_d3d9.zpp
enum class custompipes_d3d9_Enum_1 : int32_t { VSLOC_boneMatrices_v = 0, VSLOC_viewVec_v = 1, VSLOC_rampStart_v = 2, VSLOC_rampEnd_v = 3, VSLOC_rimData_v = 4, VSLOC_eye_v = 5, VSLOC_reflProps_v = 6, VSLOC_specLights_v = 7, VSLOC_emissive_v = 8, VSLOC_ambient_v = 9, VSLOC_viewMat_v = 10, VSLOC_texMat_v = 11, PSLOC_colorscale_v = 1, PSLOC_shininess_v = 2, PSLOC_skyTop_v = 3, PSLOC_skyBot_v = 4 };
// Free Type from zpp/extras/custompipes_d3d9.zpp
enum class custompipes_d3d9_Enum_2 : int32_t { VSLOC_boneMatrices_v = 0, VSLOC_viewVec_v = 1, VSLOC_rampStart_v = 2, VSLOC_rampEnd_v = 3, VSLOC_rimData_v = 4, VSLOC_eye_v = 5, VSLOC_reflProps_v = 6, VSLOC_specLights_v = 7 };
// Free Type from zpp/extras/custompipes_d3d9.zpp
enum class custompipes_d3d9_Enum_3 : int32_t { VSLOC_boneMatrices_v = 0, VSLOC_viewVec_v = 1, VSLOC_rampStart_v = 2, VSLOC_rampEnd_v = 3, VSLOC_rimData_v = 4, VSLOC_eye_v = 5, VSLOC_reflProps_v = 6, VSLOC_specLights_v = 7 };
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
struct Color_ZppContract {
    float a{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct InterpolatedValue_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct InterpolatedFloat_ZppContract {
    float curInterpolator{0.0f}; // Z++: ℝ
    float curVal{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/custompipes.zpp
struct InterpolatedColor_ZppContract {
    float curInterpolator{0.0f}; // Z++: ℝ
    int32_t curVal{0}; // Z++: Color
    int64_t instance_addr{0}; // Z++: ADDRESS
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
// State Schema Contract from zpp/extras/custompipes_d3d9.zpp
struct BuildingInst_ZppContract {
    int32_t combinedMat{0}; // Z++: rw__RawMatrix
    uint8_t instHeader_card{0u}; // Z++: Ref[rw__d3d9__InstanceDataHeader]
    int64_t fadeAlpha{0}; // Z++: ℕ
    bool lighting{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (instHeader_card <= 1u)
            && (fadeAlpha >= 0);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct Pt_ZppContract {
    int32_t y{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct CTweakVar_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct CTweakVars_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct CTweakFunc_ZppContract {
    int32_t m_pVarName{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct CTweakBool_ZppContract {
    int32_t m_pVarName{0}; // Z++: seq CHAR
    uint8_t m_pBoolVar_card{0u}; // Z++: ℙ 𝔹
    constexpr bool checkInvariant() const {
        return (m_pBoolVar_card <= 1u);
    }
};
// State Schema Contract from zpp/extras/debugmenu.zpp
struct CTweakSwitch_ZppContract {
    int32_t m_pVarName{0}; // Z++: seq CHAR
    int32_t m_pIntVar{0}; // Z++: ADDRESS
    int32_t m_nMax{0}; // Z++: ℤ
    int32_t m_aStr{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
constexpr bool verify_extras_zpp_invariants() {
    {
        GitSHA1ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GitSHA1ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        arrowModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        arrowModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        cursorModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cursorModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CustomMatExt_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CustomMatExt_ZppContract bad{};
        bad.glossTex_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        Color_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Color_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        InterpolatedValue_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        InterpolatedValue_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        InterpolatedFloat_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        InterpolatedFloat_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        InterpolatedColor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        InterpolatedColor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        VsLight_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VsLight_ZppContract bad{};
        bad.pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        BuildingInst_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        BuildingInst_ZppContract bad{};
        bad.instHeader_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        Pt_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Pt_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTweakVar_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTweakVar_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTweakVars_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTweakVars_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTweakFunc_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTweakFunc_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTweakBool_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTweakBool_ZppContract bad{};
        bad.m_pBoolVar_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTweakSwitch_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTweakSwitch_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_extras_zpp_invariants(), "Z++ formal contract invariant failed for extras");
} // namespace zpp_extras_spec
#endif // GTANGEL_ZPP_EXTRAS_SPEC_GUARD
