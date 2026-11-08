#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_VEHICLES_SPEC_GUARD
#define GTANGEL_ZPP_VEHICLES_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/vehicles
// Derived from C:/ddd/GTAngel/cpp_test/zpp/vehicles/*.zpp
// Modules: 15 | Formal Schemas: 700 | Free Types: 54
// ============================================================================
namespace zpp_vehicles_spec {
static constexpr int kZppModuleCount = 15;
static constexpr int kZppSchemaCount = 700;
static constexpr int kZppEnumCount   = 54;
// From zpp/vehicles/Automobile.zpp
enum class Automobile_Enum_1 : int32_t { CARWHEEL_FRONT_LEFT_v = 0, CARWHEEL_REAR_LEFT_v = 1, CARWHEEL_FRONT_RIGHT_v = 2, CARWHEEL_REAR_RIGHT_v = 3 };
// From zpp/vehicles/Automobile.zpp
enum class eCarNodes : int32_t { CAR_WHEEL_RF_v = 0, CAR_WHEEL_RM_v = 1, CAR_WHEEL_RB_v = 2, CAR_WHEEL_LF_v = 3, CAR_WHEEL_LM_v = 4, CAR_WHEEL_LB_v = 5, CAR_BUMP_FRONT_v = 6, CAR_BUMP_REAR_v = 7, CAR_WING_RF_v = 8, CAR_WING_RR_v = 9, CAR_DOOR_RF_v = 10, CAR_DOOR_RR_v = 11 };
// From zpp/vehicles/Automobile.zpp
enum class eBombType : int32_t { CARBOMB_NONE_v = 0, CARBOMB_TIMED_v = 1, CARBOMB_ONIGNITION_v = 2, CARBOMB_REMOTE_v = 3, CARBOMB_TIMEDACTIVE_v = 4, CARBOMB_ONIGNITIONACTIVE_v = 5 };
// From zpp/vehicles/Automobile.zpp
enum class Automobile_Enum_4 : int32_t { CAR_DOOR_FLAG_UNKNOWN_v = 0, CAR_DOOR_FLAG_LF_v = 1, CAR_DOOR_FLAG_LR_v = 2, CAR_DOOR_FLAG_RF_v = 3, CAR_DOOR_FLAG_RR_v = 4 };
// State Schema Contract from zpp/vehicles/Automobile.zpp
struct CAutomobile_ZppContract {
    int32_t Damage{0}; // Z++: CDamageManager
    int32_t Doors{0}; // Z++: seq CDoor  -- bounded array [6]
    int32_t m_aCarNodes{0}; // Z++: seq Ref[RwFrame]  -- bounded array [NUM_CAR_NODES]
    int32_t m_aWheelColPoints{0}; // Z++: seq ℤ  -- bounded array [4]
    int32_t m_aSuspensionSpringRatio{0}; // Z++: seq ℝ  -- bounded array [4]
    int32_t m_aSuspensionSpringRatioPrev{0}; // Z++: seq ℝ  -- bounded array [4]
    int32_t m_aWheelTimer{0}; // Z++: seq ℝ  -- bounded array [4]
    float m_auto_unused1{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/vehicles/Bike.zpp
struct CBike_ZppContract {
    int32_t m_aBikeNodes{0}; // Z++: seq Ref[RwFrame]  -- bounded array [BIKE_NUM_NODES]
    bool bLeanMatrixClean{false}; // Z++: 𝔹
    int32_t m_leanMatrix{0}; // Z++: CMatrix
    int32_t m_vecAvgSurfaceNormal{0}; // Z++: CVector
    int32_t m_vecAvgSurfaceRight{0}; // Z++: CVector
    int32_t pBikeHandling{0}; // Z++: Ref[tBikeHandlingData]
    int32_t m_bikeAnimType{0}; // Z++: AssocGroupId
    int32_t m_wheelStatus{0}; // Z++: seq ℕ  -- bounded array [2]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/vehicles/Boat.zpp
struct CBoat_ZppContract {
    float m_fMovingRotation{0.0f}; // Z++: ℝ
    float m_fMovingSpeed{0.0f}; // Z++: ℝ
    int32_t m_boat_unused1{0}; // Z++: ℤ
    int32_t m_aBoatNodes{0}; // Z++: seq Ref[RwFrame]  -- bounded array [NUM_BOAT_NODES]
    int32_t m_boom{0}; // Z++: CDoor
    int32_t pBoatHandling{0}; // Z++: Ref[tBoatHandlingData]
    bool bBoatInWater{false}; // Z++: 𝔹
    bool bPropellerInWater{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/vehicles/CarGen.zpp
struct CCarGenerator_ZppContract {
    int32_t m_nModelIndex{0}; // Z++: ℤ
    int32_t m_vecPos{0}; // Z++: CVector
    float m_fAngle{0.0f}; // Z++: ℝ
    int32_t m_nColor1{0}; // Z++: ℤ
    int32_t m_nColor2{0}; // Z++: ℤ
    uint32_t m_bForceSpawn{0}; // Z++: ℕ
    uint32_t m_nAlarm{0}; // Z++: ℕ
    uint32_t m_nDoorlock{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/vehicles/CarGen.zpp
struct CTheCarGenerators_ZppContract {
    uint32_t ProcessCounter{0}; // Z++: ℕ
    uint32_t NumOfCarGenerators{0}; // Z++: ℕ
    int32_t CarGeneratorArray{0}; // Z++: seq CCarGenerator  -- bounded array [NUM_CARGENS]
    uint32_t GenerateEvenIfPlayerIsCloseCounter{0}; // Z++: ℕ
    uint32_t CurrentActiveCount{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/vehicles/Cranes.zpp
struct CCrane_ZppContract {
    int32_t m_pCraneEntity{0}; // Z++: Ref[CBuilding]
    int32_t m_pHook{0}; // Z++: Ref[CObject]
    int32_t m_nAudioEntity{0}; // Z++: ℤ
    float m_fPickupX1{0.0f}; // Z++: ℝ
    float m_fPickupX2{0.0f}; // Z++: ℝ
    float m_fPickupY1{0.0f}; // Z++: ℝ
    float m_fPickupY2{0.0f}; // Z++: ℝ
    int32_t m_vecDropoffTarget{0}; // Z++: CVector
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_vehicles_zpp_invariants() {
    if (!CAutomobile_ZppContract{}.checkInvariant()) return false;
    if (!CBike_ZppContract{}.checkInvariant()) return false;
    if (!CBoat_ZppContract{}.checkInvariant()) return false;
    if (!CCarGenerator_ZppContract{}.checkInvariant()) return false;
    if (!CTheCarGenerators_ZppContract{}.checkInvariant()) return false;
    if (!CCrane_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_vehicles_zpp_invariants(), "Z++ formal contract invariant failed for vehicles");
} // namespace zpp_vehicles_spec
#endif // GTANGEL_ZPP_VEHICLES_SPEC_GUARD
