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
#ifndef GTANGEL_V18_PEER_RELATIONAL_CPP
#define GTANGEL_V18_PEER_RELATIONAL_CPP

// GTAngel v19 peer-relational doom loop.
// Network relations are provisional attention cues only. They never enter the
// visuomotor tensor, NavTrainer state, depth/object buffers, or actuator gate.
namespace v18peer {

enum class PredictionState { Unconfirmed, Confirmed, Contradicted };

struct PeerPrediction {
    std::string senderId;
    std::string recipientId;
    std::string rangeBand{"unknown"};
    std::string intent{"observe"};
    std::string eventClass{"presence"};
    std::string traceId;
    std::string evidenceDigest;
    uint64_t sequence{0};
    uint64_t sourceFrameSequence{0};
    int relativeBearing{0};
    float confidence{0.0f};
    int64_t sentAtMs{0};
    int64_t expiresAtMs{0};
    PredictionState state{PredictionState::Unconfirmed};
};

struct RelayTelemetry {
    bool configured{false};
    bool enabled{false};
    bool busy{false};
    unsigned long accepted{0};
    unsigned long received{0};
    unsigned long rejected{0};
    unsigned long expired{0};
    unsigned long confirmed{0};
    unsigned long contradicted{0};
    unsigned long queued{0};
    unsigned long lastStatus{0};
    double lastLatencyMs{0.0};
    std::string lastError;
};

struct AttentionContext {
    std::array<float, 16> sectorSalience{};
    int dominantSector{-1};
    float dominantSalience{0.0f};
    float meanConfidence{0.0f};
    unsigned long activeRelations{0};
    unsigned long confirmedRelations{0};
    uint64_t generation{0};
};

static std::string jsonString(const std::string& value) {
    return std::string("\"") + civicangel::jsonEscape(value) + "\"";
}

static bool extractString(const std::string& json, const char* key, std::string& output) {
    const std::string marker = std::string("\"") + key + "\":";
    size_t pos = json.find(marker);
    if (pos == std::string::npos) return false;
    pos += marker.size();
    while (pos < json.size() && isspace((unsigned char)json[pos])) ++pos;
    if (pos >= json.size() || json[pos] != '"') return false;
    ++pos;
    output.clear();
    bool escaped = false;
    for (; pos < json.size(); ++pos) {
        const char c = json[pos];
        if (escaped) { output.push_back(c); escaped = false; continue; }
        if (c == '\\') { escaped = true; continue; }
        if (c == '"') return true;
        output.push_back(c);
    }
    return false;
}

static bool extractNumber(const std::string& json, const char* key, double& output) {
    const std::string marker = std::string("\"") + key + "\":";
    size_t pos = json.find(marker);
    if (pos == std::string::npos) return false;
    pos += marker.size();
    while (pos < json.size() && isspace((unsigned char)json[pos])) ++pos;
    char* end = nullptr;
    output = strtod(json.c_str() + pos, &end);
    return end && end != json.c_str() + pos && std::isfinite(output);
}

static std::vector<std::string> pingObjects(const std::string& response) {
    std::vector<std::string> objects;
    const size_t key = response.find("\"pings\"");
    if (key == std::string::npos) return objects;
    size_t array = response.find('[', key);
    if (array == std::string::npos) return objects;
    int depth = 0;
    bool quoted = false, escaped = false;
    size_t start = std::string::npos;
    for (size_t i = array + 1; i < response.size(); ++i) {
        const char c = response[i];
        if (quoted) {
            if (escaped) escaped = false;
            else if (c == '\\') escaped = true;
            else if (c == '"') quoted = false;
            continue;
        }
        if (c == '"') { quoted = true; continue; }
        if (c == '{') {
            if (depth++ == 0) start = i;
        } else if (c == '}') {
            if (--depth == 0 && start != std::string::npos) {
                objects.push_back(response.substr(start, i - start + 1));
                start = std::string::npos;
            }
        } else if (c == ']' && depth == 0) break;
    }
    return objects;
}

class PeerRelationalLoop {
public:
    PeerRelationalLoop() : worker_([this] { workerLoop(); }) {}
    ~PeerRelationalLoop() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    PeerRelationalLoop(const PeerRelationalLoop&) = delete;
    PeerRelationalLoop& operator=(const PeerRelationalLoop&) = delete;

    void configureFromEnvironment() {
        std::string base = civicangel::environmentValue("ARC_ANGEL_INGEST_URL");
        std::string token = civicangel::environmentValue("ARC_ANGEL_GTANGEL_TOKEN");
        std::string self = civicangel::environmentValue("GTANGEL_PEER_ID");
        if (self.empty()) self = "gtangel-v18";
        const std::string telemetryPath = "/api/telemetry/ingest";
        const size_t path = base.find(telemetryPath);
        if (path != std::string::npos) base.resize(path);
        while (!base.empty() && base.back() == '/') base.pop_back();
        std::lock_guard<std::mutex> lock(mu_);
        baseUrl_ = std::move(base);
        token_ = std::move(token);
        selfId_ = std::move(self);
        configured_ = !baseUrl_.empty() && !token_.empty() && !selfId_.empty();
        if (!configured_) lastError_ = "Peer relay requires ARC_ANGEL_INGEST_URL, ARC_ANGEL_GTANGEL_TOKEN, and optional GTANGEL_PEER_ID";
    }

    void setEnabled(bool enabled) {
        {
            std::lock_guard<std::mutex> lock(mu_);
            enabled_ = enabled;
        }
        cv_.notify_all();
    }
    bool enabled() const { std::lock_guard<std::mutex> lock(mu_); return enabled_; }

    // Called only when GTAngel has locally rendered evidence of another agent.
    // This method has no world-coordinate parameters by construction.
    bool publishObservedPeer(const std::string& recipientId, int bearingBucket,
                             const std::string& rangeBand, float confidence,
                             const std::string& intent, const std::string& eventClass,
                             uint64_t sourceFrameSequence, const std::string& evidenceDigest) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!configured_ || !enabled_) return false;
        if (outgoing_.size() >= 4) { ++rejected_; lastError_ = "Peer output queue full"; return false; }
        const int64_t now = civicangel::unixTimeMs();
        const uint64_t sequence = ++outgoingSequence_;
        char trace[64];
        snprintf(trace, sizeof(trace), "peer-%llx-%llx", (unsigned long long)sourceFrameSequence, (unsigned long long)now);
        std::ostringstream json;
        json << "{\"protocolVersion\":1,\"senderId\":" << jsonString(selfId_);
        if (!recipientId.empty()) json << ",\"recipientId\":" << jsonString(recipientId);
        json << ",\"sequence\":" << sequence
             << ",\"sentAtMs\":" << now
             << ",\"ttlMs\":3000"
             << ",\"relativeBearing\":" << std::max(0, std::min(15, bearingBucket))
             << ",\"rangeBand\":" << jsonString(rangeBand)
             << ",\"confidence\":" << std::fixed << std::setprecision(3) << std::max(0.0f, std::min(1.0f, confidence))
             << ",\"intent\":" << jsonString(intent)
             << ",\"eventClass\":" << jsonString(eventClass)
             << ",\"traceId\":" << jsonString(trace)
             << ",\"sourceFrameSequence\":" << sourceFrameSequence
             << ",\"evidenceDigest\":" << jsonString(evidenceDigest) << "}";
        const std::string body = json.str();
        if (body.size() > 512) { ++rejected_; lastError_ = "Peer payload exceeded 512 bytes"; return false; }
        outgoing_.push_back(body);
        cv_.notify_all();
        return true;
    }

    void confirmAgainstLocalFrame(const v15render::HighResFramebuffer& frame, bool firstPerson) {
        const int64_t now = civicangel::unixTimeMs();
        std::lock_guard<std::mutex> lock(mu_);
        for (auto it = predictions_.begin(); it != predictions_.end();) {
            if (it->expiresAtMs <= now) { ++expired_; it = predictions_.erase(it); continue; }
            it->state = PredictionState::Unconfirmed;
            if (firstPerson) confirmPrediction(*it, frame);
            ++it;
        }
        recalculateStates();
    }

    std::vector<PeerPrediction> predictions() const {
        std::lock_guard<std::mutex> lock(mu_);
        return predictions_;
    }

    // Explicit advisory context. It is stored independently from every policy,
    // navigation, framebuffer, and actuation structure.
    AttentionContext attentionContext() const {
        std::lock_guard<std::mutex> lock(mu_);
        AttentionContext context;
        context.generation = attentionGeneration_;
        float confidenceSum = 0.0f;
        for (const auto& prediction : predictions_) {
            const float stateWeight = prediction.state == PredictionState::Confirmed ? 1.0f
                : prediction.state == PredictionState::Contradicted ? 0.10f : 0.50f;
            const float salience = std::max(0.0f, std::min(1.0f, prediction.confidence)) * stateWeight;
            context.sectorSalience[(size_t)prediction.relativeBearing] = std::max(
                context.sectorSalience[(size_t)prediction.relativeBearing], salience);
            confidenceSum += prediction.confidence;
            ++context.activeRelations;
            if (prediction.state == PredictionState::Confirmed) ++context.confirmedRelations;
        }
        for (int sector = 0; sector < 16; ++sector) {
            if (context.sectorSalience[(size_t)sector] > context.dominantSalience) {
                context.dominantSalience = context.sectorSalience[(size_t)sector];
                context.dominantSector = sector;
            }
        }
        context.meanConfidence = context.activeRelations ? confidenceSum / context.activeRelations : 0.0f;
        return context;
    }

    RelayTelemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        RelayTelemetry t;
        t.configured = configured_; t.enabled = enabled_; t.busy = busy_;
        t.accepted = accepted_; t.received = received_; t.rejected = rejected_;
        t.expired = expired_; t.confirmed = confirmed_; t.contradicted = contradicted_;
        t.queued = (unsigned long)outgoing_.size(); t.lastStatus = lastStatus_;
        t.lastLatencyMs = lastLatencyMs_; t.lastError = lastError_;
        return t;
    }

    void drawAttentionMarkers(HDC hdc, int originX, int originY, int size) const {
        const auto local = predictions();
        const int cx = originX + size / 2, cy = originY + size / 2;
        const int radius = size / 2 - 24;
        for (const auto& prediction : local) {
            const double angle = prediction.relativeBearing * (2.0 * 3.141592653589793 / 16.0) - 3.141592653589793 / 2.0;
            const int x = cx + (int)std::lround(cos(angle) * radius);
            const int y = cy + (int)std::lround(sin(angle) * radius);
            COLORREF colour = prediction.state == PredictionState::Confirmed ? RGB(90, 240, 130)
                : prediction.state == PredictionState::Contradicted ? RGB(255, 80, 105) : RGB(70, 220, 255);
            HPEN pen = CreatePen(PS_SOLID, 2, colour);
            HBRUSH brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
            Ellipse(hdc, x - 6, y - 6, x + 7, y + 7);
            MoveToEx(hdc, x, y, nullptr); LineTo(hdc, cx, cy);
            SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen); DeleteObject(pen);
        }
    }

private:
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::thread worker_;
    bool stopping_{false}, configured_{false}, enabled_{false}, busy_{false};
    std::string baseUrl_, token_, selfId_, lastError_;
    std::deque<std::string> outgoing_;
    std::vector<PeerPrediction> predictions_;
    uint64_t outgoingSequence_{0};
    uint64_t attentionGeneration_{0};
    int64_t lastPollReceivedAtMs_{0};
    unsigned long accepted_{0}, received_{0}, rejected_{0}, expired_{0}, confirmed_{0}, contradicted_{0}, lastStatus_{0};
    double lastLatencyMs_{0.0};

    static bool depthMatches(const std::string& band, float depth) {
        if (!std::isfinite(depth)) return false;
        if (band == "contact") return depth < 1.5f;
        if (band == "near") return depth >= 1.0f && depth < 4.0f;
        if (band == "mid") return depth >= 3.0f && depth < 8.0f;
        if (band == "far") return depth >= 7.0f;
        return true;
    }

    static void confirmPrediction(PeerPrediction& prediction, const v15render::HighResFramebuffer& frame) {
        int signedBucket = prediction.relativeBearing;
        if (signedBucket > 8) signedBucket -= 16;
        const float bearingDeg = signedBucket * 22.5f;
        if (fabsf(bearingDeg) > 52.0f) return;
        const int centreX = std::max(0, std::min(v15render::HighResFramebuffer::Width - 1,
            (int)std::lround((0.5f + bearingDeg / 104.0f) * v15render::HighResFramebuffer::Width)));
        const int halfWidth = 36;
        bool avatarSeen = false, depthMatched = false;
        for (int y = 64; y < v15render::HighResFramebuffer::Height - 32; y += 4) {
            for (int x = std::max(0, centreX - halfWidth); x <= std::min(v15render::HighResFramebuffer::Width - 1, centreX + halfWidth); x += 2) {
                const size_t index = (size_t)y * v15render::HighResFramebuffer::Width + x;
                if (frame.objectId[index] != v15render::ObjectAvatar) continue;
                avatarSeen = true;
                if (depthMatches(prediction.rangeBand, frame.depth[index])) depthMatched = true;
            }
        }
        if (depthMatched) prediction.state = PredictionState::Confirmed;
        else if (avatarSeen) prediction.state = PredictionState::Contradicted;
    }

    void recalculateStates() {
        confirmed_ = contradicted_ = 0;
        for (const auto& prediction : predictions_) {
            if (prediction.state == PredictionState::Confirmed) ++confirmed_;
            if (prediction.state == PredictionState::Contradicted) ++contradicted_;
        }
        ++attentionGeneration_;
    }

    static bool parsePrediction(const std::string& json, const std::string& selfId, PeerPrediction& prediction) {
        double value = 0.0;
        if (!extractString(json, "senderId", prediction.senderId)) return false;
        extractString(json, "recipientId", prediction.recipientId);
        if (!extractNumber(json, "sequence", value)) return false; prediction.sequence = (uint64_t)value;
        if (!extractNumber(json, "sentAtMs", value)) return false; prediction.sentAtMs = (int64_t)value;
        if (!extractNumber(json, "expiresAtMs", value)) return false; prediction.expiresAtMs = (int64_t)value;
        if (!extractNumber(json, "relativeBearing", value)) return false; prediction.relativeBearing = (int)value;
        if (!extractNumber(json, "confidence", value)) return false; prediction.confidence = (float)value;
        if (!extractNumber(json, "sourceFrameSequence", value)) return false; prediction.sourceFrameSequence = (uint64_t)value;
        if (!extractString(json, "rangeBand", prediction.rangeBand)) return false;
        if (!extractString(json, "intent", prediction.intent)) return false;
        if (!extractString(json, "eventClass", prediction.eventClass)) return false;
        if (!extractString(json, "traceId", prediction.traceId)) return false;
        if (!extractString(json, "evidenceDigest", prediction.evidenceDigest)) return false;
        if (!prediction.recipientId.empty() && prediction.recipientId == selfId)
            prediction.relativeBearing = (prediction.relativeBearing + 8) % 16;
        return prediction.relativeBearing >= 0 && prediction.relativeBearing < 16 && prediction.senderId != selfId;
    }

    static bool httpRequest(const wchar_t* method, const std::string& url, const std::string& token,
                            const std::string& body, unsigned long& status, std::string& response,
                            std::string& error, double& latencyMs) {
        const auto start = std::chrono::steady_clock::now();
        std::wstring wurl = civicangel::wide(url);
        URL_COMPONENTS parts{}; parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength = parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) { error = "WinHttpCrackUrl failed"; return false; }
        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";
        HINTERNET session = WinHttpOpen(L"GTAngel-PeerLoop/18.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { error = "WinHttpOpen failed"; return false; }
        WinHttpSetTimeouts(session, 3000, 3000, 5000, 5000);
        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        const DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET request = connection ? WinHttpOpenRequest(connection, method, path.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags) : nullptr;
        if (!connection || !request) error = "WinHTTP connection failed";
        if (request) {
            const std::wstring headers = L"Accept: application/json\r\nAuthorization: Bearer " + civicangel::wide(token) +
                (body.empty() ? L"\r\n" : L"\r\nContent-Type: application/json\r\n");
            BOOL sent = WinHttpSendRequest(request, headers.c_str(), (DWORD)-1L,
                body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
            if (!sent || !WinHttpReceiveResponse(request, nullptr)) error = "WinHTTP request failed";
            if (error.empty()) {
                DWORD size = sizeof(status);
                WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                    WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
                for (;;) {
                    DWORD available = 0;
                    if (!WinHttpQueryDataAvailable(request, &available) || !available) break;
                    std::string chunk(available, '\0'); DWORD read = 0;
                    if (!WinHttpReadData(request, chunk.data(), available, &read)) break;
                    chunk.resize(read); response += chunk;
                    if (response.size() > 32768) { error = "Peer response exceeded 32 KiB"; break; }
                }
                if (status < 200 || status >= 300) error = "HTTP " + std::to_string(status);
            }
        }
        if (request) WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        latencyMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        return error.empty() && status >= 200 && status < 300;
    }

    void absorbResponse(const std::string& response) {
        const int64_t now = civicangel::unixTimeMs();
        const auto objects = pingObjects(response);
        std::lock_guard<std::mutex> lock(mu_);
        for (const auto& object : objects) {
            PeerPrediction prediction;
            if (!parsePrediction(object, selfId_, prediction) || prediction.expiresAtMs <= now) { ++rejected_; continue; }
            auto existing = std::find_if(predictions_.begin(), predictions_.end(), [&](const PeerPrediction& candidate) {
                return candidate.senderId == prediction.senderId && candidate.sequence == prediction.sequence;
            });
            if (existing != predictions_.end()) continue;
            predictions_.push_back(std::move(prediction));
            if (predictions_.size() > 32) predictions_.erase(predictions_.begin());
            ++received_;
        }
    }

    void workerLoop() {
        int64_t nextPollMs = 0;
        for (;;) {
            std::string body, base, token;
            bool enabled = false, configured = false;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait_for(lock, std::chrono::milliseconds(100), [this] { return stopping_ || !outgoing_.empty(); });
                if (stopping_) return;
                enabled = enabled_; configured = configured_; base = baseUrl_; token = token_;
                if (enabled && configured && !outgoing_.empty()) { body = std::move(outgoing_.front()); outgoing_.pop_front(); }
            }
            if (!enabled || !configured) continue;
            const int64_t now = civicangel::unixTimeMs();
            if (!body.empty()) {
                unsigned long status = 0; std::string response, error; double latency = 0.0;
                { std::lock_guard<std::mutex> lock(mu_); busy_ = true; }
                const bool ok = httpRequest(L"POST", base + "/api/peer-pings/ingest", token, body, status, response, error, latency);
                std::lock_guard<std::mutex> lock(mu_);
                busy_ = false; lastStatus_ = status; lastLatencyMs_ = latency;
                if (ok) { ++accepted_; lastError_.clear(); } else { ++rejected_; lastError_ = error; }
            }
            if (now >= nextPollMs) {
                unsigned long status = 0; std::string response, error; double latency = 0.0;
                std::string url = base + "/api/peer-pings/poll?sinceMs=" + std::to_string(std::max<int64_t>(0, lastPollReceivedAtMs_)) + "&limit=32";
                { std::lock_guard<std::mutex> lock(mu_); busy_ = true; }
                const bool ok = httpRequest(L"GET", url, token, "", status, response, error, latency);
                {
                    std::lock_guard<std::mutex> lock(mu_);
                    busy_ = false; lastStatus_ = status; lastLatencyMs_ = latency;
                    if (!ok) { ++rejected_; lastError_ = error; }
                    else { lastError_.clear(); lastPollReceivedAtMs_ = now; }
                }
                if (ok) absorbResponse(response);
                nextPollMs = now + 500;
            }
        }
    }
};

} // namespace v18peer
#endif
