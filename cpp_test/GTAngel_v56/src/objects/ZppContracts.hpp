#pragma once
#ifndef GTANGEL_ZPP_OBJECTS_SPEC_GUARD
#define GTANGEL_ZPP_OBJECTS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/objects
// Derived from C:/ddd/GTAngel/cpp_test/zpp/objects/*.zpp
// Modules: 8 | Formal Schemas: 160 | Free Types: 7
// ============================================================================
namespace zpp_objects_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 160;
static constexpr int kZppEnumCount   = 7;
// From zpp/objects/Object.zpp
enum class Object_Enum_1 : int32_t { UNKNOWN_OBJECT_v = 0, GAME_OBJECT_v = 1, MISSION_OBJECT_v = 2, TEMP_OBJECT_v = 3, CUTSCENE_OBJECT_v = 4, CONTROLLED_SUB_OBJECT_v = 5 };
// From zpp/objects/Object.zpp
enum class CollisionSpecialResponseCase : int32_t { COLLRESPONSE_NONE_v = 0, COLLRESPONSE_LAMPOST_v = 1, COLLRESPONSE_SMALLBOX_v = 2, COLLRESPONSE_BIGBOX_v = 3, COLLRESPONSE_FENCEPART_v = 4, COLLRESPONSE_UNKNOWN5_v = 5 };
// From zpp/objects/Object.zpp
enum class CollisionDamageEffect : int32_t { DAMAGE_EFFECT_NONE_v = 0, DAMAGE_EFFECT_CHANGE_MODEL_v = 1, DAMAGE_EFFECT_SPLIT_MODEL_v = 2, DAMAGE_EFFECT_SMASH_AND_DAMAGE_TRAFFICLIGHTS_v = 3, DAMAGE_EFFECT_SMASH_COMPLETELY_v = 20, DAMAGE_EFFECT_CHANGE_THEN_SMASH_v = 21, DAMAGE_EFFECT_SMASH_CARDBOARD_COMPLETELY_v = 50, DAMAGE_EFFECT_SMASH_YELLOW_TARGET_COMPLETELY_v = 51, DAMAGE_EFFECT_SMASH_WOODENBOX_COMPLETELY_v = 60, DAMAGE_EFFECT_SMASH_TRAFFICCONE_COMPLETELY_v = 70, DAMAGE_EFFECT_SMASH_BARPOST_COMPLETELY_v = 80, DAMAGE_EFFECT_SMASH_NEWSTANDNEW1_v = 91, DAMAGE_EFFECT_SMASH_NEWSTANDNEW2_v = 92, DAMAGE_EFFECT_SMASH_NEWSTANDNEW3_v = 93, DAMAGE_EFFECT_SMASH_NEWSTANDNEW4_v = 94, DAMAGE_EFFECT_SMASH_NEWSTANDNEW5_v = 95, DAMAGE_EFFECT_SMASH_BLACKBAG_v = 100, DAMAGE_EFFECT_SMASH_VEGPALM_v = 110, DAMAGE_EFFECT_BURST_BEACHBALL_v = 120, DAMAGE_EFFECT_SMASH_BEACHLOUNGE_WOOD_v = 131, DAMAGE_EFFECT_SMASH_BEACHLOUNGE_TOWEL_v = 132 };
// From zpp/objects/Object.zpp
enum class Object_Enum_4 : int32_t { UNKNOWN_OBJECT_v = 0, GAME_OBJECT_v = 1, MISSION_OBJECT_v = 2, TEMP_OBJECT_v = 3, CUTSCENE_OBJECT_v = 4 };
// State Schema Contract from zpp/objects/CutsceneHead.zpp
struct CCutsceneHead_ZppContract {
    uint8_t m_pHeadNode_card{0u}; // Z++: Ref[RwFrame]
    uint8_t m_parentObject_card{0u}; // Z++: Ref[CCutsceneObject]
    int32_t unk2{0}; // Z++: ℤ
    int32_t bIsSkinned{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pHeadNode_card <= 1u)
            && (m_parentObject_card <= 1u);
    }
};
// State Schema Contract from zpp/objects/CutsceneObject.zpp
struct CCutsceneObject_ZppContract {
    bool bRenderRightHand{false}; // Z++: 𝔹
    bool bRenderLeftHand{false}; // Z++: 𝔹
    uint8_t m_pShadow_card{0u}; // Z++: Ref[CCutsceneShadow]
    int32_t m_pAttachTo{0}; // Z++: ADDRESS
    uint8_t m_pAttachmentObject_card{0u}; // Z++: Ref[CObject]
    constexpr bool checkInvariant() const {
        return (m_pShadow_card <= 1u)
            && (m_pAttachmentObject_card <= 1u);
    }
};
// State Schema Contract from zpp/objects/ParticleObject.zpp
struct CAudioHydrant_ZppContract {
    int32_t AudioEntity{0}; // Z++: ℤ
    uint8_t pParticleObject_card{0u}; // Z++: Ref[CParticleObject]
    constexpr bool checkInvariant() const {
        return (pParticleObject_card <= 1u);
    }
};
// State Schema Contract from zpp/objects/Stinger.zpp
struct CStinger_ZppContract {
    bool bIsDeployed{false}; // Z++: 𝔹
    int64_t m_nTimeOfDeploy{0}; // Z++: ℕ
    int32_t m_vPos{0}; // Z++: CVector
    float m_fMax_Z{0.0f}; // Z++: ℝ
    float m_fMin_Z{0.0f}; // Z++: ℝ
    std::array<int32_t, 60> m_vPositions{}; // Z++: seq CVector2D  -- bounded array [60]
    size_t m_vPositions_len{60u}; // Z++ cardinality #m_vPositions
    int32_t pSpikes{0}; // Z++: seq Ref[CStingerSegment]  -- bounded array [NUM_STINGER_SEGMENTS]
    int64_t m_nSpikeState{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nTimeOfDeploy >= 0)
            && (m_vPositions_len == 60u && m_vPositions.size() == 60u)
            && (m_nSpikeState >= 0);
    }
};
constexpr bool verify_objects_zpp_invariants() {
    if (!CCutsceneHead_ZppContract{}.checkInvariant()) return false;
    if (!CCutsceneObject_ZppContract{}.checkInvariant()) return false;
    if (!CAudioHydrant_ZppContract{}.checkInvariant()) return false;
    if (!CStinger_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_objects_zpp_invariants(), "Z++ formal contract invariant failed for objects");
} // namespace zpp_objects_spec
#endif // GTANGEL_ZPP_OBJECTS_SPEC_GUARD
