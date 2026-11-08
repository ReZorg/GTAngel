#pragma once
#ifndef GTANGEL_ZPP_CONTROL_SPEC_GUARD
#define GTANGEL_ZPP_CONTROL_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/control
// Derived from C:/ddd/GTAngel/cpp_test/zpp/control/*.zpp
// Modules: 34 | Formal Schemas: 1230 | Free Types: 66
// ============================================================================
namespace zpp_control_spec {
static constexpr int kZppModuleCount = 34;
static constexpr int kZppSchemaCount = 1230;
static constexpr int kZppEnumCount   = 66;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/control/AutoPilot.zpp
enum class eCarMission : int32_t { MISSION_NONE_v = 0, MISSION_CRUISE_v = 1, MISSION_RAMPLAYER_FARAWAY_v = 2, MISSION_RAMPLAYER_CLOSE_v = 3, MISSION_BLOCKPLAYER_FARAWAY_v = 4, MISSION_BLOCKPLAYER_CLOSE_v = 5, MISSION_BLOCKPLAYER_HANDBRAKESTOP_v = 6, MISSION_WAITFORDELETION_v = 7, MISSION_GOTOCOORDS_v = 8, MISSION_GOTOCOORDS_STRAIGHT_v = 9, MISSION_EMERGENCYVEHICLE_STOP_v = 10, MISSION_STOP_FOREVER_v = 11, MISSION_GOTOCOORDS_ACCURATE_v = 12, MISSION_GOTO_COORDS_STRAIGHT_ACCURATE_v = 13, MISSION_GOTOCOORDS_ASTHECROWSWIMS_v = 14, MISSION_RAMCAR_FARAWAY_v = 15, MISSION_RAMCAR_CLOSE_v = 16, MISSION_BLOCKCAR_FARAWAY_v = 17, MISSION_BLOCKCAR_CLOSE_v = 18, MISSION_BLOCKCAR_HANDBRAKESTOP_v = 19, MISSION_HELI_FLYTOCOORS_v = 20, MISSION_ATTACKPLAYER_v = 21, MISSION_PLANE_FLYTOCOORS_v = 22, MISSION_HELI_LAND_v = 23, MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1_v = 24, MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2_v = 25, MISSION_BLOCKPLAYER_FORWARDANDBACK_v = 26 };
// Free Type from zpp/control/AutoPilot.zpp
enum class eCarTempAction : int32_t { TEMPACT_NONE_v = 0, TEMPACT_WAIT_v = 1, TEMPACT_REVERSE_v = 2, TEMPACT_HANDBRAKETURNLEFT_v = 3, TEMPACT_HANDBRAKETURNRIGHT_v = 4, TEMPACT_HANDBRAKESTRAIGHT_v = 5, TEMPACT_TURNLEFT_v = 6, TEMPACT_TURNRIGHT_v = 7, TEMPACT_GOFORWARD_v = 8, TEMPACT_SWERVELEFT_v = 9, TEMPACT_SWERVERIGHT_v = 10 };
// Free Type from zpp/control/AutoPilot.zpp
enum class eCarDrivingStyle : int32_t { DRIVINGSTYLE_STOP_FOR_CARS_v = 0, DRIVINGSTYLE_SLOW_DOWN_FOR_CARS_v = 1, DRIVINGSTYLE_AVOID_CARS_v = 2, DRIVINGSTYLE_PLOUGH_THROUGH_v = 3, DRIVINGSTYLE_STOP_FOR_CARS_IGNORE_LIGHTS_v = 4 };
// Free Type from zpp/control/Bridge.zpp
enum class bridgeStates : int32_t { STATE_BRIDGE_LOCKED_v = 0, STATE_LIFT_PART_IS_UP_v = 1, STATE_LIFT_PART_MOVING_DOWN_v = 2, STATE_LIFT_PART_IS_DOWN_v = 3, STATE_LIFT_PART_ABOUT_TO_MOVE_UP_v = 4, STATE_LIFT_PART_MOVING_UP_v = 5, STATE_BRIDGE_ALWAYS_UNLOCKED_v = 6 };
// Free Type from zpp/control/CarCtrl.zpp
enum class CarCtrl_Enum_1 : int32_t { MAX_CARS_TO_KEEP_v = 2, MAX_CAR_MODELS_IN_ARRAY_v = 25 };
// Free Type from zpp/control/CarCtrl.zpp
enum class CarCtrl_Enum_2 : int32_t { TOTAL_MULTIPLAYER_CAR_BANKS_v = 3, CARS_IN_MULTIPLAYER_BANK_v = 8 };
// State Schema Contract from zpp/control/Bridge.zpp
struct CBridge_ZppContract {
    uint8_t pWeight_card{0u}; // Z++: Ref[CEntity_pLiftRoad__pLiftPart]
    int32_t OldState{0}; // Z++: ℤ
    float DefaultZLiftWeight{0.0f}; // Z++: ℝ
    float OldLift{0.0f}; // Z++: ℝ
    int64_t TimeOfBridgeBecomingOperational{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (pWeight_card <= 1u)
            && (TimeOfBridgeBecomingOperational >= 0);
    }
};
// State Schema Contract from zpp/control/CarAI.zpp
struct CCarAI_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/CarCtrl.zpp
struct CCarCtrl_ZppContract {
    int32_t NumLawEnforcerCars{0}; // Z++: ℤ
    int32_t NumAmbulancesOnDuty{0}; // Z++: ℤ
    int32_t NumFiretrucksOnDuty{0}; // Z++: ℤ
    int32_t NumRandomCars{0}; // Z++: ℤ
    int32_t NumMissionCars{0}; // Z++: ℤ
    int32_t NumParkedCars{0}; // Z++: ℤ
    int32_t NumPermanentCars{0}; // Z++: ℤ
    bool bCarsGeneratedAroundCamera{false}; // Z++: 𝔹
    float CarDensityMultiplier{0.0f}; // Z++: ℝ
    int32_t CountDownToCarsAtStart{0}; // Z++: ℤ
    int32_t MaxNumberOfCarsInUse{0}; // Z++: ℤ
    int64_t LastTimeLawEnforcerCreated{0}; // Z++: ℕ
    int64_t LastTimeFireTruckCreated{0}; // Z++: ℕ
    int64_t LastTimeAmbulanceCreated{0}; // Z++: ℕ
    std::array<int32_t, 28> TotalNumOfCarsOfRating{}; // Z++: seq ℤ  -- bounded array [TOTAL_CUSTOM_CLASSES]
    size_t TotalNumOfCarsOfRating_len{28u}; // Z++ cardinality #TotalNumOfCarsOfRating
    int32_t MiamiViceCycle{0}; // Z++: ℤ
    int64_t LastTimeMiamiViceGenerated{0}; // Z++: ℕ
    std::array<int32_t, 28> NumRequestsOfCarRating{}; // Z++: seq ℤ  -- bounded array [TOTAL_CUSTOM_CLASSES]
    size_t NumRequestsOfCarRating_len{28u}; // Z++ cardinality #NumRequestsOfCarRating
    std::array<int32_t, 28> NumOfLoadedCarsOfRating{}; // Z++: seq ℤ  -- bounded array [TOTAL_CUSTOM_CLASSES]
    size_t NumOfLoadedCarsOfRating_len{28u}; // Z++ cardinality #NumOfLoadedCarsOfRating
    bool scriptControlsMpCarLimit{false}; // Z++: 𝔹
    int32_t angle{0}; // Z++: return
    std::array<int32_t, 28> NextCarOfRating{}; // Z++: seq ℤ  -- bounded array [TOTAL_CUSTOM_CLASSES]
    size_t NextCarOfRating_len{28u}; // Z++ cardinality #NextCarOfRating
    constexpr bool checkInvariant() const {
        return (TotalNumOfCarsOfRating_len == 28u && TotalNumOfCarsOfRating.size() == 28u)
            && (NumRequestsOfCarRating_len == 28u && NumRequestsOfCarRating.size() == 28u)
            && (NumOfLoadedCarsOfRating_len == 28u && NumOfLoadedCarsOfRating.size() == 28u)
            && (NextCarOfRating_len == 28u && NextCarOfRating.size() == 28u)
            && (LastTimeLawEnforcerCreated >= 0)
            && (LastTimeFireTruckCreated >= 0)
            && (LastTimeAmbulanceCreated >= 0)
            && (LastTimeMiamiViceGenerated >= 0);
    }
};
// State Schema Contract from zpp/control/Curves.zpp
struct CCurves_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Garages.zpp
struct CStoredCar_ZppContract {
    int32_t m_nModelIndex{0}; // Z++: ℤ
    float m_fPosX{0.0f}; // Z++: ℝ
    float m_fPosY{0.0f}; // Z++: ℝ
    float m_fPosZ{0.0f}; // Z++: ℝ
    float m_fForwardX{0.0f}; // Z++: ℝ
    float m_fForwardY{0.0f}; // Z++: ℝ
    float m_fForwardZ{0.0f}; // Z++: ℝ
    float m_fTractionMultiplier{0.0f}; // Z++: ℝ
    int32_t m_nFlags{0}; // Z++: ℤ
    int32_t m_nPrimaryColor{0}; // Z++: ℤ
    int32_t m_nSecondaryColor{0}; // Z++: ℤ
    int32_t m_nRadioStation{0}; // Z++: ℤ
    int32_t m_nVariationA{0}; // Z++: ℤ
    int32_t m_nVariationB{0}; // Z++: ℤ
    int32_t m_nCarBombType{0}; // Z++: ℤ
    ZppVec3 m_vecPos{}; // Z++: CVector
    ZppVec3 m_vecAngle{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Garages.zpp
struct CGarage_ZppContract {
    int64_t m_eGarageType{0}; // Z++: ℕ
    int64_t m_eGarageState{0}; // Z++: ℕ
    int64_t m_nMaxStoredCars{0}; // Z++: ℕ
    bool field_2{false}; // Z++: 𝔹
    bool m_bClosingWithoutTargetCar{false}; // Z++: 𝔹
    bool m_bDeactivated{false}; // Z++: 𝔹
    bool m_bResprayHappened{false}; // Z++: 𝔹
    int32_t m_nTargetModelIndex{0}; // Z++: ℤ
    uint8_t m_pDoor1_card{0u}; // Z++: Ref[CEntity]
    uint8_t m_pDoor2_card{0u}; // Z++: Ref[CEntity]
    int64_t m_bDoor1PoolIndex{0}; // Z++: ℕ
    int64_t m_bDoor2PoolIndex{0}; // Z++: ℕ
    bool m_bDoor1IsDummy{false}; // Z++: 𝔹
    bool m_bDoor2IsDummy{false}; // Z++: 𝔹
    bool m_bRecreateDoorOnNextRefresh{false}; // Z++: 𝔹
    bool m_bRotatedDoor{false}; // Z++: 𝔹
    bool m_bCameraFollowsPlayer{false}; // Z++: 𝔹
    int32_t m_vecCorner1{0}; // Z++: CVector2D
    float m_fInfZ{0.0f}; // Z++: ℝ
    int32_t m_vDir1{0}; // Z++: CVector2D
    int32_t m_vDir2{0}; // Z++: CVector2D
    float m_fSupZ{0.0f}; // Z++: ℝ
    ZppVec3 m_vecSSGaragePos{}; // Z++: CVector
    float m_fSSGarageAngle{0.0f}; // Z++: ℝ
    float m_fDir1Len{0.0f}; // Z++: ℝ
    float m_fDir2Len{0.0f}; // Z++: ℝ
    float m_fInfX{0.0f}; // Z++: ℝ
    float m_fSupX{0.0f}; // Z++: ℝ
    float m_fInfY{0.0f}; // Z++: ℝ
    float m_fSupY{0.0f}; // Z++: ℝ
    int64_t m_nTimeCrusherCraneActivated{0}; // Z++: ℕ
    uint8_t m_pSSTargetCar_card{0u}; // Z++: Ref[CVehicle]
    float m_fDoorPos{0.0f}; // Z++: ℝ
    float m_fDoorHeight{0.0f}; // Z++: ℝ
    float m_fDoor1X{0.0f}; // Z++: ℝ
    float m_fDoor1Y{0.0f}; // Z++: ℝ
    float m_fDoor2X{0.0f}; // Z++: ℝ
    float m_fDoor2Y{0.0f}; // Z++: ℝ
    float m_fDoor1Z{0.0f}; // Z++: ℝ
    float m_fDoor2Z{0.0f}; // Z++: ℝ
    int64_t m_nTimeToStartAction{0}; // Z++: ℕ
    int64_t m_bCollectedCarsState{0}; // Z++: ℕ
    uint8_t m_pTarget_card{0u}; // Z++: Ref[CVehicle]
    int32_t m_sStoredCar{0}; // Z++: CStoredCar
    bool m_bInitialized{false}; // Z++: 𝔹
    bool m_bLocked{false}; // Z++: 𝔹
    bool m_nSSGarageState{false}; // Z++: 𝔹
    bool m_bSSGarageStateChanging{false}; // Z++: 𝔹
    float m_fX2{0.0f}; // Z++: ℝ
    float m_fY1{0.0f}; // Z++: ℝ
    float m_fY2{0.0f}; // Z++: ℝ
    float m_fZ1{0.0f}; // Z++: ℝ
    float m_fZ2{0.0f}; // Z++: ℝ
    int64_t field_96{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (m_eGarageType >= 0)
            && (m_eGarageState >= 0)
            && (m_nMaxStoredCars >= 0)
            && (m_pDoor1_card <= 1u)
            && (m_pDoor2_card <= 1u)
            && (m_bDoor1PoolIndex >= 0)
            && (m_bDoor2PoolIndex >= 0)
            && (m_nTimeCrusherCraneActivated >= 0)
            && (m_pSSTargetCar_card <= 1u)
            && (m_nTimeToStartAction >= 0)
            && (m_bCollectedCarsState >= 0)
            && (m_pTarget_card <= 1u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPedPathNode_ZppContract {
    bool bBlockade{false}; // Z++: 𝔹
    int64_t nodeIdX{0}; // Z++: ℕ
    int64_t nodeIdY{0}; // Z++: ℕ
    int32_t id{0}; // Z++: ℤ
    uint8_t prev_card{0u}; // Z++: Ref[CPedPathNode]
    uint8_t next_card{0u}; // Z++: Ref[CPedPathNode]
    constexpr bool checkInvariant() const {
        return (nodeIdX >= 0)
            && (nodeIdY >= 0)
            && (prev_card <= 1u)
            && (next_card <= 1u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPedPath_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPathNode_ZppContract {
    int32_t prevIndex{0}; // Z++: ℤ
    int32_t nextIndex{0}; // Z++: ℤ
    int32_t x{0}; // Z++: ℤ
    int32_t y{0}; // Z++: ℤ
    int32_t z{0}; // Z++: ℤ
    int32_t distance{0}; // Z++: ℤ
    int32_t firstLink{0}; // Z++: ℤ
    int64_t width{0}; // Z++: ℕ
    int32_t group{0}; // Z++: ℤ
    uint32_t numLinks{0u}; // Z++: 𝔹
    uint32_t bDeadEnd{0u}; // Z++: 𝔹
    uint32_t bDisabled{0u}; // Z++: 𝔹
    uint32_t bBetweenLevels{0u}; // Z++: 𝔹
    uint32_t bUseInRoadBlock{0u}; // Z++: 𝔹
    uint32_t bWaterPath{0u}; // Z++: 𝔹
    uint32_t bOnlySmallBoats{0u}; // Z++: 𝔹
    uint32_t bSelected{0u}; // Z++: 𝔹
    uint32_t speedLimit{0u}; // Z++: 𝔹
    uint32_t spawnRate{0u}; // Z++: 𝔹
    uint32_t flagsC{0u}; // Z++: 𝔹
    ZppVec3 pos{}; // Z++: CVector
    uint8_t prev_card{0u}; // Z++: Ref[CPathNode]
    uint8_t next_card{0u}; // Z++: Ref[CPathNode]
    int32_t objectIndex{0}; // Z++: ℤ
    uint32_t unkBits{0u}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (width >= 0)
            && (numLinks <= 15u)
            && (bDeadEnd <= 1u)
            && (bDisabled <= 1u)
            && (bBetweenLevels <= 1u)
            && (bUseInRoadBlock <= 1u)
            && (bWaterPath <= 1u)
            && (bOnlySmallBoats <= 1u)
            && (bSelected <= 1u)
            && (speedLimit <= 3u)
            && (spawnRate <= 15u)
            && (flagsC <= 15u)
            && (prev_card <= 1u)
            && (next_card <= 1u)
            && (unkBits <= 3u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CCarPathLink_ZppContract {
    int32_t x{0}; // Z++: ℤ
    int32_t y{0}; // Z++: ℤ
    int32_t pathNodeIndex{0}; // Z++: ℤ
    int32_t dirX{0}; // Z++: ℤ
    int32_t dirY{0}; // Z++: ℤ
    uint32_t numLeftLanes{0u}; // Z++: 𝔹
    uint32_t numRightLanes{0u}; // Z++: 𝔹
    uint32_t trafficLightDirection{0u}; // Z++: 𝔹
    uint32_t trafficLightType{0u}; // Z++: 𝔹
    uint32_t bBridgeLights{0u}; // Z++: 𝔹
    int64_t width{0}; // Z++: ℕ
    int32_t dir{0}; // Z++: CVector2D
    constexpr bool checkInvariant() const {
        return (numLeftLanes <= 7u)
            && (numRightLanes <= 7u)
            && (trafficLightDirection <= 1u)
            && (trafficLightType <= 3u)
            && (bBridgeLights <= 1u)
            && (width >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPathInfoForObject_ZppContract {
    float x{0.0f}; // Z++: ℝ
    float y{0.0f}; // Z++: ℝ
    float z{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int32_t next{0}; // Z++: ℤ
    int32_t numLeftLanes{0}; // Z++: ℤ
    int32_t numRightLanes{0}; // Z++: ℤ
    int32_t speedLimit{0}; // Z++: ℤ
    int64_t width{0}; // Z++: ℕ
    uint32_t crossing{0u}; // Z++: 𝔹
    uint32_t onlySmallBoats{0u}; // Z++: 𝔹
    uint32_t roadBlock{0u}; // Z++: 𝔹
    uint32_t disabled{0u}; // Z++: 𝔹
    uint32_t waterPath{0u}; // Z++: 𝔹
    uint32_t betweenLevels{0u}; // Z++: 𝔹
    uint32_t spawnRate{0u}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (width >= 0)
            && (crossing <= 1u)
            && (onlySmallBoats <= 1u)
            && (roadBlock <= 1u)
            && (disabled <= 1u)
            && (waterPath <= 1u)
            && (betweenLevels <= 1u)
            && (spawnRate <= 15u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CTempNode_ZppContract {
    ZppVec3 pos{}; // Z++: CVector
    int32_t dirX{0}; // Z++: ℤ
    int32_t dirY{0}; // Z++: ℤ
    int32_t link1{0}; // Z++: ℤ
    int32_t link2{0}; // Z++: ℤ
    int32_t numLeftLanes{0}; // Z++: ℤ
    int32_t numRightLanes{0}; // Z++: ℤ
    int64_t width{0}; // Z++: ℕ
    bool isCross{false}; // Z++: 𝔹
    int32_t linkState{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (width >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CTempNodeExternal_ZppContract {
    ZppVec3 pos{}; // Z++: CVector
    int32_t next{0}; // Z++: ℤ
    int32_t numLeftLanes{0}; // Z++: ℤ
    int32_t numRightLanes{0}; // Z++: ℤ
    int64_t width{0}; // Z++: ℕ
    bool isCross{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (width >= 0);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CRoute_ZppContract {
    std::array<int32_t, 8> m_node{}; // Z++: seq T  -- bounded array [8]
    size_t m_node_len{8u}; // Z++ cardinality #m_node
    constexpr bool checkInvariant() const {
        return (m_node_len == 8u && m_node.size() == 8u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CPathFind_ZppContract {
    std::array<int32_t, 1309> m_pathNodes{}; // Z++: seq CPathNode  -- bounded array [NUM_PATHNODES]
    size_t m_pathNodes_len{1309u}; // Z++ cardinality #m_pathNodes
    std::array<int32_t, 1310> m_carPathLinks{}; // Z++: seq CCarPathLink  -- bounded array [NUM_CARPATHLINKS]
    size_t m_carPathLinks_len{1310u}; // Z++ cardinality #m_carPathLinks
    std::array<int32_t, 1311> m_mapObjects{}; // Z++: seq Ref[CTreadable]  -- bounded array [NUM_MAPOBJECTS]
    size_t m_mapObjects_len{1311u}; // Z++ cardinality #m_mapObjects
    std::array<uint32_t, 1312> m_connections{}; // Z++: seq ℕ  -- bounded array [NUM_PATHCONNECTIONS]
    size_t m_connections_len{1312u}; // Z++ cardinality #m_connections
    std::array<uint32_t, 1312> m_distances{}; // Z++: seq ℕ  -- bounded array [NUM_PATHCONNECTIONS]
    size_t m_distances_len{1312u}; // Z++ cardinality #m_distances
    std::array<int32_t, 1312> m_carPathConnections{}; // Z++: seq ℤ  -- bounded array [NUM_PATHCONNECTIONS]
    size_t m_carPathConnections_len{1312u}; // Z++ cardinality #m_carPathConnections
    int32_t m_numPathNodes{0}; // Z++: ℤ
    int32_t m_numCarPathNodes{0}; // Z++: ℤ
    int32_t m_numPedPathNodes{0}; // Z++: ℤ
    int32_t m_numMapObjects{0}; // Z++: ℤ
    int32_t m_numConnections{0}; // Z++: ℤ
    int32_t m_numCarPathLinks{0}; // Z++: ℤ
    int32_t unk{0}; // Z++: ℤ
    std::array<uint32_t, 2> m_numGroups{}; // Z++: seq ℕ  -- bounded array [2]
    size_t m_numGroups_len{2u}; // Z++ cardinality #m_numGroups
    std::array<int32_t, 512> m_searchNodes{}; // Z++: seq CPathNode  -- bounded array [512]
    size_t m_searchNodes_len{512u}; // Z++ cardinality #m_searchNodes
    std::array<uint32_t, 1311> m_objectFlags{}; // Z++: seq ℕ  -- bounded array [NUM_MAPOBJECTS]
    size_t m_objectFlags_len{1311u}; // Z++ cardinality #m_objectFlags
    std::array<int32_t, 1312> m_connectionFlags{}; // Z++: seq CConnectionFlags  -- bounded array [NUM_PATHCONNECTIONS]
    size_t m_connectionFlags_len{1312u}; // Z++ cardinality #m_connectionFlags
    constexpr bool checkInvariant() const {
        return (m_pathNodes_len == 1309u && m_pathNodes.size() == 1309u)
            && (m_carPathLinks_len == 1310u && m_carPathLinks.size() == 1310u)
            && (m_mapObjects_len == 1311u && m_mapObjects.size() == 1311u)
            && (m_connections_len == 1312u && m_connections.size() == 1312u)
            && (m_distances_len == 1312u && m_distances.size() == 1312u)
            && (m_carPathConnections_len == 1312u && m_carPathConnections.size() == 1312u)
            && (m_numGroups_len == 2u && m_numGroups.size() == 2u)
            && (m_searchNodes_len == 512u && m_searchNodes.size() == 512u)
            && (m_objectFlags_len == 1311u && m_objectFlags.size() == 1311u)
            && (m_connectionFlags_len == 1312u && m_connectionFlags.size() == 1312u);
    }
};
// State Schema Contract from zpp/control/PathFind.zpp
struct CTempDetachedNode_ZppContract {
    std::array<uint32_t, 20> foo{}; // Z++: seq ℕ  -- bounded array [20]
    size_t foo_len{20u}; // Z++ cardinality #foo
    constexpr bool checkInvariant() const {
        return (foo_len == 20u && foo.size() == 20u);
    }
};
// State Schema Contract from zpp/control/Pickups.zpp
struct tPickupMessage_ZppContract {
    int32_t m_pos{0}; // Z++: CVector2D
    int32_t m_weaponType{0}; // Z++: eWeaponType
    int32_t m_dist{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    int64_t m_bOutOfStock{0}; // Z++: ℕ
    int64_t m_quantity{0}; // Z++: ℕ
    int64_t money{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_bOutOfStock >= 0)
            && (m_quantity >= 0)
            && (money >= 0);
    }
};
// State Schema Contract from zpp/control/Pickups.zpp
struct CPacManPickup_ZppContract {
    ZppVec3 m_vecPosn{}; // Z++: CVector
    uint8_t m_pObject_card{0u}; // Z++: Ref[CObject]
    int64_t m_eType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_pObject_card <= 1u)
            && (m_eType >= 0);
    }
};
// State Schema Contract from zpp/control/PowerPoints.zpp
struct CPowerPoint_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/PowerPoints.zpp
struct CPowerPoints_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Record.zpp
struct tCoors_ZppContract {
    ZppVec3 pos{}; // Z++: CVector
    float angle{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Record.zpp
struct CCarStateEachFrame_ZppContract {
    int32_t velX{0}; // Z++: ℤ
    int32_t velY{0}; // Z++: ℤ
    int32_t velZ{0}; // Z++: ℤ
    int32_t rightX{0}; // Z++: ℤ
    int32_t rightY{0}; // Z++: ℤ
    int32_t rightZ{0}; // Z++: ℤ
    int32_t forwardX{0}; // Z++: ℤ
    int32_t forwardY{0}; // Z++: ℤ
    int32_t forwardZ{0}; // Z++: ℤ
    int32_t wheel{0}; // Z++: ℤ
    int32_t gas{0}; // Z++: ℤ
    int32_t brake{0}; // Z++: ℤ
    bool handbrake{false}; // Z++: 𝔹
    ZppVec3 pos{}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Record.zpp
struct CRecordDataForChase_ZppContract {
    int64_t Status{0}; // Z++: ℕ
    int32_t PositionChanges{0}; // Z++: ℤ
    int64_t CurrentCar{0}; // Z++: ℕ
    float AnimTime{0.0f}; // Z++: ℝ
    int64_t AnimStartTime{0}; // Z++: ℕ
    std::array<int32_t, 20> pBaseMemForCar{}; // Z++: seq Ref[CCarStateEachFrame]  -- bounded array [NUM_CHASE_CARS]
    size_t pBaseMemForCar_len{20u}; // Z++ cardinality #pBaseMemForCar
    float TimeMultiplier{0.0f}; // Z++: ℝ
    int32_t FId2{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (pBaseMemForCar_len == 20u && pBaseMemForCar.size() == 20u)
            && (Status >= 0)
            && (CurrentCar >= 0)
            && (AnimStartTime >= 0);
    }
};
// State Schema Contract from zpp/control/Record.zpp
struct tGameBuffer_ZppContract {
    float m_fTimeStep{0.0f}; // Z++: ℝ
    int64_t m_nTimeInMilliseconds{0}; // Z++: ℕ
    std::array<uint32_t, 2> m_nSizeOfPads{}; // Z++: seq ℕ  -- bounded array [2]
    size_t m_nSizeOfPads_len{2u}; // Z++ cardinality #m_nSizeOfPads
    int64_t m_nChecksum{0}; // Z++: ℕ
    std::array<uint32_t, 116> m_ControllerBuffer{}; // Z++: seq ℕ  -- bounded array [116]
    size_t m_ControllerBuffer_len{116u}; // Z++ cardinality #m_ControllerBuffer
    constexpr bool checkInvariant() const {
        return (m_nSizeOfPads_len == 2u && m_nSizeOfPads.size() == 2u)
            && (m_ControllerBuffer_len == 116u && m_ControllerBuffer.size() == 116u)
            && (m_nTimeInMilliseconds >= 0)
            && (m_nChecksum >= 0);
    }
};
// State Schema Contract from zpp/control/Record.zpp
struct CRecordDataForGame_ZppContract {
    int64_t RecordingState{0}; // Z++: ℕ
    uint8_t pDataBuffer_card{0u}; // Z++: ℙ ℕ
    uint8_t pDataBufferPointer_card{0u}; // Z++: ℙ ℕ
    int32_t FId{0}; // Z++: ℤ
    int32_t pDataBufferForFrame{0}; // Z++: tGameBuffer
    constexpr bool checkInvariant() const {
        return (RecordingState >= 0)
            && (pDataBuffer_card <= 1u)
            && (pDataBufferPointer_card <= 1u);
    }
};
// State Schema Contract from zpp/control/Remote.zpp
struct CRemote_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct CAddressInReplayBuffer_ZppContract {
    int64_t m_nOffset{0}; // Z++: ℕ
    uint8_t m_pBase_card{0u}; // Z++: ℙ ℕ
    int64_t m_bSlot{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nOffset >= 0)
            && (m_pBase_card <= 1u)
            && (m_bSlot >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct CStoredAnimationState_ZppContract {
    int64_t animId{0}; // Z++: ℕ
    int64_t time{0}; // Z++: ℕ
    int64_t speed{0}; // Z++: ℕ
    int64_t groupId{0}; // Z++: ℕ
    int64_t secAnimId{0}; // Z++: ℕ
    int64_t secTime{0}; // Z++: ℕ
    int64_t secSpeed{0}; // Z++: ℕ
    int64_t secGroupId{0}; // Z++: ℕ
    int64_t blendAmount{0}; // Z++: ℕ
    int64_t partAnimId{0}; // Z++: ℕ
    int64_t partAnimTime{0}; // Z++: ℕ
    int64_t partAnimSpeed{0}; // Z++: ℕ
    int64_t partBlendAmount{0}; // Z++: ℕ
    int64_t partGroupId{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (animId >= 0)
            && (time >= 0)
            && (speed >= 0)
            && (groupId >= 0)
            && (secAnimId >= 0)
            && (secTime >= 0)
            && (secSpeed >= 0)
            && (secGroupId >= 0)
            && (blendAmount >= 0)
            && (partAnimId >= 0)
            && (partAnimTime >= 0)
            && (partAnimSpeed >= 0)
            && (partBlendAmount >= 0)
            && (partGroupId >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct CStoredDetailedAnimationState_ZppContract {
    std::array<uint32_t, 3> aAnimId{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aAnimId_len{3u}; // Z++ cardinality #aAnimId
    std::array<uint32_t, 3> aCurTime{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aCurTime_len{3u}; // Z++ cardinality #aCurTime
    std::array<uint32_t, 3> aSpeed{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aSpeed_len{3u}; // Z++ cardinality #aSpeed
    std::array<uint32_t, 3> aBlendAmount{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aBlendAmount_len{3u}; // Z++ cardinality #aBlendAmount
    std::array<int32_t, 3> aBlendDelta{}; // Z++: seq ℤ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aBlendDelta_len{3u}; // Z++ cardinality #aBlendDelta
    std::array<uint32_t, 3> aFunctionCallbackID{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aFunctionCallbackID_len{3u}; // Z++ cardinality #aFunctionCallbackID
    std::array<uint32_t, 3> aFlags{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aFlags_len{3u}; // Z++ cardinality #aFlags
    std::array<uint32_t, 3> aGroupId{}; // Z++: seq ℕ  -- bounded array [NUM_MAIN_ANIMS_IN_REPLAY]
    size_t aGroupId_len{3u}; // Z++ cardinality #aGroupId
    std::array<uint32_t, 6> aAnimId2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aAnimId2_len{6u}; // Z++ cardinality #aAnimId2
    std::array<uint32_t, 6> aCurTime2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aCurTime2_len{6u}; // Z++ cardinality #aCurTime2
    std::array<uint32_t, 6> aSpeed2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aSpeed2_len{6u}; // Z++ cardinality #aSpeed2
    std::array<uint32_t, 6> aBlendAmount2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aBlendAmount2_len{6u}; // Z++ cardinality #aBlendAmount2
    std::array<int32_t, 6> aBlendDelta2{}; // Z++: seq ℤ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aBlendDelta2_len{6u}; // Z++ cardinality #aBlendDelta2
    std::array<uint32_t, 6> aFunctionCallbackID2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aFunctionCallbackID2_len{6u}; // Z++ cardinality #aFunctionCallbackID2
    std::array<uint32_t, 6> aFlags2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aFlags2_len{6u}; // Z++ cardinality #aFlags2
    std::array<uint32_t, 6> aGroupId2{}; // Z++: seq ℕ  -- bounded array [NUM_PARTIAL_ANIMS_IN_REPLAY]
    size_t aGroupId2_len{6u}; // Z++ cardinality #aGroupId2
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (aAnimId_len == 3u && aAnimId.size() == 3u)
            && (aCurTime_len == 3u && aCurTime.size() == 3u)
            && (aSpeed_len == 3u && aSpeed.size() == 3u)
            && (aBlendAmount_len == 3u && aBlendAmount.size() == 3u)
            && (aBlendDelta_len == 3u && aBlendDelta.size() == 3u)
            && (aFunctionCallbackID_len == 3u && aFunctionCallbackID.size() == 3u)
            && (aFlags_len == 3u && aFlags.size() == 3u)
            && (aGroupId_len == 3u && aGroupId.size() == 3u)
            && (aAnimId2_len == 6u && aAnimId2.size() == 6u)
            && (aCurTime2_len == 6u && aCurTime2.size() == 6u)
            && (aSpeed2_len == 6u && aSpeed2.size() == 6u)
            && (aBlendAmount2_len == 6u && aBlendAmount2.size() == 6u)
            && (aBlendDelta2_len == 6u && aBlendDelta2.size() == 6u)
            && (aFunctionCallbackID2_len == 6u && aFunctionCallbackID2.size() == 6u)
            && (aFlags2_len == 6u && aFlags2.size() == 6u)
            && (aGroupId2_len == 6u && aGroupId2.size() == 6u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct CReplay_ZppContract {
    bool in_rcvehicle{false}; // Z++: 𝔹
    int32_t camera_pos{0}; // Z++: CMatrix
    ZppVec3 player_pos{}; // Z++: CVector
    int64_t hours{0}; // Z++: ℕ
    int64_t minutes{0}; // Z++: ℕ
    int64_t __align{0}; // Z++: ℕ
    int64_t old_weather{0}; // Z++: ℕ
    int64_t new_weather{0}; // Z++: ℕ
    float interpolation{0.0f}; // Z++: ℝ
    int64_t timer{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    int64_t mi{0}; // Z++: ℕ
    int64_t pedtype{0}; // Z++: ℕ
    int64_t frames{0}; // Z++: ℕ
    int64_t lifetime{0}; // Z++: ℕ
    ZppVec3 inf{}; // Z++: CVector
    ZppVec3 sup{}; // Z++: CVector
    int32_t heading{0}; // Z++: ℤ
    int32_t vehicle_index{0}; // Z++: ℤ
    int32_t anim_state{0}; // Z++: CStoredAnimationState
    int32_t matrix{0}; // Z++: CCompressedMatrixNotAligned
    int64_t weapon_model{0}; // Z++: ℕ
    int32_t assoc_group_id{0}; // Z++: ℤ
    bool is_visible{false}; // Z++: 𝔹
    int64_t health{0}; // Z++: ℕ
    int64_t acceleration{0}; // Z++: ℕ
    std::array<int32_t, 2> door_angles{}; // Z++: seq ℤ  -- bounded array [2]
    size_t door_angles_len{2u}; // Z++ cardinality #door_angles
    int64_t panels{0}; // Z++: ℕ
    int32_t velocityX{0}; // Z++: ℤ
    int32_t velocityY{0}; // Z++: ℤ
    int32_t velocityZ{0}; // Z++: ℤ
    int32_t wheel_state{0}; // Z++: ℤ
    std::array<uint32_t, 4> wheel_susp_dist{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_susp_dist_len{4u}; // Z++ cardinality #wheel_susp_dist
    std::array<uint32_t, 4> wheel_rotation{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_rotation_len{4u}; // Z++ cardinality #wheel_rotation
    int64_t door_status{0}; // Z++: ℕ
    int64_t primary_color{0}; // Z++: ℕ
    int64_t secondary_color{0}; // Z++: ℕ
    bool render_scorched{false}; // Z++: 𝔹
    int32_t skimmer_speed{0}; // Z++: ℤ
    int32_t vehicle_type{0}; // Z++: ℤ
    int32_t lean_angle{0}; // Z++: ℤ
    int32_t wheel_angle{0}; // Z++: ℤ
    int64_t particle_type{0}; // Z++: ℕ
    int32_t dir_x{0}; // Z++: ℤ
    int32_t dir_y{0}; // Z++: ℤ
    int32_t dir_z{0}; // Z++: ℤ
    int64_t r{0}; // Z++: ℕ
    int64_t g{0}; // Z++: ℕ
    int64_t b{0}; // Z++: ℕ
    int64_t a{0}; // Z++: ℕ
    int32_t pos_x{0}; // Z++: ℤ
    int32_t pos_y{0}; // Z++: ℤ
    int32_t pos_z{0}; // Z++: ℤ
    float size{0.0f}; // Z++: ℝ
    int64_t cam_shake_start{0}; // Z++: ℕ
    float cam_shake_strength{0.0f}; // Z++: ℝ
    int64_t cur_area{0}; // Z++: ℕ
    bool video_cam{false}; // Z++: 𝔹
    bool lift_cam{false}; // Z++: 𝔹
    int64_t Mode{0}; // Z++: ℕ
    int32_t Record{0}; // Z++: CAddressInReplayBuffer
    int32_t Playback{0}; // Z++: CAddressInReplayBuffer
    uint8_t pBuf0_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf1_card{0u}; // Z++: Ref[CAutomobile]
    uint8_t pBuf2_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf3_card{0u}; // Z++: Ref[CPlayerPed]
    uint8_t pBuf4_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf5_card{0u}; // Z++: Ref[CCutsceneObject]
    uint8_t pBuf6_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf7_card{0u}; // Z++: Ref[CPtrNode]
    uint8_t pBuf8_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf9_card{0u}; // Z++: Ref[CEntryInfoNode]
    uint8_t pBuf10_card{0u}; // Z++: ℙ ℕ
    uint8_t pBuf11_card{0u}; // Z++: Ref[CDummyPed]
    uint8_t pRadarBlips_card{0u}; // Z++: ℙ ℕ
    uint8_t pStoredCam_card{0u}; // Z++: ℙ ℕ
    uint8_t pWorld1_card{0u}; // Z++: ℙ ℕ
    uint8_t pEmptyReferences_card{0u}; // Z++: Ref[CReference]
    uint8_t pPedAnims_card{0u}; // Z++: Ref[CStoredDetailedAnimationState]
    uint8_t pPickups_card{0u}; // Z++: ℙ ℕ
    uint8_t pReferences_card{0u}; // Z++: ℙ ℕ
    std::array<uint32_t, 8> BufferStatus{}; // Z++: seq ℕ  -- bounded array [NUM_REPLAYBUFFERS]
    size_t BufferStatus_len{8u}; // Z++ cardinality #BufferStatus
    bool bPlayingBackFromFile{false}; // Z++: 𝔹
    bool bReplayEnabled{false}; // Z++: 𝔹
    int64_t SlowMotion{0}; // Z++: ℕ
    int64_t FramesActiveLookAroundCam{0}; // Z++: ℕ
    bool bDoLoadSceneWhenDone{false}; // Z++: 𝔹
    uint8_t WorldPtrList_card{0u}; // Z++: Ref[CPtrNode]
    uint8_t BigBuildingPtrList_card{0u}; // Z++: Ref[CPtrNode]
    int32_t PlayerWanted{0}; // Z++: CWanted
    int32_t PlayerInfo{0}; // Z++: CPlayerInfo
    int64_t Time1{0}; // Z++: ℕ
    int64_t Time2{0}; // Z++: ℕ
    int64_t Time3{0}; // Z++: ℕ
    int64_t Time4{0}; // Z++: ℕ
    int64_t Frame{0}; // Z++: ℕ
    int64_t ClockHours{0}; // Z++: ℕ
    int64_t ClockMinutes{0}; // Z++: ℕ
    int64_t OldWeatherType{0}; // Z++: ℕ
    int64_t NewWeatherType{0}; // Z++: ℕ
    float WeatherInterpolationValue{0.0f}; // Z++: ℝ
    float TimeStepNonClipped{0.0f}; // Z++: ℝ
    float TimeStep{0.0f}; // Z++: ℝ
    float TimeScale{0.0f}; // Z++: ℝ
    float CameraFixedX{0.0f}; // Z++: ℝ
    float CameraFixedY{0.0f}; // Z++: ℝ
    float CameraFixedZ{0.0f}; // Z++: ℝ
    int32_t OldRadioStation{0}; // Z++: ℤ
    int32_t CameraMode{0}; // Z++: ℤ
    bool bAllowLookAroundCam{false}; // Z++: 𝔹
    float LoadSceneX{0.0f}; // Z++: ℝ
    float LoadSceneY{0.0f}; // Z++: ℝ
    float LoadSceneZ{0.0f}; // Z++: ℝ
    float CameraFocusX{0.0f}; // Z++: ℝ
    float CameraFocusY{0.0f}; // Z++: ℝ
    float CameraFocusZ{0.0f}; // Z++: ℝ
    bool bPlayerInRCBuggy{false}; // Z++: 𝔹
    float fDistanceLookAroundCam{0.0f}; // Z++: ℝ
    float fAlphaAngleLookAroundCam{0.0f}; // Z++: ℝ
    float fBetaAngleLookAroundCam{0.0f}; // Z++: ℝ
    int32_t ms_nNumCivMale_Stored{0}; // Z++: ℤ
    int32_t ms_nNumCivFemale_Stored{0}; // Z++: ℤ
    int32_t ms_nNumCop_Stored{0}; // Z++: ℤ
    int32_t ms_nNumEmergency_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang1_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang2_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang3_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang4_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang5_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang6_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang7_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang8_Stored{0}; // Z++: ℤ
    int32_t ms_nNumGang9_Stored{0}; // Z++: ℤ
    int32_t ms_nNumDummy_Stored{0}; // Z++: ℤ
    int32_t ms_nTotalCarPassengerPeds_Stored{0}; // Z++: ℤ
    int32_t ms_nTotalCivPeds_Stored{0}; // Z++: ℤ
    int32_t ms_nTotalGangPeds_Stored{0}; // Z++: ℤ
    int32_t ms_nTotalPeds_Stored{0}; // Z++: ℤ
    int32_t ms_nTotalMissionPeds_Stored{0}; // Z++: ℤ
    uint8_t pGarages_card{0u}; // Z++: ℙ ℕ
    uint8_t FireArray_card{0u}; // Z++: Ref[CFire]
    int64_t NumOfFires{0}; // Z++: ℕ
    uint8_t paProjectileInfo_card{0u}; // Z++: ℙ ℕ
    uint8_t paProjectiles_card{0u}; // Z++: ℙ ℕ
    int64_t CurrArea{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (door_angles_len == 2u && door_angles.size() == 2u)
            && (wheel_susp_dist_len == 4u && wheel_susp_dist.size() == 4u)
            && (wheel_rotation_len == 4u && wheel_rotation.size() == 4u)
            && (BufferStatus_len == 8u && BufferStatus.size() == 8u)
            && (hours >= 0)
            && (minutes >= 0)
            && (__align >= 0)
            && (old_weather >= 0)
            && (new_weather >= 0)
            && (timer >= 0)
            && (index >= 0)
            && (mi >= 0)
            && (pedtype >= 0)
            && (frames >= 0)
            && (lifetime >= 0)
            && (weapon_model >= 0)
            && (health >= 0)
            && (acceleration >= 0)
            && (panels >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tGeneralPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    bool in_rcvehicle{false}; // Z++: 𝔹
    int32_t camera_pos{0}; // Z++: CMatrix
    ZppVec3 player_pos{}; // Z++: CVector
    constexpr bool checkInvariant() const {
        return (type >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tClockPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t hours{0}; // Z++: ℕ
    int64_t minutes{0}; // Z++: ℕ
    int64_t __align{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (hours >= 0)
            && (minutes >= 0)
            && (__align >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tWeatherPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t old_weather{0}; // Z++: ℕ
    int64_t new_weather{0}; // Z++: ℕ
    float interpolation{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (old_weather >= 0)
            && (new_weather >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tTimerPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t timer{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (timer >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tPedHeaderPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    int64_t mi{0}; // Z++: ℕ
    int64_t pedtype{0}; // Z++: ℕ
    std::array<uint32_t, 3> __align{}; // Z++: seq ℕ  -- bounded array [3]
    size_t __align_len{3u}; // Z++ cardinality #__align
    constexpr bool checkInvariant() const {
        return (__align_len == 3u && __align.size() == 3u)
            && (type >= 0)
            && (index >= 0)
            && (mi >= 0)
            && (pedtype >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tBulletTracePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t frames{0}; // Z++: ℕ
    int64_t lifetime{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    ZppVec3 inf{}; // Z++: CVector
    ZppVec3 sup{}; // Z++: CVector
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (frames >= 0)
            && (lifetime >= 0)
            && (index >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tEndOfFramePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    std::array<uint32_t, 3> __align{}; // Z++: seq ℕ  -- bounded array [3]
    size_t __align_len{3u}; // Z++ cardinality #__align
    constexpr bool checkInvariant() const {
        return (__align_len == 3u && __align.size() == 3u)
            && (type >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tPedUpdatePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    int32_t heading{0}; // Z++: ℤ
    int32_t vehicle_index{0}; // Z++: ℤ
    int32_t anim_state{0}; // Z++: CStoredAnimationState
    int32_t matrix{0}; // Z++: CCompressedMatrixNotAligned
    int64_t weapon_model{0}; // Z++: ℕ
    int32_t assoc_group_id{0}; // Z++: ℤ
    bool is_visible{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (index >= 0)
            && (weapon_model >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tVehicleUpdatePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    int64_t health{0}; // Z++: ℕ
    int64_t acceleration{0}; // Z++: ℕ
    int32_t matrix{0}; // Z++: CCompressedMatrixNotAligned
    std::array<int32_t, 2> door_angles{}; // Z++: seq ℤ  -- bounded array [2]
    size_t door_angles_len{2u}; // Z++ cardinality #door_angles
    int64_t mi{0}; // Z++: ℕ
    int64_t panels{0}; // Z++: ℕ
    int32_t velocityX{0}; // Z++: ℤ
    int32_t velocityY{0}; // Z++: ℤ
    int32_t velocityZ{0}; // Z++: ℤ
    int32_t wheel_state{0}; // Z++: ℤ
    std::array<uint32_t, 4> wheel_susp_dist{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_susp_dist_len{4u}; // Z++ cardinality #wheel_susp_dist
    std::array<uint32_t, 4> wheel_rotation{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_rotation_len{4u}; // Z++ cardinality #wheel_rotation
    int64_t door_status{0}; // Z++: ℕ
    int64_t primary_color{0}; // Z++: ℕ
    int64_t secondary_color{0}; // Z++: ℕ
    bool render_scorched{false}; // Z++: 𝔹
    int32_t skimmer_speed{0}; // Z++: ℤ
    int32_t vehicle_type{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (door_angles_len == 2u && door_angles.size() == 2u)
            && (wheel_susp_dist_len == 4u && wheel_susp_dist.size() == 4u)
            && (wheel_rotation_len == 4u && wheel_rotation.size() == 4u)
            && (type >= 0)
            && (index >= 0)
            && (health >= 0)
            && (acceleration >= 0)
            && (mi >= 0)
            && (panels >= 0)
            && (door_status >= 0)
            && (primary_color >= 0)
            && (secondary_color >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tBikeUpdatePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t index{0}; // Z++: ℕ
    int64_t health{0}; // Z++: ℕ
    int64_t acceleration{0}; // Z++: ℕ
    int32_t matrix{0}; // Z++: CCompressedMatrixNotAligned
    std::array<int32_t, 2> door_angles{}; // Z++: seq ℤ  -- bounded array [2]
    size_t door_angles_len{2u}; // Z++ cardinality #door_angles
    int64_t mi{0}; // Z++: ℕ
    int32_t velocityX{0}; // Z++: ℤ
    int32_t velocityY{0}; // Z++: ℤ
    int32_t velocityZ{0}; // Z++: ℤ
    int32_t wheel_state{0}; // Z++: ℤ
    std::array<uint32_t, 4> wheel_susp_dist{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_susp_dist_len{4u}; // Z++ cardinality #wheel_susp_dist
    std::array<uint32_t, 4> wheel_rotation{}; // Z++: seq ℕ  -- bounded array [4]
    size_t wheel_rotation_len{4u}; // Z++ cardinality #wheel_rotation
    int64_t primary_color{0}; // Z++: ℕ
    int64_t secondary_color{0}; // Z++: ℕ
    int32_t lean_angle{0}; // Z++: ℤ
    int32_t wheel_angle{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (door_angles_len == 2u && door_angles.size() == 2u)
            && (wheel_susp_dist_len == 4u && wheel_susp_dist.size() == 4u)
            && (wheel_rotation_len == 4u && wheel_rotation.size() == 4u)
            && (type >= 0)
            && (index >= 0)
            && (health >= 0)
            && (acceleration >= 0)
            && (mi >= 0)
            && (primary_color >= 0)
            && (secondary_color >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tParticlePacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t particle_type{0}; // Z++: ℕ
    int32_t dir_x{0}; // Z++: ℤ
    int32_t dir_y{0}; // Z++: ℤ
    int32_t dir_z{0}; // Z++: ℤ
    int64_t r{0}; // Z++: ℕ
    int64_t g{0}; // Z++: ℕ
    int64_t b{0}; // Z++: ℕ
    int64_t a{0}; // Z++: ℕ
    int32_t pos_x{0}; // Z++: ℤ
    int32_t pos_y{0}; // Z++: ℤ
    int32_t pos_z{0}; // Z++: ℤ
    float size{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (particle_type >= 0)
            && (r >= 0)
            && (g >= 0)
            && (b >= 0)
            && (a >= 0);
    }
};
// State Schema Contract from zpp/control/Replay.zpp
struct tMiscPacket_ZppContract {
    int64_t type{0}; // Z++: ℕ
    int64_t cam_shake_start{0}; // Z++: ℕ
    float cam_shake_strength{0.0f}; // Z++: ℝ
    int64_t cur_area{0}; // Z++: ℕ
    uint32_t video_cam{0u}; // Z++: 𝔹
    uint32_t lift_cam{0u}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (type >= 0)
            && (cam_shake_start >= 0)
            && (cur_area >= 0)
            && (video_cam <= 1u)
            && (lift_cam <= 1u);
    }
};
// State Schema Contract from zpp/control/Restart.zpp
struct CRestart_ZppContract {
    int64_t OverrideHospitalLevel{0}; // Z++: ℕ
    int64_t OverridePoliceStationLevel{0}; // Z++: ℕ
    bool bFadeInAfterNextArrest{false}; // Z++: 𝔹
    bool bFadeInAfterNextDeath{false}; // Z++: 𝔹
    bool bOverrideRestart{false}; // Z++: 𝔹
    ZppVec3 OverridePosition{}; // Z++: CVector
    float OverrideHeading{0.0f}; // Z++: ℝ
    std::array<ZppVec3, 8> HospitalRestartPoints{}; // Z++: seq CVector  -- bounded array [NUM_RESTART_POINTS]
    size_t HospitalRestartPoints_len{8u}; // Z++ cardinality #HospitalRestartPoints
    std::array<float, 8> HospitalRestartHeadings{}; // Z++: seq ℝ  -- bounded array [NUM_RESTART_POINTS]
    size_t HospitalRestartHeadings_len{8u}; // Z++ cardinality #HospitalRestartHeadings
    int64_t NumberOfHospitalRestarts{0}; // Z++: ℕ
    std::array<ZppVec3, 8> PoliceRestartPoints{}; // Z++: seq CVector  -- bounded array [NUM_RESTART_POINTS]
    size_t PoliceRestartPoints_len{8u}; // Z++ cardinality #PoliceRestartPoints
    std::array<float, 8> PoliceRestartHeadings{}; // Z++: seq ℝ  -- bounded array [NUM_RESTART_POINTS]
    size_t PoliceRestartHeadings_len{8u}; // Z++ cardinality #PoliceRestartHeadings
    int64_t NumberOfPoliceRestarts{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (HospitalRestartPoints_len == 8u && HospitalRestartPoints.size() == 8u)
            && (HospitalRestartHeadings_len == 8u && HospitalRestartHeadings.size() == 8u)
            && (PoliceRestartPoints_len == 8u && PoliceRestartPoints.size() == 8u)
            && (PoliceRestartHeadings_len == 8u && PoliceRestartHeadings.size() == 8u)
            && (OverrideHospitalLevel >= 0)
            && (OverridePoliceStationLevel >= 0)
            && (NumberOfHospitalRestarts >= 0)
            && (NumberOfPoliceRestarts >= 0);
    }
};
// State Schema Contract from zpp/control/RoadBlocks.zpp
struct CScriptRoadblock_ZppContract {
    ZppVec3 m_vInf{}; // Z++: CVector
    ZppVec3 m_vSup{}; // Z++: CVector
    bool m_bInUse{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/SceneEdit.zpp
struct CMovieCommand_ZppContract {
    int32_t m_nCommandId{0}; // Z++: ℤ
    ZppVec3 m_vecPosition{}; // Z++: CVector
    ZppVec3 m_vecCamera{}; // Z++: CVector
    int32_t m_nActorId{0}; // Z++: ℤ
    int32_t m_nActor2Id{0}; // Z++: ℤ
    int32_t m_nVehicleId{0}; // Z++: ℤ
    int32_t m_nModelIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/SceneEdit.zpp
struct CSceneEdit_ZppContract {
    int32_t m_bCameraFollowActor{0}; // Z++: ℤ
    ZppVec3 m_vecCurrentPosition{}; // Z++: CVector
    ZppVec3 m_vecCamHeading{}; // Z++: CVector
    ZppVec3 m_vecGotoPosition{}; // Z++: CVector
    int32_t m_nVehicle{0}; // Z++: ℤ
    int32_t m_nVehicle2{0}; // Z++: ℤ
    int32_t m_nActor{0}; // Z++: ℤ
    int32_t m_nActor2{0}; // Z++: ℤ
    int32_t m_nVehiclemodelId{0}; // Z++: ℤ
    int32_t m_nPedmodelId{0}; // Z++: ℤ
    int32_t m_nCurrentMovieCommand{0}; // Z++: ℤ
    int32_t m_nCurrentCommand{0}; // Z++: ℤ
    int32_t m_nCurrentVehicle{0}; // Z++: ℤ
    int32_t m_nCurrentActor{0}; // Z++: ℤ
    bool m_bEditOn{false}; // Z++: 𝔹
    bool m_bRecording{false}; // Z++: 𝔹
    bool m_bCommandActive{false}; // Z++: 𝔹
    bool m_bActorSelected{false}; // Z++: 𝔹
    bool m_bActor2Selected{false}; // Z++: 𝔹
    bool m_bVehicleSelected{false}; // Z++: 𝔹
    int32_t m_nNumActors{0}; // Z++: ℤ
    int32_t m_nNumVehicles{0}; // Z++: ℤ
    int32_t m_nNumMovieCommands{0}; // Z++: ℤ
    int32_t m_nWeaponType{0}; // Z++: ℤ
    std::array<int32_t, 5> pActors{}; // Z++: seq Ref[CPed]  -- bounded array [NUM_ACTORS_IN_MOVIE]
    size_t pActors_len{5u}; // Z++ cardinality #pActors
    std::array<int32_t, 5> pVehicles{}; // Z++: seq Ref[CVehicle]  -- bounded array [NUM_VEHICLES_IN_MOVIE]
    size_t pVehicles_len{5u}; // Z++ cardinality #pVehicles
    bool m_bDrawGotoArrow{false}; // Z++: 𝔹
    std::array<int32_t, 20> Movie{}; // Z++: seq CMovieCommand  -- bounded array [NUM_COMMANDS_IN_MOVIE]
    size_t Movie_len{20u}; // Z++ cardinality #Movie
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (pActors_len == 5u && pActors.size() == 5u)
            && (pVehicles_len == 5u && pVehicles.size() == 5u)
            && (Movie_len == 20u && Movie.size() == 20u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct CStoredLine_ZppContract {
    ZppVec3 vecInf{}; // Z++: CVector
    ZppVec3 vecSup{}; // Z++: CVector
    int64_t color1{0}; // Z++: ℕ
    int64_t color2{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (color1 >= 0)
            && (color2 >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct CMissionCleanup_ZppContract {
    std::array<int32_t, 50> m_sEntities{}; // Z++: seq cleanup_entity_struct  -- bounded array [MAX_CLEANUP]
    size_t m_sEntities_len{50u}; // Z++ cardinality #m_sEntities
    int64_t m_nCount{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_sEntities_len == 50u && m_sEntities.size() == 50u)
            && (m_nCount >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct CUpsideDownCarCheck_ZppContract {
    std::array<int32_t, 6> m_sCars{}; // Z++: seq upsidedown_car_data  -- bounded array [MAX_UPSIDEDOWN_CAR_CHECKS]
    size_t m_sCars_len{6u}; // Z++ cardinality #m_sCars
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (m_sCars_len == 6u && m_sCars.size() == 6u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct tCollectiveData_ZppContract {
    int32_t colIndex{0}; // Z++: ℤ
    int32_t pedIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct tUsedObject_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t index{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/Script.zpp
struct tBuildingSwap_ZppContract {
    uint8_t m_pBuilding_card{0u}; // Z++: Ref[CBuilding]
    int32_t m_nNewModel{0}; // Z++: ℤ
    int32_t m_nOldModel{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pBuilding_card <= 1u);
    }
};
// State Schema Contract from zpp/control/Script10.zpp
struct Script10ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{426}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 426);
    }
};
// State Schema Contract from zpp/control/Script2.zpp
struct Script2ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1637}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1637);
    }
};
// State Schema Contract from zpp/control/Script3.zpp
struct Script3ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{2502}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 2502);
    }
};
// State Schema Contract from zpp/control/Script4.zpp
struct Script4ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{2454}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 2454);
    }
};
// State Schema Contract from zpp/control/Script5.zpp
struct Script5ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{3081}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 3081);
    }
};
// State Schema Contract from zpp/control/Script6.zpp
struct Script6ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1910}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1910);
    }
};
// State Schema Contract from zpp/control/Script7.zpp
struct Script7ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1564}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1564);
    }
};
// State Schema Contract from zpp/control/Script8.zpp
struct Script8ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{841}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 841);
    }
};
// State Schema Contract from zpp/control/Script9.zpp
struct Script9ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{781}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 781);
    }
};
// State Schema Contract from zpp/control/ScriptDebug.zpp
struct ScriptDebugModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{4692}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 4692);
    }
};
// State Schema Contract from zpp/control/SetPieces.zpp
struct CSetPiece_ZppContract {
    int64_t m_nType{0}; // Z++: ℕ
    int64_t m_nLastTimeCreated{0}; // Z++: ℕ
    int32_t m_vTriggerInf{0}; // Z++: CVector2D
    int32_t m_vTriggerSup{0}; // Z++: CVector2D
    int32_t m_vSpawn1{0}; // Z++: CVector2D
    int32_t m_vSpawn2{0}; // Z++: CVector2D
    int32_t m_vTarget1{0}; // Z++: CVector2D
    int32_t m_vTarget2{0}; // Z++: CVector2D
    constexpr bool checkInvariant() const {
        return (m_nType >= 0)
            && (m_nLastTimeCreated >= 0);
    }
};
// State Schema Contract from zpp/control/TrafficLights.zpp
struct CTrafficLights_ZppContract {
    bool bGreenLightsCheat{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Delta Operation Schema Contract (ΔCCarCtrl) from zpp/control/CarCtrl.zpp
constexpr bool op_CCarCtrl_Init(const CCarCtrl_ZppContract& pre, const CCarCtrl_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.NumRandomCars == 0) && (post.NumLawEnforcerCars == 0) && (post.NumMissionCars == 0) && (post.NumParkedCars == 0) && (post.NumPermanentCars == 0) && (post.NumAmbulancesOnDuty == 0);
}
constexpr CCarCtrl_ZppContract apply_CCarCtrl_Init(const CCarCtrl_ZppContract& pre) {
    CCarCtrl_ZppContract post = pre;
    post.NumRandomCars = 0;
    post.NumLawEnforcerCars = 0;
    post.NumMissionCars = 0;
    post.NumParkedCars = 0;
    post.NumPermanentCars = 0;
    post.NumAmbulancesOnDuty = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCCarCtrl) from zpp/control/CarCtrl.zpp
constexpr bool op_CCarCtrl_ReInit(const CCarCtrl_ZppContract& pre, const CCarCtrl_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.NumRandomCars == 0) && (post.NumLawEnforcerCars == 0) && (post.NumMissionCars == 0) && (post.NumParkedCars == 0) && (post.NumPermanentCars == 0) && (post.NumAmbulancesOnDuty == 0);
}
constexpr CCarCtrl_ZppContract apply_CCarCtrl_ReInit(const CCarCtrl_ZppContract& pre) {
    CCarCtrl_ZppContract post = pre;
    post.NumRandomCars = 0;
    post.NumLawEnforcerCars = 0;
    post.NumMissionCars = 0;
    post.NumParkedCars = 0;
    post.NumPermanentCars = 0;
    post.NumAmbulancesOnDuty = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCCarCtrl) from zpp/control/CarCtrl.zpp
constexpr bool op_CCarCtrl_UpdateCarCount(const CCarCtrl_ZppContract& pre, const CCarCtrl_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.NumLawEnforcerCars == 0) && (post.NumRandomCars == 0) && (post.NumMissionCars == 0) && (post.NumParkedCars == 0) && (post.NumPermanentCars == 0);
}
constexpr CCarCtrl_ZppContract apply_CCarCtrl_UpdateCarCount(const CCarCtrl_ZppContract& pre) {
    CCarCtrl_ZppContract post = pre;
    post.NumLawEnforcerCars = 0;
    post.NumRandomCars = 0;
    post.NumMissionCars = 0;
    post.NumParkedCars = 0;
    post.NumPermanentCars = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCCarCtrl) from zpp/control/CarCtrl.zpp
constexpr bool op_CCarCtrl_ToggleScriptControlsMpCarLimit(const CCarCtrl_ZppContract& pre, const CCarCtrl_ZppContract& post, bool toggle_in = true) {
    return pre.checkInvariant() && post.checkInvariant() && (post.scriptControlsMpCarLimit == toggle_in);
}
constexpr CCarCtrl_ZppContract apply_CCarCtrl_ToggleScriptControlsMpCarLimit(const CCarCtrl_ZppContract& pre, bool toggle_in = true) {
    CCarCtrl_ZppContract post = pre;
    post.scriptControlsMpCarLimit = toggle_in;
    return post;
}
// Delta Operation Schema Contract (ΔCReplay) from zpp/control/Replay.zpp
constexpr bool op_CReplay_Init(const CReplay_ZppContract& pre, const CReplay_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pBuf0_card == 0u) && (post.pBuf1_card == 0u) && (post.pBuf2_card == 0u) && (post.pBuf3_card == 0u) && (post.pBuf4_card == 0u) && (post.pBuf5_card == 0u);
}
constexpr CReplay_ZppContract apply_CReplay_Init(const CReplay_ZppContract& pre) {
    CReplay_ZppContract post = pre;
    post.pBuf0_card = 0u;
    post.pBuf1_card = 0u;
    post.pBuf2_card = 0u;
    post.pBuf3_card = 0u;
    post.pBuf4_card = 0u;
    post.pBuf5_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCReplay) from zpp/control/Replay.zpp
constexpr bool op_CReplay_DisableReplays(const CReplay_ZppContract& pre, const CReplay_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bReplayEnabled == false);
}
constexpr CReplay_ZppContract apply_CReplay_DisableReplays(const CReplay_ZppContract& pre) {
    CReplay_ZppContract post = pre;
    post.bReplayEnabled = false;
    return post;
}
// Delta Operation Schema Contract (ΔCReplay) from zpp/control/Replay.zpp
constexpr bool op_CReplay_EnableReplays(const CReplay_ZppContract& pre, const CReplay_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bReplayEnabled == true);
}
constexpr CReplay_ZppContract apply_CReplay_EnableReplays(const CReplay_ZppContract& pre) {
    CReplay_ZppContract post = pre;
    post.bReplayEnabled = true;
    return post;
}
// Delta Operation Schema Contract (ΔCReplay) from zpp/control/Replay.zpp
constexpr bool op_CReplay_FastForwardToTime(const CReplay_ZppContract& pre, const CReplay_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.timer == 0);
}
constexpr CReplay_ZppContract apply_CReplay_FastForwardToTime(const CReplay_ZppContract& pre) {
    CReplay_ZppContract post = pre;
    post.timer = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCReplay) from zpp/control/Replay.zpp
constexpr bool op_CReplay_TriggerPlayback(const CReplay_ZppContract& pre, const CReplay_ZppContract& post, float cam_x_in = 2.5f, float cam_y_in = 2.5f, float cam_z_in = 2.5f, int32_t cam_mode_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.CameraFixedX == cam_x_in) && (post.CameraFixedY == cam_y_in) && (post.CameraFixedZ == cam_z_in) && (post.Mode == 1) && (post.FramesActiveLookAroundCam == 0) && (post.CameraMode == (int32_t)cam_mode_in);
}
constexpr CReplay_ZppContract apply_CReplay_TriggerPlayback(const CReplay_ZppContract& pre, float cam_x_in = 2.5f, float cam_y_in = 2.5f, float cam_z_in = 2.5f, int32_t cam_mode_in = 25) {
    CReplay_ZppContract post = pre;
    post.CameraFixedX = cam_x_in;
    post.CameraFixedY = cam_y_in;
    post.CameraFixedZ = cam_z_in;
    post.Mode = 1;
    post.FramesActiveLookAroundCam = 0;
    post.CameraMode = (int32_t)cam_mode_in;
    return post;
}
// Delta Operation Schema Contract (ΔCRestart) from zpp/control/Restart.zpp
constexpr bool op_CRestart_OverrideNextRestart(const CRestart_ZppContract& pre, const CRestart_ZppContract& post, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, float heading_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bOverrideRestart == true) && (post.OverridePosition == pos_in) && (post.OverrideHeading == heading_in);
}
constexpr CRestart_ZppContract apply_CRestart_OverrideNextRestart(const CRestart_ZppContract& pre, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, float heading_in = 2.5f) {
    CRestart_ZppContract post = pre;
    post.bOverrideRestart = true;
    post.OverridePosition = pos_in;
    post.OverrideHeading = heading_in;
    return post;
}
// Delta Operation Schema Contract (ΔCRestart) from zpp/control/Restart.zpp
constexpr bool op_CRestart_CancelOverrideRestart(const CRestart_ZppContract& pre, const CRestart_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bOverrideRestart == false);
}
constexpr CRestart_ZppContract apply_CRestart_CancelOverrideRestart(const CRestart_ZppContract& pre) {
    CRestart_ZppContract post = pre;
    post.bOverrideRestart = false;
    return post;
}
constexpr bool verify_control_zpp_invariants() {
    {
        CBridge_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBridge_ZppContract bad{};
        bad.pWeight_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarAI_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCarAI_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarCtrl_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCarCtrl_ZppContract bad{};
        bad.TotalNumOfCarsOfRating_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCurves_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCurves_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredCar_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredCar_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CGarage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CGarage_ZppContract bad{};
        bad.m_eGarageType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedPathNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedPathNode_ZppContract bad{};
        bad.nodeIdX = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPedPath_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPedPath_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPathNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPathNode_ZppContract bad{};
        bad.width = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarPathLink_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCarPathLink_ZppContract bad{};
        bad.numLeftLanes = 8u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPathInfoForObject_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPathInfoForObject_ZppContract bad{};
        bad.width = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTempNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTempNode_ZppContract bad{};
        bad.width = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTempNodeExternal_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTempNodeExternal_ZppContract bad{};
        bad.width = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRoute_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRoute_ZppContract bad{};
        bad.m_node_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPathFind_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPathFind_ZppContract bad{};
        bad.m_pathNodes_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTempDetachedNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTempDetachedNode_ZppContract bad{};
        bad.foo_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tPickupMessage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tPickupMessage_ZppContract bad{};
        bad.m_bOutOfStock = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPacManPickup_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPacManPickup_ZppContract bad{};
        bad.m_pObject_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPowerPoint_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPowerPoint_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPowerPoints_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPowerPoints_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tCoors_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tCoors_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarStateEachFrame_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCarStateEachFrame_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRecordDataForChase_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRecordDataForChase_ZppContract bad{};
        bad.pBaseMemForCar_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tGameBuffer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tGameBuffer_ZppContract bad{};
        bad.m_nSizeOfPads_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CRecordDataForGame_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRecordDataForGame_ZppContract bad{};
        bad.RecordingState = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRemote_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRemote_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CAddressInReplayBuffer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAddressInReplayBuffer_ZppContract bad{};
        bad.m_nOffset = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredAnimationState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredAnimationState_ZppContract bad{};
        bad.animId = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredDetailedAnimationState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredDetailedAnimationState_ZppContract bad{};
        bad.aAnimId_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CReplay_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CReplay_ZppContract bad{};
        bad.door_angles_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tGeneralPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tGeneralPacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tClockPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tClockPacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tWeatherPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tWeatherPacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tTimerPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tTimerPacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tPedHeaderPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tPedHeaderPacket_ZppContract bad{};
        bad.__align_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tBulletTracePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBulletTracePacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tEndOfFramePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tEndOfFramePacket_ZppContract bad{};
        bad.__align_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tPedUpdatePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tPedUpdatePacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tVehicleUpdatePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tVehicleUpdatePacket_ZppContract bad{};
        bad.door_angles_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tBikeUpdatePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBikeUpdatePacket_ZppContract bad{};
        bad.door_angles_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tParticlePacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tParticlePacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tMiscPacket_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tMiscPacket_ZppContract bad{};
        bad.type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRestart_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRestart_ZppContract bad{};
        bad.HospitalRestartPoints_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CScriptRoadblock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScriptRoadblock_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMovieCommand_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovieCommand_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSceneEdit_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSceneEdit_ZppContract bad{};
        bad.pActors_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredLine_ZppContract bad{};
        bad.color1 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMissionCleanup_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMissionCleanup_ZppContract bad{};
        bad.m_sEntities_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CUpsideDownCarCheck_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CUpsideDownCarCheck_ZppContract bad{};
        bad.m_sCars_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tCollectiveData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tCollectiveData_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tUsedObject_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tUsedObject_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tBuildingSwap_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBuildingSwap_ZppContract bad{};
        bad.m_pBuilding_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        Script10ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script10ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script2ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script2ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script3ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script3ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script4ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script4ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script5ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script5ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script6ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script6ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script7ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script7ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script8ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script8ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        Script9ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Script9ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        ScriptDebugModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ScriptDebugModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CSetPiece_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSetPiece_ZppContract bad{};
        bad.m_nType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTrafficLights_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrafficLights_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCarCtrl_ZppContract pre{};
        pre.NumRandomCars = 1;
        pre.NumLawEnforcerCars = 1;
        pre.NumMissionCars = 1;
        pre.NumParkedCars = 1;
        pre.NumPermanentCars = 1;
        pre.NumAmbulancesOnDuty = 1;
        const auto post = apply_CCarCtrl_Init(pre);
        if (!op_CCarCtrl_Init(pre, post)) return false;
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumRandomCars = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumLawEnforcerCars = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumMissionCars = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumParkedCars = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumPermanentCars = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumAmbulancesOnDuty = 1;
            if (op_CCarCtrl_Init(pre, badPost)) return false;
        }
    }
    {
        CCarCtrl_ZppContract pre{};
        pre.NumRandomCars = 1;
        pre.NumLawEnforcerCars = 1;
        pre.NumMissionCars = 1;
        pre.NumParkedCars = 1;
        pre.NumPermanentCars = 1;
        pre.NumAmbulancesOnDuty = 1;
        const auto post = apply_CCarCtrl_ReInit(pre);
        if (!op_CCarCtrl_ReInit(pre, post)) return false;
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumRandomCars = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumLawEnforcerCars = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumMissionCars = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumParkedCars = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumPermanentCars = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumAmbulancesOnDuty = 1;
            if (op_CCarCtrl_ReInit(pre, badPost)) return false;
        }
    }
    {
        CCarCtrl_ZppContract pre{};
        pre.NumLawEnforcerCars = 1;
        pre.NumRandomCars = 1;
        pre.NumMissionCars = 1;
        pre.NumParkedCars = 1;
        pre.NumPermanentCars = 1;
        const auto post = apply_CCarCtrl_UpdateCarCount(pre);
        if (!op_CCarCtrl_UpdateCarCount(pre, post)) return false;
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumLawEnforcerCars = 1;
            if (op_CCarCtrl_UpdateCarCount(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumRandomCars = 1;
            if (op_CCarCtrl_UpdateCarCount(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumMissionCars = 1;
            if (op_CCarCtrl_UpdateCarCount(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumParkedCars = 1;
            if (op_CCarCtrl_UpdateCarCount(pre, badPost)) return false;
        }
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.NumPermanentCars = 1;
            if (op_CCarCtrl_UpdateCarCount(pre, badPost)) return false;
        }
    }
    {
        CCarCtrl_ZppContract pre{};
        const bool toggle_in = true;
        pre.scriptControlsMpCarLimit = false;
        const auto post = apply_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, toggle_in);
        if (!op_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, post, toggle_in)) return false;
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.scriptControlsMpCarLimit = !toggle_in;
            if (op_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, badPost, toggle_in)) return false;
        }
    }
    {
        CReplay_ZppContract pre{};
        pre.pBuf0_card = 1u;
        pre.pBuf1_card = 1u;
        pre.pBuf2_card = 1u;
        pre.pBuf3_card = 1u;
        pre.pBuf4_card = 1u;
        pre.pBuf5_card = 1u;
        const auto post = apply_CReplay_Init(pre);
        if (!op_CReplay_Init(pre, post)) return false;
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf0_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf1_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf2_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf3_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf4_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.pBuf5_card = 1u;
            if (op_CReplay_Init(pre, badPost)) return false;
        }
    }
    {
        CReplay_ZppContract pre{};
        pre.bReplayEnabled = true;
        const auto post = apply_CReplay_DisableReplays(pre);
        if (!op_CReplay_DisableReplays(pre, post)) return false;
        {
            CReplay_ZppContract badPost = post;
            badPost.bReplayEnabled = true;
            if (op_CReplay_DisableReplays(pre, badPost)) return false;
        }
    }
    {
        CReplay_ZppContract pre{};
        pre.bReplayEnabled = false;
        const auto post = apply_CReplay_EnableReplays(pre);
        if (!op_CReplay_EnableReplays(pre, post)) return false;
        {
            CReplay_ZppContract badPost = post;
            badPost.bReplayEnabled = false;
            if (op_CReplay_EnableReplays(pre, badPost)) return false;
        }
    }
    {
        CReplay_ZppContract pre{};
        pre.timer = 1;
        const auto post = apply_CReplay_FastForwardToTime(pre);
        if (!op_CReplay_FastForwardToTime(pre, post)) return false;
        {
            CReplay_ZppContract badPost = post;
            badPost.timer = 1;
            if (op_CReplay_FastForwardToTime(pre, badPost)) return false;
        }
    }
    {
        CReplay_ZppContract pre{};
        const float cam_x_in = 2.5f;
        const float cam_y_in = 2.5f;
        const float cam_z_in = 2.5f;
        const int32_t cam_mode_in = 25;
        pre.CameraFixedX = 0.0f;
        pre.CameraFixedY = 0.0f;
        pre.CameraFixedZ = 0.0f;
        pre.Mode = 0;
        pre.FramesActiveLookAroundCam = 1;
        pre.CameraMode = 0;
        const auto post = apply_CReplay_TriggerPlayback(pre, cam_x_in, cam_y_in, cam_z_in, cam_mode_in);
        if (!op_CReplay_TriggerPlayback(pre, post, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        {
            CReplay_ZppContract badPost = post;
            badPost.CameraFixedX = cam_x_in + 1.0f;
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.CameraFixedY = cam_y_in + 1.0f;
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.CameraFixedZ = cam_z_in + 1.0f;
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.Mode = 0;
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.FramesActiveLookAroundCam = 1;
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
        {
            CReplay_ZppContract badPost = post;
            badPost.CameraMode = (int32_t)(cam_mode_in + 1);
            if (op_CReplay_TriggerPlayback(pre, badPost, cam_x_in, cam_y_in, cam_z_in, cam_mode_in)) return false;
        }
    }
    {
        CRestart_ZppContract pre{};
        const ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const float heading_in = 2.5f;
        pre.bOverrideRestart = false;
        pre.OverridePosition = ZppVec3{0.0f, 0.0f, 0.0f};
        pre.OverrideHeading = 0.0f;
        const auto post = apply_CRestart_OverrideNextRestart(pre, pos_in, heading_in);
        if (!op_CRestart_OverrideNextRestart(pre, post, pos_in, heading_in)) return false;
        {
            CRestart_ZppContract badPost = post;
            badPost.bOverrideRestart = false;
            if (op_CRestart_OverrideNextRestart(pre, badPost, pos_in, heading_in)) return false;
        }
        {
            CRestart_ZppContract badPost = post;
            badPost.OverridePosition = ZppVec3{pos_in.x + 1.0f, pos_in.y, pos_in.z};
            if (op_CRestart_OverrideNextRestart(pre, badPost, pos_in, heading_in)) return false;
        }
        {
            CRestart_ZppContract badPost = post;
            badPost.OverrideHeading = heading_in + 1.0f;
            if (op_CRestart_OverrideNextRestart(pre, badPost, pos_in, heading_in)) return false;
        }
    }
    {
        CRestart_ZppContract pre{};
        pre.bOverrideRestart = true;
        const auto post = apply_CRestart_CancelOverrideRestart(pre);
        if (!op_CRestart_CancelOverrideRestart(pre, post)) return false;
        {
            CRestart_ZppContract badPost = post;
            badPost.bOverrideRestart = true;
            if (op_CRestart_CancelOverrideRestart(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_control_zpp_invariants(), "Z++ formal contract invariant failed for control");
} // namespace zpp_control_spec
#endif // GTANGEL_ZPP_CONTROL_SPEC_GUARD
