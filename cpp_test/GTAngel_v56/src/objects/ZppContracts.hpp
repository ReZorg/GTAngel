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
// Free Type from zpp/objects/Object.zpp
enum class Object_Enum_1 : int32_t { UNKNOWN_OBJECT_v = 0, GAME_OBJECT_v = 1, MISSION_OBJECT_v = 2, TEMP_OBJECT_v = 3, CUTSCENE_OBJECT_v = 4, CONTROLLED_SUB_OBJECT_v = 5 };
// Free Type from zpp/objects/Object.zpp
enum class CollisionSpecialResponseCase : int32_t { COLLRESPONSE_NONE_v = 0, COLLRESPONSE_LAMPOST_v = 1, COLLRESPONSE_SMALLBOX_v = 2, COLLRESPONSE_BIGBOX_v = 3, COLLRESPONSE_FENCEPART_v = 4, COLLRESPONSE_UNKNOWN5_v = 5 };
// Free Type from zpp/objects/Object.zpp
enum class CollisionDamageEffect : int32_t { DAMAGE_EFFECT_NONE_v = 0, DAMAGE_EFFECT_CHANGE_MODEL_v = 1, DAMAGE_EFFECT_SPLIT_MODEL_v = 2, DAMAGE_EFFECT_SMASH_AND_DAMAGE_TRAFFICLIGHTS_v = 3, DAMAGE_EFFECT_SMASH_COMPLETELY_v = 20, DAMAGE_EFFECT_CHANGE_THEN_SMASH_v = 21, DAMAGE_EFFECT_SMASH_CARDBOARD_COMPLETELY_v = 50, DAMAGE_EFFECT_SMASH_YELLOW_TARGET_COMPLETELY_v = 51, DAMAGE_EFFECT_SMASH_WOODENBOX_COMPLETELY_v = 60, DAMAGE_EFFECT_SMASH_TRAFFICCONE_COMPLETELY_v = 70, DAMAGE_EFFECT_SMASH_BARPOST_COMPLETELY_v = 80, DAMAGE_EFFECT_SMASH_NEWSTANDNEW1_v = 91, DAMAGE_EFFECT_SMASH_NEWSTANDNEW2_v = 92, DAMAGE_EFFECT_SMASH_NEWSTANDNEW3_v = 93, DAMAGE_EFFECT_SMASH_NEWSTANDNEW4_v = 94, DAMAGE_EFFECT_SMASH_NEWSTANDNEW5_v = 95, DAMAGE_EFFECT_SMASH_BLACKBAG_v = 100, DAMAGE_EFFECT_SMASH_VEGPALM_v = 110, DAMAGE_EFFECT_BURST_BEACHBALL_v = 120, DAMAGE_EFFECT_SMASH_BEACHLOUNGE_WOOD_v = 131, DAMAGE_EFFECT_SMASH_BEACHLOUNGE_TOWEL_v = 132 };
// Free Type from zpp/objects/Object.zpp
enum class Object_Enum_4 : int32_t { UNKNOWN_OBJECT_v = 0, GAME_OBJECT_v = 1, MISSION_OBJECT_v = 2, TEMP_OBJECT_v = 3, CUTSCENE_OBJECT_v = 4 };
// Free Type from zpp/objects/ParticleObject.zpp
enum class eParticleObjectType : int32_t { POBJECT_PAVEMENT_STEAM_v = 0, POBJECT_PAVEMENT_STEAM_SLOWMOTION_v = 1, POBJECT_WALL_STEAM_v = 2, POBJECT_WALL_STEAM_SLOWMOTION_v = 3, POBJECT_DARK_SMOKE_v = 4, POBJECT_FIRE_HYDRANT_v = 5, POBJECT_FIRE_HYDRANT_STEAM_v = 6, POBJECT_CAR_WATER_SPLASH_v = 7, POBJECT_PED_WATER_SPLASH_v = 8, POBJECT_SPLASHES_AROUND_v = 9, POBJECT_SMALL_FIRE_v = 10, POBJECT_BIG_FIRE_v = 11, POBJECT_DRY_ICE_v = 12, POBJECT_DRY_ICE_SLOWMOTION_v = 13, POBJECT_WATER_FOUNTAIN_VERT_v = 14, POBJECT_WATER_FOUNTAIN_HORIZ_v = 15, POBJECT_FIRE_TRAIL_v = 16, POBJECT_SMOKE_TRAIL_v = 17, POBJECT_FIREBALL_AND_SMOKE_v = 18, POBJECT_ROCKET_TRAIL_v = 19, POBJECT_EXPLOSION_ONCE_v = 20, POBJECT_CATALINAS_GUNFLASH_v = 21, POBJECT_CATALINAS_SHOTGUNFLASH_v = 22 };
// Free Type from zpp/objects/ParticleObject.zpp
enum class eParticleObjectState : int32_t { POBJECTSTATE_INITIALISED_v = 0, POBJECTSTATE_UPDATE_CLOSE_v = 1, POBJECTSTATE_UPDATE_FAR_v = 2, POBJECTSTATE_FREE_v = 3 };
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
// State Schema Contract from zpp/objects/ObjectData.zpp
struct CObjectInfo_ZppContract {
    float m_fMass{0.0f}; // Z++: ℝ
    float m_fTurnMass{0.0f}; // Z++: ℝ
    float m_fAirResistance{0.0f}; // Z++: ℝ
    float m_fElasticity{0.0f}; // Z++: ℝ
    float m_fBuoyancy{0.0f}; // Z++: ℝ
    float m_fUprootLimit{0.0f}; // Z++: ℝ
    float m_fCollisionDamageMultiplier{0.0f}; // Z++: ℝ
    int64_t m_nCollisionDamageEffect{0}; // Z++: ℕ
    int64_t m_nSpecialCollisionResponseCases{0}; // Z++: ℕ
    bool m_bCameraToAvoidThisObject{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nCollisionDamageEffect >= 0)
            && (m_nSpecialCollisionResponseCases >= 0);
    }
};
// State Schema Contract from zpp/objects/ObjectData.zpp
struct CObjectData_ZppContract {
    std::array<int32_t, 210> ms_aObjectInfo{}; // Z++: seq CObjectInfo  -- bounded array [NUMOBJECTINFO]
    size_t ms_aObjectInfo_len{210u}; // Z++ cardinality #ms_aObjectInfo
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (ms_aObjectInfo_len == 210u && ms_aObjectInfo.size() == 210u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/objects/ParticleObject.zpp
struct CParticleObject_ZppContract {
    uint8_t m_pNext_card{0u}; // Z++: Ref[CParticleObject]
    uint8_t m_pPrev_card{0u}; // Z++: Ref[CParticleObject]
    uint8_t m_pParticle_card{0u}; // Z++: Ref[CParticle]
    int64_t m_nRemoveTimer{0}; // Z++: ℕ
    int32_t m_Type{0}; // Z++: eParticleObjectType
    int32_t m_ParticleType{0}; // Z++: tParticleType
    int64_t m_nNumEffectCycles{0}; // Z++: ℕ
    int64_t m_nSkipFrames{0}; // Z++: ℕ
    int64_t m_nFrameCounter{0}; // Z++: ℕ
    int64_t m_nState{0}; // Z++: ℕ
    int32_t m_vecTarget{0}; // Z++: CVector
    float m_fRandVal{0.0f}; // Z++: ℝ
    float m_fSize{0.0f}; // Z++: ℝ
    int32_t m_Color{0}; // Z++: CRGBA
    int64_t m_bRemove{0}; // Z++: ℕ
    int32_t m_nCreationChance{0}; // Z++: ℤ
    uint8_t pCloseListHead_card{0u}; // Z++: Ref[CParticleObject]
    uint8_t pFarListHead_card{0u}; // Z++: Ref[CParticleObject]
    uint8_t pUnusedListHead_card{0u}; // Z++: Ref[CParticleObject]
    constexpr bool checkInvariant() const {
        return (m_pNext_card <= 1u)
            && (m_pPrev_card <= 1u)
            && (m_pParticle_card <= 1u)
            && (m_nRemoveTimer >= 0)
            && (m_nNumEffectCycles >= 0)
            && (m_nSkipFrames >= 0)
            && (m_nFrameCounter >= 0)
            && (m_nState >= 0)
            && (m_bRemove >= 0)
            && (pCloseListHead_card <= 1u)
            && (pFarListHead_card <= 1u)
            && (pUnusedListHead_card <= 1u);
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
    std::array<int32_t, 12> pSpikes{}; // Z++: seq Ref[CStingerSegment]  -- bounded array [NUM_STINGER_SEGMENTS]
    size_t pSpikes_len{12u}; // Z++ cardinality #pSpikes
    int64_t m_nSpikeState{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_vPositions_len == 60u && m_vPositions.size() == 60u)
            && (pSpikes_len == 12u && pSpikes.size() == 12u)
            && (m_nTimeOfDeploy >= 0)
            && (m_nSpikeState >= 0);
    }
};
// Delta Operation Schema Contract (ΔCCutsceneObject) from zpp/objects/CutsceneObject.zpp
constexpr bool op_CCutsceneObject_CCutsceneObject_2(const CCutsceneObject_ZppContract& pre, const CCutsceneObject_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pShadow_card == 0u);
}
constexpr CCutsceneObject_ZppContract apply_CCutsceneObject_CCutsceneObject_2(const CCutsceneObject_ZppContract& pre) {
    CCutsceneObject_ZppContract post = pre;
    post.m_pShadow_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCAudioHydrant) from zpp/objects/ParticleObject.zpp
constexpr bool op_CAudioHydrant_Add(const CAudioHydrant_ZppContract& pre, const CAudioHydrant_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.AudioEntity == 1) && (post.pParticleObject_card == 1u);
}
constexpr CAudioHydrant_ZppContract apply_CAudioHydrant_Add(const CAudioHydrant_ZppContract& pre) {
    CAudioHydrant_ZppContract post = pre;
    post.AudioEntity = 1;
    post.pParticleObject_card = 1u;
    return post;
}
// Delta Operation Schema Contract (ΔCAudioHydrant) from zpp/objects/ParticleObject.zpp
constexpr bool op_CAudioHydrant_Remove(const CAudioHydrant_ZppContract& pre, const CAudioHydrant_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.AudioEntity == 1) && (post.pParticleObject_card == 0u);
}
constexpr CAudioHydrant_ZppContract apply_CAudioHydrant_Remove(const CAudioHydrant_ZppContract& pre) {
    CAudioHydrant_ZppContract post = pre;
    post.AudioEntity = 1;
    post.pParticleObject_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCParticleObject) from zpp/objects/ParticleObject.zpp
constexpr bool op_CParticleObject_Initialise(const CParticleObject_ZppContract& pre, const CParticleObject_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pCloseListHead_card == 0u) && (post.pFarListHead_card == 0u) && (post.pUnusedListHead_card == 1u) && (post.m_pPrev_card == 1u) && (post.m_pNext_card == 0u);
}
constexpr CParticleObject_ZppContract apply_CParticleObject_Initialise(const CParticleObject_ZppContract& pre) {
    CParticleObject_ZppContract post = pre;
    post.pCloseListHead_card = 0u;
    post.pFarListHead_card = 0u;
    post.pUnusedListHead_card = 1u;
    post.m_pPrev_card = 1u;
    post.m_pNext_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCParticleObject) from zpp/objects/ParticleObject.zpp
constexpr bool op_CParticleObject_RemoveObject(const CParticleObject_ZppContract& pre, const CParticleObject_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nState == 3);
}
constexpr CParticleObject_ZppContract apply_CParticleObject_RemoveObject(const CParticleObject_ZppContract& pre) {
    CParticleObject_ZppContract post = pre;
    post.m_nState = 3;
    return post;
}
// Delta Operation Schema Contract (ΔCParticleObject) from zpp/objects/ParticleObject.zpp
constexpr bool op_CParticleObject_RemoveAllParticleObjects(const CParticleObject_ZppContract& pre, const CParticleObject_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pUnusedListHead_card == 1u) && (post.pCloseListHead_card == 0u) && (post.pFarListHead_card == 0u) && (post.m_pPrev_card == 1u) && (post.m_pNext_card == 0u);
}
constexpr CParticleObject_ZppContract apply_CParticleObject_RemoveAllParticleObjects(const CParticleObject_ZppContract& pre) {
    CParticleObject_ZppContract post = pre;
    post.pUnusedListHead_card = 1u;
    post.pCloseListHead_card = 0u;
    post.pFarListHead_card = 0u;
    post.m_pPrev_card = 1u;
    post.m_pNext_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCStinger) from zpp/objects/Stinger.zpp
constexpr bool op_CStinger_CStinger(const CStinger_ZppContract& pre, const CStinger_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bIsDeployed == false);
}
constexpr CStinger_ZppContract apply_CStinger_CStinger(const CStinger_ZppContract& pre) {
    CStinger_ZppContract post = pre;
    post.bIsDeployed = false;
    return post;
}
constexpr bool verify_objects_zpp_invariants() {
    {
        CCutsceneHead_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCutsceneHead_ZppContract bad{};
        bad.m_pHeadNode_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCutsceneObject_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCutsceneObject_ZppContract bad{};
        bad.m_pShadow_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CObjectInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CObjectInfo_ZppContract bad{};
        bad.m_nCollisionDamageEffect = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CObjectData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CObjectData_ZppContract bad{};
        bad.ms_aObjectInfo_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CParticleObject_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CParticleObject_ZppContract bad{};
        bad.m_pNext_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAudioHydrant_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAudioHydrant_ZppContract bad{};
        bad.pParticleObject_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStinger_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStinger_ZppContract bad{};
        bad.m_vPositions_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCutsceneObject_ZppContract pre{};
        pre.m_pShadow_card = 1u;
        const auto post = apply_CCutsceneObject_CCutsceneObject_2(pre);
        if (!op_CCutsceneObject_CCutsceneObject_2(pre, post)) return false;
        {
            CCutsceneObject_ZppContract badPost = post;
            badPost.m_pShadow_card = 1u;
            if (op_CCutsceneObject_CCutsceneObject_2(pre, badPost)) return false;
        }
    }
    {
        CAudioHydrant_ZppContract pre{};
        pre.AudioEntity = 0;
        pre.pParticleObject_card = 0u;
        const auto post = apply_CAudioHydrant_Add(pre);
        if (!op_CAudioHydrant_Add(pre, post)) return false;
        {
            CAudioHydrant_ZppContract badPost = post;
            badPost.AudioEntity = 0;
            if (op_CAudioHydrant_Add(pre, badPost)) return false;
        }
        {
            CAudioHydrant_ZppContract badPost = post;
            badPost.pParticleObject_card = 0u;
            if (op_CAudioHydrant_Add(pre, badPost)) return false;
        }
    }
    {
        CAudioHydrant_ZppContract pre{};
        pre.AudioEntity = 0;
        pre.pParticleObject_card = 1u;
        const auto post = apply_CAudioHydrant_Remove(pre);
        if (!op_CAudioHydrant_Remove(pre, post)) return false;
        {
            CAudioHydrant_ZppContract badPost = post;
            badPost.AudioEntity = 0;
            if (op_CAudioHydrant_Remove(pre, badPost)) return false;
        }
        {
            CAudioHydrant_ZppContract badPost = post;
            badPost.pParticleObject_card = 1u;
            if (op_CAudioHydrant_Remove(pre, badPost)) return false;
        }
    }
    {
        CParticleObject_ZppContract pre{};
        pre.pCloseListHead_card = 1u;
        pre.pFarListHead_card = 1u;
        pre.pUnusedListHead_card = 0u;
        pre.m_pPrev_card = 0u;
        pre.m_pNext_card = 1u;
        const auto post = apply_CParticleObject_Initialise(pre);
        if (!op_CParticleObject_Initialise(pre, post)) return false;
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pCloseListHead_card = 1u;
            if (op_CParticleObject_Initialise(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pFarListHead_card = 1u;
            if (op_CParticleObject_Initialise(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pUnusedListHead_card = 0u;
            if (op_CParticleObject_Initialise(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.m_pPrev_card = 0u;
            if (op_CParticleObject_Initialise(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.m_pNext_card = 1u;
            if (op_CParticleObject_Initialise(pre, badPost)) return false;
        }
    }
    {
        CParticleObject_ZppContract pre{};
        pre.m_nState = 0;
        const auto post = apply_CParticleObject_RemoveObject(pre);
        if (!op_CParticleObject_RemoveObject(pre, post)) return false;
        {
            CParticleObject_ZppContract badPost = post;
            badPost.m_nState = 0;
            if (op_CParticleObject_RemoveObject(pre, badPost)) return false;
        }
    }
    {
        CParticleObject_ZppContract pre{};
        pre.pUnusedListHead_card = 0u;
        pre.pCloseListHead_card = 1u;
        pre.pFarListHead_card = 1u;
        pre.m_pPrev_card = 0u;
        pre.m_pNext_card = 1u;
        const auto post = apply_CParticleObject_RemoveAllParticleObjects(pre);
        if (!op_CParticleObject_RemoveAllParticleObjects(pre, post)) return false;
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pUnusedListHead_card = 0u;
            if (op_CParticleObject_RemoveAllParticleObjects(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pCloseListHead_card = 1u;
            if (op_CParticleObject_RemoveAllParticleObjects(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.pFarListHead_card = 1u;
            if (op_CParticleObject_RemoveAllParticleObjects(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.m_pPrev_card = 0u;
            if (op_CParticleObject_RemoveAllParticleObjects(pre, badPost)) return false;
        }
        {
            CParticleObject_ZppContract badPost = post;
            badPost.m_pNext_card = 1u;
            if (op_CParticleObject_RemoveAllParticleObjects(pre, badPost)) return false;
        }
    }
    {
        CStinger_ZppContract pre{};
        pre.bIsDeployed = true;
        const auto post = apply_CStinger_CStinger(pre);
        if (!op_CStinger_CStinger(pre, post)) return false;
        {
            CStinger_ZppContract badPost = post;
            badPost.bIsDeployed = true;
            if (op_CStinger_CStinger(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_objects_zpp_invariants(), "Z++ formal contract invariant failed for objects");
} // namespace zpp_objects_spec
#endif // GTANGEL_ZPP_OBJECTS_SPEC_GUARD
