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
// State Schema Contract from zpp/vehicles/CarGen.zpp
struct CCarGenerator_ZppContract {
    int32_t m_nModelIndex{0}; // Z++: ℤ
    int32_t m_vecPos{0}; // Z++: CVector
    float m_fAngle{0.0f}; // Z++: ℝ
    int32_t m_nColor1{0}; // Z++: ℤ
    int32_t m_nColor2{0}; // Z++: ℤ
    int64_t m_bForceSpawn{0}; // Z++: ℕ
    int64_t m_nAlarm{0}; // Z++: ℕ
    int64_t m_nDoorlock{0}; // Z++: ℕ
    int32_t m_nMinDelay{0}; // Z++: ℤ
    int32_t m_nMaxDelay{0}; // Z++: ℤ
    int64_t m_nTimer{0}; // Z++: ℕ
    int32_t m_nVehicleHandle{0}; // Z++: ℤ
    int64_t m_nUsesRemaining{0}; // Z++: ℕ
    bool m_bIsBlocking{false}; // Z++: 𝔹
    int32_t m_vecInf{0}; // Z++: CVector
    int32_t m_vecSup{0}; // Z++: CVector
    float m_fSize{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_bForceSpawn >= 0)
            && (m_nAlarm >= 0)
            && (m_nDoorlock >= 0)
            && (m_nTimer >= 0)
            && (m_nUsesRemaining >= 0);
    }
};
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
    int32_t m_vecSpeed{0}; // Z++: CVector
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
// State Schema Contract from zpp/vehicles/Ferry.zpp
struct CFerry_ZppContract {
    int32_t m_nFerryId{0}; // Z++: ℤ
    int64_t m_isFarAway{0}; // Z++: ℕ
    int64_t m_nCurTrackNode{0}; // Z++: ℕ
    float m_fSpeed{0.0f}; // Z++: ℝ
    bool m_bFerryDocked{false}; // Z++: 𝔹
    int64_t m_nDoorTimer{0}; // Z++: ℕ
    int64_t m_nTimeLeftStation{0}; // Z++: ℕ
    int32_t m_nDoorState{0}; // Z++: ℤ
    bool m_bApproachingDock{false}; // Z++: 𝔹
    int64_t m_nSkipFerryStatus{0}; // Z++: ℕ
    int64_t m_nTimeAlongPath{0}; // Z++: ℕ
    bool m_bUseFrontDoor{false}; // Z++: 𝔹
    int32_t m_vecForwardSpeed{0}; // Z++: CVector
    uint8_t m_pDefaultColModel_card{0u}; // Z++: Ref[CColModel]
    int64_t m_nCollision{0}; // Z++: ℕ
    std::array<int32_t, 4> m_doors{}; // Z++: seq CDoor  -- bounded array [4]
    size_t m_doors_len{4u}; // Z++ cardinality #m_doors
    std::array<int32_t, 5> m_aFerryNodes{}; // Z++: seq Ref[RwFrame]  -- bounded array [NUM_FERRY_NODES]
    size_t m_aFerryNodes_len{5u}; // Z++ cardinality #m_aFerryNodes
    bool m_bAlreadyRendered{false}; // Z++: 𝔹
    bool m_bPlayerArrivedHorn{false}; // Z++: 𝔹
    uint8_t mspInst_card{0u}; // Z++: Ref[CFerryInst]
    constexpr bool checkInvariant() const {
        return (m_doors_len == 4u && m_doors.size() == 4u)
            && (m_aFerryNodes_len == 5u && m_aFerryNodes.size() == 5u)
            && (m_isFarAway >= 0)
            && (m_nCurTrackNode >= 0)
            && (m_nDoorTimer >= 0)
            && (m_nTimeLeftStation >= 0)
            && (m_nSkipFerryStatus >= 0)
            && (m_nTimeAlongPath >= 0)
            && (m_pDefaultColModel_card <= 1u)
            && (m_nCollision >= 0)
            && (mspInst_card <= 1u);
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
    int32_t m_dimMin{0}; // Z++: CVector
    float m_numPartialVolumes{0.0f}; // Z++: ℝ
    int32_t m_field_8C{0}; // Z++: ℤ
    int32_t m_field_90{0}; // Z++: ℤ
    int32_t m_field_94{0}; // Z++: ℤ
    bool m_haveVolume{false}; // Z++: 𝔹
    int32_t m_step{0}; // Z++: CVector
    int32_t m_stepRatio{0}; // Z++: CVector
    float m_numSteps{0.0f}; // Z++: ℝ
    bool m_flipAverage{false}; // Z++: 𝔹
    int32_t m_field_B9{0}; // Z++: seq CHAR
    bool m_isBoat{false}; // Z++: 𝔹
    float m_volumeUnderWater{0.0f}; // Z++: ℝ
    int32_t m_impulsePoint{0}; // Z++: CVector
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
    float fSpeedSteer{0.0f}; // Z++: ℝ
    float fSlipSteer{0.0f}; // Z++: ℝ
    float fNoPlayerCOMz{0.0f}; // Z++: ℝ
    float fWheelieAng{0.0f}; // Z++: ℝ
    float fStoppieAng{0.0f}; // Z++: ℝ
    float fWheelieSteer{0.0f}; // Z++: ℝ
    float fWheelieStabMult{0.0f}; // Z++: ℝ
    float fStoppieStabMult{0.0f}; // Z++: ℝ
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
    float fLook_L_R_BehindCamHeight{0.0f}; // Z++: ℝ
    int32_t vecMoveRes{0}; // Z++: CVector
    int32_t vecTurnRes{0}; // Z++: CVector
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
    float fPitch{0.0f}; // Z++: ℝ
    float fPitchStab{0.0f}; // Z++: ℝ
    float fFormLift{0.0f}; // Z++: ℝ
    float fAttackLift{0.0f}; // Z++: ℝ
    float fMoveRes{0.0f}; // Z++: ℝ
    int32_t vecTurnRes{0}; // Z++: CVector
    int32_t vecSpeedRes{0}; // Z++: CVector
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
    bool m_bTempPlane{false}; // Z++: 𝔹
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
// State Schema Contract from zpp/vehicles/Train.zpp
struct CTrain_ZppContract {
    float m_fWagonPosition{0.0f}; // Z++: ℝ
    int32_t m_nWagonId{0}; // Z++: ℤ
    int32_t m_isFarAway{0}; // Z++: ℤ
    int32_t m_nCurTrackNode{0}; // Z++: ℤ
    int32_t m_nWagonGroup{0}; // Z++: ℤ
    float m_fSpeed{0.0f}; // Z++: ℝ
    bool m_bProcessDoor{false}; // Z++: 𝔹
    bool m_bTrainStopping{false}; // Z++: 𝔹
    bool m_bIsFirstWagon{false}; // Z++: 𝔹
    bool m_bIsLastWagon{false}; // Z++: 𝔹
    int64_t m_nTrackId{0}; // Z++: ℕ
    int64_t m_nDoorTimer{0}; // Z++: ℕ
    int32_t m_nDoorState{0}; // Z++: ℤ
    std::array<int32_t, 2> Doors{}; // Z++: seq CTrainDoor  -- bounded array [2]
    size_t Doors_len{2u}; // Z++ cardinality #Doors
    std::array<int32_t, 3> m_aTrainNodes{}; // Z++: seq Ref[RwFrame]  -- bounded array [NUM_TRAIN_NODES]
    size_t m_aTrainNodes_len{3u}; // Z++ cardinality #m_aTrainNodes
    std::array<int32_t, 3> aStationCoors{}; // Z++: seq CVector  -- bounded array [3]
    size_t aStationCoors_len{3u}; // Z++ cardinality #aStationCoors
    std::array<int32_t, 4> aStationCoors_S{}; // Z++: seq CVector  -- bounded array [4]
    size_t aStationCoors_S_len{4u}; // Z++ cardinality #aStationCoors_S
    constexpr bool checkInvariant() const {
        return (Doors_len == 2u && Doors.size() == 2u)
            && (m_aTrainNodes_len == 3u && m_aTrainNodes.size() == 3u)
            && (aStationCoors_len == 3u && aStationCoors.size() == 3u)
            && (aStationCoors_S_len == 4u && aStationCoors_S.size() == 4u)
            && (m_nTrackId >= 0)
            && (m_nDoorTimer >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Transmission.zpp
struct tGear_ZppContract {
    float fMaxVelocity{0.0f}; // Z++: ℝ
    float fShiftUpVelocity{0.0f}; // Z++: ℝ
    float fShiftDownVelocity{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Transmission.zpp
struct cTransmission_ZppContract {
    std::array<int32_t, 6> Gears{}; // Z++: seq tGear  -- bounded array [6]
    size_t Gears_len{6u}; // Z++ cardinality #Gears
    int32_t nDriveType{0}; // Z++: seq CHAR
    int32_t nEngineType{0}; // Z++: seq CHAR
    int32_t nNumberOfGears{0}; // Z++: ℤ
    int64_t Flags{0}; // Z++: ℕ
    float fEngineAcceleration{0.0f}; // Z++: ℝ
    float fEngineInertia{0.0f}; // Z++: ℝ
    float fMaxVelocity{0.0f}; // Z++: ℝ
    float fMaxCruiseVelocity{0.0f}; // Z++: ℝ
    float fMaxReverseVelocity{0.0f}; // Z++: ℝ
    float fCurVelocity{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (Gears_len == 6u && Gears.size() == 6u)
            && (Flags >= 0);
    }
};
// Delta Operation Schema Contract (ΔCCarGenerator) from zpp/vehicles/CarGen.zpp
constexpr bool op_CCarGenerator_SwitchOff(const CCarGenerator_ZppContract& pre, const CCarGenerator_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nUsesRemaining == 0);
}
constexpr CCarGenerator_ZppContract apply_CCarGenerator_SwitchOff(const CCarGenerator_ZppContract& pre) {
    CCarGenerator_ZppContract post = pre;
    post.m_nUsesRemaining = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCCarGenerator) from zpp/vehicles/CarGen.zpp
constexpr bool op_CCarGenerator_SwitchOn(const CCarGenerator_ZppContract& pre, const CCarGenerator_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nUsesRemaining == 1) && (post.m_nTimer == 1);
}
constexpr CCarGenerator_ZppContract apply_CCarGenerator_SwitchOn(const CCarGenerator_ZppContract& pre) {
    CCarGenerator_ZppContract post = pre;
    post.m_nUsesRemaining = 1;
    post.m_nTimer = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCCarGenerator) from zpp/vehicles/CarGen.zpp
constexpr bool op_CCarGenerator_Setup(const CCarGenerator_ZppContract& pre, const CCarGenerator_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_vecPos == 1) && (post.m_fAngle == 1.0f) && (post.m_nModelIndex == 1) && (post.m_nColor1 == 1) && (post.m_nColor2 == 1) && (post.m_bForceSpawn == 1);
}
constexpr CCarGenerator_ZppContract apply_CCarGenerator_Setup(const CCarGenerator_ZppContract& pre) {
    CCarGenerator_ZppContract post = pre;
    post.m_vecPos = 1;
    post.m_fAngle = 1.0f;
    post.m_nModelIndex = 1;
    post.m_nColor1 = 1;
    post.m_nColor2 = 1;
    post.m_bForceSpawn = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_CDamageManager(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_fWheelDamageEffect == 0.65f) && (post.m_bSmashedDoorDoesntClose == false) && (post.field_18 == 1);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_CDamageManager(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_fWheelDamageEffect = 0.65f;
    post.m_bSmashedDoorDoesntClose = false;
    post.field_18 = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_ResetDamageStatus(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_fWheelDamageEffect == 0.0f) && (post.m_engineStatus == 0) && (post.m_lightStatus == 0) && (post.m_panelStatus == 0);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_ResetDamageStatus(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_fWheelDamageEffect = 0.0f;
    post.m_engineStatus = 0;
    post.m_lightStatus = 0;
    post.m_panelStatus = 0;
    return post;
}
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
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_SetPanelStatus(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_panelStatus == 1);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_SetPanelStatus(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_panelStatus = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_SetLightStatus(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_lightStatus == 1);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_SetLightStatus(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_lightStatus = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCDamageManager) from zpp/vehicles/DamageManager.zpp
constexpr bool op_CDamageManager_SetEngineStatus(const CDamageManager_ZppContract& pre, const CDamageManager_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_engineStatus == 1);
}
constexpr CDamageManager_ZppContract apply_CDamageManager_SetEngineStatus(const CDamageManager_ZppContract& pre) {
    CDamageManager_ZppContract post = pre;
    post.m_engineStatus = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCFerry) from zpp/vehicles/Ferry.zpp
constexpr bool op_CFerry_Render(const CFerry_ZppContract& pre, const CFerry_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bAlreadyRendered == true);
}
constexpr CFerry_ZppContract apply_CFerry_Render(const CFerry_ZppContract& pre) {
    CFerry_ZppContract post = pre;
    post.m_bAlreadyRendered = true;
    return post;
}
// Delta Operation Schema Contract (ΔCFerry) from zpp/vehicles/Ferry.zpp
constexpr bool op_CFerry_CloseDoor(const CFerry_ZppContract& pre, const CFerry_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nDoorState == 3) && (post.m_nDoorTimer == 1);
}
constexpr CFerry_ZppContract apply_CFerry_CloseDoor(const CFerry_ZppContract& pre) {
    CFerry_ZppContract post = pre;
    post.m_nDoorState = 3;
    post.m_nDoorTimer = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCFerry) from zpp/vehicles/Ferry.zpp
constexpr bool op_CFerry_CompleteDorrMovement(const CFerry_ZppContract& pre, const CFerry_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nDoorTimer == 0);
}
constexpr CFerry_ZppContract apply_CFerry_CompleteDorrMovement(const CFerry_ZppContract& pre) {
    CFerry_ZppContract post = pre;
    post.m_nDoorTimer = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCFerry) from zpp/vehicles/Ferry.zpp
constexpr bool op_CFerry_SkipFerryToNextDock(const CFerry_ZppContract& pre, const CFerry_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nSkipFerryStatus == 1);
}
constexpr CFerry_ZppContract apply_CFerry_SkipFerryToNextDock(const CFerry_ZppContract& pre) {
    CFerry_ZppContract post = pre;
    post.m_nSkipFerryStatus = 1;
    return post;
}
constexpr bool verify_vehicles_zpp_invariants() {
    {
        CCarGenerator_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCarGenerator_ZppContract bad{};
        bad.m_bForceSpawn = -1;
        if (bad.checkInvariant()) return false;
    }
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
        CFerry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerry_ZppContract bad{};
        bad.m_doors_len = 0u;
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
        CTrain_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrain_ZppContract bad{};
        bad.Doors_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tGear_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tGear_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cTransmission_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cTransmission_ZppContract bad{};
        bad.Gears_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarGenerator_ZppContract pre{};
        pre.m_nUsesRemaining = 1;
        const auto post = apply_CCarGenerator_SwitchOff(pre);
        if (!op_CCarGenerator_SwitchOff(pre, post)) return false;
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nUsesRemaining = 1;
            if (op_CCarGenerator_SwitchOff(pre, badPost)) return false;
        }
    }
    {
        CCarGenerator_ZppContract pre{};
        pre.m_nUsesRemaining = 0;
        pre.m_nTimer = 0;
        const auto post = apply_CCarGenerator_SwitchOn(pre);
        if (!op_CCarGenerator_SwitchOn(pre, post)) return false;
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nUsesRemaining = 0;
            if (op_CCarGenerator_SwitchOn(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nTimer = 0;
            if (op_CCarGenerator_SwitchOn(pre, badPost)) return false;
        }
    }
    {
        CCarGenerator_ZppContract pre{};
        pre.m_vecPos = 0;
        pre.m_fAngle = 0.0f;
        pre.m_nModelIndex = 0;
        pre.m_nColor1 = 0;
        pre.m_nColor2 = 0;
        pre.m_bForceSpawn = 0;
        const auto post = apply_CCarGenerator_Setup(pre);
        if (!op_CCarGenerator_Setup(pre, post)) return false;
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_vecPos = 0;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_fAngle = 0.0f;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nModelIndex = 0;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nColor1 = 0;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_nColor2 = 0;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
        {
            CCarGenerator_ZppContract badPost = post;
            badPost.m_bForceSpawn = 0;
            if (op_CCarGenerator_Setup(pre, badPost)) return false;
        }
    }
    {
        CDamageManager_ZppContract pre{};
        pre.m_fWheelDamageEffect = 0.0f;
        pre.m_bSmashedDoorDoesntClose = true;
        pre.field_18 = 0;
        const auto post = apply_CDamageManager_CDamageManager(pre);
        if (!op_CDamageManager_CDamageManager(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_fWheelDamageEffect = 0.0f;
            if (op_CDamageManager_CDamageManager(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_bSmashedDoorDoesntClose = true;
            if (op_CDamageManager_CDamageManager(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.field_18 = 0;
            if (op_CDamageManager_CDamageManager(pre, badPost)) return false;
        }
    }
    {
        CDamageManager_ZppContract pre{};
        pre.m_fWheelDamageEffect = 1.0f;
        pre.m_engineStatus = 1;
        pre.m_lightStatus = 1;
        pre.m_panelStatus = 1;
        const auto post = apply_CDamageManager_ResetDamageStatus(pre);
        if (!op_CDamageManager_ResetDamageStatus(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_fWheelDamageEffect = 1.0f;
            if (op_CDamageManager_ResetDamageStatus(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_engineStatus = 1;
            if (op_CDamageManager_ResetDamageStatus(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_lightStatus = 1;
            if (op_CDamageManager_ResetDamageStatus(pre, badPost)) return false;
        }
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_panelStatus = 1;
            if (op_CDamageManager_ResetDamageStatus(pre, badPost)) return false;
        }
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
    {
        CDamageManager_ZppContract pre{};
        pre.m_panelStatus = 0;
        const auto post = apply_CDamageManager_SetPanelStatus(pre);
        if (!op_CDamageManager_SetPanelStatus(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_panelStatus = 0;
            if (op_CDamageManager_SetPanelStatus(pre, badPost)) return false;
        }
    }
    {
        CDamageManager_ZppContract pre{};
        pre.m_lightStatus = 0;
        const auto post = apply_CDamageManager_SetLightStatus(pre);
        if (!op_CDamageManager_SetLightStatus(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_lightStatus = 0;
            if (op_CDamageManager_SetLightStatus(pre, badPost)) return false;
        }
    }
    {
        CDamageManager_ZppContract pre{};
        pre.m_engineStatus = 0;
        const auto post = apply_CDamageManager_SetEngineStatus(pre);
        if (!op_CDamageManager_SetEngineStatus(pre, post)) return false;
        {
            CDamageManager_ZppContract badPost = post;
            badPost.m_engineStatus = 0;
            if (op_CDamageManager_SetEngineStatus(pre, badPost)) return false;
        }
    }
    {
        CFerry_ZppContract pre{};
        pre.m_bAlreadyRendered = false;
        const auto post = apply_CFerry_Render(pre);
        if (!op_CFerry_Render(pre, post)) return false;
        {
            CFerry_ZppContract badPost = post;
            badPost.m_bAlreadyRendered = false;
            if (op_CFerry_Render(pre, badPost)) return false;
        }
    }
    {
        CFerry_ZppContract pre{};
        pre.m_nDoorState = 0;
        pre.m_nDoorTimer = 0;
        const auto post = apply_CFerry_CloseDoor(pre);
        if (!op_CFerry_CloseDoor(pre, post)) return false;
        {
            CFerry_ZppContract badPost = post;
            badPost.m_nDoorState = 0;
            if (op_CFerry_CloseDoor(pre, badPost)) return false;
        }
        {
            CFerry_ZppContract badPost = post;
            badPost.m_nDoorTimer = 0;
            if (op_CFerry_CloseDoor(pre, badPost)) return false;
        }
    }
    {
        CFerry_ZppContract pre{};
        pre.m_nDoorTimer = 1;
        const auto post = apply_CFerry_CompleteDorrMovement(pre);
        if (!op_CFerry_CompleteDorrMovement(pre, post)) return false;
        {
            CFerry_ZppContract badPost = post;
            badPost.m_nDoorTimer = 1;
            if (op_CFerry_CompleteDorrMovement(pre, badPost)) return false;
        }
    }
    {
        CFerry_ZppContract pre{};
        pre.m_nSkipFerryStatus = 0;
        const auto post = apply_CFerry_SkipFerryToNextDock(pre);
        if (!op_CFerry_SkipFerryToNextDock(pre, post)) return false;
        {
            CFerry_ZppContract badPost = post;
            badPost.m_nSkipFerryStatus = 0;
            if (op_CFerry_SkipFerryToNextDock(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_vehicles_zpp_invariants(), "Z++ formal contract invariant failed for vehicles");
static_assert(static_cast<int>(eCarNodes::CAR_WHEEL_RF_v) == 1 && static_cast<int>(eCarNodes::NUM_CAR_NODES_v) == 20, "eCarNodes Z++ valuation mismatch");
} // namespace zpp_vehicles_spec
#endif // GTANGEL_ZPP_VEHICLES_SPEC_GUARD
