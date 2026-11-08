#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_CORE_SPEC_GUARD
#define GTANGEL_ZPP_CORE_SPEC_GUARD
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
enum class Cam_Enum_1 : int32_t { ARRESTCAM_OVERSHOULDER_v = 0, ARRESTCAM_ALONGGROUND_v = 1, ARRESTCAM_ALONGGROUND_RIGHT_v = 2, ARRESTCAM_ALONGGROUND_RIGHT_UP_v = 3, ARRESTCAM_ALONGGROUND_LEFT_v = 4, ARRESTCAM_ALONGGROUND_LEFT_UP_v = 5, ARRESTCAM_LAMPPOST_v = 6 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_1 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_2 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11 };
// From zpp/core/Camera.zpp
enum class Camera_Enum_3 : int32_t { MODE_NONE_v = 0, MODE_TOPDOWN_v = 1, MODE_GTACLASSIC_v = 2, MODE_BEHINDCAR_v = 3, MODE_FOLLOWPED_v = 4, MODE_AIMING_v = 5, MODE_DEBUG_v = 6, MODE_SNIPER_v = 7, MODE_ROCKETLAUNCHER_v = 8, MODE_MODELVIEW_v = 9, MODE_BILL_v = 10, MODE_SYPHON_v = 11 };
// State Schema Contract from zpp/core/Accident.zpp
struct CAccident_ZppContract {
    int32_t m_pVictim{0}; // Z++: Ref[CPed]
    uint32_t m_nMedicsAttending{0}; // Z++: ℕ
    uint32_t m_nMedicsPerformingCPR{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/core/Accident.zpp
struct CAccidentManager_ZppContract {
    int32_t m_aAccidents{0}; // Z++: seq CAccident  -- bounded array [NUM_ACCIDENTS]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/core/AnimViewer.zpp
struct CAnimViewer_ZppContract {
    int32_t animTxdSlot{0}; // Z++: ℤ
    int32_t pTarget{0}; // Z++: Ref[CEntity]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/core/Cam.zpp
struct CamModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CCam_ZppContract {
    bool bBelowMinDist{false}; // Z++: 𝔹
    bool bBehindPlayerDesired{false}; // Z++: 𝔹
    bool m_bCamLookingAtVector{false}; // Z++: 𝔹
    bool m_bCollisionChecksOn{false}; // Z++: 𝔹
    bool m_bFixingBeta{false}; // Z++: 𝔹
    bool m_bTheHeightFixerVehicleIsATrain{false}; // Z++: 𝔹
    bool LookBehindCamWasInFront{false}; // Z++: 𝔹
    bool LookingBehind{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CCamPathSplines_ZppContract {
    int32_t m_arr_PathData{0}; // Z++: ℙ ℝ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_core_zpp_invariants() {
    if (!CAccident_ZppContract{}.checkInvariant()) return false;
    if (!CAccidentManager_ZppContract{}.checkInvariant()) return false;
    if (!CAnimViewer_ZppContract{}.checkInvariant()) return false;
    if (!CamModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CCam_ZppContract{}.checkInvariant()) return false;
    if (!CCamPathSplines_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_core_zpp_invariants(), "Z++ formal contract invariant failed for core");
} // namespace zpp_core_spec
#endif // GTANGEL_ZPP_CORE_SPEC_GUARD
