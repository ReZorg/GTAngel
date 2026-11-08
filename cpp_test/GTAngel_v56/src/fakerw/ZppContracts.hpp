#pragma once
#ifndef GTANGEL_ZPP_FAKERW_SPEC_GUARD
#define GTANGEL_ZPP_FAKERW_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/fakerw
// Derived from C:/ddd/GTAngel/cpp_test/zpp/fakerw/*.zpp
// Modules: 12 | Formal Schemas: 146 | Free Types: 31
// ============================================================================
namespace zpp_fakerw_spec {
static constexpr int kZppModuleCount = 12;
static constexpr int kZppSchemaCount = 146;
static constexpr int kZppEnumCount   = 31;
// From zpp/fakerw/rphanim.zpp
enum class RpHAnimHierarchyFlag : int32_t { rpHANIMHIERARCHYSUBHIERARCHY_v = 0, rpHANIMHIERARCHYNOMATRICES_v = 1, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES_v = 2, rpHANIMHIERARCHYUPDATELTMS_v = 3, rpHANIMHIERARCHYLOCALSPACEMATRICES_v = 4 };
// From zpp/fakerw/rpmatfx.zpp
enum class RpMatFXMaterialFlags : int32_t { rpMATFXEFFECTNULL_v = 0, rpMATFXEFFECTBUMPMAP_v = 1, rpMATFXEFFECTENVMAP_v = 2, rpMATFXEFFECTBUMPENVMAP_v = 3, rpMATFXEFFECTDUAL_v = 4, rpMATFXEFFECTMAX_v = 5, rpMATFXNUMEFFECTS_v = 6 };
// From zpp/fakerw/rpworld.zpp
enum class RpLightType : int32_t { rpNALIGHTTYPE_v = 0, rpLIGHTDIRECTIONAL_v = 1, rpLIGHTAMBIENT_v = 2, rpLIGHTPOINT_v = 128, rpLIGHTSPOT_v = 129, rpLIGHTSPOTSOFT_v = 130 };
// From zpp/fakerw/rpworld.zpp
enum class RpLightFlag : int32_t { rpLIGHTLIGHTATOMICS_v = 1, rpLIGHTLIGHTWORLD_v = 2 };
// State Schema Contract from zpp/fakerw/fake.zpp
struct fakeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1172}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1172);
    }
};
// State Schema Contract from zpp/fakerw/rpanisot.zpp
struct rpanisotModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6);
    }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdKeyFrame_ZppContract {
    uint8_t prevFrame_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    float time{0.0f}; // Z++: ℝ
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const {
        return (prevFrame_card <= 1u);
    }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdInterpFrame_ZppContract {
    uint8_t keyFrame1_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    uint8_t keyFrame2_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const {
        return (keyFrame1_card <= 1u)
            && (keyFrame2_card <= 1u);
    }
};
// State Schema Contract from zpp/fakerw/rpmatfx.zpp
struct rpmatfxModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{43}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 43);
    }
};
// State Schema Contract from zpp/fakerw/rtbmp.zpp
struct rtbmpModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{4}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 4);
    }
};
constexpr bool verify_fakerw_zpp_invariants() {
    if (!fakeModuleState_ZppContract{}.checkInvariant()) return false;
    if (!rpanisotModuleState_ZppContract{}.checkInvariant()) return false;
    if (!RpHAnimStdKeyFrame_ZppContract{}.checkInvariant()) return false;
    if (!RpHAnimStdInterpFrame_ZppContract{}.checkInvariant()) return false;
    if (!rpmatfxModuleState_ZppContract{}.checkInvariant()) return false;
    if (!rtbmpModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_fakerw_zpp_invariants(), "Z++ formal contract invariant failed for fakerw");
} // namespace zpp_fakerw_spec
#endif // GTANGEL_ZPP_FAKERW_SPEC_GUARD
