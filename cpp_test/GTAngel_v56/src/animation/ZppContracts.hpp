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
// From zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_1 : int32_t { ASSOC_RUNNING_v = 1, ASSOC_REPEAT_v = 2, ASSOC_DELETEFADEDOUT_v = 4, ASSOC_FADEOUTWHENDONE_v = 8, ASSOC_PARTIAL_v = 16, ASSOC_MOVEMENT_v = 32, ASSOC_HAS_TRANSLATION_v = 64, ASSOC_HAS_X_TRANSLATION_v = 128, ASSOC_WALK_v = 256, ASSOC_IDLE_v = 512, ASSOC_NOWALK_v = 1024, ASSOC_BLOCK_v = 2048, ASSOC_FRONTAL_v = 4096, ASSOC_DRIVING_v = 8192 };
// From zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_2 : int32_t { CB_NONE_v = 0, CB_FINISH_v = 1, CB_DELETE_v = 2 };
// From zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_3 : int32_t { ASSOC_RUNNING_v = 1, ASSOC_REPEAT_v = 2, ASSOC_DELETEFADEDOUT_v = 4, ASSOC_FADEOUTWHENDONE_v = 8, ASSOC_PARTIAL_v = 16, ASSOC_MOVEMENT_v = 32, ASSOC_HAS_TRANSLATION_v = 64, ASSOC_WALK_v = 128, ASSOC_IDLE_v = 256, ASSOC_NOWALK_v = 512, ASSOC_BLOCK_v = 1024, ASSOC_FRONTAL_v = 2048, ASSOC_HAS_X_TRANSLATION_v = 4096 };
// From zpp/animation/AnimBlendClumpData.zpp
enum class AnimBlendClumpData_Enum_1 : int32_t { IGNORE_ROTATION_v = 2, IGNORE_TRANSLATION_v = 4, VELOCITY_EXTRACTION_v = 8, VELOCITY_EXTRACTION_3D_v = 16, UPDATE_KEYFRAMES_v = 32, COMPRESSED_v = 64 };
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
constexpr bool verify_animation_zpp_invariants() {
    if (!CAnimBlendAssocGroup_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendAssociation_ZppContract{}.checkInvariant()) return false;
    if (!AnimBlendFrameData_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendClumpData_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendLink_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendNode_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_animation_zpp_invariants(), "Z++ formal contract invariant failed for animation");
} // namespace zpp_animation_spec
#endif // GTANGEL_ZPP_ANIMATION_SPEC_GUARD
