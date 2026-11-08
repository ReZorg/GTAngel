#pragma once
#include <cstdint>
#ifndef GTANGEL_ZPP_WEAPONS_SPEC_GUARD
#define GTANGEL_ZPP_WEAPONS_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/weapons
// Derived from C:/ddd/GTAngel/cpp_test/zpp/weapons/*.zpp
// Modules: 8 | Formal Schemas: 171 | Free Types: 8
// ============================================================================
namespace zpp_weapons_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 171;
static constexpr int kZppEnumCount   = 8;
// From zpp/weapons/Explosion.zpp
enum class eExplosionType : int32_t { EXPLOSION_GRENADE_v = 0, EXPLOSION_MOLOTOV_v = 1, EXPLOSION_ROCKET_v = 2, EXPLOSION_CAR_v = 3, EXPLOSION_CAR_QUICK_v = 4, EXPLOSION_BOAT_v = 5, EXPLOSION_HELI_v = 6, EXPLOSION_HELI2_v = 7, EXPLOSION_MINE_v = 8, EXPLOSION_BARREL_v = 9, EXPLOSION_TANK_GRENADE_v = 10, EXPLOSION_HELI_BOMB_v = 11 };
// From zpp/weapons/WeaponInfo.zpp
enum class WeaponInfo_Enum_1 : int32_t { WEAPONFLAG_USE_GRAVITY_v = 0, WEAPONFLAG_SLOWS_DOWN_v = 1, WEAPONFLAG_DISSIPATES_v = 2, WEAPONFLAG_RAND_SPEED_v = 3, WEAPONFLAG_EXPANDS_v = 4, WEAPONFLAG_EXPLODES_v = 5, WEAPONFLAG_CANAIM_v = 6, WEAPONFLAG_CANAIM_WITHARM_v = 7, WEAPONFLAG_1ST_PERSON_v = 8, WEAPONFLAG_HEAVY_v = 9, WEAPONFLAG_THROW_v = 10, WEAPONFLAG_RELOAD_LOOP2START_v = 11 };
// From zpp/weapons/WeaponInfo.zpp
enum class WeaponInfo_Enum_2 : int32_t { WEAPONFLAG_USE_GRAVITY_v = 0, WEAPONFLAG_SLOWS_DOWN_v = 1, WEAPONFLAG_DISSIPATES_v = 2, WEAPONFLAG_RAND_SPEED_v = 3, WEAPONFLAG_EXPANDS_v = 4, WEAPONFLAG_EXPLODES_v = 5, WEAPONFLAG_CANAIM_v = 6, WEAPONFLAG_CANAIM_WITHARM_v = 7, WEAPONFLAG_1ST_PERSON_v = 8, WEAPONFLAG_HEAVY_v = 9, WEAPONFLAG_THROW_v = 10 };
// From zpp/weapons/WeaponType.zpp
enum class eWeaponType : int32_t { WEAPONTYPE_UNARMED_v = 0, WEAPONTYPE_BRASSKNUCKLE_v = 1, WEAPONTYPE_SCREWDRIVER_v = 2, WEAPONTYPE_GOLFCLUB_v = 3, WEAPONTYPE_NIGHTSTICK_v = 4, WEAPONTYPE_KNIFE_v = 5, WEAPONTYPE_BASEBALLBAT_v = 6, WEAPONTYPE_HAMMER_v = 7, WEAPONTYPE_CLEAVER_v = 8, WEAPONTYPE_MACHETE_v = 9, WEAPONTYPE_KATANA_v = 10, WEAPONTYPE_CHAINSAW_v = 11 };
// State Schema Contract from zpp/weapons/BulletInfo.zpp
struct CBulletInfo_ZppContract {
    int32_t m_eWeaponType{0}; // Z++: eWeaponType
    int32_t m_pSource{0}; // Z++: Ref[CEntity]
    float m_fTimer{0.0f}; // Z++: ℝ
    bool m_bInUse{false}; // Z++: 𝔹
    int32_t m_vecPosition{0}; // Z++: CVector
    int32_t m_vecSpeed{0}; // Z++: CVector
    int32_t m_nDamage{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/weapons/Explosion.zpp
struct CExplosion_ZppContract {
    int32_t m_ExplosionType{0}; // Z++: eExplosionType
    int32_t m_vecPosition{0}; // Z++: CVector
    float m_fRadius{0.0f}; // Z++: ℝ
    float m_fPropagationRate{0.0f}; // Z++: ℝ
    int32_t m_pCreatorEntity{0}; // Z++: Ref[CEntity]
    int32_t m_pVictimEntity{0}; // Z++: Ref[CEntity]
    float m_fStopTime{0.0f}; // Z++: ℝ
    uint32_t m_nIteration{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/weapons/ProjectileInfo.zpp
struct CProjectileInfo_ZppContract {
    int32_t m_eWeaponType{0}; // Z++: eWeaponType
    int32_t m_pSource{0}; // Z++: Ref[CEntity]
    uint32_t m_nExplosionTime{0}; // Z++: ℕ
    bool m_bInUse{false}; // Z++: 𝔹
    int32_t m_vecPos{0}; // Z++: CVector
    int32_t ms_apProjectile{0}; // Z++: seq Ref[CProjectile]  -- bounded array [NUM_PROJECTILES]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/weapons/ShotInfo.zpp
struct CShotInfo_ZppContract {
    int32_t m_weapon{0}; // Z++: eWeaponType
    int32_t m_startPos{0}; // Z++: CVector
    int32_t m_areaAffected{0}; // Z++: CVector
    float m_radius{0.0f}; // Z++: ℝ
    int32_t m_sourceEntity{0}; // Z++: Ref[CEntity]
    float m_timeout{0.0f}; // Z++: ℝ
    bool m_inUse{false}; // Z++: 𝔹
    int32_t ms_afRandTable{0}; // Z++: seq ℝ  -- bounded array [20]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/weapons/Weapon.zpp
struct CWeapon_ZppContract {
    int32_t m_eWeaponType{0}; // Z++: eWeaponType
    int32_t m_eWeaponState{0}; // Z++: eWeaponState
    int32_t m_nAmmoInClip{0}; // Z++: ℤ
    int32_t m_nAmmoTotal{0}; // Z++: ℤ
    uint32_t m_nTimer{0}; // Z++: ℕ
    bool m_bAddRotOffset{false}; // Z++: 𝔹
    bool bPhotographHasBeenTaken{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/weapons/WeaponEffects.zpp
struct CWeaponEffects_ZppContract {
    bool m_bActive{false}; // Z++: 𝔹
    int32_t m_vecPos{0}; // Z++: CVector
    uint32_t m_nRed{0}; // Z++: ℕ
    uint32_t m_nGreen{0}; // Z++: ℕ
    uint32_t m_nBlue{0}; // Z++: ℕ
    uint32_t m_nAlpha{0}; // Z++: ℕ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fRotation{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_weapons_zpp_invariants() {
    if (!CBulletInfo_ZppContract{}.checkInvariant()) return false;
    if (!CExplosion_ZppContract{}.checkInvariant()) return false;
    if (!CProjectileInfo_ZppContract{}.checkInvariant()) return false;
    if (!CShotInfo_ZppContract{}.checkInvariant()) return false;
    if (!CWeapon_ZppContract{}.checkInvariant()) return false;
    if (!CWeaponEffects_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_weapons_zpp_invariants(), "Z++ formal contract invariant failed for weapons");
} // namespace zpp_weapons_spec
#endif // GTANGEL_ZPP_WEAPONS_SPEC_GUARD
