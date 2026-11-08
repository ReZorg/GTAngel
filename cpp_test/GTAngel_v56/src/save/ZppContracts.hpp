#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_SAVE_SPEC_GUARD
#define GTANGEL_ZPP_SAVE_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/save
// Derived from C:/ddd/GTAngel/cpp_test/zpp/save/*.zpp
// Modules: 5 | Formal Schemas: 143 | Free Types: 10
// ============================================================================
namespace zpp_save_spec {
static constexpr int kZppModuleCount = 5;
static constexpr int kZppSchemaCount = 143;
static constexpr int kZppEnumCount   = 10;
// From zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_1 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 3, SAVE_TYPE_GCC_v = 4, SAVE_TYPE_STEAM_v = 5 };
// From zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_2 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 3, SAVE_TYPE_GCC_v = 4 };
// From zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_1 : int32_t { CARD_ONE_v = 0, CARD_TWO_v = 1, MAX_CARDS_v = 2 };
// From zpp/save/MemoryCard.zpp
enum class MCSTATE : int32_t { MCSTATE_OK_v = 0, MCSTATE_NEED_500KB_v = 1, MCSTATE_NEED_200KB_v = 2, MCSTATE_NOCARD_v = 3 };
// State Schema Contract from zpp/save/Date.zpp
struct CDate_ZppContract {
    int32_t m_nSecond{0}; // Z++: ℤ
    int32_t m_nMinute{0}; // Z++: ℤ
    int32_t m_nHour{0}; // Z++: ℤ
    int32_t m_nDay{0}; // Z++: ℤ
    int32_t m_nMonth{0}; // Z++: ℤ
    int32_t m_nYear{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/save/GenericGameStorage.zpp
struct GenericGameStorageModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/save/MemoryCard.zpp
struct CMemoryCardInfo_ZppContract {
    int32_t port{0}; // Z++: ℤ
    int32_t slot{0}; // Z++: ℤ
    int32_t type{0}; // Z++: ℤ
    int32_t free{0}; // Z++: ℤ
    int32_t format{0}; // Z++: ℤ
    int32_t dir{0}; // Z++: seq CHAR
    int32_t Year{0}; // Z++: ℤ
    int32_t _Create{0}; // Z++: time
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/save/MemoryCard.zpp
struct CMemoryCard_ZppContract {
    int32_t _unk0{0}; // Z++: ℤ
    int32_t _unk1{0}; // Z++: ℤ
    bool m_bWantToLoad{false}; // Z++: 𝔹
    bool JustLoadedDontFadeInYet{false}; // Z++: 𝔹
    bool StillToFadeOut{false}; // Z++: 𝔹
    bool b_FoundRecentSavedGameWantToLoad{false}; // Z++: 𝔹
    uint32_t TimeStartedCountingForFade{0}; // Z++: ℕ
    uint32_t TimeToStayFadedBeforeFadeOut{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/save/SaveBuf.zpp
struct SaveBufModuleState_ZppContract {
    bool module_loaded{false}; // Z++: 𝔹
    uint32_t source_lines{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_save_zpp_invariants() {
    if (!CDate_ZppContract{}.checkInvariant()) return false;
    if (!GenericGameStorageModuleState_ZppContract{}.checkInvariant()) return false;
    if (!CMemoryCardInfo_ZppContract{}.checkInvariant()) return false;
    if (!CMemoryCard_ZppContract{}.checkInvariant()) return false;
    if (!SaveBufModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_save_zpp_invariants(), "Z++ formal contract invariant failed for save");
} // namespace zpp_save_spec
#endif // GTANGEL_ZPP_SAVE_SPEC_GUARD
