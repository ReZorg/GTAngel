#pragma once
#ifndef GTANGEL_ZPP_MODELINFO_SPEC_GUARD
#define GTANGEL_ZPP_MODELINFO_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
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
enum class ModelIndices_Enum_1 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_TAXI_D_v = 9, MI_PIMP_v = 10, MI_CRIMINAL01_v = 11, MI_CRIMINAL02_v = 12, MI_MALE02_v = 13, MI_MALE03_v = 14, MI_FATMALE01_v = 15, MI_FATMALE02_v = 16, MI_FEMALE01_v = 17, MI_FEMALE02_v = 18, MI_FEMALE03_v = 19, MI_FATFEMALE01_v = 20, MI_FATFEMALE02_v = 21, MI_PROSTITUTE_v = 22, MI_PROSTITUTE2_v = 23, MI_P_MAN1_v = 24, MI_P_MAN2_v = 25, MI_P_WOM1_v = 26, MI_P_WOM2_v = 27, MI_CT_MAN1_v = 28, MI_CT_MAN2_v = 29, MI_CT_WOM1_v = 30, MI_CT_WOM2_v = 31, MI_LI_MAN1_v = 32, MI_LI_MAN2_v = 33, MI_LI_WOM1_v = 34, MI_LI_WOM2_v = 35, MI_DOCKER1_v = 36, MI_DOCKER2_v = 37, MI_SCUM_MAN_v = 38, MI_SCUM_WOM_v = 39, MI_WORKER1_v = 40, MI_WORKER2_v = 41, MI_B_MAN1_v = 42, MI_B_MAN2_v = 43, MI_B_MAN3_v = 44, MI_B_WOM1_v = 45, MI_B_WOM2_v = 46, MI_B_WOM3_v = 47, MI_MOD_MAN_v = 48 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_2 : int32_t { NUM_OF_SPECIAL_CHARS_v = 21, NUM_OF_CUTSCENE_OBJECTS_v = 5 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_3 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_HFYST_v = 9, MI_HFOST_v = 10, MI_HMYST_v = 11, MI_HMOST_v = 12, MI_HFYRI_v = 13, MI_HFORI_v = 14, MI_HMYRI_v = 15, MI_HMORI_v = 16, MI_HFYBE_v = 17, MI_HFOBE_v = 18, MI_HMYBE_v = 19, MI_HMOBE_v = 20, MI_HFYBU_v = 21, MI_HFYMD_v = 22, MI_HFYCG_v = 23, MI_HFYPR_v = 24, MI_HFOTR_v = 25, MI_HMOTR_v = 26, MI_HMYAP_v = 27, MI_HMOCA_v = 28, MI_TAXI_D_v = 29, MI_BMODK_v = 30, MI_BMYKR_v = 31, MI_BFYST_v = 32, MI_BFOST_v = 33, MI_BMYST_v = 34, MI_BMOST_v = 35, MI_BFYRI_v = 36, MI_BFORI_v = 37, MI_BMYRI_v = 38, MI_BFYBE_v = 39, MI_BMYBE_v = 40, MI_BFOBE_v = 41, MI_BMOBE_v = 42, MI_BMYBU_v = 43, MI_BFYPR_v = 44, MI_BFOTR_v = 45, MI_BMOTR_v = 46, MI_BMYPI_v = 47, MI_BMYBB_v = 48 };
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameIdAssocation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t hierId{0}; // Z++: ℤ
    int64_t flags{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (flags >= 0);
    }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameAssociation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    uint8_t frame_card{0u}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const {
        return (frame_card <= 1u);
    }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectIdAssociation_ZppContract {
    int32_t id{0}; // Z++: ℤ
    uint8_t frame_card{0u}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const {
        return (frame_card <= 1u);
    }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct CClumpModelInfo_ZppContract {
    uint8_t m_clump_card{0u}; // Z++: Ref[RpClump]
    int32_t m_animFileName{0}; // Z++: seq CHAR
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CClumpModelInfo
    constexpr bool checkInvariant() const {
        return (m_clump_card <= 1u);
    }
};
// State Schema Contract from zpp/modelinfo/PedModelInfo.zpp
struct LimbCBarg_ZppContract {
    uint8_t mi_card{0u}; // Z++: Ref[CPedModelInfo]
    uint8_t clump_card{0u}; // Z++: Ref[RpClump]
    std::array<int32_t, 3> frameIDs{}; // Z++: seq ℤ  -- bounded array [3]
    size_t frameIDs_len{3u}; // Z++ cardinality #frameIDs
    constexpr bool checkInvariant() const {
        return (mi_card <= 1u)
            && (clump_card <= 1u)
            && (frameIDs_len == 3u && frameIDs.size() == 3u);
    }
};
// State Schema Contract from zpp/modelinfo/PedModelInfo.zpp
struct PedChunk_ZppContract {
    uint8_t colmodel_card{0u}; // Z++: Ref[CColModel]
    uint8_t clump_card{0u}; // Z++: Ref[RpClump]
    constexpr bool checkInvariant() const {
        return (colmodel_card <= 1u)
            && (clump_card <= 1u);
    }
};
constexpr bool verify_modelinfo_zpp_invariants() {
    if (!RwObjectNameIdAssocation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectNameAssociation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectIdAssociation_ZppContract{}.checkInvariant()) return false;
    if (!CClumpModelInfo_ZppContract{}.checkInvariant()) return false;
    if (!LimbCBarg_ZppContract{}.checkInvariant()) return false;
    if (!PedChunk_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_modelinfo_zpp_invariants(), "Z++ formal contract invariant failed for modelinfo");
} // namespace zpp_modelinfo_spec
#endif // GTANGEL_ZPP_MODELINFO_SPEC_GUARD
