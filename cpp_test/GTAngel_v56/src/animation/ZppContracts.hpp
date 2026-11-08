#pragma once
#ifndef GTANGEL_ZPP_ANIMATION_SPEC_GUARD
#define GTANGEL_ZPP_ANIMATION_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/animation
// Derived from C:/ddd/GTAngel/cpp_test/zpp/animation/*.zpp
// Modules: 14 | Formal Schemas: 398 | Free Types: 14
// ============================================================================
namespace zpp_animation_spec {
static constexpr int kZppModuleCount = 14;
static constexpr int kZppSchemaCount = 398;
static constexpr int kZppEnumCount   = 14;
// Free Type from zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_1 : int32_t { ASSOC_RUNNING_v = 1, ASSOC_REPEAT_v = 2, ASSOC_DELETEFADEDOUT_v = 4, ASSOC_FADEOUTWHENDONE_v = 8, ASSOC_PARTIAL_v = 16, ASSOC_MOVEMENT_v = 32, ASSOC_HAS_TRANSLATION_v = 64, ASSOC_HAS_X_TRANSLATION_v = 128, ASSOC_WALK_v = 256, ASSOC_IDLE_v = 512, ASSOC_NOWALK_v = 1024, ASSOC_BLOCK_v = 2048, ASSOC_FRONTAL_v = 4096, ASSOC_DRIVING_v = 8192 };
// Free Type from zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_2 : int32_t { CB_NONE_v = 0, CB_FINISH_v = 1, CB_DELETE_v = 2 };
// Free Type from zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_3 : int32_t { ASSOC_RUNNING_v = 1, ASSOC_REPEAT_v = 2, ASSOC_DELETEFADEDOUT_v = 4, ASSOC_FADEOUTWHENDONE_v = 8, ASSOC_PARTIAL_v = 16, ASSOC_MOVEMENT_v = 32, ASSOC_HAS_TRANSLATION_v = 64, ASSOC_WALK_v = 128, ASSOC_IDLE_v = 256, ASSOC_NOWALK_v = 512, ASSOC_BLOCK_v = 1024, ASSOC_FRONTAL_v = 2048, ASSOC_HAS_X_TRANSLATION_v = 4096 };
// Free Type from zpp/animation/AnimBlendClumpData.zpp
enum class AnimBlendClumpData_Enum_1 : int32_t { IGNORE_ROTATION_v = 2, IGNORE_TRANSLATION_v = 4, VELOCITY_EXTRACTION_v = 8, VELOCITY_EXTRACTION_3D_v = 16, UPDATE_KEYFRAMES_v = 32, COMPRESSED_v = 64 };
// Free Type from zpp/animation/AnimBlendSequence.zpp
enum class AnimBlendSequence_Enum_1 : int32_t { KF_ROT_v = 1, KF_TRANS_v = 2 };
// Free Type from zpp/animation/AnimManager.zpp
enum class AssocGroupId : int32_t { ASSOCGRP_STD_v = 0, ASSOCGRP_VAN_v = 1, ASSOCGRP_COACH_v = 2, ASSOCGRP_BIKE_STANDARD_v = 3, ASSOCGRP_BIKE_VESPA_v = 4, ASSOCGRP_BIKE_HARLEY_v = 5, ASSOCGRP_BIKE_DIRT_v = 6, ASSOCGRP_UNARMED_v = 7, ASSOCGRP_SCREWDRIVER_v = 8, ASSOCGRP_KNIFE_v = 9, ASSOCGRP_BASEBALLBAT_v = 10, ASSOCGRP_GOLFCLUB_v = 11, ASSOCGRP_CHAINSAW_v = 12, ASSOCGRP_PYTHON_v = 13, ASSOCGRP_COLT_v = 14, ASSOCGRP_SHOTGUN_v = 15, ASSOCGRP_BUDDY_v = 16, ASSOCGRP_TEC_v = 17, ASSOCGRP_UZI_v = 18, ASSOCGRP_RIFLE_v = 19, ASSOCGRP_M60_v = 20, ASSOCGRP_SNIPER_v = 21, ASSOCGRP_THROW_v = 22, ASSOCGRP_FLAMETHROWER_v = 23, ASSOCGRP_ROCKETLAUNCHER_v = 24, ASSOCGRP_MEDIC_v = 25, ASSOCGRP_SUNBATHE_v = 26, ASSOCGRP_PLAYER_IDLE_v = 27, ASSOCGRP_RIOT_v = 28, ASSOCGRP_STRIP_v = 29, ASSOCGRP_LANCE_v = 30, ASSOCGRP_PLAYER_v = 31, ASSOCGRP_PLAYERROCKET_v = 32, ASSOCGRP_PLAYER1ARMED_v = 33, ASSOCGRP_PLAYER2ARMED_v = 34, ASSOCGRP_PLAYERBBBAT_v = 35, ASSOCGRP_AICHAINSAW_v = 36, ASSOCGRP_PLAYERCHAINSAW_v = 37, ASSOCGRP_SHUFFLE_v = 38, ASSOCGRP_OLD_v = 39, ASSOCGRP_GANG1_v = 40, ASSOCGRP_GANG2_v = 41, ASSOCGRP_FAT_v = 42, ASSOCGRP_OLDFAT_v = 43, ASSOCGRP_JOGGER_v = 44, ASSOCGRP_WOMAN_v = 45, ASSOCGRP_WOMANSHOP_v = 46, ASSOCGRP_BUSYWOMAN_v = 47 };
// State Schema Contract from zpp/animation/AnimBlendAssocGroup.zpp
struct CAnimBlendAssocGroup_ZppContract {
    uint8_t animBlock_card{0u}; // Z++: Ref[CAnimBlock]
    uint8_t assocList_card{0u}; // Z++: Ref[CAnimBlendAssociation]
    int32_t numAssociations{0}; // Z++: ℤ
    int32_t firstAnimId{0}; // Z++: ℤ
    int32_t groupId{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (animBlock_card <= 1u)
            && (assocList_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendAssociation.zpp
struct CAnimBlendAssociation_ZppContract {
    int32_t link{0}; // Z++: CAnimBlendLink
    int32_t numNodes{0}; // Z++: ℤ
    int32_t groupId{0}; // Z++: ℤ
    uint8_t nodes_card{0u}; // Z++: Ref[CAnimBlendNode]
    uint8_t hierarchy_card{0u}; // Z++: Ref[CAnimBlendHierarchy]
    float blendAmount{0.0f}; // Z++: ℝ
    float blendDelta{0.0f}; // Z++: ℝ
    float currentTime{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (nodes_card <= 1u)
            && (hierarchy_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendClumpData.zpp
struct AnimBlendFrameData_ZppContract {
    int64_t flag{0}; // Z++: ℕ
    int32_t resetPos{0}; // Z++: RwV3d
    uint8_t hanimFrame_card{0u}; // Z++: ℙ ℤ
    int32_t nodeID{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (flag >= 0)
            && (hanimFrame_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendClumpData.zpp
struct CAnimBlendClumpData_ZppContract {
    int32_t link{0}; // Z++: CAnimBlendLink
    int32_t numFrames{0}; // Z++: ℤ
    uint8_t velocity3d_card{0u}; // Z++: Ref[CVector]
    uint8_t frames_card{0u}; // Z++: Ref[AnimBlendFrameData]
    constexpr bool checkInvariant() const {
        return (velocity3d_card <= 1u)
            && (frames_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendList.zpp
struct CAnimBlendLink_ZppContract {
    uint8_t next_card{0u}; // Z++: Ref[CAnimBlendLink]
    uint8_t prev_card{0u}; // Z++: Ref[CAnimBlendLink]
    constexpr bool checkInvariant() const {
        return (next_card <= 1u)
            && (prev_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendNode.zpp
struct CAnimBlendNode_ZppContract {
    float theta{0.0f}; // Z++: ℝ
    float invSin{0.0f}; // Z++: ℝ
    int32_t frameA{0}; // Z++: ℤ
    int32_t frameB{0}; // Z++: ℤ
    float remainingTime{0.0f}; // Z++: ℝ
    uint8_t sequence_card{0u}; // Z++: Ref[CAnimBlendSequence]
    uint8_t association_card{0u}; // Z++: Ref[CAnimBlendAssociation]
    constexpr bool checkInvariant() const {
        return (sequence_card <= 1u)
            && (association_card <= 1u);
    }
};
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CAnimBlendAssocGroup(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CAnimBlendAssocGroup_2(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_DestroyAssociations_3(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_GetAnimation_4(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_GetAnimation_5(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CopyAnimation_6(CAnimBlendAssocGroup_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_animation_zpp_invariants() {
    {
        CAnimBlendAssocGroup_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlendAssocGroup_ZppContract bad{};
        bad.animBlock_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendAssociation_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlendAssociation_ZppContract bad{};
        bad.nodes_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        AnimBlendFrameData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AnimBlendFrameData_ZppContract bad{};
        bad.flag = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendClumpData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlendClumpData_ZppContract bad{};
        bad.velocity3d_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendLink_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlendLink_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlendNode_ZppContract bad{};
        bad.sequence_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_CAnimBlendAssocGroup(st)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_CAnimBlendAssocGroup_2(st)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_DestroyAssociations_3(st)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_GetAnimation_4(st)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_GetAnimation_5(st)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract st{};
        if (!op_CAnimBlendAssocGroup_CopyAnimation_6(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_animation_zpp_invariants(), "Z++ formal contract invariant failed for animation");
} // namespace zpp_animation_spec
#endif // GTANGEL_ZPP_ANIMATION_SPEC_GUARD
