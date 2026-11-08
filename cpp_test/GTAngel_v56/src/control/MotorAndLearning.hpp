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
    int32_t instance_addr{0}; // Z++: ADDRESS
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
    int32_t instance_addr{0}; // Z++: ADDRESS
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
    int32_t field_96{0}; // Z++: ADDRESS
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
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/control/PowerPoints.zpp
struct CPowerPoints_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
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
    int32_t instance_addr{0}; // Z++: ADDRESS
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
    constexpr bool checkInvariant() const {
        return (door_angles_len == 2u && door_angles.size() == 2u)
            && (wheel_susp_dist_len == 4u && wheel_susp_dist.size() == 4u)
            && (wheel_rotation_len == 4u && wheel_rotation.size() == 4u)
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
constexpr bool op_CCarCtrl_ToggleScriptControlsMpCarLimit(const CCarCtrl_ZppContract& pre, const CCarCtrl_ZppContract& post, bool scriptControlsMpCarLimit_in = true) {
    return pre.checkInvariant() && post.checkInvariant() && (post.scriptControlsMpCarLimit == scriptControlsMpCarLimit_in);
}
constexpr CCarCtrl_ZppContract apply_CCarCtrl_ToggleScriptControlsMpCarLimit(const CCarCtrl_ZppContract& pre, bool scriptControlsMpCarLimit_in = true) {
    CCarCtrl_ZppContract post = pre;
    post.scriptControlsMpCarLimit = scriptControlsMpCarLimit_in;
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
// Delta Operation Schema Contract (ΔCRestart) from zpp/control/Restart.zpp
constexpr bool op_CRestart_OverrideNextRestart(const CRestart_ZppContract& pre, const CRestart_ZppContract& post, ZppVec3 OverridePosition_in = ZppVec3{1.0f, 2.0f, 3.0f}, float OverrideHeading_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bOverrideRestart == true) && (post.OverridePosition == OverridePosition_in) && (post.OverrideHeading == OverrideHeading_in);
}
constexpr CRestart_ZppContract apply_CRestart_OverrideNextRestart(const CRestart_ZppContract& pre, ZppVec3 OverridePosition_in = ZppVec3{1.0f, 2.0f, 3.0f}, float OverrideHeading_in = 2.5f) {
    CRestart_ZppContract post = pre;
    post.bOverrideRestart = true;
    post.OverridePosition = OverridePosition_in;
    post.OverrideHeading = OverrideHeading_in;
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
        const bool scriptControlsMpCarLimit_in = true;
        pre.scriptControlsMpCarLimit = false;
        const auto post = apply_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, scriptControlsMpCarLimit_in);
        if (!op_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, post, scriptControlsMpCarLimit_in)) return false;
        {
            CCarCtrl_ZppContract badPost = post;
            badPost.scriptControlsMpCarLimit = !scriptControlsMpCarLimit_in;
            if (op_CCarCtrl_ToggleScriptControlsMpCarLimit(pre, badPost, scriptControlsMpCarLimit_in)) return false;
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
        CRestart_ZppContract pre{};
        const ZppVec3 OverridePosition_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const float OverrideHeading_in = 2.5f;
        pre.bOverrideRestart = false;
        pre.OverridePosition = ZppVec3{0.0f, 0.0f, 0.0f};
        pre.OverrideHeading = 0.0f;
        const auto post = apply_CRestart_OverrideNextRestart(pre, OverridePosition_in, OverrideHeading_in);
        if (!op_CRestart_OverrideNextRestart(pre, post, OverridePosition_in, OverrideHeading_in)) return false;
        {
            CRestart_ZppContract badPost = post;
            badPost.bOverrideRestart = false;
            if (op_CRestart_OverrideNextRestart(pre, badPost, OverridePosition_in, OverrideHeading_in)) return false;
        }
        {
            CRestart_ZppContract badPost = post;
            badPost.OverridePosition = ZppVec3{OverridePosition_in.x + 1.0f, OverridePosition_in.y, OverridePosition_in.z};
            if (op_CRestart_OverrideNextRestart(pre, badPost, OverridePosition_in, OverrideHeading_in)) return false;
        }
        {
            CRestart_ZppContract badPost = post;
            badPost.OverrideHeading = OverrideHeading_in + 1.0f;
            if (op_CRestart_OverrideNextRestart(pre, badPost, OverridePosition_in, OverrideHeading_in)) return false;
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
#ifndef GTANGEL_V16_VISUOMOTOR_CPP
#define GTANGEL_V16_VISUOMOTOR_CPP

#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace v16motor {

static constexpr uint32_t DatasetMagic = 0x4D565447u; // GTVM
static constexpr uint16_t DatasetVersion = 1;
static constexpr int TensorWidth = 96;
static constexpr int TensorHeight = 96;
static constexpr int TensorChannels = 3;
static constexpr int TensorPixels = TensorWidth * TensorHeight;
static constexpr int TensorBytes = TensorPixels * TensorChannels;
static constexpr int ProprioceptionSize = 16;
static constexpr int ControllerSize = 6;

enum class OutcomeFlags : uint32_t {
    None = 0,
    Collision = 1u << 0,
    GoalReached = 1u << 1,
    ShieldIntervened = 1u << 2,
    PeerAttentionPresent = 1u << 3,
};

static OutcomeFlags operator|(OutcomeFlags a, OutcomeFlags b) {
    return static_cast<OutcomeFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct VisualTensor96 {
    std::array<uint8_t, TensorBytes> values{};
    uint64_t sourceSequence{0};
    uint64_t sourceDigest{1469598103934665603ull};

    void derive(const v15render::HighResFramebuffer& frame) {
        sourceSequence = frame.sequence;
        sourceDigest = 1469598103934665603ull;
        static constexpr int Scale = v15render::HighResFramebuffer::Width / TensorWidth;
        static_assert(Scale == 8, "v16 tensor requires an exact 768-to-96 reduction");
        for (int ty = 0; ty < TensorHeight; ++ty) {
            for (int tx = 0; tx < TensorWidth; ++tx) {
                uint32_t luminanceSum = 0;
                float minimumDepth = 1e30f;
                std::array<int, 8> objectVotes{};
                for (int oy = 0; oy < Scale; ++oy) {
                    for (int ox = 0; ox < Scale; ++ox) {
                        const int sx = tx * Scale + ox;
                        const int sy = ty * Scale + oy;
                        const int source = sy * v15render::HighResFramebuffer::Width + sx;
                        const v15render::Colour colour = v15render::HighResFramebuffer::unpack(frame.colour[source]);
                        luminanceSum += static_cast<uint32_t>(0.2126f * colour.r + 0.7152f * colour.g + 0.0722f * colour.b);
                        minimumDepth = std::min(minimumDepth, frame.depth[source]);
                        const uint8_t objectId = frame.objectId[source];
                        if (objectId < objectVotes.size()) ++objectVotes[objectId];
                        sourceDigest ^= frame.colour[source];
                        sourceDigest *= 1099511628211ull;
                        sourceDigest ^= objectId;
                        sourceDigest *= 1099511628211ull;
                    }
                }
                const int destination = ty * TensorWidth + tx;
                values[destination] = static_cast<uint8_t>(luminanceSum / (Scale * Scale));
                const float boundedDepth = std::isfinite(minimumDepth) ? std::clamp(minimumDepth, 0.0f, 32.0f) : 32.0f;
                values[TensorPixels + destination] = static_cast<uint8_t>(std::lround((1.0f - boundedDepth / 32.0f) * 255.0f));
                const int objectId = static_cast<int>(std::max_element(objectVotes.begin(), objectVotes.end()) - objectVotes.begin());
                values[TensorPixels * 2 + destination] = static_cast<uint8_t>(objectId * 31);
            }
        }
    }
};

struct ProprioceptiveVector {
    std::array<float, ProprioceptionSize> values{};
};

struct ControllerTarget {
    std::array<float, ControllerSize> values{};
};

static ControllerTarget controllerFromDiscreteAction(int action) {
    ControllerTarget target;
    // turn, forward, strafe, lookYaw, lookPitch, actionGate
    switch (action) {
        case 0: target.values = {0.0f, 0.65f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        case 1: target.values = {-0.45f, 0.55f, 0.0f, -0.25f, 0.0f, 1.0f}; break;
        case 2: target.values = {0.45f, 0.55f, 0.0f, 0.25f, 0.0f, 1.0f}; break;
        case 3: target.values = {-0.80f, 0.0f, 0.0f, -0.60f, 0.0f, 1.0f}; break;
        case 4: target.values = {0.80f, 0.0f, 0.0f, 0.60f, 0.0f, 1.0f}; break;
        case 5: target.values = {0.0f, -0.35f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        case 7: target.values = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}; break;
        default: target.values = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; break;
    }
    return target;
}

#pragma pack(push, 1)
struct DatasetHeader {
    uint32_t magic{DatasetMagic};
    uint16_t version{DatasetVersion};
    uint16_t headerBytes{sizeof(DatasetHeader)};
    uint16_t tensorWidth{TensorWidth};
    uint16_t tensorHeight{TensorHeight};
    uint16_t tensorChannels{TensorChannels};
    uint16_t proprioceptionSize{ProprioceptionSize};
    uint16_t controllerSize{ControllerSize};
    uint16_t reserved{0};
    uint64_t episodeId{0};
    uint64_t createdAtMs{0};
};

struct RecordHeader {
    uint32_t recordBytes{sizeof(RecordHeader) + TensorBytes + sizeof(float) * (ProprioceptionSize + ControllerSize)};
    uint32_t step{0};
    uint64_t timestampMs{0};
    uint64_t frameSequence{0};
    uint64_t frameDigest{0};
    uint32_t outcomeFlags{0};
    float reward{0.0f};
    float modelConfidence{0.0f};
    float collisionRisk{0.0f};
};
#pragma pack(pop)

class TrajectoryRecorder {
    std::ofstream stream_;
    std::string path_;
    std::string directory_;
    std::string lastError_;
    std::string ledgerPath_;
    uint64_t episodeId_{0};
    uint32_t records_{0};
    uint64_t bytes_{0};
    bool active_{false};
    bool ready_{false};

    static uint64_t nowMs() {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }

    static std::string jsonEscape(const std::string& value) {
        std::string escaped;
        escaped.reserve(value.size() + 8);
        for (char character : value) {
            if (character == '\\' || character == '"') escaped.push_back('\\');
            if (character == '\n') { escaped += "\\n"; continue; }
            if (character == '\r') { escaped += "\\r"; continue; }
            escaped.push_back(character);
        }
        return escaped;
    }

    void appendLedger(const std::string& event, const std::string& detail) const {
        if (ledgerPath_.empty()) return;
        std::ofstream ledger(ledgerPath_, std::ios::app);
        if (!ledger) return;
        ledger << "{\"timestampMs\":" << nowMs()
               << ",\"event\":\"" << jsonEscape(event)
               << "\",\"detail\":\"" << jsonEscape(detail) << "\"}\n";
    }

    bool selectDirectory(const std::string& preferred) {
        std::array<std::filesystem::path, 3> candidates{};
        candidates[0] = std::filesystem::path(preferred);
        std::error_code error;
        candidates[1] = std::filesystem::current_path(error) / "datasets";
        error.clear();
        candidates[2] = std::filesystem::temp_directory_path(error) / "GTAngel" / "datasets";
        for (const auto& candidate : candidates) {
            error.clear();
            std::filesystem::create_directories(candidate, error);
            if (error) continue;
            const std::filesystem::path probe = candidate / ".gtvm_write_probe";
            std::ofstream test(probe.string(), std::ios::binary | std::ios::trunc);
            if (!test) continue;
            test.write("GTVM", 4);
            test.close();
            error.clear();
            std::filesystem::remove(probe, error);
            directory_ = std::filesystem::absolute(candidate, error).string();
            if (directory_.empty()) directory_ = candidate.string();
            ledgerPath_ = (std::filesystem::path(directory_) / "gtangel_v16_capture_ledger.jsonl").string();
            ready_ = true;
            lastError_.clear();
            appendLedger("capture.self_check.passed", directory_);
            return true;
        }
        ready_ = false;
        lastError_ = "No writable dataset directory (preferred, current, or temporary)";
        return false;
    }

public:
    bool selfCheck(const std::string& preferred) {
        stop();
        return selectDirectory(preferred);
    }

    bool start(const std::string& directory, uint64_t episodeId) {
        stop();
        if (!ready_ || (directory_ != directory && !selectDirectory(directory))) {
            if (!ready_ && !selectDirectory(directory)) return false;
        }
        episodeId_ = episodeId;
        std::ostringstream filename;
        filename << directory_ << "/gtvm_episode_" << episodeId << "_" << nowMs() << ".gtvm";
        path_ = filename.str();
        stream_.open(path_, std::ios::binary | std::ios::trunc);
        if (!stream_) {
            lastError_ = "Unable to open " + path_;
            appendLedger("capture.start.failed", lastError_);
            return false;
        }
        DatasetHeader header;
        header.episodeId = episodeId;
        header.createdAtMs = nowMs();
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        active_ = stream_.good();
        if (!active_) {
            lastError_ = "Unable to write dataset header to " + path_;
            appendLedger("capture.start.failed", lastError_);
            return false;
        }
        records_ = 0;
        bytes_ = sizeof(header);
        lastError_.clear();
        appendLedger("capture.started", path_);
        return active_;
    }

    bool stop() {
        if (stream_.is_open()) {
            stream_.flush();
            const bool flushFailed = !stream_.good();
            stream_.close();
            const bool closeFailed = stream_.fail();
            if (flushFailed || closeFailed) {
                lastError_ = std::string(flushFailed ? "Dataset flush failed" : "Dataset close failed") + " for " + path_;
                ready_ = false;
                active_ = false;
                appendLedger("capture.close.failed", lastError_);
                return false;
            }
            appendLedger("capture.stopped", path_ + ";records=" + std::to_string(records_));
        }
        active_ = false;
        return true;
    }

    bool capture(uint32_t step,
                 const VisualTensor96& visual,
                 const ProprioceptiveVector& proprioception,
                 const ControllerTarget& controller,
                 OutcomeFlags outcome,
                 float reward,
                 float modelConfidence,
                 float collisionRisk) {
        if (!active_) return false;
        RecordHeader header;
        header.step = step;
        header.timestampMs = nowMs();
        header.frameSequence = visual.sourceSequence;
        header.frameDigest = visual.sourceDigest;
        header.outcomeFlags = static_cast<uint32_t>(outcome);
        header.reward = reward;
        header.modelConfidence = std::clamp(modelConfidence, 0.0f, 1.0f);
        header.collisionRisk = std::clamp(collisionRisk, 0.0f, 1.0f);
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        stream_.write(reinterpret_cast<const char*>(visual.values.data()), visual.values.size());
        stream_.write(reinterpret_cast<const char*>(proprioception.values.data()), sizeof(float) * proprioception.values.size());
        stream_.write(reinterpret_cast<const char*>(controller.values.data()), sizeof(float) * controller.values.size());
        if (!stream_.good()) {
            lastError_ = "Dataset write failed for " + path_;
            appendLedger("capture.write.failed", lastError_);
            stop();
            return false;
        }
        ++records_;
        bytes_ += header.recordBytes;
        if ((records_ % 32u) == 0u) stream_.flush();
        return true;
    }

    bool active() const { return active_; }
    bool ready() const { return ready_; }
    uint32_t records() const { return records_; }
    uint64_t bytesWritten() const { return bytes_; }
    const std::string& path() const { return path_; }
    const std::string& directory() const { return directory_; }
    const std::string& lastError() const { return lastError_; }
};

static ProprioceptiveVector makeProprioception(float speed,
                                                float heading,
                                                float headingDelta,
                                                bool collided,
                                                bool grounded,
                                                const ControllerTarget& previous,
                                                float arousal,
                                                float reward,
                                                float visualConfidence,
                                                float visualCollisionRisk) {
    ProprioceptiveVector vector;
    vector.values[0] = speed;
    vector.values[1] = std::sin(heading);
    vector.values[2] = std::cos(heading);
    vector.values[3] = headingDelta;
    vector.values[4] = collided ? 1.0f : 0.0f;
    vector.values[5] = grounded ? 1.0f : 0.0f;
    for (int index = 0; index < ControllerSize; ++index) vector.values[6 + index] = previous.values[index];
    vector.values[12] = arousal;
    vector.values[13] = reward;
    vector.values[14] = visualConfidence;
    vector.values[15] = visualCollisionRisk;
    return vector;
}

} // namespace v16motor

#ifndef GTANGEL_V17_NEURAL_POLICY_CPP
#define GTANGEL_V17_NEURAL_POLICY_CPP

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace v17policy {

static constexpr uint32_t WeightMagic = 0x57564D47u;
static constexpr uint16_t WeightVersion = 1;
static constexpr int HiddenSize = 64;

struct Tensor {
    std::vector<uint32_t> shape;
    std::vector<float> values;
};

struct Prediction {
    v16motor::ControllerTarget controller;
    std::array<float, 3> egoMotion{};
    std::array<float, 288> cognition{};
    float collisionProbability{1.0f};
    float uncertainty{1.0f};
    float latencyMs{0.0f};
    bool valid{false};
};

struct ShieldDecision {
    v16motor::ControllerTarget proposal;
    v16motor::ControllerTarget committed;
    float disagreementRms{1.0f};
    float confidence{0.0f};
    bool accepted{false};
    std::string reason{"model unavailable"};
};

class NativeVisuomotorPolicy {
    std::unordered_map<std::string, Tensor> tensors_;
    std::array<float, HiddenSize> hidden_{};
    std::array<float, v16motor::ControllerSize> previousController_{};
    bool hasPreviousController_{false};
    Prediction prediction_{};
    bool loaded_{false};
    std::string path_;
    std::string error_;

    template<typename T>
    static bool readValue(std::ifstream& stream, T& value) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
        return stream.good();
    }

    const Tensor& tensor(const char* name) const {
        return tensors_.at(name);
    }

    static float sigmoid(float value) {
        value = std::clamp(value, -20.0f, 20.0f);
        return 1.0f / (1.0f + std::exp(-value));
    }

    static float silu(float value) {
        return value * sigmoid(value);
    }

    static bool finiteVector(const std::vector<float>& values) {
        return std::all_of(values.begin(), values.end(), [](float value) { return std::isfinite(value); });
    }

    bool validateTensor(const std::string& name, std::initializer_list<uint32_t> shape) {
        const auto found = tensors_.find(name);
        if (found == tensors_.end()) { error_ = "missing tensor " + name; return false; }
        if (found->second.shape != std::vector<uint32_t>(shape)) { error_ = "shape mismatch for " + name; return false; }
        if (!finiteVector(found->second.values)) { error_ = "non-finite tensor " + name; return false; }
        return true;
    }

public:
    bool load(const std::string& path) {
        loaded_ = false;
        tensors_.clear();
        path_ = path;
        error_.clear();
        std::ifstream stream(path, std::ios::binary);
        if (!stream) { error_ = "unable to open " + path; return false; }
        uint32_t magic = 0;
        uint16_t version = 0, tensorCount = 0;
        if (!readValue(stream, magic) || !readValue(stream, version) || !readValue(stream, tensorCount)) {
            error_ = "truncated weight header"; return false;
        }
        if (magic != WeightMagic || version != WeightVersion || tensorCount != 20) {
            error_ = "unsupported weight bundle"; return false;
        }
        for (uint16_t index = 0; index < tensorCount; ++index) {
            uint16_t nameLength = 0;
            uint8_t dimensions = 0, dtype = 0;
            uint32_t elements = 0;
            if (!readValue(stream, nameLength) || !readValue(stream, dimensions) ||
                !readValue(stream, dtype) || !readValue(stream, elements)) {
                error_ = "truncated tensor header"; return false;
            }
            if (nameLength == 0 || dimensions == 0 || dimensions > 4 || dtype != 1 || elements == 0) {
                error_ = "invalid tensor header"; return false;
            }
            std::string name(nameLength, '\0');
            stream.read(name.data(), nameLength);
            Tensor value;
            value.shape.resize(dimensions);
            uint64_t shapeElements = 1;
            for (uint8_t dimension = 0; dimension < dimensions; ++dimension) {
                if (!readValue(stream, value.shape[dimension])) { error_ = "truncated tensor shape"; return false; }
                shapeElements *= value.shape[dimension];
            }
            if (shapeElements != elements) { error_ = "tensor element mismatch"; return false; }
            value.values.resize(elements);
            stream.read(reinterpret_cast<char*>(value.values.data()), sizeof(float) * elements);
            if (!stream.good() || tensors_.count(name)) { error_ = "invalid tensor payload"; return false; }
            tensors_.emplace(std::move(name), std::move(value));
        }
        char trailing = 0;
        if (stream.read(&trailing, 1)) { error_ = "trailing weight bytes"; return false; }

        loaded_ =
            validateTensor("visual.0.weight", {8,3,5,5}) && validateTensor("visual.0.bias", {8}) &&
            validateTensor("visual.2.weight", {16,8,3,3}) && validateTensor("visual.2.bias", {16}) &&
            validateTensor("visual.6.weight", {64,144}) && validateTensor("visual.6.bias", {64}) &&
            validateTensor("proprioception.0.weight", {32,16}) && validateTensor("proprioception.0.bias", {32}) &&
            validateTensor("recurrent.weight_ih", {192,96}) && validateTensor("recurrent.weight_hh", {192,64}) &&
            validateTensor("recurrent.bias_ih", {192}) && validateTensor("recurrent.bias_hh", {192}) &&
            validateTensor("controller.weight", {6,64}) && validateTensor("controller.bias", {6}) &&
            validateTensor("collision.weight", {1,64}) && validateTensor("collision.bias", {1}) &&
            validateTensor("ego_motion.weight", {3,64}) && validateTensor("ego_motion.bias", {3}) &&
            validateTensor("cognition.weight", {288,64}) && validateTensor("cognition.bias", {288});
        reset();
        return loaded_;
    }

    void reset() {
        hidden_.fill(0.0f);
        previousController_.fill(0.0f);
        hasPreviousController_ = false;
        prediction_ = Prediction{};
    }

    Prediction infer(const v16motor::VisualTensor96& visual,
                     const v16motor::ProprioceptiveVector& proprioception) {
        const auto started = std::chrono::high_resolution_clock::now();
        prediction_ = Prediction{};
        if (!loaded_) return prediction_;

        const Tensor& conv1Weight = tensor("visual.0.weight");
        const Tensor& conv1Bias = tensor("visual.0.bias");
        std::vector<float> conv1(8 * 24 * 24);
        for (int outputChannel = 0; outputChannel < 8; ++outputChannel) {
            for (int outputY = 0; outputY < 24; ++outputY) {
                for (int outputX = 0; outputX < 24; ++outputX) {
                    float sum = conv1Bias.values[outputChannel];
                    for (int inputChannel = 0; inputChannel < 3; ++inputChannel) {
                        for (int kernelY = 0; kernelY < 5; ++kernelY) {
                            for (int kernelX = 0; kernelX < 5; ++kernelX) {
                                const int inputY = outputY * 4 + kernelY - 2;
                                const int inputX = outputX * 4 + kernelX - 2;
                                if (inputY < 0 || inputY >= 96 || inputX < 0 || inputX >= 96) continue;
                                const float input = visual.values[inputChannel * 96 * 96 + inputY * 96 + inputX] / 255.0f;
                                const int weightIndex = (((outputChannel * 3 + inputChannel) * 5 + kernelY) * 5 + kernelX);
                                sum += conv1Weight.values[weightIndex] * input;
                            }
                        }
                    }
                    conv1[(outputChannel * 24 + outputY) * 24 + outputX] = silu(sum);
                }
            }
        }

        const Tensor& conv2Weight = tensor("visual.2.weight");
        const Tensor& conv2Bias = tensor("visual.2.bias");
        std::vector<float> conv2(16 * 12 * 12);
        for (int outputChannel = 0; outputChannel < 16; ++outputChannel) {
            for (int outputY = 0; outputY < 12; ++outputY) {
                for (int outputX = 0; outputX < 12; ++outputX) {
                    float sum = conv2Bias.values[outputChannel];
                    for (int inputChannel = 0; inputChannel < 8; ++inputChannel) {
                        for (int kernelY = 0; kernelY < 3; ++kernelY) {
                            for (int kernelX = 0; kernelX < 3; ++kernelX) {
                                const int inputY = outputY * 2 + kernelY - 1;
                                const int inputX = outputX * 2 + kernelX - 1;
                                if (inputY < 0 || inputY >= 24 || inputX < 0 || inputX >= 24) continue;
                                const int inputIndex = (inputChannel * 24 + inputY) * 24 + inputX;
                                const int weightIndex = (((outputChannel * 8 + inputChannel) * 3 + kernelY) * 3 + kernelX);
                                sum += conv2Weight.values[weightIndex] * conv1[inputIndex];
                            }
                        }
                    }
                    conv2[(outputChannel * 12 + outputY) * 12 + outputX] = silu(sum);
                }
            }
        }

        std::array<float, 144> pooled{};
        for (int channel = 0; channel < 16; ++channel) {
            for (int poolY = 0; poolY < 3; ++poolY) {
                for (int poolX = 0; poolX < 3; ++poolX) {
                    float sum = 0.0f;
                    for (int y = 0; y < 4; ++y)
                        for (int x = 0; x < 4; ++x)
                            sum += conv2[(channel * 12 + poolY * 4 + y) * 12 + poolX * 4 + x];
                    pooled[(channel * 3 + poolY) * 3 + poolX] = sum / 16.0f;
                }
            }
        }

        std::array<float, 64> visualLatent{};
        const Tensor& visualWeight = tensor("visual.6.weight");
        const Tensor& visualBias = tensor("visual.6.bias");
        for (int output = 0; output < 64; ++output) {
            float sum = visualBias.values[output];
            for (int input = 0; input < 144; ++input) sum += visualWeight.values[output * 144 + input] * pooled[input];
            visualLatent[output] = silu(sum);
        }

        std::array<float, 32> proprioLatent{};
        const Tensor& proprioWeight = tensor("proprioception.0.weight");
        const Tensor& proprioBias = tensor("proprioception.0.bias");
        for (int output = 0; output < 32; ++output) {
            float sum = proprioBias.values[output];
            for (int input = 0; input < 16; ++input) sum += proprioWeight.values[output * 16 + input] * proprioception.values[input];
            proprioLatent[output] = std::tanh(sum);
        }

        std::array<float, 96> fused{};
        std::copy(visualLatent.begin(), visualLatent.end(), fused.begin());
        std::copy(proprioLatent.begin(), proprioLatent.end(), fused.begin() + 64);
        const Tensor& inputWeight = tensor("recurrent.weight_ih");
        const Tensor& hiddenWeight = tensor("recurrent.weight_hh");
        const Tensor& inputBias = tensor("recurrent.bias_ih");
        const Tensor& hiddenBias = tensor("recurrent.bias_hh");
        std::array<float, 192> inputGates{}, hiddenGates{};
        for (int gate = 0; gate < 192; ++gate) {
            float inputSum = inputBias.values[gate];
            float hiddenSum = hiddenBias.values[gate];
            for (int input = 0; input < 96; ++input) inputSum += inputWeight.values[gate * 96 + input] * fused[input];
            for (int hidden = 0; hidden < 64; ++hidden) hiddenSum += hiddenWeight.values[gate * 64 + hidden] * hidden_[hidden];
            inputGates[gate] = inputSum;
            hiddenGates[gate] = hiddenSum;
        }
        std::array<float, HiddenSize> nextHidden{};
        for (int index = 0; index < HiddenSize; ++index) {
            const float resetGate = sigmoid(inputGates[index] + hiddenGates[index]);
            const float updateGate = sigmoid(inputGates[64 + index] + hiddenGates[64 + index]);
            const float newGate = std::tanh(inputGates[128 + index] + resetGate * hiddenGates[128 + index]);
            nextHidden[index] = newGate + updateGate * (hidden_[index] - newGate);
        }
        hidden_ = nextHidden;

        const Tensor& controllerWeight = tensor("controller.weight");
        const Tensor& controllerBias = tensor("controller.bias");
        for (int output = 0; output < 6; ++output) {
            float sum = controllerBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += controllerWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.controller.values[output] = std::tanh(sum);
        }
        float temporalSquared = 0.0f;
        for (int output = 0; output < v16motor::ControllerSize; ++output) {
            const float difference = prediction_.controller.values[output] - previousController_[output];
            temporalSquared += difference * difference;
            previousController_[output] = prediction_.controller.values[output];
        }
        const float temporalRms = std::sqrt(temporalSquared / v16motor::ControllerSize);
        const float closedGatePenalty = std::max(0.0f, 0.5f - prediction_.controller.values[5]);
        prediction_.uncertainty = hasPreviousController_
            ? std::clamp(temporalRms / 0.50f + closedGatePenalty, 0.0f, 1.0f)
            : 1.0f;
        hasPreviousController_ = true;
        const Tensor& collisionWeight = tensor("collision.weight");
        float collision = tensor("collision.bias").values[0];
        for (int hidden = 0; hidden < 64; ++hidden) collision += collisionWeight.values[hidden] * hidden_[hidden];
        prediction_.collisionProbability = sigmoid(collision);
        const Tensor& egoWeight = tensor("ego_motion.weight");
        const Tensor& egoBias = tensor("ego_motion.bias");
        for (int output = 0; output < 3; ++output) {
            float sum = egoBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += egoWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.egoMotion[output] = sum;
        }
        const Tensor& cognitionWeight = tensor("cognition.weight");
        const Tensor& cognitionBias = tensor("cognition.bias");
        for (int output = 0; output < 288; ++output) {
            float sum = cognitionBias.values[output];
            for (int hidden = 0; hidden < 64; ++hidden) sum += cognitionWeight.values[output * 64 + hidden] * hidden_[hidden];
            prediction_.cognition[output] = sigmoid(sum);
        }
        prediction_.valid = true;
        for (float value : prediction_.controller.values) prediction_.valid = prediction_.valid && std::isfinite(value);
        prediction_.valid = prediction_.valid && std::isfinite(prediction_.collisionProbability);
        const auto finished = std::chrono::high_resolution_clock::now();
        prediction_.latencyMs = std::chrono::duration<float, std::milli>(finished - started).count();
        return prediction_;
    }

    bool loaded() const { return loaded_; }
    const std::string& path() const { return path_; }
    const std::string& error() const { return error_; }
    const Prediction& prediction() const { return prediction_; }
};

class EmbodiedSafetyShield {
public:
    ShieldDecision decide(const Prediction& prediction,
                          const v16motor::ControllerTarget& expert,
                          float visualCollisionRisk,
                          bool assistRequested,
                          bool ethicsApproved = true) const {
        ShieldDecision decision;
        decision.proposal = prediction.controller;
        decision.committed = expert;
        float squared = 0.0f;
        for (int index = 0; index < v16motor::ControllerSize; ++index) {
            const float difference = prediction.controller.values[index] - expert.values[index];
            squared += difference * difference;
        }
        decision.disagreementRms = std::sqrt(squared / v16motor::ControllerSize);
        decision.confidence = prediction.valid ? std::clamp(1.0f - decision.disagreementRms / 0.45f, 0.0f, 1.0f) : 0.0f;
        if (!prediction.valid) { decision.reason = "invalid inference"; return decision; }
        if (!assistRequested) { decision.reason = "shadow mode"; return decision; }
        if (!ethicsApproved) { decision.reason = "ethics gate"; return decision; }
        if (prediction.uncertainty > 0.35f) { decision.reason = "uncertainty gate"; return decision; }
        if (visualCollisionRisk > 0.20f) { decision.reason = "visual collision gate"; return decision; }
        if (prediction.collisionProbability > 0.20f) { decision.reason = "model collision gate"; return decision; }
        if (decision.disagreementRms > 0.18f) { decision.reason = "expert disagreement"; return decision; }
        if (prediction.controller.values[5] < 0.50f) { decision.reason = "action gate closed"; return decision; }
        decision.committed = prediction.controller;
        decision.committed.values[0] = std::clamp(decision.committed.values[0], -0.8f, 0.8f);
        decision.committed.values[1] = std::clamp(decision.committed.values[1], -0.35f, 1.0f);
        decision.committed.values[2] = 0.0f;
        decision.committed.values[3] = std::clamp(decision.committed.values[3], -0.6f, 0.6f);
        decision.committed.values[4] = 0.0f;
        decision.committed.values[5] = 1.0f;
        decision.accepted = true;
        decision.reason = "accepted by local shield";
        return decision;
    }
};

} // namespace v17policy

// v27 observational reinforcement learning. No policy selection or effectors.
namespace v27learn {
struct OnlineNavigationObserver {
    uint64_t updates{0}, rejected{0}, collisions{0}, windows{0};
    uint32_t stateVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST]{};
    uint32_t actionVisits[avatar3d::NavTrainer::BEARINGS][avatar3d::NavTrainer::DISTS][avatar3d::NavTrainer::OBST][avatar3d::NavTrainer::COUNT]{};
    uint32_t windowSteps{0};
    float windowReturn{0}, lastReward{0}, lastQDelta{0};
    static bool valid(const avatar3d::NavTrainer::State& s) {
        return s.bearing>=0 && s.bearing<avatar3d::NavTrainer::BEARINGS &&
               s.dist>=0 && s.dist<avatar3d::NavTrainer::DISTS &&
               s.obst>=0 && s.obst<avatar3d::NavTrainer::OBST;
    }
    static avatar3d::NavTrainer::Action action(float forward,float turn) {
        using N=avatar3d::NavTrainer;
        if(forward<-.01f) return N::BACK;
        if(forward>.01f) return turn<-.01f?N::FWD_L:turn>.01f?N::FWD_R:N::FWD;
        return turn<-.01f?N::TURN_L:turn>.01f?N::TURN_R:N::WAIT;
    }
    bool observe(avatar3d::NavTrainer& q,
                 const v15render::VisualNavEvidence& before,
                 const v15render::VisualNavEvidence& after,
                 float forward,float turn,float reward,bool humanAccepted,
                 bool contact,bool onFoot,bool enabled) {
        if(!enabled) return false;
        if(!humanAccepted || !onFoot || before.sourceSequence==0 ||
           after.sourceSequence<=before.sourceSequence || !valid(before.state) || !valid(after.state) ||
           !std::isfinite(forward) || !std::isfinite(turn) || !std::isfinite(reward) ||
           std::fabs(forward)>1 || std::fabs(turn)>1 || std::fabs(reward)>2 ||
           (std::fabs(forward)<.01f && std::fabs(turn)<.01f)) {
            ++rejected; return false;
        }
        const auto a=action(forward,turn);
        const float old=q.Q[before.state.bearing][before.state.dist][before.state.obst][a];
        q.update(before.state,a,reward,after.state);
        lastQDelta=q.Q[before.state.bearing][before.state.dist][before.state.obst][a]-old;
        ++updates; ++q.stepsTotal; ++windowSteps;
        ++stateVisits[before.state.bearing][before.state.dist][before.state.obst];
        ++actionVisits[before.state.bearing][before.state.dist][before.state.obst][a];
        if(contact) { ++collisions; ++q.collisions; }
        lastReward=reward; windowReturn+=reward;
        // These are fixed observational windows, NOT successful goal episodes.
        if(windowSteps==200) {
            ++windows; ++q.episodes;
            q.returnsPerEpisode.push_back(windowReturn);
            q.stepsPerEpisode.push_back(static_cast<int>(windowSteps));
            if(q.returnsPerEpisode.size()>1000) {
                q.returnsPerEpisode.erase(q.returnsPerEpisode.begin());
                q.stepsPerEpisode.erase(q.stepsPerEpisode.begin());
            }
            windowSteps=0; windowReturn=0;
        }
        return true;
    }
};
} // namespace v27learn
namespace v27dte {
struct ControllerProposal { float forward{0},turn{0},uncertainty{.2f}; bool learned{false}; };
class LocalNavigation {
    std::mt19937 rng_{27001};
    uint32_t escapeTicks_{0};
    uint32_t avoidHoldTicks_{0};
    float avoidTurnDir_{0.46f};
public:
    uint64_t proposals{0},learnedProposals{0},accepted{0},contacts{0};
    void reset(){*this=LocalNavigation{};}
    ControllerProposal propose(const v15render::VisualNavEvidence& v,
            const avatar3d::NavTrainer& q,const v27learn::OnlineNavigationObserver& learning,
            bool contact,int questMode=0,float goalCompass=10.0f) {
        ControllerProposal p;
        if(v.sourceSequence==0 || !v27learn::OnlineNavigationObserver::valid(v.state))return p;
        ++proposals;
        if(contact) {
            ++contacts;
            if (escapeTicks_ == 0)
                avoidTurnDir_ = (v.state.obst < 4) ? 0.48f : -0.48f;
            escapeTicks_ = 10;
        }
        // LW35: decisive non-oscillating unstick + arc around obstacles.
        if(escapeTicks_) {
            --escapeTicks_;
            p.forward = (escapeTicks_ >= 7) ? -0.18f : 0.24f;
            p.turn = avoidTurnDir_;
            return p;
        }
        if(v.obstacleVisible && v.nearestObstacleDepth<2.4f) {
            if (avoidHoldTicks_ == 0) {
                avoidTurnDir_ = (v.state.obst < 4) ? 0.46f : -0.46f;
                avoidHoldTicks_ = 12;
            } else {
                --avoidHoldTicks_;
            }
            p.turn = avoidTurnDir_;
            p.forward = v.nearestObstacleDepth < 1.1f ? 0.14f : 0.25f;
            return p;
        }
        if (avoidHoldTicks_ > 0) --avoidHoldTicks_;
        if (std::fabs(goalCompass) <= 3.1416f) {
            // LW38: authoritative egocentric bearing to the active quest target
            // (Ariadne, Signal Shard, Civic Beacon, or uncollected Data Cell).
            // Never let random ObjectNpc walkers hijack questMode==0 steering!
            // Pivot tightly when >25 deg off-heading so we never orbit past targets.
            p.turn = std::clamp(goalCompass * 0.90f, -0.55f, 0.55f);
            p.forward = std::fabs(goalCompass) > 0.45f ? 0.08f : 0.46f;
        } else if(questMode==1&&v.shardVisible) {
            const float bearing=(v.shardBearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else if(questMode==0&&v.npcVisible) {
            const float bearing=(v.npcBearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else if(v.goalVisible) {
            const float bearing=(v.state.bearing-3.5f)/3.5f;
            p.turn=std::clamp(bearing*.5f,-.42f,.42f);
            p.forward=std::fabs(bearing)>.6f?.22f:.46f;
        } else {
            p.forward=.24f;
            const int b=v.state.bearing;
            const int bl=(b+7)&7, br=(b+1)&7;
            const uint32_t vl=learning.stateVisits[bl][v.state.dist][v.state.obst];
            const uint32_t vr=learning.stateVisits[br][v.state.dist][v.state.obst];
            p.turn=(vl<vr)?-.30f:(vr<vl)?.30f:.28f;
        }
        // LW38: blend visited Q-policy choices with continuous goal steering
        // only when already roughly aligned with the target (|goalCompass|<0.45)
        // so Q-discretization never overrides pivot-to-target alignment.
        if(learning.stateVisits[v.state.bearing][v.state.dist][v.state.obst]>=8) {
            std::uniform_real_distribution<float> unit(0,1);
            if(unit(rng_)>q.epsilon) {
                float best=-1e30f;int chosen=-1;
                for(int a=0;a<avatar3d::NavTrainer::COUNT;++a)
                    if(learning.actionVisits[v.state.bearing][v.state.dist][v.state.obst][a]>0 &&
                       q.Q[v.state.bearing][v.state.dist][v.state.obst][a]>best) {
                        best=q.Q[v.state.bearing][v.state.dist][v.state.obst][a];chosen=a;
                    }
                if(chosen>=0) {
                    using N=avatar3d::NavTrainer;
                    const auto a=static_cast<N::Action>(chosen);
                    const float qFwd=(a==N::FWD||a==N::FWD_L||a==N::FWD_R)?0.46f:0.24f;
                    const float qTurn=(a==N::TURN_L||a==N::FWD_L)?-0.40f:(a==N::TURN_R||a==N::FWD_R)?0.40f:0.0f;
                    const float w = (std::fabs(goalCompass) > 0.45f) ? 0.05f : ((best > 0.0f) ? 0.25f : 0.10f);
                    p.forward = std::clamp((1.0f - w) * p.forward + w * qFwd, 0.08f, 0.48f);
                    p.turn    = std::clamp((1.0f - w) * p.turn    + w * qTurn, -0.55f, 0.55f);
                    p.learned=true;++learnedProposals;
                }
            }
        }
        return p;
    }
};
// Echo persona (superhot-gamer-girl personas/echo.json): resolves the live
// 10-channel endocrine bus into the persona emotion vocabulary used by
// echo.character.json endocrine.event_map / echo.live2d.json expressions.
// Pure classification: it never proposes, vetoes, or actuates anything.
struct EchoPersonaState {
    const char* emotion{"BORED_STOMPING"};   // echo.live2d.json key
    const char* expression{"PLAYFUL_BORED"};// Cubism expression name
    const char* facs{"AU43 AU15"};          // FACS AU set (display contract)
    float spice{0.0f};                      // resolved adaptive spice 0..1
    float confidence{0.0f};                // 1 - margin to nearest boundary
    uint64_t updates{0};
};
class EchoPersona {
public:
    EchoPersonaState resolve(double valence,double arousal,int cognitiveMode) {
        // Thresholds mirror dte::EndocrineSystem::computeCognitiveMode boundaries.
        const bool alarmed = cognitiveMode==3; // Alarm
        const bool resting  = cognitiveMode==2; // Rest
        const bool focused = cognitiveMode==1;  // Exploitation
        const char* emotion; const char* expression; const char* facs; float spice;
        if (alarmed && valence<-0.1) { emotion="GETTING_TILTED"; expression="FRUSTRATED_FOCUS"; facs="AU4 AU7 AU23"; spice=0.40f; }
        else if (valence>0.35 && arousal>0.15) { emotion="VICTORY_ROYALE"; expression="TRIUMPHANT_GRIN"; facs="AU6 AU12 AU25"; spice=0.40f; }
        else if (valence>0.35 && arousal<=0.15) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else if (valence>0.15 && arousal>0.25) { emotion="CLUTCH_MOMENT"; expression="FOCUSED_INTENSE"; facs="AU4 AU5 AU7"; spice=0.40f; }
        else if (valence>0.15 && focused) { emotion="FLOW_STATE"; expression="SERENE_FOCUS"; facs="AU43 AU6"; spice=0.30f; }
        else if (valence>0.15) { emotion="RESONANT_SYNC"; expression="ATTUNED_SMILE"; facs="AU6 AU12 AU45"; spice=0.30f; }
        else if (valence<-0.15 && arousal>0.3) { emotion="SURPRISE_ATTACK"; expression="STARTLED_ALERT"; facs="AU1 AU2 AU5 AU26"; spice=0.40f; }
        else if (valence<-0.05 && !resting) { emotion="BRANCH_DIVERGE"; expression="MISCHIEVOUS_BRANCH"; facs="AU12R AU2 AU5"; spice=0.50f; }
        else if (resting) { emotion="COZY_RECALL"; expression="WARM_REMEMBERING"; facs="AU6 AU12 AU43"; spice=0.30f; }
        else { emotion="BORED_STOMPING"; expression="PLAYFUL_BORED"; facs="AU43 AU15"; spice=0.30f; }
        // Confidence = distance to the nearest decision boundary (saturated).
        const double m1=std::fabs(valence-0.35),m2=std::fabs(valence-0.15),m3=std::fabs(valence+0.05),
                     m4=std::fabs(arousal-0.15),m5=std::fabs(arousal-0.25);
        const double margin=std::min({m1,m2,m3,m4,m5});
        state_.emotion=emotion; state_.expression=expression; state_.facs=facs; state_.spice=spice;
        state_.confidence=(float)std::min(1.0,margin/0.20);
        ++state_.updates;
        return state_;
    }
    const EchoPersonaState& state() const { return state_; }
private:
    EchoPersonaState state_{};
};

} // namespace v27dte

// gneural_motor.hpp - GNU GNeuralNet-backed online-learning motor policy for
// GTAngel v27. This is the adaptive NN actuator: a 20->16->2 tanh MLP trained
// online with gradient descent on accepted human/DTE demonstrations. Nothing
// here is locked, sealed or frozen - the network trains every session.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

extern "C" {
#include "network.h"
#include "error.h"
#include "gradient_descent.h"
#include "rnd.h"
#include "feedforward.h"
#include "save.h"
#include "load.h"
}

namespace v27gnn {

static constexpr int FeatureSize = 20;
static constexpr int HiddenSize = 16;
static constexpr int OutputSize = 2; // [0]=turn (right+), [1]=forward
static constexpr int MaxSamples = 256; // rolling demonstration buffer
static constexpr int TrainBatch = 16;  // most recent samples per pass
static constexpr int TrainEveryFrames = 20;

class GneuralMotorPolicy {
public:
    GneuralMotorPolicy() = default;
    ~GneuralMotorPolicy() { destroy(); }

    GneuralMotorPolicy(const GneuralMotorPolicy&) = delete;
    GneuralMotorPolicy& operator=(const GneuralMotorPolicy&) = delete;

    // Build the 20->16->2 tanh/linear network with small random weights.
    bool create() {
        if (net_) return true;
        net_ = network_alloc();
        if (!net_) return false;
        network_set_neuron_number(net_, FeatureSize + HiddenSize + OutputSize);
        network_set_layer_number(net_, 3);
        net_->layers[0].neurons = &net_->neurons[0];
        net_->layers[0].num_of_neurons = FeatureSize;
        net_->layers[1].neurons = &net_->neurons[FeatureSize];
        net_->layers[1].num_of_neurons = HiddenSize;
        net_->layers[2].neurons = &net_->neurons[FeatureSize + HiddenSize];
        net_->layers[2].num_of_neurons = OutputSize;
        for (int i = 0; i < FeatureSize; ++i) {
            net_->neurons[i].num_input = 0; // input neurons
        }
        for (int h = 0; h < HiddenSize; ++h) {
            neuron* ne = &net_->neurons[FeatureSize + h];
            network_neuron_set_connection_number(ne, FeatureSize);
            for (int c = 0; c < FeatureSize; ++c) {
                ne->connection[c] = &net_->neurons[c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        for (int o = 0; o < OutputSize; ++o) {
            neuron* ne = &net_->neurons[FeatureSize + HiddenSize + o];
            network_neuron_set_connection_number(ne, HiddenSize);
            for (int c = 0; c < HiddenSize; ++c) {
                ne->connection[c] = &net_->neurons[FeatureSize + c];
                ne->w[c] = (rnd() * 2.0 - 1.0) * 0.1;
            }
            ne->activation = TANH;
            ne->discriminant = LINEAR;
        }
        return true;
    }

    // Forward pass: features in, (turn, forward) out.
    bool infer(const float* features, float& turnOut, float& forwardOut) const {
        if (!net_) return false;
        for (int i = 0; i < FeatureSize; ++i)
            net_->neurons[i].output = features[i];
        feedforward(net_);
        turnOut = (float)net_->neurons[FeatureSize + HiddenSize].output;
        forwardOut = (float)net_->neurons[FeatureSize + HiddenSize + 1].output;
        return true;
    }

    // Store one demonstration pair in the rolling buffer.
    void remember(const float* features, float turn, float forward) {
        Sample s;
        for (int i = 0; i < FeatureSize; ++i) s.x[i] = features[i];
        s.y[0] = turn;
        s.y[1] = forward;
        if ((int)samples_.size() >= MaxSamples) samples_.erase(samples_.begin());
        samples_.push_back(s);
        ++newSamples_;
    }

    int pendingSamples() const { return newSamples_; }
    int totalSamples() const { return (int)samples_.size(); }
    uint64_t trainingUpdates() const { return trainingUpdates_; }
    uint64_t trainingSteps() const { return trainingSteps_; }
    float lastLoss() const { return lastLoss_; }

    // One online gradient-descent pass over the rolling buffer. Budgeted to
    // a few milliseconds: gamma=0.08, maxiter=8 (empirically stable).
    bool train() {
        if (!net_ || samples_.empty()) return false;
        const int n = (int)samples_.size();
        if (!cfg_) {
            cfg_ = network_config_alloc_default();
            if (!cfg_) return false;
            // alloc_default pre-ensures only 32 neurons x 16 fan-in; our net
            // is 38 neurons with fan-in 20 - grow the point buffers explicitly.
            network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
            cfg_->verbosity = OFF;
            cfg_->optimization_type = GRADIENT_DESCENT;
            cfg_->error_type = L2;
            // gamma set per-pass by the adaptive schedule below
            cfg_->maxiter = 1;
            cfg_->accuracy = 1e-4;
            cfg_->wmin = -1.0;
            cfg_->wmax = 1.0;
            cfg_->nxw = 1000;
        }
        // Train on the most recent TrainBatch samples: a full 256-point
        // pass costs ~600ms (central-difference gradient_descent), while a
        // 16-point single-iteration pass costs ~5ms - safe for the frame
        // loop. Older samples still shape the network across passes.
        const int batch = n < TrainBatch ? n : TrainBatch;
        for (int p = 0; p < batch; ++p) {
            const Sample& s = samples_[n - batch + p];
            for (int i = 0; i < FeatureSize; ++i)
                NET_PX(cfg_, p, i, 0) = s.x[i];
            for (int o = 0; o < OutputSize; ++o)
                NET_PY(cfg_, p, FeatureSize + HiddenSize + o) = s.y[o];
        }
        cfg_->num_points = batch; // training point count (error() reads this)
        // Adaptive learning rate: sqrt-shaped L2 error gives an almost
        // constant gradient magnitude near the optimum, so a fixed gamma
        // oscillates forever. Halve gamma when a pass fails to improve,
        // reset when it does - converges and stays responsive to new data.
        cfg_->gamma = gamma_;
        gradient_descent(net_, cfg_);
        const double after = error(net_, cfg_);
        if (after < lastError_ * 0.999 || newSamples_ > 0) gamma_ = 0.01; // fresh data re-explores
        else gamma_ = (gamma_ > 2e-4) ? gamma_ * 0.5 : 1e-4; // floor keeps learning alive
        lastError_ = after;
        lastLoss_ = (float)after;
        trainingSteps_ += (uint64_t)cfg_->maxiter;
        ++trainingUpdates_;
        newSamples_ = 0;
        return true;
    }

    bool save(const std::string& path) {
        if (!net_ || !cfg_) return false;
        if (cfg_->save_network_file_name) free(cfg_->save_network_file_name);
        cfg_->save_network_file_name = dup_(path.c_str());
        cfg_->save_neural_network = ON;
        network_save(net_, cfg_); // void; exits the process on I/O failure
        cfg_->save_neural_network = OFF;
        return true;
    }

    bool load(const std::string& path) {
        // network_load() exits the process on a missing file; guard first.
        if (!std::filesystem::exists(path)) return false;
        if (!net_ && !create()) return false;
        if (!cfg_ && !(cfg_ = network_config_alloc_default())) return false;
        network_config_ensure(cfg_, FeatureSize + HiddenSize + OutputSize, FeatureSize);
        cfg_->verbosity = OFF;
        if (cfg_->load_network_file_name) free(cfg_->load_network_file_name);
        cfg_->load_network_file_name = dup_(path.c_str());
        cfg_->load_neural_network = ON;
        network_load(net_, cfg_);
        cfg_->load_neural_network = OFF;
        return true;
    }

private:
    struct Sample {
        float x[FeatureSize];
        float y[OutputSize];
    };

    static char* dup_(const char* s) {
        char* d = (char*)malloc(strlen(s) + 1);
        if (d) strcpy(d, s);
        return d;
    }

    void destroy() {
        if (cfg_) { network_config_free(cfg_); cfg_ = nullptr; }
        if (net_) { network_free(net_); net_ = nullptr; }
    }

    network* net_{nullptr};
    network_config* cfg_{nullptr};
    std::vector<Sample> samples_;
    int newSamples_{0};
    uint64_t trainingUpdates_{0};
    uint64_t trainingSteps_{0};
    float lastLoss_{-1.0f};
    double gamma_{0.01};
    double lastError_{1e9};
};

} // namespace v27gnn



#endif
#endif
