#pragma once
#ifndef GTANGEL_ZPP_RENDERER_SPEC_GUARD
#define GTANGEL_ZPP_RENDERER_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/renderer
// Derived from C:/ddd/GTAngel/cpp_test/zpp/renderer/*.zpp
// Modules: 38 | Formal Schemas: 1267 | Free Types: 51
// ============================================================================
namespace zpp_renderer_spec {
static constexpr int kZppModuleCount = 38;
static constexpr int kZppSchemaCount = 1267;
static constexpr int kZppEnumCount   = 51;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/renderer/Console.zpp
enum class Console_Enum_1 : int32_t { MAX_LINES_v = 8, MAX_STR_LEN_v = 40 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_1 : int32_t { SUN_CORE_v = 1, SUN_CORONA_v = 2 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_2 : int32_t { TYPE_STAR_v = 0, TYPE_NORMAL_v = 1, TYPE_MOON_v = 2, TYPE_REFLECT_v = 3, TYPE_HEADLIGHT_v = 4, TYPE_HEX_v = 5, TYPE_CIRCLE_v = 6, TYPE_RING_v = 7, TYPE_STREAK_v = 8 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_3 : int32_t { FLARE_NONE_v = 0, FLARE_SUN_v = 1, FLARE_HEADLIGHTS_v = 2 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_4 : int32_t { REFLECTION_OFF_v = 0, REFLECTION_ON_v = 1 };
// Free Type from zpp/renderer/Coronas.zpp
enum class Coronas_Enum_5 : int32_t { LOSCHECK_OFF_v = 0, LOSCHECK_ON_v = 1 };
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct C2dEffect_ZppContract {
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    int64_t lightType{0}; // Z++: ℕ
    int64_t roadReflection{0}; // Z++: ℕ
    int64_t flareType{0}; // Z++: ℕ
    int64_t shadowIntensity{0}; // Z++: ℕ
    int64_t flags{0}; // Z++: ℕ
    uint8_t corona_card{0u}; // Z++: Ref[RwTexture]
    uint8_t shadow_card{0u}; // Z++: Ref[RwTexture]
    ZppVec3 dir{}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    ZppVec3 useDir{}; // Z++: CVector
    ZppVec3 pos{}; // Z++: CVector
    int32_t col{0}; // Z++: CRGBA
    int32_t particle{0}; // Z++: Particle
    int32_t attractor{0}; // Z++: Attractor
    int32_t pedattr{0}; // Z++: PedAttractor
    constexpr bool checkInvariant() const {
        return (lightType >= 0)
            && (roadReflection >= 0)
            && (flareType >= 0)
            && (shadowIntensity >= 0)
            && (flags >= 0)
            && (corona_card <= 1u)
            && (shadow_card <= 1u)
            && (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Light_ZppContract {
    float dist{0.0f}; // Z++: ℝ
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    int64_t lightType{0}; // Z++: ℕ
    int64_t roadReflection{0}; // Z++: ℕ
    int64_t flareType{0}; // Z++: ℕ
    int64_t shadowIntensity{0}; // Z++: ℕ
    int64_t flags{0}; // Z++: ℕ
    uint8_t corona_card{0u}; // Z++: Ref[RwTexture]
    uint8_t shadow_card{0u}; // Z++: Ref[RwTexture]
    constexpr bool checkInvariant() const {
        return (lightType >= 0)
            && (roadReflection >= 0)
            && (flareType >= 0)
            && (shadowIntensity >= 0)
            && (flags >= 0)
            && (corona_card <= 1u)
            && (shadow_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Particle_ZppContract {
    int32_t particleType{0}; // Z++: ℤ
    ZppVec3 dir{}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    ZppVec3 dir{}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct PedAttractor_ZppContract {
    ZppVec3 queueDir{}; // Z++: CVector
    ZppVec3 useDir{}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Antennas.zpp
struct CAntenna_ZppContract {
    bool active{false}; // Z++: 𝔹
    bool updatedLastFrame{false}; // Z++: 𝔹
    int64_t id{0}; // Z++: ℕ
    float segmentLength{0.0f}; // Z++: ℝ
    std::array<ZppVec3, 6> pos{}; // Z++: seq CVector  -- bounded array [6]
    size_t pos_len{6u}; // Z++ cardinality #pos
    std::array<ZppVec3, 6> speed{}; // Z++: seq CVector  -- bounded array [6]
    size_t speed_len{6u}; // Z++ cardinality #speed
    constexpr bool checkInvariant() const {
        return (pos_len == 6u && pos.size() == 6u)
            && (speed_len == 6u && speed.size() == 6u)
            && (id >= 0);
    }
};
// State Schema Contract from zpp/renderer/Clouds.zpp
struct CClouds_ZppContract {
    float CloudRotation{0.0f}; // Z++: ℝ
    int64_t IndividualRotation{0}; // Z++: ℕ
    float ms_cameraRoll{0.0f}; // Z++: ℝ
    float ms_horizonZ{0.0f}; // Z++: ℝ
    float ms_HorizonTilt{0.0f}; // Z++: ℝ
    int32_t ms_colourTop{0}; // Z++: CRGBA
    int32_t ms_colourBottom{0}; // Z++: CRGBA
    int32_t ms_colourBkGrd{0}; // Z++: CRGBA
    constexpr bool checkInvariant() const {
        return (IndividualRotation >= 0);
    }
};
// State Schema Contract from zpp/renderer/Console.zpp
struct CConsole_ZppContract {
    int64_t m_nLineCount{0}; // Z++: ℕ
    int64_t m_nCurrentLine{0}; // Z++: ℕ
    std::array<uint32_t, 8> m_aTimer{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aTimer_len{8u}; // Z++ cardinality #m_aTimer
    std::array<uint32_t, 8> m_aRed{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aRed_len{8u}; // Z++ cardinality #m_aRed
    std::array<uint32_t, 8> m_aGreen{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aGreen_len{8u}; // Z++ cardinality #m_aGreen
    std::array<uint32_t, 8> m_aBlue{}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    size_t m_aBlue_len{8u}; // Z++ cardinality #m_aBlue
    constexpr bool checkInvariant() const {
        return (m_aTimer_len == 8u && m_aTimer.size() == 8u)
            && (m_aRed_len == 8u && m_aRed.size() == 8u)
            && (m_aGreen_len == 8u && m_aGreen.size() == 8u)
            && (m_aBlue_len == 8u && m_aBlue.size() == 8u)
            && (m_nLineCount >= 0)
            && (m_nCurrentLine >= 0);
    }
};
// State Schema Contract from zpp/renderer/Coronas.zpp
struct FlareDef_ZppContract {
    float position{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    int32_t red{0}; // Z++: ℤ
    int32_t green{0}; // Z++: ℤ
    int32_t blue{0}; // Z++: ℤ
    int32_t alpha{0}; // Z++: ℤ
    int32_t texture{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Coronas.zpp
struct CRegisteredCorona_ZppContract {
    ZppVec3 coors{}; // Z++: CVector
    int64_t id{0}; // Z++: ℕ
    int64_t lastLOScheck{0}; // Z++: ℕ
    uint8_t texture_card{0u}; // Z++: Ref[RwTexture]
    float size{0.0f}; // Z++: ℝ
    float someAngle{0.0f}; // Z++: ℝ
    float drawDist{0.0f}; // Z++: ℝ
    float nearDist{0.0f}; // Z++: ℝ
    float heightAboveRoad{0.0f}; // Z++: ℝ
    int64_t red{0}; // Z++: ℕ
    int64_t green{0}; // Z++: ℕ
    int64_t blue{0}; // Z++: ℕ
    int64_t alpha{0}; // Z++: ℕ
    int64_t fadeAlpha{0}; // Z++: ℕ
    bool registeredThisFrame{false}; // Z++: 𝔹
    int32_t flareType{0}; // Z++: ℤ
    int32_t reflection{0}; // Z++: ℤ
    uint32_t LOScheck{0u}; // Z++: 𝔹
    uint32_t offScreen{0u}; // Z++: 𝔹
    uint32_t firstUpdate{0u}; // Z++: 𝔹
    uint32_t drawStreak{0u}; // Z++: 𝔹
    uint32_t sightClear{0u}; // Z++: 𝔹
    uint32_t useNearDist{0u}; // Z++: 𝔹
    uint32_t renderReflection{0u}; // Z++: 𝔹
    std::array<int32_t, 6> prevX{}; // Z++: seq ℤ  -- bounded array [6]
    size_t prevX_len{6u}; // Z++ cardinality #prevX
    std::array<int32_t, 6> prevY{}; // Z++: seq ℤ  -- bounded array [6]
    size_t prevY_len{6u}; // Z++ cardinality #prevY
    std::array<uint32_t, 6> prevRed{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevRed_len{6u}; // Z++ cardinality #prevRed
    std::array<uint32_t, 6> prevGreen{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevGreen_len{6u}; // Z++ cardinality #prevGreen
    std::array<uint32_t, 6> prevBlue{}; // Z++: seq ℕ  -- bounded array [6]
    size_t prevBlue_len{6u}; // Z++ cardinality #prevBlue
    std::array<bool, 6> hasValue{}; // Z++: seq 𝔹  -- bounded array [6]
    size_t hasValue_len{6u}; // Z++ cardinality #hasValue
    constexpr bool checkInvariant() const {
        return (prevX_len == 6u && prevX.size() == 6u)
            && (prevY_len == 6u && prevY.size() == 6u)
            && (prevRed_len == 6u && prevRed.size() == 6u)
            && (prevGreen_len == 6u && prevGreen.size() == 6u)
            && (prevBlue_len == 6u && prevBlue.size() == 6u)
            && (hasValue_len == 6u && hasValue.size() == 6u)
            && (id >= 0)
            && (lastLOScheck >= 0)
            && (texture_card <= 1u)
            && (red >= 0)
            && (green >= 0)
            && (blue >= 0)
            && (alpha >= 0)
            && (fadeAlpha >= 0)
            && (LOScheck <= 1u)
            && (offScreen <= 1u)
            && (firstUpdate <= 1u)
            && (drawStreak <= 1u)
            && (sightClear <= 1u)
            && (useNearDist <= 1u)
            && (renderReflection <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Credits.zpp
struct CCredits_ZppContract {
    bool bCreditsGoing{false}; // Z++: 𝔹
    int64_t CreditsStartTime{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (CreditsStartTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/CutsceneShadow.zpp
struct CCutsceneShadow_ZppContract {
    int32_t m_Camera{0}; // Z++: CShadowCamera
    bool m_bResample{false}; // Z++: 𝔹
    int32_t m_ResampleCamera{0}; // Z++: CShadowCamera
    int32_t m_nBlurPasses{0}; // Z++: ℤ
    int32_t m_BlurCamera{0}; // Z++: CShadowCamera
    bool m_bGradient{false}; // Z++: 𝔹
    int32_t m_GradientCamera{0}; // Z++: CShadowCamera
    uint8_t m_pAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t m_pClump_card{0u}; // Z++: Ref[RpClump]
    int32_t m_nRwObjectType{0}; // Z++: ℤ
    uint8_t m_pLight_card{0u}; // Z++: Ref[RpLight]
    int32_t m_BoundingSphere{0}; // Z++: RwSphere
    int32_t m_BaseSphere{0}; // Z++: RwSphere
    constexpr bool checkInvariant() const {
        return (m_pAtomic_card <= 1u)
            && (m_pClump_card <= 1u)
            && (m_pLight_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Draw.zpp
struct CDraw_ZppContract {
    float ms_fNearClipZ{0.0f}; // Z++: ℝ
    float ms_fFarClipZ{0.0f}; // Z++: ℝ
    float ms_fFOV{0.0f}; // Z++: ℝ
    float ms_fScaledFOV{0.0f}; // Z++: ℝ
    int64_t FadeValue{0}; // Z++: ℕ
    int64_t FadeRed{0}; // Z++: ℕ
    int64_t FadeGreen{0}; // Z++: ℕ
    int64_t FadeBlue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (FadeValue >= 0)
            && (FadeRed >= 0)
            && (FadeGreen >= 0)
            && (FadeBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScriptPath_ZppContract {
    int32_t m_numNodes{0}; // Z++: ℤ
    uint8_t m_pNode_card{0u}; // Z++: Ref[CPlaneNode]
    float m_fTotalLength{0.0f}; // Z++: ℝ
    float m_fSpeed{0.0f}; // Z++: ℝ
    float m_fPosition{0.0f}; // Z++: ℝ
    float m_fObjectLength{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    std::array<int32_t, 6> m_pObjects{}; // Z++: seq Ref[CObject]  -- bounded array [6]
    size_t m_pObjects_len{6u}; // Z++ cardinality #m_pObjects
    constexpr bool checkInvariant() const {
        return (m_pObjects_len == 6u && m_pObjects.size() == 6u)
            && (m_pNode_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScriptPaths_ZppContract {
    std::array<int32_t, 3> aArray{}; // Z++: seq CScriptPath  -- bounded array [3]
    size_t aArray_len{3u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 3u && aArray.size() == 3u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneTrail_ZppContract {
    std::array<ZppVec3, 16> m_pos{}; // Z++: seq CVector  -- bounded array [16]
    size_t m_pos_len{16u}; // Z++ cardinality #m_pos
    std::array<int32_t, 16> m_time{}; // Z++: seq ℤ  -- bounded array [16]
    size_t m_time_len{16u}; // Z++ cardinality #m_time
    constexpr bool checkInvariant() const {
        return (m_pos_len == 16u && m_pos.size() == 16u)
            && (m_time_len == 16u && m_time.size() == 16u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneTrails_ZppContract {
    std::array<int32_t, 6> aArray{}; // Z++: seq CPlaneTrail  -- bounded array [6]
    size_t aArray_len{6u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 6u && aArray.size() == 6u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneBanner_ZppContract {
    std::array<ZppVec3, 8> m_pos{}; // Z++: seq CVector  -- bounded array [8]
    size_t m_pos_len{8u}; // Z++ cardinality #m_pos
    constexpr bool checkInvariant() const {
        return (m_pos_len == 8u && m_pos.size() == 8u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CPlaneBanners_ZppContract {
    std::array<int32_t, 5> aArray{}; // Z++: seq CPlaneBanner  -- bounded array [5]
    size_t aArray_len{5u}; // Z++ cardinality #aArray
    constexpr bool checkInvariant() const {
        return (aArray_len == 5u && aArray.size() == 5u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CEscalator_ZppContract {
    ZppVec3 m_pos0{}; // Z++: CVector
    ZppVec3 m_pos1{}; // Z++: CVector
    ZppVec3 m_pos2{}; // Z++: CVector
    ZppVec3 m_pos3{}; // Z++: CVector
    int32_t m_matrix{0}; // Z++: CMatrix
    bool m_bIsActive{false}; // Z++: 𝔹
    bool m_bIsMovingDown{false}; // Z++: 𝔹
    int32_t m_stepsCount{0}; // Z++: ℤ
    float m_lowerEnd{0.0f}; // Z++: ℝ
    float m_upperEnd{0.0f}; // Z++: ℝ
    ZppVec3 m_midPoint{}; // Z++: CVector
    float m_radius{0.0f}; // Z++: ℝ
    std::array<int32_t, 24> m_pSteps{}; // Z++: seq Ref[CObject]  -- bounded array [24]
    size_t m_pSteps_len{24u}; // Z++ cardinality #m_pSteps
    constexpr bool checkInvariant() const {
        return (m_pSteps_len == 24u && m_pSteps.size() == 24u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CMovingThing_ZppContract {
    uint8_t m_pNext_card{0u}; // Z++: Ref[CMovingThing]
    uint8_t m_pPrev_card{0u}; // Z++: Ref[CMovingThing]
    int32_t m_nType{0}; // Z++: ℤ
    int32_t m_farAway{0}; // Z++: ℤ
    ZppVec3 m_vecPosn{}; // Z++: CVector
    uint8_t m_pEntity_card{0u}; // Z++: Ref[CEntity]
    int32_t m_nHidden{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pNext_card <= 1u)
            && (m_pPrev_card <= 1u)
            && (m_pEntity_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CMovingThings_ZppContract {
    int32_t StartCloseList{0}; // Z++: CMovingThing
    int32_t EndCloseList{0}; // Z++: CMovingThing
    int32_t Num{0}; // Z++: ℤ
    std::array<int32_t, 48> aMovingThings{}; // Z++: seq CMovingThing  -- bounded array [NUMMOVINGTHINGS]
    size_t aMovingThings_len{48u}; // Z++ cardinality #aMovingThings
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (aMovingThings_len == 48u && aMovingThings.size() == 48u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CScrollBar_ZppContract {
    int64_t m_Counter{0}; // Z++: ℕ
    int32_t m_pMessage{0}; // Z++: seq CHAR
    ZppVec3 m_Position{}; // Z++: CVector
    int64_t m_MessageCurrentChar{0}; // Z++: ℕ
    int64_t m_MessageLength{0}; // Z++: ℕ
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fIntensity{0.0f}; // Z++: ℝ
    std::array<uint32_t, 40> m_MessageBar{}; // Z++: seq ℕ  -- bounded array [40]
    size_t m_MessageBar_len{40u}; // Z++ cardinality #m_MessageBar
    int64_t m_Type{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    float m_fScale{0.0f}; // Z++: ℝ
    int32_t TonightsEvent{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_MessageBar_len == 40u && m_MessageBar.size() == 40u)
            && (m_Counter >= 0)
            && (m_MessageCurrentChar >= 0)
            && (m_MessageLength >= 0)
            && (m_Type >= 0)
            && (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CSmokeTrails_ZppContract {
    std::array<int32_t, 3> aSmoke{}; // Z++: seq CSmokeTrail  -- bounded array [3]
    size_t aSmoke_len{3u}; // Z++ cardinality #aSmoke
    bool CigOn{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (aSmoke_len == 3u && aSmoke.size() == 3u);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CTowerClock_ZppContract {
    ZppVec3 m_Position{}; // Z++: CVector
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fDrawDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    float m_fIntensity{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Fluff.zpp
struct CDigitalClock_ZppContract {
    ZppVec3 m_Position{}; // Z++: CVector
    ZppVec3 m_Size{}; // Z++: CVector
    float m_fDrawDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_uRed{0}; // Z++: ℕ
    int64_t m_uGreen{0}; // Z++: ℕ
    int64_t m_uBlue{0}; // Z++: ℕ
    bool m_bVisible{false}; // Z++: 𝔹
    float m_fIntensity{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_uRed >= 0)
            && (m_uGreen >= 0)
            && (m_uBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Font.zpp
struct CFontDetails_ZppContract {
    int32_t color{0}; // Z++: CRGBA
    float scaleX{0.0f}; // Z++: ℝ
    float scaleY{0.0f}; // Z++: ℝ
    float slant{0.0f}; // Z++: ℝ
    float slantRefX{0.0f}; // Z++: ℝ
    float slantRefY{0.0f}; // Z++: ℝ
    int32_t justify{0}; // Z++: bool8
    int32_t centre{0}; // Z++: bool8
    int32_t rightJustify{0}; // Z++: bool8
    int32_t background{0}; // Z++: bool8
    int32_t backgroundOnlyText{0}; // Z++: bool8
    int32_t proportional{0}; // Z++: bool8
    int32_t bIsShadow{0}; // Z++: bool8
    int32_t bFlash{0}; // Z++: bool8
    int32_t bBold{0}; // Z++: bool8
    float alphaFade{0.0f}; // Z++: ℝ
    int32_t backgroundColor{0}; // Z++: CRGBA
    float wrapX{0.0f}; // Z++: ℝ
    float centreSize{0.0f}; // Z++: ℝ
    float rightJustifyWrap{0.0f}; // Z++: ℝ
    int32_t style{0}; // Z++: ℤ
    int32_t bFontHalfTexture{0}; // Z++: bool8
    int64_t bank{0}; // Z++: ℕ
    int32_t dropShadowPosition{0}; // Z++: ℤ
    int32_t dropColor{0}; // Z++: CRGBA
    int32_t bFlashState{0}; // Z++: bool8
    int32_t nFlashTimer{0}; // Z++: ℤ
    int32_t anonymous_23{0}; // Z++: bool8
    int64_t anonymous_25{0}; // Z++: ℕ
    int32_t outlineColor{0}; // Z++: CRGBA
    int32_t bOutlineOn{0}; // Z++: ℤ
    int32_t line{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (bank >= 0)
            && (anonymous_25 >= 0);
    }
};
// State Schema Contract from zpp/renderer/Font.zpp
struct CFontRenderState_ZppContract {
    int64_t anonymous_0{0}; // Z++: ℕ
    float fTextPosX{0.0f}; // Z++: ℝ
    float fTextPosY{0.0f}; // Z++: ℝ
    float scaleX{0.0f}; // Z++: ℝ
    float scaleY{0.0f}; // Z++: ℝ
    int32_t color{0}; // Z++: CRGBA
    float fExtraSpace{0.0f}; // Z++: ℝ
    float slant{0.0f}; // Z++: ℝ
    float slantRefX{0.0f}; // Z++: ℝ
    float slantRefY{0.0f}; // Z++: ℝ
    int32_t bIsShadow{0}; // Z++: bool8
    int32_t bFontHalfTexture{0}; // Z++: bool8
    int32_t proportional{0}; // Z++: bool8
    int32_t anonymous_14{0}; // Z++: bool8
    int32_t style{0}; // Z++: ℤ
    int32_t bOutlineOn{0}; // Z++: ℤ
    int32_t line{0}; // Z++: ℤ
    int32_t rightJustify{0}; // Z++: bool8
    constexpr bool checkInvariant() const {
        return (anonymous_0 >= 0);
    }
};
// State Schema Contract from zpp/renderer/Glass.zpp
struct CFallingGlassPane_ZppContract {
    uint8_t m_attachment_card{0u}; // Z++: Ref[RwMatrix]
    float fz{0.0f}; // Z++: ℝ
    bool m_hasRwMatrix{false}; // Z++: 𝔹
    float uw{0.0f}; // Z++: ℝ
    float pw{0.0f}; // Z++: ℝ
    float fw{0.0f}; // Z++: ℝ
    ZppVec3 m_vecMoveSpeed{}; // Z++: CVector
    ZppVec3 m_vecTurn{}; // Z++: CVector
    int64_t m_nTimer{0}; // Z++: ℕ
    float m_fGroundZ{0.0f}; // Z++: ℝ
    float m_fStep{0.0f}; // Z++: ℝ
    int64_t m_nTriIndex{0}; // Z++: ℕ
    bool m_bActive{false}; // Z++: 𝔹
    bool m_bShattered{false}; // Z++: 𝔹
    bool m_bCarGlass{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_attachment_card <= 1u)
            && (m_nTimer >= 0)
            && (m_nTriIndex >= 0);
    }
};
// State Schema Contract from zpp/renderer/Instance.zpp
struct CInstance_ZppContract {
    int32_t m_matrix{0}; // Z++: CMatrix
    int32_t m_modelIndex{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Lines.zpp
struct CLines_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/MBlur.zpp
struct CMBlur_ZppContract {
    uint8_t pFrontBuffer_card{0u}; // Z++: Ref[RwRaster]
    bool ms_bJustInitialised{false}; // Z++: 𝔹
    bool ms_bScaledBlur{false}; // Z++: 𝔹
    bool BlurOn{false}; // Z++: 𝔹
    float Drunkness{0.0f}; // Z++: ℝ
    int32_t pBufVertCount{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (pFrontBuffer_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct ActiveOccluderLine_ZppContract {
    int32_t origin{0}; // Z++: CVector2D
    int32_t direction{0}; // Z++: CVector2D
    float length{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct CActiveOccluder_ZppContract {
    std::array<int32_t, 6> lines{}; // Z++: seq ActiveOccluderLine  -- bounded array [6]
    size_t lines_len{6u}; // Z++ cardinality #lines
    int32_t linesCount{0}; // Z++: ℤ
    float radius{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (lines_len == 6u && lines.size() == 6u);
    }
};
// State Schema Contract from zpp/renderer/Occlusion.zpp
struct COccluder_ZppContract {
    int32_t height{0}; // Z++: ℤ
    int32_t z{0}; // Z++: ℤ
    int64_t angle{0}; // Z++: ℕ
    int32_t listIndex{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (angle >= 0);
    }
};
// State Schema Contract from zpp/renderer/Particle.zpp
struct CParticle_ZppContract {
    ZppVec3 m_vecPosition{}; // Z++: CVector
    ZppVec3 m_vecVelocity{}; // Z++: CVector
    int64_t m_nTimeWhenWillBeDestroyed{0}; // Z++: ℕ
    int64_t m_nTimeWhenColorWillBeChanged{0}; // Z++: ℕ
    float m_fZGround{0.0f}; // Z++: ℝ
    ZppVec3 m_vecParticleMovementOffset{}; // Z++: CVector
    int32_t m_nCurrentZRotation{0}; // Z++: ℤ
    int64_t m_nZRotationTimer{0}; // Z++: ℕ
    float m_fCurrentZRadius{0.0f}; // Z++: ℝ
    int64_t m_nZRadiusTimer{0}; // Z++: ℕ
    int64_t m_nColorIntensity{0}; // Z++: ℕ
    int64_t m_nAlpha{0}; // Z++: ℕ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fExpansionRate{0.0f}; // Z++: ℝ
    int32_t m_nFadeToBlackTimer{0}; // Z++: ℤ
    int32_t m_nFadeAlphaTimer{0}; // Z++: ℤ
    int32_t m_nAnimationSpeedTimer{0}; // Z++: ℤ
    int32_t m_nRotationStep{0}; // Z++: ℤ
    int32_t m_nRotation{0}; // Z++: ℤ
    int64_t m_nCurrentFrame{0}; // Z++: ℕ
    int32_t m_Color{0}; // Z++: RwRGBA
    uint8_t m_pNext_card{0u}; // Z++: Ref[CParticle]
    uint8_t m_pUnusedListHead_card{0u}; // Z++: Ref[CParticle]
    std::array<float, 1024> m_SinTable{}; // Z++: seq ℝ  -- bounded array [SIN_COS_TABLE_SIZE]
    size_t m_SinTable_len{1024u}; // Z++ cardinality #m_SinTable
    std::array<float, 1024> m_CosTable{}; // Z++: seq ℝ  -- bounded array [SIN_COS_TABLE_SIZE]
    size_t m_CosTable_len{1024u}; // Z++ cardinality #m_CosTable
    int32_t field_4C{0}; // Z++: ℤ
    ZppVec3 m_vecScreenPosition{}; // Z++: CVector
    constexpr bool checkInvariant() const {
        return (m_SinTable_len == 1024u && m_SinTable.size() == 1024u)
            && (m_CosTable_len == 1024u && m_CosTable.size() == 1024u)
            && (m_nTimeWhenWillBeDestroyed >= 0)
            && (m_nTimeWhenColorWillBeChanged >= 0)
            && (m_nZRotationTimer >= 0)
            && (m_nZRadiusTimer >= 0)
            && (m_nColorIntensity >= 0)
            && (m_nAlpha >= 0)
            && (m_nCurrentFrame >= 0)
            && (m_pNext_card <= 1u)
            && (m_pUnusedListHead_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ParticleMgr.zpp
struct cParticleSystemMgr_ZppContract {
    uint8_t m_aParticles_card{0u}; // Z++: Ref[tParticleSystemData]
    constexpr bool checkInvariant() const {
        return (m_aParticles_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ParticleType.zpp
struct ParticleTypeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{98}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 98);
    }
};
// State Schema Contract from zpp/renderer/PlayerSkin.zpp
struct CPlayerSkin_ZppContract {
    int32_t m_txdSlot{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/PointLights.zpp
struct CRegisteredPointLight_ZppContract {
    ZppVec3 coors{}; // Z++: CVector
    ZppVec3 dir{}; // Z++: CVector
    float radius{0.0f}; // Z++: ℝ
    float red{0.0f}; // Z++: ℝ
    float green{0.0f}; // Z++: ℝ
    float blue{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int32_t fogType{0}; // Z++: ℤ
    bool castExtraShadows{false}; // Z++: 𝔹
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/RenderBuffer.zpp
struct RenderBuffer_ZppContract {
    int32_t VerticesToBeStored{0}; // Z++: ℤ
    int32_t IndicesToBeStored{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/RenderBuffer.zpp
struct VertexBufferUnion_ZppContract {
    std::array<int32_t, 512> im2d{}; // Z++: seq RwIm2DVertex  -- bounded array [TEMPBUFFERVERTSIZE]
    size_t im2d_len{512u}; // Z++ cardinality #im2d
    std::array<int32_t, 512> im3d{}; // Z++: seq RwIm3DVertex  -- bounded array [TEMPBUFFERVERTSIZE]
    size_t im3d_len{512u}; // Z++ cardinality #im3d
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (im2d_len == 512u && im2d.size() == 512u)
            && (im3d_len == 512u && im3d.size() == 512u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Renderer.zpp
struct EntityInfo_ZppContract {
    uint8_t ent_card{0u}; // Z++: Ref[CEntity]
    float sort{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (ent_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Renderer.zpp
struct BlockedRange_ZppContract {
    float b{0.0f}; // Z++: ℝ
    uint8_t next_card{0u}; // Z++: Ref[BlockedRange_prev]
    constexpr bool checkInvariant() const {
        return (next_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Rubbish.zpp
struct COneSheet_ZppContract {
    ZppVec3 m_basePos{}; // Z++: CVector
    ZppVec3 m_animatedPos{}; // Z++: CVector
    float m_targetZ{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    int32_t m_animationType{0}; // Z++: ℤ
    int64_t m_moveStart{0}; // Z++: ℕ
    int64_t m_moveDuration{0}; // Z++: ℕ
    float m_animHeight{0.0f}; // Z++: ℝ
    float m_xDist{0.0f}; // Z++: ℝ
    float m_yDist{0.0f}; // Z++: ℝ
    float m_angle{0.0f}; // Z++: ℝ
    bool m_isVisible{false}; // Z++: 𝔹
    bool m_targetIsVisible{false}; // Z++: 𝔹
    uint8_t m_next_card{0u}; // Z++: Ref[COneSheet]
    uint8_t m_prev_card{0u}; // Z++: Ref[COneSheet]
    constexpr bool checkInvariant() const {
        return (m_moveStart >= 0)
            && (m_moveDuration >= 0)
            && (m_next_card <= 1u)
            && (m_prev_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/ShadowCamera.zpp
struct CShadowCamera_ZppContract {
    uint8_t m_pCamera_card{0u}; // Z++: Ref[RwCamera]
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    constexpr bool checkInvariant() const {
        return (m_pCamera_card <= 1u)
            && (m_pTexture_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CStoredShadow_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    uint8_t m_pCutsceneShadow_card{0u}; // Z++: Ref[CCutsceneShadow]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_ShadowType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    uint32_t bRendered{0u}; // Z++: 𝔹
    uint32_t bDrawOnBuildings{0u}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_pTexture_card <= 1u)
            && (m_pCutsceneShadow_card <= 1u)
            && (m_ShadowType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0)
            && (bRendered <= 1u)
            && (bDrawOnBuildings <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CPolyBunch_ZppContract {
    std::array<ZppVec3, 7> m_aVerts{}; // Z++: seq CVector  -- bounded array [7]
    size_t m_aVerts_len{7u}; // Z++ cardinality #m_aVerts
    uint8_t m_pNext_card{0u}; // Z++: Ref[CPolyBunch]
    int32_t m_nNumVerts{0}; // Z++: ℤ
    std::array<uint32_t, 7> m_aU{}; // Z++: seq ℕ  -- bounded array [7]
    size_t m_aU_len{7u}; // Z++ cardinality #m_aU
    std::array<uint32_t, 7> m_aV{}; // Z++: seq ℕ  -- bounded array [7]
    size_t m_aV_len{7u}; // Z++ cardinality #m_aV
    constexpr bool checkInvariant() const {
        return (m_aVerts_len == 7u && m_aVerts.size() == 7u)
            && (m_aU_len == 7u && m_aU.size() == 7u)
            && (m_aV_len == 7u && m_aV.size() == 7u)
            && (m_pNext_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CStaticShadow_ZppContract {
    int64_t m_nId{0}; // Z++: ℕ
    uint8_t m_pPolyBunch_card{0u}; // Z++: Ref[CPolyBunch]
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    ZppVec3 m_vecPosn{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_nType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    bool m_bJustCreated{false}; // Z++: 𝔹
    bool m_bRendered{false}; // Z++: 𝔹
    bool m_bTemp{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nId >= 0)
            && (m_pPolyBunch_card <= 1u)
            && (m_nTimeCreated >= 0)
            && (m_pTexture_card <= 1u)
            && (m_nType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CPermanentShadow_ZppContract {
    ZppVec3 m_vecPos{}; // Z++: CVector
    int32_t m_vecFront{0}; // Z++: CVector2D
    int32_t m_vecSide{0}; // Z++: CVector2D
    float m_fZDistance{0.0f}; // Z++: ℝ
    float m_fScale{0.0f}; // Z++: ℝ
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    int64_t m_nLifeTime{0}; // Z++: ℕ
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t m_nIntensity{0}; // Z++: ℤ
    int64_t m_nType{0}; // Z++: ℕ
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nTimeCreated >= 0)
            && (m_nLifeTime >= 0)
            && (m_pTexture_card <= 1u)
            && (m_nType >= 0)
            && (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0);
    }
};
// State Schema Contract from zpp/renderer/Shadows.zpp
struct CShadows_ZppContract {
    int32_t ShadowsStoredToBeRendered{0}; // Z++: ℤ
    uint8_t pEmptyBunchList_card{0u}; // Z++: Ref[CPolyBunch]
    std::array<int32_t, 48> aPermanentShadows{}; // Z++: seq CPermanentShadow  -- bounded array [MAX_PERMAMENTSHADOWS]
    size_t aPermanentShadows_len{48u}; // Z++ cardinality #aPermanentShadows
    constexpr bool checkInvariant() const {
        return (aPermanentShadows_len == 48u && aPermanentShadows.size() == 48u)
            && (pEmptyBunchList_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Skidmarks.zpp
struct CSkidmark_ZppContract {
    std::array<ZppVec3, 16> m_pos{}; // Z++: seq CVector  -- bounded array [SKIDMARK_LENGTH]
    size_t m_pos_len{16u}; // Z++ cardinality #m_pos
    std::array<float, 16> m_sideX{}; // Z++: seq ℝ  -- bounded array [SKIDMARK_LENGTH]
    size_t m_sideX_len{16u}; // Z++ cardinality #m_sideX
    std::array<float, 16> m_sideY{}; // Z++: seq ℝ  -- bounded array [SKIDMARK_LENGTH]
    size_t m_sideY_len{16u}; // Z++ cardinality #m_sideY
    int64_t m_id{0}; // Z++: ℕ
    int64_t m_lastUpdate{0}; // Z++: ℕ
    int64_t m_fadeStart{0}; // Z++: ℕ
    int64_t m_fadeEnd{0}; // Z++: ℕ
    int64_t m_type{0}; // Z++: ℕ
    int32_t m_last{0}; // Z++: ℤ
    int64_t m_state{0}; // Z++: ℕ
    bool m_wasUpdated{false}; // Z++: 𝔹
    bool m_isBloody{false}; // Z++: 𝔹
    bool m_isMuddy{false}; // Z++: 𝔹
    std::array<ZppVec3, 16> m_side{}; // Z++: seq CVector  -- bounded array [SKIDMARK_LENGTH]
    size_t m_side_len{16u}; // Z++ cardinality #m_side
    constexpr bool checkInvariant() const {
        return (m_pos_len == 16u && m_pos.size() == 16u)
            && (m_sideX_len == 16u && m_sideX.size() == 16u)
            && (m_sideY_len == 16u && m_sideY.size() == 16u)
            && (m_side_len == 16u && m_side.size() == 16u)
            && (m_id >= 0)
            && (m_lastUpdate >= 0)
            && (m_fadeStart >= 0)
            && (m_fadeEnd >= 0)
            && (m_type >= 0)
            && (m_state >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CSpecialFX_ZppContract {
    bool bVideoCam{false}; // Z++: 𝔹
    bool bLiftCam{false}; // Z++: 𝔹
    bool bSnapShotActive{false}; // Z++: 𝔹
    int32_t SnapShotFrames{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CRegisteredMotionBlurStreak_ZppContract {
    int64_t m_id{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    std::array<ZppVec3, 3> m_pos1{}; // Z++: seq CVector  -- bounded array [3]
    size_t m_pos1_len{3u}; // Z++ cardinality #m_pos1
    std::array<ZppVec3, 3> m_pos2{}; // Z++: seq CVector  -- bounded array [3]
    size_t m_pos2_len{3u}; // Z++ cardinality #m_pos2
    std::array<bool, 3> m_isValid{}; // Z++: seq 𝔹  -- bounded array [3]
    size_t m_isValid_len{3u}; // Z++ cardinality #m_isValid
    constexpr bool checkInvariant() const {
        return (m_pos1_len == 3u && m_pos1.size() == 3u)
            && (m_pos2_len == 3u && m_pos2.size() == 3u)
            && (m_isValid_len == 3u && m_isValid.size() == 3u)
            && (m_id >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CBulletTrace_ZppContract {
    ZppVec3 m_vecStartPos{}; // Z++: CVector
    ZppVec3 m_vecEndPos{}; // Z++: CVector
    bool m_bInUse{false}; // Z++: 𝔹
    int64_t m_nCreationTime{0}; // Z++: ℕ
    int64_t m_nLifeTime{0}; // Z++: ℕ
    float m_fThickness{0.0f}; // Z++: ℝ
    int64_t m_fVisibility{0}; // Z++: ℕ
    ZppVec3 m_vecCurrentPos{}; // Z++: CVector
    ZppVec3 m_vecTargetPos{}; // Z++: CVector
    int64_t m_framesInUse{0}; // Z++: ℕ
    int64_t m_lifeTime{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_nCreationTime >= 0)
            && (m_nLifeTime >= 0)
            && (m_fVisibility >= 0)
            && (m_framesInUse >= 0)
            && (m_lifeTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct C3dMarker_ZppContract {
    int32_t m_Matrix{0}; // Z++: CMatrix
    uint8_t m_pAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t m_pMaterial_card{0u}; // Z++: Ref[RpMaterial]
    int64_t m_nType{0}; // Z++: ℕ
    bool m_bIsUsed{false}; // Z++: 𝔹
    bool m_bFindZOnNextPlacement{false}; // Z++: 𝔹
    int64_t m_nIdentifier{0}; // Z++: ℕ
    int32_t m_Color{0}; // Z++: RwRGBA
    int64_t m_nPulsePeriod{0}; // Z++: ℕ
    int32_t m_nRotateRate{0}; // Z++: ℤ
    int64_t m_nStartTime{0}; // Z++: ℕ
    float m_fPulseFraction{0.0f}; // Z++: ℝ
    float m_fStdSize{0.0f}; // Z++: ℝ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fBrightness{0.0f}; // Z++: ℝ
    float m_fCameraRange{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_pAtomic_card <= 1u)
            && (m_pMaterial_card <= 1u)
            && (m_nType >= 0)
            && (m_nIdentifier >= 0)
            && (m_nPulsePeriod >= 0)
            && (m_nStartTime >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CBrightLight_ZppContract {
    ZppVec3 m_pos{}; // Z++: CVector
    ZppVec3 m_up{}; // Z++: CVector
    ZppVec3 m_side{}; // Z++: CVector
    ZppVec3 m_front{}; // Z++: CVector
    float m_camDist{0.0f}; // Z++: ℝ
    int64_t m_type{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_type >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CShinyText_ZppContract {
    std::array<ZppVec3, 4> m_verts{}; // Z++: seq CVector  -- bounded array [4]
    size_t m_verts_len{4u}; // Z++ cardinality #m_verts
    std::array<int32_t, 4> m_texCoords{}; // Z++: seq CVector2D  -- bounded array [4]
    size_t m_texCoords_len{4u}; // Z++ cardinality #m_texCoords
    float m_camDist{0.0f}; // Z++: ℝ
    int64_t m_type{0}; // Z++: ℕ
    int64_t m_red{0}; // Z++: ℕ
    int64_t m_green{0}; // Z++: ℕ
    int64_t m_blue{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (m_verts_len == 4u && m_verts.size() == 4u)
            && (m_texCoords_len == 4u && m_texCoords.size() == 4u)
            && (m_type >= 0)
            && (m_red >= 0)
            && (m_green >= 0)
            && (m_blue >= 0);
    }
};
// State Schema Contract from zpp/renderer/SpecialFX.zpp
struct CSpecialParticleStuff_ZppContract {
    int64_t BoatFromStart{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (BoatFromStart >= 0);
    }
};
// State Schema Contract from zpp/renderer/Sprite.zpp
struct CSprite_ZppContract {
    float m_f2DNearScreenZ{0.0f}; // Z++: ℝ
    float m_f2DFarScreenZ{0.0f}; // Z++: ℝ
    float m_fRecipNearClipPlane{0.0f}; // Z++: ℝ
    int32_t m_bFlushSpriteBufferSwitchZTest{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/Sprite2d.zpp
struct CSprite2d_ZppContract {
    float RecipNearClip{0.0f}; // Z++: ℝ
    float NearScreenZ{0.0f}; // Z++: ℝ
    float NearCamZ{0.0f}; // Z++: ℝ
    int32_t nextBufferVertex{0}; // Z++: ℤ
    int32_t nextBufferIndex{0}; // Z++: ℤ
    std::array<int32_t, 8> maVertices{}; // Z++: seq RwIm2DVertex  -- bounded array [8]
    size_t maVertices_len{8u}; // Z++ cardinality #maVertices
    uint8_t m_pTexture_card{0u}; // Z++: Ref[RwTexture]
    int32_t mCurrentBank{0}; // Z++: ℤ
    std::array<int32_t, 10> mpBankTextures{}; // Z++: seq Ref[RwTexture]  -- bounded array [10]
    size_t mpBankTextures_len{10u}; // Z++ cardinality #mpBankTextures
    std::array<int32_t, 10> mCurrentSprite{}; // Z++: seq ℤ  -- bounded array [10]
    size_t mCurrentSprite_len{10u}; // Z++ cardinality #mCurrentSprite
    std::array<int32_t, 10> mBankStart{}; // Z++: seq ℤ  -- bounded array [10]
    size_t mBankStart_len{10u}; // Z++ cardinality #mBankStart
    std::array<int32_t, 500> maBankVertices{}; // Z++: seq RwIm2DVertex  -- bounded array [500]
    size_t maBankVertices_len{500u}; // Z++ cardinality #maBankVertices
    constexpr bool checkInvariant() const {
        return (maVertices_len == 8u && maVertices.size() == 8u)
            && (mpBankTextures_len == 10u && mpBankTextures.size() == 10u)
            && (mCurrentSprite_len == 10u && mCurrentSprite.size() == 10u)
            && (mBankStart_len == 10u && mBankStart.size() == 10u)
            && (maBankVertices_len == 500u && maBankVertices.size() == 500u)
            && (m_pTexture_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/TexList.zpp
struct CTexList_ZppContract {
    std::array<bool, 400> ms_nTexUsed{}; // Z++: seq 𝔹  -- bounded array [MAX_TEXUSED]
    size_t ms_nTexUsed_len{400u}; // Z++ cardinality #ms_nTexUsed
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (ms_nTexUsed_len == 400u && ms_nTexUsed.size() == 400u)
            && (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/VarConsole.zpp
struct CVarConsoleEntry_ZppContract {
    int32_t text{0}; // Z++: seq CHAR
    uint8_t pInt8Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt16Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt32Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pInt64Value_card{0u}; // Z++: ℙ ℤ
    uint8_t pUint8Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint16Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint32Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pUint64Value_card{0u}; // Z++: ℙ ℕ
    uint8_t pFloatValue_card{0u}; // Z++: ℙ ℝ
    uint8_t pBoolValue_card{0u}; // Z++: ℙ 𝔹
    int32_t I8_min{0}; // Z++: ℤ
    int32_t I16_min{0}; // Z++: ℤ
    int32_t I32_min{0}; // Z++: ℤ
    int32_t I64_min{0}; // Z++: ℤ
    float F_min{0.0f}; // Z++: ℝ
    int32_t bAllowExceedBounds{0}; // Z++: bool8
    int64_t VarType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (pInt8Value_card <= 1u)
            && (pInt16Value_card <= 1u)
            && (pInt32Value_card <= 1u)
            && (pInt64Value_card <= 1u)
            && (pUint8Value_card <= 1u)
            && (pUint16Value_card <= 1u)
            && (pUint32Value_card <= 1u)
            && (pUint64Value_card <= 1u)
            && (pFloatValue_card <= 1u)
            && (pBoolValue_card <= 1u)
            && (VarType >= 0);
    }
};
// State Schema Contract from zpp/renderer/VarConsole.zpp
struct CVarConsole_ZppContract {
    int32_t m_nCountEntries{0}; // Z++: ℤ
    int32_t m_bIsOpen{0}; // Z++: bool8
    int32_t m_nCurEntry{0}; // Z++: ℤ
    int32_t m_nFirstEntryOnPage{0}; // Z++: ℤ
    int32_t m_nCurPage{0}; // Z++: ℤ
    int32_t m_nNumPages{0}; // Z++: ℤ
    std::array<int32_t, 91> m_aEntries{}; // Z++: seq CVarConsoleEntry  -- bounded array [91]
    size_t m_aEntries_len{91u}; // Z++ cardinality #m_aEntries
    constexpr bool checkInvariant() const {
        return (m_aEntries_len == 91u && m_aEntries.size() == 91u);
    }
};
// State Schema Contract from zpp/renderer/WaterCannon.zpp
struct CWaterCannon_ZppContract {
    int32_t m_nId{0}; // Z++: ℤ
    int32_t m_nCur{0}; // Z++: ℤ
    int64_t m_nTimeCreated{0}; // Z++: ℕ
    int32_t field_C{0}; // Z++: ℤ
    std::array<ZppVec3, 16> m_avecPos{}; // Z++: seq CVector  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_avecPos_len{16u}; // Z++ cardinality #m_avecPos
    std::array<ZppVec3, 16> m_avecVelocity{}; // Z++: seq CVector  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_avecVelocity_len{16u}; // Z++ cardinality #m_avecVelocity
    std::array<bool, 16> m_abUsed{}; // Z++: seq 𝔹  -- bounded array [NUM_SEGMENTPOINTS]
    size_t m_abUsed_len{16u}; // Z++ cardinality #m_abUsed
    constexpr bool checkInvariant() const {
        return (m_avecPos_len == 16u && m_avecPos.size() == 16u)
            && (m_avecVelocity_len == 16u && m_avecVelocity.size() == 16u)
            && (m_abUsed_len == 16u && m_abUsed.size() == 16u)
            && (m_nTimeCreated >= 0);
    }
};
// State Schema Contract from zpp/renderer/WaterCreatures.zpp
struct CWaterCreature_ZppContract {
    uint8_t m_pObj_card{0u}; // Z++: Ref[CObject]
    float m_fFwdSpeed{0.0f}; // Z++: ℝ
    float m_fZTurnSpeed{0.0f}; // Z++: ℝ
    int32_t m_alpha{0}; // Z++: ℤ
    float m_fWaterDepth{0.0f}; // Z++: ℝ
    int32_t m_state{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pObj_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/WaterCreatures.zpp
struct WaterCreatureProperties_ZppContract {
    uint8_t modelID_card{0u}; // Z++: ℙ ℤ
    float fFwdSpeed{0.0f}; // Z++: ℝ
    float fLevel{0.0f}; // Z++: ℝ
    float fUnknown{0.0f}; // Z++: ℝ
    float fWaterDepth{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (modelID_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/WaterLevel.zpp
struct ColData_ZppContract {
    int64_t SurfaceType{0}; // Z++: ℕ
    int64_t PieceType{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (SurfaceType >= 0)
            && (PieceType >= 0);
    }
};
// State Schema Contract from zpp/renderer/WaterLevel.zpp
struct CWaterLevel_ZppContract {
    int32_t ms_nNoOfWaterLevels{0}; // Z++: ℤ
    std::array<float, 48> ms_aWaterZs{}; // Z++: seq ℝ  -- bounded array [48]
    size_t ms_aWaterZs_len{48u}; // Z++ cardinality #ms_aWaterZs
    std::array<int32_t, 48> ms_aWaterRects{}; // Z++: seq CRect  -- bounded array [48]
    size_t ms_aWaterRects_len{48u}; // Z++ cardinality #ms_aWaterRects
    bool WavesCalculatedThisFrame{false}; // Z++: 𝔹
    bool RequireWavySector{false}; // Z++: 𝔹
    bool MaskCalculatedThisFrame{false}; // Z++: 𝔹
    ZppVec3 PreCalculatedMaskPosn{}; // Z++: CVector
    bool m_bRenderSeaBed{false}; // Z++: 𝔹
    int32_t m_nRenderWaterLayers{0}; // Z++: ℤ
    uint8_t ms_pWavyAtomic_card{0u}; // Z++: Ref[RpAtomic]
    uint8_t ms_pMaskAtomic_card{0u}; // Z++: Ref[RpAtomic]
    std::array<int32_t, 8> apGeomArray{}; // Z++: seq Ref[RpGeometry]  -- bounded array [MAX_BOAT_WAKES]
    size_t apGeomArray_len{8u}; // Z++ cardinality #apGeomArray
    int32_t nGeomUsed{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (ms_aWaterZs_len == 48u && ms_aWaterZs.size() == 48u)
            && (ms_aWaterRects_len == 48u && ms_aWaterRects.size() == 48u)
            && (apGeomArray_len == 8u && apGeomArray.size() == 8u)
            && (ms_pWavyAtomic_card <= 1u)
            && (ms_pMaskAtomic_card <= 1u);
    }
};
// State Schema Contract from zpp/renderer/Weather.zpp
struct CWeather_ZppContract {
    int32_t SoundHandle{0}; // Z++: ℤ
    int32_t WeatherTypeInList{0}; // Z++: ℤ
    int32_t OldWeatherType{0}; // Z++: ℤ
    int32_t NewWeatherType{0}; // Z++: ℤ
    int32_t ForcedWeatherType{0}; // Z++: ℤ
    bool LightningFlash{false}; // Z++: 𝔹
    bool LightningBurst{false}; // Z++: 𝔹
    int64_t LightningStart{0}; // Z++: ℕ
    int64_t LightningFlashLastChange{0}; // Z++: ℕ
    int64_t WhenToPlayLightningSound{0}; // Z++: ℕ
    int64_t LightningDuration{0}; // Z++: ℕ
    int32_t StreamAfterRainTimer{0}; // Z++: ℤ
    float ExtraSunnyness{0.0f}; // Z++: ℝ
    float Foggyness{0.0f}; // Z++: ℝ
    float CloudCoverage{0.0f}; // Z++: ℝ
    float Wind{0.0f}; // Z++: ℝ
    float Rain{0.0f}; // Z++: ℝ
    float InterpolationValue{0.0f}; // Z++: ℝ
    float WetRoads{0.0f}; // Z++: ℝ
    float Rainbow{0.0f}; // Z++: ℝ
    float SunGlare{0.0f}; // Z++: ℝ
    float WindClipped{0.0f}; // Z++: ℝ
    float TrafficLightBrightness{0.0f}; // Z++: ℝ
    bool bScriptsForceRain{false}; // Z++: 𝔹
    bool Stored_StateStored{false}; // Z++: 𝔹
    float Stored_InterpolationValue{0.0f}; // Z++: ℝ
    int32_t Stored_OldWeatherType{0}; // Z++: ℤ
    int32_t Stored_NewWeatherType{0}; // Z++: ℤ
    float Stored_Rain{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (LightningStart >= 0)
            && (LightningFlashLastChange >= 0)
            && (WhenToPlayLightningSound >= 0)
            && (LightningDuration >= 0);
    }
};
// State Schema Contract from zpp/renderer/Weather.zpp
struct tRainStreak_ZppContract {
    ZppVec3 position{}; // Z++: CVector
    ZppVec3 direction{}; // Z++: CVector
    int64_t timer{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (timer >= 0);
    }
};
// State Schema Contract from zpp/renderer/WindModifiers.zpp
struct CWindModifiers_ZppContract {
    ZppVec3 m_pos{}; // Z++: CVector
    int32_t m_type{0}; // Z++: ℤ
    int32_t Number{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Query Operation Schema Contract (ΞCEscalator) from zpp/renderer/Fluff.zpp
constexpr bool op_CEscalator_IsActive_5(const CEscalator_ZppContract& state) {
    return state.checkInvariant();
}
// Query Operation Schema Contract (ΞCEscalator) from zpp/renderer/Fluff.zpp
constexpr bool op_CEscalator_GetPosition_6(const CEscalator_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCClouds) from zpp/renderer/Clouds.zpp
constexpr bool op_CClouds_Init(const CClouds_ZppContract& pre, const CClouds_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.CloudRotation == 0.0f);
}
constexpr CClouds_ZppContract apply_CClouds_Init(const CClouds_ZppContract& pre) {
    CClouds_ZppContract post = pre;
    post.CloudRotation = 0.0f;
    return post;
}
// Delta Operation Schema Contract (ΔCCredits) from zpp/renderer/Credits.zpp
constexpr bool op_CCredits_Start(const CCredits_ZppContract& pre, const CCredits_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == true) && (post.CreditsStartTime == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CCredits_ZppContract apply_CCredits_Start(const CCredits_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = true;
    post.CreditsStartTime = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCCredits) from zpp/renderer/Credits.zpp
constexpr bool op_CCredits_Stop(const CCredits_ZppContract& pre, const CCredits_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == false);
}
constexpr CCredits_ZppContract apply_CCredits_Stop(const CCredits_ZppContract& pre) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = false;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_MotionBlurClose(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.pFrontBuffer_card == 0u);
}
constexpr CMBlur_ZppContract apply_CMBlur_MotionBlurClose(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.pFrontBuffer_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_MotionBlurRender(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_bJustInitialised == false);
}
constexpr CMBlur_ZppContract apply_CMBlur_MotionBlurRender(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.ms_bJustInitialised = false;
    return post;
}
// Delta Operation Schema Contract (ΔCMBlur) from zpp/renderer/MBlur.zpp
constexpr bool op_CMBlur_ClearDrunkBlur(const CMBlur_ZppContract& pre, const CMBlur_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Drunkness == 0.0f);
}
constexpr CMBlur_ZppContract apply_CMBlur_ClearDrunkBlur(const CMBlur_ZppContract& pre) {
    CMBlur_ZppContract post = pre;
    post.Drunkness = 0.0f;
    return post;
}
// Delta Operation Schema Contract (ΔcParticleSystemMgr) from zpp/renderer/ParticleMgr.zpp
constexpr bool op_cParticleSystemMgr_cParticleSystemMgr(const cParticleSystemMgr_ZppContract& pre, const cParticleSystemMgr_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_aParticles_card == 0u);
}
constexpr cParticleSystemMgr_ZppContract apply_cParticleSystemMgr_cParticleSystemMgr(const cParticleSystemMgr_ZppContract& pre) {
    cParticleSystemMgr_ZppContract post = pre;
    post.m_aParticles_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCShadowCamera) from zpp/renderer/ShadowCamera.zpp
constexpr bool op_CShadowCamera_CShadowCamera(const CShadowCamera_ZppContract& pre, const CShadowCamera_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pCamera_card == 0u) && (post.m_pTexture_card == 0u);
}
constexpr CShadowCamera_ZppContract apply_CShadowCamera_CShadowCamera(const CShadowCamera_ZppContract& pre) {
    CShadowCamera_ZppContract post = pre;
    post.m_pCamera_card = 0u;
    post.m_pTexture_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCSpecialParticleStuff) from zpp/renderer/SpecialFX.zpp
constexpr bool op_CSpecialParticleStuff_StartBoatFoamAnimation(const CSpecialParticleStuff_ZppContract& pre, const CSpecialParticleStuff_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.BoatFromStart == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CSpecialParticleStuff_ZppContract apply_CSpecialParticleStuff_StartBoatFoamAnimation(const CSpecialParticleStuff_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CSpecialParticleStuff_ZppContract post = pre;
    post.BoatFromStart = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite) from zpp/renderer/Sprite.zpp
constexpr bool op_CSprite_InitSpriteBuffer(const CSprite_ZppContract& pre, const CSprite_ZppContract& post, float RwIm2DGetNearScreenZ_in = 2.5f, float RwIm2DGetFarScreenZ_in = 2.5f) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_f2DNearScreenZ == RwIm2DGetNearScreenZ_in) && (post.m_f2DFarScreenZ == RwIm2DGetFarScreenZ_in);
}
constexpr CSprite_ZppContract apply_CSprite_InitSpriteBuffer(const CSprite_ZppContract& pre, float RwIm2DGetNearScreenZ_in = 2.5f, float RwIm2DGetFarScreenZ_in = 2.5f) {
    CSprite_ZppContract post = pre;
    post.m_f2DNearScreenZ = RwIm2DGetNearScreenZ_in;
    post.m_f2DFarScreenZ = RwIm2DGetFarScreenZ_in;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite) from zpp/renderer/Sprite.zpp
constexpr bool op_CSprite_RenderBufferedOneXLUSprite2D(const CSprite_ZppContract& pre, const CSprite_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bFlushSpriteBufferSwitchZTest == 1);
}
constexpr CSprite_ZppContract apply_CSprite_RenderBufferedOneXLUSprite2D(const CSprite_ZppContract& pre) {
    CSprite_ZppContract post = pre;
    post.m_bFlushSpriteBufferSwitchZTest = 1;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite2d) from zpp/renderer/Sprite2d.zpp
constexpr bool op_CSprite2d_Delete(const CSprite2d_ZppContract& pre, const CSprite2d_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pTexture_card == 0u);
}
constexpr CSprite2d_ZppContract apply_CSprite2d_Delete(const CSprite2d_ZppContract& pre) {
    CSprite2d_ZppContract post = pre;
    post.m_pTexture_card = 0u;
    return post;
}
// Delta Operation Schema Contract (ΔCSprite2d) from zpp/renderer/Sprite2d.zpp
constexpr bool op_CSprite2d_RenderVertexBuffer(const CSprite2d_ZppContract& pre, const CSprite2d_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.nextBufferVertex == 0) && (post.nextBufferIndex == 0);
}
constexpr CSprite2d_ZppContract apply_CSprite2d_RenderVertexBuffer(const CSprite2d_ZppContract& pre) {
    CSprite2d_ZppContract post = pre;
    post.nextBufferVertex = 0;
    post.nextBufferIndex = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCWaterCannon) from zpp/renderer/WaterCannon.zpp
constexpr bool op_CWaterCannon_Init(const CWaterCannon_ZppContract& pre, const CWaterCannon_ZppContract& post, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nId == 0) && (post.m_nCur == 0) && (post.m_nTimeCreated == (int64_t)CTimer__GetTimeInMilliseconds_in);
}
constexpr CWaterCannon_ZppContract apply_CWaterCannon_Init(const CWaterCannon_ZppContract& pre, int64_t CTimer__GetTimeInMilliseconds_in = 25) {
    CWaterCannon_ZppContract post = pre;
    post.m_nId = 0;
    post.m_nCur = 0;
    post.m_nTimeCreated = (int64_t)CTimer__GetTimeInMilliseconds_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWaterLevel) from zpp/renderer/WaterLevel.zpp
constexpr bool op_CWaterLevel_FreeBoatWakeArray(const CWaterLevel_ZppContract& pre, const CWaterLevel_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.nGeomUsed == 0);
}
constexpr CWaterLevel_ZppContract apply_CWaterLevel_FreeBoatWakeArray(const CWaterLevel_ZppContract& pre) {
    CWaterLevel_ZppContract post = pre;
    post.nGeomUsed = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceWeather(const CWeather_ZppContract& pre, const CWeather_ZppContract& post, int32_t weather_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ForcedWeatherType == (int32_t)weather_in);
}
constexpr CWeather_ZppContract apply_CWeather_ForceWeather(const CWeather_ZppContract& pre, int32_t weather_in = 25) {
    CWeather_ZppContract post = pre;
    post.ForcedWeatherType = (int32_t)weather_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceWeatherNow(const CWeather_ZppContract& pre, const CWeather_ZppContract& post, int32_t weather_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.OldWeatherType == (int32_t)weather_in) && (post.NewWeatherType == (int32_t)weather_in) && (post.ForcedWeatherType == (int32_t)weather_in);
}
constexpr CWeather_ZppContract apply_CWeather_ForceWeatherNow(const CWeather_ZppContract& pre, int32_t weather_in = 25) {
    CWeather_ZppContract post = pre;
    post.OldWeatherType = (int32_t)weather_in;
    post.NewWeatherType = (int32_t)weather_in;
    post.ForcedWeatherType = (int32_t)weather_in;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_StoreWeatherState(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Stored_StateStored == true) && (post.Stored_InterpolationValue == pre.InterpolationValue) && (post.Stored_Rain == pre.Rain) && (post.Stored_NewWeatherType == pre.NewWeatherType) && (post.Stored_OldWeatherType == pre.OldWeatherType);
}
constexpr CWeather_ZppContract apply_CWeather_StoreWeatherState(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.Stored_StateStored = true;
    post.Stored_InterpolationValue = pre.InterpolationValue;
    post.Stored_Rain = pre.Rain;
    post.Stored_NewWeatherType = pre.NewWeatherType;
    post.Stored_OldWeatherType = pre.OldWeatherType;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_RestoreWeatherState(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.Stored_StateStored == false) && (post.InterpolationValue == pre.Stored_InterpolationValue) && (post.Rain == pre.Stored_Rain) && (post.NewWeatherType == pre.Stored_NewWeatherType) && (post.OldWeatherType == pre.Stored_OldWeatherType);
}
constexpr CWeather_ZppContract apply_CWeather_RestoreWeatherState(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.Stored_StateStored = false;
    post.InterpolationValue = pre.Stored_InterpolationValue;
    post.Rain = pre.Stored_Rain;
    post.NewWeatherType = pre.Stored_NewWeatherType;
    post.OldWeatherType = pre.Stored_OldWeatherType;
    return post;
}
// Delta Operation Schema Contract (ΔCWeather) from zpp/renderer/Weather.zpp
constexpr bool op_CWeather_ForceHurricaneWeather(const CWeather_ZppContract& pre, const CWeather_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.OldWeatherType == 5) && (post.NewWeatherType == 5) && (post.ForcedWeatherType == 5);
}
constexpr CWeather_ZppContract apply_CWeather_ForceHurricaneWeather(const CWeather_ZppContract& pre) {
    CWeather_ZppContract post = pre;
    post.OldWeatherType = 5;
    post.NewWeatherType = 5;
    post.ForcedWeatherType = 5;
    return post;
}
// Delta Operation Schema Contract (ΔCWindModifiers) from zpp/renderer/WindModifiers.zpp
constexpr bool op_CWindModifiers_RegisterOne(const CWindModifiers_ZppContract& pre, const CWindModifiers_ZppContract& post, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, int32_t type_in = 25) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_pos == pos_in) && (post.m_type == (int32_t)type_in);
}
constexpr CWindModifiers_ZppContract apply_CWindModifiers_RegisterOne(const CWindModifiers_ZppContract& pre, ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f}, int32_t type_in = 25) {
    CWindModifiers_ZppContract post = pre;
    post.m_pos = pos_in;
    post.m_type = (int32_t)type_in;
    return post;
}
constexpr bool verify_renderer_zpp_invariants() {
    {
        C2dEffect_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        C2dEffect_ZppContract bad{};
        bad.lightType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Light_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Light_ZppContract bad{};
        bad.lightType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Particle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Particle_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        Attractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Attractor_ZppContract bad{};
        bad.probability = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        PedAttractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PedAttractor_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CAntenna_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAntenna_ZppContract bad{};
        bad.pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CClouds_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CClouds_ZppContract bad{};
        bad.IndividualRotation = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CConsole_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CConsole_ZppContract bad{};
        bad.m_aTimer_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        FlareDef_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        FlareDef_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredCorona_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredCorona_ZppContract bad{};
        bad.prevX_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CCredits_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCredits_ZppContract bad{};
        bad.CreditsStartTime = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CCutsceneShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCutsceneShadow_ZppContract bad{};
        bad.m_pAtomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CDraw_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDraw_ZppContract bad{};
        bad.FadeValue = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CScriptPath_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScriptPath_ZppContract bad{};
        bad.m_pObjects_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CScriptPaths_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScriptPaths_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneTrail_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneTrail_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneTrails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneTrails_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneBanner_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneBanner_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlaneBanners_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlaneBanners_ZppContract bad{};
        bad.aArray_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CEscalator_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CEscalator_ZppContract bad{};
        bad.m_pSteps_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMovingThing_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovingThing_ZppContract bad{};
        bad.m_pNext_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CMovingThings_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovingThings_ZppContract bad{};
        bad.aMovingThings_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CScrollBar_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CScrollBar_ZppContract bad{};
        bad.m_MessageBar_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSmokeTrails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSmokeTrails_ZppContract bad{};
        bad.aSmoke_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTowerClock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTowerClock_ZppContract bad{};
        bad.m_uRed = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CDigitalClock_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CDigitalClock_ZppContract bad{};
        bad.m_uRed = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFontDetails_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFontDetails_ZppContract bad{};
        bad.bank = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFontRenderState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFontRenderState_ZppContract bad{};
        bad.anonymous_0 = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CFallingGlassPane_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CFallingGlassPane_ZppContract bad{};
        bad.m_attachment_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CInstance_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CInstance_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CLines_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CLines_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CMBlur_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMBlur_ZppContract bad{};
        bad.pFrontBuffer_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ActiveOccluderLine_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ActiveOccluderLine_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CActiveOccluder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CActiveOccluder_ZppContract bad{};
        bad.lines_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        COccluder_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        COccluder_ZppContract bad{};
        bad.angle = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CParticle_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CParticle_ZppContract bad{};
        bad.m_SinTable_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        cParticleSystemMgr_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        cParticleSystemMgr_ZppContract bad{};
        bad.m_aParticles_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ParticleTypeModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ParticleTypeModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CPlayerSkin_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPlayerSkin_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredPointLight_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredPointLight_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RenderBuffer_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RenderBuffer_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        VertexBufferUnion_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        VertexBufferUnion_ZppContract bad{};
        bad.im2d_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        EntityInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        EntityInfo_ZppContract bad{};
        bad.ent_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        BlockedRange_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        BlockedRange_ZppContract bad{};
        bad.next_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        COneSheet_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        COneSheet_ZppContract bad{};
        bad.m_moveStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShadowCamera_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShadowCamera_ZppContract bad{};
        bad.m_pCamera_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStoredShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStoredShadow_ZppContract bad{};
        bad.m_pTexture_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPolyBunch_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPolyBunch_ZppContract bad{};
        bad.m_aVerts_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CStaticShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CStaticShadow_ZppContract bad{};
        bad.m_nId = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPermanentShadow_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPermanentShadow_ZppContract bad{};
        bad.m_nTimeCreated = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShadows_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShadows_ZppContract bad{};
        bad.aPermanentShadows_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSkidmark_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSkidmark_ZppContract bad{};
        bad.m_pos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSpecialFX_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSpecialFX_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CRegisteredMotionBlurStreak_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CRegisteredMotionBlurStreak_ZppContract bad{};
        bad.m_pos1_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CBulletTrace_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBulletTrace_ZppContract bad{};
        bad.m_nCreationTime = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        C3dMarker_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        C3dMarker_ZppContract bad{};
        bad.m_pAtomic_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CBrightLight_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBrightLight_ZppContract bad{};
        bad.m_type = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CShinyText_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShinyText_ZppContract bad{};
        bad.m_verts_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CSpecialParticleStuff_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSpecialParticleStuff_ZppContract bad{};
        bad.BoatFromStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSprite_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSprite_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CSprite2d_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CSprite2d_ZppContract bad{};
        bad.maVertices_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CTexList_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CTexList_ZppContract bad{};
        bad.ms_nTexUsed_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVarConsoleEntry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVarConsoleEntry_ZppContract bad{};
        bad.pInt8Value_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CVarConsole_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CVarConsole_ZppContract bad{};
        bad.m_aEntries_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterCannon_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterCannon_ZppContract bad{};
        bad.m_avecPos_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterCreature_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterCreature_ZppContract bad{};
        bad.m_pObj_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        WaterCreatureProperties_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        WaterCreatureProperties_ZppContract bad{};
        bad.modelID_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        ColData_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        ColData_ZppContract bad{};
        bad.SurfaceType = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWaterLevel_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWaterLevel_ZppContract bad{};
        bad.ms_aWaterZs_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWeather_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeather_ZppContract bad{};
        bad.LightningStart = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        tRainStreak_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tRainStreak_ZppContract bad{};
        bad.timer = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWindModifiers_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWindModifiers_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        const CEscalator_ZppContract st{};
        if (!op_CEscalator_IsActive_5(st)) return false;
    }
    {
        const CEscalator_ZppContract st{};
        if (!op_CEscalator_GetPosition_6(st)) return false;
    }
    {
        CClouds_ZppContract pre{};
        pre.CloudRotation = 1.0f;
        const auto post = apply_CClouds_Init(pre);
        if (!op_CClouds_Init(pre, post)) return false;
        {
            CClouds_ZppContract badPost = post;
            badPost.CloudRotation = 1.0f;
            if (op_CClouds_Init(pre, badPost)) return false;
        }
    }
    {
        CCredits_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.bCreditsGoing = false;
        pre.CreditsStartTime = 0;
        const auto post = apply_CCredits_Start(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CCredits_Start(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CCredits_ZppContract badPost = post;
            badPost.bCreditsGoing = false;
            if (op_CCredits_Start(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CCredits_ZppContract badPost = post;
            badPost.CreditsStartTime = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CCredits_Start(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CCredits_ZppContract pre{};
        pre.bCreditsGoing = true;
        const auto post = apply_CCredits_Stop(pre);
        if (!op_CCredits_Stop(pre, post)) return false;
        {
            CCredits_ZppContract badPost = post;
            badPost.bCreditsGoing = true;
            if (op_CCredits_Stop(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.pFrontBuffer_card = 1u;
        const auto post = apply_CMBlur_MotionBlurClose(pre);
        if (!op_CMBlur_MotionBlurClose(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.pFrontBuffer_card = 1u;
            if (op_CMBlur_MotionBlurClose(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.ms_bJustInitialised = true;
        const auto post = apply_CMBlur_MotionBlurRender(pre);
        if (!op_CMBlur_MotionBlurRender(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.ms_bJustInitialised = true;
            if (op_CMBlur_MotionBlurRender(pre, badPost)) return false;
        }
    }
    {
        CMBlur_ZppContract pre{};
        pre.Drunkness = 1.0f;
        const auto post = apply_CMBlur_ClearDrunkBlur(pre);
        if (!op_CMBlur_ClearDrunkBlur(pre, post)) return false;
        {
            CMBlur_ZppContract badPost = post;
            badPost.Drunkness = 1.0f;
            if (op_CMBlur_ClearDrunkBlur(pre, badPost)) return false;
        }
    }
    {
        cParticleSystemMgr_ZppContract pre{};
        pre.m_aParticles_card = 1u;
        const auto post = apply_cParticleSystemMgr_cParticleSystemMgr(pre);
        if (!op_cParticleSystemMgr_cParticleSystemMgr(pre, post)) return false;
        {
            cParticleSystemMgr_ZppContract badPost = post;
            badPost.m_aParticles_card = 1u;
            if (op_cParticleSystemMgr_cParticleSystemMgr(pre, badPost)) return false;
        }
    }
    {
        CShadowCamera_ZppContract pre{};
        pre.m_pCamera_card = 1u;
        pre.m_pTexture_card = 1u;
        const auto post = apply_CShadowCamera_CShadowCamera(pre);
        if (!op_CShadowCamera_CShadowCamera(pre, post)) return false;
        {
            CShadowCamera_ZppContract badPost = post;
            badPost.m_pCamera_card = 1u;
            if (op_CShadowCamera_CShadowCamera(pre, badPost)) return false;
        }
        {
            CShadowCamera_ZppContract badPost = post;
            badPost.m_pTexture_card = 1u;
            if (op_CShadowCamera_CShadowCamera(pre, badPost)) return false;
        }
    }
    {
        CSpecialParticleStuff_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.BoatFromStart = 0;
        const auto post = apply_CSpecialParticleStuff_StartBoatFoamAnimation(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CSpecialParticleStuff_StartBoatFoamAnimation(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CSpecialParticleStuff_ZppContract badPost = post;
            badPost.BoatFromStart = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CSpecialParticleStuff_StartBoatFoamAnimation(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CSprite_ZppContract pre{};
        const float RwIm2DGetNearScreenZ_in = 2.5f;
        const float RwIm2DGetFarScreenZ_in = 2.5f;
        pre.m_f2DNearScreenZ = 0.0f;
        pre.m_f2DFarScreenZ = 0.0f;
        const auto post = apply_CSprite_InitSpriteBuffer(pre, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in);
        if (!op_CSprite_InitSpriteBuffer(pre, post, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        {
            CSprite_ZppContract badPost = post;
            badPost.m_f2DNearScreenZ = RwIm2DGetNearScreenZ_in + 1.0f;
            if (op_CSprite_InitSpriteBuffer(pre, badPost, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        }
        {
            CSprite_ZppContract badPost = post;
            badPost.m_f2DFarScreenZ = RwIm2DGetFarScreenZ_in + 1.0f;
            if (op_CSprite_InitSpriteBuffer(pre, badPost, RwIm2DGetNearScreenZ_in, RwIm2DGetFarScreenZ_in)) return false;
        }
    }
    {
        CSprite_ZppContract pre{};
        pre.m_bFlushSpriteBufferSwitchZTest = 0;
        const auto post = apply_CSprite_RenderBufferedOneXLUSprite2D(pre);
        if (!op_CSprite_RenderBufferedOneXLUSprite2D(pre, post)) return false;
        {
            CSprite_ZppContract badPost = post;
            badPost.m_bFlushSpriteBufferSwitchZTest = 0;
            if (op_CSprite_RenderBufferedOneXLUSprite2D(pre, badPost)) return false;
        }
    }
    {
        CSprite2d_ZppContract pre{};
        pre.m_pTexture_card = 1u;
        const auto post = apply_CSprite2d_Delete(pre);
        if (!op_CSprite2d_Delete(pre, post)) return false;
        {
            CSprite2d_ZppContract badPost = post;
            badPost.m_pTexture_card = 1u;
            if (op_CSprite2d_Delete(pre, badPost)) return false;
        }
    }
    {
        CSprite2d_ZppContract pre{};
        pre.nextBufferVertex = 1;
        pre.nextBufferIndex = 1;
        const auto post = apply_CSprite2d_RenderVertexBuffer(pre);
        if (!op_CSprite2d_RenderVertexBuffer(pre, post)) return false;
        {
            CSprite2d_ZppContract badPost = post;
            badPost.nextBufferVertex = 1;
            if (op_CSprite2d_RenderVertexBuffer(pre, badPost)) return false;
        }
        {
            CSprite2d_ZppContract badPost = post;
            badPost.nextBufferIndex = 1;
            if (op_CSprite2d_RenderVertexBuffer(pre, badPost)) return false;
        }
    }
    {
        CWaterCannon_ZppContract pre{};
        const int64_t CTimer__GetTimeInMilliseconds_in = 25;
        pre.m_nId = 1;
        pre.m_nCur = 1;
        pre.m_nTimeCreated = 0;
        const auto post = apply_CWaterCannon_Init(pre, CTimer__GetTimeInMilliseconds_in);
        if (!op_CWaterCannon_Init(pre, post, CTimer__GetTimeInMilliseconds_in)) return false;
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nId = 1;
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nCur = 1;
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
        {
            CWaterCannon_ZppContract badPost = post;
            badPost.m_nTimeCreated = (int64_t)(CTimer__GetTimeInMilliseconds_in + 1);
            if (op_CWaterCannon_Init(pre, badPost, CTimer__GetTimeInMilliseconds_in)) return false;
        }
    }
    {
        CWaterLevel_ZppContract pre{};
        pre.nGeomUsed = 1;
        const auto post = apply_CWaterLevel_FreeBoatWakeArray(pre);
        if (!op_CWaterLevel_FreeBoatWakeArray(pre, post)) return false;
        {
            CWaterLevel_ZppContract badPost = post;
            badPost.nGeomUsed = 1;
            if (op_CWaterLevel_FreeBoatWakeArray(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        const int32_t weather_in = 25;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceWeather(pre, weather_in);
        if (!op_CWeather_ForceWeather(pre, post, weather_in)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeather(pre, badPost, weather_in)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        const int32_t weather_in = 25;
        pre.OldWeatherType = 0;
        pre.NewWeatherType = 0;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceWeatherNow(pre, weather_in);
        if (!op_CWeather_ForceWeatherNow(pre, post, weather_in)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = (int32_t)(weather_in + 1);
            if (op_CWeather_ForceWeatherNow(pre, badPost, weather_in)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.Stored_StateStored = false;
        pre.InterpolationValue = 7.0f; pre.Stored_InterpolationValue = 0.0f;
        pre.Rain = 7.0f; pre.Stored_Rain = 0.0f;
        pre.NewWeatherType = 7; pre.Stored_NewWeatherType = 0;
        pre.OldWeatherType = 7; pre.Stored_OldWeatherType = 0;
        const auto post = apply_CWeather_StoreWeatherState(pre);
        if (!op_CWeather_StoreWeatherState(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_StateStored = false;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_InterpolationValue = pre.InterpolationValue + 1.0f;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_Rain = pre.Rain + 1.0f;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_NewWeatherType = pre.NewWeatherType + 1;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_OldWeatherType = pre.OldWeatherType + 1;
            if (op_CWeather_StoreWeatherState(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.Stored_StateStored = true;
        pre.Stored_InterpolationValue = 7.0f; pre.InterpolationValue = 0.0f;
        pre.Stored_Rain = 7.0f; pre.Rain = 0.0f;
        pre.Stored_NewWeatherType = 7; pre.NewWeatherType = 0;
        pre.Stored_OldWeatherType = 7; pre.OldWeatherType = 0;
        const auto post = apply_CWeather_RestoreWeatherState(pre);
        if (!op_CWeather_RestoreWeatherState(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.Stored_StateStored = true;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.InterpolationValue = pre.Stored_InterpolationValue + 1.0f;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.Rain = pre.Stored_Rain + 1.0f;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = pre.Stored_NewWeatherType + 1;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = pre.Stored_OldWeatherType + 1;
            if (op_CWeather_RestoreWeatherState(pre, badPost)) return false;
        }
    }
    {
        CWeather_ZppContract pre{};
        pre.OldWeatherType = 0;
        pre.NewWeatherType = 0;
        pre.ForcedWeatherType = 0;
        const auto post = apply_CWeather_ForceHurricaneWeather(pre);
        if (!op_CWeather_ForceHurricaneWeather(pre, post)) return false;
        {
            CWeather_ZppContract badPost = post;
            badPost.OldWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.NewWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
        {
            CWeather_ZppContract badPost = post;
            badPost.ForcedWeatherType = 0;
            if (op_CWeather_ForceHurricaneWeather(pre, badPost)) return false;
        }
    }
    {
        CWindModifiers_ZppContract pre{};
        const ZppVec3 pos_in = ZppVec3{1.0f, 2.0f, 3.0f};
        const int32_t type_in = 25;
        pre.m_pos = ZppVec3{0.0f, 0.0f, 0.0f};
        pre.m_type = 0;
        const auto post = apply_CWindModifiers_RegisterOne(pre, pos_in, type_in);
        if (!op_CWindModifiers_RegisterOne(pre, post, pos_in, type_in)) return false;
        {
            CWindModifiers_ZppContract badPost = post;
            badPost.m_pos = ZppVec3{pos_in.x + 1.0f, pos_in.y, pos_in.z};
            if (op_CWindModifiers_RegisterOne(pre, badPost, pos_in, type_in)) return false;
        }
        {
            CWindModifiers_ZppContract badPost = post;
            badPost.m_type = (int32_t)(type_in + 1);
            if (op_CWindModifiers_RegisterOne(pre, badPost, pos_in, type_in)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
static_assert(CConsole_ZppContract{}.m_aTimer.size() == 8u, "CConsole MAX_LINES must be 8");
static_assert(op_CWeather_StoreWeatherState(CWeather_ZppContract{}, apply_CWeather_StoreWeatherState(CWeather_ZppContract{})), "CWeather_StoreWeatherState float-copy delta contract must hold");
static_assert(op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3), 3) && []() constexpr { auto bad = apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3); bad.NewWeatherType = 2; return !op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, bad, 3); }(), "CWeather_ForceWeatherNow must require equal weather types from single weather_in input");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
