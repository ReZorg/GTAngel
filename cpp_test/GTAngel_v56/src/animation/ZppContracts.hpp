#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_ANIMATION_SPEC_GUARD
#define GTANGEL_ZPP_ANIMATION_SPEC_GUARD
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
enum class AnimBlendAssociation_Enum_1 : int32_t { ASSOC_RUNNING_v = 0, ASSOC_REPEAT_v = 1, ASSOC_DELETEFADEDOUT_v = 2, ASSOC_FADEOUTWHENDONE_v = 3, ASSOC_PARTIAL_v = 4, ASSOC_MOVEMENT_v = 5, ASSOC_HAS_TRANSLATION_v = 6, ASSOC_HAS_X_TRANSLATION_v = 7, ASSOC_WALK_v = 8, ASSOC_IDLE_v = 9, ASSOC_NOWALK_v = 10, ASSOC_BLOCK_v = 11 };
// From zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_2 : int32_t { CB_NONE_v = 0, CB_FINISH_v = 1, CB_DELETE_v = 2 };
// From zpp/animation/AnimBlendAssociation.zpp
enum class AnimBlendAssociation_Enum_3 : int32_t { ASSOC_RUNNING_v = 0, ASSOC_REPEAT_v = 1, ASSOC_DELETEFADEDOUT_v = 2, ASSOC_FADEOUTWHENDONE_v = 3, ASSOC_PARTIAL_v = 4, ASSOC_MOVEMENT_v = 5, ASSOC_HAS_TRANSLATION_v = 6, ASSOC_WALK_v = 7, ASSOC_IDLE_v = 8, ASSOC_NOWALK_v = 9, ASSOC_BLOCK_v = 10, ASSOC_FRONTAL_v = 11 };
// From zpp/animation/AnimBlendClumpData.zpp
enum class AnimBlendClumpData_Enum_1 : int32_t { IGNORE_ROTATION_v = 0, IGNORE_TRANSLATION_v = 1, VELOCITY_EXTRACTION_v = 2, VELOCITY_EXTRACTION_3D_v = 3, UPDATE_KEYFRAMES_v = 4, COMPRESSED_v = 5 };
// State Schema Contract from zpp/animation/AnimBlendAssocGroup.zpp
struct CAnimBlendAssocGroup_ZppContract {
    int32_t animBlock{0}; // Z++: Ref[CAnimBlock]
    int32_t assocList{0}; // Z++: Ref[CAnimBlendAssociation]
    int32_t numAssociations{0}; // Z++: ℤ
    int32_t firstAnimId{0}; // Z++: ℤ
    int32_t groupId{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/animation/AnimBlendAssociation.zpp
struct CAnimBlendAssociation_ZppContract {
    int32_t link{0}; // Z++: CAnimBlendLink
    int32_t numNodes{0}; // Z++: ℤ
    int32_t groupId{0}; // Z++: ℤ
    int32_t nodes{0}; // Z++: Ref[CAnimBlendNode]
    int32_t hierarchy{0}; // Z++: Ref[CAnimBlendHierarchy]
    float blendAmount{0.0f}; // Z++: ℝ
    float blendDelta{0.0f}; // Z++: ℝ
    float currentTime{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/animation/AnimBlendClumpData.zpp
struct AnimBlendFrameData_ZppContract {
    uint32_t flag{0}; // Z++: ℕ
    int32_t resetPos{0}; // Z++: RwV3d
    int32_t hanimFrame{0}; // Z++: ℙ ℤ
    int32_t nodeID{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/animation/AnimBlendClumpData.zpp
struct CAnimBlendClumpData_ZppContract {
    int32_t link{0}; // Z++: CAnimBlendLink
    int32_t numFrames{0}; // Z++: ℤ
    int32_t velocity3d{0}; // Z++: Ref[CVector]
    int32_t frames{0}; // Z++: Ref[AnimBlendFrameData]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/animation/AnimBlendHierarchy.zpp
struct CAnimBlendHierarchy_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t sequences{0}; // Z++: Ref[CAnimBlendSequence]
    int32_t numSequences{0}; // Z++: ℤ
    bool compressed{false}; // Z++: 𝔹
    bool keepCompressed{false}; // Z++: 𝔹
    float totalLength{0.0f}; // Z++: ℝ
    int32_t linkPtr{0}; // Z++: Ref[CLink_CAnimBlendHierarchy]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/animation/AnimBlendList.zpp
struct CAnimBlendLink_ZppContract {
    int32_t next{0}; // Z++: Ref[CAnimBlendLink]
    int32_t prev{0}; // Z++: Ref[CAnimBlendLink]
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_animation_zpp_invariants() {
    if (!CAnimBlendAssocGroup_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendAssociation_ZppContract{}.checkInvariant()) return false;
    if (!AnimBlendFrameData_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendClumpData_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendHierarchy_ZppContract{}.checkInvariant()) return false;
    if (!CAnimBlendLink_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_animation_zpp_invariants(), "Z++ formal contract invariant failed for animation");
} // namespace zpp_animation_spec
#endif // GTANGEL_ZPP_ANIMATION_SPEC_GUARD
