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
// State Schema Contract from zpp/animation/AnimBlendSequence.zpp
struct KeyFrame_ZppContract {
    int32_t rotation{0}; // Z++: CQuaternion
    float deltaTime{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/animation/AnimBlendSequence.zpp
struct KeyFrameTrans_ZppContract {
    int32_t translation{0}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/animation/AnimBlendSequence.zpp
struct KeyFrameCompressed_ZppContract {
    std::array<int32_t, 4> rot{}; // Z++: seq ℤ  -- bounded array [4]
    size_t rot_len{4u}; // Z++ cardinality #rot
    int32_t deltaTime{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (rot_len == 4u && rot.size() == 4u);
    }
};
// State Schema Contract from zpp/animation/AnimBlendSequence.zpp
struct KeyFrameTransCompressed_ZppContract {
    std::array<int32_t, 3> trans{}; // Z++: seq ℤ  -- bounded array [3]
    size_t trans_len{3u}; // Z++ cardinality #trans
    constexpr bool checkInvariant() const {
        return (trans_len == 3u && trans.size() == 3u);
    }
};
// State Schema Contract from zpp/animation/AnimManager.zpp
struct CAnimBlock_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    bool isLoaded{false}; // Z++: 𝔹
    int32_t refCount{0}; // Z++: ℤ
    int32_t firstIndex{0}; // Z++: ℤ
    int32_t numAnims{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/animation/AnimManager.zpp
struct AnimAssocDesc_ZppContract {
    int32_t animId{0}; // Z++: ℤ
    int32_t flags{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/animation/AnimManager.zpp
struct AnimAssocDefinition_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t blockName{0}; // Z++: seq CHAR
    int32_t modelIndex{0}; // Z++: ℤ
    int32_t numAnims{0}; // Z++: ℤ
    int32_t animNames{0}; // Z++: seq CHAR
    uint8_t animDescs_card{0u}; // Z++: Ref[AnimAssocDesc]
    constexpr bool checkInvariant() const {
        return (animDescs_card <= 1u);
    }
};
// State Schema Contract from zpp/animation/AnimationId.zpp
struct AnimationIdModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{343}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 343);
    }
};
// State Schema Contract from zpp/animation/Bones.zpp
struct BonesModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{118}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 118);
    }
};
// State Schema Contract from zpp/animation/CutsceneMgr.zpp
struct sToHideItem_ZppContract {
    float z{0.0f}; // Z++: ℝ
    int32_t name{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Delta Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CAnimBlendAssocGroup(const CAnimBlendAssocGroup_ZppContract& pre, const CAnimBlendAssocGroup_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.animBlock_card == 0u);
}
constexpr CAnimBlendAssocGroup_ZppContract apply_CAnimBlendAssocGroup_CAnimBlendAssocGroup(const CAnimBlendAssocGroup_ZppContract& pre) {
    CAnimBlendAssocGroup_ZppContract post = pre;
    post.animBlock_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_DestroyAssociations(const CAnimBlendAssocGroup_ZppContract& pre, const CAnimBlendAssocGroup_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.assocList_card == 0u);
}
constexpr CAnimBlendAssocGroup_ZppContract apply_CAnimBlendAssocGroup_DestroyAssociations(const CAnimBlendAssocGroup_ZppContract& pre) {
    CAnimBlendAssocGroup_ZppContract post = pre;
    post.assocList_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CreateAssociations(const CAnimBlendAssocGroup_ZppContract& pre, const CAnimBlendAssocGroup_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.numAssociations == 0);
}
constexpr CAnimBlendAssocGroup_ZppContract apply_CAnimBlendAssocGroup_CreateAssociations(const CAnimBlendAssocGroup_ZppContract& pre) {
    CAnimBlendAssocGroup_ZppContract post = pre;
    post.numAssociations = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CreateAssociations_2(const CAnimBlendAssocGroup_ZppContract& pre, const CAnimBlendAssocGroup_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.numAssociations == 0);
}
constexpr CAnimBlendAssocGroup_ZppContract apply_CAnimBlendAssocGroup_CreateAssociations_2(const CAnimBlendAssocGroup_ZppContract& pre) {
    CAnimBlendAssocGroup_ZppContract post = pre;
    post.numAssociations = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCAnimBlendAssocGroup) from zpp/animation/AnimBlendAssocGroup.zpp
constexpr bool op_CAnimBlendAssocGroup_CreateAssociations_3(const CAnimBlendAssocGroup_ZppContract& pre, const CAnimBlendAssocGroup_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.numAssociations == 0);
}
constexpr CAnimBlendAssocGroup_ZppContract apply_CAnimBlendAssocGroup_CreateAssociations_3(const CAnimBlendAssocGroup_ZppContract& pre) {
    CAnimBlendAssocGroup_ZppContract post = pre;
    post.numAssociations = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCAnimBlendAssociation) from zpp/animation/AnimBlendAssociation.zpp
constexpr bool op_CAnimBlendAssociation_CAnimBlendAssociation(const CAnimBlendAssociation_ZppContract& pre, const CAnimBlendAssociation_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.nodes_card == 0u);
}
constexpr CAnimBlendAssociation_ZppContract apply_CAnimBlendAssociation_CAnimBlendAssociation(const CAnimBlendAssociation_ZppContract& pre) {
    CAnimBlendAssociation_ZppContract post = pre;
    post.nodes_card = 0u;
    return post;
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
        KeyFrame_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        KeyFrame_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        KeyFrameTrans_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        KeyFrameTrans_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        KeyFrameCompressed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        KeyFrameCompressed_ZppContract bad{};
        bad.rot_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        KeyFrameTransCompressed_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        KeyFrameTransCompressed_ZppContract bad{};
        bad.trans_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimBlock_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        AnimAssocDesc_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AnimAssocDesc_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        AnimAssocDefinition_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AnimAssocDefinition_ZppContract bad{};
        bad.animDescs_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        AnimationIdModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AnimationIdModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        BonesModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        BonesModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        sToHideItem_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        sToHideItem_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract pre{};
        pre.animBlock_card = 1u;
        const auto post = apply_CAnimBlendAssocGroup_CAnimBlendAssocGroup(pre);
        if (!op_CAnimBlendAssocGroup_CAnimBlendAssocGroup(pre, post)) return false;
        CAnimBlendAssocGroup_ZppContract badPost = post;
        badPost.animBlock_card = 1u;
        if (op_CAnimBlendAssocGroup_CAnimBlendAssocGroup(pre, badPost)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract pre{};
        pre.assocList_card = 1u;
        const auto post = apply_CAnimBlendAssocGroup_DestroyAssociations(pre);
        if (!op_CAnimBlendAssocGroup_DestroyAssociations(pre, post)) return false;
        CAnimBlendAssocGroup_ZppContract badPost = post;
        badPost.assocList_card = 1u;
        if (op_CAnimBlendAssocGroup_DestroyAssociations(pre, badPost)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract pre{};
        pre.numAssociations = 1;
        const auto post = apply_CAnimBlendAssocGroup_CreateAssociations(pre);
        if (!op_CAnimBlendAssocGroup_CreateAssociations(pre, post)) return false;
        CAnimBlendAssocGroup_ZppContract badPost = post;
        badPost.numAssociations = 1;
        if (op_CAnimBlendAssocGroup_CreateAssociations(pre, badPost)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract pre{};
        pre.numAssociations = 1;
        const auto post = apply_CAnimBlendAssocGroup_CreateAssociations_2(pre);
        if (!op_CAnimBlendAssocGroup_CreateAssociations_2(pre, post)) return false;
        CAnimBlendAssocGroup_ZppContract badPost = post;
        badPost.numAssociations = 1;
        if (op_CAnimBlendAssocGroup_CreateAssociations_2(pre, badPost)) return false;
    }
    {
        CAnimBlendAssocGroup_ZppContract pre{};
        pre.numAssociations = 1;
        const auto post = apply_CAnimBlendAssocGroup_CreateAssociations_3(pre);
        if (!op_CAnimBlendAssocGroup_CreateAssociations_3(pre, post)) return false;
        CAnimBlendAssocGroup_ZppContract badPost = post;
        badPost.numAssociations = 1;
        if (op_CAnimBlendAssocGroup_CreateAssociations_3(pre, badPost)) return false;
    }
    {
        CAnimBlendAssociation_ZppContract pre{};
        pre.nodes_card = 1u;
        const auto post = apply_CAnimBlendAssociation_CAnimBlendAssociation(pre);
        if (!op_CAnimBlendAssociation_CAnimBlendAssociation(pre, post)) return false;
        CAnimBlendAssociation_ZppContract badPost = post;
        badPost.nodes_card = 1u;
        if (op_CAnimBlendAssociation_CAnimBlendAssociation(pre, badPost)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_animation_zpp_invariants(), "Z++ formal contract invariant failed for animation");
} // namespace zpp_animation_spec
#endif // GTANGEL_ZPP_ANIMATION_SPEC_GUARD
