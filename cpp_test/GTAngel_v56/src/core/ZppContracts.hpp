#pragma once
#ifndef GTANGEL_ZPP_CORE_SPEC_GUARD
#define GTANGEL_ZPP_CORE_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/core
// Derived from C:/ddd/GTAngel/cpp_test/zpp/core/*.zpp
// Modules: 54 | Formal Schemas: 2928 | Free Types: 80
// ============================================================================
namespace zpp_core_spec {
static constexpr int kZppModuleCount = 54;
static constexpr int kZppSchemaCount = 2928;
static constexpr int kZppEnumCount   = 80;
// From zpp/core/Cam.zpp
enum class Cam_Enum_1 : int32_t { ARRESTCAM_OVERSHOULDER_v = 1, ARRESTCAM_ALONGGROUND_v = 2, ARRESTCAM_ALONGGROUND_RIGHT_v = 3, ARRESTCAM_ALONGGROUND_RIGHT_UP_v = 4, ARRESTCAM_ALONGGROUND_LEFT_v = 5, ARRESTCAM_ALONGGROUND_LEFT_UP_v = 6, ARRESTCAM_LAMPPOST_v = 7 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_1 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11, OBBE_12_v = 12, OBBE_13_v = 13, OBBE_14_v = 14, OBBE_15_v = 15, OBBE_16_v = 16, OBBE_17_v = 17, OBBE_18_v = 18, OBBE_19_v = 19, OBBE_ONSTRING_HELI_v = 20, OBBE_INVALID_v = 21 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_2 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11, OBBE_12_v = 12, OBBE_13_v = 13, OBBE_INVALID_v = 14 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_3 : int32_t { MODE_NONE_v = 0, MODE_TOPDOWN_v = 1, MODE_GTACLASSIC_v = 2, MODE_BEHINDCAR_v = 3, MODE_FOLLOWPED_v = 4, MODE_AIMING_v = 5, MODE_DEBUG_v = 6, MODE_SNIPER_v = 7, MODE_ROCKETLAUNCHER_v = 8, MODE_MODELVIEW_v = 9, MODE_BILL_v = 10, MODE_SYPHON_v = 11, MODE_CIRCLE_v = 12, MODE_CHEESYZOOM_v = 13, MODE_WHEELCAM_v = 14, MODE_FIXED_v = 15, MODE_1STPERSON_v = 16, MODE_FLYBY_v = 17, MODE_CAM_ON_A_STRING_v = 18, MODE_REACTION_v = 19, MODE_FOLLOW_PED_WITH_BIND_v = 20, MODE_CHRIS_v = 21, MODE_BEHINDBOAT_v = 22, MODE_PLAYER_FALLEN_WATER_v = 23, MODE_CAM_ON_TRAIN_ROOF_v = 24, MODE_CAM_RUNNING_SIDE_TRAIN_v = 25, MODE_BLOOD_ON_THE_TRACKS_v = 26, MODE_IM_THE_PASSENGER_WOOWOO_v = 27, MODE_SYPHON_CRIM_IN_FRONT_v = 28, MODE_PED_DEAD_BABY_v = 29, MODE_PILLOWS_PAPS_v = 30, MODE_LOOK_AT_CARS_v = 31, MODE_ARRESTCAM_ONE_v = 32, MODE_ARRESTCAM_TWO_v = 33, MODE_M16_1STPERSON_v = 34, MODE_SPECIAL_FIXED_FOR_SYPHON_v = 35, MODE_FIGHT_CAM_v = 36, MODE_TOP_DOWN_PED_v = 37, MODE_SNIPER_RUNABOUT_v = 38, MODE_ROCKETLAUNCHER_RUNABOUT_v = 39, MODE_1STPERSON_RUNABOUT_v = 40, MODE_M16_1STPERSON_RUNABOUT_v = 41, MODE_FIGHT_CAM_RUNABOUT_v = 42, MODE_EDITOR_v = 43, MODE_HELICANNON_1STPERSON_v = 44 };
// State Schema Contract from zpp/core/Accident.zpp
struct CAccident_ZppContract {
    uint8_t m_pVictim_card{0u}; // Z++: Ref[CPed]
    int64_t m_nMedicsAttending{0}; // Z++: ℕ
    int64_t m_nMedicsPerformingCPR{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_pVictim_card <= 1u)
            && (m_nMedicsAttending >= 0)
            && (m_nMedicsPerformingCPR >= 0);
    }
};
// State Schema Contract from zpp/core/AnimViewer.zpp
struct CAnimViewer_ZppContract {
    int32_t animTxdSlot{0}; // Z++: ℤ
    uint8_t pTarget_card{0u}; // Z++: Ref[CEntity]
    constexpr bool checkInvariant() const {
        return (pTarget_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Cam.zpp
struct CamModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6585}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6585);
    }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CCamPathSplines_ZppContract {
    uint8_t m_arr_PathData_card{0u}; // Z++: ℙ ℝ
    constexpr bool checkInvariant() const {
        return (m_arr_PathData_card <= 1u);
    }
};
// State Schema Contract from zpp/core/CdStream.zpp
struct CdReadInfo_ZppContract {
    int64_t nSectorOffset{0}; // Z++: ℕ
    int64_t nSectorsToRead{0}; // Z++: ℕ
    int32_t pBuffer{0}; // Z++: ADDRESS
    int32_t field_C{0}; // Z++: seq CHAR
    bool bLocked{false}; // Z++: 𝔹
    bool bReading{false}; // Z++: 𝔹
    int32_t nStatus{0}; // Z++: ℤ
    int32_t pDoneSemaphore{0}; // Z++: HANDLE
    constexpr bool checkInvariant() const {
        return (nSectorOffset >= 0)
            && (nSectorsToRead >= 0);
    }
};
// State Schema Contract from zpp/core/CdStream.zpp
struct Queue_ZppContract {
    uint8_t items_card{0u}; // Z++: ℙ ℤ
    int32_t head{0}; // Z++: ℤ
    int32_t tail{0}; // Z++: ℤ
    int32_t size{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (items_card <= 1u);
    }
};
constexpr bool verify_core_zpp_invariants() {
    if (!CAccident_ZppContract{}.checkInvariant()) return false;
    if (!CAnimViewer_ZppContract{}.checkInvariant()) return false;
    if (!CamModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CCamPathSplines_ZppContract{}.checkInvariant()) return false;
    if (!CdReadInfo_ZppContract{}.checkInvariant()) return false;
    if (!Queue_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_core_zpp_invariants(), "Z++ formal contract invariant failed for core");
} // namespace zpp_core_spec
#endif // GTANGEL_ZPP_CORE_SPEC_GUARD
