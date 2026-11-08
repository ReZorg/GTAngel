#pragma once
#ifndef GTANGEL_ZPP_AUDIO_SPEC_GUARD
#define GTANGEL_ZPP_AUDIO_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/audio
// Derived from C:/ddd/GTAngel/cpp_test/zpp/audio/*.zpp
// Modules: 20 | Formal Schemas: 1021 | Free Types: 33
// ============================================================================
namespace zpp_audio_spec {
static constexpr int kZppModuleCount = 20;
static constexpr int kZppSchemaCount = 1021;
static constexpr int kZppEnumCount   = 33;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
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
    ZppVec3 m_vecPosition{}; // Z++: CVector
    float m_fDistance{0.0f}; // Z++: ℝ
    int32_t m_nBaseVolume{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pEntity1_card <= 1u)
            && (m_pEntity2_card <= 1u)
            && (m_bSurface1 >= 0)
            && (m_bSurface2 >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioCollision.zpp
struct cAudioCollisionManager_ZppContract {
    std::array<int32_t, 10> m_asCollisions1{}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    size_t m_asCollisions1_len{10u}; // Z++ cardinality #m_asCollisions1
    std::array<int32_t, 10> m_asCollisions2{}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    size_t m_asCollisions2_len{10u}; // Z++ cardinality #m_asCollisions2
    std::array<uint32_t, 10> m_bIndicesTable{}; // Z++: seq ℕ  -- bounded array [NUMAUDIOCOLLISIONS]
    size_t m_bIndicesTable_len{10u}; // Z++ cardinality #m_bIndicesTable
    int64_t m_bCollisionsInQueue{0}; // Z++: ℕ
    int32_t m_sQueue{0}; // Z++: cAudioCollision
    int32_t NUMAUDIOCOLLISIONS{0}; // Z++: i
    constexpr bool checkInvariant() const {
        return (m_asCollisions1_len == 10u && m_asCollisions1.size() == 10u)
            && (m_asCollisions2_len == 10u && m_asCollisions2.size() == 10u)
            && (m_bIndicesTable_len == 10u && m_bIndicesTable.size() == 10u)
            && (m_bCollisionsInQueue >= 0);
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
struct tSound_ZppContract {
    int32_t m_nEntityIndex{0}; // Z++: ℤ
    int64_t m_nBankIndex{0}; // Z++: ℕ
    int32_t m_bIs2D{0}; // Z++: bool8
    int64_t m_nPriority{0}; // Z++: ℕ
    int64_t m_nFrequency{0}; // Z++: ℕ
    int64_t m_nVolume{0}; // Z++: ℕ
    float m_fDistance{0.0f}; // Z++: ℝ
    int64_t m_nLoopCount{0}; // Z++: ℕ
    int32_t m_nLoopEnd{0}; // Z++: ℤ
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_bReverb{0}; // Z++: bool8
    int32_t m_bReflections{0}; // Z++: bool8
    int32_t m_bIsPlayingFinished{0}; // Z++: bool8
    int32_t m_nVolumeChange{0}; // Z++: ℤ
    int64_t field_51_lcs{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nBankIndex >= 0)
            && (m_nPriority >= 0)
            && (m_nFrequency >= 0)
            && (m_nVolume >= 0)
            && (m_nLoopCount >= 0)
            && (field_51_lcs >= 0);
    }
};
// State Schema Contract from zpp/audio/AudioManager.zpp
struct tPedComment_ZppContract {
    int64_t m_nSampleIndex{0}; // Z++: ℕ
    int32_t m_nEntityIndex{0}; // Z++: ℤ
    ZppVec3 m_vecPos{}; // Z++: CVector
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
    ZppVec3 Posn{}; // Z++: CVector
    int32_t AudioEntity{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/DMAudio.zpp
struct cDMAudio_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
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
    ZppVec3 position{}; // Z++: CVector
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
// State Schema Contract from zpp/audio/oal/aldlist.zpp
struct ALDEVICEINFO_ZppContract {
    int32_t strDeviceName{0}; // Z++: seq CHAR
    int32_t iMajorVersion{0}; // Z++: ℤ
    int32_t iMinorVersion{0}; // Z++: ℤ
    int32_t uiSourceCount{0}; // Z++: ℤ
    int32_t Extensions{0}; // Z++: ℤ
    bool bSelected{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/aldlist.zpp
struct ALDeviceList_ZppContract {
    std::array<int32_t, 64> aDeviceInfo{}; // Z++: seq ALDEVICEINFO  -- bounded array [64]
    size_t aDeviceInfo_len{64u}; // Z++ cardinality #aDeviceInfo
    int32_t nNumOfDevices{0}; // Z++: ℤ
    int32_t defaultDeviceIndex{0}; // Z++: ℤ
    int32_t filterIndex{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (aDeviceInfo_len == 64u && aDeviceInfo.size() == 64u);
    }
};
// State Schema Contract from zpp/audio/oal/channel.zpp
struct CChannel_ZppContract {
    int64_t id{0}; // Z++: ℕ
    float Gain{0.0f}; // Z++: ℝ
    float Mix{0.0f}; // Z++: ℝ
    int64_t Data{0}; // Z++: ADDRESS
    int64_t DataSize{0}; // Z++: ℕ
    int32_t Frequency{0}; // Z++: ℤ
    std::array<float, 3> Position{}; // Z++: seq ℝ  -- bounded array [3]
    size_t Position_len{3u}; // Z++ cardinality #Position
    std::array<float, 2> Distances{}; // Z++: seq ℝ  -- bounded array [2]
    size_t Distances_len{2u}; // Z++ cardinality #Distances
    int32_t LoopCount{0}; // Z++: ℤ
    std::array<int32_t, 2> LoopPoints{}; // Z++: seq ℤ  -- bounded array [2]
    size_t LoopPoints_len{2u}; // Z++ cardinality #LoopPoints
    int32_t LastProcessedOffset{0}; // Z++: ℤ
    bool bIs2D{false}; // Z++: 𝔹
    int32_t channelsThatNeedService{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (Position_len == 3u && Position.size() == 3u)
            && (Distances_len == 2u && Distances.size() == 2u)
            && (LoopPoints_len == 2u && LoopPoints.size() == 2u)
            && (id >= 0)
            && (DataSize >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CSortStereoBuffer_ZppContract {
    uint8_t PcmBuf_card{0u}; // Z++: ℙ ℕ
    int64_t BufSize{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (PcmBuf_card <= 1u)
            && (BufSize >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CImaADPCMDecoder_ZppContract {
    int32_t StepIndex{0}; // Z++: ℤ
    int32_t size{0}; // Z++: i
    int32_t Sample{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CWavFile_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    int64_t Size{0}; // Z++: ℕ
    int64_t NumChannels{0}; // Z++: ℕ
    int64_t SampleRate{0}; // Z++: ℕ
    int64_t ByteRate{0}; // Z++: ℕ
    int64_t BlockAlign{0}; // Z++: ℕ
    int64_t BitsPerSample{0}; // Z++: ℕ
    std::array<uint32_t, 2> extra{}; // Z++: seq ℕ  -- bounded array [2]
    size_t extra_len{2u}; // Z++ cardinality #extra
    uint8_t m_pFile_card{0u}; // Z++: Ref[FILE]
    bool m_bIsOpen{false}; // Z++: 𝔹
    int32_t m_FormatHeader{0}; // Z++: tFormatHeader
    int64_t m_DataStartOffset{0}; // Z++: ℕ
    int64_t m_nSampleCount{0}; // Z++: ℕ
    int64_t m_nSamplesPerBlock{0}; // Z++: ℕ
    uint8_t m_pAdpcmBuffer_card{0u}; // Z++: ℙ ℕ
    uint8_t m_ppPcmBuffers_card{0u}; // Z++: ℙ ℤ
    uint8_t m_pAdpcmDecoders_card{0u}; // Z++: Ref[CImaADPCMDecoder]
    int32_t WAVE{0}; // Z++: ℤ
    int32_t size{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (extra_len == 2u && extra.size() == 2u)
            && (instance_addr >= 0)
            && (Size >= 0)
            && (NumChannels >= 0)
            && (SampleRate >= 0)
            && (ByteRate >= 0)
            && (BlockAlign >= 0)
            && (BitsPerSample >= 0)
            && (m_pFile_card <= 1u)
            && (m_DataStartOffset >= 0)
            && (m_nSampleCount >= 0)
            && (m_nSamplesPerBlock >= 0)
            && (m_pAdpcmBuffer_card <= 1u)
            && (m_ppPcmBuffers_card <= 1u)
            && (m_pAdpcmDecoders_card <= 1u);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct tDataHeader_ZppContract {
    int64_t ID{0}; // Z++: ℕ
    int64_t Size{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (ID >= 0)
            && (Size >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct tFormatHeader_ZppContract {
    int64_t AudioFormat{0}; // Z++: ℕ
    int64_t NumChannels{0}; // Z++: ℕ
    int64_t SampleRate{0}; // Z++: ℕ
    int64_t ByteRate{0}; // Z++: ℕ
    int64_t BlockAlign{0}; // Z++: ℕ
    int64_t BitsPerSample{0}; // Z++: ℕ
    std::array<uint32_t, 2> extra{}; // Z++: seq ℕ  -- bounded array [2]
    size_t extra_len{2u}; // Z++ cardinality #extra
    constexpr bool checkInvariant() const {
        return (extra_len == 2u && extra.size() == 2u)
            && (AudioFormat >= 0)
            && (NumChannels >= 0)
            && (SampleRate >= 0)
            && (ByteRate >= 0)
            && (BlockAlign >= 0)
            && (BitsPerSample >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CSndFile_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_pfSound_card{0u}; // Z++: Ref[SNDFILE]
    int32_t m_soundInfo{0}; // Z++: SF_INFO
    int32_t size{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_pfSound_card <= 1u);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CMP3File_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_pMH_card{0u}; // Z++: Ref[mpg123_handle]
    bool m_bOpened{false}; // Z++: 𝔹
    int64_t m_nRate{0}; // Z++: ℕ
    int64_t m_nChannels{0}; // Z++: ℕ
    int32_t m_pPath{0}; // Z++: seq CHAR
    bool m_bFileNotOpenedYet{false}; // Z++: 𝔹
    int64_t size{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_pMH_card <= 1u)
            && (m_nRate >= 0)
            && (m_nChannels >= 0)
            && (size >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CADFFile_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_pMH_card{0u}; // Z++: Ref[mpg123_handle]
    bool m_bOpened{false}; // Z++: 𝔹
    int64_t m_nRate{0}; // Z++: ℕ
    int64_t m_nChannels{0}; // Z++: ℕ
    int32_t m_pPath{0}; // Z++: seq CHAR
    bool m_bFileNotOpenedYet{false}; // Z++: 𝔹
    int64_t size{0}; // Z++: ℕ
    int32_t bytesRead{0}; // Z++: return
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_pMH_card <= 1u)
            && (m_nRate >= 0)
            && (m_nChannels >= 0)
            && (size >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CVagDecoder_ZppContract {
    float s_1{0.0f}; // Z++: ℝ
    float s_2{0.0f}; // Z++: ℝ
    int32_t flags{0}; // Z++: ℤ
    int32_t VAG_SAMPLES_IN_LINE{0}; // Z++: i
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct CVbFile_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_pFile_card{0u}; // Z++: Ref[FILE]
    uint8_t m_pVagDecoders_card{0u}; // Z++: Ref[CVagDecoder]
    int64_t m_FileSize{0}; // Z++: ℕ
    int64_t m_nNumberOfBlocks{0}; // Z++: ℕ
    int64_t m_nSampleRate{0}; // Z++: ℕ
    int64_t m_nChannels{0}; // Z++: ℕ
    bool m_bBlockRead{false}; // Z++: 𝔹
    int64_t m_LineInBlock{0}; // Z++: ℕ
    int64_t m_CurrentBlock{0}; // Z++: ℕ
    uint8_t m_ppVagBuffers_card{0u}; // Z++: ℙ ℕ
    uint8_t m_ppPcmBuffers_card{0u}; // Z++: ℙ ℤ
    int32_t nChannels{0}; // Z++: i
    uint8_t VAG_SAMPLES_IN_LINE_card{0u}; // Z++: Ref[return_m_nNumberOfBlocks__NUM_VAG_LINES_IN_BLOCK]
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_pFile_card <= 1u)
            && (m_pVagDecoders_card <= 1u)
            && (m_FileSize >= 0)
            && (m_nNumberOfBlocks >= 0)
            && (m_nSampleRate >= 0)
            && (m_nChannels >= 0)
            && (m_LineInBlock >= 0)
            && (m_CurrentBlock >= 0)
            && (m_ppVagBuffers_card <= 1u)
            && (m_ppPcmBuffers_card <= 1u)
            && (VAG_SAMPLES_IN_LINE_card <= 1u);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct COpusFile_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    uint8_t m_FileH_card{0u}; // Z++: Ref[OggOpusFile]
    bool m_bOpened{false}; // Z++: 𝔹
    int64_t m_nRate{0}; // Z++: ℕ
    int64_t m_nChannels{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (m_FileH_card <= 1u)
            && (m_nRate >= 0)
            && (m_nChannels >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct IDecoder_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/audio/oal/stream.zpp
struct tsQueue_ZppContract {
    int32_t true_field{0}; // Z++: return
    int32_t count{0}; // Z++: ℤ
    int32_t m_mutex{0}; // Z++: std__mutex
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
// State Schema Contract from zpp/audio/soundlist.zpp
struct soundlistModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{433}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 433);
    }
};
// Query Operation Schema Contract (ΞCWavFile) from zpp/audio/oal/stream.zpp
constexpr bool op_CWavFile_GetCurrentSample_3(const CWavFile_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCChannel) from zpp/audio/oal/channel.zpp
constexpr bool op_CChannel_Init(const CChannel_ZppContract& pre, const CChannel_ZppContract& post, int64_t _id_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.id == (int64_t)_id_in) && (post.bIs2D == true);
}
constexpr CChannel_ZppContract apply_CChannel_Init(const CChannel_ZppContract& pre, int64_t _id_in = 25) {
    CChannel_ZppContract post = pre;
    post.id = (int64_t)_id_in;
    post.bIs2D = true;
    return post;
}
// Delta Operation Schema Contract (ΔCChannel) from zpp/audio/oal/channel.zpp
constexpr bool op_CChannel_SetSampleData(const CChannel_ZppContract& pre, const CChannel_ZppContract& post, int64_t _data_in = 25, int64_t _DataSize_in = 25, int32_t freq_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Data == (int64_t)_data_in) && (post.DataSize == (int64_t)_DataSize_in) && (post.Frequency == (int32_t)freq_in);
}
constexpr CChannel_ZppContract apply_CChannel_SetSampleData(const CChannel_ZppContract& pre, int64_t _data_in = 25, int64_t _DataSize_in = 25, int32_t freq_in = 25) {
    CChannel_ZppContract post = pre;
    post.Data = (int64_t)_data_in;
    post.DataSize = (int64_t)_DataSize_in;
    post.Frequency = (int32_t)freq_in;
    return post;
}
// Delta Operation Schema Contract (ΔCChannel) from zpp/audio/oal/channel.zpp
constexpr bool op_CChannel_SetLoopCount(const CChannel_ZppContract& pre, const CChannel_ZppContract& post, int32_t count_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.LoopCount == (int32_t)count_in);
}
constexpr CChannel_ZppContract apply_CChannel_SetLoopCount(const CChannel_ZppContract& pre, int32_t count_in = 25) {
    CChannel_ZppContract post = pre;
    post.LoopCount = (int32_t)count_in;
    return post;
}
// Delta Operation Schema Contract (ΔCChannel) from zpp/audio/oal/channel.zpp
constexpr bool op_CChannel_SetReverbMix(const CChannel_ZppContract& pre, const CChannel_ZppContract& post, float mix_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Mix == mix_in);
}
constexpr CChannel_ZppContract apply_CChannel_SetReverbMix(const CChannel_ZppContract& pre, float mix_in = 2.5f) {
    CChannel_ZppContract post = pre;
    post.Mix = mix_in;
    return post;
}
constexpr bool verify_audio_zpp_invariants() {
    {
        cAudioCollision_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cAudioCollision_ZppContract bad{};
        bad.m_pEntity1_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        cAudioCollisionManager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cAudioCollisionManager_ZppContract bad{};
        bad.m_asCollisions1_len = 0u;
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
        tSound_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tSound_ZppContract bad{};
        bad.m_nBankIndex = -1;
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
        ALDEVICEINFO_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ALDEVICEINFO_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        ALDeviceList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ALDeviceList_ZppContract bad{};
        bad.aDeviceInfo_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CChannel_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CChannel_ZppContract bad{};
        bad.Position_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSortStereoBuffer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSortStereoBuffer_ZppContract bad{};
        bad.PcmBuf_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CImaADPCMDecoder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CImaADPCMDecoder_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWavFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWavFile_ZppContract bad{};
        bad.extra_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tDataHeader_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tDataHeader_ZppContract bad{};
        bad.ID = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tFormatHeader_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tFormatHeader_ZppContract bad{};
        bad.extra_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSndFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSndFile_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMP3File_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMP3File_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CADFFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CADFFile_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CVagDecoder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVagDecoder_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CVbFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVbFile_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        COpusFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        COpusFile_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        IDecoder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        IDecoder_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tsQueue_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tsQueue_ZppContract bad{};
        bad.instance_addr = -1;
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
    {
        soundlistModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        soundlistModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        const CWavFile_ZppContract st{};
        if (!op_CWavFile_GetCurrentSample_3(st)) return false;
    }
    {
        CChannel_ZppContract pre{};
        const int64_t _id_in = 25;
        pre.id = 0;
        pre.bIs2D = false;
        const auto post = apply_CChannel_Init(pre, _id_in);
        if (!op_CChannel_Init(pre, post, _id_in)) return false;
        {
            CChannel_ZppContract badPost = post;
            badPost.id = (int64_t)(_id_in + 1);
            if (op_CChannel_Init(pre, badPost, _id_in)) return false;
        }
        {
            CChannel_ZppContract badPost = post;
            badPost.bIs2D = false;
            if (op_CChannel_Init(pre, badPost, _id_in)) return false;
        }
    }
    {
        CChannel_ZppContract pre{};
        const int64_t _data_in = 25;
        const int64_t _DataSize_in = 25;
        const int32_t freq_in = 25;
        pre.Data = 0;
        pre.DataSize = 0;
        pre.Frequency = 0;
        const auto post = apply_CChannel_SetSampleData(pre, _data_in, _DataSize_in, freq_in);
        if (!op_CChannel_SetSampleData(pre, post, _data_in, _DataSize_in, freq_in)) return false;
        {
            CChannel_ZppContract badPost = post;
            badPost.Data = (int64_t)(_data_in + 1);
            if (op_CChannel_SetSampleData(pre, badPost, _data_in, _DataSize_in, freq_in)) return false;
        }
        {
            CChannel_ZppContract badPost = post;
            badPost.DataSize = (int64_t)(_DataSize_in + 1);
            if (op_CChannel_SetSampleData(pre, badPost, _data_in, _DataSize_in, freq_in)) return false;
        }
        {
            CChannel_ZppContract badPost = post;
            badPost.Frequency = (int32_t)(freq_in + 1);
            if (op_CChannel_SetSampleData(pre, badPost, _data_in, _DataSize_in, freq_in)) return false;
        }
    }
    {
        CChannel_ZppContract pre{};
        const int32_t count_in = 25;
        pre.LoopCount = 0;
        const auto post = apply_CChannel_SetLoopCount(pre, count_in);
        if (!op_CChannel_SetLoopCount(pre, post, count_in)) return false;
        {
            CChannel_ZppContract badPost = post;
            badPost.LoopCount = (int32_t)(count_in + 1);
            if (op_CChannel_SetLoopCount(pre, badPost, count_in)) return false;
        }
    }
    {
        CChannel_ZppContract pre{};
        const float mix_in = 2.5f;
        pre.Mix = 0.0f;
        const auto post = apply_CChannel_SetReverbMix(pre, mix_in);
        if (!op_CChannel_SetReverbMix(pre, post, mix_in)) return false;
        {
            CChannel_ZppContract badPost = post;
            badPost.Mix = mix_in + 1.0f;
            if (op_CChannel_SetReverbMix(pre, badPost, mix_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_audio_zpp_invariants(), "Z++ formal contract invariant failed for audio");
} // namespace zpp_audio_spec
#endif // GTANGEL_ZPP_AUDIO_SPEC_GUARD
