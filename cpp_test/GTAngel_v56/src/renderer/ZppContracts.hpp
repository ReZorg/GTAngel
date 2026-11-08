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
    int32_t dir{0}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    int32_t useDir{0}; // Z++: CVector
    int32_t pos{0}; // Z++: CVector
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
    int32_t dir{0}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    int32_t dir{0}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (probability >= 0);
    }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct PedAttractor_ZppContract {
    int32_t queueDir{0}; // Z++: CVector
    int32_t useDir{0}; // Z++: CVector
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
    std::array<int32_t, 6> pos{}; // Z++: seq CVector  -- bounded array [6]
    size_t pos_len{6u}; // Z++ cardinality #pos
    std::array<int32_t, 6> speed{}; // Z++: seq CVector  -- bounded array [6]
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
    std::array<int32_t, 16> m_pos{}; // Z++: seq CVector  -- bounded array [16]
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
    std::array<int32_t, 8> m_pos{}; // Z++: seq CVector  -- bounded array [8]
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
    int32_t m_pos0{0}; // Z++: CVector
    int32_t m_pos1{0}; // Z++: CVector
    int32_t m_pos2{0}; // Z++: CVector
    int32_t m_pos3{0}; // Z++: CVector
    int32_t m_matrix{0}; // Z++: CMatrix
    bool m_bIsActive{false}; // Z++: 𝔹
    bool m_bIsMovingDown{false}; // Z++: 𝔹
    int32_t m_stepsCount{0}; // Z++: ℤ
    float m_lowerEnd{0.0f}; // Z++: ℝ
    float m_upperEnd{0.0f}; // Z++: ℝ
    int32_t m_midPoint{0}; // Z++: CVector
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
    int32_t m_vecPosn{0}; // Z++: CVector
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
    int32_t m_Position{0}; // Z++: CVector
    int64_t m_MessageCurrentChar{0}; // Z++: ℕ
    int64_t m_MessageLength{0}; // Z++: ℕ
    int32_t m_Size{0}; // Z++: CVector
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
    int32_t m_Position{0}; // Z++: CVector
    int32_t m_Size{0}; // Z++: CVector
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
constexpr bool op_CCredits_Start(const CCredits_ZppContract& pre, const CCredits_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == true) && (post.CreditsStartTime == 1);
}
constexpr CCredits_ZppContract apply_CCredits_Start(const CCredits_ZppContract& pre) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = true;
    post.CreditsStartTime = 1;
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
// Delta Operation Schema Contract (ΔCDraw) from zpp/renderer/Draw.zpp
constexpr bool op_CDraw_SetFOV(const CDraw_ZppContract& pre, const CDraw_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.ms_fScaledFOV == 1.0f) && (post.ms_fFOV == 1.0f);
}
constexpr CDraw_ZppContract apply_CDraw_SetFOV(const CDraw_ZppContract& pre) {
    CDraw_ZppContract post = pre;
    post.ms_fScaledFOV = 1.0f;
    post.ms_fFOV = 1.0f;
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
        pre.bCreditsGoing = false;
        pre.CreditsStartTime = 0;
        const auto post = apply_CCredits_Start(pre);
        if (!op_CCredits_Start(pre, post)) return false;
        {
            CCredits_ZppContract badPost = post;
            badPost.bCreditsGoing = false;
            if (op_CCredits_Start(pre, badPost)) return false;
        }
        {
            CCredits_ZppContract badPost = post;
            badPost.CreditsStartTime = 0;
            if (op_CCredits_Start(pre, badPost)) return false;
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
        CDraw_ZppContract pre{};
        pre.ms_fScaledFOV = 0.0f;
        pre.ms_fFOV = 0.0f;
        const auto post = apply_CDraw_SetFOV(pre);
        if (!op_CDraw_SetFOV(pre, post)) return false;
        {
            CDraw_ZppContract badPost = post;
            badPost.ms_fScaledFOV = 0.0f;
            if (op_CDraw_SetFOV(pre, badPost)) return false;
        }
        {
            CDraw_ZppContract badPost = post;
            badPost.ms_fFOV = 0.0f;
            if (op_CDraw_SetFOV(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
static_assert(CConsole_ZppContract{}.m_aTimer.size() == 8u, "CConsole MAX_LINES must be 8");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
