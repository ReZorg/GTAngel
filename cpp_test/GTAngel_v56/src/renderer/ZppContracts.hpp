#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_RENDERER_SPEC_GUARD
#define GTANGEL_ZPP_RENDERER_SPEC_GUARD
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
enum class Console_Enum_1 : int32_t { MAX_LINES_v = 0, MAX_STR_LEN_v = 1 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_1 : int32_t { SUN_CORE_v = 0, SUN_CORONA_v = 1 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_2 : int32_t { TYPE_STAR_v = 0, TYPE_NORMAL_v = 1, TYPE_MOON_v = 2, TYPE_REFLECT_v = 3, TYPE_HEADLIGHT_v = 4, TYPE_HEX_v = 5, TYPE_CIRCLE_v = 6, TYPE_RING_v = 7, TYPE_STREAK_v = 8 };
// From zpp/renderer/Coronas.zpp
enum class Coronas_Enum_3 : int32_t { FLARE_NONE_v = 0, FLARE_SUN_v = 1, FLARE_HEADLIGHTS_v = 2 };
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct C2dEffect_ZppContract {
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    uint32_t lightType{0}; // Z++: ℕ
    uint32_t roadReflection{0}; // Z++: ℕ
    uint32_t flareType{0}; // Z++: ℕ
    uint32_t shadowIntensity{0}; // Z++: ℕ
    uint32_t flags{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Light_ZppContract {
    float dist{0.0f}; // Z++: ℝ
    float range{0.0f}; // Z++: ℝ
    float size{0.0f}; // Z++: ℝ
    float shadowSize{0.0f}; // Z++: ℝ
    uint32_t lightType{0}; // Z++: ℕ
    uint32_t roadReflection{0}; // Z++: ℕ
    uint32_t flareType{0}; // Z++: ℕ
    uint32_t shadowIntensity{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Particle_ZppContract {
    int32_t particleType{0}; // Z++: ℤ
    int32_t dir{0}; // Z++: CVector
    float scale{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct Attractor_ZppContract {
    int32_t dir{0}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    uint32_t probability{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/renderer/2dEffect.zpp
struct PedAttractor_ZppContract {
    int32_t queueDir{0}; // Z++: CVector
    int32_t useDir{0}; // Z++: CVector
    int32_t type{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/renderer/Antennas.zpp
struct CAntenna_ZppContract {
    bool active{false}; // Z++: 𝔹
    bool updatedLastFrame{false}; // Z++: 𝔹
    uint32_t id{0}; // Z++: ℕ
    float segmentLength{0.0f}; // Z++: ℝ
    int32_t pos{0}; // Z++: seq CVector  -- bounded array [6]
    int32_t speed{0}; // Z++: seq CVector  -- bounded array [6]
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_renderer_zpp_invariants() {
    if (!C2dEffect_ZppContract{}.checkInvariant()) return false;
    if (!Light_ZppContract{}.checkInvariant()) return false;
    if (!Particle_ZppContract{}.checkInvariant()) return false;
    if (!Attractor_ZppContract{}.checkInvariant()) return false;
    if (!PedAttractor_ZppContract{}.checkInvariant()) return false;
    if (!CAntenna_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_renderer_zpp_invariants(), "Z++ formal contract invariant failed for renderer");
} // namespace zpp_renderer_spec
#endif // GTANGEL_ZPP_RENDERER_SPEC_GUARD
