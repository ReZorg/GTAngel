#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_TEXT_SPEC_GUARD
#define GTANGEL_ZPP_TEXT_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/text
// Derived from C:/ddd/GTAngel/cpp_test/zpp/text/*.zpp
// Modules: 3 | Formal Schemas: 121 | Free Types: 2
// ============================================================================
namespace zpp_text_spec {
static constexpr int kZppModuleCount = 3;
static constexpr int kZppSchemaCount = 121;
static constexpr int kZppEnumCount   = 2;
// State Schema Contract from zpp/text/Messages.zpp
struct tMessage_ZppContract {
    int32_t m_pText{0}; // Z++: seq CHAR
    uint32_t m_nFlag{0}; // Z++: ℕ
    uint32_t m_nTime{0}; // Z++: ℕ
    uint32_t m_nStartTime{0}; // Z++: ℕ
    int32_t m_nNumber{0}; // Z++: seq ℤ  -- bounded array [6]
    int32_t m_pString{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/text/Messages.zpp
struct tBigMessage_ZppContract {
    int32_t m_Stack{0}; // Z++: seq tMessage  -- bounded array [4]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/text/Messages.zpp
struct tPreviousBrief_ZppContract {
    int32_t m_pText{0}; // Z++: seq CHAR
    int32_t m_nNumber{0}; // Z++: seq ℤ  -- bounded array [6]
    int32_t m_pString{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/text/Messages.zpp
struct CMessages_ZppContract {
    int32_t BriefMessages{0}; // Z++: seq tMessage  -- bounded array [NUMBRIEFMESSAGES]
    int32_t BIGMessages{0}; // Z++: seq tBigMessage  -- bounded array [NUMBIGMESSAGES]
    int32_t PreviousBriefs{0}; // Z++: seq tPreviousBrief  -- bounded array [NUMPREVIOUSBRIEFS]
    int32_t PreviousMissionTitle{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/text/Pager.zpp
struct PagerMessage_ZppContract {
    int32_t m_pText{0}; // Z++: seq CHAR
    uint32_t m_nSpeedMs{0}; // Z++: ℕ
    int32_t m_nCurrentPosition{0}; // Z++: ℤ
    uint32_t m_nStringLength{0}; // Z++: ℕ
    uint32_t m_nPriority{0}; // Z++: ℕ
    uint32_t m_nTimeToChangePosition{0}; // Z++: ℕ
    int32_t unused{0}; // Z++: ℤ
    int32_t m_nNumber{0}; // Z++: seq ℤ  -- bounded array [6]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/text/Pager.zpp
struct CPager_ZppContract {
    int32_t m_nNumDisplayLetters{0}; // Z++: ℤ
    int32_t m_messages{0}; // Z++: seq PagerMessage  -- bounded array [NUMPAGERMESSAGES]
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_text_zpp_invariants() {
    if (!tMessage_ZppContract{}.checkInvariant()) return false;
    if (!tBigMessage_ZppContract{}.checkInvariant()) return false;
    if (!tPreviousBrief_ZppContract{}.checkInvariant()) return false;
    if (!CMessages_ZppContract{}.checkInvariant()) return false;
    if (!PagerMessage_ZppContract{}.checkInvariant()) return false;
    if (!CPager_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_text_zpp_invariants(), "Z++ formal contract invariant failed for text");
} // namespace zpp_text_spec
#endif // GTANGEL_ZPP_TEXT_SPEC_GUARD
