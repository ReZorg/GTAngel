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
    int32_t m_vecPos{0}; // Z++: CVector
    int32_t m_vecAngle{0}; // Z++: CVector
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
    int32_t pos{0}; // Z++: CVector
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
    int32_t pos{0}; // Z++: CVector
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
    int32_t m_vecPosn{0}; // Z++: CVector
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
    int32_t pos{0}; // Z++: CVector
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
    int32_t pos{0}; // Z++: CVector
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
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_control_zpp_invariants(), "Z++ formal contract invariant failed for control");
} // namespace zpp_control_spec
#endif // GTANGEL_ZPP_CONTROL_SPEC_GUARD
