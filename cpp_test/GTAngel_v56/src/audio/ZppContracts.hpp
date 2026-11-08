#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_AUDIO_SPEC_GUARD
#define GTANGEL_ZPP_AUDIO_SPEC_GUARD
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
enum class AudioLogic_Enum_1 : int32_t { CAR_HELI_MAX_DIST_v = 0, CAR_HELI_ENGINE_MAX_DIST_v = 1, CAR_HELI_ENGINE_START_MAX_DIST_v = 2, CAR_HELI_ENGINE_START_VOLUME_v = 3, CAR_HELI_SEAPLANE_MAX_DIST_v = 4, CAR_HELI_SEAPLANE_VOLUME_v = 5, CAR_HELI_REAR_MAX_DIST_v = 6, CAR_HELI_REAR_VOLUME_v = 7, FLAT_TYRE_MAX_DIST_v = 8, FLAT_TYRE_VOLUME_v = 9, RAIN_ON_VEHICLE_MAX_DIST_v = 10, RAIN_ON_VEHICLE_VOLUME_v = 11 };
// From zpp/audio/AudioLogic.zpp
enum class eVehicleModel : int32_t { LANDSTAL_v = 0, IDAHO_v = 1, STINGER_v = 2, LINERUN_v = 3, PEREN_v = 4, SENTINEL_v = 5, RIO_v = 6, FIRETRUK_v = 7, TRASH_v = 8, STRETCH_v = 9, MANANA_v = 10, INFERNUS_v = 11 };
// From zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_3 : int32_t { OLD_DOOR_v = 0, NEW_DOOR_v = 1, TRUCK_DOOR_v = 2, BUS_DOOR_v = 3 };
// From zpp/audio/AudioLogic.zpp
enum class AudioLogic_Enum_4 : int32_t { CAR_HELI_MAX_DIST_v = 0, CAR_HELI_ENGINE_MAX_DIST_v = 1, CAR_HELI_ENGINE_START_MAX_DIST_v = 2, CAR_HELI_ENGINE_START_VOLUME_v = 3, CAR_HELI_SEAPLANE_MAX_DIST_v = 4, CAR_HELI_SEAPLANE_VOLUME_v = 5, CAR_HELI_REAR_MAX_DIST_v = 6, CAR_HELI_REAR_VOLUME_v = 7, FLAT_TYRE_MAX_DIST_v = 8, FLAT_TYRE_VOLUME_v = 9, RAIN_ON_VEHICLE_MAX_DIST_v = 10, RAIN_ON_VEHICLE_VOLUME_v = 11 };
// State Schema Contract from zpp/audio/AudioCollision.zpp
struct cAudioCollision_ZppContract {
    int32_t m_pEntity1{0}; // Z++: Ref[CEntity]
    int32_t m_pEntity2{0}; // Z++: Ref[CEntity]
    uint32_t m_bSurface1{0}; // Z++: ℕ
    uint32_t m_bSurface2{0}; // Z++: ℕ
    float m_fIntensity1{0.0f}; // Z++: ℝ
    float m_fIntensity2{0.0f}; // Z++: ℝ
    int32_t m_vecPosition{0}; // Z++: CVector
    float m_fDistance{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/audio/AudioCollision.zpp
struct cAudioCollisionManager_ZppContract {
    int32_t m_asCollisions1{0}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    int32_t m_asCollisions2{0}; // Z++: seq cAudioCollision  -- bounded array [NUMAUDIOCOLLISIONS]
    int32_t m_bIndicesTable{0}; // Z++: seq ℕ  -- bounded array [NUMAUDIOCOLLISIONS]
    uint32_t m_bCollisionsInQueue{0}; // Z++: ℕ
    int32_t m_sQueue{0}; // Z++: cAudioCollision
    int32_t NUMAUDIOCOLLISIONS{0}; // Z++: i
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct tVehicleSampleData_ZppContract {
    int32_t m_nAccelerationSampleIndex{0}; // Z++: eSfxSample
    uint32_t m_nBank{0}; // Z++: ℕ
    int32_t m_nHornSample{0}; // Z++: eSfxSample
    int32_t m_nHornFrequency{0}; // Z++: ℤ
    uint32_t m_nSirenOrAlarmSample{0}; // Z++: ℕ
    int32_t m_nSirenOrAlarmFrequency{0}; // Z++: ℤ
    uint32_t m_bDoorType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct sToniReactions_ZppContract {
    int32_t sfx{0}; // Z++: seq ℕ  -- bounded array [4]
    int32_t count{0}; // Z++: seq ℕ  -- bounded array [4]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct MissionAudioData_ZppContract {
    int32_t m_pName{0}; // Z++: seq CHAR
    uint32_t m_nId{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/audio/AudioLogic.zpp
struct tHelicopterSampleData_ZppContract {
    float m_fMaxDistance{0.0f}; // Z++: ℝ
    float m_fBaseDistance{0.0f}; // Z++: ℝ
    uint32_t m_bBaseVolume{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
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
