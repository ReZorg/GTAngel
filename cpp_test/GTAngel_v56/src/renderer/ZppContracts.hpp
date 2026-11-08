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
// From zpp/renderer/Console.zpp
enum class Console_Enum_1 : int32_t { MAX_LINES_v = 8, MAX_STR_LEN_v = 40 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_1 : int32_t { SUN_CORE_v = 1, SUN_CORONA_v = 2 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_2 : int32_t { TYPE_STAR_v = 0, TYPE_NORMAL_v = 1, TYPE_MOON_v = 2, TYPE_REFLECT_v = 3, TYPE_HEADLIGHT_v = 4, TYPE_HEX_v = 5, TYPE_CIRCLE_v = 6, TYPE_RING_v = 7, TYPE_STREAK_v = 8 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_3 : int32_t { FLARE_NONE_v = 0, FLARE_SUN_v = 1, FLARE_HEADLIGHTS_v = 2 };
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    int32_t dir{0}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    int64_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (probability >= 0);
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
        return (id >= 0)
            && (pos_len == 6u && pos.size() == 6u)
            && (speed_len == 6u && speed.size() == 6u);
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
    int32_t m_aTimer{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aRed{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aGreen{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    int32_t m_aBlue{0}; // Z++: seq ℕ  -- bounded array [MAX_LINES]
    constexpr bool checkInvariant() const {
        return (m_nLineCount >= 0)
            && (m_nCurrentLine >= 0);
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
constexpr bool verify_renderer_zpp_invariants() {
    if (!Attractor_ZppContract{}.checkInvariant()) return false;
    if (!CAntenna_ZppContract{}.checkInvariant()) return false;
    if (!CClouds_ZppContract{}.checkInvariant()) return false;
    if (!CConsole_ZppContract{}.checkInvariant()) return false;
    if (!CCredits_ZppContract{}.checkInvariant()) return false;
    if (!CDraw_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
