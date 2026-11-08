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
// Free Type from zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_1 : int32_t { CAR_HELI_MAX_DIST_v = 250, CAR_HELI_ENGINE_MAX_DIST_v = 140, CAR_HELI_ENGINE_START_MAX_DIST_v = 30, CAR_HELI_ENGINE_START_VOLUME_v = 70, CAR_HELI_SEAPLANE_MAX_DIST_v = 20, CAR_HELI_SEAPLANE_VOLUME_v = 100, CAR_HELI_REAR_MAX_DIST_v = 27, CAR_HELI_REAR_VOLUME_v = 25, FLAT_TYRE_MAX_DIST_v = 60, FLAT_TYRE_VOLUME_v = 100, RAIN_ON_VEHICLE_MAX_DIST_v = 22, RAIN_ON_VEHICLE_VOLUME_v = 30, REVERSE_GEAR_MAX_DIST_v = 30, REVERSE_GEAR_VOLUME_v = 24, MODEL_CAR_ENGINE_MAX_DIST_v = 35, MODEL_CAR_ENGINE_VOLUME_v = 90, MODEL_HELI_ENGINE_VOLUME_v = 70, VEHICLE_ROAD_NOISE_MAX_DIST_v = 95, VEHICLE_ROAD_NOISE_VOLUME_v = 30, WET_ROAD_NOISE_MAX_DIST_v = 30, WET_ROAD_NOISE_VOLUME_v = 23, VEHICLE_ENGINE_MAX_DIST_v = 50, VEHICLE_ENGINE_BASE_VOLUME_v = 75, VEHICLE_ENGINE_FULL_VOLUME_v = 90, CESNA_IDLE_MAX_DIST_v = 91, CESNA_REV_MAX_DIST_v = 92, CESNA_VOLUME_v = 93, PLAYER_VEHICLE_ENGINE_VOLUME_v = 94, VEHICLE_SKIDDING_MAX_DIST_v = 95, VEHICLE_SKIDDING_VOLUME_v = 96, VEHICLE_HORN_MAX_DIST_v = 97, VEHICLE_HORN_VOLUME_v = 98, VEHICLE_SIREN_MAX_DIST_v = 99, VEHICLE_SIREN_VOLUME_v = 100, VEHICLE_REVERSE_WARNING_MAX_DIST_v = 101, VEHICLE_REVERSE_WARNING_VOLUME_v = 102, VEHICLE_DOORS_MAX_DIST_v = 103, VEHICLE_DOORS_VOLUME_v = 104, AIR_BRAKES_MAX_DIST_v = 105, AIR_BRAKES_VOLUME_v = 106, ENGINE_DAMAGE_MAX_DIST_v = 107, ENGINE_DAMAGE_VOLUME_v = 108, ENGINE_DAMAGE_ON_FIRE_VOLUME_v = 109, CAR_BOMB_TICK_MAX_DIST_v = 110, CAR_BOMB_TICK_VOLUME_v = 111, VEHICLE_ONE_SHOT_HELI_BLADE_MAX_DIST_v = 112, VEHICLE_ONE_SHOT_HELI_BLADE_VOLUME_v = 113, VEHICLE_ONE_SHOT_CAR_TYRE_POP_MAX_DIST_v = 114 };
// Free Type from zpp/audio/AudioLogic.zpp
enum class eVehicleModel : int32_t { LANDSTAL_v = 0, IDAHO_v = 1, STINGER_v = 2, LINERUN_v = 3, PEREN_v = 4, SENTINEL_v = 5, RIO_v = 6, FIRETRUK_v = 7, TRASH_v = 8, STRETCH_v = 9, MANANA_v = 10, INFERNUS_v = 11, VOODOO_v = 12, PONY_v = 13, MULE_v = 14, CHEETAH_v = 15, AMBULAN_v = 16, FBICAR_v = 17, MOONBEAM_v = 18, ESPERANT_v = 19, TAXI_v = 20, WASHING_v = 21, BOBCAT_v = 22, MRWHOOP_v = 23, BFINJECT_v = 24, HUNTER_v = 25, POLICE_v = 26, ENFORCER_v = 27, SECURICA_v = 28, BANSHEE_v = 29, PREDATOR_v = 30, BUS_v = 31, RHINO_v = 32, BARRACKS_v = 33, CUBAN_v = 34, CHOPPER_v = 35, ANGEL_v = 36, COACH_v = 37, CABBIE_v = 38, STALLION_v = 39, RUMPO_v = 40, RCBANDIT_v = 41, ROMERO_v = 42, PACKER_v = 43, SENTXS_v = 44, ADMIRAL_v = 45, SQUALO_v = 46, SEASPAR_v = 47 };
// Free Type from zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_3 : int32_t { OLD_DOOR_v = 0, NEW_DOOR_v = 1, TRUCK_DOOR_v = 2, BUS_DOOR_v = 3 };
// Free Type from zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_4 : int32_t { CAR_HELI_MAX_DIST_v = 250, CAR_HELI_ENGINE_MAX_DIST_v = 140, CAR_HELI_ENGINE_START_MAX_DIST_v = 30, CAR_HELI_ENGINE_START_VOLUME_v = 70, CAR_HELI_SEAPLANE_MAX_DIST_v = 20, CAR_HELI_SEAPLANE_VOLUME_v = 100, CAR_HELI_REAR_MAX_DIST_v = 27, CAR_HELI_REAR_VOLUME_v = 25, FLAT_TYRE_MAX_DIST_v = 60, FLAT_TYRE_VOLUME_v = 100, RAIN_ON_VEHICLE_MAX_DIST_v = 22, RAIN_ON_VEHICLE_VOLUME_v = 30, REVERSE_GEAR_MAX_DIST_v = 30, REVERSE_GEAR_VOLUME_v = 24, MODEL_CAR_ENGINE_MAX_DIST_v = 35, MODEL_CAR_ENGINE_VOLUME_v = 90, MODEL_HELI_ENGINE_VOLUME_v = 70, VEHICLE_ROAD_NOISE_MAX_DIST_v = 95, VEHICLE_ROAD_NOISE_VOLUME_v = 30, WET_ROAD_NOISE_MAX_DIST_v = 30, WET_ROAD_NOISE_VOLUME_v = 23, VEHICLE_ENGINE_MAX_DIST_v = 50, VEHICLE_ENGINE_BASE_VOLUME_v = 75, VEHICLE_ENGINE_FULL_VOLUME_v = 90, CESNA_IDLE_MAX_DIST_v = 91, CESNA_REV_MAX_DIST_v = 92, CESNA_VOLUME_v = 93, PLAYER_VEHICLE_ENGINE_VOLUME_v = 94, VEHICLE_SKIDDING_MAX_DIST_v = 95, VEHICLE_SKIDDING_VOLUME_v = 96, VEHICLE_HORN_MAX_DIST_v = 97, VEHICLE_HORN_VOLUME_v = 98, VEHICLE_SIREN_MAX_DIST_v = 99, VEHICLE_SIREN_VOLUME_v = 100, VEHICLE_REVERSE_WARNING_MAX_DIST_v = 101, VEHICLE_REVERSE_WARNING_VOLUME_v = 102, VEHICLE_DOORS_MAX_DIST_v = 103, VEHICLE_DOORS_VOLUME_v = 104, AIR_BRAKES_MAX_DIST_v = 105, AIR_BRAKES_VOLUME_v = 106, ENGINE_DAMAGE_MAX_DIST_v = 107, ENGINE_DAMAGE_VOLUME_v = 108, ENGINE_DAMAGE_ON_FIRE_VOLUME_v = 109, CAR_BOMB_TICK_MAX_DIST_v = 110, CAR_BOMB_TICK_VOLUME_v = 111, VEHICLE_ONE_SHOT_HELI_BLADE_MAX_DIST_v = 112, VEHICLE_ONE_SHOT_HELI_BLADE_VOLUME_v = 113, VEHICLE_ONE_SHOT_CAR_TYRE_POP_MAX_DIST_v = 114 };
// Free Type from zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_5 : int32_t { RAIN_ON_VEHICLE_MAX_DIST_v = 22, RAIN_ON_VEHICLE_VOLUME_v = 30, REVERSE_GEAR_MAX_DIST_v = 30, REVERSE_GEAR_VOLUME_v = 24, MODEL_CAR_ENGINE_MAX_DIST_v = 30, MODEL_CAR_ENGINE_VOLUME_v = 90, VEHICLE_ROAD_NOISE_MAX_DIST_v = 95, VEHICLE_ROAD_NOISE_VOLUME_v = 30, WET_ROAD_NOISE_MAX_DIST_v = 30, WET_ROAD_NOISE_VOLUME_v = 23, VEHICLE_ENGINE_MAX_DIST_v = 50, VEHICLE_ENGINE_BASE_VOLUME_v = 80, VEHICLE_ENGINE_FULL_VOLUME_v = 120, CESNA_IDLE_MAX_DIST_v = 200, CESNA_REV_MAX_DIST_v = 90, CESNA_VOLUME_v = 80, PLAYER_VEHICLE_ENGINE_VOLUME_v = 85, VEHICLE_SKIDDING_MAX_DIST_v = 40, VEHICLE_SKIDDING_VOLUME_v = 50, VEHICLE_HORN_MAX_DIST_v = 40, VEHICLE_HORN_VOLUME_v = 80, VEHICLE_SIREN_MAX_DIST_v = 110, VEHICLE_SIREN_VOLUME_v = 80, VEHICLE_REVERSE_WARNING_MAX_DIST_v = 50, VEHICLE_REVERSE_WARNING_VOLUME_v = 51, VEHICLE_DOORS_MAX_DIST_v = 52, VEHICLE_DOORS_VOLUME_v = 53, AIR_BRAKES_MAX_DIST_v = 54, AIR_BRAKES_VOLUME_v = 55, ENGINE_DAMAGE_MAX_DIST_v = 56, ENGINE_DAMAGE_VOLUME_v = 57, ENGINE_DAMAGE_ON_FIRE_VOLUME_v = 58, CAR_BOMB_TICK_MAX_DIST_v = 59, CAR_BOMB_TICK_VOLUME_v = 60, VEHICLE_ONE_SHOT_DOOR_MAX_DIST_v = 61, VEHICLE_ONE_SHOT_DOOR_OPEN_VOLUME_v = 62, VEHICLE_ONE_SHOT_DOOR_CLOSE_VOLUME_v = 63, VEHICLE_ONE_SHOT_WINDSHIELD_CRACK_MAX_DIST_v = 64, VEHICLE_ONE_SHOT_WINDSHIELD_CRACK_VOLUME_v = 65, VEHICLE_ONE_SHOT_CAR_JUMP_MAX_DIST_v = 66, VEHICLE_ONE_SHOT_CAR_JUMP_VOLUME_v = 67, VEHICLE_ONE_SHOT_CAR_ENGINE_START_MAX_DIST_v = 68, VEHICLE_ONE_SHOT_CAR_ENGINE_START_VOLUME_v = 69, VEHICLE_ONE_SHOT_CAR_LIGHT_BREAK_VOLUME_v = 70, VEHICLE_ONE_SHOT_CAR_HYDRAULIC_MAX_DIST_v = 71, VEHICLE_ONE_SHOT_CAR_HYDRAULIC_VOLUME_v = 72, VEHICLE_ONE_SHOT_CAR_SPLASH_MAX_DIST_v = 73, VEHICLE_ONE_SHOT_CAR_SPLASH_VOLUME_v = 74 };
// Free Type from zpp/audio/AudioManager.zpp
enum class AudioManager_Enum_1 : int32_t { REFLECTION_TOP_v = 0, REFLECTION_BOTTOM_v = 1, REFLECTION_LEFT_v = 2, REFLECTION_RIGHT_v = 3, REFLECTION_UP_v = 4, MAX_REFLECTIONS_v = 5 };
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
// State Schema Contract from zpp/audio/AudioManager.zpp
struct tPedComment_ZppContract {
    int64_t m_nSampleIndex{0}; // Z++: ℕ
    int32_t m_nEntityIndex{0}; // Z++: ℤ
    int32_t m_vecPos{0}; // Z++: CVector
    float m_fDistance{0.0f}; // Z++: ℝ
    int64_t m_nVolume{0}; // Z++: ℕ
    int32_t m_nLoadingTimeout{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_nSampleIndex >= 0)
            && (m_nVolume >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioManager.zpp
struct cPedParams_ZppContract {
    int32_t m_bDistanceCalculated{0}; // Z++: bool8
    float m_fDistance{0.0f}; // Z++: ℝ
    uint8_t m_pPed_card{0u}; // Z++: Ref[CPed]
    constexpr bool checkInvariant() const {
        return (m_pPed_card <= 1u);
    }
};
// State Schema Contract from zpp/audio/AudioManager.zpp
struct cVehicleParams_ZppContract {
    int32_t m_VehicleType{0}; // Z++: ℤ
    int32_t m_bDistanceCalculated{0}; // Z++: bool8
    float m_fDistance{0.0f}; // Z++: ℝ
    uint8_t m_pVehicle_card{0u}; // Z++: Ref[CVehicle]
    uint8_t m_pTransmission_card{0u}; // Z++: Ref[cTransmission]
    int64_t m_nIndex{0}; // Z++: ℕ
    float m_fVelocityChange{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_pVehicle_card <= 1u)
            && (m_pTransmission_card <= 1u)
            && (m_nIndex >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioSamples.zpp
struct AudioSamplesModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{17962}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 17962);
    }
};
// State Schema Contract from zpp/audio/AudioScriptObject.zpp
struct cAudioScriptObject_ZppContract {
    int32_t AudioId{0}; // Z++: ℤ
    int32_t Posn{0}; // Z++: CVector
    int32_t AudioEntity{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/DMAudio.zpp
struct cDMAudio_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/MusicManager.zpp
struct tStreamedSample_ZppContract {
    int64_t m_nLength{0}; // Z++: ℕ
    int64_t m_nPosition{0}; // Z++: ℕ
    int64_t m_nLastPosCheckTimer{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nLength >= 0)
            && (m_nPosition >= 0)
            && (m_nLastPosCheckTimer >= 0);
    }
};
// State Schema Contract from zpp/audio/PolRadio.zpp
struct cAMCrime_ZppContract {
    int32_t type{0}; // Z++: ℤ
    int32_t position{0}; // Z++: CVector
    int64_t timer{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (timer >= 0);
    }
};
// State Schema Contract from zpp/audio/PolRadio.zpp
struct cPoliceRadioQueue_ZppContract {
    std::array<uint32_t, 60> m_aSamples{}; // Z++: seq ℕ  -- bounded array [POLICE_RADIO_QUEUE_MAX_SAMPLES]
    size_t m_aSamples_len{60u}; // Z++ cardinality #m_aSamples
    int64_t m_nSamplesInQueue{0}; // Z++: ℕ
    int64_t m_nAddOffset{0}; // Z++: ℕ
    int64_t m_nRemoveOffset{0}; // Z++: ℕ
    int32_t TRUE_field{0}; // Z++: return
    int32_t sample{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (m_aSamples_len == 60u && m_aSamples.size() == 60u)
            && (m_nSamplesInQueue >= 0)
            && (m_nAddOffset >= 0)
            && (m_nRemoveOffset >= 0);
    }
};
// State Schema Contract from zpp/audio/sampman.zpp
struct tSample_ZppContract {
    int64_t nOffset{0}; // Z++: ℕ
    int64_t nSize{0}; // Z++: ℕ
    int64_t nFrequency{0}; // Z++: ℕ
    int64_t nLoopStart{0}; // Z++: ℕ
    int32_t nLoopEnd{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (nOffset >= 0)
            && (nSize >= 0)
            && (nFrequency >= 0)
            && (nLoopStart >= 0);
    }
};
// State Schema Contract from zpp/audio/sampman_miles.zpp
struct tMP3Entry_ZppContract {
    int32_t aFilename{0}; // Z++: seq CHAR
    int64_t nTrackLength{0}; // Z++: ℕ
    int64_t nTrackStreamPos{0}; // Z++: ℕ
    uint8_t pNext_card{0u}; // Z++: Ref[tMP3Entry]
    int32_t pLinkPath{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const {
        return (nTrackLength >= 0)
            && (nTrackStreamPos >= 0)
            && (pNext_card <= 1u);
    }
};
constexpr bool verify_audio_zpp_invariants() {
    {
        cAudioCollision_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cAudioCollision_ZppContract bad{};
        bad.m_pEntity1_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        tVehicleSampleData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tVehicleSampleData_ZppContract bad{};
        bad.m_nBank = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        sToniReactions_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        sToniReactions_ZppContract bad{};
        bad.sfx_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        MissionAudioData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MissionAudioData_ZppContract bad{};
        bad.m_nId = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tHelicopterSampleData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tHelicopterSampleData_ZppContract bad{};
        bad.m_bBaseVolume = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tPedComment_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tPedComment_ZppContract bad{};
        bad.m_nSampleIndex = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cPedParams_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cPedParams_ZppContract bad{};
        bad.m_pPed_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cVehicleParams_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cVehicleParams_ZppContract bad{};
        bad.m_pVehicle_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        AudioSamplesModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        AudioSamplesModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        cAudioScriptObject_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cAudioScriptObject_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cDMAudio_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cDMAudio_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tStreamedSample_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tStreamedSample_ZppContract bad{};
        bad.m_nLength = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cAMCrime_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cAMCrime_ZppContract bad{};
        bad.timer = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        cPoliceRadioQueue_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cPoliceRadioQueue_ZppContract bad{};
        bad.m_aSamples_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tSample_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tSample_ZppContract bad{};
        bad.nOffset = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tMP3Entry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tMP3Entry_ZppContract bad{};
        bad.nTrackLength = -1;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_audio_zpp_invariants(), "Z++ formal contract invariant failed for audio");
} // namespace zpp_audio_spec
#endif // GTANGEL_ZPP_AUDIO_SPEC_GUARD
