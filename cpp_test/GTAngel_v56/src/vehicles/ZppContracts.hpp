#pragma once
#ifndef GTANGEL_ZPP_VEHICLES_SPEC_GUARD
#define GTANGEL_ZPP_VEHICLES_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/vehicles
// Derived from C:/ddd/GTAngel/cpp_test/zpp/vehicles/*.zpp
// Modules: 15 | Formal Schemas: 700 | Free Types: 54
// ============================================================================
namespace zpp_vehicles_spec {
static constexpr int kZppModuleCount = 15;
static constexpr int kZppSchemaCount = 700;
static constexpr int kZppEnumCount   = 54;
// Free Type from zpp/vehicles/Automobile.zpp
enum class Automobile_Enum_1 : int32_t { CARWHEEL_FRONT_LEFT_v = 0, CARWHEEL_REAR_LEFT_v = 1, CARWHEEL_FRONT_RIGHT_v = 2, CARWHEEL_REAR_RIGHT_v = 3 };
// Free Type from zpp/vehicles/Automobile.zpp
enum class eCarNodes : int32_t { CAR_WHEEL_RF_v = 1, CAR_WHEEL_RM_v = 2, CAR_WHEEL_RB_v = 3, CAR_WHEEL_LF_v = 4, CAR_WHEEL_LM_v = 5, CAR_WHEEL_LB_v = 6, CAR_BUMP_FRONT_v = 7, CAR_BUMP_REAR_v = 8, CAR_WING_RF_v = 9, CAR_WING_RR_v = 10, CAR_DOOR_RF_v = 11, CAR_DOOR_RR_v = 12, CAR_WING_LF_v = 13, CAR_WING_LR_v = 14, CAR_DOOR_LF_v = 15, CAR_DOOR_LR_v = 16, CAR_BONNET_v = 17, CAR_BOOT_v = 18, CAR_WINDSCREEN_v = 19, NUM_CAR_NODES_v = 20 };
// Free Type from zpp/vehicles/Automobile.zpp
enum class eBombType : int32_t { CARBOMB_NONE_v = 0, CARBOMB_TIMED_v = 1, CARBOMB_ONIGNITION_v = 2, CARBOMB_REMOTE_v = 3, CARBOMB_TIMEDACTIVE_v = 4, CARBOMB_ONIGNITIONACTIVE_v = 5 };
// Free Type from zpp/vehicles/Automobile.zpp
enum class Automobile_Enum_4 : int32_t { CAR_DOOR_FLAG_UNKNOWN_v = 0, CAR_DOOR_FLAG_LF_v = 1, CAR_DOOR_FLAG_LR_v = 2, CAR_DOOR_FLAG_RF_v = 4, CAR_DOOR_FLAG_RR_v = 8 };
// Free Type from zpp/vehicles/Bike.zpp
enum class eBikeNodes : int32_t { BIKE_NODE_NONE_v = 0, BIKE_CHASSIS_v = 1, BIKE_FORKS_FRONT_v = 2, BIKE_FORKS_REAR_v = 3, BIKE_WHEEL_FRONT_v = 4, BIKE_WHEEL_REAR_v = 5, BIKE_MUDGUARD_v = 6, BIKE_HANDLEBARS_v = 7, BIKE_NUM_NODES_v = 8 };
// Free Type from zpp/vehicles/Bike.zpp
enum class Bike_Enum_2 : int32_t { BIKEWHEEL_FRONT_v = 0, BIKEWHEEL_REAR_v = 1 };
// State Schema Contract from zpp/vehicles/DamageManager.zpp
struct CDamageManager_ZppContract {
    float m_fWheelDamageEffect{0.0f}; // Z++: ℝ
    bool m_bSmashedDoorDoesntClose{false}; // Z++: 𝔹
    int64_t m_engineStatus{0}; // Z++: ℕ
    std::array<uint32_t, 4> m_wheelStatus{}; // Z++: seq ℕ  -- bounded array [4]
    size_t m_wheelStatus_len{4u}; // Z++ cardinality #m_wheelStatus
    std::array<uint32_t, 6> m_doorStatus{}; // Z++: seq ℕ  -- bounded array [6]
    size_t m_doorStatus_len{6u}; // Z++ cardinality #m_doorStatus
    int64_t m_lightStatus{0}; // Z++: ℕ
    int64_t m_panelStatus{0}; // Z++: ℕ
    int64_t field_18{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_wheelStatus_len == 4u && m_wheelStatus.size() == 4u)
            && (m_doorStatus_len == 6u && m_doorStatus.size() == 6u)
            && (m_engineStatus >= 0)
            && (m_lightStatus >= 0)
            && (m_panelStatus >= 0)
            && (field_18 >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Door.zpp
struct CDoor_ZppContract {
    float m_fMaxAngle{0.0f}; // Z++: ℝ
    float m_fMinAngle{0.0f}; // Z++: ℝ
    int32_t m_nDirn{0}; // Z++: ℤ
    int32_t m_nAxis{0}; // Z++: ℤ
    int32_t m_nDoorState{0}; // Z++: ℤ
    float m_fAngle{0.0f}; // Z++: ℝ
    float m_fPrevAngle{0.0f}; // Z++: ℝ
    float m_fAngVel{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Door.zpp
struct CTrainDoor_ZppContract {
    float m_fClosedPosn{0.0f}; // Z++: ℝ
    float m_fOpenPosn{0.0f}; // Z++: ℝ
    int32_t m_nDirn{0}; // Z++: ℤ
    int32_t m_nDoorState{0}; // Z++: ℤ
    int32_t m_nAxis{0}; // Z++: ℤ
    float m_fPosn{0.0f}; // Z++: ℝ
    float m_fPrevPosn{0.0f}; // Z++: ℝ
    int32_t field_14{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Door.zpp
struct CFerryDoor_ZppContract {
    float m_fClosedPosn{0.0f}; // Z++: ℝ
    float m_fOpenPosn{0.0f}; // Z++: ℝ
    int32_t m_nDirn{0}; // Z++: ℤ
    int32_t m_nAxis{0}; // Z++: ℤ
    int32_t m_nDoorState{0}; // Z++: ℤ
    float m_fPosn{0.0f}; // Z++: ℝ
    float m_fPrevPosn{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Ferry.zpp
struct CFerryNode_ZppContract {
    float x{0.0f}; // Z++: ℝ
    float y{0.0f}; // Z++: ℝ
    float z{0.0f}; // Z++: ℝ
    float t{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Ferry.zpp
struct CFerryInterpolationLine_ZppContract {
    int64_t type{0}; // Z++: ℕ
    float time{0.0f}; // Z++: ℝ
    float position{0.0f}; // Z++: ℝ
    float speed{0.0f}; // Z++: ℝ
    float acceleration{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (type >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Ferry.zpp
struct CFerryPath_ZppContract {
    float TotalLengthOfTrack{0.0f}; // Z++: ℝ
    float TotalDurationOfTrack{0.0f}; // Z++: ℝ
    int32_t NumTrackNodes{0}; // Z++: ℤ
    uint8_t aTrackNodes_card{0u}; // Z++: Ref[CFerryNode]
    uint8_t aLineBits_card{0u}; // Z++: ℙ ℤ
    constexpr bool checkInvariant() const {
        return (aTrackNodes_card <= 1u)
            && (aLineBits_card <= 1u);
    }
};
// State Schema Contract from zpp/vehicles/Floater.zpp
struct cBuoyancy_ZppContract {
    int32_t m_position{0}; // Z++: CVector
    int32_t m_matrix{0}; // Z++: CMatrix
    int32_t m_field_54{0}; // Z++: ℤ
    int32_t m_positionZ{0}; // Z++: CVector
    float m_waterlevel{0.0f}; // Z++: ℝ
    float m_waterLevelInc{0.0f}; // Z++: ℝ
    float m_buoyancy{0.0f}; // Z++: ℝ
    int32_t m_dimMax{0}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/HandlingMgr.zpp
struct tBikeHandlingData_ZppContract {
    int32_t nIdentifier{0}; // Z++: tVehicleType
    float fLeanFwdCOM{0.0f}; // Z++: ℝ
    float fLeanFwdForce{0.0f}; // Z++: ℝ
    float fLeanBakCOM{0.0f}; // Z++: ℝ
    float fLeanBackForce{0.0f}; // Z++: ℝ
    float fMaxLean{0.0f}; // Z++: ℝ
    float fFullAnimLean{0.0f}; // Z++: ℝ
    float fDesLean{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/HandlingMgr.zpp
struct tBoatHandlingData_ZppContract {
    int32_t nIdentifier{0}; // Z++: tVehicleType
    float fThrustY{0.0f}; // Z++: ℝ
    float fThrustZ{0.0f}; // Z++: ℝ
    float fThrustAppZ{0.0f}; // Z++: ℝ
    float fAqPlaneForce{0.0f}; // Z++: ℝ
    float fAqPlaneLimit{0.0f}; // Z++: ℝ
    float fAqPlaneOffset{0.0f}; // Z++: ℝ
    float fWaveAudioMult{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/HandlingMgr.zpp
struct tFlyingHandlingData_ZppContract {
    int32_t nIdentifier{0}; // Z++: tVehicleType
    float fThrust{0.0f}; // Z++: ℝ
    float fThrustFallOff{0.0f}; // Z++: ℝ
    float fYaw{0.0f}; // Z++: ℝ
    float fYawStab{0.0f}; // Z++: ℝ
    float fSideSlip{0.0f}; // Z++: ℝ
    float fRoll{0.0f}; // Z++: ℝ
    float fRollStab{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Plane.zpp
struct CPlaneNode_ZppContract {
    int32_t p{0}; // Z++: CVector
    float t{0.0f}; // Z++: ℝ
    bool bOnGround{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Plane.zpp
struct CPlaneInterpolationLine_ZppContract {
    int64_t type{0}; // Z++: ℕ
    float time{0.0f}; // Z++: ℝ
    float position{0.0f}; // Z++: ℝ
    float speed{0.0f}; // Z++: ℝ
    float acceleration{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (type >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Plane.zpp
struct CPlane_ZppContract {
    float m_fRotorRotation{0.0f}; // Z++: ℝ
    int32_t m_isFarAway{0}; // Z++: ℤ
    int32_t m_nCurPathNode{0}; // Z++: ℤ
    float m_fSpeed{0.0f}; // Z++: ℝ
    int64_t m_nFrameWhenHit{0}; // Z++: ℕ
    bool m_bHasBeenHit{false}; // Z++: 𝔹
    bool m_bIsDrugRunCesna{false}; // Z++: 𝔹
    bool m_bIsDropOffCesna{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nFrameWhenHit >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Train.zpp
struct CTrainNode_ZppContract {
    int32_t p{0}; // Z++: CVector
    float t{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Train.zpp
struct CTrainInterpolationLine_ZppContract {
    int64_t type{0}; // Z++: ℕ
    float time{0.0f}; // Z++: ℝ
    float position{0.0f}; // Z++: ℝ
    float speed{0.0f}; // Z++: ℝ
    float acceleration{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (type >= 0);
    }
};
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_FuckCarCompletely(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_lightStatus == 0) && (post.m_panelStatus == 0);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_FuckCarCompletely(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_lightStatus = 0;
    post.m_panelStatus = 0;
    return post;
}
constexpr bool verify_vehicles_zpp_invariants() {
    {
        CDamageManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDamageManager_ZppContract bad{};
        bad.m_wheelStatus_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CDoor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDoor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTrainDoor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrainDoor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFerryDoor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerryDoor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFerryNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerryNode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFerryInterpolationLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerryInterpolationLine_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFerryPath_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerryPath_ZppContract bad{};
        bad.aTrackNodes_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cBuoyancy_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cBuoyancy_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tBikeHandlingData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBikeHandlingData_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tBoatHandlingData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBoatHandlingData_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tFlyingHandlingData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tFlyingHandlingData_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneNode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneInterpolationLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneInterpolationLine_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlane_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlane_ZppContract bad{};
        bad.m_nFrameWhenHit = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTrainNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrainNode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTrainInterpolationLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrainInterpolationLine_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CDamageManager_ZppContract pre{};
        pre.m_lightStatus = 1;
        pre.m_panelStatus = 1;
        const auto post = apply_CDamageManager_FuckCarCompletely(pre);
        if (!op_CDamageManager_FuckCarCompletely(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_lightStatus = 1;
            if (op_CDamageManager_FuckCarCompletely(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_panelStatus = 1;
            if (op_CDamageManager_FuckCarCompletely(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_vehicles_zpp_invariants(), "Z++ formal contract invariant failed for vehicles");
static_assert(static_cast<int>(eCarNodes::CAR_WHEEL_RF_v) == 1 && static_cast<int>(eCarNodes::NUM_CAR_NODES_v) == 20, "eCarNodes Z++ valuation mismatch");
} // namespace zpp_vehicles_spec
#endif // GTANGEL_ZPP_VEHICLES_SPEC_GUARD
