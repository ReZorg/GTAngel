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
// Free Type from zpp/modelinfo/BaseModelInfo.zpp
enum class ModelInfoType : int32_t { MITYPE_NA_v = 0, MITYPE_SIMPLE_v = 1, MITYPE_MLO_v = 2, MITYPE_TIME_v = 3, MITYPE_WEAPON_v = 4, MITYPE_CLUMP_v = 5, MITYPE_VEHICLE_v = 6, MITYPE_PED_v = 7, MITYPE_XTRACOMPS_v = 8, MITYPE_HAND_v = 9 };
// Free Type from zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_1 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_TAXI_D_v = 9, MI_PIMP_v = 10, MI_CRIMINAL01_v = 11, MI_CRIMINAL02_v = 12, MI_MALE02_v = 13, MI_MALE03_v = 14, MI_FATMALE01_v = 15, MI_FATMALE02_v = 16, MI_FEMALE01_v = 17, MI_FEMALE02_v = 18, MI_FEMALE03_v = 19, MI_FATFEMALE01_v = 20, MI_FATFEMALE02_v = 21, MI_PROSTITUTE_v = 22, MI_PROSTITUTE2_v = 23, MI_P_MAN1_v = 24, MI_P_MAN2_v = 25, MI_P_WOM1_v = 26, MI_P_WOM2_v = 27, MI_CT_MAN1_v = 28, MI_CT_MAN2_v = 29, MI_CT_WOM1_v = 30, MI_CT_WOM2_v = 31, MI_LI_MAN1_v = 32, MI_LI_MAN2_v = 33, MI_LI_WOM1_v = 34, MI_LI_WOM2_v = 35, MI_DOCKER1_v = 36, MI_DOCKER2_v = 37, MI_SCUM_MAN_v = 38, MI_SCUM_WOM_v = 39, MI_WORKER1_v = 40, MI_WORKER2_v = 41, MI_B_MAN1_v = 42, MI_B_MAN2_v = 43, MI_B_MAN3_v = 44, MI_B_WOM1_v = 45, MI_B_WOM2_v = 46, MI_B_WOM3_v = 47, MI_MOD_MAN_v = 48 };
// Free Type from zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_2 : int32_t { NUM_OF_SPECIAL_CHARS_v = 21, NUM_OF_CUTSCENE_OBJECTS_v = 5 };
// Free Type from zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_3 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_HFYST_v = 9, MI_HFOST_v = 10, MI_HMYST_v = 11, MI_HMOST_v = 12, MI_HFYRI_v = 13, MI_HFORI_v = 14, MI_HMYRI_v = 15, MI_HMORI_v = 16, MI_HFYBE_v = 17, MI_HFOBE_v = 18, MI_HMYBE_v = 19, MI_HMOBE_v = 20, MI_HFYBU_v = 21, MI_HFYMD_v = 22, MI_HFYCG_v = 23, MI_HFYPR_v = 24, MI_HFOTR_v = 25, MI_HMOTR_v = 26, MI_HMYAP_v = 27, MI_HMOCA_v = 28, MI_TAXI_D_v = 29, MI_BMODK_v = 30, MI_BMYKR_v = 31, MI_BFYST_v = 32, MI_BFOST_v = 33, MI_BMYST_v = 34, MI_BMOST_v = 35, MI_BFYRI_v = 36, MI_BFORI_v = 37, MI_BMYRI_v = 38, MI_BFYBE_v = 39, MI_BMYBE_v = 40, MI_BFOBE_v = 41, MI_BMOBE_v = 42, MI_BMYBU_v = 43, MI_BFYPR_v = 44, MI_BFOTR_v = 45, MI_BMOTR_v = 46, MI_BMYPI_v = 47, MI_BMYBB_v = 48 };
// Free Type from zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_4 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_TAXI_D_v = 8, MI_PIMP_v = 9, MI_GANG01_v = 10, MI_GANG02_v = 11, MI_GANG03_v = 12, MI_GANG04_v = 13, MI_GANG05_v = 14, MI_GANG06_v = 15, MI_GANG07_v = 16, MI_GANG08_v = 17, MI_GANG09_v = 18, MI_GANG10_v = 19, MI_GANG11_v = 20, MI_GANG12_v = 21, MI_GANG13_v = 22, MI_GANG14_v = 23, MI_CRIMINAL01_v = 24, MI_CRIMINAL02_v = 25, MI_SPECIAL01_v = 26, MI_SPECIAL02_v = 27, MI_SPECIAL03_v = 28, MI_SPECIAL04_v = 29, MI_MALE02_v = 30, MI_MALE03_v = 31, MI_FATMALE01_v = 32, MI_FATMALE02_v = 33, MI_FEMALE01_v = 34, MI_FEMALE02_v = 35, MI_FEMALE03_v = 36, MI_FATFEMALE01_v = 37, MI_FATFEMALE02_v = 38, MI_PROSTITUTE_v = 39, MI_PROSTITUTE2_v = 40, MI_P_MAN1_v = 41, MI_P_MAN2_v = 42, MI_P_WOM1_v = 43, MI_P_WOM2_v = 44, MI_CT_MAN1_v = 45, MI_CT_MAN2_v = 46, MI_CT_WOM1_v = 47 };
// Free Type from zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_5 : int32_t { NUM_OF_SPECIAL_CHARS_v = 4, NUM_OF_CUTSCENE_OBJECTS_v = 5 };
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
        return (frameIDs_len == 3u && frameIDs.size() == 3u)
            && (mi_card <= 1u)
            && (clump_card <= 1u);
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
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_CClumpModelInfo(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_CClumpModelInfo_2(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_CClumpModelInfo_3(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_CreateInstance_4(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_CreateInstance_5(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_GetRwObject_6(CClumpModelInfo_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_modelinfo_zpp_invariants() {
    {
        RwObjectNameIdAssocation_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwObjectNameIdAssocation_ZppContract bad{};
        bad.flags = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RwObjectNameAssociation_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwObjectNameAssociation_ZppContract bad{};
        bad.frame_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        RwObjectIdAssociation_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwObjectIdAssociation_ZppContract bad{};
        bad.frame_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClumpModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CClumpModelInfo_ZppContract bad{};
        bad.m_clump_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        LimbCBarg_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LimbCBarg_ZppContract bad{};
        bad.frameIDs_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        PedChunk_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedChunk_ZppContract bad{};
        bad.colmodel_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_CClumpModelInfo(st)) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_CClumpModelInfo_2(st)) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_CClumpModelInfo_3(st)) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_CreateInstance_4(st)) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_CreateInstance_5(st)) return false;
    }
    {
        CClumpModelInfo_ZppContract st{};
        if (!op_CClumpModelInfo_GetRwObject_6(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_modelinfo_zpp_invariants(), "Z++ formal contract invariant failed for modelinfo");
} // namespace zpp_modelinfo_spec
#endif // GTANGEL_ZPP_MODELINFO_SPEC_GUARD
