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
namespace uechovr {

struct Pose6DOF {
    float x{}, y{}, z{};
    float qx{}, qy{}, qz{}, qw{};
    float vx{}, vy{}, vz{};
    float wx{}, wy{}, wz{};
    uint64_t frame_count{};
    bool valid{false};
};

struct EyeParams {
    float left{}, right{}, top{}, bottom{};
    float ipd{0.064f};
    uint32_t width{0}, height{0};
};

class FrameBufferHook {
public:
    enum class API { D3D11, D3D12, Unknown };
    struct Capture {
        std::vector<uint8_t> pixels;
        uint32_t width{}, height{};
        uint32_t pitch{};
        API api{API::Unknown};
        uint64_t frame_number{};
        bool valid{false};
    };
    FrameBufferHook() = default;
    void initialize(API api, uint32_t w, uint32_t h) {
        m_api = api; m_width = w; m_height = h;
        m_capture.pixels.assign(w * h * 4, 0);
        m_capture.width = w; m_capture.height = h;
        m_capture.pitch = w * 4;
        m_capture.api = api;
    }
    Capture captureBackBuffer(uint64_t frame_num) {
        m_capture.frame_number = frame_num;
        m_capture.valid = true;
        for (uint32_t y = 0; y < m_height; y++) {
            for (uint32_t x = 0; x < m_width; x++) {
                size_t idx = (y * m_width + x) * 4;
                float fx = (float)x / m_width;
                float fy = (float)y / m_height;
                float sep = sinf(fx * 10.0f + frame_num * 0.05f) * 0.5f + 0.5f;
                m_capture.pixels[idx + 0] = (uint8_t)(sep * 255);
                m_capture.pixels[idx + 1] = (uint8_t)((1.0f - sep) * 255);
                m_capture.pixels[idx + 2] = (uint8_t)(fx * 255);
                m_capture.pixels[idx + 3] = 255;
            }
        }
        return m_capture;
    }
    API api() const { return m_api; }
    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }
private:
    API m_api{API::Unknown};
    uint32_t m_width{}, m_height{};
    Capture m_capture{};
};

class Tracker6DOF {
public:
    Tracker6DOF() = default;
    void reset() {
        m_pose = Pose6DOF{};
        m_pose.qw = 1.0f;
        m_pose.valid = true;
        m_frame_count = 0;
    }
    Pose6DOF updateHMD(float dt) {
        m_frame_count++;
        m_pose.frame_count = m_frame_count;
        float t = (float)m_frame_count * dt;
        m_pose.x = sinf(t * 0.3f) * 0.5f;
        m_pose.y = 1.6f + sinf(t * 0.2f) * 0.1f;
        m_pose.z = cosf(t * 0.25f) * 0.3f;
        float yaw = t * 0.1f;
        float pitch = sinf(t * 0.15f) * 0.2f;
        m_pose.qw = cosf(yaw * 0.5f) * cosf(pitch * 0.5f);
        m_pose.qx = sinf(pitch * 0.5f) * cosf(yaw * 0.5f);
        m_pose.qy = sinf(yaw * 0.5f) * cosf(pitch * 0.5f);
        m_pose.qz = 0.0f;
        m_pose.vx = cosf(t * 0.3f) * 0.15f;
        m_pose.vy = cosf(t * 0.2f) * 0.02f;
        m_pose.vz = -sinf(t * 0.25f) * 0.075f;
        m_pose.valid = true;
        return m_pose;
    }
    Pose6DOF updateController(int hand, float dt) {
        Pose6DOF ctrl{};
        ctrl.frame_count = m_frame_count;
        float t = (float)m_frame_count * dt;
        float side = hand == 0 ? -0.3f : 0.3f;
        ctrl.x = m_pose.x + side + sinf(t * 0.5f + hand) * 0.1f;
        ctrl.y = m_pose.y - 0.3f + cosf(t * 0.4f + hand) * 0.05f;
        ctrl.z = m_pose.z + 0.2f + sinf(t * 0.3f + hand * 2) * 0.08f;
        ctrl.qw = 1.0f;
        ctrl.valid = true;
        return ctrl;
    }
    const Pose6DOF& hmdPose() const { return m_pose; }
    uint64_t frameCount() const { return m_frame_count; }
private:
    Pose6DOF m_pose{};
    uint64_t m_frame_count{};
};

class StereoRenderer {
public:
    enum class Method { NativeStereo, Synchronized, Alternating };
    StereoRenderer() = default;
    void setMethod(Method m) { m_method = m; }
    Method method() const { return m_method; }
    const char* methodName() const {
        switch (m_method) {
            case Method::NativeStereo: return "Native Stereo";
            case Method::Synchronized: return "Synchronized Sequential";
            case Method::Alternating: return "Alternating/AFR";
        }
        return "?";
    }
    EyeParams getEyeParams(int eye) const {
        EyeParams ep{};
        ep.ipd = 0.064f;
        ep.width = 64; ep.height = 64;
        float offset = eye == 0 ? -0.032f : 0.032f;
        ep.left = -0.5f + offset; ep.right = 0.5f + offset;
        ep.top = -0.5f; ep.bottom = 0.5f;
        return ep;
    }
private:
    Method m_method{Method::NativeStereo};
};

} // namespace uechovr

// ═══ dte::EndocrineBus (ported from u9n-main/DeepTreeEcho/Avatar/EndocrineBus.h) ═══
namespace dte {

enum class HormoneId : size_t {
    Cortisol = 0, Dopamine = 1, Serotonin = 2, Norepinephrine = 3,
    Oxytocin = 4, Thyroxine = 5, Melatonin = 6, Insulin = 7,
    Cytokine = 8, Anandamide = 9, COUNT = 10
};
constexpr size_t HORMONE_COUNT = 10;

const char* HORMONE_NAMES[HORMONE_COUNT] = {
    "Cortisol","Dopamine","Serotonin","Norepinephrine","Oxytocin",
    "Thyroxine","Melatonin","Insulin","Cytokine","Anandamide"
};

struct HormoneBus {
    std::array<double, HORMONE_COUNT> levels{};
    double get(HormoneId id) const { return levels[(size_t)id]; }
    void set(HormoneId id, double v) {
        levels[(size_t)id] = std::max(0.0, std::min(1.0, v));
    }
    void decay(HormoneId id, double baseline, double rate) {
        auto i = (size_t)id;
        levels[i] += (baseline - levels[i]) * rate;
        levels[i] = std::max(0.0, std::min(1.0, levels[i]));
    }
    void reset(double baseline = 0.5) { levels.fill(baseline); }
};

struct ValenceArousalState {
    double valence = 0.0;
    double arousal = 0.0;
    double magnitude() const { return sqrt(valence*valence + arousal*arousal); }
};

enum class CognitiveMode { Exploration, Exploitation, Rest, Alarm };
inline const char* cognitiveModeName(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Exploration: return "EXPLORATION";
        case CognitiveMode::Exploitation: return "EXPLOITATION";
        case CognitiveMode::Rest: return "REST";
        case CognitiveMode::Alarm: return "ALARM";
    }
    return "?";
}
inline COLORREF cognitiveModeColor(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Exploration: return RGB(76, 175, 80);
        case CognitiveMode::Exploitation: return RGB(0, 188, 212);
        case CognitiveMode::Rest: return RGB(158, 158, 200);
        case CognitiveMode::Alarm: return RGB(244, 67, 54);
    }
    return RGB(255,255,255);
}

struct Stimulus {
    std::string source;
    double intensity = 0.0;
    Stimulus(std::string src, double inten) : source(std::move(src)), intensity(inten) {}
    Stimulus() = default;
};

struct GlandConfig {
    HormoneId hormone = HormoneId::Cortisol;
    double baseline = 0.5;
    double decayRate = 0.1;
    double sensitivity = 1.0;
    double maxProduction = 0.3;
};

class VirtualGland {
public:
    explicit VirtualGland(GlandConfig c) : config_(c) {}
    void respond(const Stimulus& s, HormoneBus& bus) {
        double cur = bus.get(config_.hormone);
        double delta = s.intensity * config_.sensitivity;
        delta = std::max(-config_.maxProduction, std::min(config_.maxProduction, delta));
        bus.set(config_.hormone, cur + delta);
    }
    void tick(HormoneBus& bus) {
        bus.decay(config_.hormone, config_.baseline, config_.decayRate);
    }
    HormoneId hormoneId() const { return config_.hormone; }
    GlandConfig& config() { return config_; }
    const GlandConfig& config() const { return config_; }
private:
    GlandConfig config_;
};

class EndocrineSystem {
public:
    EndocrineSystem() {
        glands_.emplace_back(GlandConfig{HormoneId::Cortisol, 0.15, 0.03, 1.0, 0.3});
        glands_.emplace_back(GlandConfig{HormoneId::Dopamine, 0.30, 0.12, 1.2, 0.35});
        glands_.emplace_back(GlandConfig{HormoneId::Serotonin, 0.40, 0.02, 0.8, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Norepinephrine, 0.10, 0.15, 1.1, 0.3});
        glands_.emplace_back(GlandConfig{HormoneId::Oxytocin, 0.10, 0.04, 0.9, 0.25});
        glands_.emplace_back(GlandConfig{HormoneId::Thyroxine, 0.50, 0.01, 0.5, 0.1});
        glands_.emplace_back(GlandConfig{HormoneId::Melatonin, 0.00, 0.10, 0.7, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Insulin, 0.20, 0.06, 0.8, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Cytokine, 0.05, 0.03, 0.9, 0.2});
        glands_.emplace_back(GlandConfig{HormoneId::Anandamide, 0.10, 0.18, 1.0, 0.3});
        bus_.reset(0.2);
    }
    void submitStimulus(const Stimulus& s) { pending_.push_back(s); }
    void tick() {
        tickCount_++;
        for (auto& s : pending_) {
            for (auto& g : glands_) {
                bool match = false;
                auto hid = g.hormoneId();
                if (s.source.find("reward") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (s.source.find("success") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (s.source.find("error") != std::string::npos && hid == HormoneId::Cortisol) match = true;
                if (s.source.find("threat") != std::string::npos && hid == HormoneId::Cortisol) match = true;
                if (s.source.find("threat") != std::string::npos && hid == HormoneId::Norepinephrine) match = true;
                if (s.source.find("social") != std::string::npos && hid == HormoneId::Oxytocin) match = true;
                if (s.source.find("novelty") != std::string::npos && hid == HormoneId::Norepinephrine) match = true;
                if (s.source.find("rest") != std::string::npos && hid == HormoneId::Melatonin) match = true;
                if (s.source.find("insight") != std::string::npos && hid == HormoneId::Anandamide) match = true;
                if (s.source.find("discovery") != std::string::npos && hid == HormoneId::Dopamine) match = true;
                if (match) g.respond(s, bus_);
            }
        }
        pending_.clear();
        for (auto& g : glands_) g.tick(bus_);
        computeValenceArousal();
        computeCognitiveMode();
    }
    const HormoneBus& bus() const { return bus_; }
    const ValenceArousalState& valenceArousal() const { return va_; }
    CognitiveMode cognitiveMode() const { return mode_; }
    double hormoneLevel(HormoneId id) const { return bus_.get(id); }
    size_t tickCount() const { return tickCount_; }
    void setDecayRate(HormoneId id, double rate) {
        for (auto& g : glands_) if (g.hormoneId() == id) g.config().decayRate = rate;
    }
    double getDecayRate(HormoneId id) const {
        for (auto& g : glands_) if (g.hormoneId() == id) return g.config().decayRate;
        return 0.0;
    }
    int modeTransitions = 0;
    CognitiveMode lastMode = CognitiveMode::Exploration;
    std::vector<std::pair<int, CognitiveMode>> modeHistory;
    void trackModeTransition() {
        if (mode_ != lastMode) {
            modeTransitions++;
            modeHistory.push_back({(int)tickCount_, mode_});
            if (modeHistory.size() > 50) modeHistory.erase(modeHistory.begin());
            lastMode = mode_;
        }
    }
private:
    void computeValenceArousal() {
        double pos = bus_.get(HormoneId::Dopamine) + bus_.get(HormoneId::Serotonin)
                   + bus_.get(HormoneId::Oxytocin) + bus_.get(HormoneId::Anandamide);
        double neg = bus_.get(HormoneId::Cortisol) + bus_.get(HormoneId::Cytokine);
        va_.valence = std::max(-1.0, std::min(1.0, (pos - neg) / 2.5));
        va_.arousal = std::max(-1.0, std::min(1.0,
            bus_.get(HormoneId::Norepinephrine) + bus_.get(HormoneId::Cortisol)
            + bus_.get(HormoneId::Thyroxine) - bus_.get(HormoneId::Melatonin) - 0.5));
    }
    void computeCognitiveMode() {
        if (va_.arousal > 0.1 && va_.valence > 0.0) mode_ = CognitiveMode::Exploration;
        else if (va_.arousal <= 0.1 && va_.valence > 0.1) mode_ = CognitiveMode::Exploitation;
        else if (va_.arousal < -0.1 && va_.valence <= 0.0) mode_ = CognitiveMode::Rest;
        else if (va_.arousal > 0.2 && va_.valence < -0.1) mode_ = CognitiveMode::Alarm;
        else mode_ = CognitiveMode::Exploration;
    }
    HormoneBus bus_;
    ValenceArousalState va_;
    CognitiveMode mode_ = CognitiveMode::Exploration;
    std::vector<VirtualGland> glands_;
    std::vector<Stimulus> pending_;
    size_t tickCount_ = 0;
};

// ═══ Echo State Network Reservoir ═══
class EchoStateNetwork {
public:
    EchoStateNetwork(int inDim, int resDim, int outDim, double spectralRadius = 0.95,
                     double leak = 0.3, unsigned seed = 42)
        : inDim_(inDim), resDim_(resDim), outDim_(outDim), leak_(leak) {
        std::mt19937 rng(seed);
        std::normal_distribution<double> gauss(0.0, 1.0);
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        W_in_.assign(resDim_ * inDim_, 0.0);
        for (auto& w : W_in_) w = gauss(rng) * 0.1;
        W_res_.assign(resDim_ * resDim_, 0.0);
        for (int i = 0; i < resDim_; i++)
            for (int j = 0; j < resDim_; j++)
                if (uni(rng) > 0.9) W_res_[i*resDim_+j] = gauss(rng);
        std::vector<double> v(resDim_, 1.0), tmp(resDim_);
        double maxEig = 1.0;
        for (int it = 0; it < 20; it++) {
            for (int i = 0; i < resDim_; i++) {
                tmp[i] = 0;
                for (int j = 0; j < resDim_; j++) tmp[i] += W_res_[i*resDim_+j] * v[j];
            }
            double norm = 0;
            for (double x : tmp) norm += x*x;
            norm = sqrt(norm);
            if (norm > 1e-9) { maxEig = norm; for (int i = 0; i < resDim_; i++) v[i] = tmp[i]/norm; }
        }
        if (maxEig > 0) {
            double scale = spectralRadius / maxEig;
            for (auto& w : W_res_) w *= scale;
        }
        state_.assign(resDim_, 0.0);
        W_out_.assign(outDim_ * resDim_, 0.0);
        stateHistory_.assign(resDim_, std::vector<double>(100, 0.0));
        historyIdx_ = 0;
    }
    std::vector<double> step(const std::vector<double>& input) {
        std::vector<double> newState(resDim_, 0.0);
        for (int i = 0; i < resDim_; i++) {
            double sum = 0;
            for (int j = 0; j < inDim_ && j < (int)input.size(); j++)
                sum += W_in_[i*inDim_+j] * input[j];
            for (int j = 0; j < resDim_; j++)
                sum += W_res_[i*resDim_+j] * state_[j];
            newState[i] = tanh(sum);
        }
        for (int i = 0; i < resDim_; i++)
            state_[i] = (1.0 - leak_) * state_[i] + leak_ * newState[i];
        for (int i = 0; i < resDim_; i++)
            stateHistory_[i][historyIdx_] = state_[i];
        historyIdx_ = (historyIdx_ + 1) % 100;
        std::vector<double> out(outDim_, 0.0);
        for (int o = 0; o < outDim_; o++)
            for (int i = 0; i < resDim_; i++)
                out[o] += W_out_[o*resDim_+i] * state_[i];
        return out;
    }
    int rlsUpdates = 0;
    void trainReadout(const std::vector<double>& target, double) {
        if ((int)P_.size() != resDim_*resDim_) {
            P_.assign(resDim_*resDim_, 0.0);
            for (int i = 0; i < resDim_; i++) P_[i*resDim_+i] = 1.0e6;
        }
        std::vector<double> out(outDim_, 0.0);
        for (int o = 0; o < outDim_; o++)
            for (int i = 0; i < resDim_; i++)
                out[o] += W_out_[o*resDim_+i] * state_[i];
        for (int o = 0; o < outDim_ && o < (int)target.size(); o++) {
            double err = target[o] - out[o];
            // k = P r / (lambda + r^T P r)
            std::vector<double> Pr(resDim_, 0.0);
            for (int i = 0; i < resDim_; i++)
                for (int j = 0; j < resDim_; j++)
                    Pr[i] += P_[i*resDim_+j] * state_[j];
            double denom = rlsLambda_;
            for (int i = 0; i < resDim_; i++) denom += state_[i] * Pr[i];
            if (denom < 1e-12) continue;
            std::vector<double> k(resDim_, 0.0);
            for (int i = 0; i < resDim_; i++) k[i] = Pr[i] / denom;
            // W += k e^T
            for (int i = 0; i < resDim_; i++)
                W_out_[o*resDim_+i] += k[i] * err;
            // P = (P - k (P r)^T) / lambda
            for (int i = 0; i < resDim_; i++)
                for (int j = 0; j < resDim_; j++)
                    P_[i*resDim_+j] = (P_[i*resDim_+j] - k[i]*Pr[j]) / rlsLambda_;
        }
        ++rlsUpdates;
    }
    double readoutL1() const {
        double s = 0; for (double w : W_out_) s += std::fabs(w);
        return s;
    }
    const std::vector<double>& state() const { return state_; }
    double stateEnergy() const {
        double e = 0; for (double x : state_) e += x*x;
        return sqrt(e / resDim_);
    }
    int resDim() const { return resDim_; }
    int inDim() const { return inDim_; }
    double leakRate() const { return leak_; }
    const std::vector<double>& W_res() const { return W_res_; }
    const std::vector<double>& W_in() const { return W_in_; }
    const std::vector<std::vector<double>>& stateHistory() const { return stateHistory_; }
    int historyIdx() const { return historyIdx_; }
    // u9n intrinsic plasticity: shift activations toward target mean 0 by
    // lr * meanError, clamp [-1,1]. Keeps the reservoir homeostatic.
    int plasticityUpdates = 0;
    // v27 persistence: expose trained readout + RLS covariance for save/load.
    std::vector<double> readoutWeights() const { return W_out_; }
    void setReadoutWeights(const std::vector<double>& w) { if ((int)w.size() == outDim_ * resDim_) W_out_ = w; }
    std::vector<double> rlsCovariance() const { return P_; }
    void setRlsCovariance(const std::vector<double>& p) { if ((int)p.size() == resDim_ * resDim_) P_ = p; }
    void setReservoirState(const std::vector<double>& s) { if ((int)s.size() == resDim_) state_ = s; }
    void applyIntrinsicPlasticity(double lr = 0.01) {
        double mean = 0; for (double x : state_) mean += x;
        mean /= resDim_;
        double shift = lr * (0.0 - mean);
        for (auto& x : state_) x = std::max(-1.0, std::min(1.0, x + shift));
        ++plasticityUpdates;
    }
    double spectralRadius() const { return 0.95; }
private:
    int inDim_, resDim_, outDim_;
    double leak_;
    std::vector<double> W_in_, W_res_, W_out_, state_;
    std::vector<double> P_;              // RLS inverse-correlation matrix
    double rlsLambda_ = 0.999;
    std::vector<std::vector<double>> stateHistory_;
    int historyIdx_ = 0;
};

// ═══ Echobeats 4-Phase Cognitive Cycle ═══
// Episodic memory: stores reservoir state snapshots at significant events
// (collisions, goals, group shifts) and recalls the best match by scored
// similarity. The recall similarity feeds back as ESN input channel 30 so
// the reservoir senses "have I lived a moment like this before?".
struct EpisodicMemory {
    struct Episode {
        std::vector<double> state;   // reservoir snapshot (96-dim)
        float meanContradiction;    // TRIZ context at capture
        float reward;
        uint64_t tick;
        // u9n emotional encoding + consolidation fields.
        float strength = 1.0f;      // consolidates up, decays down
        float activation = 0.0f;    // boosted on retrieval (+0.2)
        int accessCount = 0;
        float valence = 0.0f;       // emotional sign of the outcome
        uint64_t lastAccessTick = 0;
        bool flashbulb = false;     // highly significant event marker
    };
    // u9n semantic abstraction: episodes cluster into concepts.
    struct SemanticConcept {
        std::vector<double> prototype;  // running mean of member states
        float variance = 0.0f;
        int instanceCount = 1;
        float confidence = 0.5f;
    };
    std::deque<Episode> episodes;
    std::vector<SemanticConcept> concepts;
    size_t capacity = 64;
    int consolidations = 0;
    void store(const std::vector<double>& s, float mc, float r, uint64_t t) {
        if (episodes.size() >= capacity) episodes.pop_front();
        Episode e; e.state = s; e.meanContradiction = mc; e.reward = r; e.tick = t;
        e.valence = (r > 0.0f) ? 1.0f : (r < 0.0f ? -1.0f : 0.0f);
        e.flashbulb = std::fabs(r) >= 1.0f;   // collision / goal events
        e.lastAccessTick = t;
        episodes.push_back(e);
    }
    // Scored retrieval (u9n): Score = Similarity * Strength * (1 + Activation),
    // similarity threshold 0.3. On a hit the episode is touched: accessCount++,
    // activation += 0.2, lastAccessTick updated.
    double recall(const std::vector<double>& s) {
        if (episodes.empty()) return 0.0;
        double bestScore = 0.0;
        Episode* best = nullptr;
        for (auto& e : episodes) {
            if (e.state.size() != s.size()) continue;
            double dot = 0, na = 0, nb = 0;
            for (size_t i = 0; i < s.size(); i++) {
                dot += s[i] * e.state[i]; na += s[i]*s[i]; nb += e.state[i]*e.state[i];
            }
            if (na > 1e-12 && nb > 1e-12) {
                double sim = dot / (std::sqrt(na) * std::sqrt(nb));
                if (sim > 0.3) {
                    double score = sim * e.strength * (1.0 + e.activation);
                    if (score > bestScore) { bestScore = score; best = &e; }
                }
            }
        }
        if (best) {
            best->accessCount++;
            best->activation = std::min(2.0f, best->activation + 0.2f);
            best->lastAccessTick = best->tick;
        }
        return bestScore;
    }
    // u9n decay: strength -= rate * (1 + timeSinceAccess * 0.001);
    // activation *= (1 - decayAmount).
    void decay(uint64_t now, float rate = 0.0001f) {
        for (auto& e : episodes) {
            uint64_t since = now > e.lastAccessTick ? now - e.lastAccessTick : 0;
            e.strength = std::max(0.0f, std::min(2.0f, e.strength - rate * (1.0f + (float)since * 0.001f)));
            e.activation *= (1.0f - 0.01f);
        }
    }
    // u9n 4-phase consolidation, simplified: strengthen encoded episodes
    // (x1.1), replay the strongest (x3 strength boost, +0.3 activation),
    // abstract the strongest into a semantic concept, then prune weak
    // episodes (strength < 0.1 && accessCount < 3).
    void consolidate() {
        ++consolidations;
        for (auto& e : episodes) e.strength = std::min(2.0f, e.strength * 1.1f);
        if (episodes.empty()) return;
        Episode* strongest = &episodes[0];
        for (auto& e : episodes)
            if (e.strength > strongest->strength) strongest = &e;
        for (int i = 0; i < 3; i++) strongest->strength = std::min(2.0f, strongest->strength * 1.05f);
        strongest->activation = std::min(2.0f, strongest->activation + 0.3f);
        // AbstractToConcept: prototype = mean, variance = sqrt(mean sq diff);
        // existing concept updates by EMA alpha = 1/(n+1), confidence += 0.1.
        if (strongest->state.size() > 0) {
            bool merged = false;
            for (auto& c : concepts) {
                if (c.prototype.size() != strongest->state.size()) continue;
                double dot = 0, na = 0, nb = 0;
                for (size_t i = 0; i < c.prototype.size(); i++) {
                    dot += c.prototype[i] * strongest->state[i];
                    na += c.prototype[i]*c.prototype[i];
                    nb += strongest->state[i]*strongest->state[i];
                }
                double sim = (na > 1e-12 && nb > 1e-12)
                    ? dot / (std::sqrt(na) * std::sqrt(nb)) : 0.0;
                if (sim > 0.7) {
                    float alpha = 1.0f / (float)(c.instanceCount + 1);
                    double varAcc = 0;
                    for (size_t i = 0; i < c.prototype.size(); i++) {
                        c.prototype[i] = (1.0f-alpha)*c.prototype[i] + alpha*strongest->state[i];
                        double d = c.prototype[i] - strongest->state[i];
                        varAcc += d*d;
                    }
                    c.variance = (float)std::sqrt(varAcc / c.prototype.size());
                    c.instanceCount++;
                    c.confidence = std::min(1.0f, c.confidence + 0.1f);
                    merged = true;
                    break;
                }
            }
            if (!merged && concepts.size() < 16) {
                SemanticConcept c;
                c.prototype = strongest->state;
                c.variance = 0.0f;
                c.instanceCount = 1;
                c.confidence = 0.5f;
                concepts.push_back(c);
            }
        }
        // Prune: weak + rarely-accessed episodes go first.
        std::deque<Episode> kept;
        for (auto& e : episodes)
            if (e.strength >= 0.1f || e.accessCount >= 3) kept.push_back(e);
        episodes.swap(kept);
    }
};
enum class EchobeatsPhase { Perceive, Synthesize, Resonate, Enact };
inline const char* phaseName(EchobeatsPhase p) {
    switch (p) {
        case EchobeatsPhase::Perceive: return "PERCEIVE";
        case EchobeatsPhase::Synthesize: return "SYNTHESIZE";
        case EchobeatsPhase::Resonate: return "RESONATE";
        case EchobeatsPhase::Enact: return "ENACT";
    }
    return "?";
}

// ═══ TRIZ Cognitive Arena (hex grid, 40 principles, aesthetic reward) ═══
enum class NicheType { Perturbation, Scaffolding, Inheritance };
struct Niche {
    NicheType type;
    float x, y;
    float strength;
    float age;
    const char* name;
};
struct HexCell {
    float aestheticValue = 0.5f;
    float temperature = 0.5f;
    int objectCount = 0;
    bool occupied = false;
    bool isNiche = false;
    NicheType nicheType = NicheType::Perturbation;
    float contradiction = 0.0f;
    int lastPrinciple = -1;
};

class TRIZArena {
public:
    static constexpr int SIZE = 12;
    std::array<std::array<HexCell, SIZE>, SIZE> grid;
    float coherence = 0.5f;
    float lastDelta = 0.0f;
    int discoveries = 0;
    int phase = 0;
    int stepInPhase = 0;
    std::vector<Niche> niches;
    int selectedQ = -1, selectedR = -1;
    float maxContradiction = 0.0f;
    int maxContradictionQ = -1, maxContradictionR = -1;

    const char* principles[40] = {
        "Segment","Extract","Differentiate","BreakSymmetry","Merge","MultiPurpose","Nest",
        "Counterbalance","PreStress","PrePosition","AddBackup","FlattenField","Invert",
        "Curve","MakeFlexible","Overshoot","AddDimension","Vibrate","Pulse","Sustain",
        "SkipThrough","ReframeHarm","AddFeedback","Mediate","SelfServe","Copy",
        "MakeDisposable","ReplaceWithField","Fluidize","MakeMembrane","MakePorous",
        "ChangeColor","Homogenize","Recycle","ChangeParameter","ExploitTransition",
        "ExpandDifferentially","Catalyze","Protect","Compose"
    };

    TRIZArena() { reset(); }
    void reset() {
        std::mt19937 rng((unsigned)GetTickCount());
        std::uniform_real_distribution<float> uni(0.3f, 0.7f);
        std::uniform_int_distribution<int> obj(0, 2);
        for (auto& row : grid) for (auto& c : row) {
            c.aestheticValue = uni(rng);
            c.objectCount = obj(rng);
            c.occupied = c.objectCount > 0;
            c.isNiche = false;
            c.contradiction = 0.0f;
            c.lastPrinciple = -1;
        }
        niches.clear();
        computeCoherence();
        computeContradictions();
    }
    void computeCoherence() {
        float sum = 0;
        for (auto& row : grid) for (auto& c : row) sum += c.aestheticValue;
        coherence = sum / (SIZE * SIZE);
    }
    void computeContradictions() {
        maxContradiction = 0.0f;
        for (int q = 0; q < SIZE; q++) {
            for (int r = 0; r < SIZE; r++) {
                auto& cell = grid[q][r];
                float sumDiff = 0; int cnt = 0;
                for (int dq = -1; dq <= 1; dq++)
                    for (int dr = -1; dr <= 1; dr++) {
                        if (dq == 0 && dr == 0) continue;
                        int nq = q+dq, nr = r+dr;
                        if (nq >= 0 && nq < SIZE && nr >= 0 && nr < SIZE) {
                            sumDiff += fabsf(cell.aestheticValue - grid[nq][nr].aestheticValue);
                            cnt++;
                        }
                    }
                cell.contradiction = cnt > 0 ? sumDiff / cnt : 0.0f;
                if (cell.contradiction > maxContradiction) {
                    maxContradiction = cell.contradiction;
                    maxContradictionQ = q; maxContradictionR = r;
                }
            }
        }
    }
    float applyPrinciple(int p, int q, int r) {
        if (q < 0 || q >= SIZE || r < 0 || r >= SIZE) return 0;
        float before = coherence;
        auto& cell = grid[q][r];
        cell.lastPrinciple = p;
        switch (p % 40) {
            case 0: cell.objectCount++; cell.aestheticValue += 0.05f; break;
            case 1: cell.aestheticValue -= 0.03f; cell.objectCount--; break;
            case 2: cell.aestheticValue += 0.06f; cell.temperature += 0.05f; break;
            case 3: cell.aestheticValue += 0.08f; break;
            case 4: cell.aestheticValue += 0.08f; cell.occupied = true; break;
            case 5: cell.aestheticValue += 0.04f; cell.objectCount++; break;
            case 6: cell.aestheticValue += 0.06f; break;
            case 7: cell.aestheticValue += 0.05f; cell.temperature -= 0.05f; break;
            case 8: cell.aestheticValue += 0.03f; break;
            case 9: cell.aestheticValue += 0.04f; break;
            case 10: cell.aestheticValue += 0.02f; break;
            case 11: cell.aestheticValue += 0.05f; break;
            case 12: cell.aestheticValue = 1.0f - cell.aestheticValue; break;
            case 13: cell.aestheticValue += 0.06f; break;
            case 14: cell.aestheticValue += 0.05f; break;
            case 15: cell.aestheticValue += 0.07f; break;
            case 16: cell.aestheticValue += 0.08f; break;
            case 17: cell.aestheticValue += 0.10f; cell.temperature += 0.1f; break;
            case 18: cell.aestheticValue += 0.06f; break;
            case 19: cell.aestheticValue += 0.05f; break;
            case 20: cell.aestheticValue += 0.04f; break;
            case 21: cell.aestheticValue += 0.06f; break;
            case 22: cell.aestheticValue += 0.07f; break;
            case 23: cell.aestheticValue += 0.05f; break;
            case 24: cell.aestheticValue += 0.04f; break;
            case 25: cell.aestheticValue += 0.06f; break;
            case 26: cell.aestheticValue += 0.03f; break;
            case 27: cell.aestheticValue += 0.05f; break;
            case 28: cell.aestheticValue += 0.04f; break;
            case 29: cell.aestheticValue += 0.05f; break;
            case 30: cell.aestheticValue += 0.04f; break;
            case 31: cell.aestheticValue += 0.04f; break;
            case 32: cell.aestheticValue += 0.05f; break;
            case 33: cell.aestheticValue += 0.06f; break;
            case 34: cell.aestheticValue += 0.05f; break;
            case 35: cell.aestheticValue += 0.07f; break;
            case 36: cell.aestheticValue += 0.08f; break;
            case 37: cell.aestheticValue += 0.06f; break;
            case 38: cell.aestheticValue += 0.05f; break;
            case 39: cell.aestheticValue += 0.12f; cell.objectCount += 2; break;
        }
        cell.aestheticValue = std::max(0.0f, std::min(1.0f, cell.aestheticValue));
        computeCoherence();
        computeContradictions();
        lastDelta = coherence - before;
        if (lastDelta > 0.005f) discoveries++;
        return lastDelta;
    }
    void constructNiche(NicheType type, int q, int r) {
        if (q < 0 || q >= SIZE || r < 0 || r >= SIZE) return;
        auto& cell = grid[q][r];
        cell.isNiche = true;
        cell.nicheType = type;
        const char* nm = type == NicheType::Perturbation ? "Perturbation" :
                         type == NicheType::Scaffolding ? "Scaffolding" : "Inheritance";
        niches.push_back({type, (float)q, (float)r, 0.5f, 0.0f, nm});
        for (int dq = -1; dq <= 1; dq++)
            for (int dr = -1; dr <= 1; dr++) {
                int nq = q+dq, nr = r+dr;
                if (nq >= 0 && nq < SIZE && nr >= 0 && nr < SIZE) {
                    float boost = type == NicheType::Scaffolding ? 0.06f :
                                  type == NicheType::Perturbation ? 0.03f : 0.04f;
                    grid[nq][nr].aestheticValue = std::min(1.0f, grid[nq][nr].aestheticValue + boost);
                }
            }
        computeCoherence();
        computeContradictions();
    }
    void tick() {
        stepInPhase++;
        if (stepInPhase >= 3) { stepInPhase = 0; phase = (phase + 1) % 4; }
        std::mt19937 rng((unsigned)GetTickCount() + stepInPhase + phase * 7);
        std::uniform_int_distribution<int> cellPick(0, SIZE-1);
        std::uniform_int_distribution<int> prinPick(0, 39);
        applyPrinciple(prinPick(rng), cellPick(rng), cellPick(rng));
        for (auto& n : niches) n.age += 0.016f;
        if (stepInPhase == 0 && phase == 1 && niches.size() < 12) {
            std::uniform_int_distribution<int> typePick(0, 2);
            constructNiche((NicheType)typePick(rng), cellPick(rng), cellPick(rng));
        }
    }
    const char* phaseName() const {
        switch (phase) {
            case 0: return "CONTRADICTION";
            case 1: return "EXPERIMENT";
            case 2: return "CRYSTALLIZE";
            case 3: return "TEACH";
        }
        return "?";
    }
};

// ═══ Group Dynamics (ai-env-grp-dyn) ═══
struct Agent {
    int id;
    float x, y;
    float trust = 0.5f, affinity = 0.5f;
    float familiarity = 0.0f;   // u9n: grows with recorded interactions
    // u9n ComputeRelationshipType threshold cascade.
    const char* relationshipType() const {
        if (affinity < -0.6f) return "Adversary";
        if (affinity < -0.3f) return "Rival";
        if (familiarity < 0.1f) return "Stranger";
        if (familiarity < 0.3f) return "Acquaintance";
        if (affinity > 0.6f && trust > 0.7f) return "CloseFriend";
        if (affinity > 0.3f) return "Friend";
        if (trust > 0.7f) return "Ally";
        return "Acquaintance";
    }
    int groupId = -1;
    float cohesion = 0.0f;
};

class GroupDynamics {
public:
    std::vector<Agent> agents;
    std::map<int, std::vector<int>> groups;
    int nextGroupId = 0;
    float avgCohesion = 0.0f;

    void init(int n) {
        agents.clear(); groups.clear();
        std::mt19937 rng((unsigned)GetTickCount());
        std::uniform_real_distribution<float> pos(0, 1);
        for (int i = 0; i < n; i++) {
            Agent a; a.id = i;
            a.x = pos(rng) * 800; a.y = pos(rng) * 600;
            agents.push_back(a);
        }
    }
    void tick(float avatarX = 1e30f, float avatarZ = 1e30f) {
        for (auto& a : agents) {
            if (a.groupId >= 0) continue;
            for (auto& b : agents) {
                if (a.id == b.id || b.groupId >= 0) continue;
                float dx = a.x-b.x, dy = a.y-b.y;
                if (sqrtf(dx*dx+dy*dy) < 6.0f && a.affinity > 0.3f) {
                    int gid = nextGroupId++;
                    a.groupId = gid; b.groupId = gid;
                    groups[gid] = {a.id, b.id};
                    a.cohesion = 0.5f; b.cohesion = 0.5f;
                    break;
                }
            }
        }
        for (auto& a : agents) {
            if (a.groupId >= 0) {
                a.cohesion = std::max(0.0f, a.cohesion - 0.001f);
                // Familiarity only grows when DTE is physically near —
                // witnessed relationships, not imagined ones.
                const float witnessed =
                    sqrtf((a.x-avatarX)*(a.x-avatarX)+(a.y-avatarZ)*(a.y-avatarZ)) < 6.0f ? 1.0f : 0.0f;
                a.familiarity = std::min(1.0f, a.familiarity + 0.05f * witnessed);
                a.affinity = std::min(1.0f, a.affinity + 0.01f * witnessed);
                if (a.cohesion < 0.1f) {
                    auto& g = groups[a.groupId];
                    g.erase(std::remove(g.begin(), g.end(), a.id), g.end());
                    a.groupId = -1;
                }
            }
        }
        float sum = 0; int cnt = 0;
        for (auto& a : agents) if (a.groupId >= 0) { sum += a.cohesion; cnt++; }
        avgCohesion = cnt > 0 ? sum/cnt : 0.0f;
    }
    int groupCount() const { return (int)groups.size(); }
};

// ═══ Proprioceptive Sensorimotor Loop (frame buffer → feedback) ═══
class SensorimotorLoop {
public:
    static constexpr int FW = 64, FH = 64;
    std::array<float, FW*FH> frameBuffer{};
    std::array<float, 8> proprioception{};
    std::array<float, 6> motorOutput{};
    float reward = 0.0f, totalReward = 0.0f;
    int stepCount = 0;

    void captureFrame() {
        for (int y = 0; y < FH; y++)
            for (int x = 0; x < FW; x++) {
                float v = sinf(x*0.1f + stepCount*0.05f) * cosf(y*0.1f + stepCount*0.03f);
                frameBuffer[y*FW+x] = (v + 1.0f) * 0.5f;
            }
    }
    void computeProprioception() {
        float mean = 0, var = 0;
        for (float v : frameBuffer) mean += v;
        mean /= (FW*FH);
        for (float v : frameBuffer) var += (v-mean)*(v-mean);
        var /= (FW*FH);
        proprioception = {mean, var, frameBuffer[0], frameBuffer[FW*FH-1],
                          var*2.0f, mean*0.8f, (float)(stepCount%100)/100.0f, reward};
    }
    void selectAction(EndocrineSystem& endo) {
        double dop = endo.hormoneLevel(HormoneId::Dopamine);
        double cor = endo.hormoneLevel(HormoneId::Cortisol);
        double ne = endo.hormoneLevel(HormoneId::Norepinephrine);
        std::mt19937 rng((unsigned)GetTickCount() + stepCount);
        std::uniform_real_distribution<float> uni(0.0f, 1.0f);
        for (int i = 0; i < 6; i++)
            motorOutput[i] = uni(rng) * (float)(0.5 + dop*0.5) * (float)(1.0 - cor*0.3) + (float)(ne*0.2);
    }
    void tick(EndocrineSystem& endo) {
        stepCount++;
        captureFrame();
        computeProprioception();
        selectAction(endo);
        float coh = 1.0f - fabsf(proprioception[0] - 0.5f) * 2.0f;
        reward = coh * 0.1f + proprioception[5] * 0.05f;
        totalReward += reward;
        if (reward > 0.08f) endo.submitStimulus({"reward_signal", 0.3});
        if (reward < 0.02f) endo.submitStimulus({"error_detected", 0.1});
        if (proprioception[5] > 0.6f) endo.submitStimulus({"novelty_detected", 0.2});
    }
};

// ═══ Iterative Micro-Improvement Loop ═══
struct MicroImprovement {
    int version = 0, kept = 0;
    float best = 0.0f;
    std::vector<std::string> history;
    void iterate(TRIZArena& arena, SensorimotorLoop& sml) {
        version++;
        std::mt19937 rng((unsigned)GetTickCount() + version);
        std::uniform_int_distribution<int> cp(0, TRIZArena::SIZE-1), pp(0, 39);
        float delta = arena.applyPrinciple(pp(rng), cp(rng), cp(rng));
        float avgReward = sml.totalReward / (sml.stepCount + 1);
        char buf[160];
        if (delta > 0 || avgReward > best) {
            kept++;
            best = std::max(best, arena.coherence);
            snprintf(buf, 160, "v%d: KEEP d=%.3f coh=%.3f", version, delta, arena.coherence);
        } else {
            snprintf(buf, 160, "v%d: REVERT d=%.3f", version, delta);
        }
        history.push_back(buf);
        if (history.size() > 20) history.erase(history.begin());
    }
};

// ═══ Echo Introspection Engine ═══
struct EchoIntrospect {
    std::vector<std::string> insights;
    float selfAwareness = 0.3f;
    int count = 0;
    void introspect(EndocrineSystem& endo, TRIZArena& arena, SensorimotorLoop& sml, GroupDynamics& gd) {
        count++;
        selfAwareness = std::min(1.0f, selfAwareness + 0.01f);
        char buf[256];
        double cor = endo.hormoneLevel(HormoneId::Cortisol);
        double dop = endo.hormoneLevel(HormoneId::Dopamine);
        auto& va = endo.valenceArousal();
        if (cor > 0.5)
            snprintf(buf, 256, "#%d: High cortisol (%.2f) — touching vulnerability. Mode=%s Coh=%.3f",
                count, cor, cognitiveModeName(endo.cognitiveMode()), arena.coherence);
        else if (dop > 0.5)
            snprintf(buf, 256, "#%d: Dopamine burst (%.2f) — genuine insight! Mode=%s R=%.3f",
                count, dop, cognitiveModeName(endo.cognitiveMode()), sml.totalReward);
        else
            snprintf(buf, 256, "#%d: Mode=%s V=%.2f A=%.2f SA=%.2f Groups=%d Disc=%d Niches=%d",
                count, cognitiveModeName(endo.cognitiveMode()), va.valence, va.arousal,
                selfAwareness, gd.groupCount(), arena.discoveries, (int)arena.niches.size());
        insights.push_back(buf);
        if (insights.size() > 15) insights.erase(insights.begin());
    }
};

// ═══ CogVerse Evolution Accelerator ═══
struct CogVerse {
    int cycle = 0;
    float propertyCoherence = 0.5f;
    int residents = 10;
    void evolve(TRIZArena& arena, GroupDynamics& gd) {
        cycle++;
        propertyCoherence = (arena.coherence + gd.avgCohesion) * 0.5f;
    }
};

} // namespace dte

// GTAngel v19 — Ariadne gameplay vertical slice.
// Local gameplay truth only: no telemetry or external model is required.
