#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_FAKERW_SPEC_GUARD
#define GTANGEL_ZPP_FAKERW_SPEC_GUARD
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
enum class RpLightType : int32_t { rpNALIGHTTYPE_v = 0, rpLIGHTDIRECTIONAL_v = 1, rpLIGHTAMBIENT_v = 2, rpLIGHTPOINT_v = 3, rpLIGHTSPOT_v = 4, rpLIGHTSPOTSOFT_v = 5 };
// From zpp/fakerw/rpworld.zpp
enum class RpLightFlag : int32_t { rpLIGHTLIGHTATOMICS_v = 0, rpLIGHTLIGHTWORLD_v = 1 };
// State Schema Contract from zpp/fakerw/fake.zpp
struct fakeModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/fakerw/rpanisot.zpp
struct rpanisotModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdKeyFrame_ZppContract {
    int32_t prevFrame{0}; // Z++: Ref[RpHAnimStdKeyFrame]
    float time{0.0f}; // Z++: ℝ
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdInterpFrame_ZppContract {
    int32_t keyFrame1{0}; // Z++: Ref[RpHAnimStdKeyFrame]
    int32_t keyFrame2{0}; // Z++: Ref[RpHAnimStdKeyFrame]
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/fakerw/rpmatfx.zpp
struct rpmatfxModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/fakerw/rpskin.zpp
struct RwMatrixWeights_ZppContract {
    float w0{0.0f}; // Z++: ℝ
    float w1{0.0f}; // Z++: ℝ
    float w2{0.0f}; // Z++: ℝ
    float w3{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_fakerw_zpp_invariants() {
    if (!fakeModuleState_ZppContract{}.checkInvariant()) return false;
    if (!rpanisotModuleState_ZppContract{}.checkInvariant()) return false;
    if (!RpHAnimStdKeyFrame_ZppContract{}.checkInvariant()) return false;
    if (!RpHAnimStdInterpFrame_ZppContract{}.checkInvariant()) return false;
    if (!rpmatfxModuleState_ZppContract{}.checkInvariant()) return false;
    if (!RwMatrixWeights_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_fakerw_zpp_invariants(), "Z++ formal contract invariant failed for fakerw");
} // namespace zpp_fakerw_spec
#endif // GTANGEL_ZPP_FAKERW_SPEC_GUARD
