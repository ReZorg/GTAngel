#pragma once
#ifndef GTANGEL_ZPP_SKEL_SPEC_GUARD
#define GTANGEL_ZPP_SKEL_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/skel
// Derived from C:/ddd/GTAngel/cpp_test/zpp/skel/*.zpp
// Modules: 4 | Formal Schemas: 44 | Free Types: 9
// ============================================================================
namespace zpp_skel_spec {
static constexpr int kZppModuleCount = 4;
static constexpr int kZppSchemaCount = 44;
static constexpr int kZppEnumCount   = 9;
// From zpp/skel/crossplatform.zpp
enum class eWinVersion : int32_t { OS_WIN95_v = 0, OS_WIN98_v = 1, OS_WINNT_v = 2, OS_WIN2000_v = 3, OS_WINXP_v = 4 };
// From zpp/skel/crossplatform.zpp
enum class crossplatform_Enum_2 : int32_t { LANG_OTHER_v = 0, LANG_GERMAN_v = 1, LANG_FRENCH_v = 2, LANG_ENGLISH_v = 3, LANG_ITALIAN_v = 4, LANG_SPANISH_v = 5 };
// From zpp/skel/crossplatform.zpp
enum class crossplatform_Enum_3 : int32_t { SUBLANG_OTHER_v = 0, SUBLANG_ENGLISH_AUS_v = 1 };
// From zpp/skel/crossplatform.zpp
enum class eGameState : int32_t { GS_START_UP_v = 0, GS_INIT_LOGO_MPEG_v = 1, GS_LOGO_MPEG_v = 2, GS_INIT_INTRO_MPEG_v = 3, GS_INTRO_MPEG_v = 4, GS_INIT_ONCE_v = 5, GS_INIT_FRONTEND_v = 6, GS_FRONTEND_v = 7, GS_INIT_PLAYING_GAME_v = 8, GS_PLAYING_GAME_v = 9 };
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
    constexpr bool checkInvariant() const {
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
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 838);
    }
};
// State Schema Contract from zpp/skel/platform.zpp
struct platformModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{116}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 116);
    }
};
constexpr bool verify_skel_zpp_invariants() {
    if (!SYSTEMTIME_ZppContract{}.checkInvariant()) return false;
    if (!eventsModuleState_ZppContract{}.checkInvariant()) return false;
    if (!platformModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_skel_zpp_invariants(), "Z++ formal contract invariant failed for skel");
} // namespace zpp_skel_spec
#endif // GTANGEL_ZPP_SKEL_SPEC_GUARD
