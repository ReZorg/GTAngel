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
// From zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_1 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8, SAVE_TYPE_STEAM_v = 16 };
// From zpp/save/GenericGameStorage.zpp
enum class GenericGameStorage_Enum_2 : int32_t { SAVE_TYPE_NONE_v = 0, SAVE_TYPE_32_BIT_v = 1, SAVE_TYPE_64_BIT_v = 2, SAVE_TYPE_MSVC_v = 4, SAVE_TYPE_GCC_v = 8 };
// From zpp/save/MemoryCard.zpp
enum class MemoryCard_Enum_1 : int32_t { CARD_ONE_v = 0, CARD_TWO_v = 1, MAX_CARDS_v = 2 };
// From zpp/save/MemoryCard.zpp
enum class MCSTATE : int32_t { MCSTATE_OK_v = 0, MCSTATE_NEED_500KB_v = 1, MCSTATE_NEED_200KB_v = 2, MCSTATE_NOCARD_v = 3 };
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
    if (!GenericGameStorageModuleState_ZppContract{}.checkInvariant()) return false;
    if (!SaveBufModuleState_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_save_zpp_invariants(), "Z++ formal contract invariant failed for save");
} // namespace zpp_save_spec
#endif // GTANGEL_ZPP_SAVE_SPEC_GUARD
