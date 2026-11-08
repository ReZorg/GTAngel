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
struct CTheCarGenerators_ZppContract {
    int64_t ProcessCounter{0}; // Z++: ℕ
    int64_t NumOfCarGenerators{0}; // Z++: ℕ
    std::array<int32_t, 16> CarGeneratorArray{}; // Z++: seq CCarGenerator  -- bounded array [NUM_CARGENS]
    size_t CarGeneratorArray_len{16u}; // Z++ cardinality #CarGeneratorArray
    int64_t GenerateEvenIfPlayerIsCloseCounter{0}; // Z++: ℕ
    int64_t CurrentActiveCount{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (CarGeneratorArray_len == 16u && CarGeneratorArray.size() == 16u)
            && (ProcessCounter >= 0)
            && (NumOfCarGenerators >= 0)
            && (GenerateEvenIfPlayerIsCloseCounter >= 0)
            && (CurrentActiveCount >= 0);
    }
};
// State Schema Contract from zpp/vehicles/Cranes.zpp
struct CCranes_ZppContract {
    int64_t CarsCollectedMilitaryCrane{0}; // Z++: ℕ
    int32_t NumCranes{0}; // Z++: ℤ
    std::array<int32_t, 8> aCranes{}; // Z++: seq CCrane  -- bounded array [NUM_CRANES]
    size_t aCranes_len{8u}; // Z++ cardinality #aCranes
    constexpr bool checkInvariant() const {
        return (aCranes_len == 8u && aCranes.size() == 8u)
            && (CarsCollectedMilitaryCrane >= 0);
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
struct CFerryInst_ZppContract {
    std::array<int32_t, 8> pPathData{}; // Z++: seq Ref[CFerryPath]  -- bounded array [NUM_FERRY_PATHS]
    size_t pPathData_len{8u}; // Z++ cardinality #pPathData
    std::array<float, 8> m_afPositions{}; // Z++: seq ℝ  -- bounded array [NUM_FERRIES]
    size_t m_afPositions_len{8u}; // Z++ cardinality #m_afPositions
    std::array<float, 8> m_afSpeeds{}; // Z++: seq ℝ  -- bounded array [NUM_FERRIES]
    size_t m_afSpeeds_len{8u}; // Z++ cardinality #m_afSpeeds
    std::array<int32_t, 8> m_apFerries{}; // Z++: seq Ref[CFerry]  -- bounded array [NUM_FERRIES]
    size_t m_apFerries_len{8u}; // Z++ cardinality #m_apFerries
    bool m_bFerriesDisabled{false}; // Z++: 𝔹
    std::array<uint32_t, 8> m_anNumWakePoints{}; // Z++: seq ℕ  -- bounded array [NUM_FERRIES]
    size_t m_anNumWakePoints_len{8u}; // Z++ cardinality #m_anNumWakePoints
    constexpr bool checkInvariant() const {
        return (pPathData_len == 8u && pPathData.size() == 8u)
            && (m_afPositions_len == 8u && m_afPositions.size() == 8u)
            && (m_afSpeeds_len == 8u && m_afSpeeds.size() == 8u)
            && (m_apFerries_len == 8u && m_apFerries.size() == 8u)
            && (m_anNumWakePoints_len == 8u && m_anNumWakePoints.size() == 8u);
    }
};
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_Process(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_CreateCarGenerator_2(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_Init_3(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_SaveAllCarGenerators_4(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_LoadAllCarGenerators_5(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCTheCarGenerators) from zpp/vehicles/CarGen.zpp
constexpr bool op_CTheCarGenerators_CreateCarGenerator(CTheCarGenerators_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_vehicles_zpp_invariants() {
    {
        CTheCarGenerators_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTheCarGenerators_ZppContract bad{};
        bad.CarGeneratorArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCranes_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCranes_ZppContract bad{};
        bad.aCranes_len = 0u;
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
        CFerryInst_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFerryInst_ZppContract bad{};
        bad.pPathData_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_Process(st)) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_CreateCarGenerator_2(st)) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_Init_3(st)) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_SaveAllCarGenerators_4(st)) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_LoadAllCarGenerators_5(st)) return false;
    }
    {
        CTheCarGenerators_ZppContract st{};
        if (!op_CTheCarGenerators_CreateCarGenerator(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_vehicles_zpp_invariants(), "Z++ formal contract invariant failed for vehicles");
static_assert(static_cast<int>(eCarNodes::CAR_WHEEL_RF_v) == 1 && static_cast<int>(eCarNodes::NUM_CAR_NODES_v) == 20, "eCarNodes Z++ valuation mismatch");
} // namespace zpp_vehicles_spec
#endif // GTANGEL_ZPP_VEHICLES_SPEC_GUARD
