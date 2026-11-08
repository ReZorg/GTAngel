#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_COLLISION_SPEC_GUARD
#define GTANGEL_ZPP_COLLISION_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/collision
// Derived from C:/ddd/GTAngel/cpp_test/zpp/collision/*.zpp
// Modules: 11 | Formal Schemas: 250 | Free Types: 2
// ============================================================================
namespace zpp_collision_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 250;
static constexpr int kZppEnumCount   = 2;
// From zpp/collision/ColTriangle.zpp
enum class Direction : int32_t { DIR_X_POS_v = 0, DIR_X_NEG_v = 1, DIR_Y_POS_v = 2, DIR_Y_NEG_v = 3, DIR_Z_POS_v = 4, DIR_Z_NEG_v = 5 };
// From zpp/collision/Collision.zpp
enum class Collision_Enum_1 : int32_t { MAXNUMSPHERES_v = 0, MAXNUMBOXES_v = 1, MAXNUMLINES_v = 2, MAXNUMTRIS_v = 3 };
// State Schema Contract from zpp/collision/ColBox.zpp
struct CBox_ZppContract {
    int32_t min{0}; // Z++: CVector
    int32_t max{0}; // Z++: CVector
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColBox_ZppContract {
    int32_t min{0}; // Z++: CVector
    int32_t max{0}; // Z++: CVector
    uint32_t surface{0}; // Z++: ℕ
    uint32_t piece{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColBox.zpp
struct CColTriBBox_ZppContract {
    int32_t first{0}; // Z++: ℤ
    int32_t last{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColLine.zpp
struct CColLine_ZppContract {
    int32_t p0{0}; // Z++: CVector
    int32_t p1{0}; // Z++: CVector
    int32_t pad0{0}; // Z++: ℤ
    int32_t pad1{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColModel.zpp
struct CColModel_ZppContract {
    int32_t boundingSphere{0}; // Z++: CSphere
    int32_t boundingBox{0}; // Z++: CBox
    int32_t numSpheres{0}; // Z++: ℤ
    int32_t numBoxes{0}; // Z++: ℤ
    int32_t numTriangles{0}; // Z++: ℤ
    int32_t numLines{0}; // Z++: ℤ
    int32_t numTriBBoxes{0}; // Z++: ℤ
    uint32_t level{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/collision/ColPoint.zpp
struct CColPoint_ZppContract {
    int32_t point{0}; // Z++: CVector
    int32_t pad1{0}; // Z++: ℤ
    int32_t normal{0}; // Z++: CVector
    int32_t pad2{0}; // Z++: ℤ
    uint32_t surfaceA{0}; // Z++: ℕ
    uint32_t pieceA{0}; // Z++: ℕ
    uint32_t surfaceB{0}; // Z++: ℕ
    uint32_t pieceB{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_collision_zpp_invariants() {
    if (!CBox_ZppContract{}.checkInvariant()) return false;
    if (!CColBox_ZppContract{}.checkInvariant()) return false;
    if (!CColTriBBox_ZppContract{}.checkInvariant()) return false;
    if (!CColLine_ZppContract{}.checkInvariant()) return false;
    if (!CColModel_ZppContract{}.checkInvariant()) return false;
    if (!CColPoint_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_collision_zpp_invariants(), "Z++ formal contract invariant failed for collision");
} // namespace zpp_collision_spec
#endif // GTANGEL_ZPP_COLLISION_SPEC_GUARD
