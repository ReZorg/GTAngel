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
// Free Type from zpp/core/Cam.zpp
enum class Cam_Enum_1 : int32_t { ARRESTCAM_OVERSHOULDER_v = 1, ARRESTCAM_ALONGGROUND_v = 2, ARRESTCAM_ALONGGROUND_RIGHT_v = 3, ARRESTCAM_ALONGGROUND_RIGHT_UP_v = 4, ARRESTCAM_ALONGGROUND_LEFT_v = 5, ARRESTCAM_ALONGGROUND_LEFT_UP_v = 6, ARRESTCAM_LAMPPOST_v = 7 };
// Free Type from zpp/core/Camera.zpp
enum class Camera_Enum_1 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11, OBBE_12_v = 12, OBBE_13_v = 13, OBBE_14_v = 14, OBBE_15_v = 15, OBBE_16_v = 16, OBBE_17_v = 17, OBBE_18_v = 18, OBBE_19_v = 19, OBBE_ONSTRING_HELI_v = 20, OBBE_INVALID_v = 21 };
// Free Type from zpp/core/Camera.zpp
enum class Camera_Enum_2 : int32_t { OBBE_WHEEL_v = 0, OBBE_1_v = 1, OBBE_2_v = 2, OBBE_3_v = 3, OBBE_1STPERSON_v = 4, OBBE_5_v = 5, OBBE_ONSTRING_v = 6, OBBE_COPCAR_v = 7, OBBE_COPCAR_WHEEL_v = 8, OBBE_9_v = 9, OBBE_10_v = 10, OBBE_11_v = 11, OBBE_12_v = 12, OBBE_13_v = 13, OBBE_INVALID_v = 14 };
// Free Type from zpp/core/Camera.zpp
enum class Camera_Enum_3 : int32_t { MODE_NONE_v = 0, MODE_TOPDOWN_v = 1, MODE_GTACLASSIC_v = 2, MODE_BEHINDCAR_v = 3, MODE_FOLLOWPED_v = 4, MODE_AIMING_v = 5, MODE_DEBUG_v = 6, MODE_SNIPER_v = 7, MODE_ROCKETLAUNCHER_v = 8, MODE_MODELVIEW_v = 9, MODE_BILL_v = 10, MODE_SYPHON_v = 11, MODE_CIRCLE_v = 12, MODE_CHEESYZOOM_v = 13, MODE_WHEELCAM_v = 14, MODE_FIXED_v = 15, MODE_1STPERSON_v = 16, MODE_FLYBY_v = 17, MODE_CAM_ON_A_STRING_v = 18, MODE_REACTION_v = 19, MODE_FOLLOW_PED_WITH_BIND_v = 20, MODE_CHRIS_v = 21, MODE_BEHINDBOAT_v = 22, MODE_PLAYER_FALLEN_WATER_v = 23, MODE_CAM_ON_TRAIN_ROOF_v = 24, MODE_CAM_RUNNING_SIDE_TRAIN_v = 25, MODE_BLOOD_ON_THE_TRACKS_v = 26, MODE_IM_THE_PASSENGER_WOOWOO_v = 27, MODE_SYPHON_CRIM_IN_FRONT_v = 28, MODE_PED_DEAD_BABY_v = 29, MODE_PILLOWS_PAPS_v = 30, MODE_LOOK_AT_CARS_v = 31, MODE_ARRESTCAM_ONE_v = 32, MODE_ARRESTCAM_TWO_v = 33, MODE_M16_1STPERSON_v = 34, MODE_SPECIAL_FIXED_FOR_SYPHON_v = 35, MODE_FIGHT_CAM_v = 36, MODE_TOP_DOWN_PED_v = 37, MODE_SNIPER_RUNABOUT_v = 38, MODE_ROCKETLAUNCHER_RUNABOUT_v = 39, MODE_1STPERSON_RUNABOUT_v = 40, MODE_M16_1STPERSON_RUNABOUT_v = 41, MODE_FIGHT_CAM_RUNABOUT_v = 42, MODE_EDITOR_v = 43, MODE_HELICANNON_1STPERSON_v = 44 };
// Free Type from zpp/core/Camera.zpp
enum class Camera_Enum_5 : int32_t { LOOKING_BEHIND_v = 0, LOOKING_LEFT_v = 1, LOOKING_RIGHT_v = 2, LOOKING_FORWARD_v = 3 };
// Free Type from zpp/core/Camera.zpp
enum class Camera_Enum_6 : int32_t { FADE_0_v = 0, FADE_1_v = 1, FADE_2_v = 2, FADE_OUT_v = 0, FADE_IN_v = 1, FADE_NONE_v = 2 };
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
// State Schema Contract from zpp/core/Camera.zpp
struct CTrainCamNode_ZppContract {
    int32_t m_cvecCamPosition{0}; // Z++: CVector
    int32_t m_cvecPointToLookAt{0}; // Z++: CVector
    int32_t m_cvecMinPointInRange{0}; // Z++: CVector
    int32_t m_cvecMaxPointInRange{0}; // Z++: CVector
    float m_fDesiredFOV{0.0f}; // Z++: ℝ
    float m_fNearClip{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CQueuedMode_ZppContract {
    int32_t Mode{0}; // Z++: ℤ
    float Duration{0.0f}; // Z++: ℝ
    int32_t MinZoom{0}; // Z++: ℤ
    int32_t MaxZoom{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
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
    int32_t hFile{0}; // Z++: HANDLE
    int32_t Overlapped{0}; // Z++: OVERLAPPED
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
// State Schema Contract from zpp/core/Clock.zpp
struct CClock_ZppContract {
    int64_t ms_nGameClockHours{0}; // Z++: ℕ
    int64_t ms_nGameClockMinutes{0}; // Z++: ℕ
    int64_t ms_nGameClockSeconds{0}; // Z++: ℕ
    int64_t ms_Stored_nGameClockHours{0}; // Z++: ℕ
    int64_t ms_Stored_nGameClockMinutes{0}; // Z++: ℕ
    int64_t ms_Stored_nGameClockSeconds{0}; // Z++: ℕ
    int64_t ms_nMillisecondsPerGameMinute{0}; // Z++: ℕ
    int64_t ms_nLastClockTick{0}; // Z++: ℕ
    bool ms_bClockHasBeenStored{false}; // Z++: 𝔹
    float ms_EnvMapTimeMultiplicator{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (ms_nGameClockHours >= 0)
            && (ms_nGameClockMinutes >= 0)
            && (ms_nGameClockSeconds >= 0)
            && (ms_Stored_nGameClockHours >= 0)
            && (ms_Stored_nGameClockMinutes >= 0)
            && (ms_Stored_nGameClockSeconds >= 0)
            && (ms_nMillisecondsPerGameMinute >= 0)
            && (ms_nLastClockTick >= 0);
    }
};
// State Schema Contract from zpp/core/ControllerConfig.zpp
struct GlfwJoyState_ZppContract {
    int32_t id{0}; // Z++: ℤ
    bool isGamepad{false}; // Z++: 𝔹
    int64_t numButtons{0}; // Z++: ℕ
    uint8_t buttons_card{0u}; // Z++: ℙ ℕ
    std::array<bool, 17> mappedButtons{}; // Z++: seq 𝔹  -- bounded array [17]
    size_t mappedButtons_len{17u}; // Z++ cardinality #mappedButtons
    constexpr bool checkInvariant() const {
        return (mappedButtons_len == 17u && mappedButtons.size() == 17u)
            && (numButtons >= 0)
            && (buttons_card <= 1u);
    }
};
// State Schema Contract from zpp/core/ControllerConfig.zpp
struct tControllerConfigBind_ZppContract {
    int32_t m_Key{0}; // Z++: ℤ
    int32_t m_ContSetOrder{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Crime.zpp
struct CCrimeBeingQd_ZppContract {
    int32_t m_nType{0}; // Z++: eCrimeType
    int32_t m_nId{0}; // Z++: ℤ
    int64_t m_nTime{0}; // Z++: ℕ
    int32_t m_vecPosn{0}; // Z++: CVector
    bool m_bReported{false}; // Z++: 𝔹
    bool m_bPoliceDoesntCare{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nTime >= 0);
    }
};
// State Schema Contract from zpp/core/Debug.zpp
struct Line_ZppContract {
    int32_t p2{0}; // Z++: CVector_p1
    int64_t c2{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (c2 >= 0);
    }
};
// State Schema Contract from zpp/core/EventList.zpp
struct CEvent_ZppContract {
    int32_t type{0}; // Z++: eEventType
    int32_t entityType{0}; // Z++: eEventEntity
    int32_t entityRef{0}; // Z++: ℤ
    uint8_t criminal_card{0u}; // Z++: Ref[CPed]
    int32_t posn{0}; // Z++: CVector
    int64_t timeout{0}; // Z++: ℕ
    int32_t state{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (criminal_card <= 1u)
            && (timeout >= 0);
    }
};
// State Schema Contract from zpp/core/EventList.zpp
struct CEventList_ZppContract {
    int32_t ms_nFirstFreeSlotIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FileLoader.zpp
struct ColHeader_ZppContract {
    int64_t ident{0}; // Z++: ℕ
    int64_t size{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (ident >= 0)
            && (size >= 0);
    }
};
// State Schema Contract from zpp/core/FileMgr.zpp
struct myFILE_ZppContract {
    bool isText{false}; // Z++: 𝔹
    uint8_t file_card{0u}; // Z++: Ref[FILE]
    constexpr bool checkInvariant() const {
        return (file_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Fire.zpp
struct CFire_ZppContract {
    bool m_bIsOngoing{false}; // Z++: 𝔹
    bool m_bIsScriptFire{false}; // Z++: 𝔹
    bool m_bPropagationFlag{false}; // Z++: 𝔹
    bool m_bAudioSet{false}; // Z++: 𝔹
    int32_t m_vecPos{0}; // Z++: CVector
    uint8_t m_pEntity_card{0u}; // Z++: Ref[CEntity]
    uint8_t m_pSource_card{0u}; // Z++: Ref[CEntity]
    int64_t m_nExtinguishTime{0}; // Z++: ℕ
    int64_t m_nStartTime{0}; // Z++: ℕ
    int64_t m_nNextTimeToAddFlames{0}; // Z++: ℕ
    float m_fStrength{0.0f}; // Z++: ℝ
    float m_fWaterExtinguishCountdown{0.0f}; // Z++: ℝ
    bool m_bExtinguishedWithWater{false}; // Z++: 𝔹
    int32_t field_20{0}; // Z++: ℤ
    int64_t m_nFiremenPuttingOut{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_pEntity_card <= 1u)
            && (m_pSource_card <= 1u)
            && (m_nExtinguishTime >= 0)
            && (m_nStartTime >= 0)
            && (m_nNextTimeToAddFlames >= 0)
            && (m_nFiremenPuttingOut >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CTriggerCaller_ZppContract {
    bool bHasTrigger{false}; // Z++: 𝔹
    int32_t pTrigger{0}; // Z++: ADDRESS
    int32_t field_C{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableText_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    int32_t m_text{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableTextTwoLines_ZppContract {
    int32_t m_line1{0}; // Z++: CPlaceableText
    int32_t m_line2{0}; // Z++: CPlaceableText
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CShadowInfo_ZppContract {
    bool m_bRightJustify{false}; // Z++: 𝔹
    bool m_bDropShadow{false}; // Z++: 𝔹
    int32_t m_shadowColor{0}; // Z++: CRGBA
    int32_t m_shadowOffset{0}; // Z++: CVector2D
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CSelectable_ZppContract {
    bool m_bSelected{false}; // Z++: 𝔹
    int32_t m_selectedColor{0}; // Z++: CRGBA
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableSprite_ZppContract {
    uint8_t m_pSprite_card{0u}; // Z++: Ref[CSprite2d]
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_size{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    constexpr bool checkInvariant() const {
        return (m_pSprite_card <= 1u);
    }
};
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_Initialise(const CClock_ZppContract& pre, const CClock_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nGameClockHours == 12) && (post.ms_nGameClockMinutes == 0) && (post.ms_nGameClockSeconds == 0) && (post.ms_nMillisecondsPerGameMinute == 1) && (post.ms_nLastClockTick == 1) && (post.ms_bClockHasBeenStored == false);
}
constexpr CClock_ZppContract apply_CClock_Initialise(const CClock_ZppContract& pre) {
    CClock_ZppContract post = pre;
    post.ms_nGameClockHours = 12;
    post.ms_nGameClockMinutes = 0;
    post.ms_nGameClockSeconds = 0;
    post.ms_nMillisecondsPerGameMinute = 1;
    post.ms_nLastClockTick = 1;
    post.ms_bClockHasBeenStored = false;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_Update(const CClock_ZppContract& pre, const CClock_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nLastClockTick == 1) && (post.ms_nGameClockMinutes == 0) && (post.ms_nGameClockHours == 0);
}
constexpr CClock_ZppContract apply_CClock_Update(const CClock_ZppContract& pre) {
    CClock_ZppContract post = pre;
    post.ms_nLastClockTick = 1;
    post.ms_nGameClockMinutes = 0;
    post.ms_nGameClockHours = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_SetGameClock(const CClock_ZppContract& pre, const CClock_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nGameClockMinutes == 1) && (post.ms_nGameClockHours == 1) && (post.ms_nGameClockSeconds == 0) && (post.ms_nLastClockTick == 1);
}
constexpr CClock_ZppContract apply_CClock_SetGameClock(const CClock_ZppContract& pre) {
    CClock_ZppContract post = pre;
    post.ms_nGameClockMinutes = 1;
    post.ms_nGameClockHours = 1;
    post.ms_nGameClockSeconds = 0;
    post.ms_nLastClockTick = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_StoreClock(const CClock_ZppContract& pre, const CClock_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_Stored_nGameClockHours == pre.ms_nGameClockHours) && (post.ms_Stored_nGameClockMinutes == pre.ms_nGameClockMinutes) && (post.ms_Stored_nGameClockSeconds == pre.ms_nGameClockSeconds) && (post.ms_bClockHasBeenStored == true);
}
constexpr CClock_ZppContract apply_CClock_StoreClock(const CClock_ZppContract& pre) {
    CClock_ZppContract post = pre;
    post.ms_Stored_nGameClockHours = pre.ms_nGameClockHours;
    post.ms_Stored_nGameClockMinutes = pre.ms_nGameClockMinutes;
    post.ms_Stored_nGameClockSeconds = pre.ms_nGameClockSeconds;
    post.ms_bClockHasBeenStored = true;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_RestoreClock(const CClock_ZppContract& pre, const CClock_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nGameClockHours == pre.ms_Stored_nGameClockHours) && (post.ms_nGameClockMinutes == pre.ms_Stored_nGameClockMinutes) && (post.ms_nGameClockSeconds == pre.ms_Stored_nGameClockSeconds);
}
constexpr CClock_ZppContract apply_CClock_RestoreClock(const CClock_ZppContract& pre) {
    CClock_ZppContract post = pre;
    post.ms_nGameClockHours = pre.ms_Stored_nGameClockHours;
    post.ms_nGameClockMinutes = pre.ms_Stored_nGameClockMinutes;
    post.ms_nGameClockSeconds = pre.ms_Stored_nGameClockSeconds;
    return post;
}
// Delta Operation Schema Contract (ΔCFire) from zpp/core/Fire.zpp
constexpr bool op_CFire_CFire(const CFire_ZppContract& pre, const CFire_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bIsOngoing == false) && (post.m_bIsScriptFire == false) && (post.m_bPropagationFlag == true) && (post.m_bAudioSet == true) && (post.m_vecPos == 1) && (post.m_nExtinguishTime == 0);
}
constexpr CFire_ZppContract apply_CFire_CFire(const CFire_ZppContract& pre) {
    CFire_ZppContract post = pre;
    post.m_bIsOngoing = false;
    post.m_bIsScriptFire = false;
    post.m_bPropagationFlag = true;
    post.m_bAudioSet = true;
    post.m_vecPos = 1;
    post.m_nExtinguishTime = 0;
    return post;
}
constexpr bool verify_core_zpp_invariants() {
    {
        CAccident_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAccident_ZppContract bad{};
        bad.m_pVictim_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAnimViewer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAnimViewer_ZppContract bad{};
        bad.pTarget_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CamModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CamModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CCamPathSplines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCamPathSplines_ZppContract bad{};
        bad.m_arr_PathData_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTrainCamNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTrainCamNode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CQueuedMode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CQueuedMode_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CdReadInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CdReadInfo_ZppContract bad{};
        bad.nSectorOffset = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Queue_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Queue_ZppContract bad{};
        bad.items_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CClock_ZppContract bad{};
        bad.ms_nGameClockHours = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        GlfwJoyState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GlfwJoyState_ZppContract bad{};
        bad.mappedButtons_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tControllerConfigBind_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tControllerConfigBind_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCrimeBeingQd_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCrimeBeingQd_ZppContract bad{};
        bad.m_nTime = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Line_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Line_ZppContract bad{};
        bad.c2 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CEvent_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEvent_ZppContract bad{};
        bad.criminal_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CEventList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEventList_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        ColHeader_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ColHeader_ZppContract bad{};
        bad.ident = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        myFILE_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        myFILE_ZppContract bad{};
        bad.file_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CFire_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFire_ZppContract bad{};
        bad.m_pEntity_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTriggerCaller_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTriggerCaller_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableText_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableText_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableTextTwoLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableTextTwoLines_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShadowInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShadowInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSelectable_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSelectable_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableSprite_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableSprite_ZppContract bad{};
        bad.m_pSprite_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClock_ZppContract pre{};
        pre.ms_nGameClockHours = 0;
        pre.ms_nGameClockMinutes = 1;
        pre.ms_nGameClockSeconds = 1;
        pre.ms_nMillisecondsPerGameMinute = 0;
        pre.ms_nLastClockTick = 0;
        pre.ms_bClockHasBeenStored = true;
        const auto post = apply_CClock_Initialise(pre);
        if (!op_CClock_Initialise(pre, post)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = 0;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = 1;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockSeconds = 1;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nMillisecondsPerGameMinute = 0;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = 0;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_bClockHasBeenStored = true;
            if (op_CClock_Initialise(pre, badPost)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        pre.ms_nLastClockTick = 0;
        pre.ms_nGameClockMinutes = 1;
        pre.ms_nGameClockHours = 1;
        const auto post = apply_CClock_Update(pre);
        if (!op_CClock_Update(pre, post)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = 0;
            if (op_CClock_Update(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = 1;
            if (op_CClock_Update(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = 1;
            if (op_CClock_Update(pre, badPost)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        pre.ms_nGameClockMinutes = 0;
        pre.ms_nGameClockHours = 0;
        pre.ms_nGameClockSeconds = 1;
        pre.ms_nLastClockTick = 0;
        const auto post = apply_CClock_SetGameClock(pre);
        if (!op_CClock_SetGameClock(pre, post)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = 0;
            if (op_CClock_SetGameClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = 0;
            if (op_CClock_SetGameClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockSeconds = 1;
            if (op_CClock_SetGameClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = 0;
            if (op_CClock_SetGameClock(pre, badPost)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        pre.ms_nGameClockHours = 7; pre.ms_Stored_nGameClockHours = 0;
        pre.ms_nGameClockMinutes = 7; pre.ms_Stored_nGameClockMinutes = 0;
        pre.ms_nGameClockSeconds = 7; pre.ms_Stored_nGameClockSeconds = 0;
        pre.ms_bClockHasBeenStored = false;
        const auto post = apply_CClock_StoreClock(pre);
        if (!op_CClock_StoreClock(pre, post)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_Stored_nGameClockHours = pre.ms_nGameClockHours + 1;
            if (op_CClock_StoreClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_Stored_nGameClockMinutes = pre.ms_nGameClockMinutes + 1;
            if (op_CClock_StoreClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_Stored_nGameClockSeconds = pre.ms_nGameClockSeconds + 1;
            if (op_CClock_StoreClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_bClockHasBeenStored = false;
            if (op_CClock_StoreClock(pre, badPost)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        pre.ms_Stored_nGameClockHours = 7; pre.ms_nGameClockHours = 0;
        pre.ms_Stored_nGameClockMinutes = 7; pre.ms_nGameClockMinutes = 0;
        pre.ms_Stored_nGameClockSeconds = 7; pre.ms_nGameClockSeconds = 0;
        const auto post = apply_CClock_RestoreClock(pre);
        if (!op_CClock_RestoreClock(pre, post)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = pre.ms_Stored_nGameClockHours + 1;
            if (op_CClock_RestoreClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = pre.ms_Stored_nGameClockMinutes + 1;
            if (op_CClock_RestoreClock(pre, badPost)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockSeconds = pre.ms_Stored_nGameClockSeconds + 1;
            if (op_CClock_RestoreClock(pre, badPost)) return false;
        }
    }
    {
        CFire_ZppContract pre{};
        pre.m_bIsOngoing = true;
        pre.m_bIsScriptFire = true;
        pre.m_bPropagationFlag = false;
        pre.m_bAudioSet = false;
        pre.m_vecPos = 0;
        pre.m_nExtinguishTime = 1;
        const auto post = apply_CFire_CFire(pre);
        if (!op_CFire_CFire(pre, post)) return false;
        {
            CFire_ZppContract badPost = post;
            badPost.m_bIsOngoing = true;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
        {
            CFire_ZppContract badPost = post;
            badPost.m_bIsScriptFire = true;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
        {
            CFire_ZppContract badPost = post;
            badPost.m_bPropagationFlag = false;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
        {
            CFire_ZppContract badPost = post;
            badPost.m_bAudioSet = false;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
        {
            CFire_ZppContract badPost = post;
            badPost.m_vecPos = 0;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
        {
            CFire_ZppContract badPost = post;
            badPost.m_nExtinguishTime = 1;
            if (op_CFire_CFire(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_core_zpp_invariants(), "Z++ formal contract invariant failed for core");
static_assert(op_CClock_Initialise(CClock_ZppContract{}, apply_CClock_Initialise(CClock_ZppContract{})), "CClock_Initialise delta contract must hold");
} // namespace zpp_core_spec
#endif // GTANGEL_ZPP_CORE_SPEC_GUARD
