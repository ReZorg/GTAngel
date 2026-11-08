#pragma once
#ifndef GTANGEL_ZPP_FAKERW_SPEC_GUARD
#define GTANGEL_ZPP_FAKERW_SPEC_GUARD
#include <array>
#include <cstddef>
#include <cstdint>
// ============================================================================
// Z++ Formal Specification Contract Block: src/fakerw
// Derived from C:/ddd/GTAngel/cpp_test/zpp/fakerw/*.zpp
// Modules: 12 | Formal Schemas: 146 | Free Types: 31
// ============================================================================
namespace zpp_fakerw_spec {
static constexpr int kZppModuleCount = 12;
static constexpr int kZppSchemaCount = 146;
static constexpr int kZppEnumCount   = 31;
// Free Type from zpp/fakerw/rphanim.zpp
enum class RpHAnimHierarchyFlag : int32_t { rpHANIMHIERARCHYSUBHIERARCHY_v = 0, rpHANIMHIERARCHYNOMATRICES_v = 1, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES_v = 2, rpHANIMHIERARCHYUPDATELTMS_v = 3, rpHANIMHIERARCHYLOCALSPACEMATRICES_v = 4 };
// Free Type from zpp/fakerw/rpmatfx.zpp
enum class RpMatFXMaterialFlags : int32_t { rpMATFXEFFECTNULL_v = 0, rpMATFXEFFECTBUMPMAP_v = 1, rpMATFXEFFECTENVMAP_v = 2, rpMATFXEFFECTBUMPENVMAP_v = 3, rpMATFXEFFECTDUAL_v = 4, rpMATFXEFFECTMAX_v = 5, rpMATFXNUMEFFECTS_v = 6 };
// Free Type from zpp/fakerw/rpworld.zpp
enum class RpLightType : int32_t { rpNALIGHTTYPE_v = 0, rpLIGHTDIRECTIONAL_v = 1, rpLIGHTAMBIENT_v = 2, rpLIGHTPOINT_v = 128, rpLIGHTSPOT_v = 129, rpLIGHTSPOTSOFT_v = 130 };
// Free Type from zpp/fakerw/rpworld.zpp
enum class RpLightFlag : int32_t { rpLIGHTLIGHTATOMICS_v = 1, rpLIGHTLIGHTWORLD_v = 2 };
// Free Type from zpp/fakerw/rpworld.zpp
enum class RpGeometryFlag : int32_t { rpGEOMETRYTRISTRIP_v = 0, rpGEOMETRYPOSITIONS_v = 1, rpGEOMETRYTEXTURED_v = 2, rpGEOMETRYPRELIT_v = 3, rpGEOMETRYNORMALS_v = 4, rpGEOMETRYLIGHT_v = 5, rpGEOMETRYMODULATEMATERIALCOLOR_v = 6, rpGEOMETRYTEXTURED2_v = 7, rpGEOMETRYNATIVE_v = 8, rpGEOMETRYNATIVEINSTANCE_v = 9, rpGEOMETRYFLAGSMASK_v = 255, rpGEOMETRYNATIVEFLAGSMASK_v = 251658240 };
// Free Type from zpp/fakerw/rpworld.zpp
enum class RpGeometryLockMode : int32_t { rpGEOMETRYLOCKPOLYGONS_v = 0, rpGEOMETRYLOCKVERTICES_v = 1, rpGEOMETRYLOCKNORMALS_v = 2, rpGEOMETRYLOCKPRELIGHT_v = 3, rpGEOMETRYLOCKTEXCOORDS_v = 4, rpGEOMETRYLOCKTEXCOORDS1_v = 5, rpGEOMETRYLOCKTEXCOORDS2_v = 6, rpGEOMETRYLOCKTEXCOORDS3_v = 7, rpGEOMETRYLOCKTEXCOORDS4_v = 8, rpGEOMETRYLOCKTEXCOORDS5_v = 9, rpGEOMETRYLOCKTEXCOORDS6_v = 10, rpGEOMETRYLOCKTEXCOORDS7_v = 11, rpGEOMETRYLOCKTEXCOORDS8_v = 12, rpGEOMETRYLOCKTEXCOORDSALL_v = 13, rpGEOMETRYLOCKALL_v = 14 };
// State Schema Contract from zpp/fakerw/fake.zpp
struct fakeModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{1172}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 1172);
    }
};
// State Schema Contract from zpp/fakerw/rpanisot.zpp
struct rpanisotModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{6}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 6);
    }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdKeyFrame_ZppContract {
    uint8_t prevFrame_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    float time{0.0f}; // Z++: ℝ
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const {
        return (prevFrame_card <= 1u);
    }
};
// State Schema Contract from zpp/fakerw/rphanim.zpp
struct RpHAnimStdInterpFrame_ZppContract {
    uint8_t keyFrame1_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    uint8_t keyFrame2_card{0u}; // Z++: Ref[RpHAnimStdKeyFrame]
    int32_t q{0}; // Z++: RtQuat
    int32_t t{0}; // Z++: RwV3d
    constexpr bool checkInvariant() const {
        return (keyFrame1_card <= 1u)
            && (keyFrame2_card <= 1u);
    }
};
// State Schema Contract from zpp/fakerw/rpmatfx.zpp
struct rpmatfxModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{43}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 43);
    }
};
// State Schema Contract from zpp/fakerw/rpskin.zpp
struct RwMatrixWeights_ZppContract {
    float w0{0.0f}; // Z++: ℝ
    float w1{0.0f}; // Z++: ℝ
    float w2{0.0f}; // Z++: ℝ
    float w3{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rpworld.zpp
struct RpClumpChunkInfo_ZppContract {
    int32_t numAtomics{0}; // Z++: ℤ
    int32_t numLights{0}; // Z++: ℤ
    int32_t numCameras{0}; // Z++: ℤ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rtbmp.zpp
struct rtbmpModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{4}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 4);
    }
};
// State Schema Contract from zpp/fakerw/rtcharse.zpp
struct rtcharseModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{14}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 14);
    }
};
// State Schema Contract from zpp/fakerw/rtpng.zpp
struct rtpngModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{4}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 4);
    }
};
// State Schema Contract from zpp/fakerw/rtquat.zpp
struct RtQuat_ZppContract {
    int32_t imag{0}; // Z++: rw__V3d
    float real{0.0f}; // Z++: ℝ
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rwcore.zpp
struct rwcoreModuleState_ZppContract {
    bool module_loaded{true}; // Z++: 𝔹
    int64_t source_lines{684}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (module_loaded == true)
            && (source_lines == 684);
    }
};
// State Schema Contract from zpp/fakerw/rwplcore.zpp
struct RwMemoryFunctions_ZppContract {
    int32_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rwplcore.zpp
struct RwMemory_ZppContract {
    uint8_t start_card{0u}; // Z++: ℙ ℕ
    int64_t length{0}; // Z++: ℕ
    constexpr bool checkInvariant() const {
        return (start_card <= 1u)
            && (length >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rwplcore.zpp
struct RwEngineOpenParams_ZppContract {
    int32_t displayID{0}; // Z++: ADDRESS
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
// State Schema Contract from zpp/fakerw/rwplcore.zpp
struct RwFileFunctions_ZppContract {
    int32_t rwfexist{0}; // Z++: rwFnFexist
    int32_t rwfopen{0}; // Z++: rwFnFopen
    int32_t rwfclose{0}; // Z++: rwFnFclose
    int32_t rwfread{0}; // Z++: rwFnFread
    int32_t rwfwrite{0}; // Z++: rwFnFwrite
    int32_t rwfgets{0}; // Z++: rwFnFgets
    int32_t rwfputs{0}; // Z++: rwFnFputs
    int32_t rwfeof{0}; // Z++: rwFnFeof
    int64_t instance_addr{0}; // Z++: ADDRESS
    constexpr bool checkInvariant() const {
        return (instance_addr >= 0);
    }
};
constexpr bool verify_fakerw_zpp_invariants() {
    {
        fakeModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        fakeModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        rpanisotModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rpanisotModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RpHAnimStdKeyFrame_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RpHAnimStdKeyFrame_ZppContract bad{};
        bad.prevFrame_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        RpHAnimStdInterpFrame_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RpHAnimStdInterpFrame_ZppContract bad{};
        bad.keyFrame1_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        rpmatfxModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rpmatfxModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RwMatrixWeights_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwMatrixWeights_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RpClumpChunkInfo_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RpClumpChunkInfo_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        rtbmpModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rtbmpModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        rtcharseModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rtcharseModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        rtpngModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rtpngModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RtQuat_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RtQuat_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        rwcoreModuleState_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        rwcoreModuleState_ZppContract bad{};
        bad.module_loaded = false;
        if (bad.checkInvariant()) return false;
    }
    {
        RwMemoryFunctions_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwMemoryFunctions_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RwMemory_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwMemory_ZppContract bad{};
        bad.start_card = 2u;
        if (bad.checkInvariant()) return false;
    }
    {
        RwEngineOpenParams_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwEngineOpenParams_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    {
        RwFileFunctions_ZppContract valid{};
        if (!valid.checkInvariant()) return false;
        RwFileFunctions_ZppContract bad{};
        bad.instance_addr = -1;
        if (bad.checkInvariant()) return false;
    }
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_fakerw_zpp_invariants(), "Z++ formal contract invariant failed for fakerw");
} // namespace zpp_fakerw_spec
#endif // GTANGEL_ZPP_FAKERW_SPEC_GUARD
namespace v15render {
// Z++ Formal Schema Contracts derived from cpp_test/zpp/{vehicles,peds,renderer,animation}/*.zpp
namespace zpp_spec {
enum class eCarNodes : uint8_t {
    CAR_WHEEL_RF = 1, CAR_WHEEL_RM, CAR_WHEEL_RB, CAR_WHEEL_LF, CAR_WHEEL_LM, CAR_WHEEL_LB,
    CAR_BUMP_FRONT, CAR_BUMP_REAR, CAR_WING_RF, CAR_WING_RR, CAR_DOOR_RF, CAR_DOOR_RR,
    CAR_WING_LF, CAR_WING_LR, CAR_DOOR_LF, CAR_DOOR_LR, CAR_BONNET, CAR_BOOT, CAR_WINDSCREEN, NUM_CAR_NODES
};
enum class eEngineStatus : uint8_t { STEAM1 = 100, STEAM2 = 150, SMOKE = 200, ON_FIRE = 225 };
enum class eWheelStatus  : uint8_t { OK = 0, BURST = 1, MISSING = 2 };
enum class eFormation    : uint8_t { UNDEFINED = 0, REAR, REAR_LEFT, REAR_RIGHT, FRONT_LEFT, FRONT_RIGHT, LEFT, RIGHT, FRONT };
struct CDamageManagerSpec {
    float m_fWheelDamageEffect{0.0f};
    uint8_t m_engineStatus{0};
    uint8_t m_wheelStatus[4]{0, 0, 0, 0};
    uint8_t m_doorStatus[6]{0, 0, 0, 0, 0, 0};
    constexpr bool invariant() const { return m_fWheelDamageEffect >= 0.0f && m_fWheelDamageEffect <= 1.0f; }
};
struct CTimeCycleSpec {
    uint8_t m_nCurrentSkyTopBlue{46}, m_nCurrentSkyTopGreen{28}, m_nCurrentSkyTopRed{34};
    uint8_t m_nCurrentLowCloudsBlue{128}, m_nCurrentLowCloudsGreen{86}, m_nCurrentLowCloudsRed{148};
    float m_fCurrentLightsOnGroundBrightness{1.0f};
    constexpr bool invariant() const { return m_fCurrentLightsOnGroundBrightness >= 0.0f; }
};
} // namespace zpp_spec

using avatar3d::Vec3;

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static Vec3 cross3(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

static Vec3 normalize3(Vec3 value) {
    float length = sqrtf(value.x * value.x + value.y * value.y + value.z * value.z);
    if (length <= 1e-6f) return {0.0f, 1.0f, 0.0f};
    return {value.x / length, value.y / length, value.z / length};
}

struct Colour {
    uint8_t b{0}, g{0}, r{0}, a{255};
};

static Colour shade(Colour base, float illumination, float fog) {
    illumination = clamp01(illumination);
    fog = clamp01(fog);
    const float skyB = 46.0f, skyG = 30.0f, skyR = 34.0f;
    Colour result;
    result.b = (uint8_t)std::lround((base.b * illumination) * (1.0f - fog) + skyB * fog);
    result.g = (uint8_t)std::lround((base.g * illumination) * (1.0f - fog) + skyG * fog);
    result.r = (uint8_t)std::lround((base.r * illumination) * (1.0f - fog) + skyR * fog);
    result.a = 255;
    return result;
}

enum ObjectId : uint16_t {
    ObjectSky = 0,
    ObjectGround = 1,
    ObjectObstacle = 2,
    ObjectGoal = 3,
    ObjectAvatar = 4,
    ObjectGrid = 5,
    ObjectNpc = 6,
    ObjectQuestItem = 7,
    ObjectVehicle = 8,
    ObjectTraffic = 9,
    ObjectPickup = 10,
    ObjectProp = 11
};

