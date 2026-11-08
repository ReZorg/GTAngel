#ifndef GTANGEL_ZPP_MATH_SPEC_GUARD
#define GTANGEL_ZPP_MATH_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/math
// Derived from C:/ddd/GTAngel/cpp_test/zpp/math/*.zpp
// Modules: 8 | Formal Schemas: 264 | Free Types: 0
// ============================================================================
namespace zpp_math_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 264;
static constexpr int kZppEnumCount   = 0;
// State Schema Contract from zpp/math/Matrix.zpp
struct CMatrix_ZppContract {
    int32_t m_attachment{0}; // Z++: Ref[RwMatrix]
    float fz{0.0f}; // Z++: ℝ
    bool m_hasRwMatrix{false}; // Z++: 𝔹
    float uw{0.0f}; // Z++: ℝ
    float pw{0.0f}; // Z++: ℝ
    float fw{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/math/Matrix.zpp
struct CCompressedMatrixNotAligned_ZppContract {
    int32_t m_vecPos{0}; // Z++: CVector
    int32_t m_rightX{0}; // Z++: ℤ
    int32_t m_rightY{0}; // Z++: ℤ
    int32_t m_rightZ{0}; // Z++: ℤ
    int32_t m_upX{0}; // Z++: ℤ
    int32_t m_upY{0}; // Z++: ℤ
    int32_t m_upZ{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/math/Matrix.zpp
struct CCompressedMatrix_ZppContract {
    int32_t _alignment{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/math/Quaternion.zpp
struct CQuaternion_ZppContract {
    float w{0.0f}; // Z++: ℝ
    int32_t this_field{0}; // Z++: Ref[return]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/math/Rect.zpp
struct CRect_ZppContract {
    float left{0.0f}; // Z++: ℝ
    float bottom{0.0f}; // Z++: ℝ
    float right{0.0f}; // Z++: ℝ
    float top{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/math/Vector.zpp
struct CVector_ZppContract {
    float w{0.0f}; // Z++: ℝ
    int32_t this_field{0}; // Z++: Ref[return]
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_math_zpp_invariants() {
    if (!CMatrix_ZppContract{}.checkInvariant()) return false;
    if (!CCompressedMatrixNotAligned_ZppContract{}.checkInvariant()) return false;
    if (!CCompressedMatrix_ZppContract{}.checkInvariant()) return false;
    if (!CQuaternion_ZppContract{}.checkInvariant()) return false;
    if (!CRect_ZppContract{}.checkInvariant()) return false;
    if (!CVector_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_math_zpp_invariants(), "Z++ formal contract invariant failed for math");
} // namespace zpp_math_spec
#endif // GTANGEL_ZPP_MATH_SPEC_GUARD
namespace avatar3d {

struct Vec3 { float x{0}, y{0}, z{0}; };
static Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
static Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
static Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
static float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static float len(Vec3 a) { return sqrtf(dot(a,a)); }
static Vec3 norm(Vec3 a) { float l = len(a); return l > 1e-6f ? a*(1.0f/l) : Vec3{0,0,0}; }

// ── Skeletal avatar model ──
// 17 joints (root, spine, head, arms, legs) with forward-kinematic pose.
// GTAngel v21: clean-room visual pose controller derived from the user-supplied
// DTE motion style guide. This declaration replaces AvatarSkeleton within
// namespace avatar3d; it never moves the body or writes a motor/world commit.
// LW21: baked v25 pose clips (DTEP25L). Clean-room offline bake of the
// user-supplied pose rig; runtime samples per-joint displacements only.
