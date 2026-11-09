#pragma once
#ifndef GTANGEL_ZPP_SKEL_SPEC_GUARD
#define GTANGEL_ZPP_SKEL_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/skel
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/skel/*.zpp
// Modules: 7 | Formal Schemas: 145 | Free Types: 11
// ============================================================================
namespace zpp_skel_spec {
static constexpr int kZppModuleCount = 7;
static constexpr int kZppSchemaCount = 145;
static constexpr int kZppEnumCount   = 11;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
// Free Type from zpp/skel/crossplatform.zpp
enum class eWinVersion : int32_t { OS_WIN95_v = 0, OS_WIN98_v = 1, OS_WINNT_v = 2, OS_WIN2000_v = 3, OS_WINXP_v = 4 };
// Free Type from zpp/skel/crossplatform.zpp
enum class crossplatform_Enum_2 : int32_t { LANG_OTHER_v = 0, LANG_GERMAN_v = 1, LANG_FRENCH_v = 2, LANG_ENGLISH_v = 3, LANG_ITALIAN_v = 4, LANG_SPANISH_v = 5 };
// Free Type from zpp/skel/crossplatform.zpp
enum class crossplatform_Enum_3 : int32_t { SUBLANG_OTHER_v = 0, SUBLANG_ENGLISH_AUS_v = 1 };
// Free Type from zpp/skel/crossplatform.zpp
enum class eGameState : int32_t { GS_START_UP_v = 0, GS_INIT_LOGO_MPEG_v = 1, GS_LOGO_MPEG_v = 2, GS_INIT_INTRO_MPEG_v = 3, GS_INTRO_MPEG_v = 4, GS_INIT_ONCE_v = 5, GS_INIT_FRONTEND_v = 6, GS_FRONTEND_v = 7, GS_INIT_PLAYING_GAME_v = 8, GS_PLAYING_GAME_v = 9 };
// Free Type from zpp/skel/skeleton.zpp
enum class RsInputDeviceType : int32_t { rsKEYBOARD_v = 0, rsMOUSE_v = 1, rsPAD_v = 2 };
// Free Type from zpp/skel/skeleton.zpp
enum class RsEventStatus : int32_t { rsEVENTERROR_v = 0, rsEVENTPROCESSED_v = 1, rsEVENTNOTPROCESSED_v = 2 };
// State Schema Contract from zpp/skel/crossplatform.zpp
struct SYSTEMTIME_ZppContract {
    int64_t wYear{0}; // Z++: ℕ
    int64_t wMonth{0}; // Z++: ℕ
    int64_t wDayOfWeek{0}; // Z++: ℕ
    int64_t wDay{0}; // Z++: ℕ
    int64_t wHour{0}; // Z++: ℕ
    int64_t wMinute{0}; // Z++: ℕ
    int64_t wSecond{0}; // Z++: ℕ
    int64_t wMilliseconds{0}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (wYear >= 0)
            && (wMonth >= 0)
            && (wDayOfWeek >= 0)
            && (wDay >= 0)
            && (wHour >= 0)
            && (wMinute >= 0)
            && (wSecond >= 0)
            && (wMilliseconds >= 0);
    }
};
// State Schema Contract from zpp/skel/events.zpp
struct eventsModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{838}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 838);
    }
};
// State Schema Contract from zpp/skel/glfw/glfw.zpp
struct glfwModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{2602}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 2602);
    }
};
// State Schema Contract from zpp/skel/platform.zpp
struct platformModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{116}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 116);
    }
};
// State Schema Contract from zpp/skel/skeleton.zpp
struct RsInputDevice_ZppContract {
    int32_t inputDeviceType{0}; // Z++: RsInputDeviceType
    bool used{false}; // Z++: 𝔹
    int32_t inputEventHandler{0}; // Z++: RsInputEventHandler
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/skel/skeleton.zpp
struct RsGlobalType_ZppContract {
    int32_t appName{0}; // Z++: seq CHAR
    int32_t width{0}; // Z++: ℤ
    int32_t height{0}; // Z++: ℤ
    int32_t maximumWidth{0}; // Z++: ℤ
    int32_t maximumHeight{0}; // Z++: ℤ
    int32_t maxFPS{0}; // Z++: ℤ
    bool quit{false}; // Z++: 𝔹
    int64_t ps{0}; // Z++: ADDRESS
    int32_t keyboard{0}; // Z++: RsInputDevice
    int32_t mouse{0}; // Z++: RsInputDevice
    int32_t pad{0}; // Z++: RsInputDevice
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/skel/skeleton.zpp
struct RsKeyStatus_ZppContract {
    int32_t keyCharCode{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/skel/skeleton.zpp
struct RsPadButtonStatus_ZppContract {
    int32_t padID{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/skel/win/resource.zpp
struct resourceModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{35}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 35);
    }
};
// State Schema Contract from zpp/skel/win/win.zpp
struct tJoy_ZppContract {
    int32_t m_State{0}; // Z++: eJoypadState
    bool m_bInitialised{false}; // Z++: 𝔹
    bool m_bHasAxisZ{false}; // Z++: 𝔹
    bool m_bHasAxisR{false}; // Z++: 𝔹
    int32_t m_nVendorID{0}; // Z++: ℤ
    int32_t m_nProductID{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/skel/win/win.zpp
struct CJoySticks_ZppContract {
    std::array<int32_t, 2> m_aJoys{}; // Z++: seq tJoy  -- bounded array [MAX_JOYSTICKS]
    size_t m_aJoys_len{2u}; // Z++ cardinality #m_aJoys
    int64_t instance_addr{0}; // Z++: ADDRESS
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_aJoys_len == 2u && m_aJoys.size() == 2u)
            && (instance_addr >= 0);
    }
};
constexpr bool verify_skel_zpp_invariants() {
    {
        SYSTEMTIME_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        SYSTEMTIME_ZppContract bad{};
        bad.wYear = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        eventsModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        eventsModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        glfwModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        glfwModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        platformModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        platformModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RsInputDevice_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RsInputDevice_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RsGlobalType_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RsGlobalType_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RsKeyStatus_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RsKeyStatus_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RsPadButtonStatus_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RsPadButtonStatus_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        resourceModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        resourceModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        tJoy_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tJoy_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CJoySticks_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CJoySticks_ZppContract bad{};
        bad.m_aJoys_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_skel_zpp_invariants(), "Z++ formal contract invariant failed for skel");
} // namespace zpp_skel_spec
#endif // GTANGEL_ZPP_SKEL_SPEC_GUARD
static void RenderV15Frame(HDC rawHdc, RECT& rc) {
    // LW41: Unified Full-Cockpit Screen — renders the F6 dashboard elements
    // on the TOP, LEFT, and BOTTOM around the central 768x768 3D viewport and
    // the existing right-hand Story/Quest info panel on one flicker-free canvas.
    const int clientW = std::max(1440, (int)(rc.right - rc.left));
    const int clientH = std::max(996,  (int)(rc.bottom - rc.top));
    HDC hdc = CreateCompatibleDC(rawHdc);
    HBITMAP memBmp = CreateCompatibleBitmap(rawHdc, clientW, clientH);
    HBITMAP oldBmp = (HBITMAP)SelectObject(hdc, memBmp);
    DrawRect(hdc, 0, 0, clientW, clientH, COL_BG);

    const int viewX = 368, viewY = 48;
    const int railX = viewX + 768 + 4; // 1140
    const int railW = std::max(294, clientW - railX - 6);

    // ═══ 1. TOP BAR (y = 0..46): F6 Global Status & Architecture Strip ═══
    DrawRect(hdc, 0, 0, clientW, 46, RGB(16, 18, 34));
    DrawLine(hdc, 0, 46, clientW, 46, COL_BRIDGE);
    DrawText(hdc, 12, 4, "GTAngel v56 -- Unified Embodied AGI Cockpit (768x768 VRD + F6 Telemetry + 4-Act Story)", COL_ACCENT, g_hFont);
    DrawText(hdc, 590, 6, "EndocrineBus | Echobeats | NicheConstruction | ESN Reservoir | TRIZ Arena | Group Dynamics | CogVerse | ECAN | Phasor", COL_TEXT2, g_hFontSmall);
    {
        char sb[320];
        snprintf(sb, sizeof(sb),
            "Status: %s | Tick: %d | Mode: %s | Echobeats: %s | ESN Energy: %.3f | Transitions: %d | Cls Steps: %llu | VR Frame: %llu | Niches: %d | LLM Acc: %.2f | Echo: %s (%.2f)",
            g_agi.running ? (g_agi.paused ? "PAUSED" : "RUNNING") : "STOPPED",
            g_agi.tickCount, dte::cognitiveModeName(g_agi.endo.cognitiveMode()),
            dte::phaseName(g_agi.echoPhase), g_agi.esnEnergy, g_agi.endo.modeTransitions,
            (unsigned long long)g_agi.env.steps(), (unsigned long long)g_agi.tracker.frameCount(),
            g_agi.bridgeTelemetry.niche_count, g_agi.llmMetrics.accuracy,
            g_agi.echoPersona.state().emotion, g_agi.echoPersona.state().spice);
        DrawText(hdc, 12, 25, sb, g_agi.running ? COL_GREEN : COL_RED, g_hFontSmall);
    }

    // ═══ 2. LEFT COLUMN (x = 6..362, y = 50..816): Endocrine + TRIZ Arena + Retina/Radar ═══
    // 2a. Virtual Endocrine System (y = 50..324, 10 hormones in 5x2 grid with clean spacing)
    {
        const int lx = 6, ly = 50, lw = 356, lh = 274;
        DrawRect(hdc, lx, ly, lw, lh, COL_CARD);
        DrawText(hdc, lx + 8, ly + 4, "Virtual Endocrine System (dte::EndocrineBus)", COL_CYAN, g_hFontSmall);
        for (int i = 0; i < (int)dte::HORMONE_COUNT; i++) {
            const int row = i / 2, col = i % 2;
            const int bx = lx + 10 + col * 172, by = ly + 21 + row * 39;
            if (i == g_agi.selectedHormone) DrawRect(hdc, bx - 2, by - 1, 166, 37, RGB(52, 52, 92));
            DrawText(hdc, bx, by, dte::HORMONE_NAMES[i], i == g_agi.selectedHormone ? COL_ACCENT : COL_TEXT2, g_hFontSmall);
            const float val = (float)g_agi.endo.hormoneLevel((dte::HormoneId)i);
            const COLORREF bc = val > 0.6f ? COL_RED : (val > 0.4f ? COL_ACCENT : COL_GREEN);
            DrawBar(hdc, bx, by + 13, 152, 7, val, bc, RGB(38, 38, 58));
            char hb[64];
            snprintf(hb, sizeof(hb), "%.2f  d=%.3f", val, g_agi.endo.getDecayRate((dte::HormoneId)i));
            DrawText(hdc, bx, by + 22, hb, COL_TEXT, g_hFontSmall);
        }
        auto& va = g_agi.endo.valenceArousal();
        char vab[128];
        snprintf(vab, sizeof(vab), "Valence: %+.2f  Arousal: %+.2f  |V-A|: %.2f", va.valence, va.arousal, va.magnitude());
        DrawText(hdc, lx + 10, ly + lh - 52, vab, COL_TEXT, g_hFontSmall);
        char mb[96];
        snprintf(mb, sizeof(mb), "Cognitive Mode: %s   [TAB/+/- tune]", dte::cognitiveModeName(g_agi.endo.cognitiveMode()));
        DrawText(hdc, lx + 10, ly + lh - 35, mb, dte::cognitiveModeColor(g_agi.endo.cognitiveMode()), g_hFontSmall);
        int mhCount = 0;
        for (int i = (int)g_agi.endo.modeHistory.size() - 1; i >= 0 && mhCount < 3; i--, mhCount++) {
            auto& mh = g_agi.endo.modeHistory[i];
            char mhb[64];
            snprintf(mhb, sizeof(mhb), "T%d:%s", mh.first, dte::cognitiveModeName(mh.second));
            DrawText(hdc, lx + 10 + mhCount * 112, ly + lh - 18, mhb, dte::cognitiveModeColor(mh.second), g_hFontSmall);
        }
    }

    // 2b. TRIZ Arena + Spatial Contradiction Inspector (y = 328..646)
    {
        const int cx = 6, cy = 328, cw = 356, ch = 318;
        DrawRect(hdc, cx, cy, cw, ch, COL_CARD);
        DrawText(hdc, cx + 8, cy + 5, "TRIZ Arena + Spatial Contradiction Inspector", COL_CYAN, g_hFontSmall);
        const int cs = 19, gox = cx + 18, goy = cy + 24;
        for (int q = 0; q < dte::TRIZArena::SIZE; q++) {
            for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                auto& cell = g_agi.arena.grid[q][r];
                const int px = gox + q * (cs + 2) + (r % 2) * (cs / 2 + 1);
                const int py = goy + r * (cs + 2);
                const float v = cell.aestheticValue;
                const COLORREF cc = cell.isNiche
                    ? (cell.nicheType == dte::NicheType::Perturbation ? COL_NICHE_P :
                       cell.nicheType == dte::NicheType::Scaffolding ? COL_NICHE_S : COL_NICHE_I)
                    : RGB((int)(v * 255), (int)(v * 190), (int)(v * 90));
                if (q == g_agi.arena.maxContradictionQ && r == g_agi.arena.maxContradictionR)
                    DrawRect(hdc, px - 2, py - 2, cs + 4, cs + 4, COL_CONTRA);
                if (q == g_agi.selectedCellQ && r == g_agi.selectedCellR)
                    DrawRect(hdc, px - 1, py - 1, cs + 2, cs + 2, COL_ACCENT);
                DrawRect(hdc, px, py, cs, cs, cc);
                if (cell.contradiction > 0.1f) {
                    const int ci = (int)(cell.contradiction * 255);
                    DrawRect(hdc, px + cs / 2 - 2, py + cs / 2 - 2, 4, 4, RGB(ci, 0, 0));
                }
            }
        }
        char ab[200];
        snprintf(ab, sizeof(ab), "Coh: %.3f | Phase: %s | Disc: %d | Niches: %d",
            g_agi.arena.coherence, g_agi.arena.phaseName(), g_agi.arena.discoveries, (int)g_agi.arena.niches.size());
        DrawText(hdc, cx + 8, cy + ch - 42, ab, COL_TEXT, g_hFontSmall);
        if (g_agi.selectedCellQ >= 0 && g_agi.selectedCellR >= 0) {
            auto& sc = g_agi.arena.grid[g_agi.selectedCellQ][g_agi.selectedCellR];
            snprintf(ab, sizeof(ab), "Cell(%d,%d) Aes=%.2f Con=%.2f P=%s",
                g_agi.selectedCellQ, g_agi.selectedCellR, sc.aestheticValue, sc.contradiction,
                sc.lastPrinciple >= 0 ? g_agi.arena.principles[sc.lastPrinciple] : "none");
            DrawText(hdc, cx + 8, cy + ch - 22, ab, COL_ACCENT, g_hFontSmall);
        } else {
            snprintf(ab, sizeof(ab), "Max Contra: %.3f at (%d,%d) | Click cell to inspect",
                g_agi.arena.maxContradiction, g_agi.arena.maxContradictionQ, g_agi.arena.maxContradictionR);
            DrawText(hdc, cx + 8, cy + ch - 22, ab, COL_TEXT2, g_hFontSmall);
        }
    }

    // 2c. Egocentric Retina (64x64) + 128m Tactical Radar Minimap side-by-side (y = 650..816)
    auto drawWorldMapPanel = [&](int mx, int my, int mw, bool fullLabels) {
        DrawRect(hdc, mx - 2, my - 2, mw + 4, mw + 4, COL_BRIDGE);
        DrawRect(hdc, mx, my, mw, mw, RGB(14, 18, 28));
        const float scale = (float)mw / 128.0f;
        auto toX = [&](float wx) { return mx + std::clamp((int)((wx + 64.0f) * scale), 2, mw - 3); };
        auto toY = [&](float wz) { return my + std::clamp((int)((64.0f - wz) * scale), 2, mw - 3); };
        const int rHalf = std::max(2, (int)(2.2f * scale));
        DrawRect(hdc, mx + 2, toY(0.0f) - rHalf, mw - 4, rHalf * 2, RGB(38, 42, 52));
        DrawRect(hdc, toX(0.0f) - rHalf, my + 2, rHalf * 2, mw - 4, RGB(38, 42, 52));
        for (const auto& b : g_agi.navWorld.buildings) {
            const int bx0 = toX(b.pos.x - b.w * 0.5f), bx1 = toX(b.pos.x + b.w * 0.5f);
            const int bz0 = toY(b.pos.z + b.d * 0.5f), bz1 = toY(b.pos.z - b.d * 0.5f);
            DrawRect(hdc, bx0, bz0, std::max(3, bx1 - bx0), std::max(3, bz1 - bz0), RGB(72, 68, 86));
        }
        for (const auto& p : g_agi.navWorld.pillars)
            DrawRect(hdc, toX(p.pos.x) - 1, toY(p.pos.z) - 1, 3, 3, RGB(54, 130, 72));
        for (const auto& w : g_agi.navWorld.walkers)
            DrawRect(hdc, toX(w.pos.x) - 1, toY(w.pos.z) - 1, 3, 3, RGB(180, 195, 220));
        for (const auto& v : g_agi.navWorld.traffic)
            DrawRect(hdc, toX(v.pos.x) - 2, toY(v.pos.z) - 2, 5, 5, v.orbitR > 0.0f ? RGB(235, 85, 85) : RGB(95, 175, 245));
        DrawRect(hdc, toX(g_agi.navWorld.vehiclePos.x) - 3, toY(g_agi.navWorld.vehiclePos.z) - 3, 6, 6, RGB(245, 155, 65));
        for (const auto& pk : g_agi.navWorld.pickups)
            if (!pk.collected) DrawRect(hdc, toX(pk.pos.x) - 2, toY(pk.pos.z) - 2, 5, 5, COL_GOLD);
        DrawRect(hdc, toX(g_agi.navWorld.npcPos.x) - 3, toY(g_agi.navWorld.npcPos.z) - 3, 7, 7, COL_PURPLE);
        if (g_agi.navWorld.questShardVisible)
            DrawRect(hdc, toX(g_agi.navWorld.questShard.x) - 3, toY(g_agi.navWorld.questShard.z) - 3, 6, 6, COL_CYAN);
        DrawRect(hdc, toX(g_agi.navWorld.goal.x) - 3, toY(g_agi.navWorld.goal.z) - 3, 7, 7, g_agi.gameplay.quest.beaconRestored ? COL_CYAN : COL_GOLD);
        DrawRect(hdc, toX(-9.0f) - 3, toY(-2.4f) - 3, 6, 6, g_agi.gameplay.quest.obeliskDecrypted ? COL_GREEN : RGB(215, 130, 245));
        DrawRect(hdc, toX( 9.0f) - 3, toY( 2.4f) - 3, 6, 6, g_agi.gameplay.quest.relayAttuned ? COL_GREEN : COL_CYAN);
        DrawRect(hdc, toX( 2.6f) - 3, toY(-18.0f) - 3, 6, 6, g_agi.gameplay.quest.shadowNodePurified ? COL_GREEN : COL_RED);
        DrawRect(hdc, toX(-18.0f) - 3, toY( 2.6f) - 3, 6, 6, g_agi.gameplay.quest.skylineTowerSynced ? COL_GREEN : COL_GOLD);
        const int px = toX(g_agi.navPos.x), py = toY(g_agi.navPos.z);
        const float gAng = g_agi.navHeading + g_agi.navGoalCompass;
        DrawLine(hdc, px, py, px + (int)(sinf(gAng) * 18.0f), py - (int)(cosf(gAng) * 18.0f), COL_GOLD);
        const int hx = px + (int)(sinf(g_agi.navHeading) * 11.0f);
        const int hy = py - (int)(cosf(g_agi.navHeading) * 11.0f);
        DrawLine(hdc, px, py, hx, hy, COL_GREEN);
        DrawRect(hdc, px - 3, py - 3, 7, 7, COL_GREEN);
        if (fullLabels) {
            DrawText(hdc, mx + 12, my + 10, "SANCTUARY 128m TACTICAL WORLD MAP  [F3 CLOSE]", COL_GOLD, g_hFont);
            DrawText(hdc, toX(g_agi.navWorld.npcPos.x) + 6, toY(g_agi.navWorld.npcPos.z) - 6, "Ariadne", COL_PURPLE, g_hFontSmall);
            DrawText(hdc, toX(g_agi.navWorld.goal.x) + 6, toY(g_agi.navWorld.goal.z) - 6, "Civic Beacon", COL_CYAN, g_hFontSmall);
            DrawText(hdc, toX(-9.0f) + 6, toY(-2.4f) - 6, "Memory Obelisk", RGB(215, 130, 245), g_hFontSmall);
            DrawText(hdc, toX( 9.0f) + 6, toY( 2.4f) - 6, "Transit Relay", COL_CYAN, g_hFontSmall);
            DrawText(hdc, toX( 2.6f) + 6, toY(-18.0f) - 6, "Shadow Node", COL_RED, g_hFontSmall);
            DrawText(hdc, toX(-18.0f) + 6, toY( 2.6f) - 6, "Skyline Tower", COL_GOLD, g_hFontSmall);
            DrawText(hdc, px + 6, py - 6, "DTE", COL_GREEN, g_hFontSmall);
        }
    };
    {
        const int rx = 6, ry = 650, rw = 356, rh = 166;
        DrawRect(hdc, rx, ry, rw, rh, COL_CARD);
        DrawText(hdc, rx + 8, ry + 4, "Egocentric Retina (64x64)", COL_CYAN, g_hFontSmall);
        DrawText(hdc, rx + 188, ry + 4, "128m Radar [F3 Map]", COL_CYAN, g_hFontSmall);
        const int vx = rx + 14, vy = ry + 22;
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 64; x++) {
                const float v = g_agi.navRenderer.frame[y * 64 + x];
                const float sem = g_agi.navRenderer.semantic[y * 64 + x];
                const COLORREF c = sem > 0.9f ? RGB(0, 255, 120) :
                                   sem > 0.6f ? RGB(255, 80, 60) :
                                   sem > 0.4f ? RGB(120, 180, 255) :
                                   sem > 0.2f ? RGB(60, 60, 80) :
                                                RGB((int)(v * 40), (int)(v * 40), (int)(v * 60));
                DrawRect(hdc, vx + x * 2, vy + y * 2, 2, 2, c);
            }
        }
        drawWorldMapPanel(rx + 196, ry + 22, 136, false);
    }

    // ═══ 3. CENTER 768x768 3D VIEWPORT (x = 368..1136, y = 48..816) ═══
    g_agi.highResRenderer.framebuffer.blit(hdc, viewX, viewY, g_agi.highResDisplayMode);

    // ═══ 4. RIGHT-HAND STORY, QUEST & CONTROLS PANEL (x = 1140..clientW, y = 48..816) ═══
    DrawRect(hdc, railX, viewY, railW, 768, RGB(12, 14, 27));
    DrawRect(hdc, railX, viewY, 3, 768, COL_BRIDGE);
    DrawText(hdc, railX + 12, viewY + 8, "GTAngel v56 / LOCAL LEARNING", COL_ACCENT, g_hFont);
    DrawText(hdc, railX + 12, viewY + 28, g_agi.gameplay.actTitle(), COL_CYAN, g_hFontSmall);

    char line[256];
    snprintf(line, sizeof(line), "Stage  %s", g_agi.gameplay.stageName().c_str());
    DrawText(hdc, railX + 12, viewY + 50, line, g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_GOLD, g_hFontMono);
    snprintf(line, sizeof(line), "DBG pos(%.1f,%.1f) hd(%.2f) cmp(%.2f)",
             g_agi.navPos.x, g_agi.navPos.z, g_agi.navHeading, g_agi.navGoalCompass);
    DrawText(hdc, railX + 12, viewY + 66, line, COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 82, "CURRENT PURPOSE", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 98, g_agi.gameplay.objective().c_str(), COL_TEXT, g_hFontSmall);
    DrawLine(hdc, railX + 12, viewY + 118, railX + railW - 12, viewY + 118, RGB(70, 70, 100));

    DrawText(hdc, railX + 12, viewY + 126, "ARIADNE", COL_PURPLE, g_hFont);
    snprintf(line, sizeof(line), "Routine  %s   Disposition  %.0f%%", g_agi.gameplay.routineName(), g_agi.gameplay.ariadne.disposition * 100.0f);
    DrawText(hdc, railX + 12, viewY + 146, line, COL_TEXT2, g_hFontSmall);
    DrawBar(hdc, railX + 12, viewY + 164, std::max(20, railW - 24), 8,
            g_agi.gameplay.ariadne.disposition, COL_PURPLE, RGB(36, 34, 55));

    DrawText(hdc, railX + 12, viewY + 182, "INVENTORY & WORLD", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 198,
        g_agi.gameplay.quest.hasSignalShard ? "Signal Shard  x1" : "Signal Shard  --",
        g_agi.gameplay.quest.hasSignalShard ? COL_CYAN : COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 214,
        g_agi.gameplay.quest.hasResonanceSigil ? "Resonance Sigil  x1" : "Resonance Sigil  --",
        g_agi.gameplay.quest.hasResonanceSigil ? COL_GOLD : COL_TEXT2, g_hFontSmall);
    char cellBuf[64];
    snprintf(cellBuf, sizeof(cellBuf), "Data Cells  %u/4   Beacon: %s",
        g_agi.gameplay.quest.pickupsCollected,
        g_agi.gameplay.quest.beaconRestored ? "RESTORED" : "BROKEN");
    DrawText(hdc, railX + 12, viewY + 230, cellBuf, g_agi.gameplay.quest.beaconRestored ? COL_CYAN : COL_GOLD, g_hFontSmall);
    snprintf(line, sizeof(line), "Frame %llu   %.2f ms%s%s",
        (unsigned long long)g_agi.highResRenderer.metrics.sequence,
        g_agi.highResRenderer.metrics.renderMs,
        g_agi.highResRenderer.lastValidationFailure() ? "  ZPP:" : "",
        g_agi.highResRenderer.lastValidationFailure() ? g_agi.highResRenderer.lastValidationFailure() : "");
    DrawText(hdc, railX + 12, viewY + 246, line, COL_TEXT2, g_hFontSmall);

    DrawText(hdc, railX + 12, viewY + 266, "LAST GROUNDED EVENT", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 282, g_agi.gameplay.quest.lastEvent.c_str(), COL_TEXT, g_hFontSmall);
    snprintf(line, sizeof(line), "Events %u   Save %s",
        g_agi.gameplay.quest.groundedEvents,
        g_agi.gameplay.quest.persistenceStatus.c_str());
    DrawText(hdc, railX + 12, viewY + 298, line, COL_TEXT2, g_hFontSmall);

    if (g_agi.gameplay.quest.journalOpen) {
        DrawText(hdc, railX + 12, viewY + 320, "STORY & QUEST JOURNAL (ACTS I-IV)", COL_GOLD, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 336, "1. Meet Ariadne (Act I)", g_agi.gameplay.quest.stage > v19game::QuestStage::Offered ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 352, "2. Recover Signal Shard", g_agi.gameplay.quest.stage > v19game::QuestStage::SeekShard ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 368, "3. Restore Civic Beacon", g_agi.gameplay.quest.stage > v19game::QuestStage::RestoreBeacon ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 384, "4. Receive Resonance Sigil", g_agi.gameplay.quest.stage == v19game::QuestStage::Complete ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        char sideBuf[64];
        snprintf(sideBuf, sizeof(sideBuf), "5. Archive Data Cells (%u/4)", g_agi.gameplay.quest.pickupsCollected);
        DrawText(hdc, railX + 12, viewY + 400, sideBuf, g_agi.gameplay.quest.pickupsCollected >= 4 ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 416, "6. Decrypt Memory Obelisk (Act II)", g_agi.gameplay.quest.obeliskDecrypted ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 432, "7. Attune Transit Relay (Act II)", g_agi.gameplay.quest.relayAttuned ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 448, "8. Purify Shadow Node (Act III)", g_agi.gameplay.quest.shadowNodePurified ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 464, "9. Sync Skyline Tower (Act III)", g_agi.gameplay.quest.skylineTowerSynced ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 480, "10. Awaken Sanctuary Crown (Act IV)", g_agi.gameplay.quest.sanctuaryAwakened ? COL_GREEN : COL_TEXT2, g_hFontSmall);
        DrawText(hdc, railX + 12, viewY + 496, "11. Shard Glow & Crowd Sync", (g_agi.gameplay.quest.shardGlowSeen && g_agi.gameplay.quest.crowdSyncDone) ? COL_GREEN : COL_TEXT2, g_hFontSmall);
    }
    DrawText(hdc, railX + 12, viewY + 522, "CONTROLS & MOTOR TELEMETRY", COL_BRIDGE, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 538, "WASD move / E interact / F3 Map", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 554, "J journal / F5 save / F9 load", COL_TEXT2, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 572,
        g_agi.avatar.wingDeploy>0.9f?"DTE WINGS: GUARD / B FOLD":"DTE WINGS: NEUTRAL / B GUARD",
        COL_CYAN, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 590, g_agi.cns.body().occupied ? "CART OCCUPIED / Q EXIT" : "Q ENTER CART NEAR SPAWN", COL_CYAN, g_hFontSmall);
    snprintf(line,sizeof(line),"NN frames %llu | GNN steps %llu",
        (unsigned long long)g_agi.neuralFrames,(unsigned long long)g_agi.neuralTrainingSteps);
    DrawText(hdc,railX+12,viewY + 610,line,g_agi.neuralPolicyLoaded?COL_GREEN:COL_RED,g_hFontSmall);
    snprintf(line,sizeof(line),"GNN shield %llu / Q %s (%llu)",
        (unsigned long long)g_agi.neuralAccepted, g_agi.navTraining?"ON":"OFF",
        (unsigned long long)g_agi.navLearning.updates);
    DrawText(hdc,railX+12,viewY + 628,line,COL_GOLD,g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 646,
        g_agi.echoCommons.candidate().pattern==160 ? "DTE APL160: REPAIR IDEA / E CONFIRM" :
        "DTE APL253: ADVISORY / LOCAL VRD", COL_CYAN, g_hFontSmall);
    DrawText(hdc, railX + 12, viewY + 664, "SPACE pause / O DTE / F4 ML / F8 S7->8", COL_TEXT2, g_hFontSmall);
    snprintf(line,sizeof(line),"Motion: %s | travelled %.1fm",g_agi.motorStatus(),g_agi.motorTravelMetres);
    DrawText(hdc,railX+12,viewY+684,line,COL_TEXT,g_hFontSmall);
    snprintf(line,sizeof(line),"Skill: %s | checkpoint: %s",
        gtskill::name(g_agi.selectedSkill.skill),g_agi.checkpointStatus.c_str());
    DrawText(hdc,railX+12,viewY+702,line,COL_CYAN,g_hFontSmall);

    // ═══ 5. BOTTOM F6 TELEMETRY DECK (y = 820..992): 3 Side-by-Side F6 Cards ═══
    // 5a. Bottom-Left Card (x = 6..518, y = 820..992): Sensorimotor + Proprioception + ESN Reservoir + VR/Bridge/LLM
    {
        const int rx = 6, ry = 820, rw = 512, rh = 172;
        DrawRect(hdc, rx, ry, rw, rh, COL_CARD);
        DrawText(hdc, rx + 8, ry + 4, "Sensorimotor + Proprioception + ESN Reservoir (96N) + uechovr + Bridge + mech0", COL_CYAN, g_hFontSmall);
        char b1[256];
        snprintf(b1, sizeof(b1), "HMD:(%.1f,%.1f,%.1f) Stereo:%s | Bridge R=%.2f N=%.2f PE=%.2f | LLM Acc=%.2f Loss=%.3f Ep=%d",
            g_agi.hmdPose.x, g_agi.hmdPose.y, g_agi.hmdPose.z, g_agi.stereo.methodName(),
            g_agi.bridgeTelemetry.reward, g_agi.bridgeTelemetry.novelty, g_agi.bridgeTelemetry.prediction_error,
            g_agi.llmMetrics.accuracy, g_agi.llmMetrics.averageLoss, g_agi.llmMetrics.epochs);
        DrawText(hdc, rx + 8, ry + 20, b1, COL_VR, g_hFontSmall);
        const char* pn[8] = {"Pos","Vel","Ori","Acc","Fat","Aro","Pha","Rew"};
        for (int i = 0; i < 8; i++) {
            const int col = i / 4, row = i % 4;
            const int px = rx + 8 + col * 136, py = ry + 38 + row * 16;
            DrawText(hdc, px, py, pn[i], COL_TEXT2, g_hFontSmall);
            DrawBar(hdc, px + 30, py + 3, 72, 8, g_agi.sensorimotor.proprioception[i], COL_PURPLE, RGB(38, 38, 58));
        }
        // ESN 96-neuron strip + 24x24 W_res matrix + 8-neuron Leaky State History waveforms
        auto& st = g_agi.esn.state();
        for (int i = 0; i < 96; i++) {
            const float v = std::min(1.0f, (float)std::fabs(st[i]));
            DrawRect(hdc, rx + 8 + (i % 32) * 8, ry + 106 + (i / 32) * 7, 7, 5,
                RGB((int)(v * 255), (int)(v * 100), (int)(v * 200)));
        }
        auto& W_res = g_agi.esn.W_res();
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 24; j++) {
                const double w = W_res[(i * 4) * 96 + (j * 4)];
                const float v = std::min(1.0f, (float)std::fabs(w) * 2.0f);
                const COLORREF wc = w > 0 ? RGB((int)(v * 255), (int)(v * 50), 0) : RGB(0, (int)(v * 50), (int)(v * 255));
                DrawRect(hdc, rx + 284 + j * 3, ry + 40 + i * 3, 2, 2, wc);
            }
        }
        auto& hist = g_agi.esn.stateHistory();
        const int hidx = g_agi.esn.historyIdx();
        for (int n = 0; n < 8; n++) {
            const int ni = n * 12;
            for (int t = 0; t < 68; t++) {
                const int t1 = (hidx + t) % 100, t2 = (hidx + t + 1) % 100;
                const float v1 = (float)hist[ni][t1], v2 = (float)hist[ni][t2];
                const int x1 = rx + 362 + t * 2, x2 = rx + 362 + (t + 1) * 2;
                const int y1 = ry + 48 + n * 10 + (int)(v1 * 10.0f);
                const int y2 = ry + 48 + n * 10 + (int)(v2 * 10.0f);
                DrawLine(hdc, x1, y1, x2, y2, RGB(100 + n * 20, 150, 255 - n * 20));
            }
        }
        char rwb[160];
        snprintf(rwb, sizeof(rwb), "Rew:%.2f Steps:%d Epi:%d Wout:%.1f TrizD:%+.3f RLS:%d Cpt:%d IP:%d",
            g_agi.sensorimotor.totalReward, g_agi.sensorimotor.stepCount,
            (int)g_agi.episodic.episodes.size(), g_agi.esn.readoutL1(), g_agi.lastTrizDerivative,
            g_agi.esn.rlsUpdates, (int)g_agi.episodic.concepts.size(), g_agi.esn.plasticityUpdates);
        DrawText(hdc, rx + 8, ry + rh - 18, rwb, COL_ACCENT, g_hFontSmall);
    }

    // 5b. Bottom-Center Card (x = 522..942, y = 820..992): Group Dynamics + Micro-Improvement + Introspection
    {
        const int gx = 522, gy = 820, gw = 420, gh = 172;
        DrawRect(hdc, gx, gy, gw, gh, COL_CARD);
        DrawText(hdc, gx + 8, gy + 4, "Group Dynamics (Emergent) + Micro-Improvement & Introspection", COL_CYAN, g_hFontSmall);
        char gb[128];
        snprintf(gb, sizeof(gb), "Agents:%d Groups:%d Coh:%.3f | v%d Kept:%d Best:%.2f SelfAw:%.2f",
            (int)g_agi.groups.agents.size(), g_agi.groups.groupCount(), g_agi.groups.avgCohesion,
            g_agi.improvement.version, g_agi.improvement.kept, g_agi.improvement.best, g_agi.introspect.selfAwareness);
        DrawText(hdc, gx + 8, gy + 20, gb, COL_TEXT, g_hFontSmall);
        const int mox = gx + 8, moy = gy + 38, mw = 148, mh = 126;
        DrawRect(hdc, mox, moy, mw, mh, RGB(18, 18, 36));
        for (const auto& a : g_agi.groups.agents) {
            const int ax = mox + std::clamp((int)((a.x + 64.0f) / 128.0f * mw), 2, mw - 3);
            const int ay = moy + std::clamp((int)((a.y + 64.0f) / 128.0f * mh), 2, mh - 3);
            DrawRect(hdc, ax - 2, ay - 2, 4, 4, a.groupId >= 0 ? COL_GREEN : COL_TEXT2);
        }
        int hy = gy + 40;
        const int hs = (int)g_agi.improvement.history.size();
        for (int i = hs - 1; i >= 0 && i >= hs - 5; i--) {
            DrawText(hdc, gx + 166, hy, g_agi.improvement.history[i].c_str(), COL_TEXT2, g_hFontSmall);
            hy += 18;
        }
        if (!g_agi.introspect.insights.empty())
            DrawText(hdc, gx + 166, gy + gh - 20, g_agi.introspect.insights.back().c_str(), COL_ACCENT, g_hFontSmall);
    }

    // 5c. Bottom-Right Card (x = 946..clientW-6, y = 820..992): CogVerse + AgentLoop + ECAN + Phasor + Echo KSM + CogZero + Layla + Log
    {
        const int bx = 946, by = 820, bw = std::max(480, clientW - bx - 6), bh = 172;
        DrawRect(hdc, bx, by, bw, bh, COL_CARD);
        DrawText(hdc, bx + 8, by + 4, "CogVerse + AgentLoop + ECAN + kys Phasor + Echo KSM + CogZero + Layla", COL_CYAN, g_hFontSmall);
        char eb[256];
        snprintf(eb, sizeof(eb), "Loop:%s Load:%.2f CB:%s Iters:%llu | ECAN Avg:%.3f Util:%.0f%% Focus:%s",
            elizaos::healthName(g_agi.agentLoop.health()), g_agi.agentLoop.load().composite(),
            g_agi.agentLoop.breaker().stateName(), (unsigned long long)g_agi.agentLoop.stats().totalIterations,
            g_agi.avgAttention, g_agi.attention.budget().utilization() * 100.0, g_agi.attentionFocus.c_str());
        DrawText(hdc, bx + 8, by + 22, eb, g_agi.agentLoop.degraded() ? COL_RED : COL_GREEN, g_hFontSmall);
        kys::StepWord triad[3];
        g_agi.phasor.currentTriad(triad);
        snprintf(eb, sizeof(eb), "Phasor T%d [%s %s %s] U1=%s U2=%s | Inv:%d/6 PL=%s T8=%s B7E5R=%s",
            g_agi.phasor.globalTick(), triad[0].name(), triad[1].name(), triad[2].name(),
            kys::uTermName(g_agi.phasor.currentU1()), kys::uTermName(g_agi.phasor.currentU2()),
            g_agi.phasor.invariants().passing(),
            g_agi.phasor.invariants().phaseLock ? "OK" : "NO",
            g_agi.phasor.invariants().t8Coherence ? "OK" : "NO",
            g_agi.phasor.invariants().budget7E5R ? "OK" : "NO");
        DrawText(hdc, bx + 8, by + 40, eb, COL_GOLD, g_hFontSmall);
        const int ksmCyc6 = g_agi.echoField.activeKsmCycle();
        const int wkP = g_agi.echoField.weakestProperty(), stP = g_agi.echoField.strongestProperty();
        snprintf(eb, sizeof(eb), "Echo KSM C%d s=%d (%s) A15:+%s(%.2f) Target:%s(%.2f) Keep=%d/%d",
            ksmCyc6, g_agi.echoSnap.step,
            echo::discoveryPhaseName(g_agi.echoSnap.phase),
            echo::ALEXANDER_15_PROPERTIES[stP].name, g_agi.echoField.propScore(stP),
            echo::ALEXANDER_15_PROPERTIES[wkP].name, g_agi.echoField.propScore(wkP),
            g_agi.echoLedger.keepCount(), g_agi.echoLedger.totalCount());
        DrawText(hdc, bx + 8, by + 58, eb, COL_CYAN, g_hFontSmall);
        snprintf(eb, sizeof(eb), "CogZero c=%d Atoms=%d Tasks=%d/%d (%.1fms) | Layla:%s Q=%d (%.1fms)",
            g_agi.cogzeroStats.cycleNumber, g_agi.cogzeroAgent.atomCount(),
            g_agi.cogzeroAgent.completedTasks(), g_agi.cogzeroAgent.pendingTasks(), g_agi.cogzeroStats.durationMs,
            g_agi.laylaTelemetry.online ? "ONLINE" : "OFFLINE", g_agi.laylaTelemetry.queueDepth, g_agi.laylaTelemetry.lastLatencyMs);
        DrawText(hdc, bx + 8, by + 76, eb, COL_GREEN, g_hFontSmall);
        snprintf(eb, sizeof(eb), "Nav3D Pos(%.1f,%.1f) Goal=%.1fm Contra=%.3f | CogVerse c=%d Coh=%.3f Res=%d",
            g_agi.navPos.x, g_agi.navPos.z, g_agi.navWorld.goalDistance(g_agi.navPos), g_agi.navMeanContradiction,
            g_agi.cogverse.cycle, g_agi.cogverse.propertyCoherence, g_agi.cogverse.residents);
        DrawText(hdc, bx + 8, by + 94, eb, COL_TEXT, g_hFontSmall);
        int lyy = by + 114;
        const int ls = (int)g_agi.log.size();
        for (int i = 0; i < 3 && i < ls; i++) {
            std::string entry = g_agi.log[ls - 1 - i];
            if (entry.size() > 74) entry.resize(74);
            DrawText(hdc, bx + 8, lyy, entry.c_str(), COL_TEXT2, g_hFontSmall);
            lyy += 17;
        }
    }

    // ═══ 6. VIEWPORT OVERLAYS (Compass, Prompts, Dialogue, F3 Map, F4/F8 Inspectors) ═══
    const std::string prompt = g_agi.gameplay.interactionPrompt(g_agi.navPos, g_agi.navHeading, g_agi.navWorld);
    if (!prompt.empty()) {
        DrawRect(hdc, viewX + 164, viewY + 674, 440, 42, RGB(20, 18, 34));
        DrawLine(hdc, viewX + 164, viewY + 674, viewX + 604, viewY + 674, COL_GOLD);
        DrawText(hdc, viewX + 190, viewY + 688, prompt.c_str(), COL_TEXT, g_hFont);
    }
    if (g_agi.worldMapOpen) drawWorldMapPanel(viewX + 96, viewY + 84, 576, true);

    const avatar3d::Vec3 objective = g_agi.gameplay.objectivePosition(g_agi.navWorld);
    const float dx = objective.x - g_agi.navPos.x;
    const float dz = objective.z - g_agi.navPos.z;
    float bearing = atan2f(dx, dz) - g_agi.navHeading;
    while (bearing > 3.14159265f) bearing -= 6.2831853f;
    while (bearing < -3.14159265f) bearing += 6.2831853f;
    const int markerX = viewX + std::clamp(384 + (int)(bearing / 1.2f * 330.0f), 28, 740);
    DrawLine(hdc, markerX - 8, viewY + 28, markerX, viewY + 18, COL_GOLD);
    DrawLine(hdc, markerX, viewY + 18, markerX + 8, viewY + 28, COL_GOLD);
    DrawLine(hdc, markerX + 8, viewY + 28, markerX, viewY + 38, COL_GOLD);
    DrawLine(hdc, markerX, viewY + 38, markerX - 8, viewY + 28, COL_GOLD);

    if (g_agi.gameplay.quest.dialogueOpen) {
        DrawRect(hdc, viewX + 36, viewY + 520, 696, 214, RGB(13, 11, 25));
        DrawLine(hdc, viewX + 36, viewY + 520, viewX + 732, viewY + 520, COL_PURPLE);
        DrawText(hdc, viewX + 58, viewY + 540, "ARIADNE", COL_PURPLE, g_hFontLarge);
        const std::string dialogue = g_agi.gameplay.dialogueLine();
        RECT dialogueRect{viewX + 58, viewY + 574, viewX + 710, viewY + 616};
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COL_TEXT);
        HGDIOBJ previousDialogueFont = SelectObject(hdc, g_hFont);
        ::DrawTextA(hdc, dialogue.c_str(), -1, &dialogueRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(hdc, previousDialogueFont);
        const auto choices = g_agi.gameplay.dialogueChoices();
        if (!choices[0].empty()) DrawText(hdc, viewX + 58, viewY + 620, choices[0].c_str(), COL_GOLD, g_hFont);
        if (!choices[1].empty()) DrawText(hdc, viewX + 58, viewY + 648, choices[1].c_str(), COL_CYAN, g_hFont);
        if (!choices[2].empty()) DrawText(hdc, viewX + 58, viewY + 676, choices[2].c_str(), COL_TEXT2, g_hFont);
    }
    if(g_agi.learningInspectorOpen && !g_agi.gameplay.quest.dialogueOpen && !g_agi.lensInspectorOpen) {
        DrawRect(hdc, viewX + 40, viewY + 52, 686, 320, RGB(13, 18, 32));
        DrawText(hdc, viewX + 56, viewY + 66, "LOCAL LEARNING / F4 CLOSE", COL_CYAN, g_hFontLarge);
        char info[250];
        snprintf(info,sizeof(info),"Loop %s / tick %d / ESN %.3f",!g_agi.running?"STOPPED - SPACE TO START":g_agi.paused?"PAUSED":"RUNNING",g_agi.tickCount,g_agi.esnEnergy);
        DrawText(hdc, viewX + 56, viewY + 104, info, COL_TEXT, g_hFont);
        snprintf(info,sizeof(info),"NN %s / frames %llu / latency %.3f ms",g_agi.neuralPolicyLoaded?
            (g_agi.neuralShadowEnabled?"LEARNING INFERENCE":"DISABLED") : "MODEL MISSING",
            (unsigned long long)g_agi.neuralFrames,g_agi.neuralPrediction.latencyMs);
        DrawText(hdc, viewX + 56, viewY + 132, info, COL_GREEN, g_hFont);
        snprintf(info,sizeof(info),"GNN train %llu / loss %.4f / accepted %llu",
            (unsigned long long)g_agi.neuralTrainingUpdates,g_agi.lastNeuralTrainLoss,
            (unsigned long long)g_agi.neuralAccepted);
        DrawText(hdc, viewX + 56, viewY + 152, info, COL_GREEN, g_hFont);
        snprintf(info,sizeof(info),"Q observation %s / updates %llu / contacts %llu",g_agi.navTraining?"ON":"OFF",
            (unsigned long long)g_agi.navLearning.updates,(unsigned long long)g_agi.navLearning.collisions);
        DrawText(hdc, viewX + 56, viewY + 176, info, COL_GOLD, g_hFont);
        snprintf(info,sizeof(info),"Motor: %s | moved %llu | contact %llu | gated %llu",
            g_agi.motorStatus(),(unsigned long long)g_agi.motorMovedFrames,
            (unsigned long long)g_agi.motorContactFrames,(unsigned long long)g_agi.motorRejectedFrames);
        DrawText(hdc,viewX+56,viewY+204,info,COL_TEXT,g_hFont);
        snprintf(info,sizeof(info),"GNN shield: %s",g_agi.lastGnnGate.c_str());
        DrawText(hdc,viewX+56,viewY+232,info,COL_TEXT,g_hFont);
        snprintf(info,sizeof(info),"Gradient steps %llu | samples %d | travel %.2fm",
            (unsigned long long)g_agi.neuralTrainingSteps,g_agi.gnnMotor.totalSamples(),g_agi.motorTravelMetres);
        DrawText(hdc,viewX+56,viewY+260,info,COL_TEXT,g_hFont);
        snprintf(info,sizeof(info),"Skill: %s (%s)",gtskill::name(g_agi.selectedSkill.skill),g_agi.selectedSkill.reason);
        DrawText(hdc,viewX+56,viewY+286,info,COL_TEXT,g_hFontSmall);
        snprintf(info,sizeof(info),"Checkpoint: %s / %s",g_agi.checkpointStatus.c_str(),g_agi.checkpointId.c_str());
        DrawText(hdc,viewX+56,viewY+310,info,COL_TEXT,g_hFontSmall);
        snprintf(info,sizeof(info),"Controller: %s | candidate learning remains active",
            g_agi.gnnChampionReady?"validated GNN":"DTE baseline");
        DrawText(hdc,viewX+56,viewY+334,info,COL_TEXT,g_hFontSmall);
        snprintf(info,sizeof(info),"Echo: %s / %s / spice %.2f / conf %.2f",
            g_agi.echoPersona.state().emotion,g_agi.echoPersona.state().facs,
            g_agi.echoPersona.state().spice,g_agi.echoPersona.state().confidence);
        DrawText(hdc, viewX + 56, viewY + 348, info, RGB(238,130,238), g_hFontSmall);
    }
    if(g_agi.lensInspectorOpen && !g_agi.gameplay.quest.dialogueOpen) {
        DrawRect(hdc, viewX + 40, viewY + 46, 686, 262, RGB(13, 18, 32));
        DrawLine(hdc, viewX + 40, viewY + 46, viewX + 726, viewY + 46, COL_CYAN);
        DrawText(hdc, viewX + 56, viewY + 62, "SYSTEM 7 -> 8 / DTE PERSPECTIVE LAB", COL_CYAN, g_hFontLarge);
        DrawText(hdc, viewX + 56, viewY + 91, "115 rooted / 23 free -> 286 rooted / 47 free", COL_TEXT, g_hFont);
    }

    BitBlt(rawHdc, 0, 0, clientW, clientH, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(hdc);
}

static void RenderFrame(HDC hdc, RECT& rc) {
    const int clientW = rc.right - rc.left, clientH = rc.bottom - rc.top;
    if (g_agi.highResViewportVisible) {
        RenderV15Frame(hdc, rc);
        return;
    }
    const int W = 1320, H = 800;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, W, H);
    HBITMAP old = (HBITMAP)SelectObject(mem, bmp);
    DrawRect(mem, 0, 0, W, H, COL_BG);

    DrawText(mem, 20, 12, "GTAngel — AGI Gameworld Testing Environment v15", COL_ACCENT, g_hFontLarge);
    DrawText(mem, 20, 42, "mech0 + re-bhvt-edit + GTAngel-Cls + uechovr + dte::EndocrineBus | Echobeats | NicheConstruction | DeepTreeEchoReservoir | TRIZ Arena | Group Dynamics | Micro-Improvement | Echo-Introspect | CogVerse", COL_TEXT2, g_hFont);

    char sb[256];
    snprintf(sb, 256, "Status: %s | Tick: %d | Mode: %s | Echobeats: %s | ESN Energy: %.3f | Mode Transitions: %d | GTAngel-Cls Steps: %llu | VR Frame: %llu | Bridge Niches: %d | LLM Acc: %.2f",
        g_agi.running ? (g_agi.paused ? "PAUSED" : "RUNNING") : "STOPPED",
        g_agi.tickCount, dte::cognitiveModeName(g_agi.endo.cognitiveMode()),
        dte::phaseName(g_agi.echoPhase), g_agi.esnEnergy, g_agi.endo.modeTransitions,
        g_agi.env.steps(), g_agi.tracker.frameCount(), g_agi.bridgeTelemetry.niche_count,
        g_agi.llmMetrics.accuracy);
    if (g_agi.tickCount % 100 == 0 && g_agi.tickCount > 0) {
        char agentLoopLog[200];
        snprintf(agentLoopLog, 200, "AgentLoop: %s | Load=%.2f | CB=%s | Attn=%.3f Focus=%s | Steps=%llu",
            elizaos::healthName(g_agi.agentLoop.health()),
            g_agi.agentLoop.load().composite(),
            g_agi.agentLoop.breaker().stateName(),
            g_agi.avgAttention, g_agi.attentionFocus.c_str(),
            (unsigned long long)g_agi.agentLoop.stats().stepsExecuted);
        g_agi.addLog(agentLoopLog);
        // Phasor status
        kys::StepWord triad[3];
        g_agi.phasor.currentTriad(triad);
        char pb[200];
        snprintf(pb, 200, "Phasor: tick=%d | Triad=[%s %s %s] | U1=%s U2=%s | Invariants=%d/6",
            g_agi.phasor.globalTick(),
            triad[0].name(), triad[1].name(), triad[2].name(),
            kys::uTermName(g_agi.phasor.currentU1()),
            kys::uTermName(g_agi.phasor.currentU2()),
            g_agi.phasor.invariants().passing());
        g_agi.addLog(pb);
        // CogZero status
        char czb[200];
        snprintf(czb, 200, "CogZero: cycle=%d | Atoms=%d | Tasks=%d/%d | Duration=%.1fms",
            g_agi.cogzeroStats.cycleNumber,
            g_agi.cogzeroAgent.atomCount(),
            g_agi.cogzeroAgent.completedTasks(),
            g_agi.cogzeroAgent.pendingTasks(),
            g_agi.cogzeroStats.durationMs);
        g_agi.addLog(czb);
        // Layla status
        char laylaLog[200];
        snprintf(laylaLog, 200, "Layla: %s | Model=%s | Queue=%d | Latency=%.1fms",
            g_agi.laylaTelemetry.online ? "ONLINE" : "OFFLINE",
            g_agi.laylaTelemetry.model.empty() ? "none" : g_agi.laylaTelemetry.model.c_str(),
            g_agi.laylaTelemetry.queueDepth,
            g_agi.laylaTelemetry.lastLatencyMs);
        g_agi.addLog(laylaLog);
        // Echo KSM status
        char eb[200];
        snprintf(eb, 200, "Echo: cycle=%d step=%d (%s) | Load=%.2f | Ledger=%d/%d keep",
            g_agi.echoSnap.cycle, g_agi.echoSnap.step,
            echo::discoveryPhaseName(g_agi.echoSnap.phase),
            g_agi.echoSnap.cognitiveLoad,
            g_agi.echoLedger.keepCount(), g_agi.echoLedger.totalCount());
        g_agi.addLog(eb);
    }
    DrawText(mem, 20, 68, sb, g_agi.running ? COL_GREEN : COL_RED, g_hFont);

    // ── Left: Endocrine System with Decay Tuning ──
    int lx = 20, ly = 100, lw = 330, lh = 420;
    DrawRect(mem, lx, ly, lw, lh, COL_CARD);
    DrawText(mem, lx+10, ly+8, "Virtual Endocrine System (dte::EndocrineBus)", COL_CYAN, g_hFont);
    for (int i = 0; i < (int)dte::HORMONE_COUNT; i++) {
        int row = i / 2, col = i % 2;
        int bx = lx + 12 + col * 158, by = ly + 34 + row * 62;
        if (i == g_agi.selectedHormone) {
            DrawRect(mem, bx-2, by-2, 154, 58, RGB(60, 60, 100));
        }
        DrawText(mem, bx, by, dte::HORMONE_NAMES[i], i == g_agi.selectedHormone ? COL_ACCENT : COL_TEXT2, g_hFont);
        float val = (float)g_agi.endo.hormoneLevel((dte::HormoneId)i);
        COLORREF bc = val > 0.6f ? COL_RED : (val > 0.4f ? COL_ACCENT : COL_GREEN);
        DrawBar(mem, bx, by + 18, 140, 10, val, bc, RGB(40,40,60));
        char hb[64];
        double dr = g_agi.endo.getDecayRate((dte::HormoneId)i);
        snprintf(hb, 64, "%.2f d=%.3f", val, dr);
        DrawText(mem, bx, by + 32, hb, COL_TEXT, g_hFontMono);
    }
    auto& va = g_agi.endo.valenceArousal();
    char vab[128];
    snprintf(vab, 128, "Valence: %+.2f  Arousal: %+.2f  |V-A|: %.2f",
        va.valence, va.arousal, va.magnitude());
    DrawText(mem, lx+12, ly+lh-76, vab, COL_TEXT, g_hFont);
    char mb[64];
    snprintf(mb, 64, "Cognitive Mode: %s", dte::cognitiveModeName(g_agi.endo.cognitiveMode()));
    DrawText(mem, lx+12, ly+lh-52, mb, dte::cognitiveModeColor(g_agi.endo.cognitiveMode()), g_hFont);
    DrawText(mem, lx+12, ly+lh-28, "Mode History:", COL_TEXT2, g_hFont);
    int mhY = ly + lh - 12;
    int mhCount = 0;
    for (int i = (int)g_agi.endo.modeHistory.size()-1; i >= 0 && mhCount < 3; i--, mhCount++) {
        auto& mh = g_agi.endo.modeHistory[i];
        char mhb[64];
        snprintf(mhb, 64, "T%d: %s", mh.first, dte::cognitiveModeName(mh.second));
        DrawText(mem, lx+12 + mhCount*100, mhY, mhb, dte::cognitiveModeColor(mh.second), g_hFontSmall);
    }

    // ── Center: TRIZ Arena with Spatial Contradiction Inspector ──
    int cx = 370, cy = 100, cw = 470, ch = 420;
    DrawRect(mem, cx, cy, cw, ch, COL_CARD);
    DrawText(mem, cx+10, cy+8, "TRIZ Arena + Spatial Contradiction Inspector", COL_CYAN, g_hFont);
    int cs = 26, gox = cx + 40, goy = cy + 36;
    for (int q = 0; q < dte::TRIZArena::SIZE; q++)
        for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
            auto& cell = g_agi.arena.grid[q][r];
            int px = gox + q * (cs + 3) + (r % 2) * (cs/2 + 1);
            int py = goy + r * (cs + 3);
            float v = cell.aestheticValue;
            COLORREF cc;
            if (cell.isNiche) {
                cc = cell.nicheType == dte::NicheType::Perturbation ? COL_NICHE_P :
                     cell.nicheType == dte::NicheType::Scaffolding ? COL_NICHE_S : COL_NICHE_I;
            } else {
                cc = RGB((int)(v*255), (int)(v*190), (int)(v*90));
            }
            if (q == g_agi.arena.maxContradictionQ && r == g_agi.arena.maxContradictionR) {
                DrawRect(mem, px-2, py-2, cs+4, cs+4, COL_CONTRA);
            }
            if (q == g_agi.selectedCellQ && r == g_agi.selectedCellR) {
                DrawRect(mem, px-1, py-1, cs+2, cs+2, COL_ACCENT);
            }
            DrawRect(mem, px, py, cs, cs, cc);
            if (cell.contradiction > 0.1f) {
                int ci = (int)(cell.contradiction * 255);
                DrawRect(mem, px + cs/2 - 2, py + cs/2 - 2, 4, 4, RGB(ci, 0, 0));
            }
        }
    char ab[200];
    snprintf(ab, 200, "Coherence: %.3f | Phase: %s | Discoveries: %d | Niches: %d",
        g_agi.arena.coherence, g_agi.arena.phaseName(), g_agi.arena.discoveries,
        (int)g_agi.arena.niches.size());
    DrawText(mem, cx+10, cy+ch-72, ab, COL_TEXT, g_hFont);
    snprintf(ab, 200, "Max Contradiction: %.3f at (%d,%d) | Last Delta: %.4f",
        g_agi.arena.maxContradiction, g_agi.arena.maxContradictionQ,
        g_agi.arena.maxContradictionR, g_agi.arena.lastDelta);
    DrawText(mem, cx+10, cy+ch-52, ab, COL_TEXT2, g_hFont);
    if (g_agi.selectedCellQ >= 0 && g_agi.selectedCellR >= 0) {
        auto& sc = g_agi.arena.grid[g_agi.selectedCellQ][g_agi.selectedCellR];
        snprintf(ab, 200, "Cell(%d,%d): Aes=%.2f Contra=%.2f Obj=%d LastP=%s",
            g_agi.selectedCellQ, g_agi.selectedCellR, sc.aestheticValue,
            sc.contradiction, sc.objectCount,
            sc.lastPrinciple >= 0 ? g_agi.arena.principles[sc.lastPrinciple] : "none");
        DrawText(mem, cx+10, cy+ch-32, ab, COL_ACCENT, g_hFont);
    } else {
        DrawText(mem, cx+10, cy+ch-32, "Click a cell to inspect | Red dot = high contradiction", COL_TEXT2, g_hFont);
    }

    // ── Right: Sensorimotor + ESN + uechovr 6DOF + re-bhvt-edit Bridge + mech0 LLM ──
    int rx = 860, ry = 100, rw = 420, rh = 420;
    DrawRect(mem, rx, ry, rw, rh, COL_CARD);
    DrawText(mem, rx+10, ry+8, "Sensorimotor + ESN + uechovr + re-bhvt-edit + mech0", COL_CYAN, g_hFont);
    // uechovr 6DOF pose display
    char pose[256];
    snprintf(pose, 256, "HMD: (%.2f, %.2f, %.2f) q=(%.2f,%.2f,%.2f,%.2f)",
        g_agi.hmdPose.x, g_agi.hmdPose.y, g_agi.hmdPose.z,
        g_agi.hmdPose.qx, g_agi.hmdPose.qy, g_agi.hmdPose.qz, g_agi.hmdPose.qw);
    DrawText(mem, rx+12, ry+30, pose, COL_VR, g_hFontMono);
    snprintf(pose, 256, "L-Ctrl: (%.2f, %.2f, %.2f)  R-Ctrl: (%.2f, %.2f, %.2f)",
        g_agi.leftCtrl.x, g_agi.leftCtrl.y, g_agi.leftCtrl.z,
        g_agi.rightCtrl.x, g_agi.rightCtrl.y, g_agi.rightCtrl.z);
    DrawText(mem, rx+12, ry+48, pose, COL_VR, g_hFontMono);
    snprintf(pose, 256, "Stereo: %s | IPD: %.3f | Eye: %dx%d",
        g_agi.stereo.methodName(), g_agi.stereo.getEyeParams(0).ipd,
        g_agi.stereo.getEyeParams(0).width, g_agi.stereo.getEyeParams(0).height);
    DrawText(mem, rx+12, ry+66, pose, COL_VR, g_hFontMono);
    // re-bhvt-edit bridge telemetry
    char bridge[256];
    snprintf(bridge, 256, "Bridge: R=%.3f N=%.3f PE=%.3f T=%.3f Niches=%d",
        g_agi.bridgeTelemetry.reward, g_agi.bridgeTelemetry.novelty,
        g_agi.bridgeTelemetry.prediction_error, g_agi.bridgeTelemetry.threat,
        g_agi.bridgeTelemetry.niche_count);
    DrawText(mem, rx+12, ry+84, bridge, COL_BRIDGE, g_hFontMono);
    // mech0 LLM metrics
    char llm[256];
    snprintf(llm, 256, "LLM: Acc=%.2f Loss=%.3f Epochs=%d Batches=%d",
        g_agi.llmMetrics.accuracy, g_agi.llmMetrics.averageLoss,
        g_agi.llmMetrics.epochs, g_agi.llmMetrics.batches);
    DrawText(mem, rx+12, ry+102, llm, COL_LLM, g_hFontMono);
    // Frame buffer visualization
    int fb = 4, fox = rx + 16, foy = ry + 126;
    for (int y = 0; y < dte::SensorimotorLoop::FH; y += 4)
        for (int x = 0; x < dte::SensorimotorLoop::FW; x += 4) {
            float v = g_agi.sensorimotor.frameBuffer[y*dte::SensorimotorLoop::FW+x];
            DrawRect(mem, fox + x/4*fb, foy + y/4*fb, fb-1, fb-1,
                RGB((int)(v*100), (int)(v*150), (int)(v*255)));
        }
    const char* pn[8] = {"Pos","Vel","Orient","Accel","Fatigue","Arousal","Phase","Reward"};
    for (int i = 0; i < 8; i++) {
        int by = ry + 206 + i * 22;
        DrawText(mem, rx+12, by, pn[i], COL_TEXT2, g_hFont);
        DrawBar(mem, rx+80, by, 200, 10, g_agi.sensorimotor.proprioception[i], COL_PURPLE, RGB(40,40,60));
        char pb[32]; snprintf(pb, 32, "%.3f", g_agi.sensorimotor.proprioception[i]);
        DrawText(mem, rx+290, by, pb, COL_TEXT, g_hFontMono);
    }
    DrawText(mem, rx+12, ry+382, "ESN Reservoir (96 neurons, leak=0.3, sr=0.95):", COL_TEXT2, g_hFont);
    auto& st = g_agi.esn.state();
    for (int i = 0; i < 96; i++) {
        float v = (float)fabs(st[i]);
        v = std::min(1.0f, v);
        DrawRect(mem, rx+12 + (i%24)*16, ry+402 + (i/24)*8, 14, 6,
            RGB((int)(v*255), (int)(v*100), (int)(v*200)));
    }
    DrawText(mem, rx+12, ry+438, "W_res (sampled 24x24):", COL_TEXT2, g_hFont);
    auto& W_res = g_agi.esn.W_res();
    int wms = 4;
    for (int i = 0; i < 24; i++)
        for (int j = 0; j < 24; j++) {
            int si = i * 4, sj = j * 4;
            double w = W_res[si * 96 + sj];
            float v = (float)fabs(w);
            v = std::min(1.0f, v * 2.0f);
            COLORREF wc = w > 0 ? RGB((int)(v*255), (int)(v*50), 0) : RGB(0, (int)(v*50), (int)(v*255));
            DrawRect(mem, rx+12 + j*wms, ry+458 + i*wms, wms-1, wms-1, wc);
        }
    DrawText(mem, rx+220, ry+438, "Leaky State History (8 neurons):", COL_TEXT2, g_hFont);
    auto& hist = g_agi.esn.stateHistory();
    int hidx = g_agi.esn.historyIdx();
    for (int n = 0; n < 8; n++) {
        int ni = n * 12;
        for (int t = 0; t < 99; t++) {
            int t1 = (hidx + t) % 100, t2 = (hidx + t + 1) % 100;
            float v1 = (float)hist[ni][t1], v2 = (float)hist[ni][t2];
            int x1 = rx + 220 + t * 2, x2 = rx + 220 + (t+1) * 2;
            int y1 = ry + 458 + n * 6 + (int)(v1 * 20);
            int y2 = ry + 458 + n * 6 + (int)(v2 * 20);
            DrawLine(mem, x1, y1, x2, y2, RGB(100 + n*20, 150, 255 - n*20));
        }
    }
    char rwb[128];
    snprintf(rwb, 128, "Reward: %.3f | Steps: %d | Epi: %d | Wout: %.2f | TrizD: %+.3f | RLS: %d | Cpt: %d | IP: %d",
        g_agi.sensorimotor.totalReward, g_agi.sensorimotor.stepCount,
        (int)g_agi.episodic.episodes.size(), g_agi.esn.readoutL1(), g_agi.lastTrizDerivative,
        g_agi.esn.rlsUpdates, (int)g_agi.episodic.concepts.size(), g_agi.esn.plasticityUpdates);
    DrawText(mem, rx+12, ry+rh-14, rwb, COL_ACCENT, g_hFont);

    // ── Bottom Left: Group Dynamics ──
    int gx = 20, gy = 540, gw = 330, gh = 190;
    DrawRect(mem, gx, gy, gw, gh, COL_CARD);
    DrawText(mem, gx+10, gy+8, "Group Dynamics (Emergent)", COL_CYAN, g_hFont);
    char gb[128];
    snprintf(gb, 128, "Agents: %d | Groups: %d | Cohesion: %.3f",
        (int)g_agi.groups.agents.size(), g_agi.groups.groupCount(), g_agi.groups.avgCohesion);
    DrawText(mem, gx+10, gy+32, gb, COL_TEXT, g_hFont);
    int mox = gx+10, moy = gy+56, mw = gw-20, mh = gh-66;
    DrawRect(mem, mox, moy, mw, mh, RGB(20,20,40));
    for (auto& a : g_agi.groups.agents) {
        int ax = mox + (int)((a.x+64.0f)/128.0f*mw), ay = moy + (int)((a.y+64.0f)/128.0f*mh);
        DrawRect(mem, ax-2, ay-2, 5, 5, a.groupId >= 0 ? COL_GREEN : COL_TEXT2);
    }

    // ── Bottom Center: Micro-Improvement + Introspection ──
    int mx = 370, my = 540, mw2 = 470, mh2 = 190;
    DrawRect(mem, mx, my, mw2, mh2, COL_CARD);
    DrawText(mem, mx+10, my+8, "Micro-Improvement + Echo Introspection", COL_CYAN, g_hFont);
    char mib[160];
    snprintf(mib, 160, "v%d | Kept: %d | Best: %.3f | Self-Awareness: %.3f",
        g_agi.improvement.version, g_agi.improvement.kept,
        g_agi.improvement.best, g_agi.introspect.selfAwareness);
    DrawText(mem, mx+10, my+32, mib, COL_TEXT, g_hFont);
    int hy = my + 56;
    int hs = (int)g_agi.improvement.history.size();
    for (int i = hs-1; i >= 0 && i >= hs-4; i--) {
        DrawText(mem, mx+10, hy, g_agi.improvement.history[i].c_str(), COL_TEXT2, g_hFontMono);
        hy += 17;
    }
    if (!g_agi.introspect.insights.empty())
        DrawText(mem, mx+10, my+mh2-26, g_agi.introspect.insights.back().c_str(), COL_ACCENT, g_hFont);

    // ── Bottom Right: CogVerse + Log ──
    int bx = 860, by = 540, bw = 420, bh = 190;
    DrawRect(mem, bx, by, bw, bh, COL_CARD);
    DrawText(mem, bx+10, by+8, "CogVerse + elizaos AgentLoop + ECAN", COL_CYAN, g_hFont);
    {
        char eb[256];
        snprintf(eb, 256, "Loop: %s | Load: %.2f | CB: %s | Iters: %llu",
            elizaos::healthName(g_agi.agentLoop.health()),
            g_agi.agentLoop.load().composite(),
            g_agi.agentLoop.breaker().stateName(),
            (unsigned long long)g_agi.agentLoop.stats().totalIterations);
        DrawText(mem, bx+10, by+32, eb, g_agi.agentLoop.degraded() ? COL_RED : COL_GREEN, g_hFont);
        snprintf(eb, 256, "ECAN: Avg=%.3f Budget=%.0f%% Focus=%s",
            g_agi.avgAttention,
            g_agi.attention.budget().utilization() * 100.0,
            g_agi.attentionFocus.c_str());
        DrawText(mem, bx+10, by+50, eb, COL_VR, g_hFont);
        auto* w = g_agi.attention.window(g_agi.attentionFocus);
        if (w) {
            snprintf(eb, 256, "Temporal: I=%.2f S=%.2f M=%.2f L=%.2f Comp=%.3f",
                w->immediate, w->shortTerm, w->mediumTerm, w->longTerm, w->composite());
            DrawText(mem, bx+10, by+68, eb, COL_BRIDGE, g_hFont);
    // kys Triad Phasor Engine panel
    {
        kys::StepWord triad[3];
        g_agi.phasor.currentTriad(triad);
        char pb[256];
        snprintf(pb, 256, "Phasor: tick=%d | Triad=[%s %s %s] | U1=%s U2=%s",
            g_agi.phasor.globalTick(),
            triad[0].name(), triad[1].name(), triad[2].name(),
            kys::uTermName(g_agi.phasor.currentU1()),
            kys::uTermName(g_agi.phasor.currentU2()));
        DrawText(mem, bx+10, by+88, pb, COL_GOLD, g_hFont);
        snprintf(pb, 256, "Invariants: %d/6 | PhaseLock=%s T8=%s Budget=%s Mode=%s",
            g_agi.phasor.invariants().passing(),
            g_agi.phasor.invariants().phaseLock ? "OK" : "FAIL",
            g_agi.phasor.invariants().t8Coherence ? "OK" : "FAIL",
            g_agi.phasor.invariants().budget7E5R ? "OK" : "FAIL",
            g_agi.phasor.invariants().modeBalance ? "OK" : "FAIL");
        DrawText(mem, bx+10, by+106, pb, g_agi.phasor.invariants().passing() >= 5 ? COL_GREEN : COL_RED, g_hFont);
    // echo KSM panel
    {
        char eb[256];
        snprintf(eb, 256, "Echo: cycle=%d step=%d (%s) | Mode=%s",
            g_agi.echoSnap.cycle, g_agi.echoSnap.step,
            echo::discoveryPhaseName(g_agi.echoSnap.phase),
            echo::cognitiveModeName(g_agi.echoSnap.mode));
        DrawText(mem, bx+10, by+128, eb, COL_GOLD, g_hFont);
        snprintf(eb, 256, "Load=%.2f | Alignment=%.3f | Ledger=%d/%d keep",
            g_agi.echoSnap.cognitiveLoad,
            g_agi.echoSnap.alignment,
            g_agi.echoLedger.keepCount(), g_agi.echoLedger.totalCount());
        DrawText(mem, bx+10, by+146, eb, g_agi.echoSnap.cognitiveLoad > 0.7f ? COL_RED : COL_GREEN, g_hFont);
    // cogzero panel
    {
        char czb[256];
        snprintf(czb, 256, "CogZero: cycle=%d | Atoms=%d | Tasks=%d/%d",
            g_agi.cogzeroStats.cycleNumber,
            g_agi.cogzeroAgent.atomCount(),
            g_agi.cogzeroAgent.completedTasks(),
            g_agi.cogzeroAgent.pendingTasks());
        DrawText(mem, bx+10, by+168, czb, COL_GOLD, g_hFont);
        snprintf(czb, 256, "Duration=%.1fms | Percepts=%d | Conclusions=%d",
            g_agi.cogzeroStats.durationMs,
            g_agi.cogzeroStats.perceptsProcessed,
            g_agi.cogzeroStats.conclusions);
        DrawText(mem, bx+10, by+186, czb, g_agi.cogzeroStats.durationMs > 10.0f ? COL_RED : COL_GREEN, g_hFont);
    // layla panel
    {
        char laylaPanel[256];
        snprintf(laylaPanel, 256, "Layla: %s | Model=%s | Queue=%d",
            g_agi.laylaTelemetry.online ? "ONLINE" : "OFFLINE",
            g_agi.laylaTelemetry.model.empty() ? "none" : g_agi.laylaTelemetry.model.c_str(),
            g_agi.laylaTelemetry.queueDepth);
        DrawText(mem, bx+10, by+208, laylaPanel, g_agi.laylaTelemetry.online ? COL_GREEN : COL_RED, g_hFont);
        snprintf(laylaPanel, 256, "Latency=%.1fms | Requests=%d | Failures=%d",
            g_agi.laylaTelemetry.lastLatencyMs,
            g_agi.laylaTelemetry.requests,
            g_agi.laylaTelemetry.failures);
        DrawText(mem, bx+10, by+226, laylaPanel, g_agi.laylaTelemetry.failures > 0 ? COL_RED : COL_GREEN, g_hFont);
    }
    // navigation panel (v13)
    {
        char nb[256];
        snprintf(nb, 256, "Nav3D: %s | Pos=(%.1f,%.1f) | Goal=%.1fm | Contradiction=%.3f",
            g_agi.navTraining ? "OBSERVING" : "Q OFF",
            g_agi.navPos.x, g_agi.navPos.z,
            g_agi.navWorld.goalDistance(g_agi.navPos),
            g_agi.navMeanContradiction);
        DrawText(mem, bx+10, by+248, nb, g_agi.navTraining ? COL_GREEN : COL_TEXT2, g_hFont);
        snprintf(nb, 256, "Q Observe: updates=%llu | Windows=%llu | Contacts=%llu | Ret=%.2f",
            (unsigned long long)g_agi.navLearning.updates,
            (unsigned long long)g_agi.navLearning.windows,
            (unsigned long long)g_agi.navLearning.collisions,
            g_agi.navTrainer.recentMeanReturn());
        DrawText(mem, bx+10, by+266, nb,
            g_agi.navTrainer.successRate() > 0.5f ? COL_GREEN : COL_GOLD, g_hFont);
    }
    }
    }
    }
        }
    }
    char cvb[128];
    snprintf(cvb, 128, "Cycle: %d | Property Coherence: %.3f | Residents: %d",
        g_agi.cogverse.cycle, g_agi.cogverse.propertyCoherence, g_agi.cogverse.residents);
    DrawText(mem, bx+10, by+248, cvb, COL_TEXT, g_hFont);
    int lyy = by + 268;
    int ls = (int)g_agi.log.size();
    for (int i = 0; i < 2 && i < ls; i++) {
        DrawText(mem, bx+10, lyy, g_agi.log[ls-1-i].c_str(), COL_TEXT2, g_hFontMono);
        lyy += 17;
    }

    // v13: egocentric framebuffer viewport (avatar's actual view, 2x scale)
    {
        int vx = 20, vy = H - 26 - 140;
        DrawText(mem, vx, vy - 16, "Egocentric Framebuffer (64x64 -> TRIZ 12x12)", COL_TEXT2, g_hFontSmall);
        for (int y = 0; y < 64; y++)
            for (int x = 0; x < 64; x++) {
                float v = g_agi.navRenderer.frame[y * 64 + x];
                float sem = g_agi.navRenderer.semantic[y * 64 + x];
                COLORREF c;
                if (sem > 0.9f) c = RGB(0, 255, 120);        // goal
                else if (sem > 0.6f) c = RGB(255, 80, 60);   // obstacle
                else if (sem > 0.4f) c = RGB(120, 180, 255); // avatar
                else if (sem > 0.2f) c = RGB(60, 60, 80);    // ground
                else c = RGB((int)(v * 40), (int)(v * 40), (int)(v * 60));
                DrawRect(mem, vx + x * 2, vy + y * 2, 2, 2, c);
            }
        // TRIZ grid overlay (12x12)
        for (int g = 0; g <= 12; g++) {
            DrawLine(mem, vx + g * 10, vy, vx + g * 10, vy + 128, RGB(50, 50, 70));
            DrawLine(mem, vx, vy + g * 10, vx + 128, vy + g * 10, RGB(50, 50, 70));
        }
        // Highlight max-contradiction cell
        if (g_agi.trizMap.maxQ >= 0) {
            int hx = vx + g_agi.trizMap.maxQ * 10, hy = vy + g_agi.trizMap.maxR * 10;
            DrawLine(mem, hx, hy, hx + 10, hy, COL_CONTRA);
            DrawLine(mem, hx + 10, hy, hx + 10, hy + 10, COL_CONTRA);
            DrawLine(mem, hx + 10, hy + 10, hx, hy + 10, COL_CONTRA);
            DrawLine(mem, hx, hy + 10, hx, hy, COL_CONTRA);
        }
    }
    DrawText(mem, 20, H-26, "[SPACE] Run | [R] Reset | [N] Nav | [V] View | [S] Spine | [T] Overlay | [E] Emit Now | [ESC] Exit", COL_TEXT2, g_hFont);

    const int dashboardH = std::min(clientH, (clientW * H) / W);
    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, 0, 0, clientW, dashboardH, mem, 0, 0, W, H, SRCCOPY);
    DrawRect(hdc, 0, dashboardH, clientW, std::max(0, clientH-dashboardH), COL_BG);
    if (g_agi.civicOverlayVisible && clientH - dashboardH >= 104) {
        const int sx = 8, sy = dashboardH + 5, sw = clientW - 16;
        DrawRect(hdc, sx, sy, sw, clientH-sy-7, RGB(18, 22, 42));
        DrawText(hdc, sx+10, sy+6, "CIVIC ANGEL SPINE / grounded authority evidence", COL_BRIDGE, g_hFont);
        const auto embodied = g_agi.civicSpine.embodiedTelemetry();
        const auto purpose = g_agi.civicSpine.purposeTelemetry();
        const auto expression = g_agi.civicSpine.expressionTelemetry();
        auto drawProducer = [&](int y, const char* label, const civicangel::ProducerTelemetry& t, COLORREF color) {
            char line[512];
            snprintf(line, sizeof(line), "%s  %s  accepted=%lu fail=%lu queue=%lu HTTP=%lu %.1fms trace=%s",
                label, t.configured ? (t.busy ? "BUSY" : "READY") : "UNCONFIGURED",
                t.accepted, t.failures, t.queued, t.lastStatus, t.lastLatencyMs,
                t.lastTraceId.empty() ? "none" : t.lastTraceId.c_str());
            DrawText(hdc, sx+10, y, line, t.failures ? COL_RED : color, g_hFontMono);
        };
        drawProducer(sy+30, "GTAngel / EMBODIED", embodied, COL_CYAN);
        drawProducer(sy+52, "Ontogentelechy / PURPOSE", purpose, COL_GOLD);
        drawProducer(sy+74, "Meta Echo DNA / EXPRESSION", expression, COL_BRIDGE);
        DrawText(hdc, sx+10, sy+98, "[S] stream  [E] immediate grounded frame  [T] hide overlay", COL_TEXT2, g_hFontSmall);
    } else if (!g_agi.civicOverlayVisible) {
        DrawText(hdc, 10, dashboardH+10, "Civic Angel overlay hidden — [T] show", COL_TEXT2, g_hFontSmall);
    }
    SelectObject(mem, old); DeleteObject(bmp); DeleteDC(mem);
}


// v24 presentation plane: intentionally isolated from the AI perception pyramid.
static v15render::DisplayRenderer g_displayRenderer;
static HWND g_displayWindow=nullptr;
static LRESULT CALLBACK DisplayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
      case WM_PAINT: {
        PAINTSTRUCT ps; HDC dc=BeginPaint(hwnd,&ps);
        RECT area{}; GetClientRect(hwnd,&area);
        const int width=area.right-area.left, height=area.bottom-area.top;
        DrawRect(dc,0,0,width,height,COL_BG);
        // Read the same local plant/scene, never the 768x768 sensor pixels.
        const avatar3d::Vec3 pos=g_agi.navPos;
        const float heading=g_agi.navHeading;
        const bool third=g_agi.navThirdPerson;
        const auto eye=third ? g_agi.navWorld.collisionFreeEye(pos,pos+avatar3d::Vec3{-sinf(heading)*4.0f,2.5f,-cosf(heading)*4.0f})
                             : pos+avatar3d::Vec3{0,1.6f,0};
        g_displayRenderer.render(g_agi.navWorld,g_agi.avatar,eye,heading,third?-0.4f:0.0f,third);
        const auto& display=g_displayRenderer.framebuffer;
        const int targetW=std::max(1,std::min(width,(int)std::lround(height*16.0/9.0)));
        const int targetH=std::max(1,std::min(height,(int)std::lround(width*9.0/16.0)));
        const int x=(width-targetW)/2, y=(height-targetH)/2;
        BITMAPINFO bitmap{};
        bitmap.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bitmap.bmiHeader.biWidth=v15render::DisplayFramebuffer::Width;
        bitmap.bmiHeader.biHeight=-v15render::DisplayFramebuffer::Height;
        bitmap.bmiHeader.biPlanes=1;
        bitmap.bmiHeader.biBitCount=32;
        bitmap.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,HALFTONE);
        StretchDIBits(dc,x,y,targetW,targetH,0,0,display.Width,display.Height,
                      display.colour.data(),&bitmap,DIB_RGB_COLORS,SRCCOPY);
        DrawText(dc,x+12,y+12,"DISPLAY 1536x864 / AI SENSOR 768x768 / F7 CLOSE",COL_CYAN,g_hFontSmall);
        EndPaint(hwnd,&ps); return 0;
      }
      case WM_KEYDOWN:
        if(wp==VK_F7 || wp==VK_ESCAPE) {DestroyWindow(hwnd);return 0;}
        if(HWND owner=GetWindow(hwnd,GW_OWNER)) SendMessageA(owner,WM_KEYDOWN,wp,lp);
        return 0;
      case WM_CLOSE: DestroyWindow(hwnd); return 0;
      case WM_DESTROY: g_displayWindow=nullptr; return 0; // main game still runs
    }
    return DefWindowProcA(hwnd,msg,wp,lp);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            g_hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_SWISS, "Arial");
            g_hFontLarge = CreateFontA(22, 0, 0, 0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_SWISS, "Arial");
            g_hFontMono = CreateFontA(12, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_MODERN, "Consolas");
            g_hFontSmall = CreateFontA(10, 0, 0, 0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH|FF_MODERN, "Consolas");
            g_agi.highResRenderer.loadDteAsset(""); // adjacent executable/assets path only
            g_agi.navWorld.generate(1337,48);
            g_agi.gameplay.reset(g_agi.navWorld);
            g_agi.cns.reset();
            g_agi.invalidateEcho();
            g_agi.syncBodyFromCns();
            g_agi.avatar.pose(g_agi.navPos,g_agi.navHeading,0.0f,0.0f);
            g_agi.highResRenderer.render(g_agi.navWorld,g_agi.avatar,
                g_agi.navPos+avatar3d::Vec3{0,2.5f,-4.0f},0,-0.4f,true);
            g_agi.perceptionPyramid.derive(g_agi.highResRenderer.framebuffer);
            SetTimer(hwnd, 1, 50, NULL);
            g_agi.groups.init(20);
            g_agi.fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
            g_agi.tracker.reset();
            g_agi.stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
            g_agi.initializeLearning(); // same bootstrap on WM_CREATE and HOME reset
            if (g_agi.loadLearningState()) g_agi.addLog("session continues from persisted learning state");
            g_agi.civicSpine.configureFromEnvironment();
            g_agi.addLog("GTAngel AGI v14 initialized: grounded Civic Angel producer spine ready");
            return 0;
        case WM_TIMER:
            g_agi.tick();
            InvalidateRect(hwnd, NULL, FALSE);
            static uint32_t displayDivisor=0;
            if (g_displayWindow && IsWindow(g_displayWindow) && (++displayDivisor % 2u)==0u)
                InvalidateRect(g_displayWindow,NULL,FALSE);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            RenderFrame(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN:
            if (wp == VK_SPACE) {
                if (!g_agi.running) { g_agi.running = true; g_agi.addLog("Started"); }
                else { g_agi.paused = !g_agi.paused; g_agi.addLog(g_agi.paused ? "Paused" : "Resumed"); }
            } else if (wp == VK_HOME) {
                g_agi.reset();
                g_agi.groups.init(20);
                g_agi.fbHook.initialize(uechovr::FrameBufferHook::API::D3D11, 64, 64);
                g_agi.tracker.reset();
                g_agi.stereo.setMethod(uechovr::StereoRenderer::Method::NativeStereo);
                g_agi.addLog("Reset");
            } else if (wp == VK_TAB) {
                g_agi.selectedHormone = (g_agi.selectedHormone + 1) % (int)dte::HORMONE_COUNT;
            } else if (wp == VK_OEM_PLUS || wp == VK_ADD) {
                auto hid = (dte::HormoneId)g_agi.selectedHormone;
                double dr = g_agi.endo.getDecayRate(hid);
                g_agi.endo.setDecayRate(hid, std::min(0.5, dr + 0.01));
                char buf[64]; snprintf(buf, 64, "Decay %s: %.3f", dte::HORMONE_NAMES[g_agi.selectedHormone], dr + 0.01);
                g_agi.addLog(buf);
            } else if (wp == VK_OEM_MINUS || wp == VK_SUBTRACT) {
                auto hid = (dte::HormoneId)g_agi.selectedHormone;
                double dr = g_agi.endo.getDecayRate(hid);
                g_agi.endo.setDecayRate(hid, std::max(0.001, dr - 0.01));
                char buf[64]; snprintf(buf, 64, "Decay %s: %.3f", dte::HORMONE_NAMES[g_agi.selectedHormone], dr - 0.01);
                g_agi.addLog(buf);
            } else if (wp == 'P') {
                g_agi.peerLoop.setEnabled(!g_agi.peerLoop.enabled());
                g_agi.addLog(g_agi.peerLoop.enabled()
                    ? "v18 peer attention enabled; local confirmation required"
                    : "v18 peer attention disabled");
            } else if (wp == 'M') {
                g_agi.neuralShadowEnabled = !g_agi.neuralShadowEnabled;
                g_agi.neuralPolicy.reset();
                g_agi.addLog(g_agi.neuralShadowEnabled ? "v17 neural LEARNING enabled" : "v17 neural inference disabled");
            } else if (wp == 'C') {
                if (g_agi.trajectoryRecording) {
                    const bool stopped = g_agi.trajectoryRecorder.stop();
                    g_agi.trajectoryRecording = false;
                    g_agi.addLog(stopped
                        ? std::string("v16 capture stopped: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture close failed: ") + g_agi.trajectoryRecorder.lastError());
                } else {
                    const bool started = g_agi.trajectoryRecorder.start(
                        g_agi.localLearningDirectory+"/datasets", g_agi.trajectoryEpisodeId++);
                    g_agi.trajectoryRecording = started;
                    g_agi.addLog(started
                        ? std::string("v16 capture started: ") + g_agi.trajectoryRecorder.path()
                        : std::string("v16 capture failed: ") + g_agi.trajectoryRecorder.lastError());
                }
            } else if (wp == 'F') {
                int mode = ((int)g_agi.highResDisplayMode + 1) % 4;
                g_agi.highResDisplayMode = (v15render::DisplayMode)mode;
                g_agi.addLog(std::string("v15 display: ") + v15render::displayModeName(g_agi.highResDisplayMode));
            } else if (wp == VK_F7) {
                if (g_displayWindow && IsWindow(g_displayWindow)) {
                    DestroyWindow(g_displayWindow);
                } else {
                    g_displayRenderer.loadDteAsset(""); // its own validated local skin copy
                    RECT dimensions={0,0,std::min(1536,std::max(640,GetSystemMetrics(SM_CXSCREEN)-100)),
                                          std::min(864,std::max(480,GetSystemMetrics(SM_CYSCREEN)-120))};
                    AdjustWindowRect(&dimensions,WS_OVERLAPPEDWINDOW,FALSE);
                    g_displayWindow=CreateWindowExA(0,"GTAngelVisualDisplayV24",
                        "GTAngel v24 - high-resolution display only (AI: 768x768)",
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,80,60,
                        dimensions.right-dimensions.left,dimensions.bottom-dimensions.top,
                        hwnd,NULL,GetModuleHandleA(NULL),NULL);
                    if(g_displayWindow) ShowWindow(g_displayWindow,SW_SHOWNORMAL);
                }
            } else if (wp == 'O') {
                g_agi.dteEmbodimentEnabled=!g_agi.dteEmbodimentEnabled;
                g_agi.addLog(g_agi.dteEmbodimentEnabled?"DTE local embodied operator ON":"DTE embodiment paused; debug input only");
            } else if (wp == VK_F4) {
                g_agi.learningInspectorOpen=!g_agi.learningInspectorOpen;
                g_agi.addLog(g_agi.learningInspectorOpen?"Learning monitor OPEN":"Learning monitor CLOSED");
            } else if (wp == VK_F3) {
                g_agi.worldMapOpen = !g_agi.worldMapOpen;
                g_agi.addLog(g_agi.worldMapOpen ? "128m World Map OPEN (F3)" : "128m World Map CLOSED");
            } else if (wp == VK_F8) {
                g_agi.lensInspectorOpen=!g_agi.lensInspectorOpen;
                g_agi.addLog(g_agi.lensInspectorOpen?"System 7->8 lens inspector OPEN (advisory)":
                                                   "System 7->8 lens inspector CLOSED");
            } else if (wp == VK_F6) {
                g_agi.highResViewportVisible = !g_agi.highResViewportVisible;
                g_agi.addLog(g_agi.highResViewportVisible ? "v15 viewport: VISIBLE" : "v14 dashboard: VISIBLE");
            } else if (wp == 'N') {
                g_agi.navTraining = !g_agi.navTraining;
                g_agi.addLog(g_agi.navTraining ? "Q observation ON: accepted local actions only" : "Q observation OFF; human navigation unaffected");
            } else if (wp == 'V') {
                g_agi.navThirdPerson = !g_agi.navThirdPerson;
                g_agi.addLog(g_agi.navThirdPerson ? "Camera: third-person" : "Camera: first-person");
            } else if (wp == 'T') {
                g_agi.civicOverlayVisible = !g_agi.civicOverlayVisible;
                g_agi.addLog(g_agi.civicOverlayVisible ? "Civic Angel overlay: VISIBLE" : "Civic Angel overlay: HIDDEN");
            } else if (wp == VK_F10) {
                g_agi.civicSpine.setEnabled(!g_agi.civicSpine.enabled());
                g_agi.addLog(g_agi.civicSpine.enabled() ? "Civic Angel spine: ON" : "Civic Angel spine: OFF");
            } else if (wp == VK_F11) {
                g_agi.civicSpine.requestImmediate();
                g_agi.addLog("Civic Angel: immediate grounded frame requested");
            } else if (wp == 'M') {
                std::string prom = g_agi.agentLoop.toPrometheus();
                std::ofstream f("C:\\ddd\\GTAngel\\native\\GTAngel_v56\\gtangel_metrics.prom");
                f << prom;
                f.close();
                g_agi.addLog("Prometheus metrics exported to gtangel_metrics.prom");
            } else if (wp == 'B') {
                if (g_agi.cns.body().occupied) g_agi.addLog("DTE motion preview requires on-foot embodiment");
                else g_agi.addLog(g_agi.avatar.motion==avatar3d::AvatarSkeleton::Motion::Guard
                    ? (g_agi.avatar.releaseCombat()?"DTE wings relatching / visual recovery":"DTE motion held")
                    : (g_agi.avatar.triggerCombat()?"DTE combat-entry pose / visual-only":"DTE entry in progress"));
            } else if (wp == 'I') {
                g_agi.avatar.inspect=!g_agi.avatar.inspect;
                g_agi.addLog(g_agi.avatar.inspect?"DTE explore / inspect overlay":"DTE scan overlay off");
            } else if (wp == 'Q') {
                g_agi.addLog(g_agi.cns.toggleCart(g_agi.navWorld,g_agi.navWorld.vehiclePos)
                    ? "Local cart enter/exit accepted" : "Local cart enter/exit rejected");
                g_agi.syncBodyFromCns();
            } else if (wp == 'E') {
                if (g_agi.gameplay.canRestore(g_agi.navPos,g_agi.navWorld)) {
                    // The DTE candidate only recommends an E interaction; v19
                    // operator play still works if the advisory is absent/stale.
                    const bool echoConfirmed=g_agi.echoSample && g_agi.echoCommons.canConfirm(
                        g_agi.echoIngress,*g_agi.echoSample,(uint64_t)GetTickCount64(),
                        g_agi.cns.body(),g_agi.cns.niche(),g_agi.gameplay,g_agi.navWorld);
                    const bool accepted=g_agi.repairBeacon(); // ONLY WGR mints NICHE_COMMIT
                    g_agi.echoCommons.clear(); g_agi.echoSample.reset();
                    g_agi.addLog(accepted ?
                        (echoConfirmed ? "Echo Commons APL160 advised; local E/WGR committed and re-observed"
                                       : "Local E/WGR restored beacon without DTE prerequisite") :
                        "Beacon repair rejected by WGR");
                }
                else g_agi.addLog(g_agi.gameplay.interact(g_agi.navPos,g_agi.navHeading,g_agi.navWorld));
            } else if (wp == '1' || wp == '2' || wp == '3') {
                g_agi.addLog(g_agi.gameplay.choose((int)(wp - '0'), g_agi.navWorld));
            } else if (wp == 'J') {
                g_agi.gameplay.quest.journalOpen = !g_agi.gameplay.quest.journalOpen;
                g_agi.addLog(g_agi.gameplay.quest.journalOpen ? "Quest journal opened" : "Quest journal closed");
            } else if (wp == 'R') {
                g_agi.addLog(g_agi.gameplay.recover(g_agi.navWorld));
            } else if (wp == VK_F5) {
                g_agi.addLog(v20save::write(v20save::SavePath,g_agi.gameplay,g_agi.cns)
                    ? "Gameplay v20 saved atomically" : "Gameplay v20 save failed");
            } else if (wp == VK_F9) {
                const bool loaded=v20save::load(v20save::SavePath,v19game::GameplayLoop::SavePath,
                        g_agi.gameplay,g_agi.navWorld,g_agi.cns,
                        [&](const avatar3d::GameWorld& restored) {
                            v15render::HighResRenderer probe;
                            avatar3d::AvatarSkeleton pose;
                            pose.pose(restored.goal,0,0,0);
                            probe.render(restored,pose,
                                {restored.goal.x,2.2f,restored.goal.z-4.5f},0,-0.22f,false);
                            int verified=0;
                            for (int i=0;i<v15render::HighResFramebuffer::PixelCount;++i)
                                if (probe.framebuffer.objectId[i]==v15render::ObjectGoal &&
                                    std::isfinite(probe.framebuffer.depth[i])) ++verified;
                            return verified>100;
                        });
                if (loaded) {
                    g_agi.invalidateEcho();
                    g_agi.neuralPolicy.reset();
                    g_agi.previousControllerTarget=v16motor::ControllerTarget{};
                    g_agi.syncBodyFromCns();
                    g_agi.avatar.pose(g_agi.navPos,g_agi.navHeading,0.0f,0.0f);
                    g_agi.highResRenderer.render(g_agi.navWorld,g_agi.avatar,
                        g_agi.navPos+avatar3d::Vec3{0,2.5f,-4.0f},g_agi.navHeading,-0.4f,true);
                    g_agi.perceptionPyramid.derive(g_agi.highResRenderer.framebuffer);
                }
                g_agi.addLog(loaded ? "Gameplay loaded / v1 safely migrated if needed"
                                    : "Gameplay load rejected; state unchanged");
            } else if (wp == VK_ESCAPE) PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN: {
            if (g_agi.highResViewportVisible) {
                int mx = LOWORD(lp), my = HIWORD(lp);
                int cx = 6, cy = 324, cs = 19, gox = cx + 18, goy = cy + 24;
                for (int q = 0; q < dte::TRIZArena::SIZE; q++)
                    for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                        int px = gox + q * (cs + 2) + (r % 2) * (cs / 2 + 1);
                        int py = goy + r * (cs + 2);
                        if (mx >= px && mx < px + cs && my >= py && my < py + cs) {
                            g_agi.selectedCellQ = q;
                            g_agi.selectedCellR = r;
                            char buf[64]; snprintf(buf, 64, "Selected TRIZ cell (%d,%d)", q, r);
                            g_agi.addLog(buf);
                            break;
                        }
                    }
                return 0;
            }
            int mx = LOWORD(lp), my = HIWORD(lp);
            int cx = 370, cy = 100, cs = 26, gox = cx + 40, goy = cy + 36;
            for (int q = 0; q < dte::TRIZArena::SIZE; q++)
                for (int r = 0; r < dte::TRIZArena::SIZE; r++) {
                    int px = gox + q * (cs + 3) + (r % 2) * (cs/2 + 1);
                    int py = goy + r * (cs + 3);
                    if (mx >= px && mx < px + cs && my >= py && my < py + cs) {
                        g_agi.selectedCellQ = q;
                        g_agi.selectedCellR = r;
                        char buf[64]; snprintf(buf, 64, "Selected cell (%d,%d)", q, r);
                        g_agi.addLog(buf);
                        break;
                    }
                }
            return 0;
        }
        case WM_DESTROY:
            if (g_displayWindow && IsWindow(g_displayWindow)) DestroyWindow(g_displayWindow);
            g_agi.saveLearningState(); // persist learning progress across sessions
            KillTimer(hwnd, 1);
            DeleteObject(g_hFont); DeleteObject(g_hFontLarge); DeleteObject(g_hFontMono); DeleteObject(g_hFontSmall);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    if (strstr(GetCommandLineA(),"--evaluate-skills")) return g_agi.evaluateSkills();
    if (strstr(GetCommandLineA(),"--navdiag")) {
        // Headless navigation diagnostic: no window; 300 ticks; write
        // position, breaker state, and per-step failures to NAVDIAG.txt.
        FILE* nd = fopen("NAVDIAG.txt","w");
        if (!nd) nd = stderr;
        g_agi.highResRenderer.loadDteAsset("");
        g_agi.navWorld.generate(1337,48);
        g_agi.gameplay.reset(g_agi.navWorld);
        g_agi.cns.reset();
        g_agi.syncBodyFromCns();
        g_agi.initializeLearning();
        const auto start=g_agi.navPos;
        fprintf(nd,"spawn(%.2f,%.2f) cart(%.2f,%.2f)\n",start.x,start.z,
            g_agi.navWorld.vehiclePos.x,g_agi.navWorld.vehiclePos.z);
        for (int i = 1; i <= 300; ++i) {
            g_agi.tick();
            if (i % 50 == 0) {
                fprintf(nd,"tick %3d pos(%.2f,%.2f) hd(%.2f) cmp(%.2f) breaker=%s lastErr=%s\n",
                    i, g_agi.navPos.x, g_agi.navPos.z, g_agi.navHeading, g_agi.navGoalCompass,
                    g_agi.agentLoop.breaker().stateName(),
                    g_agi.agentLoop.stats().lastError.c_str());
                fprintf(nd,"  motor=%d contact=%d nn_frames=%llu gnn_samples=%d train=%llu loss=%.5f shield_accepted=%llu\n",
                    (int)g_agi.lastMotorVerdict,g_agi.cns.body().contact,
                    (unsigned long long)g_agi.neuralFrames,g_agi.gnnMotor.totalSamples(),
                    (unsigned long long)g_agi.neuralTrainingUpdates,g_agi.lastNeuralTrainLoss,
                    (unsigned long long)g_agi.neuralAccepted);
                fflush(nd);
            }
        }
        fprintf(nd,"--- per-step stats ---\n");
        for (const auto& s : g_agi.agentLoop.steps())
            fprintf(nd,"%-14s exec=%5llu fail=%5llu %.2fms\n",
                s.name.c_str(), (unsigned long long)s.executions,
                (unsigned long long)s.failures, s.lastDurationMs);
        const float displacement=avatar3d::len(g_agi.navPos-start);
        const bool navigated=displacement>.25f && g_agi.neuralFrames>0 &&
            g_agi.neuralTrainingUpdates>0 && std::isfinite(g_agi.lastNeuralTrainLoss);
        fprintf(nd,"displacement=%.3f navigation_and_learning=%s\n",
            displacement,navigated?"PASS":"FAIL");
        fflush(nd);
        if (nd != stderr) fclose(nd);
        return navigated?0:1;
    }
    if (strstr(GetCommandLineA(),"--selftest")) {
        // In-process self-test: no window, no injected input, no capture APIs.
        // Drives the same control path as the player and reports via addLog()
        // plus JSON/txt files next to the executable.
        g_agi.highResRenderer.loadDteAsset("");
        g_agi.navWorld.generate(1337,48);
        g_agi.gameplay.reset(g_agi.navWorld);
        g_agi.cns.reset();
        g_agi.syncBodyFromCns();
        g_agi.avatar.pose(g_agi.navPos,g_agi.navHeading,0.0f,0.0f);
        g_agi.highResRenderer.render(g_agi.navWorld,g_agi.avatar,
            g_agi.navPos+avatar3d::Vec3{0,2.5f,-4.0f},0,-0.4f,true);
        g_agi.perceptionPyramid.derive(g_agi.highResRenderer.framebuffer);
        g_agi.initializeLearning();
        return g_agi.runSelfTest();
    }
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "GTAngelAGIv20";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);
    WNDCLASSEXA displayClass=wc;
    displayClass.lpfnWndProc=DisplayWndProc;
    displayClass.lpszClassName="GTAngelVisualDisplayV24";
    if(!RegisterClassExA(&displayClass)) return 2;
    RECT windowRect = {0, 0, 1440, 996};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExA(0, "GTAngelAGIv20",
        "GTAngel v56 - Unified Embodied AGI Cockpit (F6 + 768x768 VRD + RHS) / local 768 VRD",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        16, 8, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top,
        NULL, NULL, hInst, NULL);
    if (!hwnd) return 1;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}

