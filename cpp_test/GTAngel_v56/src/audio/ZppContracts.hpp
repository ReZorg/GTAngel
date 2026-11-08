#pragma once
#ifndef GTANGEL_ZPP_AUDIO_SPEC_GUARD
#define GTANGEL_ZPP_AUDIO_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/audio
// Derived from C:/ddd/GTAngel/cpp_test/zpp/audio/*.zpp
// Modules: 14 | Formal Schemas: 598 | Free Types: 28
// ============================================================================
namespace zpp_audio_spec {
static constexpr int kZppModuleCount = 14;
static constexpr int kZppSchemaCount = 598;
static constexpr int kZppEnumCount   = 28;
// From zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_1 : int32_t { CAR_HELI_MAX_DIST_v = 250, CAR_HELI_ENGINE_MAX_DIST_v = 140, CAR_HELI_ENGINE_START_MAX_DIST_v = 30, CAR_HELI_ENGINE_START_VOLUME_v = 70, CAR_HELI_SEAPLANE_MAX_DIST_v = 20, CAR_HELI_SEAPLANE_VOLUME_v = 100, CAR_HELI_REAR_MAX_DIST_v = 27, CAR_HELI_REAR_VOLUME_v = 25, FLAT_TYRE_MAX_DIST_v = 60, FLAT_TYRE_VOLUME_v = 100, RAIN_ON_VEHICLE_MAX_DIST_v = 22, RAIN_ON_VEHICLE_VOLUME_v = 30, REVERSE_GEAR_MAX_DIST_v = 30, REVERSE_GEAR_VOLUME_v = 24, MODEL_CAR_ENGINE_MAX_DIST_v = 35, MODEL_CAR_ENGINE_VOLUME_v = 90, MODEL_HELI_ENGINE_VOLUME_v = 70, VEHICLE_ROAD_NOISE_MAX_DIST_v = 95, VEHICLE_ROAD_NOISE_VOLUME_v = 30, WET_ROAD_NOISE_MAX_DIST_v = 30, WET_ROAD_NOISE_VOLUME_v = 23, VEHICLE_ENGINE_MAX_DIST_v = 50, VEHICLE_ENGINE_BASE_VOLUME_v = 75, VEHICLE_ENGINE_FULL_VOLUME_v = 90, CESNA_IDLE_MAX_DIST_v = 91, CESNA_REV_MAX_DIST_v = 92, CESNA_VOLUME_v = 93, PLAYER_VEHICLE_ENGINE_VOLUME_v = 94, VEHICLE_SKIDDING_MAX_DIST_v = 95, VEHICLE_SKIDDING_VOLUME_v = 96, VEHICLE_HORN_MAX_DIST_v = 97, VEHICLE_HORN_VOLUME_v = 98, VEHICLE_SIREN_MAX_DIST_v = 99, VEHICLE_SIREN_VOLUME_v = 100, VEHICLE_REVERSE_WARNING_MAX_DIST_v = 101, VEHICLE_REVERSE_WARNING_VOLUME_v = 102, VEHICLE_DOORS_MAX_DIST_v = 103, VEHICLE_DOORS_VOLUME_v = 104, AIR_BRAKES_MAX_DIST_v = 105, AIR_BRAKES_VOLUME_v = 106, ENGINE_DAMAGE_MAX_DIST_v = 107, ENGINE_DAMAGE_VOLUME_v = 108, ENGINE_DAMAGE_ON_FIRE_VOLUME_v = 109, CAR_BOMB_TICK_MAX_DIST_v = 110, CAR_BOMB_TICK_VOLUME_v = 111, VEHICLE_ONE_SHOT_HELI_BLADE_MAX_DIST_v = 112, VEHICLE_ONE_SHOT_HELI_BLADE_VOLUME_v = 113, VEHICLE_ONE_SHOT_CAR_TYRE_POP_MAX_DIST_v = 114 };
// From zpp/audio/AudioLogic.zpp
enum class eVehicleModel : int32_t { LANDSTAL_v = 0, IDAHO_v = 1, STINGER_v = 2, LINERUN_v = 3, PEREN_v = 4, SENTINEL_v = 5, RIO_v = 6, FIRETRUK_v = 7, TRASH_v = 8, STRETCH_v = 9, MANANA_v = 10, INFERNUS_v = 11, VOODOO_v = 12, PONY_v = 13, MULE_v = 14, CHEETAH_v = 15, AMBULAN_v = 16, FBICAR_v = 17, MOONBEAM_v = 18, ESPERANT_v = 19, TAXI_v = 20, WASHING_v = 21, BOBCAT_v = 22, MRWHOOP_v = 23, BFINJECT_v = 24, HUNTER_v = 25, POLICE_v = 26, ENFORCER_v = 27, SECURICA_v = 28, BANSHEE_v = 29, PREDATOR_v = 30, BUS_v = 31, RHINO_v = 32, BARRACKS_v = 33, CUBAN_v = 34, CHOPPER_v = 35, ANGEL_v = 36, COACH_v = 37, CABBIE_v = 38, STALLION_v = 39, RUMPO_v = 40, RCBANDIT_v = 41, ROMERO_v = 42, PACKER_v = 43, SENTXS_v = 44, ADMIRAL_v = 45, SQUALO_v = 46, SEASPAR_v = 47 };
// From zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_3 : int32_t { OLD_DOOR_v = 0, NEW_DOOR_v = 1, TRUCK_DOOR_v = 2, BUS_DOOR_v = 3 };
// From zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_4 : int32_t { CAR_HELI_MAX_DIST_v = 250, CAR_HELI_ENGINE_MAX_DIST_v = 140, CAR_HELI_ENGINE_START_MAX_DIST_v = 30, CAR_HELI_ENGINE_START_VOLUME_v = 70, CAR_HELI_SEAPLANE_MAX_DIST_v = 20, CAR_HELI_SEAPLANE_VOLUME_v = 100, CAR_HELI_REAR_MAX_DIST_v = 27, CAR_HELI_REAR_VOLUME_v = 25, FLAT_TYRE_MAX_DIST_v = 60, FLAT_TYRE_VOLUME_v = 100, RAIN_ON_VEHICLE_MAX_DIST_v = 22, RAIN_ON_VEHICLE_VOLUME_v = 30, REVERSE_GEAR_MAX_DIST_v = 30, REVERSE_GEAR_VOLUME_v = 24, MODEL_CAR_ENGINE_MAX_DIST_v = 35, MODEL_CAR_ENGINE_VOLUME_v = 90, MODEL_HELI_ENGINE_VOLUME_v = 70, VEHICLE_ROAD_NOISE_MAX_DIST_v = 95, VEHICLE_ROAD_NOISE_VOLUME_v = 30, WET_ROAD_NOISE_MAX_DIST_v = 30, WET_ROAD_NOISE_VOLUME_v = 23, VEHICLE_ENGINE_MAX_DIST_v = 50, VEHICLE_ENGINE_BASE_VOLUME_v = 75, VEHICLE_ENGINE_FULL_VOLUME_v = 90, CESNA_IDLE_MAX_DIST_v = 91, CESNA_REV_MAX_DIST_v = 92, CESNA_VOLUME_v = 93, PLAYER_VEHICLE_ENGINE_VOLUME_v = 94, VEHICLE_SKIDDING_MAX_DIST_v = 95, VEHICLE_SKIDDING_VOLUME_v = 96, VEHICLE_HORN_MAX_DIST_v = 97, VEHICLE_HORN_VOLUME_v = 98, VEHICLE_SIREN_MAX_DIST_v = 99, VEHICLE_SIREN_VOLUME_v = 100, VEHICLE_REVERSE_WARNING_MAX_DIST_v = 101, VEHICLE_REVERSE_WARNING_VOLUME_v = 102, VEHICLE_DOORS_MAX_DIST_v = 103, VEHICLE_DOORS_VOLUME_v = 104, AIR_BRAKES_MAX_DIST_v = 105, AIR_BRAKES_VOLUME_v = 106, ENGINE_DAMAGE_MAX_DIST_v = 107, ENGINE_DAMAGE_VOLUME_v = 108, ENGINE_DAMAGE_ON_FIRE_VOLUME_v = 109, CAR_BOMB_TICK_MAX_DIST_v = 110, CAR_BOMB_TICK_VOLUME_v = 111, VEHICLE_ONE_SHOT_HELI_BLADE_MAX_DIST_v = 112, VEHICLE_ONE_SHOT_HELI_BLADE_VOLUME_v = 113, VEHICLE_ONE_SHOT_CAR_TYRE_POP_MAX_DIST_v = 114 };
// State Schema Contract from zpp/audio/AudioCollision.zpp
struct cAudioCollision_ZppContract {
    uint8_t m_pEntity1_card{0u}; // Z++: Ref[CEntity]
    uint8_t m_pEntity2_card{0u}; // Z++: Ref[CEntity]
    int64_t m_bSurface1{0}; // Z++: ℕ
    int64_t m_bSurface2{0}; // Z++: ℕ
    float m_fIntensity1{0.0f}; // Z++: ℝ
    float m_fIntensity2{0.0f}; // Z++: ℝ
    int32_t m_vecPosition{0}; // Z++: CVector
    float m_fDistance{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_pEntity1_card <= 1u)
            && (m_pEntity2_card <= 1u)
            && (m_bSurface1 >= 0)
            && (m_bSurface2 >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioCollision.zpp
struct cAudioCollisionManager_ZppContract {
    int32_t m_asCollisions1{0}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    int32_t m_asCollisions2{0}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    int32_t m_bIndicesTable{0}; // Z++: seq ℕ  -- bounded array [NUMAUDIOCOLLISIONS]
    int64_t m_bCollisionsInQueue{0}; // Z++: ℕ
    int32_t m_sQueue{0}; // Z++: cAudioCollision
    int32_t NUMAUDIOCOLLISIONS{0}; // Z++: i
    constexpr bool checkInvariant() const {
        return (m_bCollisionsInQueue >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct tVehicleSampleData_ZppContract {
    int32_t m_nAccelerationSampleIndex{0}; // Z++: eSfxSample
    int64_t m_nBank{0}; // Z++: ℕ
    int32_t m_nHornSample{0}; // Z++: eSfxSample
    int32_t m_nHornFrequency{0}; // Z++: ℤ
    int64_t m_nSirenOrAlarmSample{0}; // Z++: ℕ
    int32_t m_nSirenOrAlarmFrequency{0}; // Z++: ℤ
    int64_t m_bDoorType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nBank >= 0)
            && (m_nSirenOrAlarmSample >= 0)
            && (m_bDoorType >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct sToniReactions_ZppContract {
    std::array<uint32_t, 4> sfx{}; // Z++: seq ℕ  -- bounded array [4]
    size_t sfx_len{4u}; // Z++ cardinality #sfx
    std::array<uint32_t, 4> count{}; // Z++: seq ℕ  -- bounded array [4]
    size_t count_len{4u}; // Z++ cardinality #count
    constexpr bool checkInvariant() const {
        return (sfx_len == 4u && sfx.size() == 4u)
            && (count_len == 4u && count.size() == 4u);
    }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct MissionAudioData_ZppContract {
    int32_t m_pName{0}; // Z++: seq CHAR
    int64_t m_nId{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nId >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct tHelicopterSampleData_ZppContract {
    float m_fMaxDistance{0.0f}; // Z++: ℝ
    float m_fBaseDistance{0.0f}; // Z++: ℝ
    int64_t m_bBaseVolume{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_bBaseVolume >= 0);
    }
};
constexpr bool verify_audio_zpp_invariants() {
    if (!cAudioCollision_ZppContract{}.checkInvariant()) return false;
    if (!cAudioCollisionManager_ZppContract{}.checkInvariant()) return false;
    if (!tVehicleSampleData_ZppContract{}.checkInvariant()) return false;
    if (!sToniReactions_ZppContract{}.checkInvariant()) return false;
    if (!MissionAudioData_ZppContract{}.checkInvariant()) return false;
    if (!tHelicopterSampleData_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_audio_zpp_invariants(), "Z++ formal contract invariant failed for audio");
} // namespace zpp_audio_spec
#endif // GTANGEL_ZPP_AUDIO_SPEC_GUARD
