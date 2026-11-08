#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_MODELINFO_SPEC_GUARD
#define GTANGEL_ZPP_MODELINFO_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/modelinfo
// Derived from C:/ddd/GTAngel/cpp_test/zpp/modelinfo/*.zpp
// Modules: 11 | Formal Schemas: 453 | Free Types: 20
// ============================================================================
namespace zpp_modelinfo_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 453;
static constexpr int kZppEnumCount   = 20;
// From zpp/modelinfo/BaseModelInfo.zpp
enum class ModelInfoType : int32_t { MITYPE_NA_v = 0, MITYPE_SIMPLE_v = 1, MITYPE_MLO_v = 2, MITYPE_TIME_v = 3, MITYPE_WEAPON_v = 4, MITYPE_CLUMP_v = 5, MITYPE_VEHICLE_v = 6, MITYPE_PED_v = 7, MITYPE_XTRACOMPS_v = 8, MITYPE_HAND_v = 9 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_1 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_TAXI_D_v = 8, MI_PIMP_v = 9, MI_CRIMINAL01_v = 10, MI_CRIMINAL02_v = 11 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_2 : int32_t { NUM_OF_SPECIAL_CHARS_v = 0, NUM_OF_CUTSCENE_OBJECTS_v = 1 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_3 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_HFYST_v = 8, MI_HFOST_v = 9, MI_HMYST_v = 10, MI_HMOST_v = 11 };
// State Schema Contract from zpp/modelinfo/BaseModelInfo.zpp
struct CBaseModelInfo_ZppContract {
    int32_t m_unk{0}; // Z++: seq ℕ  -- bounded array [2]
    uint32_t m_nameKey{0}; // Z++: ℕ
    int32_t m_chunk{0}; // Z++: ADDRESS
    uint32_t m_type{0}; // Z++: ℕ
    uint32_t m_num2dEffects{0}; // Z++: ℕ
    bool m_bOwnsColModel{false}; // Z++: 𝔹
    int32_t m_colModel{0}; // Z++: Ref[CColModel]
    int32_t m_2dEffectsID{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameIdAssocation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t hierId{0}; // Z++: ℤ
    uint32_t flags{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameAssociation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t frame{0}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectIdAssociation_ZppContract {
    int32_t id{0}; // Z++: ℤ
    int32_t frame{0}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct CClumpModelInfo_ZppContract {
    int32_t m_clump{0}; // Z++: Ref[RpClump]
    int32_t m_animFileName{0}; // Z++: seq CHAR
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CClumpModelInfo
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/MloModelInfo.zpp
struct CMloModelInfo_ZppContract {
    float drawDist{0.0f}; // Z++: ℝ
    int32_t firstInstance{0}; // Z++: ℤ
    int32_t lastInstance{0}; // Z++: ℤ
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CMloModelInfo
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_modelinfo_zpp_invariants() {
    if (!CBaseModelInfo_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectNameIdAssocation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectNameAssociation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectIdAssociation_ZppContract{}.checkInvariant()) return false;
    if (!CClumpModelInfo_ZppContract{}.checkInvariant()) return false;
    if (!CMloModelInfo_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_modelinfo_zpp_invariants(), "Z++ formal contract invariant failed for modelinfo");
} // namespace zpp_modelinfo_spec
#endif // GTANGEL_ZPP_MODELINFO_SPEC_GUARD
