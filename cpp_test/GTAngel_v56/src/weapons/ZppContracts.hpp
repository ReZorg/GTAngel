#pragma once
#ifndef GTANGEL_ZPP_WEAPONS_SPEC_GUARD
#define GTANGEL_ZPP_WEAPONS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/weapons
// Derived from C:/ddd/GTAngel/cpp_test/zpp/weapons/*.zpp
// Modules: 8 | Formal Schemas: 171 | Free Types: 8
// ============================================================================
namespace zpp_weapons_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 171;
static constexpr int kZppEnumCount   = 8;
// Free Type from zpp/weapons/Explosion.zpp
enum class eExplosionType : int32_t { EXPLOSION_GRENADE_v = 0, EXPLOSION_MOLOTOV_v = 1, EXPLOSION_ROCKET_v = 2, EXPLOSION_CAR_v = 3, EXPLOSION_CAR_QUICK_v = 4, EXPLOSION_BOAT_v = 5, EXPLOSION_HELI_v = 6, EXPLOSION_HELI2_v = 7, EXPLOSION_MINE_v = 8, EXPLOSION_BARREL_v = 9, EXPLOSION_TANK_GRENADE_v = 10, EXPLOSION_HELI_BOMB_v = 11 };
// Free Type from zpp/weapons/WeaponInfo.zpp
enum class WeaponInfo_Enum_1 : int32_t { WEAPONFLAG_USE_GRAVITY_v = 1, WEAPONFLAG_SLOWS_DOWN_v = 1, WEAPONFLAG_DISSIPATES_v = 1, WEAPONFLAG_RAND_SPEED_v = 1, WEAPONFLAG_EXPANDS_v = 1, WEAPONFLAG_EXPLODES_v = 1, WEAPONFLAG_CANAIM_v = 1, WEAPONFLAG_CANAIM_WITHARM_v = 1, WEAPONFLAG_1ST_PERSON_v = 1, WEAPONFLAG_HEAVY_v = 1, WEAPONFLAG_THROW_v = 1, WEAPONFLAG_RELOAD_LOOP2START_v = 1, WEAPONFLAG_USE_2ND_v = 1, WEAPONFLAG_GROUND_2ND_v = 1, WEAPONFLAG_FINISH_3RD_v = 1, WEAPONFLAG_RELOAD_v = 1, WEAPONFLAG_FIGHTMODE_v = 1, WEAPONFLAG_CROUCHFIRE_v = 1, WEAPONFLAG_COP3_RD_v = 1, WEAPONFLAG_GROUND_3RD_v = 1, WEAPONFLAG_PARTIALATTACK_v = 1, WEAPONFLAG_ANIMDETONATE_v = 1 };
// Free Type from zpp/weapons/WeaponInfo.zpp
enum class WeaponInfo_Enum_2 : int32_t { WEAPONFLAG_USE_GRAVITY_v = 1, WEAPONFLAG_SLOWS_DOWN_v = 1, WEAPONFLAG_DISSIPATES_v = 1, WEAPONFLAG_RAND_SPEED_v = 1, WEAPONFLAG_EXPANDS_v = 1, WEAPONFLAG_EXPLODES_v = 1, WEAPONFLAG_CANAIM_v = 1, WEAPONFLAG_CANAIM_WITHARM_v = 1, WEAPONFLAG_1ST_PERSON_v = 1, WEAPONFLAG_HEAVY_v = 1, WEAPONFLAG_THROW_v = 1 };
// Free Type from zpp/weapons/WeaponType.zpp
enum class eWeaponType : int32_t { WEAPONTYPE_UNARMED_v = 0, WEAPONTYPE_BRASSKNUCKLE_v = 1, WEAPONTYPE_SCREWDRIVER_v = 2, WEAPONTYPE_GOLFCLUB_v = 3, WEAPONTYPE_NIGHTSTICK_v = 4, WEAPONTYPE_KNIFE_v = 5, WEAPONTYPE_BASEBALLBAT_v = 6, WEAPONTYPE_HAMMER_v = 7, WEAPONTYPE_CLEAVER_v = 8, WEAPONTYPE_MACHETE_v = 9, WEAPONTYPE_KATANA_v = 10, WEAPONTYPE_CHAINSAW_v = 11, WEAPONTYPE_GRENADE_v = 12, WEAPONTYPE_DETONATOR_GRENADE_v = 13, WEAPONTYPE_TEARGAS_v = 14, WEAPONTYPE_MOLOTOV_v = 15, WEAPONTYPE_ROCKET_v = 16, WEAPONTYPE_COLT45_v = 17, WEAPONTYPE_PYTHON_v = 18, WEAPONTYPE_SHOTGUN_v = 19, WEAPONTYPE_SPAS12_SHOTGUN_v = 20, WEAPONTYPE_STUBBY_SHOTGUN_v = 21, WEAPONTYPE_TEC9_v = 22, WEAPONTYPE_UZI_v = 23, WEAPONTYPE_SILENCED_INGRAM_v = 24, WEAPONTYPE_MP5_v = 25, WEAPONTYPE_M4_v = 26, WEAPONTYPE_RUGER_v = 27, WEAPONTYPE_SNIPERRIFLE_v = 28, WEAPONTYPE_LASERSCOPE_v = 29, WEAPONTYPE_ROCKETLAUNCHER_v = 30, WEAPONTYPE_FLAMETHROWER_v = 31, WEAPONTYPE_M60_v = 32, WEAPONTYPE_MINIGUN_v = 33, WEAPONTYPE_DETONATOR_v = 34, WEAPONTYPE_HELICANNON_v = 35, WEAPONTYPE_CAMERA_v = 36, WEAPONTYPE_TOTALWEAPONS_v = 37, WEAPONTYPE_HEALTH_v = 38, WEAPONTYPE_ARMOUR_v = 39, WEAPONTYPE_RAMMEDBYCAR_v = 40, WEAPONTYPE_RUNOVERBYCAR_v = 41, WEAPONTYPE_EXPLOSION_v = 42, WEAPONTYPE_UZI_DRIVEBY_v = 43, WEAPONTYPE_DROWNING_v = 44, WEAPONTYPE_FALL_v = 45, WEAPONTYPE_UNIDENTIFIED_v = 46, WEAPONTYPE_ANYMELEE_v = 47 };
// Free Type from zpp/weapons/WeaponType.zpp
enum class WeaponType_Enum_2 : int32_t { WEAPONSLOT_UNARMED_v = 0, WEAPONSLOT_MELEE_v = 1, WEAPONSLOT_PROJECTILE_v = 2, WEAPONSLOT_HANDGUN_v = 3, WEAPONSLOT_SHOTGUN_v = 4, WEAPONSLOT_SUBMACHINEGUN_v = 5, WEAPONSLOT_RIFLE_v = 6, WEAPONSLOT_HEAVY_v = 7, WEAPONSLOT_SNIPER_v = 8, WEAPONSLOT_OTHER_v = 9, TOTAL_WEAPON_SLOTS_v = 10 };
// Free Type from zpp/weapons/WeaponType.zpp
enum class eWeaponFire : int32_t { WEAPON_FIRE_MELEE_v = 0, WEAPON_FIRE_INSTANT_HIT_v = 1, WEAPON_FIRE_PROJECTILE_v = 2, WEAPON_FIRE_AREA_EFFECT_v = 3 };
// State Schema Contract from zpp/weapons/BulletInfo.zpp
struct CBulletInfo_ZppContract {
    int32_t m_eWeaponType{0}; // Z++: eWeaponType
    uint8_t m_pSource_card{0u}; // Z++: Ref[CEntity]
    float m_fTimer{0.0f}; // Z++: ℝ
    bool m_bInUse{false}; // Z++: 𝔹
    int32_t m_vecPosition{0}; // Z++: CVector
    int32_t m_vecSpeed{0}; // Z++: CVector
    int32_t m_nDamage{0}; // Z++: ℤ
    constexpr bool checkInvariant() const {
        return (m_pSource_card <= 1u);
    }
};
// State Schema Contract from zpp/weapons/ShotInfo.zpp
struct CShotInfo_ZppContract {
    int32_t m_weapon{0}; // Z++: eWeaponType
    int32_t m_startPos{0}; // Z++: CVector
    int32_t m_areaAffected{0}; // Z++: CVector
    float m_radius{0.0f}; // Z++: ℝ
    uint8_t m_sourceEntity_card{0u}; // Z++: Ref[CEntity]
    float m_timeout{0.0f}; // Z++: ℝ
    bool m_inUse{false}; // Z++: 𝔹
    std::array<float, 20> ms_afRandTable{}; // Z++: seq ℝ  -- bounded array [20]
    size_t ms_afRandTable_len{20u}; // Z++ cardinality #ms_afRandTable
    constexpr bool checkInvariant() const {
        return (ms_afRandTable_len == 20u && ms_afRandTable.size() == 20u)
            && (m_sourceEntity_card <= 1u);
    }
};
// State Schema Contract from zpp/weapons/Weapon.zpp
struct CWeapon_ZppContract {
    int32_t m_eWeaponType{0}; // Z++: eWeaponType
    int32_t m_eWeaponState{0}; // Z++: eWeaponState
    int32_t m_nAmmoInClip{0}; // Z++: ℤ
    int32_t m_nAmmoTotal{0}; // Z++: ℤ
    int64_t m_nTimer{0}; // Z++: ℕ
    bool m_bAddRotOffset{false}; // Z++: 𝔹
    bool bPhotographHasBeenTaken{false}; // Z++: 𝔹
    constexpr bool checkInvariant() const {
        return (m_nTimer >= 0);
    }
};
// State Schema Contract from zpp/weapons/WeaponEffects.zpp
struct CWeaponEffects_ZppContract {
    bool m_bActive{false}; // Z++: 𝔹
    int32_t m_vecPos{0}; // Z++: CVector
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    int64_t m_nAlpha{0}; // Z++: ℕ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fRotation{0.0f}; // Z++: ℝ
    constexpr bool checkInvariant() const {
        return (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0)
            && (m_nAlpha >= 0);
    }
};
// State Schema Contract from zpp/weapons/WeaponType.zpp
struct WeaponTypeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{99}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 99);
    }
};
// Delta Operation Schema Contract (ΔCBulletInfo) from zpp/weapons/BulletInfo.zpp
constexpr bool op_CBulletInfo_Initialise(const CBulletInfo_ZppContract& pre, const CBulletInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_bInUse == false);
}
constexpr CBulletInfo_ZppContract apply_CBulletInfo_Initialise(const CBulletInfo_ZppContract& pre) {
    CBulletInfo_ZppContract post = pre;
    post.m_bInUse = false;
    return post;
}
// Delta Operation Schema Contract (ΔCShotInfo) from zpp/weapons/ShotInfo.zpp
constexpr bool op_CShotInfo_Initialise(const CShotInfo_ZppContract& pre, const CShotInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_inUse == false);
}
constexpr CShotInfo_ZppContract apply_CShotInfo_Initialise(const CShotInfo_ZppContract& pre) {
    CShotInfo_ZppContract post = pre;
    post.m_inUse = false;
    return post;
}
// Delta Operation Schema Contract (ΔCShotInfo) from zpp/weapons/ShotInfo.zpp
constexpr bool op_CShotInfo_AddShot(const CShotInfo_ZppContract& pre, const CShotInfo_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_inUse == true);
}
constexpr CShotInfo_ZppContract apply_CShotInfo_AddShot(const CShotInfo_ZppContract& pre) {
    CShotInfo_ZppContract post = pre;
    post.m_inUse = true;
    return post;
}
// Delta Operation Schema Contract (ΔCWeapon) from zpp/weapons/Weapon.zpp
constexpr bool op_CWeapon_CWeapon(const CWeapon_ZppContract& pre, const CWeapon_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nAmmoInClip == 0);
}
constexpr CWeapon_ZppContract apply_CWeapon_CWeapon(const CWeapon_ZppContract& pre) {
    CWeapon_ZppContract post = pre;
    post.m_nAmmoInClip = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCWeapon) from zpp/weapons/Weapon.zpp
constexpr bool op_CWeapon_InitialiseWeapons(const CWeapon_ZppContract& pre, const CWeapon_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.bPhotographHasBeenTaken == false);
}
constexpr CWeapon_ZppContract apply_CWeapon_InitialiseWeapons(const CWeapon_ZppContract& pre) {
    CWeapon_ZppContract post = pre;
    post.bPhotographHasBeenTaken = false;
    return post;
}
// Delta Operation Schema Contract (ΔCWeapon) from zpp/weapons/Weapon.zpp
constexpr bool op_CWeapon_Initialise(const CWeapon_ZppContract& pre, const CWeapon_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nAmmoInClip == 0);
}
constexpr CWeapon_ZppContract apply_CWeapon_Initialise(const CWeapon_ZppContract& pre) {
    CWeapon_ZppContract post = pre;
    post.m_nAmmoInClip = 0;
    return post;
}
constexpr bool verify_weapons_zpp_invariants() {
    {
        CBulletInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBulletInfo_ZppContract bad{};
        bad.m_pSource_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CShotInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CShotInfo_ZppContract bad{};
        bad.ms_afRandTable_len = 0u;
        if (bad.checkInvariant()) return false;
    }
    {
        CWeapon_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeapon_ZppContract bad{};
        bad.m_nTimer = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        CWeaponEffects_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeaponEffects_ZppContract bad{};
        bad.m_nRed = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        WeaponTypeModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        WeaponTypeModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        CBulletInfo_ZppContract pre{};
        pre.m_bInUse = true;
        const auto post = apply_CBulletInfo_Initialise(pre);
        if (!op_CBulletInfo_Initialise(pre, post)) return false;
        CBulletInfo_ZppContract badPost = post;
        badPost.m_bInUse = true;
        if (op_CBulletInfo_Initialise(pre, badPost)) return false;
    }
    {
        CShotInfo_ZppContract pre{};
        pre.m_inUse = true;
        const auto post = apply_CShotInfo_Initialise(pre);
        if (!op_CShotInfo_Initialise(pre, post)) return false;
        CShotInfo_ZppContract badPost = post;
        badPost.m_inUse = true;
        if (op_CShotInfo_Initialise(pre, badPost)) return false;
    }
    {
        CShotInfo_ZppContract pre{};
        pre.m_inUse = false;
        const auto post = apply_CShotInfo_AddShot(pre);
        if (!op_CShotInfo_AddShot(pre, post)) return false;
        CShotInfo_ZppContract badPost = post;
        badPost.m_inUse = false;
        if (op_CShotInfo_AddShot(pre, badPost)) return false;
    }
    {
        CWeapon_ZppContract pre{};
        pre.m_nAmmoInClip = 1;
        const auto post = apply_CWeapon_CWeapon(pre);
        if (!op_CWeapon_CWeapon(pre, post)) return false;
        CWeapon_ZppContract badPost = post;
        badPost.m_nAmmoInClip = 1;
        if (op_CWeapon_CWeapon(pre, badPost)) return false;
    }
    {
        CWeapon_ZppContract pre{};
        pre.bPhotographHasBeenTaken = true;
        const auto post = apply_CWeapon_InitialiseWeapons(pre);
        if (!op_CWeapon_InitialiseWeapons(pre, post)) return false;
        CWeapon_ZppContract badPost = post;
        badPost.bPhotographHasBeenTaken = true;
        if (op_CWeapon_InitialiseWeapons(pre, badPost)) return false;
    }
    {
        CWeapon_ZppContract pre{};
        pre.m_nAmmoInClip = 1;
        const auto post = apply_CWeapon_Initialise(pre);
        if (!op_CWeapon_Initialise(pre, post)) return false;
        CWeapon_ZppContract badPost = post;
        badPost.m_nAmmoInClip = 1;
        if (op_CWeapon_Initialise(pre, badPost)) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_weapons_zpp_invariants(), "Z++ formal contract invariant failed for weapons");
} // namespace zpp_weapons_spec
#endif // GTANGEL_ZPP_WEAPONS_SPEC_GUARD
