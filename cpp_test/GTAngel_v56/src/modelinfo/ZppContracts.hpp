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
// State Schema Contract from zpp/modelinfo/BaseModelInfo.zpp
struct CBaseModelInfo_ZppContract {
    std::array<uint32_t, 2> m_unk{}; // Z++: seq ℕ  -- bounded array [2]
    size_t m_unk_len{2u}; // Z++ cardinality #m_unk
    int64_t m_nameKey{0}; // Z++: ℕ
    int32_t m_chunk{0}; // Z++: ADDRESS
    int64_t m_type{0}; // Z++: ℕ
    int64_t m_num2dEffects{0}; // Z++: ℕ
    bool m_bOwnsColModel{false}; // Z++: 𝔹
    uint8_t m_colModel_card{0u}; // Z++: Ref[CColModel]
    int32_t m_2dEffectsID{0}; // Z++: ℤ
    int32_t m_objectId{0}; // Z++: ℤ
    int64_t m_refCount{0}; // Z++: ℕ
    int32_t m_txdSlot{0}; // Z++: ℤ
    uint8_t m_twodEffects_card{0u}; // Z++: Ref[C2dEffect]
    bool m_bIsTree{false}; // Z++: 𝔹
    bool m_bCanBeIgnored{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_unk_len == 2u && m_unk.size() == 2u)
            && (m_nameKey >= 0)
            && (m_type >= 0)
            && (m_num2dEffects >= 0)
            && (m_colModel_card <= 1u)
            && (m_refCount >= 0)
            && (m_twodEffects_card <= 1u);
    }
};
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
// State Schema Contract from zpp/modelinfo/MloModelInfo.zpp
struct CMloModelInfo_ZppContract {
    float drawDist{0.0f}; // Z++: ℝ
    int32_t firstInstance{0}; // Z++: ℤ
    int32_t lastInstance{0}; // Z++: ℤ
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CMloModelInfo
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/modelinfo/ModelIndices.zpp
struct CModelIndices_ZppContract {
    int32_t TRAFFICLIGHTS{0}; // Z++: ℤ
    int32_t TLIGHT_POST{0}; // Z++: ℤ
    int32_t TLIGHT_WALK{0}; // Z++: ℤ
    int32_t TLIGHT_BOX1{0}; // Z++: ℤ
    int32_t TLIGHT_BOX2{0}; // Z++: ℤ
    int32_t TRAFFICLIGHTS_VERTICAL{0}; // Z++: ℤ
    int32_t TRAFFICLIGHTS_MIAMI{0}; // Z++: ℤ
    int32_t TRAFFICLIGHTS_TWOVERTICAL{0}; // Z++: ℤ
    int32_t SINGLESTREETLIGHTS1{0}; // Z++: ℤ
    int32_t SINGLESTREETLIGHTS2{0}; // Z++: ℤ
    int32_t SINGLESTREETLIGHTS3{0}; // Z++: ℤ
    int32_t DOUBLESTREETLIGHTS{0}; // Z++: ℤ
    int32_t STREETLAMP1{0}; // Z++: ℤ
    int32_t STREETLAMP2{0}; // Z++: ℤ
    int32_t ROADSFORROADBLOCKSSTART{0}; // Z++: ℤ
    int32_t ROADSFORROADBLOCKSEND{0}; // Z++: ℤ
    int32_t TREE2{0}; // Z++: ℤ
    int32_t TREE3{0}; // Z++: ℤ
    int32_t TREE6{0}; // Z++: ℤ
    int32_t TREE8{0}; // Z++: ℤ
    int32_t TREE1{0}; // Z++: ℤ
    int32_t TREE4{0}; // Z++: ℤ
    int32_t TREE5{0}; // Z++: ℤ
    int32_t TREE7{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/modelinfo/ModelInfo.zpp
struct CModelInfo_ZppContract {
    uint8_t ms_modelInfoPtrs_card{0u}; // Z++: Ref[CBaseModelInfo]
    int32_t msNumModelInfos{0}; // Z++: ℤ
    int32_t ms_simpleModelStore{0}; // Z++: CStore_CSimpleModelInfo__SIMPLEMODELSIZE
    int32_t ms_timeModelStore{0}; // Z++: CStore_CTimeModelInfo__TIMEMODELSIZE
    int32_t ms_weaponModelStore{0}; // Z++: CStore_CWeaponModelInfo__WEAPONMODELSIZE
    int32_t ms_clumpModelStore{0}; // Z++: CStore_CClumpModelInfo__CLUMPMODELSIZE
    int32_t ms_pedModelStore{0}; // Z++: CStore_CPedModelInfo__PEDMODELSIZE
    int32_t ms_vehicleModelStore{0}; // Z++: CStore_CVehicleModelInfo__VEHICLEMODELSIZE
    int32_t ms_2dEffectStore{0}; // Z++: CStore_C2dEffect__TWODFXSIZE
    int32_t resNumModelInfos{0}; // Z++: ℤ
    uint8_t resModelInfoPtrs_card{0u}; // Z++: Ref[CBaseModelInfo]
    int32_t ms_mloModelStore{0}; // Z++: CStore_CMloModelInfo__MLOMODELSIZE
    int32_t ms_mloInstanceStore{0}; // Z++: CStore_CInstance__MLOINSTANCESIZE
    int32_t ms_xtraCompsModelStore{0}; // Z++: CStore_CXtraCompsModelInfo__XTRACOMPSMODELSIZE
    constexpr bool checkInvariant() const {
        return (ms_modelInfoPtrs_card <= 1u)
            && (resModelInfoPtrs_card <= 1u);
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
struct ColNodeInfo_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t pedNode{0}; // Z++: ℤ
    int32_t pieceType{0}; // Z++: ℤ
    float z{0.0f}; // Z++: ℝ
    float radius{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
// State Schema Contract from zpp/modelinfo/PedModelInfo.zpp
struct CPedModelInfo_ZppContract {
    int64_t m_animGroup{0}; // Z++: ℕ
    int32_t m_pedType{0}; // Z++: ePedType
    int32_t m_pedStatType{0}; // Z++: ePedStats
    int64_t m_carsCanDrive{0}; // Z++: ℕ
    uint8_t m_hitColModel_card{0u}; // Z++: Ref[CColModel]
    uint8_t m_lhand_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t m_rhand_card{0u}; // Z++: Ref[RpAtomic]
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CPedModelInfo
    constexpr bool checkInvariant() const {
        return (m_animGroup >= 0)
            && (m_carsCanDrive >= 0)
            && (m_hitColModel_card <= 1u)
            && (m_lhand_card <= 1u)
            && (m_rhand_card <= 1u);
    }
};
// State Schema Contract from zpp/modelinfo/TimeModelInfo.zpp
struct CTimeModelInfo_ZppContract {
    int32_t m_timeOn{0}; // Z++: ℤ
    int32_t m_timeOff{0}; // Z++: ℤ
    int32_t m_otherTimeModelID{0}; // Z++: ℤ
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CTimeModelInfo
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/modelinfo/VehicleModelInfo.zpp
struct editableMatCBData_ZppContract {
    uint8_t vehicle_card{0u}; // Z++: Ref[CVehicleModelInfo]
    int32_t numMats1{0}; // Z++: ℤ
    int32_t numMats2{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (vehicle_card <= 1u);
    }
};
// State Schema Contract from zpp/modelinfo/VehicleModelInfo.zpp
struct Statics_ZppContract {
    int32_t unknown{0}; // Z++: ADDRESS
    std::array<int32_t, 256> ms_vehicleColourTable{}; // Z++: seq RwRGBA  -- bounded array [256]
    size_t ms_vehicleColourTable_len{256u}; // Z++ cardinality #ms_vehicleColourTable
    std::array<int32_t, 2> ms_compsUsed{}; // Z++: seq ℤ  -- bounded array [2]
    size_t ms_compsUsed_len{2u}; // Z++ cardinality #ms_compsUsed
    std::array<int32_t, 2> ms_compsToUse{}; // Z++: seq ℤ  -- bounded array [2]
    size_t ms_compsToUse_len{2u}; // Z++ cardinality #ms_compsToUse
    constexpr bool checkInvariant() const {
        return (ms_vehicleColourTable_len == 256u && ms_vehicleColourTable.size() == 256u)
            && (ms_compsUsed_len == 2u && ms_compsUsed.size() == 2u)
            && (ms_compsToUse_len == 2u && ms_compsToUse.size() == 2u);
    }
};
// State Schema Contract from zpp/modelinfo/WeaponModelInfo.zpp
struct CWeaponModelInfo_ZppContract {
    int32_t m_animFileName{0}; // Z++: seq CHAR
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CWeaponModelInfo
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/modelinfo/XtraCompsModelInfo.zpp
struct CXtraCompsModelInfo_ZppContract {
    int32_t field_34{0}; // Z++: ℤ
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CXtraCompsModelInfo
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Query Operation Schema Contract (ΞCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_GetModelType_14(const CBaseModelInfo_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_DeleteCollisionModel(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_colModel_card == 0u);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_DeleteCollisionModel(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_colModel_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_Init2dEffects(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_twodEffects_card == 0u) && (post.m_num2dEffects == 0);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_Init2dEffects(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_twodEffects_card = 0u;
    post.m_num2dEffects = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_Add2dEffect(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_2dEffectsID == 1) && (post.m_num2dEffects == 1);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_Add2dEffect(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_2dEffectsID = 1;
    post.m_num2dEffects = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_SetModelName(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nameKey == 1);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_SetModelName(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_nameKey = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_DeleteChunk(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_chunk == 1);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_DeleteChunk(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_chunk = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCBaseModelInfo) from zpp/modelinfo/BaseModelInfo.zpp
constexpr bool op_CBaseModelInfo_Write(const CBaseModelInfo_ZppContract& pre, const CBaseModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_chunk == 1);
}
constexpr CBaseModelInfo_ZppContract apply_CBaseModelInfo_Write(const CBaseModelInfo_ZppContract& pre) {
    CBaseModelInfo_ZppContract post = pre;
    post.m_chunk = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_DeleteRwObject(const CClumpModelInfo_ZppContract& pre, const CClumpModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_clump_card == 0u);
}
constexpr CClumpModelInfo_ZppContract apply_CClumpModelInfo_DeleteRwObject(const CClumpModelInfo_ZppContract& pre) {
    CClumpModelInfo_ZppContract post = pre;
    post.m_clump_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCClumpModelInfo) from zpp/modelinfo/ClumpModelInfo.zpp
constexpr bool op_CClumpModelInfo_SetAnimFile(const CClumpModelInfo_ZppContract& pre, const CClumpModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_animFileName == 1);
}
constexpr CClumpModelInfo_ZppContract apply_CClumpModelInfo_SetAnimFile(const CClumpModelInfo_ZppContract& pre) {
    CClumpModelInfo_ZppContract post = pre;
    post.m_animFileName = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCModelInfo) from zpp/modelinfo/ModelInfo.zpp
constexpr bool op_CModelInfo_Load(const CModelInfo_ZppContract& pre, const CModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.msNumModelInfos == 1) && (post.ms_modelInfoPtrs_card == 1u);
}
constexpr CModelInfo_ZppContract apply_CModelInfo_Load(const CModelInfo_ZppContract& pre) {
    CModelInfo_ZppContract post = pre;
    post.msNumModelInfos = 1;
    post.ms_modelInfoPtrs_card = 1u;
    return post;
}
// Delta Operation Schema Contract (ΔCWeaponModelInfo) from zpp/modelinfo/WeaponModelInfo.zpp
constexpr bool op_CWeaponModelInfo_SetAnimFile(const CWeaponModelInfo_ZppContract& pre, const CWeaponModelInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_animFileName == 1);
}
constexpr CWeaponModelInfo_ZppContract apply_CWeaponModelInfo_SetAnimFile(const CWeaponModelInfo_ZppContract& pre) {
    CWeaponModelInfo_ZppContract post = pre;
    post.m_animFileName = 1;
    return post;
}
constexpr bool verify_modelinfo_zpp_invariants() {
    {
        CBaseModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBaseModelInfo_ZppContract bad{};
        bad.m_unk_len = 0u;
        if (bad.checkInvariant()) return false;
    }
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
        CMloModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMloModelInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CModelIndices_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CModelIndices_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CModelInfo_ZppContract bad{};
        bad.ms_modelInfoPtrs_card = 2u;
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
        ColNodeInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ColNodeInfo_ZppContract bad{};
        bad.instance_addr = -1;
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
        CPedModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedModelInfo_ZppContract bad{};
        bad.m_animGroup = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTimeModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTimeModelInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        editableMatCBData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        editableMatCBData_ZppContract bad{};
        bad.vehicle_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        Statics_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Statics_ZppContract bad{};
        bad.ms_vehicleColourTable_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWeaponModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeaponModelInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CXtraCompsModelInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CXtraCompsModelInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const CBaseModelInfo_ZppContract st{};
        if (!op_CBaseModelInfo_GetModelType_14(st)) return false;
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_colModel_card = 1u;
        const auto post = apply_CBaseModelInfo_DeleteCollisionModel(pre);
        if (!op_CBaseModelInfo_DeleteCollisionModel(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_colModel_card = 1u;
            if (op_CBaseModelInfo_DeleteCollisionModel(pre, badPost)) return false;
        }
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_twodEffects_card = 1u;
        pre.m_num2dEffects = 1;
        const auto post = apply_CBaseModelInfo_Init2dEffects(pre);
        if (!op_CBaseModelInfo_Init2dEffects(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_twodEffects_card = 1u;
            if (op_CBaseModelInfo_Init2dEffects(pre, badPost)) return false;
        }
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_num2dEffects = 1;
            if (op_CBaseModelInfo_Init2dEffects(pre, badPost)) return false;
        }
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_2dEffectsID = 0;
        pre.m_num2dEffects = 0;
        const auto post = apply_CBaseModelInfo_Add2dEffect(pre);
        if (!op_CBaseModelInfo_Add2dEffect(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_2dEffectsID = 0;
            if (op_CBaseModelInfo_Add2dEffect(pre, badPost)) return false;
        }
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_num2dEffects = 0;
            if (op_CBaseModelInfo_Add2dEffect(pre, badPost)) return false;
        }
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_nameKey = 0;
        const auto post = apply_CBaseModelInfo_SetModelName(pre);
        if (!op_CBaseModelInfo_SetModelName(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_nameKey = 0;
            if (op_CBaseModelInfo_SetModelName(pre, badPost)) return false;
        }
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_chunk = 0;
        const auto post = apply_CBaseModelInfo_DeleteChunk(pre);
        if (!op_CBaseModelInfo_DeleteChunk(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_chunk = 0;
            if (op_CBaseModelInfo_DeleteChunk(pre, badPost)) return false;
        }
    }
    {
        CBaseModelInfo_ZppContract pre{};
        pre.m_chunk = 0;
        const auto post = apply_CBaseModelInfo_Write(pre);
        if (!op_CBaseModelInfo_Write(pre, post)) return false;
        {
            CBaseModelInfo_ZppContract badPost = post;
            badPost.m_chunk = 0;
            if (op_CBaseModelInfo_Write(pre, badPost)) return false;
        }
    }
    {
        CClumpModelInfo_ZppContract pre{};
        pre.m_clump_card = 1u;
        const auto post = apply_CClumpModelInfo_DeleteRwObject(pre);
        if (!op_CClumpModelInfo_DeleteRwObject(pre, post)) return false;
        {
            CClumpModelInfo_ZppContract badPost = post;
            badPost.m_clump_card = 1u;
            if (op_CClumpModelInfo_DeleteRwObject(pre, badPost)) return false;
        }
    }
    {
        CClumpModelInfo_ZppContract pre{};
        pre.m_animFileName = 0;
        const auto post = apply_CClumpModelInfo_SetAnimFile(pre);
        if (!op_CClumpModelInfo_SetAnimFile(pre, post)) return false;
        {
            CClumpModelInfo_ZppContract badPost = post;
            badPost.m_animFileName = 0;
            if (op_CClumpModelInfo_SetAnimFile(pre, badPost)) return false;
        }
    }
    {
        CModelInfo_ZppContract pre{};
        pre.msNumModelInfos = 0;
        pre.ms_modelInfoPtrs_card = 0u;
        const auto post = apply_CModelInfo_Load(pre);
        if (!op_CModelInfo_Load(pre, post)) return false;
        {
            CModelInfo_ZppContract badPost = post;
            badPost.msNumModelInfos = 0;
            if (op_CModelInfo_Load(pre, badPost)) return false;
        }
        {
            CModelInfo_ZppContract badPost = post;
            badPost.ms_modelInfoPtrs_card = 0u;
            if (op_CModelInfo_Load(pre, badPost)) return false;
        }
    }
    {
        CWeaponModelInfo_ZppContract pre{};
        pre.m_animFileName = 0;
        const auto post = apply_CWeaponModelInfo_SetAnimFile(pre);
        if (!op_CWeaponModelInfo_SetAnimFile(pre, post)) return false;
        {
            CWeaponModelInfo_ZppContract badPost = post;
            badPost.m_animFileName = 0;
            if (op_CWeaponModelInfo_SetAnimFile(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_modelinfo_zpp_invariants(), "Z++ formal contract invariant failed for modelinfo");
} // namespace zpp_modelinfo_spec
#endif // GTANGEL_ZPP_MODELINFO_SPEC_GUARD
