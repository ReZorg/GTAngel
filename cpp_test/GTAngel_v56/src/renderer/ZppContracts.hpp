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
struct CSmokeTrails_ZppContract {
    std::array<int32_t, 3> aSmoke{}; // Z++: seq CSmokeTrail  -- bounded array [3]
    size_t aSmoke_len{3u}; // Z++ cardinality #aSmoke
    bool CigOn{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (aSmoke_len == 3u && aSmoke.size() == 3u);
    }
};
// Delta Operation Schema Contract (ΔCCredits) from zpp/renderer/Credits.zpp
constexpr bool op_CCredits_Stop(const CCredits_ZppContract& pre, const CCredits_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bCreditsGoing == false);
}
constexpr CCredits_ZppContract apply_CCredits_Stop(const CCredits_ZppContract& pre) {
    CCredits_ZppContract post = pre;
    post.bCreditsGoing = false;
    return post;
}
constexpr bool verify_renderer_zpp_invariants() {
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
        CMovingThing_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CMovingThing_ZppContract bad{};
        bad.m_pNext_card = 2u;
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
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
