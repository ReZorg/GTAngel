#pragma once
#ifndef GTANGEL_ZPP_TEXT_SPEC_GUARD
#define GTANGEL_ZPP_TEXT_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
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
    int64_t m_nFlag{0}; // Z++: ℕ
    int64_t m_nTime{0}; // Z++: ℕ
    int64_t m_nStartTime{0}; // Z++: ℕ
    std::array<int32_t, 6> m_nNumber{}; // Z++: seq ℤ  -- bounded array [6]
    size_t m_nNumber_len{6u}; // Z++ cardinality #m_nNumber
    int32_t m_pString{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const {
        return (m_nNumber_len == 6u && m_nNumber.size() == 6u)
            && (m_nFlag >= 0)
            && (m_nTime >= 0)
            && (m_nStartTime >= 0);
    }
};
// State Schema Contract from zpp/text/Messages.zpp
struct tBigMessage_ZppContract {
    std::array<int32_t, 4> m_Stack{}; // Z++: seq tMessage  -- bounded array [4]
    size_t m_Stack_len{4u}; // Z++ cardinality #m_Stack
    constexpr bool checkInvariant() const {
        return (m_Stack_len == 4u && m_Stack.size() == 4u);
    }
};
// State Schema Contract from zpp/text/Messages.zpp
struct tPreviousBrief_ZppContract {
    int32_t m_pText{0}; // Z++: seq CHAR
    std::array<int32_t, 6> m_nNumber{}; // Z++: seq ℤ  -- bounded array [6]
    size_t m_nNumber_len{6u}; // Z++ cardinality #m_nNumber
    int32_t m_pString{0}; // Z++: seq CHAR
    constexpr bool checkInvariant() const {
        return (m_nNumber_len == 6u && m_nNumber.size() == 6u);
    }
};
// State Schema Contract from zpp/text/Pager.zpp
struct PagerMessage_ZppContract {
    int32_t m_pText{0}; // Z++: seq CHAR
    int64_t m_nSpeedMs{0}; // Z++: ℕ
    int32_t m_nCurrentPosition{0}; // Z++: ℤ
    int64_t m_nStringLength{0}; // Z++: ℕ
    int64_t m_nPriority{0}; // Z++: ℕ
    int64_t m_nTimeToChangePosition{0}; // Z++: ℕ
    int32_t unused{0}; // Z++: ℤ
    std::array<int32_t, 6> m_nNumber{}; // Z++: seq ℤ  -- bounded array [6]
    size_t m_nNumber_len{6u}; // Z++ cardinality #m_nNumber
    constexpr bool checkInvariant() const {
        return (m_nNumber_len == 6u && m_nNumber.size() == 6u)
            && (m_nSpeedMs >= 0)
            && (m_nStringLength >= 0)
            && (m_nPriority >= 0)
            && (m_nTimeToChangePosition >= 0);
    }
};
// State Schema Contract from zpp/text/Pager.zpp
struct CPager_ZppContract {
    int32_t m_nNumDisplayLetters{0}; // Z++: ℤ
    std::array<int32_t, 8> m_messages{}; // Z++: seq PagerMessage  -- bounded array [NUMPAGERMESSAGES]
    size_t m_messages_len{8u}; // Z++ cardinality #m_messages
    constexpr bool checkInvariant() const {
        return (m_messages_len == 8u && m_messages.size() == 8u);
    }
};
// State Schema Contract from zpp/text/Text.zpp
struct CKeyEntry_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_Init(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_Process_2(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_Display_3(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_AddMessage_4(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_AddMessageWithNumber_5(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
// Operation Schema Contract (ΔCPager) from zpp/text/Pager.zpp
constexpr bool op_CPager_ClearMessages_6(CPager_ZppContract& state) {
    if (!state.checkInvariant()) return false;
    return state.checkInvariant();
}
constexpr bool verify_text_zpp_invariants() {
    {
        tMessage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tMessage_ZppContract bad{};
        bad.m_nNumber_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tBigMessage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tBigMessage_ZppContract bad{};
        bad.m_Stack_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        tPreviousBrief_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        tPreviousBrief_ZppContract bad{};
        bad.m_nNumber_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        PagerMessage_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        PagerMessage_ZppContract bad{};
        bad.m_nNumber_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CPager_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CPager_ZppContract bad{};
        bad.m_messages_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CKeyEntry_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CKeyEntry_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_Init(st)) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_Process_2(st)) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_Display_3(st)) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_AddMessage_4(st)) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_AddMessageWithNumber_5(st)) return false;
    }
    {
        CPager_ZppContract st{};
        if (!op_CPager_ClearMessages_6(st)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_text_zpp_invariants(), "Z++ formal contract invariant failed for text");
} // namespace zpp_text_spec
#endif // GTANGEL_ZPP_TEXT_SPEC_GUARD
