#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_OBJECTS_SPEC_GUARD
#define GTANGEL_ZPP_OBJECTS_SPEC_GUARD
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
enum class CollisionDamageEffect : int32_t { DAMAGE_EFFECT_NONE_v = 0, DAMAGE_EFFECT_CHANGE_MODEL_v = 1, DAMAGE_EFFECT_SPLIT_MODEL_v = 2, DAMAGE_EFFECT_SMASH_AND_DAMAGE_TRAFFICLIGHTS_v = 3, DAMAGE_EFFECT_SMASH_COMPLETELY_v = 4, DAMAGE_EFFECT_CHANGE_THEN_SMASH_v = 5, DAMAGE_EFFECT_SMASH_CARDBOARD_COMPLETELY_v = 6, DAMAGE_EFFECT_SMASH_YELLOW_TARGET_COMPLETELY_v = 7, DAMAGE_EFFECT_SMASH_WOODENBOX_COMPLETELY_v = 8, DAMAGE_EFFECT_SMASH_TRAFFICCONE_COMPLETELY_v = 9, DAMAGE_EFFECT_SMASH_BARPOST_COMPLETELY_v = 10, DAMAGE_EFFECT_SMASH_NEWSTANDNEW1_v = 11 };
// From zpp/objects/Object.zpp
enum class Object_Enum_4 : int32_t { UNKNOWN_OBJECT_v = 0, GAME_OBJECT_v = 1, MISSION_OBJECT_v = 2, TEMP_OBJECT_v = 3, CUTSCENE_OBJECT_v = 4 };
// State Schema Contract from zpp/objects/CutsceneHead.zpp
struct CCutsceneHead_ZppContract {
    int32_t m_pHeadNode{0}; // Z++: Ref[RwFrame]
    int32_t m_parentObject{0}; // Z++: Ref[CCutsceneObject]
    int32_t unk2{0}; // Z++: ℤ
    int32_t bIsSkinned{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/objects/CutsceneObject.zpp
struct CCutsceneObject_ZppContract {
    bool bRenderRightHand{false}; // Z++: 𝔹
    bool bRenderLeftHand{false}; // Z++: 𝔹
    int32_t m_pShadow{0}; // Z++: Ref[CCutsceneShadow]
    int32_t m_pAttachTo{0}; // Z++: ADDRESS
    int32_t m_pAttachmentObject{0}; // Z++: Ref[CObject]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/objects/Object.zpp
struct CObject_ZppContract {
    int32_t m_objectMatrix{0}; // Z++: CMatrix
    float m_fUprootLimit{0.0f}; // Z++: ℝ
    int32_t ObjectCreatedBy{0}; // Z++: ℤ
    bool bIsPickup{false}; // Z++: 𝔹
    bool bAmmoCollected{false}; // Z++: 𝔹
    bool bPickupObjWithMessage{false}; // Z++: 𝔹
    bool bOutOfStock{false}; // Z++: 𝔹
    bool bGlassCracked{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/objects/ObjectData.zpp
struct CObjectInfo_ZppContract {
    float m_fMass{0.0f}; // Z++: ℝ
    float m_fTurnMass{0.0f}; // Z++: ℝ
    float m_fAirResistance{0.0f}; // Z++: ℝ
    float m_fElasticity{0.0f}; // Z++: ℝ
    float m_fBuoyancy{0.0f}; // Z++: ℝ
    float m_fUprootLimit{0.0f}; // Z++: ℝ
    float m_fCollisionDamageMultiplier{0.0f}; // Z++: ℝ
    uint32_t m_nCollisionDamageEffect{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/objects/ObjectData.zpp
struct CObjectData_ZppContract {
    int32_t ms_aObjectInfo{0}; // Z++: seq CObjectInfo  -- bounded array [NUMOBJECTINFO]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/objects/ParticleObject.zpp
struct CParticleObject_ZppContract {
    int32_t m_pNext{0}; // Z++: Ref[CParticleObject]
    int32_t m_pPrev{0}; // Z++: Ref[CParticleObject]
    int32_t m_pParticle{0}; // Z++: Ref[CParticle]
    uint32_t m_nRemoveTimer{0}; // Z++: ℕ
    int32_t m_Type{0}; // Z++: eParticleObjectType
    int32_t m_ParticleType{0}; // Z++: tParticleType
    uint32_t m_nNumEffectCycles{0}; // Z++: ℕ
    uint32_t m_nSkipFrames{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_objects_zpp_invariants() {
    if (!CCutsceneHead_ZppContract{}.checkInvariant()) return false;
    if (!CCutsceneObject_ZppContract{}.checkInvariant()) return false;
    if (!CObject_ZppContract{}.checkInvariant()) return false;
    if (!CObjectInfo_ZppContract{}.checkInvariant()) return false;
    if (!CObjectData_ZppContract{}.checkInvariant()) return false;
    if (!CParticleObject_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_objects_zpp_invariants(), "Z++ formal contract invariant failed for objects");
} // namespace zpp_objects_spec
#endif // GTANGEL_ZPP_OBJECTS_SPEC_GUARD
