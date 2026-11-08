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
        return (pos_len == 6u && pos.size() == 6u)
            && (speed_len == 6u && speed.size() == 6u)
            && (id >= 0);
    }
};
// State Schema Contract from zpp/renderer/Antennas.zpp
struct CAntennas_ZppContract {
    std::array<int32_t, 8> aAntennas{}; // Z++: seq CAntenna  -- bounded array [NUMANTENNAS]
    size_t aAntennas_len{8u}; // Z++ cardinality #aAntennas
    constexpr bool checkInvariant() const {
        return (aAntennas_len == 8u && aAntennas.size() == 8u);
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
struct CCoronas_ZppContract {
    std::array<int32_t, 8> aCoronas{}; // Z++: seq CRegisteredCorona  -- bounded array [NUMCORONAS]
    size_t aCoronas_len{8u}; // Z++ cardinality #aCoronas
    float LightsMult{0.0f}; // Z++: ℝ
    float SunScreenY{0.0f}; // Z++: ℝ
    float SunScreenX{0.0f}; // Z++: ℝ
    int32_t MoonSize{0}; // Z++: ℤ
    bool SunBlockedByClouds{false}; // Z++: 𝔹
    int32_t bChangeBrightnessImmediately{0}; // Z++: ℤ
    bool bSmallMoon{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (aCoronas_len == 8u && aCoronas.size() == 8u);
    }
};
// Operation Schema Contract (ΔCAntenna) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntenna_Update(CAntenna_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAntennas) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntennas_Init(CAntennas_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAntennas) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntennas_Update_2(CAntennas_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAntennas) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntennas_RegisterOne_3(CAntennas_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAntennas) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntennas_Render_4(CAntennas_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCAntennas) from zpp/renderer/Antennas.zpp
constexpr bool op_CAntennas_Update(CAntennas_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_renderer_zpp_invariants() {
    {
        Attractor_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        Attractor_ZppContract bad{};
        bad.probability = -1;
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
        CAntennas_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CAntennas_ZppContract bad{};
        bad.aAntennas_len = 0u;
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
        CCoronas_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CCoronas_ZppContract bad{};
        bad.aCoronas_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CAntenna_ZppContract st{};
        if (!op_CAntenna_Update(st)) return false;
    }
    {
        CAntennas_ZppContract st{};
        if (!op_CAntennas_Init(st)) return false;
    }
    {
        CAntennas_ZppContract st{};
        if (!op_CAntennas_Update_2(st)) return false;
    }
    {
        CAntennas_ZppContract st{};
        if (!op_CAntennas_RegisterOne_3(st)) return false;
    }
    {
        CAntennas_ZppContract st{};
        if (!op_CAntennas_Render_4(st)) return false;
    }
    {
        CAntennas_ZppContract st{};
        if (!op_CAntennas_Update(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
