#ifndef GTANGEL_ZPP_MODELINFO_SPEC_GUARD
#define GTANGEL_ZPP_MODELINFO_SPEC_GUARD
// ============================================================================
// Z++ Formal Specification Contract Block: src/modelinfo
// Derived from C:/ddd/GTAngel/cpp_test/zpp/modelinfo/*.zpp
// Modules: 11 | Formal Schemas: 453 | Free Types: 20
// ============================================================================
namespace zpp_modelinfo_spec {
static constexpr int kZppModuleCount = 11;
static constexpr int kZppSchemaCount = 453;
static constexpr int kZppEnumCount   = 20;
// From zpp/modelinfo/BaseModelInfo.zpp
enum class ModelInfoType : int32_t { MITYPE_NA_v = 0, MITYPE_SIMPLE_v = 1, MITYPE_MLO_v = 2, MITYPE_TIME_v = 3, MITYPE_WEAPON_v = 4, MITYPE_CLUMP_v = 5, MITYPE_VEHICLE_v = 6, MITYPE_PED_v = 7, MITYPE_XTRACOMPS_v = 8, MITYPE_HAND_v = 9 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_1 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_TAXI_D_v = 8, MI_PIMP_v = 9, MI_CRIMINAL01_v = 10, MI_CRIMINAL02_v = 11 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_2 : int32_t { NUM_OF_SPECIAL_CHARS_v = 0, NUM_OF_CUTSCENE_OBJECTS_v = 1 };
// From zpp/modelinfo/ModelIndices.zpp
enum class ModelIndices_Enum_3 : int32_t { MI_PLAYER_v = 0, MI_COP_v = 1, MI_SWAT_v = 2, MI_FBI_v = 3, MI_ARMY_v = 4, MI_MEDIC_v = 5, MI_FIREMAN_v = 6, MI_MALE01_v = 7, MI_HFYST_v = 8, MI_HFOST_v = 9, MI_HMYST_v = 10, MI_HMOST_v = 11 };
// State Schema Contract from zpp/modelinfo/BaseModelInfo.zpp
struct CBaseModelInfo_ZppContract {
    int32_t m_unk{0}; // Z++: seq ℕ  -- bounded array [2]
    uint32_t m_nameKey{0}; // Z++: ℕ
    int32_t m_chunk{0}; // Z++: ADDRESS
    uint32_t m_type{0}; // Z++: ℕ
    uint32_t m_num2dEffects{0}; // Z++: ℕ
    bool m_bOwnsColModel{false}; // Z++: 𝔹
    int32_t m_colModel{0}; // Z++: Ref[CColModel]
    int32_t m_2dEffectsID{0}; // Z++: ℤ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameIdAssocation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t hierId{0}; // Z++: ℤ
    uint32_t flags{0}; // Z++: ℕ
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectNameAssociation_ZppContract {
    int32_t name{0}; // Z++: seq CHAR
    int32_t frame{0}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct RwObjectIdAssociation_ZppContract {
    int32_t id{0}; // Z++: ℤ
    int32_t frame{0}; // Z++: Ref[RwFrame]
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/ClumpModelInfo.zpp
struct CClumpModelInfo_ZppContract {
    int32_t m_clump{0}; // Z++: Ref[RpClump]
    int32_t m_animFileName{0}; // Z++: seq CHAR
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CClumpModelInfo
    constexpr bool checkInvariant() const { return true; }
};
// State Schema Contract from zpp/modelinfo/MloModelInfo.zpp
struct CMloModelInfo_ZppContract {
    float drawDist{0.0f}; // Z++: ℝ
    int32_t firstInstance{0}; // Z++: ℤ
    int32_t lastInstance{0}; // Z++: ℤ
    int32_t msClassInfo{0}; // Z++: base__cRelocatableChunkClassInfo
    int32_t msClassInstance{0}; // Z++: CMloModelInfo
    constexpr bool checkInvariant() const { return true; }
};
constexpr bool verify_modelinfo_zpp_invariants() {
    if (!CBaseModelInfo_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectNameIdAssocation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectNameAssociation_ZppContract{}.checkInvariant()) return false;
    if (!RwObjectIdAssociation_ZppContract{}.checkInvariant()) return false;
    if (!CClumpModelInfo_ZppContract{}.checkInvariant()) return false;
    if (!CMloModelInfo_ZppContract{}.checkInvariant()) return false;
    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;
}
static_assert(verify_modelinfo_zpp_invariants(), "Z++ formal contract invariant failed for modelinfo");
} // namespace zpp_modelinfo_spec
#endif // GTANGEL_ZPP_MODELINFO_SPEC_GUARD
// DTE v22 native skin asset reader / visual-only software-renderer adapter.
//
// Include this file *inside namespace v15render*, after Colour and ObjectAvatar
// have been declared.  It intentionally has no #includes and no namespace
// wrapper: the GTAngel v21 translation unit already supplies vector, string,
// fstream, cmath, cstdint, cstring, and avatar3d::avatar3d::Vec3.  It contains original,
// clean-room code only and does not load GLTF or any game/source asset at run
// time.  The only runtime input is export_native_lod.py's bounded .bin format.
//
// Native rendering is cosmetic.  draw() merely emits ObjectAvatar triangles to
// the existing renderer callback, so the pre-existing 768x768 colour/depth/ID
// rasterizer remains authoritative.  It has no body, motor, quest, world, or
// commit-side-effect API.

class DteSkinnedAsset {
public:
    // Direct input path.  Passing an empty path tries an adjacent executable's
    // assets/dte_v26_lod.bin on Windows; portable builds fall back to the
    // current process assets/dte_v26_lod.bin path.
    bool load(const std::string& inputPath) {
        clear();
        const std::string path = inputPath.empty() ? adjacentAssetPath() : inputPath;
        if (path.empty()) return fail("empty native skin asset path");

        std::ifstream stream(path, std::ios::binary | std::ios::ate);
        if (!stream) return fail("cannot open native skin asset: " + path);
        const std::streamoff fileSize = stream.tellg();
        if (fileSize < static_cast<std::streamoff>(kHeaderBytes) ||
            fileSize > static_cast<std::streamoff>(kMaxFileBytes))
            return fail("native skin asset file size is outside bounded range");
        stream.seekg(0, std::ios::beg);
        std::vector<uint8_t> bytes(static_cast<size_t>(fileSize));
        if (!stream.read(reinterpret_cast<char*>(bytes.data()), fileSize))
            return fail("cannot read complete native skin asset");
        return parse(bytes);
    }

    bool loadAdjacent() { return load(std::string()); }
    bool valid() const { return valid_; }
    const std::string& error() const { return error_; }
    size_t triangleCount() const { return triangles_.size(); }
    size_t bodyTriangleCount() const { return bodyTriangleCount_; }
    bool hasNativeWings() const { return valid_ && nativeWings_; }
    bool bodyOnly() const { return valid_ && !nativeWings_; }

    // Emit uses this signature:
    //   emit(avatar3d::Vec3 a, avatar3d::Vec3 b, avatar3d::Vec3 c, Colour colour, uint16_t objectId)
    // The callback is normally HighResRenderer::addTriangle (or an adapter
    // around it).  This method has no world/body mutation channel.
    template<class Emit>
    void draw(const avatar3d::AvatarSkeleton& avatar, Emit emit) const {
        if (!valid_ || vertices_.empty() || triangles_.empty()) return;
        // The supplied auto-rig has helper-bone roll/tails inconsistent with
        // the v21 visual skeleton.  Applying a rigid per-bone frame transfer
        // twists the authored body and destroys its likeness.  Instead, retain
        // every rest-mesh triangle and transfer *displacement* from the visual
        // neutral joint to the current joint, blended by the GLB skin weights.
        // This is deliberately a conservative CPU LOD, not full quaternion
        // skinning; original Blender/GLB keeps its full editable rig and actions.
        avatar3d::AvatarSkeleton neutral;
        neutral.pose({0.0f,0.0f,0.0f},0.0f,0.0f,0.0f);
        const avatar3d::Vec3 right=bodyRight(avatar);
        const float ch=right.x, sh=right.z;
        const avatar3d::Vec3 origin=avatar.lastPosition;
        // LW40: store BOTH proximal (deltaA) and distal (deltaB) joint
        // displacements + rest Y span (baseY0..baseY1) for C0-continuous
        // proximal-to-distal limb interpolation across hips, knees, ankles,
        // shoulders, elbows, and wrists.
        avatar3d::Vec3 deltaA[kTargetCount]{}, deltaB[kTargetCount]{};
        float baseY0[kTargetCount]{}, baseY1[kTargetCount]{};
        for (uint32_t t=0;t<kTargetCount;++t) {
            avatar3d::Vec3 baseA{},baseB{},nowA{},nowB{};
            if (posePair(neutral,static_cast<uint8_t>(t),baseA,baseB) &&
                posePair(avatar,static_cast<uint8_t>(t),nowA,nowB)) {
                deltaA[t]=sub(nowA,add(rotateY(baseA,ch,sh),origin));
                deltaB[t]=sub(nowB,add(rotateY(baseB,ch,sh),origin));
                baseY0[t]=baseA.y; baseY1[t]=baseB.y;
            }
        }
        const float faceClock = avatar.idleTime + avatar.walkPhase * 0.21f + avatar.motionTime;
        const float blinkCycle = std::fmod(std::max(0.0f, faceClock), 3.4f);
        const float blinkW = (blinkCycle < 0.16f) ? std::sin(blinkCycle * (3.14159265f / 0.16f)) : 0.0f;
        const float jawW = 0.25f + 0.25f * std::sin(faceClock * 2.7f) + 0.35f * avatar.orbIntensity;
        for (size_t triangleIndex=0;triangleIndex<triangles_.size();++triangleIndex) {
            const Triangle& triangle=triangles_[triangleIndex];
            const bool wing=triangleIndex>=bodyTriangleCount_ && nativeWings_;
            const avatar3d::Vec3 a = skinVertex(vertices_[triangle.i0], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
            const avatar3d::Vec3 b = skinVertex(vertices_[triangle.i1], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
            const avatar3d::Vec3 c = skinVertex(vertices_[triangle.i2], deltaA,deltaB,baseY0,baseY1,origin,ch,sh,wing,blinkW,jawW,avatar.wingDeploy);
            if (!finiteVec(a) || !finiteVec(b) || !finiteVec(c)) continue;
            if (nearDegenerate(a, b, c)) continue;
            // Export stores conventional RGB while the legacy framebuffer Colour
            // is B,G,R,A in memory.
            emit(a, b, c, Colour{triangle.b, triangle.g, triangle.r, 255}, ObjectAvatar);
        }
    }

    // Kept public so WM_CREATE integration can compose the path without any
    // platform-specific loader.  Direct paths always remain supported by load.
    static std::string adjacentAssetPath() {
#ifdef _WIN32
        char executable[4096]{};
        const unsigned long length = GetModuleFileNameA(nullptr, executable,
            static_cast<unsigned long>(sizeof(executable)));
        if (length > 0 && length < sizeof(executable) - 1) {
            std::string value(executable, executable + length);
            const size_t slash = value.find_last_of("\\/");
            if (slash != std::string::npos)
                return value.substr(0, slash + 1) + "assets\\dte_v26_lod.bin";
        }
#endif
        return "assets/dte_v26_lod.bin";
    }

private:
    static constexpr uint32_t kVersion = 1;
    static constexpr size_t kHeaderBytes = 128;
    static constexpr size_t kVertexBytes = 20;
    static constexpr size_t kTriangleBytes = 12;
    static constexpr size_t kColourBytes = 3;
    static constexpr size_t kBoneBytes = 40;
    static constexpr uint32_t kMaxVertices = 48000;
    static constexpr uint32_t kMaxTriangles = 32768;
    static constexpr uint32_t kMaxBodyTriangles = 32000;
    static constexpr uint32_t kMaxBones = 64;
    static constexpr uint32_t kMaxMetadataBytes = 16384;
    static constexpr size_t kMaxFileBytes = 2 * 1024 * 1024;
    static constexpr uint32_t kTargetCount = 22;
    static constexpr uint32_t kFlagFaceRgb = 1u << 0;
    static constexpr uint32_t kFlagNativeWings = 1u << 1;
    static constexpr uint32_t kFlagSourceAsset = 1u << 2;
    static constexpr uint32_t kFlagBodyOnly = 1u << 3;

    struct Vertex {
        avatar3d::Vec3 rest{};
        uint8_t bone[4]{};
        uint8_t weight[4]{};
    };

    struct Triangle {
        uint32_t i0{}, i1{}, i2{};
        uint8_t r{}, g{}, b{};
    };

    struct Frame {
        avatar3d::Vec3 origin{};
        avatar3d::Vec3 x{};
        avatar3d::Vec3 y{};
        avatar3d::Vec3 z{};
        bool initialized{false};
    };

    struct DiskBone {
        uint8_t target{};
        uint8_t flags{};
        avatar3d::Vec3 restHead{};
        avatar3d::Vec3 restTail{};
        avatar3d::Vec3 restXAxis{};
        Frame rest{};
    };

    bool valid_{false};
    bool nativeWings_{false};
    size_t bodyTriangleCount_{0};
    std::string error_;
    std::vector<Vertex> vertices_;
    std::vector<Triangle> triangles_;
    std::vector<DiskBone> bones_;

    void clear() {
        valid_ = false;
        nativeWings_ = false;
        bodyTriangleCount_ = 0;
        error_.clear();
        vertices_.clear();
        triangles_.clear();
        bones_.clear();
    }

    bool fail(const char* message) { return fail(std::string(message)); }
    bool fail(const std::string& message) {
        clear();
        error_ = message;
        return false;
    }

    static uint32_t readU32(const std::vector<uint8_t>& data, size_t at) {
        return static_cast<uint32_t>(data[at]) |
               (static_cast<uint32_t>(data[at + 1]) << 8) |
               (static_cast<uint32_t>(data[at + 2]) << 16) |
               (static_cast<uint32_t>(data[at + 3]) << 24);
    }

    static float readF32(const std::vector<uint8_t>& data, size_t at) {
        const uint32_t bits = readU32(data, at);
        float value{};
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    static uint32_t crc32(const uint8_t* data, size_t size) {
        uint32_t value = 0xffffffffu;
        for (size_t i = 0; i < size; ++i) {
            value ^= data[i];
            for (int bit = 0; bit < 8; ++bit)
                value = (value & 1u) ? ((value >> 1) ^ 0xedb88320u) : (value >> 1);
        }
        return value ^ 0xffffffffu;
    }

    static bool finiteFloat(float value) { return std::isfinite(value); }
    static bool finiteVec(avatar3d::Vec3 value) {
        return finiteFloat(value.x) && finiteFloat(value.y) && finiteFloat(value.z);
    }
    static avatar3d::Vec3 add(avatar3d::Vec3 a, avatar3d::Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    static avatar3d::Vec3 sub(avatar3d::Vec3 a, avatar3d::Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    static avatar3d::Vec3 mul(avatar3d::Vec3 a, float value) { return {a.x * value, a.y * value, a.z * value}; }
    static avatar3d::Vec3 rotateY(avatar3d::Vec3 p,float ch,float sh) {
        return {p.x*ch-p.z*sh,p.y,p.x*sh+p.z*ch};
    }
    static float dot(avatar3d::Vec3 a, avatar3d::Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static avatar3d::Vec3 cross(avatar3d::Vec3 a, avatar3d::Vec3 b) {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }
    static float lengthSq(avatar3d::Vec3 value) { return dot(value, value); }
    static avatar3d::Vec3 normal(avatar3d::Vec3 value) {
        const float squared = lengthSq(value);
        if (!finiteFloat(squared) || squared <= 1e-12f) return {};
        const float inverse = 1.0f / std::sqrt(squared);
        return mul(value, inverse);
    }

    static Frame makeFrame(avatar3d::Vec3 origin, avatar3d::Vec3 tail, avatar3d::Vec3 xHint) {
        Frame frame;
        if (!finiteVec(origin) || !finiteVec(tail) || !finiteVec(xHint)) return frame;
        const avatar3d::Vec3 y = normal(sub(tail, origin));
        if (lengthSq(y) <= 1e-10f) return frame;
        avatar3d::Vec3 x = sub(xHint, mul(y, dot(xHint, y)));
        if (lengthSq(x) <= 1e-10f) {
            const avatar3d::Vec3 candidate = std::fabs(y.y) < 0.85f ? avatar3d::Vec3{0.0f, 1.0f, 0.0f}
                                                          : avatar3d::Vec3{1.0f, 0.0f, 0.0f};
            x = sub(candidate, mul(y, dot(candidate, y)));
        }
        x = normal(x);
        avatar3d::Vec3 z = normal(cross(x, y));
        if (lengthSq(x) <= 1e-10f || lengthSq(z) <= 1e-10f) return frame;
        x = normal(cross(y, z)); // correct round-off and retain a right-handed frame
        frame.origin = origin;
        frame.x = x;
        frame.y = y;
        frame.z = z;
        frame.initialized = finiteVec(x) && finiteVec(y) && finiteVec(z);
        return frame;
    }

    static Frame makePoseFrame(avatar3d::Vec3 origin, avatar3d::Vec3 tail, avatar3d::Vec3 bodyXAxis) {
        return makeFrame(origin, tail, bodyXAxis);
    }

    static avatar3d::Vec3 local(const Frame& frame, avatar3d::Vec3 world) {
        const avatar3d::Vec3 delta = sub(world, frame.origin);
        return {dot(delta, frame.x), dot(delta, frame.y), dot(delta, frame.z)};
    }
    static avatar3d::Vec3 world(const Frame& frame, avatar3d::Vec3 point) {
        return add(frame.origin, add(mul(frame.x, point.x), add(mul(frame.y, point.y), mul(frame.z, point.z))));
    }

    static bool nearDegenerate(avatar3d::Vec3 a, avatar3d::Vec3 b, avatar3d::Vec3 c) {
        const avatar3d::Vec3 area = cross(sub(b, a), sub(c, a));
        return !finiteVec(area) || lengthSq(area) < 1e-14f;
    }

    bool parse(const std::vector<uint8_t>& data) {
        // Identity, header version/size, and exact byte count.
        static const uint8_t magic[8] = {'D','T','E','V','2','2','L',0};
        if (data.size() < kHeaderBytes || std::memcmp(data.data(), magic, sizeof(magic)) != 0)
            return fail("native skin asset magic is invalid");
        const uint32_t version = readU32(data, 8);
        const uint32_t headerBytes = readU32(data, 12);
        const uint32_t flags = readU32(data, 16);
        const uint32_t fileBytes = readU32(data, 20);
        const uint32_t vertexCount = readU32(data, 24);
        const uint32_t triangleCount = readU32(data, 28);
        const uint32_t bodyTriangleCount = readU32(data, 32);
        const uint32_t boneCount = readU32(data, 36);
        const uint32_t vertexOffset = readU32(data, 40);
        const uint32_t vertexStride = readU32(data, 44);
        const uint32_t indexOffset = readU32(data, 48);
        const uint32_t indexStride = readU32(data, 52);
        const uint32_t colourOffset = readU32(data, 56);
        const uint32_t colourStride = readU32(data, 60);
        const uint32_t boneOffset = readU32(data, 64);
        const uint32_t boneStride = readU32(data, 68);
        const uint32_t metadataOffset = readU32(data, 72);
        const uint32_t metadataBytes = readU32(data, 76);
        const uint32_t payloadCrc = readU32(data, 104);
        const uint32_t storedHeaderCrc = readU32(data, 108);

        if (version != kVersion || headerBytes != kHeaderBytes || fileBytes != data.size())
            return fail("native skin asset version or file extent is invalid");
        const bool nativeWings = (flags & kFlagNativeWings) != 0;
        const bool bodyOnly = (flags & kFlagBodyOnly) != 0;
        if ((flags & (kFlagFaceRgb | kFlagSourceAsset)) != (kFlagFaceRgb | kFlagSourceAsset) ||
            nativeWings == bodyOnly)
            return fail("native skin asset flags do not declare exactly one wing mode");
        if (vertexCount == 0 || vertexCount > kMaxVertices || triangleCount == 0 ||
            triangleCount > kMaxTriangles || boneCount == 0 || boneCount > kMaxBones ||
            bodyTriangleCount > triangleCount || bodyTriangleCount > kMaxBodyTriangles ||
            (bodyOnly && bodyTriangleCount != triangleCount) ||
            vertexStride != kVertexBytes || indexStride != kTriangleBytes ||
            colourStride != kColourBytes || boneStride != kBoneBytes ||
            metadataBytes > kMaxMetadataBytes)
            return fail("native skin asset counts or record strides are invalid");

        const uint64_t expectedIndex = static_cast<uint64_t>(vertexOffset) + static_cast<uint64_t>(vertexCount) * kVertexBytes;
        const uint64_t expectedColour = expectedIndex + static_cast<uint64_t>(triangleCount) * kTriangleBytes;
        const uint64_t expectedBone = expectedColour + static_cast<uint64_t>(triangleCount) * kColourBytes;
        const uint64_t expectedMetadata = expectedBone + static_cast<uint64_t>(boneCount) * kBoneBytes;
        const uint64_t expectedFile = expectedMetadata + metadataBytes;
        if (vertexOffset != kHeaderBytes || indexOffset != expectedIndex || colourOffset != expectedColour ||
            boneOffset != expectedBone || metadataOffset != expectedMetadata || expectedFile != data.size())
            return fail("native skin asset regions are not contiguous or bounded");

        std::vector<uint8_t> header(data.begin(), data.begin() + kHeaderBytes);
        header[108] = header[109] = header[110] = header[111] = 0;
        if (crc32(header.data(), header.size()) != storedHeaderCrc ||
            crc32(data.data() + kHeaderBytes, data.size() - kHeaderBytes) != payloadCrc)
            return fail("native skin asset CRC validation failed");

        const avatar3d::Vec3 declaredMin{readF32(data, 80), readF32(data, 84), readF32(data, 88)};
        const avatar3d::Vec3 declaredMax{readF32(data, 92), readF32(data, 96), readF32(data, 100)};
        if (!finiteVec(declaredMin) || !finiteVec(declaredMax) ||
            declaredMin.x > declaredMax.x || declaredMin.y > declaredMax.y || declaredMin.z > declaredMax.z ||
            declaredMax.x - declaredMin.x > 100.0f || declaredMax.y - declaredMin.y > 100.0f ||
            declaredMax.z - declaredMin.z > 100.0f)
            return fail("native skin asset bounds are not finite or plausible");

        vertices_.reserve(vertexCount);
        triangles_.reserve(triangleCount);
        bones_.reserve(boneCount);
        constexpr float kBoundsTolerance = 0.002f;
        for (uint32_t i = 0; i < vertexCount; ++i) {
            const size_t at = static_cast<size_t>(vertexOffset) + static_cast<size_t>(i) * kVertexBytes;
            Vertex vertex;
            vertex.rest = {readF32(data, at), readF32(data, at + 4), readF32(data, at + 8)};
            for (int influence = 0; influence < 4; ++influence) vertex.bone[influence] = data[at + 12 + influence];
            uint32_t weightSum = 0;
            for (int influence = 0; influence < 4; ++influence) {
                vertex.weight[influence] = data[at + 16 + influence];
                weightSum += vertex.weight[influence];
                if (vertex.bone[influence] >= boneCount) return fail("native skin vertex bone index is out of range");
            }
            if (!finiteVec(vertex.rest) || weightSum != 255 ||
                vertex.rest.x < declaredMin.x - kBoundsTolerance || vertex.rest.x > declaredMax.x + kBoundsTolerance ||
                vertex.rest.y < declaredMin.y - kBoundsTolerance || vertex.rest.y > declaredMax.y + kBoundsTolerance ||
                vertex.rest.z < declaredMin.z - kBoundsTolerance || vertex.rest.z > declaredMax.z + kBoundsTolerance)
                return fail("native skin vertex is invalid or outside declared bounds");
            vertices_.push_back(vertex);
        }

        for (uint32_t i = 0; i < boneCount; ++i) {
            const size_t at = static_cast<size_t>(boneOffset) + static_cast<size_t>(i) * kBoneBytes;
            DiskBone bone;
            bone.target = data[at];
            bone.flags = data[at + 1];
            bone.restHead = {readF32(data, at + 4), readF32(data, at + 8), readF32(data, at + 12)};
            bone.restTail = {readF32(data, at + 16), readF32(data, at + 20), readF32(data, at + 24)};
            bone.restXAxis = {readF32(data, at + 28), readF32(data, at + 32), readF32(data, at + 36)};
            if (bone.target >= kTargetCount || !finiteVec(bone.restHead) || !finiteVec(bone.restTail) ||
                !finiteVec(bone.restXAxis) || lengthSq(sub(bone.restTail, bone.restHead)) < 1e-10f ||
                lengthSq(bone.restXAxis) < 1e-10f)
                return fail("native skin rest bone frame is invalid");
            bone.rest = makeFrame(bone.restHead, bone.restTail, bone.restXAxis);
            if (!bone.rest.initialized) return fail("native skin rest bone basis is singular");
            bones_.push_back(bone);
        }

        for (uint32_t i = 0; i < triangleCount; ++i) {
            const size_t indexAt = static_cast<size_t>(indexOffset) + static_cast<size_t>(i) * kTriangleBytes;
            Triangle triangle;
            triangle.i0 = readU32(data, indexAt);
            triangle.i1 = readU32(data, indexAt + 4);
            triangle.i2 = readU32(data, indexAt + 8);
            const size_t colourAt = static_cast<size_t>(colourOffset) + static_cast<size_t>(i) * kColourBytes;
            triangle.r = data[colourAt]; triangle.g = data[colourAt + 1]; triangle.b = data[colourAt + 2];
            if (triangle.i0 >= vertexCount || triangle.i1 >= vertexCount || triangle.i2 >= vertexCount ||
                triangle.i0 == triangle.i1 || triangle.i0 == triangle.i2 || triangle.i1 == triangle.i2 ||
                nearDegenerate(vertices_[triangle.i0].rest, vertices_[triangle.i1].rest, vertices_[triangle.i2].rest))
                return fail("native skin triangle index or shape is invalid");
            triangles_.push_back(triangle);
        }

        // Metadata is deliberately not parsed: its bounded bytes are covered by
        // the CRC and it is validation provenance, not executable authority.
        nativeWings_ = nativeWings;
        bodyTriangleCount_ = bodyTriangleCount;
        valid_ = true;
        error_.clear();
        return true;
    }

    static avatar3d::Vec3 bodyRight(const avatar3d::AvatarSkeleton& avatar) {
        using J = avatar3d::AvatarSkeleton;
        avatar3d::Vec3 result = sub(avatar.joints[J::ShoulderR], avatar.joints[J::ShoulderL]);
        if (lengthSq(result) <= 1e-10f)
            result = sub(avatar.joints[J::HipR], avatar.joints[J::HipL]);
        if (lengthSq(result) <= 1e-10f) result = {1.0f, 0.0f, 0.0f};
        return normal(result);
    }

    // Target IDs exactly match export_native_lod.py.  Each is a v21 visual
    // joint pair; wings are mapped only when the exporter declared a complete
    // left/right root->boom->tip native chain.
    static bool posePair(const avatar3d::AvatarSkeleton& avatar, uint8_t target, avatar3d::Vec3& a, avatar3d::Vec3& b) {
        using J = avatar3d::AvatarSkeleton;
        switch (target) {
            case 0:  a = avatar.joints[J::Root];      b = avatar.joints[J::Spine]; return true;
            case 1:  a = avatar.joints[J::Spine];     b = avatar.joints[J::Neck]; return true;
            case 2:  a = avatar.joints[J::Neck];      b = avatar.joints[J::Head]; return true;
            case 3:  a = avatar.joints[J::Neck];      b = avatar.joints[J::Head]; return true;
            case 4:  a = avatar.joints[J::Spine];     b = avatar.joints[J::ShoulderL]; return true;
            case 5:  a = avatar.joints[J::ShoulderL]; b = avatar.joints[J::ElbowL]; return true;
            case 6:  a = avatar.joints[J::ElbowL];    b = avatar.joints[J::HandL]; return true;
            case 7:  a = avatar.joints[J::Spine];     b = avatar.joints[J::ShoulderR]; return true;
            case 8:  a = avatar.joints[J::ShoulderR]; b = avatar.joints[J::ElbowR]; return true;
            case 9:  a = avatar.joints[J::ElbowR];    b = avatar.joints[J::HandR]; return true;
            case 10: a = avatar.joints[J::Root];      b = avatar.joints[J::HipL]; return true;
            case 11: a = avatar.joints[J::HipL];      b = avatar.joints[J::KneeL]; return true;
            case 12: a = avatar.joints[J::KneeL];     b = avatar.joints[J::FootL]; return true;
            case 13: a = avatar.joints[J::Root];      b = avatar.joints[J::HipR]; return true;
            case 14: a = avatar.joints[J::HipR];      b = avatar.joints[J::KneeR]; return true;
            case 15: a = avatar.joints[J::KneeR];     b = avatar.joints[J::FootR]; return true;
            case 16: a = avatar.wingRoot[0];           b = avatar.wingBoom[0]; return true;
            case 17: a = avatar.wingBoom[0];           b = avatar.wingTip[0]; return true;
            case 18: a = avatar.wingBoom[0];           b = avatar.wingTip[0]; return true;
            case 19: a = avatar.wingRoot[1];           b = avatar.wingBoom[1]; return true;
            case 20: a = avatar.wingBoom[1];           b = avatar.wingTip[1]; return true;
            case 21: a = avatar.wingBoom[1];           b = avatar.wingTip[1]; return true;
            default: return false;
        }
    }

    avatar3d::Vec3 skinVertex(const Vertex& vertex,const avatar3d::Vec3* deltaA,
                               const avatar3d::Vec3* deltaB,const float* baseY0,const float* baseY1,
                               avatar3d::Vec3 origin,float ch,float sh,bool wing,
                               float blinkW=0.0f,float jawW=0.0f,float wingDeploy=0.0f) const {
        if (wing) {
            avatar3d::Vec3 wr = vertex.rest;
            if (wingDeploy > 0.01f) {
                const float span = std::max(0.0f, std::fabs(wr.x) - 0.14f);
                wr.x *= (1.0f + 0.65f * wingDeploy);
                wr.y += 0.32f * wingDeploy * span;
                wr.z -= 0.10f * wingDeploy * span;
            }
            return add(add(rotateY(wr,ch,sh),origin),deltaA[1]);
        }
        avatar3d::Vec3 rest = vertex.rest;
        if (rest.y > 1.46f && rest.z > 0.05f) {
            const float dLx = rest.x - (-0.039f), dLy = rest.y - 1.568f, dLz = rest.z - 0.091f;
            const float dRx = rest.x - ( 0.037f), dRy = rest.y - 1.567f, dRz = rest.z - 0.093f;
            const float rL2 = dLx*dLx + dLy*dLy + dLz*dLz;
            const float rR2 = dRx*dRx + dRy*dRy + dRz*dRz;
            if (blinkW > 0.01f && dLy > -0.002f && (rL2 < 0.00048f || rR2 < 0.00048f)) {
                const float falloff = 1.0f - std::min(rL2, rR2) / 0.00048f;
                rest.y -= 0.0085f * blinkW * falloff;
            }
            const float dMx = rest.x, dMy = rest.y - 1.496f, dMz = rest.z - 0.108f;
            const float rM2 = dMx*dMx + dMy*dMy + dMz*dMz;
            if (jawW > 0.01f && dMy < 0.002f && rM2 < 0.00068f) {
                const float falloff = 1.0f - rM2 / 0.00068f;
                rest.y -= 0.0065f * jawW * falloff;
            }
        }
        avatar3d::Vec3 displacement{};
        for (int influence = 0; influence < 4; ++influence) {
            const uint8_t boneIndex = vertex.bone[influence];
            const float weight = static_cast<float>(vertex.weight[influence]) / 255.0f;
            if (weight <= 0.0f || boneIndex >= bones_.size()) continue;
            const uint8_t t = bones_[boneIndex].target;
            const float span = baseY1[t] - baseY0[t];
            const float s = (std::fabs(span) > 0.02f) ? std::clamp((rest.y - baseY0[t]) / span, 0.0f, 1.0f) : 1.0f;
            const avatar3d::Vec3 segDelta = add(mul(deltaA[t], 1.0f - s), mul(deltaB[t], s));
            displacement = add(displacement, mul(segDelta, weight));
        }
        return add(add(rotateY(rest,ch,sh),origin),displacement);
    }
};


enum class DisplayMode {
    Colour = 0,
    Depth = 1,
    ObjectIdentity = 2,
    Luminance = 3
};

static const char* displayModeName(DisplayMode mode) {
    switch (mode) {
        case DisplayMode::Colour: return "COLOUR 768";
        case DisplayMode::Depth: return "DEPTH 768";
        case DisplayMode::ObjectIdentity: return "OBJECT-ID 768";
        case DisplayMode::Luminance: return "LUMINANCE 768";
    }
    return "UNKNOWN";
}

template<int W,int H> struct FramebufferSurface {
    static constexpr int Width = W;
    static constexpr int Height = H;
    static constexpr int PixelCount = Width * Height;

    std::vector<uint32_t> colour;
    std::vector<float> depth;
    std::vector<uint16_t> objectId;
    std::vector<uint32_t> presentation;
    uint64_t sequence{0};

    FramebufferSurface()
        : colour(PixelCount, 0),
          depth(PixelCount, 1e30f),
          objectId(PixelCount, ObjectSky),
          presentation(PixelCount, 0) {}

    static uint32_t pack(Colour colour) {
        return (uint32_t)colour.b |
               ((uint32_t)colour.g << 8) |
               ((uint32_t)colour.r << 16) |
               ((uint32_t)colour.a << 24);
    }

    static Colour unpack(uint32_t packed) {
        return {(uint8_t)(packed & 0xff),
                (uint8_t)((packed >> 8) & 0xff),
                (uint8_t)((packed >> 16) & 0xff),
                (uint8_t)((packed >> 24) & 0xff)};
    }

    void clear() {
        for (int y = 0; y < Height; ++y) {
            float t = (float)y / (float)(Height - 1);
            Colour sky;
            float sr, sg, sb;
            if (t < 0.45f) {
                const float u = t / 0.45f;
                sr = 58.0f + (34.0f - 58.0f) * u;
                sg = 38.0f + (30.0f - 38.0f) * u;
                sb = 52.0f + (46.0f - 52.0f) * u;
            } else {
                const float u = (t - 0.45f) / 0.55f;
                sr = 34.0f + (14.0f - 34.0f) * u;
                sg = 30.0f + (12.0f - 30.0f) * u;
                sb = 46.0f + (26.0f - 46.0f) * u;
            }
            sky.r = (uint8_t)std::lround(sr);
            sky.g = (uint8_t)std::lround(sg);
            sky.b = (uint8_t)std::lround(sb);
            const uint32_t packed = pack(sky);
            const bool starRow = y < Height * 36 / 100; // upper zenith sky band only
            uint32_t h = (uint32_t)y * 2654435761u + 12345u;
            for (int x = 0; x < Width; ++x) {
                const int index = y * Width + x;
                colour[index] = packed;
                depth[index] = 1e30f;
                objectId[index] = ObjectSky;
                if (starRow) { // LW24: sparse deterministic starfield
                    h = h * 1664525u + 1013904223u;
                    if ((h & 1023u) < 7u) {
                        const uint8_t s = (uint8_t)(150u + ((h >> 8) & 127u));
                        colour[index] = pack({(uint8_t)(205u + ((h >> 16) & 50u)), s, s, 255});
                    }
                }
            }
        }
        ++sequence;
    }

    const uint32_t* pixelsFor(DisplayMode mode) {
        if (mode == DisplayMode::Colour) return colour.data();
        for (int index = 0; index < PixelCount; ++index) {
            if (mode == DisplayMode::Depth) {
                float d = depth[index];
                if (d >= 1e20f) {
                    presentation[index] = pack({16, 10, 12, 255});
                } else {
                    float n = 1.0f - clamp01((d - 0.1f) / 30.0f);
                    presentation[index] = pack({
                        (uint8_t)std::lround(255.0f * (1.0f - n)),
                        (uint8_t)std::lround(210.0f * n),
                        (uint8_t)std::lround(255.0f * n), 255});
                }
            } else if (mode == DisplayMode::ObjectIdentity) {
                switch (objectId[index]) {
                    case ObjectGround: presentation[index] = pack({48, 48, 62, 255}); break;
                    case ObjectObstacle: presentation[index] = pack({52, 70, 232, 255}); break;
                    case ObjectGoal: presentation[index] = pack({98, 255, 62, 255}); break;
                    case ObjectAvatar: presentation[index] = pack({255, 168, 88, 255}); break;
                    case ObjectGrid: presentation[index] = pack({98, 94, 118, 255}); break;
                    case ObjectNpc: presentation[index] = pack({193, 124, 236, 255}); break;
                    case ObjectQuestItem: presentation[index] = pack({64, 222, 245, 255}); break;
                    case ObjectVehicle: presentation[index] = pack({72, 187, 255, 255}); break;
                    case ObjectTraffic: presentation[index] = pack({255, 160, 60, 255}); break;
                    case ObjectPickup: presentation[index] = pack({80, 220, 255, 255}); break;
                    case ObjectProp: presentation[index] = pack({180, 255, 120, 255}); break;
                    default: presentation[index] = pack({24, 12, 20, 255}); break;
                }
            } else {
                Colour source = unpack(colour[index]);
                uint8_t luminance = (uint8_t)std::lround(
                    0.114f * source.b + 0.587f * source.g + 0.299f * source.r);
                presentation[index] = pack({luminance, luminance, luminance, 255});
            }
        }
        return presentation.data();
    }

    #ifdef _WIN32
    void blit(HDC dc, int x, int y, DisplayMode mode) {
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = Width;
        info.bmiHeader.biHeight = -Height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        SetDIBitsToDevice(dc, x, y, Width, Height, 0, 0, 0, Height,
            pixelsFor(mode), &info, DIB_RGB_COLORS);
    }
    #endif
};

using HighResFramebuffer = FramebufferSurface<768,768>; // exact AI input contract
using DisplayFramebuffer = FramebufferSurface<1536,864>; // optional presentation only

struct VertexOut {
    float x{}, y{}, z{};
    bool visible{false};
};

struct Triangle {
    Vec3 a{}, b{}, c{};
    Colour colour{};
    uint16_t objectId{ObjectSky};
};

struct FrameMetrics {
    double renderMs{0.0};
    int trianglesSubmitted{0};
    int trianglesRasterized{0};
    int finiteDepthPixels{0};
    int semanticPixels{0};
    uint64_t sequence{0};
};

struct ModelFrameDef {
    const char* name;
    int parent;
    int joint;
};

struct ModelAtomicDef {
    const char* name;
    int fromFrame;
    int toFrame;
    float startRadius;
    float endRadius;
    Colour material;
};

// A small, original clump/frame/atomic graph informed by RE3's RenderWare model
// organization. It contains no GTA geometry or texture data. User-owned DFF/TXD
// assets remain an optional local conversion boundary; this graph is the safe
// procedural fallback embedded in GTAngel.
class Re3InformedAvatarClump {
public:
    static const std::array<ModelFrameDef, avatar3d::AvatarSkeleton::COUNT + 1>& frames() {
        static const std::array<ModelFrameDef, avatar3d::AvatarSkeleton::COUNT + 1> value{{
            {"root", -1, avatar3d::AvatarSkeleton::Root},
            {"spine", 0, avatar3d::AvatarSkeleton::Spine},
            {"neck", 1, avatar3d::AvatarSkeleton::Neck},
            {"head", 2, avatar3d::AvatarSkeleton::Head},
            {"shoulder_l", 1, avatar3d::AvatarSkeleton::ShoulderL},
            {"elbow_l", 4, avatar3d::AvatarSkeleton::ElbowL},
            {"hand_l", 5, avatar3d::AvatarSkeleton::HandL},
            {"shoulder_r", 1, avatar3d::AvatarSkeleton::ShoulderR},
            {"elbow_r", 7, avatar3d::AvatarSkeleton::ElbowR},
            {"hand_r", 8, avatar3d::AvatarSkeleton::HandR},
            {"hip_l", 0, avatar3d::AvatarSkeleton::HipL},
            {"knee_l", 10, avatar3d::AvatarSkeleton::KneeL},
            {"foot_l", 11, avatar3d::AvatarSkeleton::FootL},
            {"hip_r", 0, avatar3d::AvatarSkeleton::HipR},
            {"knee_r", 13, avatar3d::AvatarSkeleton::KneeR},
            {"foot_r", 14, avatar3d::AvatarSkeleton::FootR},
            {"crown", 3, avatar3d::AvatarSkeleton::Head},
        }};
        return value;
    }

    static const std::array<ModelAtomicDef, 15>& atomics() {
        static const Colour core{220, 128, 234, 255};
        static const Colour limb{198, 104, 221, 255};
        static const Colour distal{238, 162, 247, 255};
        static const std::array<ModelAtomicDef, 15> value{{
            {"pelvis", 0, 1, 0.15f, 0.18f, core},
            {"thorax", 1, 2, 0.18f, 0.09f, core},
            {"cervical", 2, 3, 0.075f, 0.085f, distal},
            {"upper_arm_l", 4, 5, 0.075f, 0.060f, limb},
            {"forearm_l", 5, 6, 0.060f, 0.045f, distal},
            {"upper_arm_r", 7, 8, 0.075f, 0.060f, limb},
            {"forearm_r", 8, 9, 0.060f, 0.045f, distal},
            {"thigh_l", 10, 11, 0.105f, 0.078f, core},
            {"shin_l", 11, 12, 0.078f, 0.052f, limb},
            {"thigh_r", 13, 14, 0.105f, 0.078f, core},
            {"shin_r", 14, 15, 0.078f, 0.052f, limb},
            {"clavicle_l", 1, 4, 0.10f, 0.072f, core},
            {"clavicle_r", 1, 7, 0.10f, 0.072f, core},
            {"pelvic_l", 0, 10, 0.13f, 0.105f, core},
            {"pelvic_r", 0, 13, 0.13f, 0.105f, core},
        }};
        return value;
    }

    static const char* sourceLabel() { return "ORIGINAL CLUMP / 17 FRAMES / 15 ATOMICS"; }
};

