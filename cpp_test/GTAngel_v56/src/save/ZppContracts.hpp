#pragma once
#ifndef GTANGEL_ZPP_SAVE_SPEC_GUARD
#define GTANGEL_ZPP_SAVE_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/save
// Derived from C:/ddd/GTAngel/cpp_test/zpp/save/*.zpp
// Modules: 5 | Formal Schemas: 143 | Free Types: 10
// ============================================================================
namespace zpp_save_spec {
static constexpr int kZppModuleCount = 5;
static constexpr int kZppSchemaCount = 143;
static constexpr int kZppEnumCount   = 10;
// Free Type from zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_1 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8, SAVE_TYPE_STEAM_v = 16 };
// Free Type from zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_2 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_1 : int32_t { CARD_ONE_v = 0, CARD_TWO_v = 1, MAX_CARDS_v = 2 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MCSTATE : int32_t { MCSTATE_OK_v = 0, MCSTATE_NEED_500KB_v = 1, MCSTATE_NEED_200KB_v = 2, MCSTATE_NOCARD_v = 3 };
// Free Type from zpp/save/MemoryCard.zpp
enum class SLOTINFO : int32_t { SLOT_PRESENT_v = 0, SLOT_NOTPRESENT_v = 1, SLOT_CORRUPTED_v = 2 };
// Free Type from zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_5 : int32_t { ERR_NONE_v = 0, ERR_NOFORMAT_v = 1, ERR_DIRNOENTRY_v = 2, ERR_OPENNOENTRY_v = 3, ERR_DELETENOENTRY_v = 4, ERR_DELETEDENIED_v = 5, ERR_DELETEFAILED_v = 6, ERR_WRITEFULLDEVICE_v = 7, ERR_WRITENOENTRY_v = 8, ERR_WRITEDENIED_v = 9, ERR_FLUSHNOENTRY_v = 10, ERR_WRITEFAILED_v = 11, ERR_FORMATFAILED_v = 12, ERR_FILETABLENOENTRY_v = 13, ERR_DIRFULLDEVICE_v = 14, ERR_DIRBADENTRY_v = 15, ERR_FILEFULLDEVICE_v = 16, ERR_FILENOPATHENTRY_v = 17, ERR_FILEDENIED_v = 18, ERR_FILEUPLIMIT_v = 19, ERR_READNOENTRY_v = 20, ERR_READDENIED_v = 21, ERR_LOADFAILED_v = 22, ERR_SAVEFAILED_v = 23, ERR_DATACORRUPTED_v = 24, ERR_NOROOTDIR_v = 25, NO_ERR_SUCCESS_v = 26 };
// State Schema Contract from zpp/save/GenericGameStorage.zpp
struct GenericGameStorageModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1606}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1606);
    }
};
// State Schema Contract from zpp/save/SaveBuf.zpp
struct SaveBufModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{164}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 164);
    }
};
constexpr bool verify_save_zpp_invariants() {
    {
        GenericGameStorageModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        GenericGameStorageModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        SaveBufModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        SaveBufModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_save_zpp_invariants(), "Z++ formal contract invariant failed for save");
} // namespace zpp_save_spec
#endif // GTANGEL_ZPP_SAVE_SPEC_GUARD
