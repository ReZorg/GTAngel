#pragma once
#ifndef GTANGEL_ZPP_WEAPONS_SPEC_GUARD
#define GTANGEL_ZPP_WEAPONS_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/weapons
// Derived from C:/ddd/GTAngel/native/re3_reference/zpp/weapons/*.zpp
// Modules: 8 | Formal Schemas: 171 | Free Types: 8
// ============================================================================
namespace zpp_weapons_spec {
static constexpr int kZppModuleCount = 8;
static constexpr int kZppSchemaCount = 171;
static constexpr int kZppEnumCount   = 8;
struct ZppVec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};
    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }
};
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
    ZppVec3 m_vecPosition{}; // Z++: CVector
    ZppVec3 m_vecSpeed{}; // Z++: CVector
    int32_t m_nDamage{0}; // Z++: ℤ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pSource_card <= 1u);
    }
};
// State Schema Contract from zpp/weapons/Explosion.zpp
struct CExplosion_ZppContract {
    int32_t m_ExplosionType{0}; // Z++: eExplosionType
    ZppVec3 m_vecPosition{}; // Z++: CVector
    float m_fRadius{0.0f}; // Z++: ℝ
    float m_fPropagationRate{0.0f}; // Z++: ℝ
    uint8_t m_pCreatorEntity_card{0u}; // Z++: Ref[CEntity]
    uint8_t m_pVictimEntity_card{0u}; // Z++: Ref[CEntity]
    float m_fStopTime{0.0f}; // Z++: ℝ
    int64_t m_nIteration{0}; // Z++: ℕ
    int64_t m_nActiveCounter{0}; // Z++: ℕ
    bool m_bIsBoat{false}; // Z++: 𝔹
    bool m_bMakeSound{false}; // Z++: 𝔹
    float m_fStartTime{0.0f}; // Z++: ℝ
    int64_t m_nParticlesExpireTime{0}; // Z++: ℕ
    float m_fPower{0.0f}; // Z++: ℝ
    float m_fZshift{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_pCreatorEntity_card <= 1u)
            && (m_pVictimEntity_card <= 1u)
            && (m_nIteration >= 0)
            && (m_nActiveCounter >= 0)
            && (m_nParticlesExpireTime >= 0);
    }
};
// State Schema Contract from zpp/weapons/ShotInfo.zpp
struct CShotInfo_ZppContract {
    int32_t m_weapon{0}; // Z++: eWeaponType
    ZppVec3 m_startPos{}; // Z++: CVector
    ZppVec3 m_areaAffected{}; // Z++: CVector
    float m_radius{0.0f}; // Z++: ℝ
    uint8_t m_sourceEntity_card{0u}; // Z++: Ref[CEntity]
    float m_timeout{0.0f}; // Z++: ℝ
    bool m_inUse{false}; // Z++: 𝔹
    std::array<float, 20> ms_afRandTable{}; // Z++: seq ℝ  -- bounded array [20]
    size_t ms_afRandTable_len{20u}; // Z++ cardinality #ms_afRandTable
    [[nodiscard]] constexpr bool checkInvariant() const {
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
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_nTimer >= 0);
    }
};
// State Schema Contract from zpp/weapons/WeaponEffects.zpp
struct CWeaponEffects_ZppContract {
    bool m_bActive{false}; // Z++: 𝔹
    ZppVec3 m_vecPos{}; // Z++: CVector
    int64_t m_nRed{0}; // Z++: ℕ
    int64_t m_nGreen{0}; // Z++: ℕ
    int64_t m_nBlue{0}; // Z++: ℕ
    int64_t m_nAlpha{0}; // Z++: ℕ
    float m_fSize{0.0f}; // Z++: ℝ
    float m_fRotation{0.0f}; // Z++: ℝ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (m_nRed >= 0)
            && (m_nGreen >= 0)
            && (m_nBlue >= 0)
            && (m_nAlpha >= 0);
    }
};
// State Schema Contract from zpp/weapons/WeaponInfo.zpp
struct CWeaponInfo_ZppContract {
    std::array<uint32_t, 37> ms_aReloadSampleTime{}; // Z++: seq ℕ  -- bounded array [WEAPONTYPE_TOTALWEAPONS]
    size_t ms_aReloadSampleTime_len{37u}; // Z++ cardinality #ms_aReloadSampleTime
    std::array<int32_t, 37> ms_aMaxAmmoForWeapon{}; // Z++: seq ℤ  -- bounded array [WEAPONTYPE_TOTALWEAPONS]
    size_t ms_aMaxAmmoForWeapon_len{37u}; // Z++ cardinality #ms_aMaxAmmoForWeapon
    int32_t m_eWeaponFire{0}; // Z++: eWeaponFire
    float m_fRange{0.0f}; // Z++: ℝ
    int64_t m_nFiringRate{0}; // Z++: ℕ
    int64_t m_nReload{0}; // Z++: ℕ
    int32_t m_nAmountofAmmunition{0}; // Z++: ℤ
    int64_t m_nDamage{0}; // Z++: ℕ
    float m_fSpeed{0.0f}; // Z++: ℝ
    float m_fRadius{0.0f}; // Z++: ℝ
    float m_fLifespan{0.0f}; // Z++: ℝ
    float m_fSpread{0.0f}; // Z++: ℝ
    ZppVec3 m_vecFireOffset{}; // Z++: CVector
    int32_t m_AnimToPlay{0}; // Z++: AssocGroupId
    float m_fAnimLoopStart{0.0f}; // Z++: ℝ
    float m_fAnimLoopEnd{0.0f}; // Z++: ℝ
    float m_fAnimFrameFire{0.0f}; // Z++: ℝ
    float m_fAnim2LoopStart{0.0f}; // Z++: ℝ
    float m_fAnim2LoopEnd{0.0f}; // Z++: ℝ
    float m_fAnim2FrameFire{0.0f}; // Z++: ℝ
    float m_fAnimBreakout{0.0f}; // Z++: ℝ
    int32_t m_nModelId{0}; // Z++: ℤ
    int32_t m_nModel2Id{0}; // Z++: ℤ
    int64_t m_Flags{0}; // Z++: ℕ
    int64_t m_nWeaponSlot{0}; // Z++: ℕ
    int32_t m_Anim2ToPlay{0}; // Z++: AnimationId
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (ms_aReloadSampleTime_len == 37u && ms_aReloadSampleTime.size() == 37u)
            && (ms_aMaxAmmoForWeapon_len == 37u && ms_aMaxAmmoForWeapon.size() == 37u)
            && (m_nFiringRate >= 0)
            && (m_nReload >= 0)
            && (m_nDamage >= 0)
            && (m_Flags >= 0)
            && (m_nWeaponSlot >= 0);
    }
};
// State Schema Contract from zpp/weapons/WeaponType.zpp
struct WeaponTypeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{99}; // Z++: ℕ
    [[nodiscard]] constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 99);
    }
};
// Query Operation Schema Contract (ΞCWeaponInfo) from zpp/weapons/WeaponInfo.zpp
constexpr bool op_CWeaponInfo_IsFlagSet_8(const CWeaponInfo_ZppContract& state) {
    return state.checkInvariant();
}
// Delta Operation Schema Contract (ΔCExplosion) from zpp/weapons/Explosion.zpp
constexpr bool op_CExplosion_ResetExplosionActiveCounter(const CExplosion_ZppContract& pre, const CExplosion_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nActiveCounter == 0);
}
constexpr CExplosion_ZppContract apply_CExplosion_ResetExplosionActiveCounter(const CExplosion_ZppContract& pre) {
    CExplosion_ZppContract post = pre;
    post.m_nActiveCounter = 0;
    return post;
}
// Delta Operation Schema Contract (ΔCExplosion) from zpp/weapons/Explosion.zpp
constexpr bool op_CExplosion_RemoveAllExplosionsInArea(const CExplosion_ZppContract& pre, const CExplosion_ZppContract& post) {
    return pre.checkInvariant() && post.checkInvariant() && (post.m_nIteration == 0);
}
constexpr CExplosion_ZppContract apply_CExplosion_RemoveAllExplosionsInArea(const CExplosion_ZppContract& pre) {
    CExplosion_ZppContract post = pre;
    post.m_nIteration = 0;
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
constexpr bool verify_weapons_zpp_invariants() {
    {
        CBulletInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CBulletInfo_ZppContract bad{};
        bad.m_pSource_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        CExplosion_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CExplosion_ZppContract bad{};
        bad.m_pCreatorEntity_card = 2u;
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
        CWeaponInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        CWeaponInfo_ZppContract bad{};
        bad.ms_aReloadSampleTime_len = 0u;
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
        const CWeaponInfo_ZppContract st{};
        if (!op_CWeaponInfo_IsFlagSet_8(st)) return false;
    }
    {
        CExplosion_ZppContract pre{};
        pre.m_nActiveCounter = 1;
        const auto post = apply_CExplosion_ResetExplosionActiveCounter(pre);
        if (!op_CExplosion_ResetExplosionActiveCounter(pre, post)) return false;
        {
            CExplosion_ZppContract badPost = post;
            badPost.m_nActiveCounter = 1;
            if (op_CExplosion_ResetExplosionActiveCounter(pre, badPost)) return false;
        }
    }
    {
        CExplosion_ZppContract pre{};
        pre.m_nIteration = 1;
        const auto post = apply_CExplosion_RemoveAllExplosionsInArea(pre);
        if (!op_CExplosion_RemoveAllExplosionsInArea(pre, post)) return false;
        {
            CExplosion_ZppContract badPost = post;
            badPost.m_nIteration = 1;
            if (op_CExplosion_RemoveAllExplosionsInArea(pre, badPost)) return false;
        }
    }
    {
        CWeapon_ZppContract pre{};
        pre.bPhotographHasBeenTaken = true;
        const auto post = apply_CWeapon_InitialiseWeapons(pre);
        if (!op_CWeapon_InitialiseWeapons(pre, post)) return false;
        {
            CWeapon_ZppContract badPost = post;
            badPost.bPhotographHasBeenTaken = true;
            if (op_CWeapon_InitialiseWeapons(pre, badPost)) return false;
        }
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_weapons_zpp_invariants(), "Z++ formal contract invariant failed for weapons");
} // namespace zpp_weapons_spec
#endif // GTANGEL_ZPP_WEAPONS_SPEC_GUARD
