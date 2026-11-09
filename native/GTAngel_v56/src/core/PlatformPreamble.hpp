#pragma once
// GTAngel AGI Gameworld Testing Environment v15
// Pure C++ Win32 — statically linked, no external deps
// Integrates Layla-Server + cog-zero + echo + kys + elizaos.cpp + GTAngel-Cls + uechovr + re-bhvt-edit + mech0 + u9n-main DeepTreeEcho
// mech0 concepts: SimpleLanguageModel, AttentionMechanism, TrainingPipeline
// Composition: /iterative-micro-improvement(/meta-echo-dna</echo-angel[/superhot-gamer-girl]>
//              & /vorticog</triz-cognitive-arena[/ai-env-grp-dyn]>
//              & /echo-introspect[/cogverse-evolution-accelerator])

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <deque>
#include <functional>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winhttp.h>
#include <sstream>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

// ═══ mech0 Integration (ported from ReZorg/mech0) ═══
// ═══ kys Integration (ported from Cosmic-Construction/kys) ═══
// Triad Phase-Locked Phasor Engine — 3 concurrent KSM 12-step cycles
// running 120° (4 steps) apart with U1/U2 Universal Set policy integrators
// over 4 shared interfaces (C1 Electronic, C2 Cellular, C3 Routine, C4 Environment)
// ═══ EchoCog/echo Integration (ported from EchoCog/echo arena-desktop) ═══
// TRIZ Cognitive Arena with KSM evolution cycle, cognitive controller,
// aesthetic field evaluation, and evolution ledger
// ═══ cogpy/cog-zero Integration (ported from cogpy/cog-zero agentzero-core) ═══
// OpenCog AtomSpace-backed cognitive loop with 5 phases:
// Perception → Reasoning → Planning → Action → Reflection
// ═══ l3utterfly/Layla-Server native integration ═══
// Faithful native adaptation of Layla's backend discovery, serial request queue,
// OpenAI-compatible endpoint, bounded message chunks, and explicit failure telemetry.
// ═══ GTAngel v14 — 3D Avatar + Gameworld→TRIZ Grid Mapping ═══
// Implements a 3D avatar model with skeletal pose, an egocentric software
// renderer producing a real framebuffer, and a projection of that framebuffer
// onto the TRIZ hex grid so navigation-learning difficulty can be evaluated
// with the existing cognitive stack (ESN, endocrine, phasor, AgentLoop).
//
// Design goals:
//  - Real 3D math (perspective projection, quaternion-free yaw/pitch camera)
//  - Real framebuffer (z-buffered rasterizer, 64x64 cognitive resolution)
//  - Real mapping (retinotopic bins -> TRIZ cells with contradiction scoring)
//  - Real training signal (tabular Q-learning navigator + difficulty metrics)
#pragma once
#ifndef GTANGEL_ZPP_CORE_SPEC_GUARD
#define GTANGEL_ZPP_CORE_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/core
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/core/*.zpp
// Modules: 54 | Formal Schemas: 2928 | Free Types: 80
// ============================================================================
namespace zpp_core_spec {
static constexpr int kZppModuleCount = 54;
static constexpr int kZppSchemaCount = 2928;
static constexpr int kZppEnumCount   = 80;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pVictim_card <= 1u)
            && (m_nMedicsAttending >= 0)
            && (m_nMedicsPerformingCPR >= 0);
    }
};
// State Schema Contract from zpp/core/AnimViewer.zpp
struct CAnimViewer_ZppContract {
    int32_t animTxdSlot{0}; // Z++: ℤ
    uint8_t pTarget_card{0u}; // Z++: Ref[CEntity]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (pTarget_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Cam.zpp
struct CamModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6585}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6585);
    }
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
    bool LookingLeft{false}; // Z++: 𝔹
    bool LookingRight{false}; // Z++: 𝔹
    bool ResetStatics{false}; // Z++: 𝔹
    bool Rotating{false}; // Z++: 𝔹
    int32_t Mode{0}; // Z++: ℤ
    int64_t m_uiFinishTime{0}; // Z++: ℕ
    int32_t m_iDoCollisionChecksOnFrameNum{0}; // Z++: ℤ
    int32_t m_iDoCollisionCheckEveryNumOfFrames{0}; // Z++: ℤ
    int32_t m_iFrameNumWereAt{0}; // Z++: ℤ
    int32_t m_iRunningVectorArrayPos{0}; // Z++: ℤ
    int32_t m_iRunningVectorCounter{0}; // Z++: ℤ
    int32_t DirectionWasLooking{0}; // Z++: ℤ
    float f_max_role_angle{0.0f}; // Z++: ℝ
    float f_Roll{0.0f}; // Z++: ℝ
    float f_rollSpeed{0.0f}; // Z++: ℝ
    float m_fSyphonModeTargetZOffSet{0.0f}; // Z++: ℝ
    float m_fRoadOffSet{0.0f}; // Z++: ℝ
    float m_fAmountFractionObscured{0.0f}; // Z++: ℝ
    float m_fAlphaSpeedOverOneFrame{0.0f}; // Z++: ℝ
    float m_fBetaSpeedOverOneFrame{0.0f}; // Z++: ℝ
    float m_fBufferedTargetBeta{0.0f}; // Z++: ℝ
    float m_fBufferedTargetOrientation{0.0f}; // Z++: ℝ
    float m_fBufferedTargetOrientationSpeed{0.0f}; // Z++: ℝ
    float m_fCamBufferedHeight{0.0f}; // Z++: ℝ
    float m_fCamBufferedHeightSpeed{0.0f}; // Z++: ℝ
    float m_fCloseInPedHeightOffset{0.0f}; // Z++: ℝ
    float m_fCloseInPedHeightOffsetSpeed{0.0f}; // Z++: ℝ
    float m_fCloseInCarHeightOffset{0.0f}; // Z++: ℝ
    float m_fCloseInCarHeightOffsetSpeed{0.0f}; // Z++: ℝ
    float m_fDimensionOfHighestNearCar{0.0f}; // Z++: ℝ
    float m_fDistanceBeforeChanges{0.0f}; // Z++: ℝ
    float m_fFovSpeedOverOneFrame{0.0f}; // Z++: ℝ
    float m_fMinDistAwayFromCamWhenInterPolating{0.0f}; // Z++: ℝ
    float m_fPedBetweenCameraHeightOffset{0.0f}; // Z++: ℝ
    float m_fPlayerInFrontSyphonAngleOffSet{0.0f}; // Z++: ℝ
    float m_fRadiusForDead{0.0f}; // Z++: ℝ
    float m_fRealGroundDist{0.0f}; // Z++: ℝ
    float m_fTargetBeta{0.0f}; // Z++: ℝ
    float m_fTimeElapsedFloat{0.0f}; // Z++: ℝ
    float m_fTransitionBeta{0.0f}; // Z++: ℝ
    float m_fTrueBeta{0.0f}; // Z++: ℝ
    float m_fTrueAlpha{0.0f}; // Z++: ℝ
    float m_fInitialPlayerOrientation{0.0f}; // Z++: ℝ
    float Alpha{0.0f}; // Z++: ℝ
    float AlphaSpeed{0.0f}; // Z++: ℝ
    float FOV{0.0f}; // Z++: ℝ
    float FOVSpeed{0.0f}; // Z++: ℝ
    float Beta{0.0f}; // Z++: ℝ
    float BetaSpeed{0.0f}; // Z++: ℝ
    float Distance{0.0f}; // Z++: ℝ
    float DistanceSpeed{0.0f}; // Z++: ℝ
    float CA_MIN_DISTANCE{0.0f}; // Z++: ℝ
    float CA_MAX_DISTANCE{0.0f}; // Z++: ℝ
    float SpeedVar{0.0f}; // Z++: ℝ
    ZppVec3 m_cvecSourceSpeedOverOneFrame{}; // Z++: CVector
    ZppVec3 m_cvecTargetSpeedOverOneFrame{}; // Z++: CVector
    ZppVec3 m_cvecUpOverOneFrame{}; // Z++: CVector
    ZppVec3 m_cvecTargetCoorsForFudgeInter{}; // Z++: CVector
    ZppVec3 m_cvecCamFixedModeVector{}; // Z++: CVector
    ZppVec3 m_cvecCamFixedModeSource{}; // Z++: CVector
    ZppVec3 m_cvecCamFixedModeUpOffSet{}; // Z++: CVector
    ZppVec3 m_vecLastAboveWaterCamPosition{}; // Z++: CVector
    ZppVec3 m_vecBufferedPlayerBodyOffset{}; // Z++: CVector
    ZppVec3 Front{}; // Z++: CVector
    ZppVec3 Source{}; // Z++: CVector
    ZppVec3 SourceBeforeLookBehind{}; // Z++: CVector
    ZppVec3 Up{}; // Z++: CVector
    std::array<ZppVec3, 2> m_arrPreviousVectors{}; // Z++: seq CVector  -- bounded array [NUMBER_OF_VECTORS_FOR_AVERAGE]
    size_t m_arrPreviousVectors_len{2u}; // Z++ cardinality #m_arrPreviousVectors
    uint8_t CamTargetEntity_card{0u}; // Z++: Ref[CEntity]
    float m_fCameraDistance{0.0f}; // Z++: ℝ
    float m_fIdealAlpha{0.0f}; // Z++: ℝ
    float m_fPlayerVelocity{0.0f}; // Z++: ℝ
    uint8_t m_pLastCarEntered_card{0u}; // Z++: Ref[CAutomobile]
    uint8_t m_pLastPedLookedAt_card{0u}; // Z++: Ref[CPed]
    bool m_bFirstPersonRunAboutActive{false}; // Z++: 𝔹
    float m_fTilt{0.0f}; // Z++: ℝ
    float m_fTiltSpeed{0.0f}; // Z++: ℝ
    float m_fTargetZoomGroundOne{0.0f}; // Z++: ℝ
    float m_fTargetZoomGroundTwo{0.0f}; // Z++: ℝ
    float m_fTargetZoomGroundThree{0.0f}; // Z++: ℝ
    float m_fTargetZoomOneZExtra{0.0f}; // Z++: ℝ
    float m_fTargetZoomTwoZExtra{0.0f}; // Z++: ℝ
    float m_fTargetZoomThreeZExtra{0.0f}; // Z++: ℝ
    float m_fTargetZoomZCloseIn{0.0f}; // Z++: ℝ
    float m_fMinRealGroundDist{0.0f}; // Z++: ℝ
    float m_fTargetCloseInDist{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_arrPreviousVectors_len == 2u && m_arrPreviousVectors.size() == 2u)
            && (m_uiFinishTime >= 0)
            && (CamTargetEntity_card <= 1u)
            && (m_pLastCarEntered_card <= 1u)
            && (m_pLastPedLookedAt_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CCamPathSplines_ZppContract {
    uint8_t m_arr_PathData_card{0u}; // Z++: ℙ ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_arr_PathData_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CTrainCamNode_ZppContract {
    ZppVec3 m_cvecCamPosition{}; // Z++: CVector
    ZppVec3 m_cvecPointToLookAt{}; // Z++: CVector
    ZppVec3 m_cvecMinPointInRange{}; // Z++: CVector
    ZppVec3 m_cvecMaxPointInRange{}; // Z++: CVector
    float m_fDesiredFOV{0.0f}; // Z++: ℝ
    float m_fNearClip{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Camera.zpp
struct CCamera_ZppContract {
    int32_t m_matrix{0}; // Z++: CMatrix
    bool m_bAboveGroundTrainNodesLoaded{false}; // Z++: 𝔹
    bool m_bBelowGroundTrainNodesLoaded{false}; // Z++: 𝔹
    bool m_bCamDirectlyBehind{false}; // Z++: 𝔹
    bool m_bCamDirectlyInFront{false}; // Z++: 𝔹
    bool m_bCameraJustRestored{false}; // Z++: 𝔹
    bool m_bcutsceneFinished{false}; // Z++: 𝔹
    bool m_bCullZoneChecksOn{false}; // Z++: 𝔹
    bool m_bFirstPersonBeingUsed{false}; // Z++: 𝔹
    bool m_bJustJumpedOutOf1stPersonBecauseOfTarget{false}; // Z++: 𝔹
    bool m_bIdleOn{false}; // Z++: 𝔹
    bool m_bInATunnelAndABigVehicle{false}; // Z++: 𝔹
    bool m_bInitialNodeFound{false}; // Z++: 𝔹
    bool m_bInitialNoNodeStaticsSet{false}; // Z++: 𝔹
    bool m_bIgnoreFadingStuffForMusic{false}; // Z++: 𝔹
    bool m_bPlayerIsInGarage{false}; // Z++: 𝔹
    bool m_bPlayerWasOnBike{false}; // Z++: 𝔹
    bool m_bJustCameOutOfGarage{false}; // Z++: 𝔹
    bool m_bJustInitalised{false}; // Z++: 𝔹
    bool m_bJust_Switched{false}; // Z++: 𝔹
    bool m_bLookingAtPlayer{false}; // Z++: 𝔹
    bool m_bLookingAtVector{false}; // Z++: 𝔹
    bool m_bMoveCamToAvoidGeom{false}; // Z++: 𝔹
    bool m_bObbeCinematicPedCamOn{false}; // Z++: 𝔹
    bool m_bObbeCinematicCarCamOn{false}; // Z++: 𝔹
    bool m_bRestoreByJumpCut{false}; // Z++: 𝔹
    bool m_bUseNearClipScript{false}; // Z++: 𝔹
    bool m_bStartInterScript{false}; // Z++: 𝔹
    bool m_bStartingSpline{false}; // Z++: 𝔹
    bool m_bTargetJustBeenOnTrain{false}; // Z++: 𝔹
    bool m_bTargetJustCameOffTrain{false}; // Z++: 𝔹
    bool m_bUseSpecialFovTrain{false}; // Z++: 𝔹
    bool m_bUseTransitionBeta{false}; // Z++: 𝔹
    bool m_bUseScriptZoomValuePed{false}; // Z++: 𝔹
    bool m_bUseScriptZoomValueCar{false}; // Z++: 𝔹
    bool m_bWaitForInterpolToFinish{false}; // Z++: 𝔹
    bool m_bItsOkToLookJustAtThePlayer{false}; // Z++: 𝔹
    bool m_bWantsToSwitchWidescreenOff{false}; // Z++: 𝔹
    bool m_WideScreenOn{false}; // Z++: 𝔹
    bool m_1rstPersonRunCloseToAWall{false}; // Z++: 𝔹
    bool m_bHeadBob{false}; // Z++: 𝔹
    bool m_bVehicleSuspenHigh{false}; // Z++: 𝔹
    bool m_bEnable1rstPersonCamCntrlsScript{false}; // Z++: 𝔹
    bool m_bAllow1rstPersonWeaponsCamera{false}; // Z++: 𝔹
    bool m_bFreezePedZoomSwitch{false}; // Z++: 𝔹
    bool m_bFailedCullZoneTestPreviously{false}; // Z++: 𝔹
    bool m_FadeTargetIsSplashScreen{false}; // Z++: 𝔹
    bool WorldViewerBeingUsed{false}; // Z++: 𝔹
    int64_t ActiveCam{0}; // Z++: ℕ
    int64_t m_uiCamShakeStart{0}; // Z++: ℕ
    int64_t m_uiFirstPersonCamLastInputTime{0}; // Z++: ℕ
    int64_t m_uiLongestTimeInMill{0}; // Z++: ℕ
    int64_t m_uiNumberOfTrainCamNodes{0}; // Z++: ℕ
    int64_t m_uiTransitionJUSTStarted{0}; // Z++: ℕ
    int64_t m_uiTransitionState{0}; // Z++: ℕ
    int64_t m_uiTimeLastChange{0}; // Z++: ℕ
    int64_t m_uiTimeWeLeftIdle_StillNoInput{0}; // Z++: ℕ
    int64_t m_uiTimeWeEnteredIdle{0}; // Z++: ℕ
    int64_t m_uiTimeTransitionStart{0}; // Z++: ℕ
    int64_t m_uiTransitionDuration{0}; // Z++: ℕ
    int64_t m_uiTransitionDurationTargetCoors{0}; // Z++: ℕ
    int32_t m_BlurBlue{0}; // Z++: ℤ
    int32_t m_BlurGreen{0}; // Z++: ℤ
    int32_t m_BlurRed{0}; // Z++: ℤ
    int32_t m_BlurType{0}; // Z++: ℤ
    int32_t m_iWorkOutSpeedThisNumFrames{0}; // Z++: ℤ
    int32_t m_iNumFramesSoFar{0}; // Z++: ℤ
    int32_t m_iCurrentTrainCamNode{0}; // Z++: ℤ
    int32_t m_motionBlur{0}; // Z++: ℤ
    int32_t m_imotionBlurAddAlpha{0}; // Z++: ℤ
    int32_t m_iCheckCullZoneThisNumFrames{0}; // Z++: ℤ
    int32_t m_iZoneCullFrameNumWereAt{0}; // Z++: ℤ
    int32_t WhoIsInControlOfTheCamera{0}; // Z++: ℤ
    float CamFrontXNorm{0.0f}; // Z++: ℝ
    float CamFrontYNorm{0.0f}; // Z++: ℝ
    float CarZoomValueSmooth{0.0f}; // Z++: ℝ
    float DistanceToWater{0.0f}; // Z++: ℝ
    float FOVDuringInter{0.0f}; // Z++: ℝ
    float LODDistMultiplier{0.0f}; // Z++: ℝ
    float GenerationDistMultiplier{0.0f}; // Z++: ℝ
    float m_fAlphaSpeedAtStartInter{0.0f}; // Z++: ℝ
    float m_fAlphaWhenInterPol{0.0f}; // Z++: ℝ
    float m_fAlphaDuringInterPol{0.0f}; // Z++: ℝ
    float m_fBetaDuringInterPol{0.0f}; // Z++: ℝ
    float m_fBetaSpeedAtStartInter{0.0f}; // Z++: ℝ
    float m_fBetaWhenInterPol{0.0f}; // Z++: ℝ
    float m_fFOVWhenInterPol{0.0f}; // Z++: ℝ
    float m_fFOVSpeedAtStartInter{0.0f}; // Z++: ℝ
    float m_fStartingBetaForInterPol{0.0f}; // Z++: ℝ
    float m_fStartingAlphaForInterPol{0.0f}; // Z++: ℝ
    float m_PedOrientForBehindOrInFront{0.0f}; // Z++: ℝ
    float m_CameraAverageSpeed{0.0f}; // Z++: ℝ
    float m_CameraSpeedSoFar{0.0f}; // Z++: ℝ
    float m_fCamShakeForce{0.0f}; // Z++: ℝ
    float m_fCarZoomValueScript{0.0f}; // Z++: ℝ
    float m_fFovForTrain{0.0f}; // Z++: ℝ
    float m_fFOV_Wide_Screen{0.0f}; // Z++: ℝ
    float m_fNearClipScript{0.0f}; // Z++: ℝ
    float m_fOldBetaDiff{0.0f}; // Z++: ℝ
    float m_fPedZoomValue{0.0f}; // Z++: ℝ
    float m_fPedZoomValueScript{0.0f}; // Z++: ℝ
    float m_fPedZoomValueSmooth{0.0f}; // Z++: ℝ
    float m_fPositionAlongSpline{0.0f}; // Z++: ℝ
    float m_ScreenReductionPercentage{0.0f}; // Z++: ℝ
    float m_ScreenReductionSpeed{0.0f}; // Z++: ℝ
    float m_AlphaForPlayerAnim1rstPerson{0.0f}; // Z++: ℝ
    float Orientation{0.0f}; // Z++: ℝ
    float SoundDistUp{0.0f}; // Z++: ℝ
    float SoundDistUpAsRead{0.0f}; // Z++: ℝ
    float SoundDistUpAsReadOld{0.0f}; // Z++: ℝ
    float m_fAvoidTheGeometryProbsTimer{0.0f}; // Z++: ℝ
    int32_t m_nAvoidTheGeometryProbsDirn{0}; // Z++: ℤ
    float m_fWideScreenReductionAmount{0.0f}; // Z++: ℝ
    float m_fStartingFOVForInterPol{0.0f}; // Z++: ℝ
    float m_fMouseAccelHorzntl{0.0f}; // Z++: ℝ
    float m_fMouseAccelVertical{0.0f}; // Z++: ℝ
    float m_f3rdPersonCHairMultX{0.0f}; // Z++: ℝ
    float m_f3rdPersonCHairMultY{0.0f}; // Z++: ℝ
    std::array<int32_t, 3> Cams{}; // Z++: seq CCam  -- bounded array [3]
    size_t Cams_len{3u}; // Z++ cardinality #Cams
    uint8_t pToGarageWeAreIn_card{0u}; // Z++: Ref[CGarage]
    uint8_t pToGarageWeAreInForHackAvoidFirstPerson_card{0u}; // Z++: Ref[CGarage]
    int32_t m_PlayerMode{0}; // Z++: CQueuedMode
    int32_t PlayerWeaponMode{0}; // Z++: CQueuedMode
    ZppVec3 m_PreviousCameraPosition{}; // Z++: CVector
    ZppVec3 m_RealPreviousCameraPosition{}; // Z++: CVector
    ZppVec3 m_cvecAimingTargetCoors{}; // Z++: CVector
    ZppVec3 m_vecFixedModeVector{}; // Z++: CVector
    ZppVec3 m_vecFixedModeSource{}; // Z++: CVector
    ZppVec3 m_vecFixedModeUpOffSet{}; // Z++: CVector
    ZppVec3 m_vecCutSceneOffset{}; // Z++: CVector
    ZppVec3 m_cvecStartingSourceForInterPol{}; // Z++: CVector
    ZppVec3 m_cvecStartingTargetForInterPol{}; // Z++: CVector
    ZppVec3 m_cvecStartingUpForInterPol{}; // Z++: CVector
    ZppVec3 m_cvecSourceSpeedAtStartInter{}; // Z++: CVector
    ZppVec3 m_cvecTargetSpeedAtStartInter{}; // Z++: CVector
    ZppVec3 m_cvecUpSpeedAtStartInter{}; // Z++: CVector
    ZppVec3 m_vecSourceWhenInterPol{}; // Z++: CVector
    ZppVec3 m_vecTargetWhenInterPol{}; // Z++: CVector
    ZppVec3 m_vecUpWhenInterPol{}; // Z++: CVector
    ZppVec3 m_vecClearGeometryVec{}; // Z++: CVector
    ZppVec3 m_vecGameCamPos{}; // Z++: CVector
    ZppVec3 SourceDuringInter{}; // Z++: CVector
    ZppVec3 TargetDuringInter{}; // Z++: CVector
    ZppVec3 UpDuringInter{}; // Z++: CVector
    uint8_t m_pRwCamera_card{0u}; // Z++: Ref[RwCamera]
    uint8_t pTargetEntity_card{0u}; // Z++: Ref[CEntity]
    std::array<int32_t, 4> m_arrPathArray{}; // Z++: seq CCamPathSplines  -- bounded array [MAX_NUM_OF_SPLINETYPES]
    size_t m_arrPathArray_len{4u}; // Z++ cardinality #m_arrPathArray
    bool m_bGarageFixedCamPositionSet{false}; // Z++: 𝔹
    bool m_vecDoingSpecialInterPolation{false}; // Z++: 𝔹
    bool m_bScriptParametersSetForInterPol{false}; // Z++: 𝔹
    bool m_bFading{false}; // Z++: 𝔹
    bool m_bMusicFading{false}; // Z++: 𝔹
    int32_t m_viewMatrix{0}; // Z++: CMatrix
    std::array<ZppVec3, 4> m_vecFrustumNormals{}; // Z++: seq CVector  -- bounded array [4]
    size_t m_vecFrustumNormals_len{4u}; // Z++ cardinality #m_vecFrustumNormals
    ZppVec3 m_vecOldSourceForInter{}; // Z++: CVector
    ZppVec3 m_vecOldFrontForInter{}; // Z++: CVector
    ZppVec3 m_vecOldUpForInter{}; // Z++: CVector
    float m_vecOldFOVForInter{0.0f}; // Z++: ℝ
    float m_fFLOATingFade{0.0f}; // Z++: ℝ
    float m_fFLOATingFadeMusic{0.0f}; // Z++: ℝ
    float m_fTimeToFadeOut{0.0f}; // Z++: ℝ
    float m_fTimeToFadeMusic{0.0f}; // Z++: ℝ
    float m_fFractionInterToStopMoving{0.0f}; // Z++: ℝ
    float m_fFractionInterToStopCatchUp{0.0f}; // Z++: ℝ
    float m_fFractionInterToStopMovingTarget{0.0f}; // Z++: ℝ
    float m_fFractionInterToStopCatchUpTarget{0.0f}; // Z++: ℝ
    float m_fGaitSwayBuffer{0.0f}; // Z++: ℝ
    float m_fScriptPercentageInterToStopMoving{0.0f}; // Z++: ℝ
    float m_fScriptPercentageInterToCatchUp{0.0f}; // Z++: ℝ
    int64_t m_fScriptTimeForInterPolation{0}; // Z++: ℕ
    int32_t m_iFadingDirection{0}; // Z++: ℤ
    int32_t m_iModeObbeCamIsInForCar{0}; // Z++: ℤ
    int32_t m_iModeToGoTo{0}; // Z++: ℤ
    int32_t m_iMusicFadingDirection{0}; // Z++: ℤ
    int32_t m_iTypeOfSwitch{0}; // Z++: ℤ
    int64_t m_uiFadeTimeStarted{0}; // Z++: ℕ
    int64_t m_uiFadeTimeStartedMusic{0}; // Z++: ℕ
    bool m_bUseMouse3rdPerson{false}; // Z++: 𝔹
    bool m_bUnknown{false}; // Z++: 𝔹
    int64_t unknown{0}; // Z++: ℕ
    float SoundDistRightAsRead{0.0f}; // Z++: ℝ
    float SoundDistRightAsReadOld{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (Cams_len == 3u && Cams.size() == 3u)
            && (m_arrPathArray_len == 4u && m_arrPathArray.size() == 4u)
            && (m_vecFrustumNormals_len == 4u && m_vecFrustumNormals.size() == 4u)
            && (instance_addr >= 0)
            && (ActiveCam >= 0)
            && (m_uiCamShakeStart >= 0)
            && (m_uiFirstPersonCamLastInputTime >= 0)
            && (m_uiLongestTimeInMill >= 0)
            && (m_uiNumberOfTrainCamNodes >= 0)
            && (m_uiTransitionJUSTStarted >= 0)
            && (m_uiTransitionState >= 0)
            && (m_uiTimeLastChange >= 0)
            && (m_uiTimeWeLeftIdle_StillNoInput >= 0)
            && (m_uiTimeWeEnteredIdle >= 0)
            && (m_uiTimeTransitionStart >= 0)
            && (m_uiTransitionDuration >= 0)
            && (m_uiTransitionDurationTargetCoors >= 0)
            && (pToGarageWeAreIn_card <= 1u)
            && (pToGarageWeAreInForHackAvoidFirstPerson_card <= 1u);
    }
};
// State Schema Contract from zpp/core/CdStream.zpp
struct CdReadInfo_ZppContract {
    int64_t nSectorOffset{0}; // Z++: ℕ
    int64_t nSectorsToRead{0}; // Z++: ℕ
    int64_t pBuffer{0}; // Z++: ADDRESS
    int32_t field_C{0}; // Z++: seq CHAR
    bool bLocked{false}; // Z++: 𝔹
    bool bReading{false}; // Z++: 𝔹
    int32_t nStatus{0}; // Z++: ℤ
    int64_t pDoneSemaphore{0}; // Z++: HANDLE
    int64_t hFile{0}; // Z++: HANDLE
    int32_t Overlapped{0}; // Z++: OVERLAPPED
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Crime.zpp
struct CCrimeBeingQd_ZppContract {
    int32_t m_nType{0}; // Z++: eCrimeType
    int32_t m_nId{0}; // Z++: ℤ
    int64_t m_nTime{0}; // Z++: ℕ
    ZppVec3 m_vecPosn{}; // Z++: CVector
    bool m_bReported{false}; // Z++: 𝔹
    bool m_bPoliceDoesntCare{false}; // Z++: 𝔹
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_nTime >= 0);
    }
};
// State Schema Contract from zpp/core/Debug.zpp
struct Line_ZppContract {
    int32_t p2{0}; // Z++: CVector_p1
    int64_t c2{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (c2 >= 0);
    }
};
// State Schema Contract from zpp/core/EventList.zpp
struct CEvent_ZppContract {
    int32_t type{0}; // Z++: eEventType
    int32_t entityType{0}; // Z++: eEventEntity
    int32_t entityRef{0}; // Z++: ℤ
    uint8_t criminal_card{0u}; // Z++: Ref[CPed]
    ZppVec3 posn{}; // Z++: CVector
    int64_t timeout{0}; // Z++: ℕ
    int32_t state{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (criminal_card <= 1u)
            && (timeout >= 0);
    }
};
// State Schema Contract from zpp/core/EventList.zpp
struct CEventList_ZppContract {
    int32_t ms_nFirstFreeSlotIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FileLoader.zpp
struct ColHeader_ZppContract {
    int64_t ident{0}; // Z++: ℕ
    int64_t size{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ident >= 0)
            && (size >= 0);
    }
};
// State Schema Contract from zpp/core/FileMgr.zpp
struct myFILE_ZppContract {
    bool isText{false}; // Z++: 𝔹
    uint8_t file_card{0u}; // Z++: Ref[FILE]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (file_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Fire.zpp
struct CFire_ZppContract {
    bool m_bIsOngoing{false}; // Z++: 𝔹
    bool m_bIsScriptFire{false}; // Z++: 𝔹
    bool m_bPropagationFlag{false}; // Z++: 𝔹
    bool m_bAudioSet{false}; // Z++: 𝔹
    ZppVec3 m_vecPos{}; // Z++: CVector
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
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    int64_t pTrigger{0}; // Z++: ADDRESS
    int32_t field_C{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableText_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    int32_t m_text{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableTextTwoLines_ZppContract {
    int32_t m_line1{0}; // Z++: CPlaceableText
    int32_t m_line2{0}; // Z++: CPlaceableText
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CSelectable_ZppContract {
    bool m_bSelected{false}; // Z++: 𝔹
    int32_t m_selectedColor{0}; // Z++: CRGBA
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableShText_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    int32_t m_text{0}; // Z++: seq CHAR
    bool m_bRightJustify{false}; // Z++: 𝔹
    bool m_bDropShadow{false}; // Z++: 𝔹
    int32_t m_shadowColor{0}; // Z++: CRGBA
    int32_t m_shadowOffset{0}; // Z++: CVector2D
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableShTextTwoLines_ZppContract {
    int32_t m_line1{0}; // Z++: CPlaceableText
    int32_t m_line2{0}; // Z++: CPlaceableText
    bool m_bRightJustify{false}; // Z++: 𝔹
    bool m_bDropShadow{false}; // Z++: 𝔹
    int32_t m_shadowColor{0}; // Z++: CRGBA
    int32_t m_shadowOffset{0}; // Z++: CVector2D
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableShOption_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    int32_t m_text{0}; // Z++: seq CHAR
    bool m_bRightJustify{false}; // Z++: 𝔹
    bool m_bDropShadow{false}; // Z++: 𝔹
    int32_t m_shadowColor{0}; // Z++: CRGBA
    int32_t m_shadowOffset{0}; // Z++: CVector2D
    bool m_bSelected{false}; // Z++: 𝔹
    int32_t m_selectedColor{0}; // Z++: CRGBA
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableShOptionTwoLines_ZppContract {
    int32_t m_line1{0}; // Z++: CPlaceableText
    int32_t m_line2{0}; // Z++: CPlaceableText
    bool m_bRightJustify{false}; // Z++: 𝔹
    bool m_bDropShadow{false}; // Z++: 𝔹
    int32_t m_shadowColor{0}; // Z++: CRGBA
    int32_t m_shadowOffset{0}; // Z++: CVector2D
    bool m_bSelected{false}; // Z++: 𝔹
    int32_t m_selectedColor{0}; // Z++: CRGBA
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableSprite_ZppContract {
    uint8_t m_pSprite_card{0u}; // Z++: Ref[CSprite2d]
    int32_t m_position{0}; // Z++: CVector2D
    int32_t m_size{0}; // Z++: CVector2D
    int32_t m_color{0}; // Z++: CRGBA
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pSprite_card <= 1u);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CPlaceableShSprite_ZppContract {
    int32_t m_sprite{0}; // Z++: CPlaceableSprite
    int32_t m_shadow{0}; // Z++: CPlaceableSprite
    bool m_bDropShadow{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuBase_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuDummy_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    bool m_bActive{false}; // Z++: 𝔹
    int32_t false_field{0}; // Z++: return
    int32_t true_field{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuPictureAndText_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numSprites{0}; // Z++: ℤ
    std::array<int32_t, 5> m_sprites{}; // Z++: seq CPlaceableShSprite  -- bounded array [5]
    size_t m_sprites_len{5u}; // Z++ cardinality #m_sprites
    int32_t m_numTexts{0}; // Z++: ℤ
    std::array<int32_t, 20> m_texts{}; // Z++: seq CPlaceableShText  -- bounded array [20]
    size_t m_texts_len{20u}; // Z++ cardinality #m_texts
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    float m_wrapX{0.0f}; // Z++: ℝ
    float m_oldWrapx{0.0f}; // Z++: ℝ
    bool m_bWrap{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_sprites_len == 5u && m_sprites.size() == 5u)
            && (m_texts_len == 20u && m_texts.size() == 20u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoice_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoiceTriggered_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_triggers{}; // Z++: seq Trigger  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_triggers_len{16u}; // Z++ cardinality #m_triggers
    int32_t m_defaultCancel{0}; // Z++: Trigger
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_triggers_len == 16u && m_triggers.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoiceTriggeredAlways_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_triggers{}; // Z++: seq Trigger  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_triggers_len{16u}; // Z++ cardinality #m_triggers
    int32_t m_defaultCancel{0}; // Z++: Trigger
    int32_t m_alwaysNormalTrigger{0}; // Z++: Trigger
    int32_t m_alwaysHighlightTrigger{0}; // Z++: Trigger
    int32_t m_alwaysTrigger{0}; // Z++: Trigger
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_triggers_len == 16u && m_triggers.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoicePictured_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_sprites{}; // Z++: seq CPlaceableSprite  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_sprites_len{16u}; // Z++ cardinality #m_sprites
    std::array<bool, 16> m_bHasSprite{}; // Z++: seq 𝔹  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_bHasSprite_len{16u}; // Z++ cardinality #m_bHasSprite
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_sprites_len == 16u && m_sprites.size() == 16u)
            && (m_bHasSprite_len == 16u && m_bHasSprite.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoicePicturedTriggered_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_sprites{}; // Z++: seq CPlaceableSprite  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_sprites_len{16u}; // Z++ cardinality #m_sprites
    std::array<bool, 16> m_bHasSprite{}; // Z++: seq 𝔹  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_bHasSprite_len{16u}; // Z++ cardinality #m_bHasSprite
    std::array<int32_t, 16> m_triggers{}; // Z++: seq Trigger  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_triggers_len{16u}; // Z++ cardinality #m_triggers
    int32_t m_defaultCancel{0}; // Z++: Trigger
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_sprites_len == 16u && m_sprites.size() == 16u)
            && (m_bHasSprite_len == 16u && m_bHasSprite.size() == 16u)
            && (m_triggers_len == 16u && m_triggers.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoicePicturedTriggeredAnyMove_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOption  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_sprites{}; // Z++: seq CPlaceableSprite  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_sprites_len{16u}; // Z++ cardinality #m_sprites
    std::array<bool, 16> m_bHasSprite{}; // Z++: seq 𝔹  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_bHasSprite_len{16u}; // Z++ cardinality #m_bHasSprite
    std::array<int32_t, 16> m_triggers{}; // Z++: seq Trigger  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_triggers_len{16u}; // Z++ cardinality #m_triggers
    int32_t m_defaultCancel{0}; // Z++: Trigger
    std::array<int32_t, 16> m_moveTab{}; // Z++: seq FEC_MOVETAB  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_moveTab_len{16u}; // Z++ cardinality #m_moveTab
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_sprites_len == 16u && m_sprites.size() == 16u)
            && (m_bHasSprite_len == 16u && m_bHasSprite.size() == 16u)
            && (m_triggers_len == 16u && m_triggers.size() == 16u)
            && (m_moveTab_len == 16u && m_moveTab.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoiceTwoLines_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOptionTwoLines  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuMultiChoiceTwoLinesTriggered_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_numOptions{0}; // Z++: ℤ
    int32_t m_title{0}; // Z++: CPlaceableShText
    std::array<int32_t, 16> m_options{}; // Z++: seq CPlaceableShOptionTwoLines  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_options_len{16u}; // Z++ cardinality #m_options
    int32_t m_cursor{0}; // Z++: ℤ
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_textScale{0}; // Z++: CVector2D
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t true_field{0}; // Z++: return
    std::array<int32_t, 16> m_triggers{}; // Z++: seq Trigger  -- bounded array [NUM_MULTICHOICE_OPTIONS]
    size_t m_triggers_len{16u}; // Z++ cardinality #m_triggers
    int32_t m_defaultCancel{0}; // Z++: Trigger
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 16u && m_options.size() == 16u)
            && (m_triggers_len == 16u && m_triggers.size() == 16u)
            && (instance_addr >= 0)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuOnOff_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_title{0}; // Z++: CPlaceableShOption
    std::array<int32_t, 2> m_options{}; // Z++: seq CPlaceableShText  -- bounded array [2]
    size_t m_options_len{2u}; // Z++ cardinality #m_options
    bool m_bActive{false}; // Z++: 𝔹
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t m_textScale{0}; // Z++: CVector2D
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_type{0}; // Z++: ℤ
    int32_t false_field{0}; // Z++: return
    int32_t true_field{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 2u && m_options.size() == 2u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuOnOffTriggered_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    int32_t m_title{0}; // Z++: CPlaceableShOption
    std::array<int32_t, 2> m_options{}; // Z++: seq CPlaceableShText  -- bounded array [2]
    size_t m_options_len{2u}; // Z++ cardinality #m_options
    bool m_bActive{false}; // Z++: 𝔹
    bool m_bSetTextScale{false}; // Z++: 𝔹
    bool m_bSetTitleTextScale{false}; // Z++: 𝔹
    int32_t m_textScale{0}; // Z++: CVector2D
    int32_t m_oldTextScale{0}; // Z++: CVector2D
    int32_t m_type{0}; // Z++: ℤ
    int32_t false_field{0}; // Z++: return
    int32_t true_field{0}; // Z++: return
    int32_t m_trigger{0}; // Z++: Trigger
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_options_len == 2u && m_options.size() == 2u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuLineLister_ZppContract {
    int32_t m_position{0}; // Z++: CVector2D
    bool m_bTwoState{false}; // Z++: 𝔹
    float m_width{0.0f}; // Z++: ℝ
    float m_height{0.0f}; // Z++: ℝ
    int32_t m_numLines{0}; // Z++: ℤ
    std::array<int32_t, 51> m_linesLeft{}; // Z++: seq CPlaceableShText  -- bounded array [NUM_LINELISTER_LINES_TOTAL]
    size_t m_linesLeft_len{51u}; // Z++ cardinality #m_linesLeft
    std::array<int32_t, 51> m_linesRight{}; // Z++: seq CPlaceableShText  -- bounded array [NUM_LINELISTER_LINES_TOTAL]
    size_t m_linesRight_len{51u}; // Z++ cardinality #m_linesRight
    std::array<uint32_t, 51> m_lineAlphas{}; // Z++: seq ℕ  -- bounded array [NUM_LINELISTER_LINES_TOTAL]
    size_t m_lineAlphas_len{51u}; // Z++ cardinality #m_lineAlphas
    std::array<int32_t, 51> m_lineFade{}; // Z++: seq ℤ  -- bounded array [NUM_LINELISTER_LINES_TOTAL]
    size_t m_lineFade_len{51u}; // Z++ cardinality #m_lineFade
    float m_scrollPosition{0.0f}; // Z++: ℝ
    float m_scrollSpeed{0.0f}; // Z++: ℝ
    int32_t field_10E8{0}; // Z++: ℤ
    float m_lineSpacing{0.0f}; // Z++: ℝ
    int32_t true_field{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_linesLeft_len == 51u && m_linesLeft.size() == 51u)
            && (m_linesRight_len == 51u && m_linesRight.size() == 51u)
            && (m_lineAlphas_len == 51u && m_lineAlphas.size() == 51u)
            && (m_lineFade_len == 51u && m_lineFade.size() == 51u)
            && (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuPage_ZppContract {
    std::array<int32_t, 10> m_controls{}; // Z++: seq Ref[CMenuBase]  -- bounded array [NUM_PAGE_WIDGETS]
    size_t m_controls_len{10u}; // Z++ cardinality #m_controls
    int32_t m_numControls{0}; // Z++: ℤ
    uint8_t m_pCurrentControl_card{0u}; // Z++: Ref[CMenuBase]
    int32_t m_cursor{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_controls_len == 10u && m_controls.size() == 10u)
            && (m_pCurrentControl_card <= 1u);
    }
};
// State Schema Contract from zpp/core/FrontEndControls.zpp
struct CMenuPageAnyMove_ZppContract {
    std::array<int32_t, 10> m_controls{}; // Z++: seq Ref[CMenuBase]  -- bounded array [NUM_PAGE_WIDGETS]
    size_t m_controls_len{10u}; // Z++ cardinality #m_controls
    int32_t m_numControls{0}; // Z++: ℤ
    uint8_t m_pCurrentControl_card{0u}; // Z++: Ref[CMenuBase]
    int32_t m_cursor{0}; // Z++: ℤ
    std::array<int32_t, 10> m_moveTab{}; // Z++: seq FEC_MOVETAB  -- bounded array [NUM_PAGE_WIDGETS]
    size_t m_moveTab_len{10u}; // Z++ cardinality #m_moveTab
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_controls_len == 10u && m_controls.size() == 10u)
            && (m_moveTab_len == 10u && m_moveTab.size() == 10u)
            && (m_pCurrentControl_card <= 1u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct CCustomScreenLayout_ZppContract {
    int32_t startX{0}; // Z++: ℤ
    int32_t startY{0}; // Z++: ℤ
    int32_t lineHeight{0}; // Z++: ℤ
    bool showLeftRightHelper{false}; // Z++: 𝔹
    bool noInvasiveBorders{false}; // Z++: 𝔹
    int32_t xMargin{0}; // Z++: ℤ
    int32_t sprite{0}; // Z++: eMenuSprites
    int32_t columnWidth{0}; // Z++: ℤ
    int32_t headerHeight{0}; // Z++: ℤ
    int32_t font{0}; // Z++: ℤ
    int32_t alignment{0}; // Z++: ℤ
    float fontScaleX{0.0f}; // Z++: ℝ
    float fontScaleY{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct CCFO_ZppContract {
    int64_t value{0}; // Z++: ADDRESS
    int32_t saveCat{0}; // Z++: seq CHAR
    int32_t save{0}; // Z++: seq CHAR
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct CCFOSelect_ZppContract {
    int64_t value{0}; // Z++: ADDRESS
    int32_t saveCat{0}; // Z++: seq CHAR
    int32_t save{0}; // Z++: seq CHAR
    int32_t rightTexts{0}; // Z++: seq CHAR
    int32_t numRightTexts{0}; // Z++: ℤ
    bool onlyApplyOnEnter{false}; // Z++: 𝔹
    int32_t displayedValue{0}; // Z++: ℤ
    int32_t lastSavedValue{0}; // Z++: ℤ
    int32_t changeFunc{0}; // Z++: ChangeFunc
    bool disableIfGameLoaded{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct CCFOSlider_ZppContract {
    int64_t value{0}; // Z++: ADDRESS
    int32_t saveCat{0}; // Z++: seq CHAR
    int32_t save{0}; // Z++: seq CHAR
    float changeFunc{0.0f}; // Z++: ℝ
    float min_field{0.0f}; // Z++: ℝ
    float max_field{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct CCFODynamic_ZppContract {
    int64_t value{0}; // Z++: ADDRESS
    int32_t saveCat{0}; // Z++: seq CHAR
    int32_t save{0}; // Z++: seq CHAR
    int32_t drawFunc{0}; // Z++: DrawFunc
    int32_t buttonPressFunc{0}; // Z++: ButtonPressFunc
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Frontend.zpp
struct MenuTrapezoid_ZppContract {
    float topLeft_x{0.0f}; // Z++: ℝ
    float topLeft_y{0.0f}; // Z++: ℝ
    float topRight_x{0.0f}; // Z++: ℝ
    float topRight_y{0.0f}; // Z++: ℝ
    float bottomLeft_x{0.0f}; // Z++: ℝ
    float bottomLeft_y{0.0f}; // Z++: ℝ
    float bottomRight_x{0.0f}; // Z++: ℝ
    float bottomRight_y{0.0f}; // Z++: ℝ
    float old_topRight_x{0.0f}; // Z++: ℝ
    float old_topRight_y{0.0f}; // Z++: ℝ
    float old_topLeft_x{0.0f}; // Z++: ℝ
    float old_topLeft_y{0.0f}; // Z++: ℝ
    float old_bottomLeft_x{0.0f}; // Z++: ℝ
    float old_bottomLeft_y{0.0f}; // Z++: ℝ
    float old_bottomRight_x{0.0f}; // Z++: ℝ
    float old_bottomRight_y{0.0f}; // Z++: ℝ
    float mult_topRight_x{0.0f}; // Z++: ℝ
    float mult_topRight_y{0.0f}; // Z++: ℝ
    float mult_topLeft_x{0.0f}; // Z++: ℝ
    float mult_topLeft_y{0.0f}; // Z++: ℝ
    float mult_bottomLeft_x{0.0f}; // Z++: ℝ
    float mult_bottomLeft_y{0.0f}; // Z++: ℝ
    float mult_bottomRight_x{0.0f}; // Z++: ℝ
    float mult_bottomRight_y{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/FrontendTriggers.zpp
struct FrontendTriggersModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1393}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1393);
    }
};
// State Schema Contract from zpp/core/Game.zpp
struct SkyDataPrefix_ZppContract {
    int64_t pktSize1{0}; // Z++: ℕ
    int64_t data{0}; // Z++: ℕ
    int64_t pktSize2{0}; // Z++: ℕ
    int64_t unused{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (pktSize1 >= 0)
            && (data >= 0)
            && (pktSize2 >= 0)
            && (unused >= 0);
    }
};
// State Schema Contract from zpp/core/Game.zpp
struct DMAGIFUpload_ZppContract {
    int64_t tag1_addr{0}; // Z++: ℕ
    int64_t vif_direct1{0}; // Z++: ℕ
    std::array<uint32_t, 4> giftag{}; // Z++: seq ℕ  -- bounded array [4]
    size_t giftag_len{4u}; // Z++ cardinality #giftag
    std::array<uint32_t, 4> gs_bitbltbuf{}; // Z++: seq ℕ  -- bounded array [4]
    size_t gs_bitbltbuf_len{4u}; // Z++ cardinality #gs_bitbltbuf
    int64_t tag2_addr{0}; // Z++: ℕ
    int64_t vif_direct2{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (giftag_len == 4u && giftag.size() == 4u)
            && (gs_bitbltbuf_len == 4u && gs_bitbltbuf.size() == 4u)
            && (tag1_addr >= 0)
            && (vif_direct1 >= 0)
            && (tag2_addr >= 0)
            && (vif_direct2 >= 0);
    }
};
// State Schema Contract from zpp/core/General.zpp
struct CGeneral_ZppContract {
    int32_t true_field{0}; // Z++: return
    int32_t arc{0}; // Z++: return
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/IniFile.zpp
struct CIniFile_ZppContract {
    float PedNumberMultiplier{0.0f}; // Z++: ℝ
    float CarNumberMultiplier{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/KeyGen.zpp
struct CKeyGen_ZppContract {
    std::array<uint32_t, 256> keyTable{}; // Z++: seq ℕ  -- bounded array [256]
    size_t keyTable_len{256u}; // Z++ cardinality #keyTable
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (keyTable_len == 256u && keyTable.size() == 256u);
    }
};
// State Schema Contract from zpp/core/Leeds.zpp
struct LeedsModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{50}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 50);
    }
};
// State Schema Contract from zpp/core/Lists.zpp
struct CPtrNode_ZppContract {
    int64_t item{0}; // Z++: ADDRESS
    uint8_t prev_card{0u}; // Z++: Ref[CPtrNode]
    uint8_t next_card{0u}; // Z++: Ref[CPtrNode]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (prev_card <= 1u)
            && (next_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Lists.zpp
struct CPtrList_ZppContract {
    uint8_t first_card{0u}; // Z++: Ref[CPtrNode]
    int32_t nil{0}; // Z++: return
    int32_t node{0}; // Z++: return
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (first_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Lists.zpp
struct CEntryInfoNode_ZppContract {
    uint8_t list_card{0u}; // Z++: Ref[CPtrList]
    uint8_t listnode_card{0u}; // Z++: Ref[CPtrNode]
    uint8_t sector_card{0u}; // Z++: Ref[CSector]
    uint8_t prev_card{0u}; // Z++: Ref[CEntryInfoNode]
    uint8_t next_card{0u}; // Z++: Ref[CEntryInfoNode]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (list_card <= 1u)
            && (listnode_card <= 1u)
            && (sector_card <= 1u)
            && (prev_card <= 1u)
            && (next_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Lists.zpp
struct CEntryInfoList_ZppContract {
    uint8_t first_card{0u}; // Z++: Ref[CEntryInfoNode]
    int32_t node{0}; // Z++: return
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (first_card <= 1u);
    }
};
// State Schema Contract from zpp/core/MenuScreens.zpp
struct MenuScreensModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{800}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 800);
    }
};
// State Schema Contract from zpp/core/MenuScreensCustom.zpp
struct MenuScreensCustomModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1457}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1457);
    }
};
// State Schema Contract from zpp/core/Pad.zpp
struct CControllerState_ZppContract {
    int32_t LeftStickY{0}; // Z++: ℤ
    int32_t RightStickY{0}; // Z++: ℤ
    int32_t LeftShoulder2{0}; // Z++: ℤ
    int32_t RightShoulder2{0}; // Z++: ℤ
    int32_t DPadRight{0}; // Z++: ℤ
    int32_t Select{0}; // Z++: ℤ
    int32_t Circle{0}; // Z++: ℤ
    int32_t RightShock{0}; // Z++: ℤ
    int32_t NetworkTalk{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Pad.zpp
struct CMouseControllerState_ZppContract {
    bool LMB{false}; // Z++: 𝔹
    bool RMB{false}; // Z++: 𝔹
    bool MMB{false}; // Z++: 𝔹
    bool WHEELUP{false}; // Z++: 𝔹
    bool WHEELDN{false}; // Z++: 𝔹
    bool MXB1{false}; // Z++: 𝔹
    bool MXB2{false}; // Z++: 𝔹
    int32_t _pad0{0}; // Z++: seq CHAR
    float y{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Pad.zpp
struct CMousePointerStateHelper_ZppContract {
    bool bInvertHorizontally{false}; // Z++: 𝔹
    bool bInvertVertically{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Pad.zpp
struct CKeyboardState_ZppContract {
    std::array<int32_t, 12> F{}; // Z++: seq ℤ  -- bounded array [12]
    size_t F_len{12u}; // Z++ cardinality #F
    std::array<int32_t, 256> VK_KEYS{}; // Z++: seq ℤ  -- bounded array [256]
    size_t VK_KEYS_len{256u}; // Z++ cardinality #VK_KEYS
    int32_t ESC{0}; // Z++: ℤ
    int32_t INS{0}; // Z++: ℤ
    int32_t DEL{0}; // Z++: ℤ
    int32_t HOME{0}; // Z++: ℤ
    int32_t END{0}; // Z++: ℤ
    int32_t PGUP{0}; // Z++: ℤ
    int32_t PGDN{0}; // Z++: ℤ
    int32_t UP{0}; // Z++: ℤ
    int32_t DOWN{0}; // Z++: ℤ
    int32_t LEFT{0}; // Z++: ℤ
    int32_t RIGHT{0}; // Z++: ℤ
    int32_t SCROLLLOCK{0}; // Z++: ℤ
    int32_t PAUSE{0}; // Z++: ℤ
    int32_t NUMLOCK{0}; // Z++: ℤ
    int32_t DIV{0}; // Z++: ℤ
    int32_t MUL{0}; // Z++: ℤ
    int32_t SUB{0}; // Z++: ℤ
    int32_t ADD{0}; // Z++: ℤ
    int32_t ENTER{0}; // Z++: ℤ
    int32_t DECIMAL{0}; // Z++: ℤ
    int32_t NUM1{0}; // Z++: ℤ
    int32_t NUM2{0}; // Z++: ℤ
    int32_t NUM3{0}; // Z++: ℤ
    int32_t NUM4{0}; // Z++: ℤ
    int32_t NUM5{0}; // Z++: ℤ
    int32_t NUM6{0}; // Z++: ℤ
    int32_t NUM7{0}; // Z++: ℤ
    int32_t NUM8{0}; // Z++: ℤ
    int32_t NUM9{0}; // Z++: ℤ
    int32_t NUM0{0}; // Z++: ℤ
    int32_t BACKSP{0}; // Z++: ℤ
    int32_t TAB{0}; // Z++: ℤ
    int32_t CAPSLOCK{0}; // Z++: ℤ
    int32_t EXTENTER{0}; // Z++: ℤ
    int32_t LSHIFT{0}; // Z++: ℤ
    int32_t RSHIFT{0}; // Z++: ℤ
    int32_t SHIFT{0}; // Z++: ℤ
    int32_t LCTRL{0}; // Z++: ℤ
    int32_t RCTRL{0}; // Z++: ℤ
    int32_t LALT{0}; // Z++: ℤ
    int32_t RALT{0}; // Z++: ℤ
    int32_t LWIN{0}; // Z++: ℤ
    int32_t RWIN{0}; // Z++: ℤ
    int32_t APPS{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (F_len == 12u && F.size() == 12u)
            && (VK_KEYS_len == 256u && VK_KEYS.size() == 256u);
    }
};
// State Schema Contract from zpp/core/Placeable.zpp
struct CPlaceable_ZppContract {
    int32_t m_matrix{0}; // Z++: CMatrix
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Pools.zpp
struct CPools_ZppContract {
    uint8_t ms_pPtrNodePool_card{0u}; // Z++: Ref[CCPtrNodePool]
    uint8_t ms_pEntryInfoNodePool_card{0u}; // Z++: Ref[CEntryInfoNodePool]
    uint8_t ms_pPedPool_card{0u}; // Z++: Ref[CPedPool]
    uint8_t ms_pVehiclePool_card{0u}; // Z++: Ref[CVehiclePool]
    uint8_t ms_pBuildingPool_card{0u}; // Z++: Ref[CBuildingPool]
    uint8_t ms_pTreadablePool_card{0u}; // Z++: Ref[CTreadablePool]
    uint8_t ms_pObjectPool_card{0u}; // Z++: Ref[CObjectPool]
    uint8_t ms_pDummyPool_card{0u}; // Z++: Ref[CDummyPool]
    uint8_t ms_pAudioScriptObjectPool_card{0u}; // Z++: Ref[CAudioScriptObjectPool]
    uint8_t ms_pColModelPool_card{0u}; // Z++: Ref[CColModelPool]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_pPtrNodePool_card <= 1u)
            && (ms_pEntryInfoNodePool_card <= 1u)
            && (ms_pPedPool_card <= 1u)
            && (ms_pVehiclePool_card <= 1u)
            && (ms_pBuildingPool_card <= 1u)
            && (ms_pTreadablePool_card <= 1u)
            && (ms_pObjectPool_card <= 1u)
            && (ms_pDummyPool_card <= 1u)
            && (ms_pAudioScriptObjectPool_card <= 1u)
            && (ms_pColModelPool_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Profile.zpp
struct CProfile_ZppContract {
    std::array<float, 7> ms_afStartTime{}; // Z++: seq ℝ  -- bounded array [NUM_PROFILES]
    size_t ms_afStartTime_len{7u}; // Z++ cardinality #ms_afStartTime
    std::array<float, 7> ms_afCumulativeTime{}; // Z++: seq ℝ  -- bounded array [NUM_PROFILES]
    size_t ms_afCumulativeTime_len{7u}; // Z++ cardinality #ms_afCumulativeTime
    std::array<float, 7> ms_afEndTime{}; // Z++: seq ℝ  -- bounded array [NUM_PROFILES]
    size_t ms_afEndTime_len{7u}; // Z++ cardinality #ms_afEndTime
    std::array<float, 7> ms_afMaxEndTime{}; // Z++: seq ℝ  -- bounded array [NUM_PROFILES]
    size_t ms_afMaxEndTime_len{7u}; // Z++ cardinality #ms_afMaxEndTime
    std::array<float, 7> ms_afMaxCumulativeTime{}; // Z++: seq ℝ  -- bounded array [NUM_PROFILES]
    size_t ms_afMaxCumulativeTime_len{7u}; // Z++ cardinality #ms_afMaxCumulativeTime
    int32_t ms_pProfileString{0}; // Z++: seq CHAR
    std::array<int32_t, 7> ms_aBarColours{}; // Z++: seq RwRGBA  -- bounded array [NUM_PROFILES]
    size_t ms_aBarColours_len{7u}; // Z++ cardinality #ms_aBarColours
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_afStartTime_len == 7u && ms_afStartTime.size() == 7u)
            && (ms_afCumulativeTime_len == 7u && ms_afCumulativeTime.size() == 7u)
            && (ms_afEndTime_len == 7u && ms_afEndTime.size() == 7u)
            && (ms_afMaxEndTime_len == 7u && ms_afMaxEndTime.size() == 7u)
            && (ms_afMaxCumulativeTime_len == 7u && ms_afMaxCumulativeTime.size() == 7u)
            && (ms_aBarColours_len == 7u && ms_aBarColours.size() == 7u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Radar.zpp
struct sRadarTrace_ZppContract {
    int64_t m_nColor{0}; // Z++: ℕ
    int64_t m_eBlipType{0}; // Z++: ℕ
    int32_t m_nEntityHandle{0}; // Z++: ℤ
    ZppVec3 m_vec2DPos{}; // Z++: CVector
    ZppVec3 m_vecPos{}; // Z++: CVector
    int64_t m_BlipIndex{0}; // Z++: ℕ
    bool m_bDim{false}; // Z++: 𝔹
    bool m_bInUse{false}; // Z++: 𝔹
    bool m_bShortRange{false}; // Z++: 𝔹
    bool m_unused{false}; // Z++: 𝔹
    float m_Radius{0.0f}; // Z++: ℝ
    int32_t m_wScale{0}; // Z++: ℤ
    int64_t m_eBlipDisplay{0}; // Z++: ℕ
    int64_t m_eRadarSprite{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_nColor >= 0)
            && (m_eBlipType >= 0)
            && (m_BlipIndex >= 0)
            && (m_eBlipDisplay >= 0)
            && (m_eRadarSprite >= 0);
    }
};
// State Schema Contract from zpp/core/Radar.zpp
struct sRadarTraceSave_ZppContract {
    int64_t m_nColor{0}; // Z++: ℕ
    float m_Radius{0.0f}; // Z++: ℝ
    int64_t m_eBlipType{0}; // Z++: ℕ
    int32_t m_nEntityHandle{0}; // Z++: ℤ
    int32_t m_vec2DPos{0}; // Z++: CVector2D
    ZppVec3 m_vecPos{}; // Z++: CVector
    int64_t m_BlipIndex{0}; // Z++: ℕ
    bool m_bDim{false}; // Z++: 𝔹
    bool m_bInUse{false}; // Z++: 𝔹
    bool m_bShortRange{false}; // Z++: 𝔹
    bool m_unused{false}; // Z++: 𝔹
    int32_t m_wScale{0}; // Z++: ℤ
    int64_t m_eBlipDisplay{0}; // Z++: ℕ
    int64_t m_eRadarSprite{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_nColor >= 0)
            && (m_eBlipType >= 0)
            && (m_BlipIndex >= 0)
            && (m_eBlipDisplay >= 0)
            && (m_eRadarSprite >= 0);
    }
};
// State Schema Contract from zpp/core/Range2D.zpp
struct CRange2D_ZppContract {
    int32_t max_field{0}; // Z++: CVector2D_min
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Range3D.zpp
struct CRange3D_ZppContract {
    int32_t max_field{0}; // Z++: CVector_min
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/References.zpp
struct CReference_ZppContract {
    uint8_t next_card{0u}; // Z++: Ref[CReference]
    uint8_t pentity_card{0u}; // Z++: Ref[CEntity]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (next_card <= 1u)
            && (pentity_card <= 1u);
    }
};
// State Schema Contract from zpp/core/References.zpp
struct CReferences_ZppContract {
    std::array<int32_t, 1318> aRefs{}; // Z++: seq CReference  -- bounded array [NUMREFERENCES]
    size_t aRefs_len{1318u}; // Z++ cardinality #aRefs
    uint8_t pEmptyList_card{0u}; // Z++: Ref[CReference]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (aRefs_len == 1318u && aRefs.size() == 1318u)
            && (pEmptyList_card <= 1u);
    }
};
// State Schema Contract from zpp/core/Ropes.zpp
struct CRope_ZppContract {
    bool m_bActive{false}; // Z++: 𝔹
    bool m_bWasRegistered{false}; // Z++: 𝔹
    bool m_unk{false}; // Z++: 𝔹
    int64_t m_id{0}; // Z++: ℕ
    int64_t m_updateTimer{0}; // Z++: ℕ
    std::array<ZppVec3, 32> m_pos{}; // Z++: seq CVector  -- bounded array [32]
    size_t m_pos_len{32u}; // Z++ cardinality #m_pos
    std::array<ZppVec3, 32> m_speed{}; // Z++: seq CVector  -- bounded array [32]
    size_t m_speed_len{32u}; // Z++ cardinality #m_speed
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pos_len == 32u && m_pos.size() == 32u)
            && (m_speed_len == 32u && m_speed.size() == 32u)
            && (m_id >= 0)
            && (m_updateTimer >= 0);
    }
};
// State Schema Contract from zpp/core/Ropes.zpp
struct CRopes_ZppContract {
    std::array<int32_t, 8> aRopes{}; // Z++: seq CRope  -- bounded array [8]
    size_t aRopes_len{8u}; // Z++ cardinality #aRopes
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (aRopes_len == 8u && aRopes.size() == 8u);
    }
};
// State Schema Contract from zpp/core/Streaming.zpp
struct CStreamingInfo_ZppContract {
    uint8_t m_next_card{0u}; // Z++: Ref[CStreamingInfo]
    uint8_t m_prev_card{0u}; // Z++: Ref[CStreamingInfo]
    int64_t m_loadState{0}; // Z++: ℕ
    int64_t m_flags{0}; // Z++: ℕ
    int32_t m_nextID{0}; // Z++: ℤ
    int64_t m_position{0}; // Z++: ℕ
    int64_t m_size{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_next_card <= 1u)
            && (m_prev_card <= 1u)
            && (m_loadState >= 0)
            && (m_flags >= 0)
            && (m_position >= 0)
            && (m_size >= 0);
    }
};
// State Schema Contract from zpp/core/Streaming.zpp
struct CStreamingChannel_ZppContract {
    std::array<int32_t, 4> streamIds{}; // Z++: seq ℤ  -- bounded array [4]
    size_t streamIds_len{4u}; // Z++ cardinality #streamIds
    std::array<int32_t, 4> offsets{}; // Z++: seq ℤ  -- bounded array [4]
    size_t offsets_len{4u}; // Z++ cardinality #offsets
    int32_t state{0}; // Z++: ℤ
    int32_t field24{0}; // Z++: ℤ
    int32_t position{0}; // Z++: ℤ
    int32_t size{0}; // Z++: ℤ
    int32_t numTries{0}; // Z++: ℤ
    int32_t status{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (streamIds_len == 4u && streamIds.size() == 4u)
            && (offsets_len == 4u && offsets.size() == 4u);
    }
};
// State Schema Contract from zpp/core/SurfaceTable.zpp
struct CSurfaceTable_ZppContract {
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/TimeStep.zpp
struct CTimeStep_ZppContract {
    float ms_fTimeScale{0.0f}; // Z++: ℝ
    float ms_fFramesPerUpdate{0.0f}; // Z++: ℝ
    float ms_fTimeStep{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Timer.zpp
struct CTimer_ZppContract {
    int64_t m_snTimeInMilliseconds{0}; // Z++: ℕ
    int64_t m_snTimeInMillisecondsPauseMode{0}; // Z++: ℕ
    int64_t m_snTimeInMillisecondsNonClipped{0}; // Z++: ℕ
    int64_t m_snPreviousTimeInMilliseconds{0}; // Z++: ℕ
    int64_t m_FrameCounter{0}; // Z++: ℕ
    float ms_fTimeScale{0.0f}; // Z++: ℝ
    float ms_fTimeStep{0.0f}; // Z++: ℝ
    float ms_fTimeStepNonClipped{0.0f}; // Z++: ℝ
    int64_t m_LogicalFramesPassed{0}; // Z++: ℕ
    bool m_CodePause{false}; // Z++: 𝔹
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_snTimeInMilliseconds >= 0)
            && (m_snTimeInMillisecondsPauseMode >= 0)
            && (m_snTimeInMillisecondsNonClipped >= 0)
            && (m_snPreviousTimeInMilliseconds >= 0)
            && (m_FrameCounter >= 0)
            && (m_LogicalFramesPassed >= 0);
    }
};
// State Schema Contract from zpp/core/User.zpp
struct CPlaceName_ZppContract {
    uint8_t m_pZone_card{0u}; // Z++: Ref[CZone]
    uint8_t m_pZone2_card{0u}; // Z++: Ref[CZone]
    int32_t m_nAdditionalTimer{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pZone_card <= 1u)
            && (m_pZone2_card <= 1u);
    }
};
// State Schema Contract from zpp/core/User.zpp
struct CCurrentVehicle_ZppContract {
    uint8_t m_pCurrentVehicle_card{0u}; // Z++: Ref[CVehicle]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pCurrentVehicle_card <= 1u);
    }
};
// State Schema Contract from zpp/core/User.zpp
struct CUserDisplay_ZppContract {
    int32_t PlaceName{0}; // Z++: CPlaceName
    int32_t OnscnTimer{0}; // Z++: COnscreenTimer
    int32_t Pager{0}; // Z++: CPager
    int32_t CurrentVehicle{0}; // Z++: CCurrentVehicle
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/Wanted.zpp
struct CWanted_ZppContract {
    int32_t m_nChaos{0}; // Z++: ℤ
    int32_t m_nMinChaos{0}; // Z++: ℤ
    int32_t m_nLastUpdateTime{0}; // Z++: ℤ
    int64_t m_nLastWantedLevelChange{0}; // Z++: ℕ
    int64_t m_nLastTimeSuspended{0}; // Z++: ℕ
    float m_fCrimeSensitivity{0.0f}; // Z++: ℝ
    int64_t m_CurrentCops{0}; // Z++: ℕ
    int64_t m_MaxCops{0}; // Z++: ℕ
    int64_t m_MaximumLawEnforcerVehicles{0}; // Z++: ℕ
    int64_t m_CopsBeatingSuspect{0}; // Z++: ℕ
    int32_t m_RoadblockDensity{0}; // Z++: ℤ
    uint32_t m_bIgnoredByCops{0u}; // Z++: 𝔹
    uint32_t m_bIgnoredByEveryone{0u}; // Z++: 𝔹
    uint32_t m_bSwatRequired{0u}; // Z++: 𝔹
    uint32_t m_bFbiRequired{0u}; // Z++: 𝔹
    uint32_t m_bArmyRequired{0u}; // Z++: 𝔹
    int32_t m_nWantedLevel{0}; // Z++: ℤ
    int32_t m_nMinWantedLevel{0}; // Z++: ℤ
    std::array<int32_t, 16> m_aCrimes{}; // Z++: seq CCrimeBeingQd  -- bounded array [16]
    size_t m_aCrimes_len{16u}; // Z++ cardinality #m_aCrimes
    std::array<int32_t, 10> m_pCops{}; // Z++: seq Ref[CCopPed]  -- bounded array [10]
    size_t m_pCops_len{10u}; // Z++ cardinality #m_pCops
    int32_t MaximumWantedLevel{0}; // Z++: ℤ
    int32_t nMaximumWantedLevel{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_aCrimes_len == 16u && m_aCrimes.size() == 16u)
            && (m_pCops_len == 10u && m_pCops.size() == 10u)
            && (m_nLastWantedLevelChange >= 0)
            && (m_nLastTimeSuspended >= 0)
            && (m_CurrentCops >= 0)
            && (m_MaxCops >= 0)
            && (m_MaximumLawEnforcerVehicles >= 0)
            && (m_CopsBeatingSuspect >= 0)
            && (m_bIgnoredByCops <= 1u)
            && (m_bIgnoredByEveryone <= 1u)
            && (m_bSwatRequired <= 1u)
            && (m_bFbiRequired <= 1u)
            && (m_bArmyRequired <= 1u);
    }
};
// State Schema Contract from zpp/core/ZoneCull.zpp
struct CCullZone_ZppContract {
    ZppVec3 position{}; // Z++: CVector
    float minx{0.0f}; // Z++: ℝ
    float maxx{0.0f}; // Z++: ℝ
    float miny{0.0f}; // Z++: ℝ
    float maxy{0.0f}; // Z++: ℝ
    float minz{0.0f}; // Z++: ℝ
    float maxz{0.0f}; // Z++: ℝ
    int32_t m_indexStart{0}; // Z++: ℤ
    std::array<int32_t, 3> m_groupIndexCount{}; // Z++: seq ℤ  -- bounded array [3]
    size_t m_groupIndexCount_len{3u}; // Z++ cardinality #m_groupIndexCount
    int32_t m_numBuildings{0}; // Z++: ℤ
    int32_t m_numTreadablesPlus10m{0}; // Z++: ℤ
    int32_t m_numTreadables{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_groupIndexCount_len == 3u && m_groupIndexCount.size() == 3u);
    }
};
// State Schema Contract from zpp/core/ZoneCull.zpp
struct CAttributeZone_ZppContract {
    float minx{0.0f}; // Z++: ℝ
    float maxx{0.0f}; // Z++: ℝ
    float miny{0.0f}; // Z++: ℝ
    float maxy{0.0f}; // Z++: ℝ
    float minz{0.0f}; // Z++: ℝ
    float maxz{0.0f}; // Z++: ℝ
    int32_t attributes{0}; // Z++: ℤ
    int32_t wantedLevel{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/core/common.zpp
struct CRGBA_ZppContract {
    uint8_t this_field_card{0u}; // Z++: Ref[return]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (this_field_card <= 1u);
    }
};
// State Schema Contract from zpp/core/config.zpp
struct configModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{739}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 739);
    }
};
// State Schema Contract from zpp/core/main.zpp
struct GlobalScene_ZppContract {
    uint8_t world_card{0u}; // Z++: Ref[RpWorld]
    uint8_t camera_card{0u}; // Z++: Ref[RwCamera]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (world_card <= 1u)
            && (camera_card <= 1u);
    }
};
// State Schema Contract from zpp/core/obrstr.zpp
struct obrstrModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{128}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 128);
    }
};
// State Schema Contract from zpp/core/re3.zpp
struct re3ModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1720}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1720);
    }
};
// State Schema Contract from zpp/core/templates.zpp
struct CPool_ZppContract {
    uint8_t m_entries_card{0u}; // Z++: Ref[U]
    uint8_t m_flags_card{0u}; // Z++: ℙ ℕ
    int32_t m_size{0}; // Z++: ℤ
    int32_t m_allocPtr{0}; // Z++: ℤ
    int32_t size{0}; // Z++: i
    int32_t entry{0}; // Z++: return
    int32_t index{0}; // Z++: return
    int32_t n{0}; // Z++: return
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_entries_card <= 1u)
            && (m_flags_card <= 1u);
    }
};
// State Schema Contract from zpp/core/templates.zpp
struct CLink_ZppContract {
    int32_t item{0}; // Z++: T
    uint8_t prev_card{0u}; // Z++: Ref[CLink_T]
    uint8_t next_card{0u}; // Z++: Ref[CLink_T]
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (prev_card <= 1u)
            && (next_card <= 1u);
    }
};
// State Schema Contract from zpp/core/templates.zpp
struct CLinkList_ZppContract {
    int32_t tail{0}; // Z++: CLink_T__head
    int32_t freeTail{0}; // Z++: CLink_T__freeHead
    uint8_t links_card{0u}; // Z++: Ref[CLink_T]
    int32_t node{0}; // Z++: return
    uint8_t lnk_card{0u}; // Z++: Ref[CLink_T]
    int32_t n{0}; // Z++: return
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (links_card <= 1u)
            && (lnk_card <= 1u);
    }
};
// Query Operation Schema Contract (ΞCRGBA) from zpp/core/common.zpp
constexpr bool op_CRGBA_operator_RwRGBA_4(const CRGBA_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPool) from zpp/core/templates.zpp
constexpr bool op_CPool_GetId_4(const CPool_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPool) from zpp/core/templates.zpp
constexpr bool op_CPool_GetIsFree_5(const CPool_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCPool) from zpp/core/templates.zpp
constexpr bool op_CPool_GetSize_10(const CPool_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCCamera) from zpp/core/Camera.zpp
constexpr bool op_CCamera_CCamera(const CCamera_ZppContract& pre, const CCamera_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_fMouseAccelHorzntl == 0.0025f) && (post.m_fMouseAccelVertical == 0.003f);
}
constexpr CCamera_ZppContract apply_CCamera_CCamera(const CCamera_ZppContract& pre) {
    CCamera_ZppContract post = pre;
    post.m_fMouseAccelHorzntl = 0.0025f;
    post.m_fMouseAccelVertical = 0.003f;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_Initialise(const CClock_ZppContract& pre, const CClock_ZppContract& post, int64_t scale_in = 25, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nGameClockHours == 12) && (post.ms_nGameClockMinutes == 0) && (post.ms_nGameClockSeconds == 0) && (post.ms_nMillisecondsPerGameMinute == (int64_t)scale_in) && (post.ms_nLastClockTick == (int64_t)CTimer__GetTimeInMilliseconds_in) && (post.ms_bClockHasBeenStored == false);
}
constexpr CClock_ZppContract apply_CClock_Initialise(const CClock_ZppContract& pre, int64_t scale_in = 25, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CClock_ZppContract post = pre;
    post.ms_nGameClockHours = 12;
    post.ms_nGameClockMinutes = 0;
    post.ms_nGameClockSeconds = 0;
    post.ms_nMillisecondsPerGameMinute = (int64_t)scale_in;
    post.ms_nLastClockTick = (int64_t)CTimer__GetTimeInMilliseconds_in;
    post.ms_bClockHasBeenStored = false;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_Update(const CClock_ZppContract& pre, const CClock_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nLastClockTick == (int64_t)CTimer__GetTimeInMilliseconds_in) && (post.ms_nGameClockMinutes == 0) && (post.ms_nGameClockHours == 0);
}
constexpr CClock_ZppContract apply_CClock_Update(const CClock_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CClock_ZppContract post = pre;
    post.ms_nLastClockTick = (int64_t)CTimer__GetTimeInMilliseconds_in;
    post.ms_nGameClockMinutes = 0;
    post.ms_nGameClockHours = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCClock) from zpp/core/Clock.zpp
constexpr bool op_CClock_SetGameClock(const CClock_ZppContract& pre, const CClock_ZppContract& post, int64_t m_in = 25, int64_t h_in = 25, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_nGameClockMinutes == (int64_t)m_in) && (post.ms_nGameClockHours == (int64_t)h_in) && (post.ms_nGameClockSeconds == 0) && (post.ms_nLastClockTick == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CClock_ZppContract apply_CClock_SetGameClock(const CClock_ZppContract& pre, int64_t m_in = 25, int64_t h_in = 25, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CClock_ZppContract post = pre;
    post.ms_nGameClockMinutes = (int64_t)m_in;
    post.ms_nGameClockHours = (int64_t)h_in;
    post.ms_nGameClockSeconds = 0;
    post.ms_nLastClockTick = (int64_t)CTimer__GetTimeInMilliseconds_in;
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
// Delta Operation Schema Contract (ΔCMenuPictureAndText) from zpp/core/FrontEndControls.zpp
constexpr bool op_CMenuPictureAndText_SetNewOldShadowWrapX(const CMenuPictureAndText_ZppContract& pre, const CMenuPictureAndText_ZppContract& post, bool bWrapX_in = true, float newWrapX_in = 2.5f, float oldWrapX_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bWrap == bWrapX_in) && (post.m_wrapX == newWrapX_in) && (post.m_oldWrapx == oldWrapX_in);
}
constexpr CMenuPictureAndText_ZppContract apply_CMenuPictureAndText_SetNewOldShadowWrapX(const CMenuPictureAndText_ZppContract& pre, bool bWrapX_in = true, float newWrapX_in = 2.5f, float oldWrapX_in = 2.5f) {
    CMenuPictureAndText_ZppContract post = pre;
    post.m_bWrap = bWrapX_in;
    post.m_wrapX = newWrapX_in;
    post.m_oldWrapx = oldWrapX_in;
    return post;
}
// Delta Operation Schema Contract (ΔCMenuPage) from zpp/core/FrontEndControls.zpp
constexpr bool op_CMenuPage_Initialise(const CMenuPage_ZppContract& pre, const CMenuPage_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_numControls == 0) && (post.m_pCurrentControl_card == 0u) && (post.m_cursor == 0);
}
constexpr CMenuPage_ZppContract apply_CMenuPage_Initialise(const CMenuPage_ZppContract& pre) {
    CMenuPage_ZppContract post = pre;
    post.m_numControls = 0;
    post.m_pCurrentControl_card = 0u;
    post.m_cursor = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCControllerState) from zpp/core/Pad.zpp
constexpr bool op_CControllerState_Clear(const CControllerState_ZppContract& pre, const CControllerState_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.RightStickY == 0) && (post.RightShoulder2 == 0) && (post.DPadRight == 0) && (post.Select == 0) && (post.Circle == 0) && (post.RightShock == 0);
}
constexpr CControllerState_ZppContract apply_CControllerState_Clear(const CControllerState_ZppContract& pre) {
    CControllerState_ZppContract post = pre;
    post.RightStickY = 0;
    post.RightShoulder2 = 0;
    post.DPadRight = 0;
    post.Select = 0;
    post.Circle = 0;
    post.RightShock = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCKeyboardState) from zpp/core/Pad.zpp
constexpr bool op_CKeyboardState_Clear(const CKeyboardState_ZppContract& pre, const CKeyboardState_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.PGDN == 0) && (post.RIGHT == 0) && (post.NUMLOCK == 0) && (post.ADD == 0) && (post.NUM4 == 0) && (post.NUM8 == 0);
}
constexpr CKeyboardState_ZppContract apply_CKeyboardState_Clear(const CKeyboardState_ZppContract& pre) {
    CKeyboardState_ZppContract post = pre;
    post.PGDN = 0;
    post.RIGHT = 0;
    post.NUMLOCK = 0;
    post.ADD = 0;
    post.NUM4 = 0;
    post.NUM8 = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCStreamingInfo) from zpp/core/Streaming.zpp
constexpr bool op_CStreamingInfo_SetCdPosnAndSize(const CStreamingInfo_ZppContract& pre, const CStreamingInfo_ZppContract& post, int64_t posn_in = 25, int64_t size_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_position == (int64_t)posn_in) && (post.m_size == (int64_t)size_in);
}
constexpr CStreamingInfo_ZppContract apply_CStreamingInfo_SetCdPosnAndSize(const CStreamingInfo_ZppContract& pre, int64_t posn_in = 25, int64_t size_in = 25) {
    CStreamingInfo_ZppContract post = pre;
    post.m_position = (int64_t)posn_in;
    post.m_size = (int64_t)size_in;
    return post;
}
// Delta Operation Schema Contract (ΔCTimer) from zpp/core/Timer.zpp
constexpr bool op_CTimer_Stop(const CTimer_ZppContract& pre, const CTimer_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_snPreviousTimeInMilliseconds == pre.m_snTimeInMilliseconds);
}
constexpr CTimer_ZppContract apply_CTimer_Stop(const CTimer_ZppContract& pre) {
    CTimer_ZppContract post = pre;
    post.m_snPreviousTimeInMilliseconds = pre.m_snTimeInMilliseconds;
    return post;
}
// Delta Operation Schema Contract (ΔCPlaceName) from zpp/core/User.zpp
constexpr bool op_CPlaceName_Init(const CPlaceName_ZppContract& pre, const CPlaceName_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pZone_card == 0u) && (post.m_pZone2_card == 0u) && (post.m_nAdditionalTimer == 0);
}
constexpr CPlaceName_ZppContract apply_CPlaceName_Init(const CPlaceName_ZppContract& pre) {
    CPlaceName_ZppContract post = pre;
    post.m_pZone_card = 0u;
    post.m_pZone2_card = 0u;
    post.m_nAdditionalTimer = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCCurrentVehicle) from zpp/core/User.zpp
constexpr bool op_CCurrentVehicle_Init(const CCurrentVehicle_ZppContract& pre, const CCurrentVehicle_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pCurrentVehicle_card == 0u);
}
constexpr CCurrentVehicle_ZppContract apply_CCurrentVehicle_Init(const CCurrentVehicle_ZppContract& pre) {
    CCurrentVehicle_ZppContract post = pre;
    post.m_pCurrentVehicle_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCWanted) from zpp/core/Wanted.zpp
constexpr bool op_CWanted_Initialise(const CWanted_ZppContract& pre, const CWanted_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nChaos == 0) && (post.m_nMinChaos == 0) && (post.m_nLastUpdateTime == 0) && (post.m_nLastWantedLevelChange == 0) && (post.m_nLastTimeSuspended == 0) && (post.m_CurrentCops == 0);
}
constexpr CWanted_ZppContract apply_CWanted_Initialise(const CWanted_ZppContract& pre) {
    CWanted_ZppContract post = pre;
    post.m_nChaos = 0;
    post.m_nMinChaos = 0;
    post.m_nLastUpdateTime = 0;
    post.m_nLastWantedLevelChange = 0;
    post.m_nLastTimeSuspended = 0;
    post.m_CurrentCops = 0;
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
        CCam_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCam_ZppContract bad{};
        bad.m_arrPreviousVectors_len = 0u;
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
        CCamera_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCamera_ZppContract bad{};
        bad.Cams_len = 0u;
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
        CPlaceableShText_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableShText_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableShTextTwoLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableShTextTwoLines_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableShOption_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableShOption_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceableShOptionTwoLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableShOptionTwoLines_ZppContract bad{};
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
        CPlaceableShSprite_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceableShSprite_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuBase_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuBase_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuDummy_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuDummy_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuPictureAndText_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuPictureAndText_ZppContract bad{};
        bad.m_sprites_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoice_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoice_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoiceTriggered_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoiceTriggered_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoiceTriggeredAlways_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoiceTriggeredAlways_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoicePictured_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoicePictured_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoicePicturedTriggered_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoicePicturedTriggered_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoicePicturedTriggeredAnyMove_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoicePicturedTriggeredAnyMove_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoiceTwoLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoiceTwoLines_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuMultiChoiceTwoLinesTriggered_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuMultiChoiceTwoLinesTriggered_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuOnOff_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuOnOff_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuOnOffTriggered_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuOnOffTriggered_ZppContract bad{};
        bad.m_options_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuLineLister_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuLineLister_ZppContract bad{};
        bad.m_linesLeft_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuPage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuPage_ZppContract bad{};
        bad.m_controls_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMenuPageAnyMove_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMenuPageAnyMove_ZppContract bad{};
        bad.m_controls_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCustomScreenLayout_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCustomScreenLayout_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCFO_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCFO_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCFOSelect_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCFOSelect_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCFOSlider_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCFOSlider_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCFODynamic_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCFODynamic_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        MenuTrapezoid_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MenuTrapezoid_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        FrontendTriggersModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        FrontendTriggersModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        SkyDataPrefix_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        SkyDataPrefix_ZppContract bad{};
        bad.pktSize1 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        DMAGIFUpload_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        DMAGIFUpload_ZppContract bad{};
        bad.giftag_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CGeneral_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CGeneral_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CIniFile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CIniFile_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CKeyGen_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CKeyGen_ZppContract bad{};
        bad.keyTable_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        LeedsModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        LeedsModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CPtrNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPtrNode_ZppContract bad{};
        bad.prev_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPtrList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPtrList_ZppContract bad{};
        bad.first_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CEntryInfoNode_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEntryInfoNode_ZppContract bad{};
        bad.list_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CEntryInfoList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEntryInfoList_ZppContract bad{};
        bad.first_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        MenuScreensModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MenuScreensModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        MenuScreensCustomModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        MenuScreensCustomModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CControllerState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CControllerState_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMouseControllerState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMouseControllerState_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMousePointerStateHelper_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMousePointerStateHelper_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CKeyboardState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CKeyboardState_ZppContract bad{};
        bad.F_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceable_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceable_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPools_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPools_ZppContract bad{};
        bad.ms_pPtrNodePool_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CProfile_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CProfile_ZppContract bad{};
        bad.ms_afStartTime_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        sRadarTrace_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        sRadarTrace_ZppContract bad{};
        bad.m_nColor = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        sRadarTraceSave_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        sRadarTraceSave_ZppContract bad{};
        bad.m_nColor = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRange2D_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRange2D_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRange3D_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRange3D_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CReference_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CReference_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CReferences_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CReferences_ZppContract bad{};
        bad.aRefs_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CRope_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRope_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CRopes_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRopes_ZppContract bad{};
        bad.aRopes_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStreamingInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStreamingInfo_ZppContract bad{};
        bad.m_next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStreamingChannel_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStreamingChannel_ZppContract bad{};
        bad.streamIds_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSurfaceTable_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSurfaceTable_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTimeStep_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTimeStep_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CTimer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTimer_ZppContract bad{};
        bad.m_snTimeInMilliseconds = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaceName_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaceName_ZppContract bad{};
        bad.m_pZone_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCurrentVehicle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCurrentVehicle_ZppContract bad{};
        bad.m_pCurrentVehicle_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CUserDisplay_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CUserDisplay_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWanted_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWanted_ZppContract bad{};
        bad.m_aCrimes_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCullZone_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCullZone_ZppContract bad{};
        bad.m_groupIndexCount_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAttributeZone_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAttributeZone_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRGBA_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRGBA_ZppContract bad{};
        bad.this_field_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        configModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        configModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        GlobalScene_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GlobalScene_ZppContract bad{};
        bad.world_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        obrstrModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        obrstrModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        re3ModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        re3ModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CPool_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPool_ZppContract bad{};
        bad.m_entries_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CLink_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CLink_ZppContract bad{};
        bad.prev_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CLinkList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CLinkList_ZppContract bad{};
        bad.links_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        const CRGBA_ZppContract st{};
        if (!op_CRGBA_operator_RwRGBA_4(st)) return false;
    }
    {
        const CPool_ZppContract st{};
        if (!op_CPool_GetId_4(st)) return false;
    }
    {
        const CPool_ZppContract st{};
        if (!op_CPool_GetIsFree_5(st)) return false;
    }
    {
        const CPool_ZppContract st{};
        if (!op_CPool_GetSize_10(st)) return false;
    }
    {
        CCamera_ZppContract pre{};
        pre.m_fMouseAccelHorzntl = 0.0f;
        pre.m_fMouseAccelVertical = 0.0f;
        const auto post = apply_CCamera_CCamera(pre);
        if (!op_CCamera_CCamera(pre, post)) return false;
        {
            CCamera_ZppContract badPost = post;
            badPost.m_fMouseAccelHorzntl = 0.0f;
            if (op_CCamera_CCamera(pre, badPost)) return false;
        }
        {
            CCamera_ZppContract badPost = post;
            badPost.m_fMouseAccelVertical = 0.0f;
            if (op_CCamera_CCamera(pre, badPost)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        const int64_t scale_in = 25;
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.ms_nGameClockHours = 0;
        pre.ms_nGameClockMinutes = 1;
        pre.ms_nGameClockSeconds = 1;
        pre.ms_nMillisecondsPerGameMinute = 0;
        pre.ms_nLastClockTick = 0;
        pre.ms_bClockHasBeenStored = true;
        const auto post = apply_CClock_Initialise(pre, scale_in, CTimer__GetTimeInMilliseconds_in);
        if (!op_CClock_Initialise(pre, post, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = 0;
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = 1;
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockSeconds = 1;
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nMillisecondsPerGameMinute = (int64_t)(scale_in + 1);
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_bClockHasBeenStored = true;
            if (op_CClock_Initialise(pre, badPost, scale_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.ms_nLastClockTick = 0;
        pre.ms_nGameClockMinutes = 1;
        pre.ms_nGameClockHours = 1;
        const auto post = apply_CClock_Update(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CClock_Update(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CClock_Update(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = 1;
            if (op_CClock_Update(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = 1;
            if (op_CClock_Update(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CClock_ZppContract pre{};
        const int64_t m_in = 25;
        const int64_t h_in = 25;
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.ms_nGameClockMinutes = 0;
        pre.ms_nGameClockHours = 0;
        pre.ms_nGameClockSeconds = 1;
        pre.ms_nLastClockTick = 0;
        const auto post = apply_CClock_SetGameClock(pre, m_in, h_in, CTimer__GetTimeInMilliseconds_in);
        if (!op_CClock_SetGameClock(pre, post, m_in, h_in, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockMinutes = (int64_t)(m_in + 1);
            if (op_CClock_SetGameClock(pre, badPost, m_in, h_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockHours = (int64_t)(h_in + 1);
            if (op_CClock_SetGameClock(pre, badPost, m_in, h_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nGameClockSeconds = 1;
            if (op_CClock_SetGameClock(pre, badPost, m_in, h_in, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CClock_ZppContract badPost = post;
            badPost.ms_nLastClockTick = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CClock_SetGameClock(pre, badPost, m_in, h_in, CTimer__GetTimeInMilliseconds_in)) return false;
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
        CMenuPictureAndText_ZppContract pre{};
        const bool bWrapX_in = true;
        const float newWrapX_in = 2.5f;
        const float oldWrapX_in = 2.5f;
        pre.m_bWrap = false;
        pre.m_wrapX = 0.0f;
        pre.m_oldWrapx = 0.0f;
        const auto post = apply_CMenuPictureAndText_SetNewOldShadowWrapX(pre, bWrapX_in, newWrapX_in, oldWrapX_in);
        if (!op_CMenuPictureAndText_SetNewOldShadowWrapX(pre, post, bWrapX_in, newWrapX_in, oldWrapX_in)) return false;
        {
            CMenuPictureAndText_ZppContract badPost = post;
            badPost.m_bWrap = !bWrapX_in;
            if (op_CMenuPictureAndText_SetNewOldShadowWrapX(pre, badPost, bWrapX_in, newWrapX_in, oldWrapX_in)) return false;
        }
        {
            CMenuPictureAndText_ZppContract badPost = post;
            badPost.m_wrapX = newWrapX_in + 1.0f;
            if (op_CMenuPictureAndText_SetNewOldShadowWrapX(pre, badPost, bWrapX_in, newWrapX_in, oldWrapX_in)) return false;
        }
        {
            CMenuPictureAndText_ZppContract badPost = post;
            badPost.m_oldWrapx = oldWrapX_in + 1.0f;
            if (op_CMenuPictureAndText_SetNewOldShadowWrapX(pre, badPost, bWrapX_in, newWrapX_in, oldWrapX_in)) return false;
        }
    }
    {
        CMenuPage_ZppContract pre{};
        pre.m_numControls = 1;
        pre.m_pCurrentControl_card = 1u;
        pre.m_cursor = 1;
        const auto post = apply_CMenuPage_Initialise(pre);
        if (!op_CMenuPage_Initialise(pre, post)) return false;
        {
            CMenuPage_ZppContract badPost = post;
            badPost.m_numControls = 1;
            if (op_CMenuPage_Initialise(pre, badPost)) return false;
        }
        {
            CMenuPage_ZppContract badPost = post;
            badPost.m_pCurrentControl_card = 1u;
            if (op_CMenuPage_Initialise(pre, badPost)) return false;
        }
        {
            CMenuPage_ZppContract badPost = post;
            badPost.m_cursor = 1;
            if (op_CMenuPage_Initialise(pre, badPost)) return false;
        }
    }
    {
        CControllerState_ZppContract pre{};
        pre.RightStickY = 1;
        pre.RightShoulder2 = 1;
        pre.DPadRight = 1;
        pre.Select = 1;
        pre.Circle = 1;
        pre.RightShock = 1;
        const auto post = apply_CControllerState_Clear(pre);
        if (!op_CControllerState_Clear(pre, post)) return false;
        {
            CControllerState_ZppContract badPost = post;
            badPost.RightStickY = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
        {
            CControllerState_ZppContract badPost = post;
            badPost.RightShoulder2 = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
        {
            CControllerState_ZppContract badPost = post;
            badPost.DPadRight = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
        {
            CControllerState_ZppContract badPost = post;
            badPost.Select = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
        {
            CControllerState_ZppContract badPost = post;
            badPost.Circle = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
        {
            CControllerState_ZppContract badPost = post;
            badPost.RightShock = 1;
            if (op_CControllerState_Clear(pre, badPost)) return false;
        }
    }
    {
        CKeyboardState_ZppContract pre{};
        pre.PGDN = 1;
        pre.RIGHT = 1;
        pre.NUMLOCK = 1;
        pre.ADD = 1;
        pre.NUM4 = 1;
        pre.NUM8 = 1;
        const auto post = apply_CKeyboardState_Clear(pre);
        if (!op_CKeyboardState_Clear(pre, post)) return false;
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.PGDN = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.RIGHT = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.NUMLOCK = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.ADD = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.NUM4 = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
        {
            CKeyboardState_ZppContract badPost = post;
            badPost.NUM8 = 1;
            if (op_CKeyboardState_Clear(pre, badPost)) return false;
        }
    }
    {
        CStreamingInfo_ZppContract pre{};
        const int64_t posn_in = 25;
        const int64_t size_in = 25;
        pre.m_position = 0;
        pre.m_size = 0;
        const auto post = apply_CStreamingInfo_SetCdPosnAndSize(pre, posn_in, size_in);
        if (!op_CStreamingInfo_SetCdPosnAndSize(pre, post, posn_in, size_in)) return false;
        {
            CStreamingInfo_ZppContract badPost = post;
            badPost.m_position = (int64_t)(posn_in + 1);
            if (op_CStreamingInfo_SetCdPosnAndSize(pre, badPost, posn_in, size_in)) return false;
        }
        {
            CStreamingInfo_ZppContract badPost = post;
            badPost.m_size = (int64_t)(size_in + 1);
            if (op_CStreamingInfo_SetCdPosnAndSize(pre, badPost, posn_in, size_in)) return false;
        }
    }
    {
        CTimer_ZppContract pre{};
        pre.m_snTimeInMilliseconds = 7; pre.m_snPreviousTimeInMilliseconds = 0;
        const auto post = apply_CTimer_Stop(pre);
        if (!op_CTimer_Stop(pre, post)) return false;
        {
            CTimer_ZppContract badPost = post;
            badPost.m_snPreviousTimeInMilliseconds = pre.m_snTimeInMilliseconds + 1;
            if (op_CTimer_Stop(pre, badPost)) return false;
        }
    }
    {
        CPlaceName_ZppContract pre{};
        pre.m_pZone_card = 1u;
        pre.m_pZone2_card = 1u;
        pre.m_nAdditionalTimer = 1;
        const auto post = apply_CPlaceName_Init(pre);
        if (!op_CPlaceName_Init(pre, post)) return false;
        {
            CPlaceName_ZppContract badPost = post;
            badPost.m_pZone_card = 1u;
            if (op_CPlaceName_Init(pre, badPost)) return false;
        }
        {
            CPlaceName_ZppContract badPost = post;
            badPost.m_pZone2_card = 1u;
            if (op_CPlaceName_Init(pre, badPost)) return false;
        }
        {
            CPlaceName_ZppContract badPost = post;
            badPost.m_nAdditionalTimer = 1;
            if (op_CPlaceName_Init(pre, badPost)) return false;
        }
    }
    {
        CCurrentVehicle_ZppContract pre{};
        pre.m_pCurrentVehicle_card = 1u;
        const auto post = apply_CCurrentVehicle_Init(pre);
        if (!op_CCurrentVehicle_Init(pre, post)) return false;
        {
            CCurrentVehicle_ZppContract badPost = post;
            badPost.m_pCurrentVehicle_card = 1u;
            if (op_CCurrentVehicle_Init(pre, badPost)) return false;
        }
    }
    {
        CWanted_ZppContract pre{};
        pre.m_nChaos = 1;
        pre.m_nMinChaos = 1;
        pre.m_nLastUpdateTime = 1;
        pre.m_nLastWantedLevelChange = 1;
        pre.m_nLastTimeSuspended = 1;
        pre.m_CurrentCops = 1;
        const auto post = apply_CWanted_Initialise(pre);
        if (!op_CWanted_Initialise(pre, post)) return false;
        {
            CWanted_ZppContract badPost = post;
            badPost.m_nChaos = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
        {
            CWanted_ZppContract badPost = post;
            badPost.m_nMinChaos = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
        {
            CWanted_ZppContract badPost = post;
            badPost.m_nLastUpdateTime = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
        {
            CWanted_ZppContract badPost = post;
            badPost.m_nLastWantedLevelChange = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
        {
            CWanted_ZppContract badPost = post;
            badPost.m_nLastTimeSuspended = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
        {
            CWanted_ZppContract badPost = post;
            badPost.m_CurrentCops = 1;
            if (op_CWanted_Initialise(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_core_zpp_invariants(), "Z++ formal contract invariant failed for core");
static_assert(op_CClock_Initialise(CClock_ZppContract{}, apply_CClock_Initialise(CClock_ZppContract{}, 1000, 4200), 1000, 4200), "CClock_Initialise must accept arbitrary scale and timestamp");
} // namespace zpp_core_spec
#endif // GTANGEL_ZPP_CORE_SPEC_GUARD
