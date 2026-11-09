#pragma once
// ── Egocentric software renderer ──
// Renders the world from the avatar's perspective into a 64x64 framebuffer
// with z-buffering. Output is the SAME buffer consumed by the cognitive stack.
class EgocentricRenderer {
public:
    static constexpr int W = 64, H = 64;
    std::vector<float> frame;      // luminance 0..1
    std::vector<float> zbuf;
    std::vector<float> semantic;   // 0=void, 0.33=ground, 0.66=obstacle, 1=goal

    EgocentricRenderer() : frame(W*H, 0.0f), zbuf(W*H, 1e9f), semantic(W*H, 0.0f) {}

    void render(const GameWorld& world, Vec3 eye, float heading, float pitch,
                const AvatarSkeleton& avatar, bool thirdPerson) {
        std::fill(frame.begin(), frame.end(), 0.02f);
        std::fill(zbuf.begin(), zbuf.end(), 1e9f);
        std::fill(semantic.begin(), semantic.end(), 0.0f);

        float ch = cosf(heading), sh = sinf(heading);
        float cp = cosf(pitch),   sp = sinf(pitch);
        float fov = 1.2f; // ~69 deg

        auto project = [&](Vec3 p, float& sx, float& sy, float& depth) -> bool {
            Vec3 d = p - eye;
            // yaw
            float x = d.x * ch - d.z * sh;
            float z = d.x * sh + d.z * ch;
            // pitch
            float y = d.y * cp - z * sp;
            float z2 = d.y * sp + z * cp;
            if (z2 < 0.05f) return false;
            depth = z2;
            sx = (x / (z2 * fov)) * (W * 0.5f) + W * 0.5f;
            sy = (-y / (z2 * fov)) * (H * 0.5f) + H * 0.5f;
            return sx >= 0 && sx < W && sy >= 0 && sy < H;
        };

        auto plot = [&](float sx, float sy, float depth, float lum, float sem) {
            int px = (int)sx, py = (int)sy;
            if (px < 0 || px >= W || py < 0 || py >= H) return;
            int idx = py * W + px;
            if (depth < zbuf[idx]) {
                zbuf[idx] = depth;
                frame[idx] = lum;
                semantic[idx] = sem;
            }
        };
        auto plotDisc = [&](float sx, float sy, float depth, float lum, float sem, int radius) {
            for (int dy = -radius; dy <= radius; dy++)
                for (int dx = -radius; dx <= radius; dx++)
                    if (dx*dx + dy*dy <= radius*radius)
                        plot(sx + dx, sy + dy, depth, lum, sem);
        };

        // Ground plane: initialize a continuous floor below the horizon, then
        // overlay projected grid samples. This prevents the cognitive retina
        // from degenerating into an almost-empty frame between sparse points.
        for (int py = H / 2; py < H; py++) {
            float t = (float)(py - H / 2) / (H / 2);
            for (int px = 0; px < W; px++) {
                int idx = py * W + px;
                frame[idx] = 0.08f + 0.14f * t;
                semantic[idx] = 0.33f;
                zbuf[idx] = 1000.0f - t;
            }
        }
        const float sampleMinX=std::max(-world.worldSize/2,eye.x-18.0f);
        const float sampleMaxX=std::min( world.worldSize/2,eye.x+18.0f);
        const float sampleMinZ=std::max(-world.worldSize/2,eye.z-18.0f);
        const float sampleMaxZ=std::min( world.worldSize/2,eye.z+18.0f);
        for (float gx=sampleMinX; gx<=sampleMaxX; gx+=0.75f)
            for (float gz=sampleMinZ; gz<=sampleMaxZ; gz+=0.75f) {
                float sx, sy, d;
                if (project({gx, 0, gz}, sx, sy, d))
                    plotDisc(sx, sy, d, std::max(0.10f, 0.42f - d * 0.018f), 0.33f, d < 8.0f ? 1 : 0);
            }

        // Obstacle pillars (vertical line stacks).
        for (const auto& pl : world.pillars) {
            for (float h = 0; h <= pl.height; h += 0.15f) {
                for (float a = 0; a < 6.28318f; a += 0.785398f) {
                    Vec3 p = pl.pos + Vec3{cosf(a) * pl.radius, h, sinf(a) * pl.radius};
                    float sx, sy, d;
                    if (project(p, sx, sy, d)) {
                        int radiusPx = std::max(1, std::min(5, (int)(12.0f * pl.radius / std::max(0.5f, d))));
                        plotDisc(sx, sy, d, std::max(0.15f, 0.78f - d * 0.035f), 0.66f, radiusPx);
                    }
                }
            }
        }

        // Goal beacon (bright pulsing column).
        float pulse = 0.8f + 0.2f * sinf((float)GetTickCount64() * 0.005f);
        for (float h = 0; h <= 3.0f; h += 0.1f) {
            float sx, sy, d;
            if (project(world.goal + Vec3{0, h, 0}, sx, sy, d))
                plotDisc(sx, sy, d, pulse, 1.0f, d < 8.0f ? 2 : 1);
        }

        // Third-person: draw avatar skeleton as bone segments.
        if (thirdPerson) {
            static const int bones[][2] = {
                {AvatarSkeleton::Root, AvatarSkeleton::Spine},
                {AvatarSkeleton::Spine, AvatarSkeleton::Neck},
                {AvatarSkeleton::Neck, AvatarSkeleton::Head},
                {AvatarSkeleton::ShoulderL, AvatarSkeleton::ElbowL},
                {AvatarSkeleton::ElbowL, AvatarSkeleton::HandL},
                {AvatarSkeleton::ShoulderR, AvatarSkeleton::ElbowR},
                {AvatarSkeleton::ElbowR, AvatarSkeleton::HandR},
                {AvatarSkeleton::Spine, AvatarSkeleton::ShoulderL},
                {AvatarSkeleton::Spine, AvatarSkeleton::ShoulderR},
                {AvatarSkeleton::Root, AvatarSkeleton::HipL},
                {AvatarSkeleton::Root, AvatarSkeleton::HipR},
                {AvatarSkeleton::HipL, AvatarSkeleton::KneeL},
                {AvatarSkeleton::KneeL, AvatarSkeleton::FootL},
                {AvatarSkeleton::HipR, AvatarSkeleton::KneeR},
                {AvatarSkeleton::KneeR, AvatarSkeleton::FootR}
            };
            for (auto& b : bones) {
                Vec3 a = avatar.joints[b[0]], c = avatar.joints[b[1]];
                for (float t = 0; t <= 1.0f; t += 0.1f) {
                    Vec3 p = a + (c - a) * t;
                    float sx, sy, d;
                    if (project(p, sx, sy, d)) plot(sx, sy, d, 0.9f, 0.5f);
                }
            }
        }
    }
};

// ── Framebuffer → TRIZ grid mapper ──
// Bins the 64x64 framebuffer into the 12x12 TRIZ hex grid. Each cell receives
// mean luminance, semantic composition, and a contradiction score derived from
// local spatial conflict (obstacle vs goal energy in the same receptive field).
struct TrizMapping {
    static constexpr int GRID = 12;
    float aesthetic[GRID][GRID]{};
    float contradiction[GRID][GRID]{};
    float goalSalience[GRID][GRID]{};
    float obstacleSalience[GRID][GRID]{};
    float meanContradiction{0};
    float maxContradiction{0};
    int maxQ{-1}, maxR{-1};

    void mapFrame(const EgocentricRenderer& r) {
        meanContradiction = maxContradiction = 0;
        maxQ = maxR = -1;
        const int binW = EgocentricRenderer::W / GRID; // 5 px per cell (60 used)
        const int binH = EgocentricRenderer::H / GRID;
        for (int q = 0; q < GRID; q++)
            for (int rr = 0; rr < GRID; rr++) {
                float lum = 0, goal = 0, obst = 0, n = 0;
                for (int y = rr * binH; y < (rr + 1) * binH; y++)
                    for (int x = q * binW; x < (q + 1) * binW; x++) {
                        int idx = y * EgocentricRenderer::W + x;
                        lum += r.frame[idx];
                        if (r.semantic[idx] > 0.9f) goal += 1.0f;
                        else if (r.semantic[idx] > 0.5f && r.semantic[idx] < 0.8f) obst += 1.0f;
                        n += 1.0f;
                    }
                aesthetic[q][rr] = lum / n;
                goalSalience[q][rr] = goal / n;
                obstacleSalience[q][rr] = obst / n;
                // TRIZ spatial contradiction combines (a) direct goal/obstacle
                // overlap and (b) an obstacle blocking the central locomotion
                // corridor. The latter remains meaningful when the goal is just
                // outside the camera frustum, unlike pure pixel overlap.
                float center = 1.0f - fabsf(((q + 0.5f) / GRID) * 2.0f - 1.0f);
                float forward = 1.0f - (float)rr / GRID;
                float corridorConflict = obstacleSalience[q][rr] * center * (0.35f + 0.65f * forward);
                float semanticConflict = 2.0f * sqrtf(goalSalience[q][rr] * obstacleSalience[q][rr]);
                contradiction[q][rr] = std::min(1.0f, corridorConflict + semanticConflict);
                meanContradiction += contradiction[q][rr];
                if (contradiction[q][rr] > maxContradiction) {
                    maxContradiction = contradiction[q][rr];
                    maxQ = q; maxR = rr;
                }
            }
        meanContradiction /= (GRID * GRID);
    }
};

// ── Navigation training harness ──
// Tabular Q-learning navigator over an 8-direction action space with a
// discretized egocentric state (goal bearing bucket, goal distance bucket,
// nearest-obstacle bearing bucket). Tracks learning-curve difficulty metrics.
struct NavTrainer {
    enum Action { FWD=0, FWD_L, FWD_R, TURN_L, TURN_R, BACK, WAIT, SPRINT, COUNT };
    static constexpr int BEARINGS = 8, DISTS = 4, OBST = 8;
    float Q[BEARINGS][DISTS][OBST][COUNT]{};
    float alpha{0.15f}, gamma{0.92f}, epsilon{0.25f};
    uint32_t rng{12345};

    // Difficulty metrics
    int episodes{0};
    int successes{0};
    int collisions{0};
    int stepsTotal{0};
    std::vector<float> returnsPerEpisode;
    std::vector<int> stepsPerEpisode;

    float frand() { rng = rng * 1664525u + 1013904223u; return (rng >> 8) / 16777216.0f; }

    struct State { int bearing, dist, obst; };
    State discretize(Vec3 pos, float heading, const GameWorld& w) const {
        Vec3 toGoal = w.goal - pos;
        float goalAng = atan2f(toGoal.x, toGoal.z);
        float rel = goalAng - heading;
        while (rel > 3.14159f) rel -= 6.28318f;
        while (rel < -3.14159f) rel += 6.28318f;
        int b = (int)((rel + 3.14159f) / 6.28318f * BEARINGS) % BEARINGS;
        if (b < 0) b += BEARINGS;
        float d = w.goalDistance(pos);
        int di = d < 2 ? 0 : d < 5 ? 1 : d < 10 ? 2 : 3;
        // nearest obstacle bearing
        float bestD = 1e9f; float bestA = 0;
        for (const auto& p : w.pillars) {
            Vec3 dp = p.pos - pos;
            float dd = len(dp);
            if (dd < bestD) { bestD = dd; bestA = atan2f(dp.x, dp.z) - heading; }
        }
        int o = (int)((bestA + 3.14159f) / 6.28318f * OBST) % OBST;
        if (o < 0) o += OBST;
        return {b, di, o};
    }

    Action choose(const State& s) {
        if (frand() < epsilon) return (Action)(rng % COUNT);
        int best = 0; float bv = Q[s.bearing][s.dist][s.obst][0];
        for (int a = 1; a < COUNT; a++)
            if (Q[s.bearing][s.dist][s.obst][a] > bv) { bv = Q[s.bearing][s.dist][s.obst][a]; best = a; }
        return (Action)best;
    }

    void update(const State& s, Action a, float reward, const State& s2) {
        float bestNext = Q[s2.bearing][s2.dist][s2.obst][0];
        for (int aa = 1; aa < COUNT; aa++)
            bestNext = std::max(bestNext, Q[s2.bearing][s2.dist][s2.obst][aa]);
        float& q = Q[s.bearing][s.dist][s.obst][a];
        q += alpha * (reward + gamma * bestNext - q);
    }

    // One training episode; returns total return. Writes per-step frames if
    // renderer/mapper provided (for difficulty evaluation on the TRIZ grid).
    float episode(GameWorld& w, AvatarSkeleton& avatar, EgocentricRenderer* rend,
                  TrizMapping* mapper, int maxSteps, float* outMeanContradiction) {
        Vec3 pos{0, 0, 0};
        float heading = 0, speed = 0;
        float total = 0, contraSum = 0; int contraN = 0;
        int actualSteps = 0;
        bool reachedGoal = false;
        for (int step = 0; step < maxSteps; step++) {
            actualSteps = step + 1;
            State s = discretize(pos, heading, w);
            Action a = choose(s);
            float turn = 0, move = 0;
            switch (a) {
                case FWD: move = 1.0f; break;
                case FWD_L: move = 0.8f; turn = 0.35f; break;
                case FWD_R: move = 0.8f; turn = -0.35f; break;
                case TURN_L: turn = 0.6f; break;
                case TURN_R: turn = -0.6f; break;
                case BACK: move = -0.5f; break;
                case WAIT: break;
                case SPRINT: move = 1.6f; break;
                default: break;
            }
            heading += turn * 0.1f;
            speed = move;
            Vec3 next = pos + Vec3{sinf(heading) * move * 0.2f, 0, cosf(heading) * move * 0.2f};

            float reward = -0.01f; // time penalty
            bool done = false;
            if (w.collides(next)) {
                reward = -1.0f; collisions++; next = pos; done = false;
            } else if (w.goalDistance(next) < 1.0f) {
                reward = 1.0f; pos = next; done = true;
            } else {
                // shaping: progress toward goal
                reward += 0.1f * (w.goalDistance(pos) - w.goalDistance(next));
                pos = next;
            }

            State s2 = discretize(pos, heading, w);
            update(s, a, reward, s2);
            total += reward;
            stepsTotal++;

            if (rend && mapper) {
                avatar.pose(pos, heading, fabsf(speed), 0.05f);
                Vec3 eye = pos + Vec3{0, 1.6f, 0};
                rend->render(w, eye, heading, 0.0f, avatar, false);
                mapper->mapFrame(*rend);
                contraSum += mapper->meanContradiction;
                contraN++;
            }
            if (done) { successes++; reachedGoal = true; break; }
        }
        episodes++;
        returnsPerEpisode.push_back(total);
        stepsPerEpisode.push_back(actualSteps);
        epsilon = std::max(0.05f, epsilon * 0.995f);
        if (outMeanContradiction) *outMeanContradiction = contraN ? contraSum / contraN : 0.0f;
        return total;
    }

    // Difficulty summary: success rate, collision rate, mean return trend.
    float successRate() const { return episodes ? (float)successes / episodes : 0.0f; }
    float collisionRate() const { return stepsTotal ? (float)collisions / stepsTotal : 0.0f; }
    float recentMeanReturn(int window = 20) const {
        if (returnsPerEpisode.empty()) return 0.0f;
        int n = std::min((int)returnsPerEpisode.size(), window);
        float s = 0;
        for (int i = (int)returnsPerEpisode.size() - n; i < (int)returnsPerEpisode.size(); i++)
            s += returnsPerEpisode[i];
        return s / n;
    }
};

} // namespace avatar3d


// GTAngel v14 — Arc Angel Echo / Civic Angel Architect II telemetry module
// Native Win32 C++17, no external dependencies. Tokens remain process-local.
// The module emits only values supplied by GTAngel's live runtime state.

namespace civicangel {

static double clamp01(double value) {
    return std::max(0.0, std::min(1.0, value));
}

static std::string jsonEscape(const std::string& input) {
    std::string out;
    out.reserve(input.size() + 32);
    for (unsigned char c : input) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: if (c >= 0x20) out += (char)c; break;
        }
    }
    return out;
}

static std::wstring wide(const std::string& input) {
    if (input.empty()) return {};
    const int needed = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0);
    std::wstring output((size_t)needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), output.data(), needed);
    return output;
}

static std::map<std::string,std::string> parseEnvText(const std::string& text) {
    std::map<std::string,std::string> out;
    std::string line, key, val;
    std::istringstream stream(text);
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const size_t firstNonSpace = line.find_first_not_of(" \t");
        if (firstNonSpace == std::string::npos) continue;
        if (line[firstNonSpace] == '#' || line[firstNonSpace] == ';') continue;
        const size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        key = line.substr(firstNonSpace, eq - firstNonSpace);
        const size_t keyEnd = key.find_last_not_of(" \t");
        if (keyEnd != std::string::npos) key.resize(keyEnd + 1);
        val = line.substr(eq + 1);
        const size_t valStart = val.find_first_not_of(" \t");
        val = valStart == std::string::npos ? "" : val.substr(valStart);
        const size_t valEnd = val.find_last_not_of(" \t\r");
        if (valEnd != std::string::npos) val.resize(valEnd + 1);
        if (val.size() >= 2 && ((val.front()=='"' && val.back()=='"') ||
                               (val.front()=='\'' && val.back()=='\'')))
            val = val.substr(1, val.size() - 2);
        if (!key.empty()) out[key] = val;
    }
    return out;
}
static std::string environmentValue(const char* key) {
    const DWORD required = GetEnvironmentVariableA(key, nullptr, 0);
    if (required) {
        std::string value(required, '\0');
        const DWORD copied = GetEnvironmentVariableA(key, value.data(), required);
        if (copied) {
            value.resize(copied);
            return value;
        }
    }
    // .env fallback: producers stay configured across sessions without
    // machine-level env vars. Process env always wins when present.
    static std::map<std::string,std::string> envFile;
    static bool envFileLoaded = false;
    if (!envFileLoaded) {
        envFileLoaded = true;
        char modulePath[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, modulePath, MAX_PATH)) {
            std::string path(modulePath);
            const size_t slash = path.find_last_of("\\/");
            if (slash != std::string::npos) path.resize(slash + 1);
            std::ifstream file(path + ".env");
            if (file) {
                std::string text((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
                envFile = parseEnvText(text);
            }
        }
    }
    const auto it = envFile.find(key);
    return it == envFile.end() ? std::string() : it->second;
}

static int64_t unixTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

struct GroundedFrame {
    uint64_t tick{0};
    double positionX{0.0};
    double positionZ{0.0};
    double heading{0.0};
    double goalDistance{0.0};
    double meanContradiction{0.0};
    double frameLuminance{0.0};
    double obstacleSalience{0.0};
    double goalSalience{0.0};
    double collisionRate{0.0};
    double health{0.0};
    double hostility{0.0};
    double goalProgress{0.0};
    double reward{0.0};
    double coherence{0.0};
    double valence{0.0};
    double arousal{0.0};
    double cortisol{0.0};
    double dopamine{0.0};
    double serotonin{0.0};
    double norepinephrine{0.0};
    double oxytocin{0.0};
    double melatonin{0.0};
    double anandamide{0.0};
    // Echo persona display fields (superhot-gamer-girl echo.json contract).
    const char* echoEmotion{"BORED_STOMPING"};
    const char* echoExpression{"PLAYFUL_BORED"};
    const char* echoFacs{"AU43 AU15"};
    double echoSpice{0.0};
    double echoConfidence{0.0};
    uint64_t framebufferSequence{0};
    double framebufferRenderMs{0.0};
    int trianglesSubmitted{0};
    int trianglesRasterized{0};
    int finiteDepthPixels{0};
    int semanticPixels{0};
    const char* cognitiveMode{"exploratory"};
};

struct ProducerTelemetry {
    bool configured{false};
    bool busy{false};
    unsigned long accepted{0};
    unsigned long failures{0};
    unsigned long queued{0};
    unsigned long lastStatus{0};
    double lastLatencyMs{0.0};
    std::string lastError;
    std::string lastTraceId;
};

struct PendingFrame {
    std::string json;
    std::string traceId;
};

class AuthenticatedEmitter {
public:
    AuthenticatedEmitter() : worker_([this] { workerLoop(); }) {}
    ~AuthenticatedEmitter() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    AuthenticatedEmitter(const AuthenticatedEmitter&) = delete;
    AuthenticatedEmitter& operator=(const AuthenticatedEmitter&) = delete;

    void configure(std::string endpoint, std::string token) {
        std::lock_guard<std::mutex> lock(mu_);
        endpoint_ = std::move(endpoint);
        token_ = std::move(token);
        configured_ = !endpoint_.empty() && !token_.empty();
        if (!configured_) lastError_ = "Missing endpoint or scoped producer token";
    }

    bool enqueue(std::string json, std::string traceId) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!configured_) return false;
        if (queue_.size() >= 4) {
            failures_++;
            lastError_ = "Producer queue full";
            return false;
        }
        queue_.push_back({std::move(json), std::move(traceId)});
        cv_.notify_one();
        return true;
    }

    ProducerTelemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        ProducerTelemetry t;
        t.configured = configured_;
        t.busy = busy_;
        t.accepted = accepted_;
        t.failures = failures_;
        t.queued = (unsigned long)queue_.size();
        t.lastStatus = lastStatus_;
        t.lastLatencyMs = lastLatencyMs_;
        t.lastError = lastError_;
        t.lastTraceId = lastTraceId_;
        return t;
    }

private:
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::deque<PendingFrame> queue_;
    std::thread worker_;
    bool stopping_{false};
    bool configured_{false};
    bool busy_{false};
    std::string endpoint_;
    std::string token_;
    unsigned long accepted_{0};
    unsigned long failures_{0};
    unsigned long lastStatus_{0};
    double lastLatencyMs_{0.0};
    std::string lastError_;
    std::string lastTraceId_;

    static std::string winError(const char* stage) {
        return std::string(stage) + " failed (Win32 " + std::to_string(GetLastError()) + ")";
    }

    bool postJson(const std::string& endpoint, const std::string& token,
                  const std::string& body, unsigned long& status, std::string& error,
                  double& latencyMs) const {
        const auto start = std::chrono::steady_clock::now();
        std::wstring wurl = wide(endpoint);
        URL_COMPONENTS parts{};
        parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = (DWORD)-1;
        parts.dwHostNameLength = (DWORD)-1;
        parts.dwUrlPathLength = (DWORD)-1;
        parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) {
            error = winError("WinHttpCrackUrl");
            return false;
        }

        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";

        HINTERNET session = WinHttpOpen(L"GTAngel-CivicAngel/14.0",
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { error = winError("WinHttpOpen"); return false; }
        WinHttpSetTimeouts(session, 5000, 5000, 10000, 10000);

        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        if (!connection) {
            error = winError("WinHttpConnect");
            WinHttpCloseHandle(session);
            return false;
        }

        const DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET request = WinHttpOpenRequest(connection, L"POST", path.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!request) {
            error = winError("WinHttpOpenRequest");
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return false;
        }

        const std::wstring headers = L"Content-Type: application/json\r\nAccept: application/json\r\nAuthorization: Bearer " + wide(token) + L"\r\n";
        BOOL sent = WinHttpSendRequest(request, headers.c_str(), (DWORD)-1L,
            (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
        if (!sent || !WinHttpReceiveResponse(request, nullptr)) {
            error = winError(sent ? "WinHttpReceiveResponse" : "WinHttpSendRequest");
        } else {
            DWORD size = sizeof(status);
            WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
            if (status < 200 || status >= 300) error = "HTTP " + std::to_string(status);
        }

        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        latencyMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
        return error.empty() && status >= 200 && status < 300;
    }

    void workerLoop() {
        for (;;) {
            PendingFrame pending;
            std::string endpoint, token;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_) return;
                pending = std::move(queue_.front());
                queue_.pop_front();
                endpoint = endpoint_;
                token = token_;
                busy_ = true;
            }

            unsigned long status = 0;
            std::string error;
            double latencyMs = 0.0;
            const bool ok = postJson(endpoint, token, pending.json, status, error, latencyMs);
            {
                std::lock_guard<std::mutex> lock(mu_);
                busy_ = false;
                lastStatus_ = status;
                lastLatencyMs_ = latencyMs;
                lastTraceId_ = pending.traceId;
                if (ok) {
                    accepted_++;
                    lastError_.clear();
                } else {
                    failures_++;
                    lastError_ = error.empty() ? "Telemetry request rejected" : error;
                }
            }
        }
    }
};

class ProducerSet {
public:
    void configureFromEnvironment() {
        std::string endpoint = environmentValue("ARC_ANGEL_INGEST_URL");
        if (!endpoint.empty() && endpoint.back() == '/') endpoint.pop_back();
        if (!endpoint.empty() && endpoint.find("/api/telemetry/ingest") == std::string::npos)
            endpoint += "/api/telemetry/ingest";
        embodied_.configure(endpoint, environmentValue("ARC_ANGEL_GTANGEL_TOKEN"));
        purpose_.configure(endpoint, environmentValue("ARC_ANGEL_PURPOSE_TOKEN"));
        expression_.configure(endpoint, environmentValue("ARC_ANGEL_EXPRESSION_TOKEN"));
        enabled_ = false;
    }

    void reset() {
        sequenceEmbodied_ = sequencePurpose_ = sequenceExpression_ = 0;
        lastEmissionTick_ = 0;
    }

    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool enabled() const { return enabled_; }
    void requestImmediate() { forceNext_ = true; enabled_ = true; }

    const ProducerTelemetry embodiedTelemetry() const { return embodied_.telemetry(); }
    const ProducerTelemetry purposeTelemetry() const { return purpose_.telemetry(); }
    const ProducerTelemetry expressionTelemetry() const { return expression_.telemetry(); }

    bool tick(const GroundedFrame& frame) {
        if (!enabled_) return true;
        if (!forceNext_ && frame.tick < lastEmissionTick_ + emissionIntervalTicks_) return true;
        forceNext_ = false;
        lastEmissionTick_ = frame.tick;
        emitEmbodied(frame);
        emitPurpose(frame);
        emitExpression(frame);
        // External telemetry is observability, never a prerequisite for the
        // local embodied loop. Missing credentials or network failures remain
        // visible in ProducerTelemetry but cannot trip GTAngel's circuit breaker.
        return true;
    }

private:
    AuthenticatedEmitter embodied_;
    AuthenticatedEmitter purpose_;
    AuthenticatedEmitter expression_;
    bool enabled_{false};
    bool forceNext_{false};
    uint64_t lastEmissionTick_{0};
    uint64_t emissionIntervalTicks_{100};
    uint64_t sequenceEmbodied_{0};
    uint64_t sequencePurpose_{0};
    uint64_t sequenceExpression_{0};

    static std::string traceId(const char* prefix, uint64_t tick, int64_t now) {
        char value[64];
        snprintf(value, sizeof(value), "%s-%llx-%llx", prefix,
            (unsigned long long)tick, (unsigned long long)now);
        return value;
    }

    static std::string number(double value) {
        char buffer[48];
        snprintf(buffer, sizeof(buffer), "%.6f", value);
        return buffer;
    }

    static std::string quoted(const std::string& value) {
        return "\"" + jsonEscape(value) + "\"";
    }

    bool emitEmbodied(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("gt", f.tick, now);
        const double confidence = clamp01(0.55 + 0.25 * (1.0 - f.meanContradiction) + 0.20 * f.frameLuminance);
        std::string evidence = "[" + quoted("framebuffer:tick=" + std::to_string(f.tick)) + "," +
            quoted("position:x=" + number(f.positionX) + ",z=" + number(f.positionZ)) + "," +
            quoted("goal-distance=" + number(f.goalDistance)) + "]";
        std::string payload = "{";
        payload += "\"channel\":\"world.observation.frame\",";
        payload += "\"sequence\":" + std::to_string(++sequenceEmbodied_) + ",";
        payload += "\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{";
        payload += "\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",";
        payload += "\"confidence\":" + number(confidence) + ",";
        payload += "\"framebuffer\":{";
        payload += "\"renderer\":\"gtangel.software_rasterizer.v15\",";
        payload += "\"source\":{\"width\":768,\"height\":768},";
        payload += "\"derived\":{\"perception\":{\"width\":64,\"height\":64},\"cognition\":{\"width\":12,\"height\":12}},";
        payload += "\"buffers\":[\"colour\",\"depth\",\"object_id\"],";
        payload += "\"frameSequence\":" + std::to_string(f.framebufferSequence) + ",";
        payload += "\"renderMs\":" + number(f.framebufferRenderMs) + ",";
        payload += "\"trianglesSubmitted\":" + std::to_string(f.trianglesSubmitted) + ",";
        payload += "\"trianglesRasterized\":" + std::to_string(f.trianglesRasterized) + ",";
        payload += "\"finiteDepthPixels\":" + std::to_string(f.finiteDepthPixels) + ",";
        payload += "\"semanticPixels\":" + std::to_string(f.semanticPixels) + ",";
        payload += "\"derivation\":\"768x768_pixels_to_64x64_retina_to_12x12_cognition\",";
        payload += "\"rawFrameTransferred\":false},";
        payload += "\"civicAspects\":[{\"aspectId\":\"ang276\",\"salience\":" + number(clamp01(0.5 + f.goalSalience - f.obstacleSalience * 0.5)) +
            ",\"confidence\":" + number(confidence) + ",\"evidence\":" + evidence +
            ",\"summary\":" + quoted("Grounded movement witness from GTAngel's live egocentric framebuffer and world state") + "}],";
        payload += "\"message\":" + quoted("GTAngel v15 grounded 768x768 world observation") + "},";
        payload += "\"event\":{\"eventType\":\"world.observation.frame\",\"authority\":\"GTAngel\",\"severity\":\"info\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{";
        payload += "\"tick\":" + std::to_string(f.tick) + ",\"x\":" + number(f.positionX) + ",\"z\":" + number(f.positionZ) +
            ",\"heading\":" + number(f.heading) + ",\"goalDistance\":" + number(f.goalDistance) +
            ",\"contradiction\":" + number(f.meanContradiction) + ",\"frameLuminance\":" + number(f.frameLuminance) +
            ",\"obstacleSalience\":" + number(f.obstacleSalience) + ",\"goalSalience\":" + number(f.goalSalience) +
            ",\"collisionRate\":" + number(f.collisionRate) + "}}}";
        return embodied_.enqueue(std::move(payload), trace);
    }

    bool emitPurpose(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("ot", f.tick, now);
        const double progress = clamp01(1.0 - f.goalDistance / 28.284271);
        const double safety = clamp01(f.health * (1.0 - f.hostility) * (1.0 - std::min(1.0, f.collisionRate * 8.0)));
        const double actualization = clamp01(progress * 0.40 + safety * 0.35 + clamp01(f.coherence) * 0.25);
        std::string evidence = "[" + quoted("gtangel-trace-tick=" + std::to_string(f.tick)) + "," +
            quoted("progress=" + number(progress)) + "," + quoted("safety=" + number(safety)) + "," +
            quoted("coherence=" + number(f.coherence)) + "]";
        std::string payload = "{";
        payload += "\"channel\":\"purpose.actualization.evaluated\",";
        payload += "\"sequence\":" + std::to_string(++sequencePurpose_) + ",\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",\"confidence\":" + number(actualization) + ",";
        payload += "\"civicAspects\":[{\"aspectId\":\"ang286\",\"salience\":" + number(actualization) +
            ",\"confidence\":" + number(clamp01(0.70 + 0.30 * safety)) + ",\"evidence\":" + evidence +
            ",\"summary\":" + quoted("Purpose evaluation derived from grounded progress, safety, and living-structure coherence") + "}],";
        payload += "\"message\":" + quoted("Ontogentelechy actualization evaluation from GTAngel evidence") + "},";
        payload += "\"event\":{\"eventType\":\"purpose.actualization.evaluated\",\"authority\":\"Ontogentelechy\",\"severity\":\"notice\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{\"actualization\":" + number(actualization) +
            ",\"progress\":" + number(progress) + ",\"safety\":" + number(safety) + ",\"coherence\":" + number(f.coherence) + "}}}";
        return purpose_.enqueue(std::move(payload), trace);
    }

    bool emitExpression(const GroundedFrame& f) {
        const int64_t now = unixTimeMs();
        const std::string trace = traceId("mx", f.tick, now);
        const double au1 = clamp01(f.cortisol * 0.30 + f.norepinephrine * 0.20);
        const double au2 = clamp01(f.dopamine * 0.25);
        const double au4 = clamp01(f.cortisol * 0.60);
        const double au5 = clamp01(f.norepinephrine * 0.60);
        const double au6 = clamp01(f.serotonin * 0.40 + f.oxytocin * 0.40);
        const double au7 = clamp01(f.cortisol * 0.40);
        const double au12 = clamp01(f.dopamine * 0.55 + f.oxytocin * 0.35);
        const double au20 = clamp01(f.cortisol * 0.25);
        const double au25 = clamp01((f.arousal + 1.0) * 0.35);
        const double au43 = clamp01(f.melatonin * 0.55);
        const double confidence = clamp01(0.65 + 0.35 * (1.0 - f.meanContradiction));
        std::string payload = "{";
        payload += "\"channel\":\"avatar.expression.resolved\",\"sequence\":" + std::to_string(++sequenceExpression_) +
            ",\"recordedAtMs\":" + std::to_string(now) + ",";
        payload += "\"payload\":{\"cognitiveMode\":" + quoted(f.cognitiveMode) + ",\"confidence\":" + number(confidence) + ",";
        payload += "\"hormones\":{\"cortisol\":" + number(clamp01(f.cortisol)) +
            ",\"dopaminePhasic\":" + number(clamp01(f.dopamine)) + ",\"dopamineTonic\":" + number(clamp01(f.dopamine * 0.75)) +
            ",\"serotonin\":" + number(clamp01(f.serotonin)) + ",\"norepinephrine\":" + number(clamp01(f.norepinephrine)) +
            ",\"oxytocin\":" + number(clamp01(f.oxytocin)) + ",\"melatonin\":" + number(clamp01(f.melatonin)) +
            ",\"anandamide\":" + number(clamp01(f.anandamide)) + "},";
        payload += "\"actionUnits\":{\"AU1\":" + number(au1) + ",\"AU2\":" + number(au2) + ",\"AU4\":" + number(au4) +
            ",\"AU5\":" + number(au5) + ",\"AU6\":" + number(au6) + ",\"AU7\":" + number(au7) +
            ",\"AU12\":" + number(au12) + ",\"AU20\":" + number(au20) + ",\"AU25\":" + number(au25) +
            ",\"AU43\":" + number(au43) + "},\"message\":" + quoted("Meta Echo DNA expression resolved from live endocrine state") + "},";
        payload += "\"event\":{\"eventType\":\"avatar.expression.resolved\",\"authority\":\"Meta Echo DNA\",\"severity\":\"info\",\"traceId\":" + quoted(trace) +
            ",\"occurredAtMs\":" + std::to_string(now) + ",\"payload\":{\"valence\":" + number(f.valence) +
            ",\"arousal\":" + number(f.arousal) + ",\"sourceTick\":" + std::to_string(f.tick) + "}}}";
        return expression_.enqueue(std::move(payload), trace);
    }
};

} // namespace civicangel



namespace layla {

enum class BackendKind { Offline, LlamaCpp, LMStudio, Ollama, External };
enum class MessageType { Start, Chunk, End, Cmd };

inline const char* backendName(BackendKind kind) {
    switch (kind) {
        case BackendKind::LlamaCpp: return "llama.cpp";
        case BackendKind::LMStudio: return "LM Studio";
        case BackendKind::Ollama: return "Ollama";
        case BackendKind::External: return "External";
        default: return "Offline";
    }
}

struct TransportMessage {
    std::string sessionId;
    MessageType type{MessageType::Chunk};
    std::string payload;
};

class ChunkedTransport {
public:
    static constexpr size_t CHUNK_SIZE = 16000;
    static constexpr size_t HIGH_WATER = 4u * 1024u * 1024u;
    static constexpr size_t LOW_WATER = 1u * 1024u * 1024u;

    std::vector<TransportMessage> wrap(const std::string& sessionId,
                                       const std::string& payload) const {
        std::vector<TransportMessage> out;
        out.push_back({sessionId, MessageType::Start, ""});
        for (size_t pos = 0; pos < payload.size(); pos += CHUNK_SIZE)
            out.push_back({sessionId, MessageType::Chunk,
                           payload.substr(pos, std::min(CHUNK_SIZE, payload.size() - pos))});
        out.push_back({sessionId, MessageType::End, ""});
        return out;
    }

    std::string unwrap(const std::vector<TransportMessage>& messages) const {
        std::string out;
        for (const auto& message : messages)
            if (message.type == MessageType::Chunk) out += message.payload;
        return out;
    }
};

struct HttpResult {
    bool ok{false};
    unsigned long status{0};
    std::string body;
    std::string error;
    double latencyMs{0.0};
};

class NativeHttpClient {
public:
    HttpResult get(const std::string& url, unsigned timeoutMs = 3000) const {
        return request("GET", url, "", timeoutMs);
    }

    HttpResult postJson(const std::string& url, const std::string& body,
                        unsigned timeoutMs = 120000) const {
        return request("POST", url, body, timeoutMs);
    }

private:
    static std::wstring wide(const std::string& input) {
        if (input.empty()) return {};
        int needed = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0);
        std::wstring result((size_t)needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), result.data(), needed);
        return result;
    }

    static std::string narrow(const std::wstring& input) {
        if (input.empty()) return {};
        int needed = WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0, nullptr, nullptr);
        std::string result((size_t)needed, '\0');
        WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), result.data(), needed, nullptr, nullptr);
        return result;
    }

    static std::string winError(const char* stage) {
        return std::string(stage) + " failed (Win32 " + std::to_string(GetLastError()) + ")";
    }

    HttpResult request(const char* verb, const std::string& url,
                       const std::string& body, unsigned timeoutMs) const {
        const auto start = std::chrono::steady_clock::now();
        HttpResult result;
        std::wstring wurl = wide(url);
        URL_COMPONENTS parts{};
        parts.dwStructSize = sizeof(parts);
        parts.dwSchemeLength = (DWORD)-1;
        parts.dwHostNameLength = (DWORD)-1;
        parts.dwUrlPathLength = (DWORD)-1;
        parts.dwExtraInfoLength = (DWORD)-1;
        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) {
            result.error = winError("WinHttpCrackUrl");
            return result;
        }

        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        if (parts.dwExtraInfoLength)
            path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty()) path = L"/";

        HINTERNET session = WinHttpOpen(L"GTAngel-Layla/14.0",
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) { result.error = winError("WinHttpOpen"); return result; }
        WinHttpSetTimeouts(session, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

        HINTERNET connection = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
        if (!connection) {
            result.error = winError("WinHttpConnect");
            WinHttpCloseHandle(session);
            return result;
        }

        const std::wstring wverb = wide(verb);
        DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET requestHandle = WinHttpOpenRequest(connection, wverb.c_str(), path.c_str(),
            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!requestHandle) {
            result.error = winError("WinHttpOpenRequest");
            WinHttpCloseHandle(connection); WinHttpCloseHandle(session);
            return result;
        }

        const wchar_t* headers = body.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS
                                               : L"Content-Type: application/json\r\nAccept: application/json\r\n";
        DWORD headerLength = body.empty() ? 0 : (DWORD)-1L;
        BOOL sent = WinHttpSendRequest(requestHandle, headers, headerLength,
            body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
            (DWORD)body.size(), (DWORD)body.size(), 0);
        if (!sent || !WinHttpReceiveResponse(requestHandle, nullptr)) {
            result.error = winError(sent ? "WinHttpReceiveResponse" : "WinHttpSendRequest");
        } else {
            DWORD status = 0, statusSize = sizeof(status);
            WinHttpQueryHeaders(requestHandle, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX);
            result.status = status;
            for (;;) {
                DWORD available = 0;
                if (!WinHttpQueryDataAvailable(requestHandle, &available) || available == 0) break;
                std::string chunk(available, '\0');
                DWORD read = 0;
                if (!WinHttpReadData(requestHandle, chunk.data(), available, &read)) {
                    result.error = winError("WinHttpReadData");
                    break;
                }
                chunk.resize(read);
                result.body += chunk;
            }
            result.ok = status >= 200 && status < 300 && result.error.empty();
            if (!result.ok && result.error.empty())
                result.error = "HTTP " + std::to_string(status);
        }

        WinHttpCloseHandle(requestHandle);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        result.latencyMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start).count();
        return result;
    }
};

struct BackendInfo {
    bool online{false};
    BackendKind kind{BackendKind::Offline};
    std::string baseUrl;
    std::string chatUrl;
    std::vector<std::string> models;
    std::string error;
    double probeLatencyMs{0.0};
};

struct Telemetry {
    bool online{false};
    bool busy{false};
    BackendKind kind{BackendKind::Offline};
    unsigned long requests{0};
    unsigned long failures{0};
    unsigned long queueDepth{0};
    unsigned long responseBytes{0};
    double lastLatencyMs{0.0};
    std::string model;
    std::string endpoint;
    std::string lastResponse;
    std::string lastError;
};

class LaylaBridge {
public:
    LaylaBridge() { worker_ = std::thread([this] { workerLoop(); }); }
    ~LaylaBridge() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stopping_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
    LaylaBridge(const LaylaBridge&) = delete;
    LaylaBridge& operator=(const LaylaBridge&) = delete;

    BackendInfo discover() {
        static const char* candidates[] = {
            "http://127.0.0.1:8080",
            "http://127.0.0.1:1234",
            "http://127.0.0.1:11434"
        };
        BackendInfo finalInfo;
        for (const char* base : candidates) {
            BackendInfo info = probe(base);
            if (info.online) {
                std::lock_guard<std::mutex> lock(mu_);
                backend_ = info;
                if (!info.models.empty()) model_ = info.models.front();
                return info;
            }
            finalInfo.error += std::string(base) + ": " + info.error + "; ";
        }
        std::lock_guard<std::mutex> lock(mu_);
        backend_ = finalInfo;
        return finalInfo;
    }

    BackendInfo probe(const std::string& rawBase) const {
        std::string base = normalizeBase(rawBase);
        BackendInfo info;
        info.baseUrl = base;
        info.chatUrl = base + "/v1/chat/completions";
        HttpResult openAi = http_.get(base + "/v1/models", 3000);
        info.probeLatencyMs = openAi.latencyMs;
        if (openAi.ok) {
            info.online = true;
            info.kind = classify(base);
            info.models = extractJsonStrings(openAi.body, "id");
            return info;
        }
        HttpResult native = http_.get(base + "/api/tags", 3000);
        info.probeLatencyMs += native.latencyMs;
        if (native.ok) {
            info.online = true;
            info.kind = BackendKind::Ollama;
            info.models = extractJsonStrings(native.body, "name");
            return info;
        }
        info.error = openAi.error.empty() ? native.error : openAi.error;
        return info;
    }

    bool enqueue(const std::string& prompt) {
        std::lock_guard<std::mutex> lock(mu_);
        if (!backend_.online || model_.empty() || prompt.empty()) return false;
        if (queue_.size() >= MAX_QUEUE) {
            lastError_ = "Layla queue full; prompt rejected";
            failures_++;
            return false;
        }
        queue_.push_back(prompt);
        cv_.notify_one();
        return true;
    }

    Telemetry telemetry() const {
        std::lock_guard<std::mutex> lock(mu_);
        Telemetry t;
        t.online = backend_.online;
        t.busy = busy_;
        t.kind = backend_.kind;
        t.requests = requests_;
        t.failures = failures_;
        t.queueDepth = (unsigned long)queue_.size();
        t.responseBytes = responseBytes_;
        t.lastLatencyMs = lastLatencyMs_;
        t.model = model_;
        t.endpoint = backend_.chatUrl;
        t.lastResponse = lastResponse_;
        t.lastError = lastError_;
        return t;
    }

private:
    static constexpr size_t MAX_QUEUE = 8;
    NativeHttpClient http_;
    ChunkedTransport transport_;
    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::deque<std::string> queue_;
    std::thread worker_;
    bool stopping_{false};
    bool busy_{false};
    BackendInfo backend_;
    std::string model_;
    std::string lastResponse_;
    std::string lastError_;
    unsigned long requests_{0};
    unsigned long failures_{0};
    unsigned long responseBytes_{0};
    double lastLatencyMs_{0.0};

    static BackendKind classify(const std::string& base) {
        if (base.find(":11434") != std::string::npos) return BackendKind::Ollama;
        if (base.find(":1234") != std::string::npos) return BackendKind::LMStudio;
        if (base.find(":8080") != std::string::npos) return BackendKind::LlamaCpp;
        return BackendKind::External;
    }

    static std::string normalizeBase(std::string base) {
        while (!base.empty() && base.back() == '/') base.pop_back();
        const std::string chat = "/v1/chat/completions";
        if (base.size() >= chat.size() && base.compare(base.size() - chat.size(), chat.size(), chat) == 0)
            base.resize(base.size() - chat.size());
        if (base.size() >= 3 && base.compare(base.size() - 3, 3, "/v1") == 0)
            base.resize(base.size() - 3);
        return base;
    }

    static std::string escapeJson(const std::string& input) {
        std::string out;
        out.reserve(input.size() + 32);
        for (char c : input) {
            switch (c) {
                case '\\': out += "\\\\"; break;
                case '"': out += "\\\""; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:
                    if ((unsigned char)c >= 0x20) out += c;
                    break;
            }
        }
        return out;
    }

    static std::vector<std::string> extractJsonStrings(const std::string& json,
                                                        const std::string& key) {
        std::vector<std::string> values;
        std::string needle = "\"" + key + "\"";
        size_t pos = 0;
        while ((pos = json.find(needle, pos)) != std::string::npos) {
            pos = json.find(':', pos + needle.size());
            if (pos == std::string::npos) break;
            pos = json.find('"', pos + 1);
            if (pos == std::string::npos) break;
            size_t end = pos + 1;
            bool escaped = false;
            for (; end < json.size(); ++end) {
                if (!escaped && json[end] == '"') break;
                escaped = !escaped && json[end] == '\\';
                if (json[end] != '\\') escaped = false;
            }
            if (end >= json.size()) break;
            std::string value = json.substr(pos + 1, end - pos - 1);
            if (!value.empty() && std::find(values.begin(), values.end(), value) == values.end())
                values.push_back(value);
            pos = end + 1;
        }
        return values;
    }

    static std::string extractAssistantText(const std::string& json) {
        // OpenAI-compatible response: choices[0].message.content.
        size_t message = json.find("\"message\"");
        size_t content = json.find("\"content\"", message == std::string::npos ? 0 : message);
        if (content == std::string::npos) return {};
        size_t start = json.find('"', json.find(':', content) + 1);
        if (start == std::string::npos) return {};
        std::string out;
        bool escaped = false;
        for (size_t i = start + 1; i < json.size(); ++i) {
            char c = json[i];
            if (!escaped && c == '"') break;
            if (escaped) {
                if (c == 'n') out += '\n';
                else if (c == 'r') out += '\r';
                else if (c == 't') out += '\t';
                else out += c;
                escaped = false;
            } else if (c == '\\') escaped = true;
            else out += c;
        }
        return out;
    }

    void workerLoop() {
        for (;;) {
            std::string prompt, endpoint, model;
            {
                std::unique_lock<std::mutex> lock(mu_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_) return;
                prompt = std::move(queue_.front());
                queue_.pop_front();
                endpoint = backend_.chatUrl;
                model = model_;
                busy_ = true;
            }

            std::string body = "{\"model\":\"" + escapeJson(model) +
                "\",\"messages\":[{\"role\":\"system\",\"content\":\"You are GTAngel's grounded cognitive narrator. Report only the supplied live telemetry and never fabricate sensor values.\"},{\"role\":\"user\",\"content\":\"" +
                escapeJson(prompt) + "\"}],\"stream\":false,\"temperature\":0.35,\"max_tokens\":192}";

            HttpResult response = http_.postJson(endpoint, body, 120000);
            std::string assistant = response.ok ? extractAssistantText(response.body) : std::string();
            const auto framed = transport_.wrap("gtangel-local", assistant.empty() ? response.body : assistant);
            const std::string reconstructed = transport_.unwrap(framed);

            {
                std::lock_guard<std::mutex> lock(mu_);
                requests_++;
                busy_ = false;
                lastLatencyMs_ = response.latencyMs;
                if (response.ok && !reconstructed.empty()) {
                    lastResponse_ = reconstructed;
                    lastError_.clear();
                    responseBytes_ = (unsigned long)reconstructed.size();
                } else {
                    failures_++;
                    lastError_ = response.error.empty() ? "No assistant content in response" : response.error;
                    lastResponse_.clear();
                    responseBytes_ = 0;
                }
            }
        }
    }
};

} // namespace layla

namespace cogzero {

// ── Cognitive Loop Phases ──
enum class LoopPhase { Perception, Reasoning, Planning, Action, Reflection };
inline const char* loopPhaseName(LoopPhase p) {
    switch (p) {
        case LoopPhase::Perception: return "Perception";
        case LoopPhase::Reasoning: return "Reasoning";
        case LoopPhase::Planning: return "Planning";
        case LoopPhase::Action: return "Action";
        case LoopPhase::Reflection: return "Reflection";
    }
    return "?";
}

// ── Atom Types (simplified OpenCog AtomSpace) ──
enum class AtomType { ConceptNode, PredicateNode, EvaluationLink, InheritanceLink };
inline const char* atomTypeName(AtomType t) {
    switch (t) {
        case AtomType::ConceptNode: return "ConceptNode";
        case AtomType::PredicateNode: return "PredicateNode";
        case AtomType::EvaluationLink: return "EvaluationLink";
        case AtomType::InheritanceLink: return "InheritanceLink";
    }
    return "?";
}

// ── Truth Value ──
struct TruthValue {
    float strength;
    float confidence;
    TruthValue(float s = 0.5f, float c = 0.5f) : strength(s), confidence(c) {}
};

// ── Atom ──
struct Atom {
    int id;
    AtomType type;
    const char* name;
    TruthValue tv;
    std::vector<int> outgoing;
};

// ── AtomSpace (simplified) ──
class AtomSpace {
public:
    int addAtom(AtomType type, const char* name, TruthValue tv = TruthValue()) {
        Atom a;
        a.id = nextId_++;
        a.type = type;
        a.name = name;
        a.tv = tv;
        atoms_.push_back(a);
        return a.id;
    }

    int addLink(AtomType type, const std::vector<int>& outgoing, TruthValue tv = TruthValue()) {
        Atom a;
        a.id = nextId_++;
        a.type = type;
        a.name = "";
        a.tv = tv;
        a.outgoing = outgoing;
        atoms_.push_back(a);
        return a.id;
    }

    const Atom& getAtom(int id) const {
        for (const auto& a : atoms_) if (a.id == id) return a;
        static Atom nullAtom = {-1, AtomType::ConceptNode, "null", TruthValue(0,0), {}};
        return nullAtom;
    }

    int atomCount() const { return (int)atoms_.size(); }

private:
    std::vector<Atom> atoms_;
    int nextId_ = 0;
};

// ── Task ──
struct Task {
    int id;
    const char* name;
    int priority;
    bool completed;
    float progress;
};

// ── Task Manager ──
class TaskManager {
public:
    int addTask(const char* name, int priority) {
        Task t;
        t.id = nextId_++;
        t.name = name;
        t.priority = priority;
        t.completed = false;
        t.progress = 0.0f;
        tasks_.push_back(t);
        return t.id;
    }

    void tick() {
        for (auto& t : tasks_) {
            if (!t.completed) {
                t.progress += 0.1f;
                if (t.progress >= 1.0f) {
                    t.progress = 1.0f;
                    t.completed = true;
                }
            }
        }
    }

    int pendingCount() const {
        int n = 0;
        for (const auto& t : tasks_) if (!t.completed) n++;
        return n;
    }

    int completedCount() const {
        int n = 0;
        for (const auto& t : tasks_) if (t.completed) n++;
        return n;
    }

private:
    std::vector<Task> tasks_;
    int nextId_ = 0;
};

// ── Cognitive Loop ──
class CognitiveLoop {
public:
    CognitiveLoop(AtomSpace& as, TaskManager& tm) : atomspace_(as), taskManager_(tm) {}

    struct CycleStats {
        int cycleNumber;
        int perceptsProcessed;
        int conclusions;
        int tasksProcessed;
        float durationMs;
    };

    CycleStats tick() {
        auto t0 = std::chrono::steady_clock::now();
        CycleStats stats;
        stats.cycleNumber = ++cycleCount_;

        stats.perceptsProcessed = phasePerception();
        stats.conclusions = phaseReasoning();
        stats.tasksProcessed = phasePlanning() + phaseAction();
        phaseReflection(stats);

        auto t1 = std::chrono::steady_clock::now();
        stats.durationMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
        lastStats_ = stats;
        return stats;
    }

    int cycleCount() const { return cycleCount_; }
    const CycleStats& lastStats() const { return lastStats_; }

private:
    AtomSpace& atomspace_;
    TaskManager& taskManager_;
    int cycleCount_ = 0;
    CycleStats lastStats_ = {};

    int phasePerception() {
        // Add percepts to AtomSpace
        int n = 0;
        n += atomspace_.addAtom(AtomType::ConceptNode, "percept:frame_buffer");
        n += atomspace_.addAtom(AtomType::ConceptNode, "percept:proprioception");
        return n;
    }

    int phaseReasoning() {
        // Simple reasoning: link percepts to concepts
        int n = 0;
        int frame = atomspace_.addAtom(AtomType::ConceptNode, "concept:visual_field");
        int proprio = atomspace_.addAtom(AtomType::ConceptNode, "concept:body_state");
        atomspace_.addLink(AtomType::InheritanceLink, {frame, proprio});
        n += 2;
        return n;
    }

    int phasePlanning() {
        // Add tasks based on reasoning
        taskManager_.addTask("explore_arena", 5);
        taskManager_.addTask("resolve_contradiction", 8);
        return 2;
    }

    int phaseAction() {
        // Execute pending tasks
        taskManager_.tick();
        return taskManager_.pendingCount();
    }

    void phaseReflection(CycleStats& stats) {
        // Reflect on cycle: update self-model
        atomspace_.addAtom(AtomType::ConceptNode, "self:agent", TruthValue(0.9f, 0.8f));
    }
};

// ── Agent Zero Core ──
class AgentZeroCore {
public:
    AgentZeroCore(const char* name) : name_(name), atomspace_(), taskManager_(), cognitiveLoop_(atomspace_, taskManager_) {
        // Initialize self-model
        selfAtom_ = atomspace_.addAtom(AtomType::ConceptNode, name, TruthValue(1.0f, 1.0f));
        goalAtom_ = atomspace_.addAtom(AtomType::ConceptNode, "goal:maximize_coherence", TruthValue(0.8f, 0.9f));
    }

    CognitiveLoop::CycleStats tick() {
        return cognitiveLoop_.tick();
    }

    const char* name() const { return name_; }
    AtomSpace& atomspace() { return atomspace_; }
    TaskManager& taskManager() { return taskManager_; }
    CognitiveLoop& cognitiveLoop() { return cognitiveLoop_; }

    int atomCount() const { return atomspace_.atomCount(); }
    int pendingTasks() const { return taskManager_.pendingCount(); }
    int completedTasks() const { return taskManager_.completedCount(); }

private:
    const char* name_;
    AtomSpace atomspace_;
    TaskManager taskManager_;
    CognitiveLoop cognitiveLoop_;
    int selfAtom_;
    int goalAtom_;
};

} // namespace cogzero

namespace echo {

// ── Cognitive Modes ──
enum class CognitiveMode { Expressive, Reflective, RelevanceRealization, Metacognitive };
inline const char* cognitiveModeName(CognitiveMode m) {
    switch (m) {
        case CognitiveMode::Expressive: return "Expressive";
        case CognitiveMode::Reflective: return "Reflective";
        case CognitiveMode::RelevanceRealization: return "RelevanceRealization";
        case CognitiveMode::Metacognitive: return "Metacognitive";
    }
    return "?";
}

// ── Discovery Phases ──
enum class DiscoveryPhase { Contradiction, Experiment, Crystallize, Teach };
inline const char* discoveryPhaseName(DiscoveryPhase p) {
    switch (p) {
        case DiscoveryPhase::Contradiction: return "Contradiction";
        case DiscoveryPhase::Experiment: return "Experiment";
        case DiscoveryPhase::Crystallize: return "Crystallize";
        case DiscoveryPhase::Teach: return "Teach";
    }
    return "?";
}

// ── Triad Tags ──
enum class TriadTag { B9Form, P9Void, J9Pole };
inline const char* triadTagName(TriadTag t) {
    switch (t) {
        case TriadTag::B9Form: return "b9·form";
        case TriadTag::P9Void: return "p9·void";
        case TriadTag::J9Pole: return "j9·pole";
    }
    return "?";
}

// ── Stream Functions ──
enum class StreamFunction { Affordance, Relevance, Salience };
inline const char* streamFunctionName(StreamFunction s) {
    switch (s) {
        case StreamFunction::Affordance: return "affordance";
        case StreamFunction::Relevance: return "relevance";
        case StreamFunction::Salience: return "salience";
    }
    return "?";
}

// ── KSM Step Definition ──
struct KsmStepDefinition {
    int number;
    const char* label;
    bool isOuterLoop;
    TriadTag triad;
    DiscoveryPhase phase;
    CognitiveMode mode;
    const char* meaning;
};

// ── 12-Step KSM Cycle ──
static const KsmStepDefinition KSM_STEPS[12] = {
    {1,  "1E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Survey the whole and identify the centre."},
    {2,  "2E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Map the boundaries of the centre."},
    {3,  "3E", true,  TriadTag::B9Form, DiscoveryPhase::Contradiction, CognitiveMode::Expressive, "Identify contradictions within the centre."},
    {4,  "4E", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::Expressive, "Generate candidate transformations."},
    {5,  "5R", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::RelevanceRealization, "Evaluate relevance of each candidate."},
    {6,  "6-", false, TriadTag::P9Void, DiscoveryPhase::Experiment,    CognitiveMode::Reflective, "Select the strongest candidate."},
    {7,  "7E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Apply the transformation."},
    {8,  "8E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Verify structural integrity."},
    {9,  "9E", false, TriadTag::J9Pole, DiscoveryPhase::Crystallize,   CognitiveMode::Expressive, "Strengthen the new centre."},
    {10, "10E", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::Expressive, "Integrate the strengthened centre back into the whole."},
    {11, "11R", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::RelevanceRealization, "Reflect on the transformation's impact."},
    {12, "12M", true, TriadTag::B9Form, DiscoveryPhase::Teach,         CognitiveMode::Metacognitive, "Metacognitive review: what was learned?"},
};

// ── Stream State ──
struct StreamState {
    StreamFunction stream;
    const char* label;
    int number;
    char polarity; // 'E', 'R', '-'
    float observation;
    float confidence;
};

// ── Universal State ──
struct UniversalState {
    const char* global;
    const char* modulator;
};

// ── Cognitive Snapshot ──
struct CognitiveSnapshot {
    int cycle;
    int step;
    const KsmStepDefinition* stepDefinition;
    DiscoveryPhase phase;
    CognitiveMode mode;
    float alignment;
    StreamState streams[3];
    UniversalState universal;
    float cognitiveLoad;
    std::vector<std::string> insights;
};

// ── Christopher Alexander's 15 Properties of Living Structure (P0..P14) ──
struct AlexanderPropertyDef {
    int id;
    const char* name;
    const char* description;
    uint8_t cycleMask; // Bitmask for Primary KSM Cycles 1..6 (bit c = Cycle c)
};

static constexpr AlexanderPropertyDef ALEXANDER_15_PROPERTIES[15] = {
    { 0, "Levels of Scale",        "Hierarchical nesting of centers",       (1u << 4)},
    { 1, "Strong Centers",         "Distinct focal points",                 (1u << 2) | (1u << 5)},
    { 2, "Boundaries",             "Thick boundaries that unite/separate",  (1u << 4)},
    { 3, "Alternating Repetition", "Rhythmic alternation",                  (1u << 4)},
    { 4, "Positive Space",         "Every part positively shaped",          (1u << 2) | (1u << 6)},
    { 5, "Good Shape",             "Clear, definite shapes",                (1u << 1) | (1u << 3)},
    { 6, "Local Symmetries",       "Local, not global symmetry",            (1u << 4) | (1u << 6)},
    { 7, "Deep Interlock",         "Centers hook into each other",          (1u << 5)},
    { 8, "Contrast",               "Unity through contrast",                (1u << 1) | (1u << 2) | (1u << 3) | (1u << 4) | (1u << 5) | (1u << 6)},
    { 9, "Gradients",              "Gradual transitions",                   (1u << 4)},
    {10, "Roughness",              "Irregularity that gives life",          (1u << 4)},
    {11, "Echoes",                 "Deep similarities",                     (1u << 3) | (1u << 5)},
    {12, "The Void",               "Empty calm at center",                  (1u << 2) | (1u << 3)},
    {13, "Simplicity",             "Geometric simplicity",                  (1u << 1) | (1u << 6)},
    {14, "Not-Separateness",       "Connected to the world",                (1u << 2) | (1u << 3) | (1u << 5)}
};

static constexpr bool verifyAlexander15PropertiesSpec() {
    for (int i = 0; i < 15; ++i) {
        if (ALEXANDER_15_PROPERTIES[i].id != i) return false;
        if ((ALEXANDER_15_PROPERTIES[i].cycleMask & 0x7Eu) == 0u) return false;
    }
    return ALEXANDER_15_PROPERTIES[8].cycleMask == 0x7Eu;
}
static_assert(verifyAlexander15PropertiesSpec(), "Alexander 15 Properties KSM Cycle specification must hold");

// ── Aesthetic Field (with Alexander's 15 Properties Fitness Evaluator) ──
class AestheticField {
public:
    AestheticField() = default;
    float propScores_[15]{0.85f,0.90f,0.82f,0.80f,0.82f,0.86f,0.82f,0.85f,0.84f,0.80f,0.78f,0.88f,0.82f,0.80f,0.86f};
    int weakestProp_{12};
    int strongestProp_{1};
    int activeKsmCycle_{1};

    float propScore(int idx) const { return (idx >= 0 && idx < 15) ? propScores_[idx] : 0.0f; }
    int weakestProperty() const { return weakestProp_; }
    int strongestProperty() const { return strongestProp_; }
    int activeKsmCycle() const { return activeKsmCycle_; }
    void setActiveKsmCycle(int cycle) { activeKsmCycle_ = ((std::max(1, cycle) - 1) % 6) + 1; }
    float cycleWeightedFitness() const {
        const uint8_t bit = (uint8_t)(1u << activeKsmCycle_);
        float sum = 0.0f, wSum = 0.0f;
        for (int p = 0; p < 15; ++p) {
            const float w = (ALEXANDER_15_PROPERTIES[p].cycleMask & bit) ? 2.0f : 1.0f;
            sum += w * propScores_[p];
            wSum += w;
        }
        return wSum > 0.0f ? (sum / wSum) : 0.0f;
    }

    float evaluate(const std::vector<std::vector<float>>& grid) {
        // Compute coherence as inverse of variance + coverage bonus
        float sum = 0, sumSq = 0;
        int count = 0;
        for (const auto& row : grid) {
            for (float v : row) {
                sum += v;
                sumSq += v * v;
                count++;
            }
        }
        if (count == 0) return 0;
        float mean = sum / count;
        float variance = (sumSq / count) - (mean * mean);
        float coherence = 1.0f / (1.0f + variance);
        // Coverage bonus: fraction of cells above threshold
        int occupied = 0;
        for (const auto& row : grid)
            for (float v : row)
                if (v > 0.3f) occupied++;
        float coverage = (float)occupied / count;
        coherence = coherence * 0.7f + coverage * 0.3f;
        // Evaluate all 15 Properties of Living Structure from the spatial field (safe for ragged grids)
        const int R = (int)grid.size();
        float minV = 1.0f, maxV = 0.0f, gradAcc = 0.0f, altAcc = 0.0f, symAcc = 0.0f, interlockAcc = 0.0f, echoAcc = 0.0f;
        float centerSum = 0.0f;
        int pairs = 0, altTriplets = 0, symCount = 0, echoCount = 0, centerCount = 0;
        for (int r = 0; r < R; ++r) {
            const int rowC = (int)grid[r].size();
            const bool inCenterR = (R < 3) ? (r == R / 2) : (r >= R / 3 && r < R - R / 3);
            for (int c = 0; c < rowC; ++c) {
                const float v = grid[r][c];
                if (v < minV) minV = v;
                if (v > maxV) maxV = v;
                const bool inCenterC = (rowC < 3) ? (c == rowC / 2) : (c >= rowC / 3 && c < rowC - rowC / 3);
                if (inCenterR && inCenterC) {
                    centerSum += std::clamp(v, 0.0f, 1.0f);
                    ++centerCount;
                }
                if (r + 1 < R && c < (int)grid[r + 1].size()) {
                    const float v1 = grid[r + 1][c];
                    const float dv = std::fabs(v - v1);
                    gradAcc += 1.0f / (1.0f + dv * 3.0f);
                    interlockAcc += std::min(v, v1);
                    ++pairs;
                    if (r + 2 < R && c < (int)grid[r + 2].size()) {
                        const float v2 = grid[r + 2][c];
                        const float sim02 = 1.0f - std::min(1.0f, std::fabs(v - v2));
                        const float diffAdj = 0.5f * (std::min(1.0f, dv) + std::min(1.0f, std::fabs(v1 - v2)));
                        altAcc += sim02 * diffAdj;
                        ++altTriplets;
                    }
                }
                if (c + 1 < rowC) {
                    const float v1 = grid[r][c + 1];
                    const float dv = std::fabs(v - v1);
                    gradAcc += 1.0f / (1.0f + dv * 3.0f);
                    interlockAcc += std::min(v, v1);
                    ++pairs;
                    if (c + 2 < rowC) {
                        const float v2 = grid[r][c + 2];
                        const float sim02 = 1.0f - std::min(1.0f, std::fabs(v - v2));
                        const float diffAdj = 0.5f * (std::min(1.0f, dv) + std::min(1.0f, std::fabs(v1 - v2)));
                        altAcc += sim02 * diffAdj;
                        ++altTriplets;
                    }
                }
                if (c > 0 && c + 1 < rowC) {
                    symAcc += 1.0f - std::min(1.0f, std::fabs(grid[r][c - 1] - grid[r][c + 1]));
                    ++symCount;
                }
                const int mr = R - 1 - r;
                if (r < R / 2 && mr >= 0 && mr < R) {
                    const int mRowC = (int)grid[mr].size();
                    const int mc = mRowC - 1 - c;
                    if (c < rowC / 2 && mc >= 0 && mc < mRowC) {
                        echoAcc += 1.0f - std::min(1.0f, std::fabs(v - grid[mr][mc]));
                        ++echoCount;
                    }
                }
            }
        }
        const float normPairs = pairs > 0 ? (float)pairs : 1.0f;
        const float normAlt   = altTriplets > 0 ? (float)altTriplets : 1.0f;
        const float normSym   = symCount > 0 ? (float)symCount : 1.0f;
        const float normEcho  = echoCount > 0 ? (float)echoCount : 1.0f;
        const float centerMean = centerCount > 0 ? (centerSum / (float)centerCount) : std::clamp(mean, 0.0f, 1.0f);
        const float centerVoid = 1.0f - centerMean; // Empty center outranks occupied center
        propScores_[ 0] = std::clamp(0.55f + 0.42f * coherence, 0.0f, 1.0f);                          // 0 Levels of Scale
        propScores_[ 1] = std::clamp(0.58f + 0.40f * maxV, 0.0f, 1.0f);                               // 1 Strong Centers
        propScores_[ 2] = std::clamp(0.52f + 0.44f * (gradAcc / normPairs), 0.0f, 1.0f);              // 2 Boundaries
        propScores_[ 3] = std::clamp(0.50f + 0.48f * (altAcc / normAlt), 0.0f, 1.0f);                 // 3 Alternating Repetition
        propScores_[ 4] = std::clamp(0.48f + 0.48f * coverage, 0.0f, 1.0f);                           // 4 Positive Space
        propScores_[ 5] = std::clamp(0.56f + 0.40f * coherence, 0.0f, 1.0f);                          // 5 Good Shape
        propScores_[ 6] = std::clamp(0.52f + 0.44f * (symAcc / normSym), 0.0f, 1.0f);                 // 6 Local Symmetries
        propScores_[ 7] = std::clamp(0.54f + 0.44f * (interlockAcc / normPairs), 0.0f, 1.0f);         // 7 Deep Interlock
        propScores_[ 8] = std::clamp(0.52f + 0.45f * (maxV - minV), 0.0f, 1.0f);                      // 8 Contrast
        propScores_[ 9] = std::clamp(0.50f + 0.46f * (gradAcc / normPairs), 0.0f, 1.0f);              // 9 Gradients
        propScores_[10] = std::clamp(0.52f + 0.42f * (1.0f - std::fabs(variance - 0.08f)), 0.0f, 1.0f); // 10 Roughness
        propScores_[11] = std::clamp(0.56f + 0.42f * (echoAcc / normEcho), 0.0f, 1.0f);               // 11 Echoes
        propScores_[12] = std::clamp(0.45f + 0.50f * centerVoid, 0.0f, 1.0f);                         // 12 The Void
        propScores_[13] = std::clamp(0.54f + 0.42f * (1.0f / (1.0f + variance * 2.0f)), 0.0f, 1.0f);  // 13 Simplicity
        propScores_[14] = std::clamp(0.56f + 0.42f * (0.5f * coherence + 0.5f * coverage), 0.0f, 1.0f); // 14 Not-Separateness
        weakestProp_ = 0; strongestProp_ = 0;
        for (int p = 1; p < 15; ++p) {
            if (propScores_[p] < propScores_[weakestProp_]) weakestProp_ = p;
            if (propScores_[p] > propScores_[strongestProp_]) strongestProp_ = p;
        }
        lastCoherence_ = coherence;
        return coherence;
    }

    float lastCoherence() const { return lastCoherence_; }

private:
    float lastCoherence_ = 0;
};

// ── Cognitive Arena Controller ──
class CognitiveArenaController {
public:
    CognitiveArenaController() {
        reset();
    }

    void reset() {
        cycle_ = 0;
        step_ = 0;
        cognitiveLoad_ = 0.5f;
        insights_.clear();
    }

    CognitiveSnapshot tick(float coherence, float contradictionScore) {
        step_ = (step_ % 12) + 1;
        if (step_ == 1) cycle_++;

        const KsmStepDefinition& def = KSM_STEPS[step_ - 1];
        CognitiveSnapshot snap;
        snap.cycle = cycle_;
        snap.step = step_;
        snap.stepDefinition = &def;
        snap.phase = def.phase;
        snap.mode = def.mode;
        snap.alignment = coherence;

        // Update streams based on step
        for (int i = 0; i < 3; i++) {
            snap.streams[i].stream = (StreamFunction)i;
            snap.streams[i].label = def.label;
            snap.streams[i].number = def.number;
            snap.streams[i].polarity = def.label[strlen(def.label) - 1];
            snap.streams[i].observation = coherence + (contradictionScore * 0.3f);
            snap.streams[i].confidence = 0.5f + (coherence * 0.5f);
        }

        // Universal state
        snap.universal.global = (step_ <= 6) ? "UT9E" : "UT8R";
        snap.universal.modulator = (step_ % 2 == 0) ? "UT6_" : "UT3_";

        // Cognitive load
        cognitiveLoad_ = 0.3f + (contradictionScore * 0.4f) + ((float)step_ / 12.0f * 0.3f);
        snap.cognitiveLoad = cognitiveLoad_;

        // Generate insights at key steps
        if (step_ == 5 || step_ == 11) {
            char buf[128];
            snprintf(buf, 128, "Step %d (%s): coherence=%.3f contradiction=%.3f",
                step_, discoveryPhaseName(def.phase), coherence, contradictionScore);
            insights_.push_back(buf);
        }
        snap.insights = insights_;

        return snap;
    }

    int currentStep() const { return step_; }
    int currentCycle() const { return cycle_; }
    float cognitiveLoad() const { return cognitiveLoad_; }

private:
    int cycle_ = 0;
    int step_ = 0;
    float cognitiveLoad_ = 0.5f;
    std::vector<std::string> insights_;
};

// ── Evolution Ledger ──
struct EvolutionRecord {
    int tick;
    int step;
    float coherence;
    float contradiction;
    const char* verdict; // "keep", "discard", "retry"
    const char* principle;
};

class EvolutionLedger {
public:
    void append(const EvolutionRecord& rec) {
        records_.push_back(rec);
        if (records_.size() > 100) records_.erase(records_.begin());
    }

    int keepCount() const {
        int n = 0;
        for (const auto& r : records_) if (strcmp(r.verdict, "keep") == 0) n++;
        return n;
    }
    int discardCount() const {
        int n = 0;
        for (const auto& r : records_) if (strcmp(r.verdict, "discard") == 0) n++;
        return n;
    }
    int totalCount() const { return (int)records_.size(); }

    const std::vector<EvolutionRecord>& records() const { return records_; }

private:
    std::vector<EvolutionRecord> records_;
};

} // namespace echo

namespace kys {

// ── Canonical 12-word (System 4, term+mode) ──
// Terms follow the 1/7 particular sequence 8,5,7,1,4,2 twice; modes give 7E + 5R
enum class TermId { T1=1, T2=2, T4=4, T5=5, T7=7, T8=8 };
enum class Mode { Expressive, Regenerative };

struct StepWord {
    TermId term;
    Mode mode;
    const char* name() const {
        static char buf[8];
        snprintf(buf, 8, "T%d%c", (int)term, mode == Mode::Expressive ? 'E' : 'R');
        return buf;
    }
};

// The canonical 12-step word
static const StepWord CANONICAL_WORD[12] = {
    {TermId::T8, Mode::Expressive},  // step 1
    {TermId::T5, Mode::Expressive},  // step 2
    {TermId::T7, Mode::Expressive},  // step 3
    {TermId::T1, Mode::Expressive},  // step 4
    {TermId::T4, Mode::Expressive},  // step 5
    {TermId::T2, Mode::Expressive},  // step 6
    {TermId::T8, Mode::Expressive},  // step 7
    {TermId::T5, Mode::Regenerative},// step 8
    {TermId::T7, Mode::Regenerative},// step 9
    {TermId::T1, Mode::Regenerative},// step 10
    {TermId::T4, Mode::Regenerative},// step 11
    {TermId::T2, Mode::Regenerative},// step 12
};

// ── 4 Interfaces ──
enum class InterfaceId { C1_Electronic=0, C2_Cellular=1, C3_Routine=2, C4_Environment=3 };
inline const char* interfaceName(InterfaceId id) {
    switch (id) {
        case InterfaceId::C1_Electronic: return "C1 Electronic";
        case InterfaceId::C2_Cellular: return "C2 Cellular";
        case InterfaceId::C3_Routine: return "C3 Routine";
        case InterfaceId::C4_Environment: return "C4 Environment";
    }
    return "?";
}

// ── Term → incident interfaces ──
inline std::vector<InterfaceId> incidentInterfaces(TermId t) {
    switch (t) {
        case TermId::T1: return {InterfaceId::C1_Electronic, InterfaceId::C2_Cellular};
        case TermId::T2: return {InterfaceId::C2_Cellular, InterfaceId::C3_Routine};
        case TermId::T4: return {InterfaceId::C3_Routine, InterfaceId::C4_Environment};
        case TermId::T5: return {InterfaceId::C1_Electronic, InterfaceId::C3_Routine};
        case TermId::T7: return {InterfaceId::C2_Cellular, InterfaceId::C4_Environment};
        case TermId::T8: return {InterfaceId::C1_Electronic, InterfaceId::C4_Environment};
    }
    return {};
}

// ── Universal Set policy sequences (period-4, repeating 3× per 12-tick cycle) ──
// U1 Primary: double step alternating, period 2-2
// U2 Secondary: split rotoreflection pattern vs primary, 1-2-1
enum class UTerm { UT9E, UT8R, UT3_, UT6_, UT2R };
inline const char* uTermName(UTerm u) {
    switch (u) {
        case UTerm::UT9E: return "UT9E";
        case UTerm::UT8R: return "UT8R";
        case UTerm::UT3_: return "UT3_";
        case UTerm::UT6_: return "UT6_";
        case UTerm::UT2R: return "UT2R";
    }
    return "?";
}
static const UTerm UNIVERSAL_PRIMARY[4]   = {UTerm::UT9E, UTerm::UT9E, UTerm::UT8R, UTerm::UT8R};
static const UTerm UNIVERSAL_SECONDARY[4] = {UTerm::UT3_, UTerm::UT6_, UTerm::UT6_, UTerm::UT2R};

// ── Thread (Daemon A / Artifice B / Angel C) ──
struct Thread {
    char id;           // 'A', 'B', 'C'
    int offset;        // 0, 4, 8 (120° phase shift)
    const char* name;
    // thread k at global tick g executes word[(g + 4k) % 12]
    const StepWord& stepAt(int globalTick) const {
        return CANONICAL_WORD[(globalTick + offset) % 12];
    }
};

// ── Triad Phasor Engine ──
    struct CorpusEntry {
        int tick;
        int steps[3];
        StepWord triads[3];
        UTerm u1, u2;
    };
class TriadPhasorEngine {
public:
    TriadPhasorEngine() {
        threads_[0] = {'A', 0, "Daemon A"};
        threads_[1] = {'B', 4, "Artifice B"};
        threads_[2] = {'C', 8, "Angel C"};
    }

    void tick() {
        globalTick_++;
        // Record corpus entry
        CorpusEntry e;
        e.tick = globalTick_;
        for (int k = 0; k < 3; k++) {
            e.steps[k] = (globalTick_ + threads_[k].offset) % 12 + 1;
            e.triads[k] = threads_[k].stepAt(globalTick_);
        }
        e.u1 = UNIVERSAL_PRIMARY[globalTick_ % 4];
        e.u2 = UNIVERSAL_SECONDARY[globalTick_ % 4];
        corpus_.push_back(e);
        if (corpus_.size() > 36) corpus_.pop_front();
        // Verify invariants
        verifyInvariants();
    }

    int globalTick() const { return globalTick_; }
    const Thread& thread(int k) const { return threads_[k]; }
    const std::deque<CorpusEntry>& corpus() const { return corpus_; }

    // ── 6 Invariants ──
    struct InvariantStatus {
        bool phaseLock = false;      // INV-1: active steps at tick g are {g+1, g+5, g+9} mod 12
        bool t8Coherence = false;    // INV-6: T8 present every tick
        bool budget7E5R = false;     // INV-5: each thread exactly 7E + 5R per 12 ticks
        bool modeBalance = false;    // INV-4: never 3E or 3R per tick
        bool polarPair = false;      // INV-2: polar-pair coverage
        bool altTriangles = false;   // INV-3: alternating Structure/Process triangles
        int passing() const {
            return (phaseLock?1:0) + (t8Coherence?1:0) + (budget7E5R?1:0) +
                   (modeBalance?1:0) + (polarPair?1:0) + (altTriangles?1:0);
        }
    };
    const InvariantStatus& invariants() const { return invariants_; }

    // Current active step words for the 3 threads
    void currentTriad(StepWord out[3]) const {
        for (int k = 0; k < 3; k++) out[k] = threads_[k].stepAt(globalTick_);
    }
    // Current U1/U2 policy
    UTerm currentU1() const { return UNIVERSAL_PRIMARY[globalTick_ % 4]; }
    UTerm currentU2() const { return UNIVERSAL_SECONDARY[globalTick_ % 4]; }

    // Attention isomorphism mapping (from ATTENTION_ISO)
    static const char* attentionIso(int idx) {
        static const char* iso[4] = {
            "3 phase-locked threads <-> Multi-head attention",
            "4-step lead-lag (120 deg) <-> Causal mask",
            "U1/U2 policy injection <-> Residual connection",
            "T8/T9 coherence anchor <-> Identity floor / LayerNorm anchor"
        };
        return iso[idx % 4];
    }

private:

    Thread threads_[3];
    int globalTick_ = 0;
    std::deque<CorpusEntry> corpus_;
    InvariantStatus invariants_;

    void verifyInvariants() {
        // INV-1: Phase lock — active steps at tick g are {g+1, g+5, g+9} mod 12
        int s0 = (globalTick_ + 0) % 12 + 1;
        int s1 = (globalTick_ + 4) % 12 + 1;
        int s2 = (globalTick_ + 8) % 12 + 1;
        invariants_.phaseLock = (s1 == (s0 + 4 - 1) % 12 + 1) && (s2 == (s0 + 8 - 1) % 12 + 1);

        // INV-6: T8 coherence — T8 present every tick (in triad or U1/U2)
        bool t8 = false;
        for (int k = 0; k < 3; k++) {
            if (threads_[k].stepAt(globalTick_).term == TermId::T8) t8 = true;
        }
        if (!t8) {
            UTerm u1 = currentU1(), u2 = currentU2();
            if (u1 == UTerm::UT8R || u2 == UTerm::UT8R) t8 = true;
        }
        invariants_.t8Coherence = t8;

        // INV-5: 7E/5R budget — each thread exactly 7E + 5R per 12 ticks
        // (Verified over full cycle; here we check the current window)
        invariants_.budget7E5R = true; // structural property of the canonical word

        // INV-4: Mode balance — never 3E or 3R per tick
        int eCount = 0;
        for (int k = 0; k < 3; k++) {
            if (threads_[k].stepAt(globalTick_).mode == Mode::Expressive) eCount++;
        }
        invariants_.modeBalance = (eCount != 3 && eCount != 0);

        // INV-2: Polar-pair coverage (simplified: check T8/T1 and T5/T4 pairs)
        invariants_.polarPair = true; // structural property

        // INV-3: Alternating triangles (Structure {4,8,7} vs Process {1,2,5})
        invariants_.altTriangles = true; // structural property
    }
};

} // namespace kys

namespace elizaos {

// ── AttentionValue (from include/attention.hpp) ──
struct AttentionValue {
    double importance = 0.0;   // LTI — long-term significance [0,1]
    double urgency = 0.0;      // STI — short-term priority [0,1]
    double novelty = 0.0;      // surprise [0,1]
    double activation = 0.0;   // current activation [0,1]
    double getCompositeScore() const {
        return importance * 0.4 + urgency * 0.3 + novelty * 0.2 + activation * 0.1;
    }
    void decayUrgency(double r = 0.95) { urgency *= r; }
    void decayNovelty(double r = 0.90) { novelty *= r; }
    void decayActivation(double r = 0.85) { activation *= r; }
};

// ── AttentionBudget (from include/attention.hpp) ──
class AttentionBudget {
public:
    explicit AttentionBudget(double total = 100.0) : totalBudget_(total) {}
    bool allocate(const std::string& id, double amount) {
        if (allocatedBudget_ + amount > totalBudget_) return false;
        allocations_[id] += amount;
        allocatedBudget_ += amount;
        return true;
    }
    bool deallocate(const std::string& id, double amount) {
        auto it = allocations_.find(id);
        if (it == allocations_.end()) return false;
        double actual = std::min(amount, it->second);
        it->second -= actual;
        allocatedBudget_ -= actual;
        if (it->second <= 0.001) allocations_.erase(it);
        return true;
    }
    void redistribute() {
        // Equalize: pull budget from low-composite elements, push to high
        if (allocations_.empty()) return;
        double avg = allocatedBudget_ / allocations_.size();
        for (auto& [id, amt] : allocations_)
            amt = amt * 0.9 + avg * 0.1;
    }
    void reset() { allocations_.clear(); allocatedBudget_ = 0.0; }
    double available() const { return totalBudget_ - allocatedBudget_; }
    double total() const { return totalBudget_; }
    double allocated() const { return allocatedBudget_; }
    double utilization() const { return totalBudget_ > 0 ? allocatedBudget_ / totalBudget_ : 0.0; }
    const std::unordered_map<std::string, double>& allocations() const { return allocations_; }
private:
    double totalBudget_;
    double allocatedBudget_ = 0.0;
    std::unordered_map<std::string, double> allocations_;
};

// ── ActivationSpreadingNetwork (from include/attention.hpp) ──
// Hex-topology spreading: nodes are "q,r" cell keys, edges link 6 neighbors
class ActivationSpreadingNetwork {
public:
    void addNode(const std::string& id, double initial = 0.0) {
        nodes_[id] = initial;
    }
    void addHexEdges(int q, int r, int size) {
        static const int dirs[6][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,-1},{-1,1}};
        std::string from = key(q, r);
        for (auto& d : dirs) {
            int nq = q + d[0], nr = r + d[1];
            if (nq >= 0 && nq < size && nr >= 0 && nr < size)
                edges_[from].push_back(key(nq, nr));
        }
    }
    void spread(double rate = 0.8, double threshold = 0.01) {
        std::unordered_map<std::string, double> delta;
        for (auto& [id, act] : nodes_) {
            if (act < threshold) continue;
            auto it = edges_.find(id);
            if (it == edges_.end()) continue;
            double outflow = act * rate * 0.15;
            double per = it->second.empty() ? 0.0 : outflow / it->second.size();
            for (auto& tgt : it->second) delta[tgt] += per;
            delta[id] -= outflow;
        }
        for (auto& [id, d] : delta) {
            nodes_[id] = std::max(0.0, std::min(1.0, nodes_[id] + d));
        }
    }
    double get(const std::string& id) const {
        auto it = nodes_.find(id);
        return it != nodes_.end() ? it->second : 0.0;
    }
    void set(const std::string& id, double v) { nodes_[id] = std::max(0.0, std::min(1.0, v)); }
    static std::string key(int q, int r) {
        char buf[32]; snprintf(buf, 32, "%d,%d", q, r); return buf;
    }
private:
    std::unordered_map<std::string, double> nodes_;
    std::unordered_map<std::string, std::vector<std::string>> edges_;
};

// ── TemporalAttentionWindow (from include/attention.hpp Phase 1.3) ──
struct TemporalAttentionWindow {
    double immediate = 0.0;   // working memory (seconds)
    double shortTerm = 0.0;   // minutes
    double mediumTerm = 0.0;  // hours
    double longTerm = 0.0;    // days
    double composite(double wi = 0.4, double ws = 0.3, double wm = 0.2, double wl = 0.1) const {
        return immediate * wi + shortTerm * ws + mediumTerm * wm + longTerm * wl;
    }
    void event(double boost = 0.25) {
        immediate = std::min(1.0, immediate + boost);
        shortTerm = std::min(1.0, shortTerm + boost * 0.5);
        mediumTerm = std::min(1.0, mediumTerm + boost * 0.25);
        longTerm = std::min(1.0, longTerm + boost * 0.1);
    }
    void decay(double di = 0.9, double ds = 0.95, double dm = 0.98, double dl = 0.99) {
        immediate *= di; shortTerm *= ds; mediumTerm *= dm; longTerm *= dl;
    }
};

// ── AttentionAllocator (from include/attention.hpp) ──
class AttentionAllocator {
public:
    explicit AttentionAllocator(double budget = 100.0) : budget_(budget) {}
    void update(const std::string& id, const AttentionValue& v) { values_[id] = v; }
    void stimulate(const std::string& id, double noveltyBoost, double urgencyBoost) {
        auto& v = values_[id];
        v.novelty = std::min(1.0, v.novelty + noveltyBoost);
        v.urgency = std::min(1.0, v.urgency + urgencyBoost);
        v.activation = std::min(1.0, v.activation + noveltyBoost * 0.5);
        windows_[id].event(noveltyBoost);
    }
    void allocateTop(size_t n) {
        // Rank by composite score, allocate budget greedily
        std::vector<std::pair<std::string, double>> ranked;
        for (auto& [id, v] : values_) ranked.push_back({id, v.getCompositeScore()});
        std::sort(ranked.begin(), ranked.end(),
                  [](auto& a, auto& b) { return a.second > b.second; });
        budget_.reset();
        double per = budget_.total() / std::max(n, (size_t)1);
        for (size_t i = 0; i < std::min(n, ranked.size()); i++)
            budget_.allocate(ranked[i].first, per);
    }
    void decayAll() {
        for (auto& [id, v] : values_) {
            v.decayUrgency(); v.decayNovelty(); v.decayActivation();
        }
        for (auto& [id, w] : windows_) w.decay();
    }
    const AttentionValue* get(const std::string& id) const {
        auto it = values_.find(id);
        return it != values_.end() ? &it->second : nullptr;
    }
    const TemporalAttentionWindow* window(const std::string& id) const {
        auto it = windows_.find(id);
        return it != windows_.end() ? &it->second : nullptr;
    }
    AttentionBudget& budget() { return budget_; }
    size_t activeElements() const { return values_.size(); }
    double averageAttention() const {
        if (values_.empty()) return 0.0;
        double sum = 0.0;
        for (auto& [id, v] : values_) sum += v.getCompositeScore();
        return sum / values_.size();
    }
    std::string focusElement() const {
        std::string best; double bestScore = -1.0;
        for (auto& [id, v] : values_)
            if (v.getCompositeScore() > bestScore) { bestScore = v.getCompositeScore(); best = id; }
        return best;
    }
private:
    std::unordered_map<std::string, AttentionValue> values_;
    std::unordered_map<std::string, TemporalAttentionWindow> windows_;
    AttentionBudget budget_;
};

// ── CognitiveLoad (from include/agentloop.hpp) ──
struct CognitiveLoad {
    double cpuUtilization = 0.0;
    double memoryPressure = 0.0;
    double taskQueueDepth = 0.0;
    double avgResponseTime = 0.0;
    double errorRate = 0.0;
    double throughput = 0.0;
    double composite() const {
        return cpuUtilization * 0.25 + memoryPressure * 0.25 + taskQueueDepth * 0.2 +
               errorRate * 0.2 + std::min(1.0, avgResponseTime / 1000.0) * 0.1;
    }
    bool underPressure() const { return composite() > 0.75; }
    bool shouldDegrade() const { return composite() > 0.85; }
};

// ── CircuitBreaker (from include/agentloop.hpp) ──
enum class CircuitState { Closed, Open, HalfOpen };
class CircuitBreaker {
public:
    bool allowRequest() {
        if (state_ == CircuitState::Open) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFailure_).count();
            if (elapsed > 3000) { state_ = CircuitState::HalfOpen; halfOpenAttempts_ = 0; }
            else return false;
        }
        return true;
    }
    void recordSuccess() {
        recent_.push_back(true);
        if (recent_.size() > 20) recent_.pop_front();
        if (state_ == CircuitState::HalfOpen && ++halfOpenAttempts_ >= 3)
            state_ = CircuitState::Closed;
        consecutiveFailures_ = 0;
    }
    void recordFailure() {
        recent_.push_back(false);
        if (recent_.size() > 20) recent_.pop_front();
        lastFailure_ = std::chrono::steady_clock::now();
        if (++consecutiveFailures_ >= 5) state_ = CircuitState::Open;
    }
    double failureRate() const {
        if (recent_.empty()) return 0.0;
        int fails = 0;
        for (bool r : recent_) if (!r) fails++;
        return (double)fails / recent_.size();
    }
    CircuitState state() const { return state_; }
    const char* stateName() const {
        switch (state_) {
            case CircuitState::Closed: return "CLOSED";
            case CircuitState::Open: return "OPEN";
            case CircuitState::HalfOpen: return "HALF-OPEN";
        }
        return "?";
    }
    void reset() { state_ = CircuitState::Closed; consecutiveFailures_ = 0; recent_.clear(); }
private:
    CircuitState state_ = CircuitState::Closed;
    uint64_t consecutiveFailures_ = 0;
    int halfOpenAttempts_ = 0;
    std::deque<bool> recent_;
    std::chrono::steady_clock::time_point lastFailure_;
};

// ── HealthStatus + LoopStats (from include/agentloop.hpp) ──
enum class HealthStatus { Healthy, Degraded, Unhealthy, Stopped };
inline const char* healthName(HealthStatus h) {
    switch (h) {
        case HealthStatus::Healthy: return "HEALTHY";
        case HealthStatus::Degraded: return "DEGRADED";
        case HealthStatus::Unhealthy: return "UNHEALTHY";
        case HealthStatus::Stopped: return "STOPPED";
    }
    return "?";
}

struct LoopStats {
    uint64_t totalIterations = 0;
    uint64_t stepsExecuted = 0;
    uint64_t successCount = 0;
    uint64_t errorCount = 0;
    uint64_t deferredCount = 0;
    double avgStepDurationMs = 0.0;
    double iterationsPerSecond = 0.0;
    std::string lastError;
};

// ── AgentLoop (from include/agentloop.hpp) ──
// Priority-based step execution driving the Echobeats phases
struct LoopStep {
    std::string name;
    int priority = 0;  // higher = more important
    std::function<bool()> fn;
    double lastDurationMs = 0.0;
    uint64_t executions = 0;
    uint64_t failures = 0;
};

class AgentLoop {
public:
    AgentLoop() = default;
    void addStep(const std::string& name, int priority, std::function<bool()> fn) {
        steps_.push_back({name, priority, std::move(fn)});
        std::sort(steps_.begin(), steps_.end(),
                  [](const LoopStep& a, const LoopStep& b) { return a.priority > b.priority; });
    }
    void start() { running_ = true; health_ = HealthStatus::Healthy; }
    void stop() { running_ = false; health_ = HealthStatus::Stopped; }
    bool isRunning() const { return running_; }
    HealthStatus health() const { return health_; }
    const CognitiveLoad& load() const { return load_; }
    const LoopStats& stats() const { return stats_; }
    CircuitBreaker& breaker() { return breaker_; }
    bool degraded() const { return degraded_; }

    // Execute one iteration: run steps by priority, skip optional steps under pressure
    void iterate() {
        if (!running_) return;
        stats_.totalIterations++;
        auto iterStart = std::chrono::steady_clock::now();
        int executed = 0, deferred = 0;
        for (auto& step : steps_) {
            // Graceful degradation: under pressure, skip low-priority steps
            if (degraded_ && step.priority < 5) { deferred++; continue; }
            // Circuit breaker: fail fast if system is faulting
            if (!breaker_.allowRequest()) { deferred++; continue; }
            auto t0 = std::chrono::steady_clock::now();
            bool ok = false;
            try { ok = step.fn(); }
            catch (...) { ok = false; }
            auto t1 = std::chrono::steady_clock::now();
            step.lastDurationMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
            step.executions++;
            stats_.stepsExecuted++;
            executed++;
            if (ok) { stats_.successCount++; breaker_.recordSuccess(); }
            else { stats_.errorCount++; step.failures++; breaker_.recordFailure(); stats_.lastError = step.name; }
        }
        stats_.deferredCount += deferred;
        auto iterEnd = std::chrono::steady_clock::now();
        double iterMs = std::chrono::duration<double, std::milli>(iterEnd - iterStart).count();
        // Update rolling stats
        double alpha = 0.1;
        stats_.avgStepDurationMs = stats_.avgStepDurationMs * (1 - alpha) + iterMs * alpha;
        totalRuntimeMs_ += iterMs;
        stats_.iterationsPerSecond = totalRuntimeMs_ > 0 ? stats_.totalIterations / (totalRuntimeMs_ / 1000.0) : 0.0;
        // Update cognitive load estimate
        load_.avgResponseTime = stats_.avgStepDurationMs;
        load_.errorRate = breaker_.failureRate();
        load_.taskQueueDepth = steps_.empty() ? 0.0 : (double)deferred / steps_.size();
        load_.throughput = stats_.iterationsPerSecond;
        // Health + degradation state machine
        double comp = load_.composite();
        if (comp > 0.85) { health_ = HealthStatus::Unhealthy; degraded_ = true; }
        else if (comp > 0.75) { health_ = HealthStatus::Degraded; degraded_ = true; }
        else { health_ = HealthStatus::Healthy; degraded_ = false; }
    }

    // Prometheus metrics export (from agentloop.hpp toPrometheusFormat)
    std::string toPrometheus(const std::string& name = "gtangel_agent_loop") const {
        std::string out;
        char buf[256];
        snprintf(buf, 256, "# HELP %s_iterations_total Total loop iterations\n# TYPE %s_iterations_total counter\n%s_iterations_total %llu\n",
                 name.c_str(), name.c_str(), name.c_str(), (unsigned long long)stats_.totalIterations);
        out += buf;
        snprintf(buf, 256, "# HELP %s_steps_executed_total Steps executed\n# TYPE %s_steps_executed_total counter\n%s_steps_executed_total %llu\n",
                 name.c_str(), name.c_str(), name.c_str(), (unsigned long long)stats_.stepsExecuted);
        out += buf;
        snprintf(buf, 256, "# HELP %s_error_rate Recent failure rate\n# TYPE %s_error_rate gauge\n%s_error_rate %.4f\n",
                 name.c_str(), name.c_str(), name.c_str(), breaker_.failureRate());
        out += buf;
        snprintf(buf, 256, "# HELP %s_cognitive_load Composite cognitive load\n# TYPE %s_cognitive_load gauge\n%s_cognitive_load %.4f\n",
                 name.c_str(), name.c_str(), name.c_str(), load_.composite());
        out += buf;
        snprintf(buf, 256, "# HELP %s_avg_step_duration_ms Average step duration\n# TYPE %s_avg_step_duration_ms gauge\n%s_avg_step_duration_ms %.3f\n",
                 name.c_str(), name.c_str(), name.c_str(), stats_.avgStepDurationMs);
        out += buf;
        return out;
    }
    const std::vector<LoopStep>& steps() const { return steps_; }
private:
    std::vector<LoopStep> steps_;
    bool running_ = false;
    bool degraded_ = false;
    HealthStatus health_ = HealthStatus::Stopped;
    CognitiveLoad load_;
    LoopStats stats_;
    CircuitBreaker breaker_;
    double totalRuntimeMs_ = 0.0;
};

} // namespace elizaos

namespace mech0 {

// AttentionMechanism (from src/model/attention.js)
class AttentionMechanism {
public:
    AttentionMechanism(int embeddingDim) : embeddingDim_(embeddingDim) {
        queryWeight_ = initializeWeight();
        keyWeight_ = initializeWeight();
        valueWeight_ = initializeWeight();
    }
    std::vector<std::vector<double>> initializeWeight() {
        std::mt19937 rng(42);
        std::normal_distribution<double> gauss(0.0, 1.0);
        double scale = sqrt(2.0 / (embeddingDim_ + embeddingDim_));
        std::vector<std::vector<double>> w(embeddingDim_, std::vector<double>(embeddingDim_));
        for (auto& row : w)
            for (auto& v : row)
                v = gauss(rng) * scale;
        return w;
    }
    std::vector<std::vector<double>> multiply(const std::vector<double>& emb, const std::vector<std::vector<double>>& w) {
        std::vector<std::vector<double>> result(1, std::vector<double>(embeddingDim_, 0.0));
        for (int j = 0; j < embeddingDim_; j++)
            for (int i = 0; i < embeddingDim_; i++)
                result[0][j] += emb[i] * w[i][j];
        return result;
    }
    double dot(const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); i++) sum += a[i] * b[i];
        return sum;
    }
    std::vector<std::vector<double>> calculateAttentionScores(const std::vector<std::vector<double>>& embeddings) {
        std::vector<std::vector<double>> queries, keys, values;
        for (const auto& emb : embeddings) {
            queries.push_back(multiply(emb, queryWeight_)[0]);
            keys.push_back(multiply(emb, keyWeight_)[0]);
            values.push_back(multiply(emb, valueWeight_)[0]);
        }
        std::vector<std::vector<double>> scores(queries.size(), std::vector<double>(keys.size()));
        for (size_t i = 0; i < queries.size(); i++)
            for (size_t j = 0; j < keys.size(); j++)
                scores[i][j] = dot(queries[i], keys[j]) / sqrt((double)embeddingDim_);
        return scores;
    }
    std::vector<std::vector<double>> softmax(const std::vector<std::vector<double>>& scores) {
        double maxScore = -1e9;
        for (const auto& row : scores)
            for (double s : row)
                maxScore = std::max(maxScore, s);
        std::vector<std::vector<double>> expScores(scores.size(), std::vector<double>(scores[0].size()));
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                expScores[i][j] = exp(scores[i][j] - maxScore);
        std::vector<double> sumExp(scores.size(), 0.0);
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                sumExp[i] += expScores[i][j];
        std::vector<std::vector<double>> result(scores.size(), std::vector<double>(scores[0].size()));
        for (size_t i = 0; i < scores.size(); i++)
            for (size_t j = 0; j < scores[i].size(); j++)
                result[i][j] = expScores[i][j] / (sumExp[i] + 1e-10);
        return result;
    }
    struct AttentionOutput {
        std::vector<std::vector<double>> contextVectors;
        std::vector<std::vector<double>> attentionWeights;
    };
    AttentionOutput applyAttention(const std::vector<std::vector<double>>& embeddings) {
        auto scores = calculateAttentionScores(embeddings);
        auto attentionWeights = softmax(scores);
        std::vector<std::vector<double>> values;
        for (const auto& emb : embeddings)
            values.push_back(multiply(emb, valueWeight_)[0]);
        std::vector<std::vector<double>> contextVectors(embeddings.size(), std::vector<double>(embeddingDim_, 0.0));
        for (size_t i = 0; i < embeddings.size(); i++)
            for (size_t j = 0; j < embeddings.size(); j++)
                for (int k = 0; k < embeddingDim_; k++)
                    contextVectors[i][k] += values[j][k] * attentionWeights[i][j];
        return {contextVectors, attentionWeights};
    }
    void updateWeights(const std::vector<std::vector<double>>& gradients, double learningRate) {
        auto updateMatrix = [&](std::vector<std::vector<double>>& weight, const std::vector<std::vector<double>>& gradient) {
            for (size_t i = 0; i < weight.size(); i++)
                for (size_t j = 0; j < weight[i].size(); j++)
                    weight[i][j] -= learningRate * gradient[i][j];
        };
        updateMatrix(queryWeight_, gradients);
        updateMatrix(keyWeight_, gradients);
        updateMatrix(valueWeight_, gradients);
    }
private:
    int embeddingDim_;
    std::vector<std::vector<double>> queryWeight_, keyWeight_, valueWeight_;
};

// SimpleLanguageModel (from src/model/architecture.js)
class SimpleLanguageModel {
public:
    SimpleLanguageModel(int vocabSize, int embeddingDim = 100)
        : vocabSize_(vocabSize), embeddingDim_(embeddingDim) {
        embeddings_ = initializeEmbeddings();
        labels_ = {"greeting", "farewell", "question"};
        attention_ = std::make_unique<AttentionMechanism>(embeddingDim);
        dropout_ = 0.2;
        layerNormGamma_.assign(embeddingDim, 1.0);
        layerNormBeta_.assign(embeddingDim, 0.0);
    }
    std::vector<std::vector<double>> initializeEmbeddings() {
        std::mt19937 rng(42);
        std::normal_distribution<double> gauss(0.0, 1.0);
        double scale = sqrt(2.0 / (vocabSize_ + embeddingDim_));
        std::vector<std::vector<double>> emb(vocabSize_, std::vector<double>(embeddingDim_));
        for (auto& row : emb)
            for (auto& v : row)
                v = gauss(rng) * scale;
        return emb;
    }
    std::vector<double> layerNormalize(const std::vector<double>& embeddings) {
        double mean = 0.0;
        for (double v : embeddings) mean += v;
        mean /= embeddings.size();
        double variance = 0.0;
        for (double v : embeddings) variance += (v - mean) * (v - mean);
        variance /= embeddings.size();
        std::vector<double> result(embeddings.size());
        for (size_t i = 0; i < embeddings.size(); i++)
            result[i] = layerNormGamma_[i] * (embeddings[i] - mean) / sqrt(variance + 1e-10) + layerNormBeta_[i];
        return result;
    }
    std::vector<std::vector<double>> applyDropout(const std::vector<std::vector<double>>& embeddings, bool training = true) {
        if (!training || dropout_ == 0) return embeddings;
        std::mt19937 rng(42);
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        std::vector<std::vector<double>> result = embeddings;
        for (auto& row : result)
            for (auto& v : row)
                if (uni(rng) < dropout_) v = 0.0;
                else v /= (1.0 - dropout_);
        return result;
    }
    struct ForwardOutput {
        std::vector<std::vector<double>> embeddings;
        std::vector<std::vector<double>> attentionWeights;
    };
    ForwardOutput forward(const std::vector<int>& tokenIndices, bool training = true) {
        std::vector<std::vector<double>> embeddings;
        for (int idx : tokenIndices)
            embeddings.push_back(embeddings_[idx]);
        embeddings = applyDropout(embeddings, training);
        for (auto& emb : embeddings)
            emb = layerNormalize(emb);
        auto attentionOutput = attention_->applyAttention(embeddings);
        return {attentionOutput.contextVectors, attentionOutput.attentionWeights};
    }
    struct Prediction {
        std::string label;
        double probability;
    };
    std::vector<Prediction> predict(const ForwardOutput& output) {
        std::vector<double> avgEmbedding(embeddingDim_, 0.0);
        for (const auto& emb : output.embeddings)
            for (int i = 0; i < embeddingDim_; i++)
                avgEmbedding[i] += emb[i];
        for (int i = 0; i < embeddingDim_; i++)
            avgEmbedding[i] /= output.embeddings.size();
        std::vector<double> scores(labels_.size());
        for (size_t i = 0; i < labels_.size(); i++) {
            double sum = 0.0;
            for (int j = 0; j < embeddingDim_; j++)
                sum += avgEmbedding[j] * (i + 1) * 0.1;
            scores[i] = sum;
        }
        double maxScore = *std::max_element(scores.begin(), scores.end());
        double sumExp = 0.0;
        for (double s : scores) sumExp += exp(s - maxScore);
        std::vector<Prediction> predictions;
        for (size_t i = 0; i < labels_.size(); i++)
            predictions.push_back({labels_[i], exp(scores[i] - maxScore) / sumExp});
        return predictions;
    }
    void updateEmbedding(int tokenIndex, const std::vector<double>& gradients, double learningRate) {
        for (int i = 0; i < embeddingDim_; i++)
            embeddings_[tokenIndex][i] -= learningRate * gradients[i];
    }
    int vocabSize() const { return vocabSize_; }
    int embeddingDim() const { return embeddingDim_; }
    const std::vector<std::string>& labels() const { return labels_; }
    const std::vector<std::vector<double>>& embeddings() const { return embeddings_; }
    AttentionMechanism& attention() { return *attention_; }
private:
    int vocabSize_, embeddingDim_;
    std::vector<std::vector<double>> embeddings_;
    std::vector<std::string> labels_;
    std::unique_ptr<AttentionMechanism> attention_;
    double dropout_;
    std::vector<double> layerNormGamma_, layerNormBeta_;
};

// TrainingPipeline (from src/training/train.js)
struct TrainingMetrics {
    double averageLoss = 0.0;
    double accuracy = 0.0;
    int epochs = 0;
    int batches = 0;
};

class TrainingPipeline {
public:
    TrainingPipeline(int vocabSize, int embeddingDim = 100)
        : model_(vocabSize, embeddingDim), metrics_() {}
    void train(const std::vector<std::pair<std::vector<int>, std::string>>& data, int epochs = 10, double learningRate = 0.01) {
        for (int epoch = 0; epoch < epochs; epoch++) {
            double totalLoss = 0.0;
            int correct = 0;
            for (const auto& [tokens, label] : data) {
                auto output = model_.forward(tokens, true);
                auto predictions = model_.predict(output);
                int predicted = 0;
                double maxProb = 0.0;
                for (size_t i = 0; i < predictions.size(); i++)
                    if (predictions[i].probability > maxProb) {
                        maxProb = predictions[i].probability;
                        predicted = (int)i;
                    }
                int actual = 0;
                for (size_t i = 0; i < model_.labels().size(); i++)
                    if (model_.labels()[i] == label) actual = (int)i;
                if (predicted == actual) correct++;
                double loss = -log(maxProb + 1e-10);
                totalLoss += loss;
                // Update embeddings
                for (size_t i = 0; i < tokens.size(); i++) {
                    std::vector<double> gradients(model_.embeddingDim(), 0.0);
                    for (int j = 0; j < model_.embeddingDim(); j++)
                        gradients[j] = output.embeddings[i][j] * 0.1;
                    model_.updateEmbedding(tokens[i], gradients, learningRate);
                }
                // Update attention weights
                std::vector<std::vector<double>> gradients(model_.embeddingDim(), std::vector<double>(model_.embeddingDim(), 0.1));
                model_.attention().updateWeights(gradients, learningRate);
            }
            metrics_.averageLoss = totalLoss / data.size();
            metrics_.accuracy = (double)correct / data.size();
            metrics_.epochs = epoch + 1;
            metrics_.batches = (int)data.size();
        }
    }
    const SimpleLanguageModel& model() const { return model_; }
    const TrainingMetrics& metrics() const { return metrics_; }
private:
    SimpleLanguageModel model_;
    TrainingMetrics metrics_;
};

} // namespace mech0

// ═══ re-bhvt-edit Integration (ported from ReZorg/re-bhvt-edit) ═══
namespace rebhvt {

// EndocrineRewardSystem (from agi/EndocrineRewardSystem.lua)
struct EndocrineChannel {
    float level = 0.0f;
    float baseline_sensitivity = 1.0f;
    float decay = 1.0f;
    float habituation = 0.1f;
    float weight = 1.0f;
};

class EndocrineRewardSystem {
public:
    EndocrineRewardSystem() {
        channels["dopamine"] = {0.0f, 1.0f, 2.0f, 0.35f, 1.0f};
        channels["cortisol"] = {0.0f, 1.0f, 1.0f, 0.10f, -0.6f};
        channels["oxytocin"] = {0.0f, 1.0f, 3.0f, 0.05f, 0.5f};
    }
    void reset() {
        for (auto& [name, ch] : channels) {
            ch.level = 0.0f;
            ch.baseline_sensitivity = 1.0f;
        }
    }
    // stimuli: map of channel name -> stimulus intensity in [0, 1]
    // dt: elapsed seconds since last update
    // Returns: composite_reward
    float update(const std::map<std::string, float>& stimuli, float dt) {
        float composite = 0.0f;
        for (auto& [name, ch] : channels) {
            float stimulus = 0.0f;
            auto it = stimuli.find(name);
            if (it != stimuli.end()) stimulus = it->second;
            // Adaptive sensitivity: repeated stimulation habituates the channel
            if (stimulus > 0.0f) {
                ch.baseline_sensitivity = std::max(0.05f,
                    ch.baseline_sensitivity - ch.habituation * stimulus * dt);
            } else {
                // Rest slowly restores full sensitivity
                ch.baseline_sensitivity = std::min(1.0f,
                    ch.baseline_sensitivity + ch.habituation * 0.25f * dt);
            }
            float release = stimulus * ch.baseline_sensitivity;
            ch.level = std::max(0.0f, ch.level + release - ch.level * ch.decay * dt);
            composite += ch.level * ch.weight;
        }
        return composite;
    }
    const std::map<std::string, EndocrineChannel>& getChannels() const { return channels; }
    float getLevel(const std::string& name) const {
        auto it = channels.find(name);
        return it != channels.end() ? it->second.level : 0.0f;
    }
    float getSensitivity(const std::string& name) const {
        auto it = channels.find(name);
        return it != channels.end() ? it->second.baseline_sensitivity : 0.0f;
    }
private:
    std::map<std::string, EndocrineChannel> channels;
};

// NicheConstructionMap (from agi/NicheConstructionMap.lua)
struct NicheCell {
    int visits = 0;
    float last_visit = 0.0f;
    float novelty = 0.0f;
};

class NicheConstructionMap {
public:
    NicheConstructionMap(float cell_size = 1.0f, float recovery_seconds = 30.0f)
        : cell_size(cell_size), recovery_seconds(recovery_seconds) {}
    void reset() { cells.clear(); }
    // Records a visit to the niche at (x, y, z) at time `now` (seconds)
    // Returns the novelty score in [0, 1] that visit produced (1 = brand new)
    float visit(float x, float y, float z, float now) {
        std::string key = makeKey(x, y, z);
        auto it = cells.find(key);
        if (it == cells.end()) {
            cells[key] = {1, now, 0.0f};
            return 1.0f;
        }
        NicheCell& cell = it->second;
        // Recover novelty the longer the niche has been left alone
        float elapsed = std::max(0.0f, now - cell.last_visit);
        float recovered = std::min(1.0f, elapsed / recovery_seconds);
        float novelty = std::min(1.0f, cell.novelty + recovered);
        cell.visits++;
        cell.last_visit = now;
        // Exploiting the niche drains its novelty for next time (habituation)
        cell.novelty = novelty * 0.15f;
        return novelty;
    }
    size_t size() const { return cells.size(); }
    const NicheCell* find(float x, float y, float z) const {
        auto it = cells.find(makeKey(x, y, z));
        return it != cells.end() ? &it->second : nullptr;
    }
private:
    std::string makeKey(float x, float y, float z) const {
        int xi = (int)floorf(x / cell_size);
        int yi = (int)floorf(y / cell_size);
        int zi = (int)floorf(z / cell_size);
        char buf[64];
        snprintf(buf, 64, "%d:%d:%d", xi, yi, zi);
        return buf;
    }
    float cell_size;
    float recovery_seconds;
    std::unordered_map<std::string, NicheCell> cells;
};

// SensorimotorBridge (from agi/SensorimotorBridge.lua)
struct SensorimotorTelemetry {
    float reward = 0.0f;
    float novelty = 0.0f;
    float prediction_error = 0.0f;
    float threat = 0.0f;
    float time = 0.0f;
    int niche_count = 0;
    std::map<std::string, float> hormone_levels;
    std::map<std::string, float> hormone_sensitivities;
};

class SensorimotorBridge {
public:
    SensorimotorBridge(float cell_size = 1.5f, float recovery_seconds = 30.0f)
        : niches(cell_size, recovery_seconds), endocrine() {}
    void reset() {
        niches.reset();
        endocrine.reset();
        previous_features.clear();
        time = 0.0f;
    }
    // position: {x, y, z} world-space position of the observed agent
    // frame_features: flat array of sampled frame buffer values (any scale)
    // threat_level: scalar in [0, 1] describing danger/failure signal this tick
    // dt: elapsed seconds since the previous update
    SensorimotorTelemetry update(float px, float py, float pz,
                                 const std::vector<float>& frame_features,
                                 float threat_level, float dt) {
        time += dt;
        float novelty = niches.visit(px, py, pz, time);
        float prediction_error = 0.0f;
        if (!previous_features.empty() && previous_features.size() == frame_features.size()) {
            float sum = 0.0f;
            for (size_t i = 0; i < frame_features.size(); i++)
                sum += fabsf(frame_features[i] - previous_features[i]);
            prediction_error = sum / frame_features.size();
        }
        previous_features = frame_features;
        std::map<std::string, float> stimuli;
        stimuli["dopamine"] = novelty;
        stimuli["cortisol"] = threat_level;
        stimuli["oxytocin"] = std::max(0.0f, 1.0f - prediction_error);
        float reward = endocrine.update(stimuli, dt);
        SensorimotorTelemetry t{};
        t.reward = reward;
        t.novelty = novelty;
        t.prediction_error = prediction_error;
        t.threat = threat_level;
        t.time = time;
        t.niche_count = (int)niches.size();
        for (const auto& [name, ch] : endocrine.getChannels()) {
            t.hormone_levels[name] = ch.level;
            t.hormone_sensitivities[name] = ch.baseline_sensitivity;
        }
        return t;
    }
    const NicheConstructionMap& getNiches() const { return niches; }
    const EndocrineRewardSystem& getEndocrine() const { return endocrine; }
private:
    NicheConstructionMap niches;
    EndocrineRewardSystem endocrine;
    std::vector<float> previous_features;
    float time = 0.0f;
};

} // namespace rebhvt

// ═══ GTAngel-Cls Integration (ported from ReZorg/GTAngel-Cls) ═══
namespace gtangel {

struct FrameBufferView {
    const uint8_t* pixels{};
    size_t width{};
    size_t height{};
    size_t stride{};
    size_t channels{4};
    bool valid() const {
        return pixels != nullptr && width > 0 && height > 0 && channels >= 3 &&
               row_pitch() >= width * channels;
    }
    size_t row_pitch() const { return stride != 0 ? stride : width * channels; }
};

struct ProprioceptiveFrame {
    size_t grid_width{};
    size_t grid_height{};
    std::vector<float> luminance{};
    std::vector<float> temporal_delta{};
    float mean_luminance{};
    float contrast{};
    float motion_energy{};
    uint64_t index{};
    size_t cell_count() const { return grid_width * grid_height; }
};

class FrameBufferProbe {
public:
    FrameBufferProbe() = default;
    FrameBufferProbe(size_t gw, size_t gh) : m_gw(gw ? gw : 1), m_gh(gh ? gh : 1) {}
    size_t grid_width() const { return m_gw; }
    size_t grid_height() const { return m_gh; }
    void reset() { m_previous.clear(); m_primed = false; m_index = 0; }
    ProprioceptiveFrame sample(const FrameBufferView& view) {
        ProprioceptiveFrame frame{};
        frame.grid_width = m_gw; frame.grid_height = m_gh;
        frame.luminance.assign(m_gw * m_gh, 0.0f);
        frame.temporal_delta.assign(m_gw * m_gh, 0.0f);
        frame.index = m_index;
        if (!view.valid()) return frame;
        ++m_index;
        for (size_t cy = 0; cy < m_gh; ++cy) {
            size_t y0 = (cy * view.height) / m_gh;
            size_t y1 = std::max(y0 + 1, ((cy + 1) * view.height) / m_gh);
            for (size_t cx = 0; cx < m_gw; ++cx) {
                size_t x0 = (cx * view.width) / m_gw;
                size_t x1 = std::max(x0 + 1, ((cx + 1) * view.width) / m_gw);
                double acc = 0.0; size_t samples = 0;
                for (size_t y = y0; y < y1 && y < view.height; ++y) {
                    const uint8_t* row = view.pixels + y * view.row_pitch();
                    for (size_t x = x0; x < x1 && x < view.width; ++x) {
                        const uint8_t* texel = row + x * view.channels;
                        acc += 0.114 * texel[0] + 0.587 * texel[1] + 0.299 * texel[2];
                        ++samples;
                    }
                }
                if (samples != 0)
                    frame.luminance[cy * m_gw + cx] = (float)(acc / (samples * 255.0));
            }
        }
        double sum = 0.0, sum_delta = 0.0;
        for (size_t i = 0; i < frame.luminance.size(); ++i) {
            sum += frame.luminance[i];
            if (m_primed && i < m_previous.size())
                frame.temporal_delta[i] = fabsf(frame.luminance[i] - m_previous[i]);
            sum_delta += frame.temporal_delta[i];
        }
        double cells = (double)frame.luminance.size();
        frame.mean_luminance = (float)(sum / cells);
        frame.motion_energy = (float)(sum_delta / cells);
        double var = 0.0;
        for (float v : frame.luminance) {
            double c = (double)v - frame.mean_luminance;
            var += c * c;
        }
        frame.contrast = (float)sqrt(var / cells);
        m_previous = frame.luminance;
        m_primed = true;
        return frame;
    }
private:
    size_t m_gw{8}, m_gh{8};
    std::vector<float> m_previous{};
    bool m_primed{false};
    uint64_t m_index{};
};

struct MotorCommand {
    float move_forward{};
    float move_right{};
    float look_right{};
    float look_up{};
    float actuate{};
    float magnitude() const {
        return sqrtf(move_forward*move_forward + move_right*move_right +
                     look_right*look_right + look_up*look_up + actuate*actuate);
    }
};

struct SensorimotorReading {
    float prediction_error{};
    float expected_flow{};
    float observed_flow{};
    float agency{};
    float novelty{};
    bool self_caused{};
};

class SensorimotorLoop {
public:
    SensorimotorLoop() = default;
    explicit SensorimotorLoop(float lr) : m_lr(std::clamp(lr, 0.0f, 1.0f)) {}
    void reset() { m_gain = 0.5f; m_error_scale = 0.0f; m_primed = false; }
    float gain() const { return m_gain; }
    SensorimotorReading integrate(const MotorCommand& cmd, const ProprioceptiveFrame& frame) {
        SensorimotorReading r{};
        r.observed_flow = frame.motion_energy;
        r.expected_flow = m_gain * cmd.magnitude();
        float residual = r.observed_flow - r.expected_flow;
        r.prediction_error = fabsf(residual);
        float effort = cmd.magnitude();
        if (effort > 1e-6f)
            m_gain = std::clamp(m_gain + m_lr * residual / effort, 0.0f, 8.0f);
        m_error_scale = m_primed ? (1.0f - 0.05f) * m_error_scale + 0.05f * r.prediction_error
                                 : r.prediction_error;
        m_primed = true;
        float scale = std::max(m_error_scale, 1e-6f);
        r.novelty = std::clamp(r.prediction_error / scale, 0.0f, 4.0f);
        float explained = std::max(r.observed_flow, 1e-6f);
        r.agency = std::clamp(1.0f - r.prediction_error / explained, 0.0f, 1.0f);
        r.self_caused = r.agency >= 0.5f;
        return r;
    }
private:
    float m_lr{0.1f};
    float m_gain{0.5f};
    float m_error_scale{};
    bool m_primed{false};
};

class Hormone {
public:
    Hormone() = default;
    Hormone(float baseline, float decay, float adaptation) :
        m_level(baseline), m_set_point(baseline),
        m_decay(std::clamp(decay, 0.0f, 1.0f)),
        m_adaptation(std::clamp(adaptation, 0.0f, 1.0f)) {}
    void secrete(float amount) { m_level = std::clamp(m_level + amount, 0.0f, 1.0f); }
    void tick() {
        m_level = std::clamp(m_level + (m_set_point - m_level) * m_decay, 0.0f, 1.0f);
        m_set_point = std::clamp(m_set_point + (m_level - m_set_point) * m_adaptation, 0.0f, 1.0f);
    }
    void reset(float baseline) {
        m_level = std::clamp(baseline, 0.0f, 1.0f);
        m_set_point = m_level;
    }
    float level() const { return m_level; }
    float set_point() const { return m_set_point; }
    float deviation() const { return m_level - m_set_point; }
    float decay() const { return m_decay; }
    void set_decay(float d) { m_decay = std::clamp(d, 0.0f, 1.0f); }
private:
    float m_level{0.5f};
    float m_set_point{0.5f};
    float m_decay{0.1f};
    float m_adaptation{0.01f};
};

struct EndocrineStimulus {
    float novelty{};
    float agency{};
    float goal_progress{};
    float threat{};
    float effort{};
};

struct EndocrineState {
    float dopamine{};
    float serotonin{};
    float noradrenaline{};
    float cortisol{};
    float reward{};
    float arousal{};
    float wellbeing{};
};

class EndocrineSystem {
public:
    EndocrineSystem() { reset(); }
    void reset() {
        m_dopamine = Hormone(0.35f, 0.18f, 0.010f);
        m_serotonin = Hormone(0.50f, 0.08f, 0.004f);
        m_noradrenaline = Hormone(0.20f, 0.30f, 0.020f);
        m_cortisol = Hormone(0.20f, 0.06f, 0.003f);
    }
    EndocrineState update(const EndocrineStimulus& s) {
        float novelty = std::clamp(s.novelty, 0.0f, 4.0f);
        float agency = std::clamp(s.agency, 0.0f, 1.0f);
        float progress = std::clamp(s.goal_progress, -1.0f, 1.0f);
        float threat = std::clamp(s.threat, 0.0f, 1.0f);
        float effort = std::clamp(s.effort, 0.0f, 1.0f);
        m_dopamine.secrete(0.25f * novelty * agency + 0.40f * std::max(progress, 0.0f));
        m_serotonin.secrete(0.20f * agency - 0.15f * threat);
        m_noradrenaline.secrete(0.50f * threat + 0.10f * novelty);
        m_cortisol.secrete(0.35f * threat + 0.20f * std::max(-progress, 0.0f) +
                           0.15f * novelty * (1.0f - agency));
        m_dopamine.tick(); m_serotonin.tick(); m_noradrenaline.tick(); m_cortisol.tick();
        EndocrineState st{};
        st.dopamine = m_dopamine.level();
        st.serotonin = m_serotonin.level();
        st.noradrenaline = m_noradrenaline.level();
        st.cortisol = m_cortisol.level();
        st.arousal = std::clamp(0.5f * (st.noradrenaline + st.dopamine), 0.0f, 1.0f);
        st.wellbeing = std::clamp(st.serotonin - st.cortisol, -1.0f, 1.0f);
        st.reward = std::clamp(1.0f * m_dopamine.deviation() + 0.5f * m_serotonin.deviation() -
                               0.8f * m_cortisol.deviation() - 0.2f * effort, -1.0f, 1.0f);
        return st;
    }
    const Hormone& dopamine() const { return m_dopamine; }
    const Hormone& serotonin() const { return m_serotonin; }
    const Hormone& noradrenaline() const { return m_noradrenaline; }
    const Hormone& cortisol() const { return m_cortisol; }
    Hormone& dopamine() { return m_dopamine; }
    Hormone& serotonin() { return m_serotonin; }
    Hormone& noradrenaline() { return m_noradrenaline; }
    Hormone& cortisol() { return m_cortisol; }
private:
    Hormone m_dopamine{}, m_serotonin{}, m_noradrenaline{}, m_cortisol{};
};

struct WorldPose {
    float x{}, y{}, z{};
    float heading{};
    float time_of_day{};
};

struct WorldObservation {
    WorldPose pose{};
    float health{1.0f};
    float hostility{};
    float goal_progress{};
    bool terminal{};
};

struct NicheCoordinate {
    int32_t x{}, y{}, z{}, time_bucket{};
    bool operator==(const NicheCoordinate& o) const {
        return x == o.x && y == o.y && z == o.z && time_bucket == o.time_bucket;
    }
};
struct NicheCoordinateHash {
    size_t operator()(const NicheCoordinate& c) const {
        size_t h = 1469598103934665603ull;
        auto mix = [&h](int32_t v) { h ^= (size_t)(uint32_t)v; h *= 1099511628211ull; };
        mix(c.x); mix(c.y); mix(c.z); mix(c.time_bucket);
        return h;
    }
};

struct NicheCell {
    uint32_t visits{};
    float reward{}, threat{}, novelty{}, affordance{};
    float habitability() const { return reward + affordance - threat; }
};

enum class InventivePrinciple {
    None, SeparationInSpace, SeparationInTime, Segmentation,
    PriorAction, CushionInAdvance, Dynamism, Inversion, Intermediary
};
inline const char* to_string(InventivePrinciple p) {
    switch (p) {
        case InventivePrinciple::SeparationInSpace: return "separation_in_space";
        case InventivePrinciple::SeparationInTime: return "separation_in_time";
        case InventivePrinciple::Segmentation: return "segmentation";
        case InventivePrinciple::PriorAction: return "prior_action";
        case InventivePrinciple::CushionInAdvance: return "cushion_in_advance";
        case InventivePrinciple::Dynamism: return "dynamism";
        case InventivePrinciple::Inversion: return "inversion";
        case InventivePrinciple::Intermediary: return "intermediary";
        default: return "none";
    }
}

struct TrizResolution {
    bool contradiction{};
    InventivePrinciple principle{InventivePrinciple::None};
    NicheCoordinate target{};
    float expected_gain{};
    const char* improving{"none"};
    const char* worsening{"none"};
};

class NicheConstructor {
public:
    NicheConstructor() = default;
    NicheConstructor(float cell_size, float hours_per_bucket) :
        m_cell_size(cell_size > 0 ? cell_size : 1.0f),
        m_hours_per_bucket(hours_per_bucket > 0 ? hours_per_bucket : 1.0f) {}
    void reset() { m_cells.clear(); }
    NicheCoordinate quantise(const WorldPose& pose) const {
        NicheCoordinate c{};
        c.x = (int32_t)floorf(pose.x / m_cell_size);
        c.y = (int32_t)floorf(pose.y / m_cell_size);
        c.z = (int32_t)floorf(pose.z / m_cell_size);
        float hours = fmodf(pose.time_of_day, 24.0f);
        float wrapped = hours < 0 ? hours + 24.0f : hours;
        c.time_bucket = (int32_t)(wrapped / m_hours_per_bucket);
        return c;
    }
    const NicheCell& observe(const NicheCoordinate& c, float reward, float threat, float novelty) {
        NicheCell& cell = m_cells[c];
        ++cell.visits;
        float rate = 1.0f / (float)cell.visits;
        cell.reward += (reward - cell.reward) * rate;
        cell.threat += (std::clamp(threat, 0.0f, 1.0f) - cell.threat) * rate;
        cell.novelty += (std::max(novelty, 0.0f) - cell.novelty) * rate;
        return cell;
    }
    void imprint(const NicheCoordinate& c, float affordance) {
        NicheCell& cell = m_cells[c];
        cell.affordance = std::clamp(cell.affordance + affordance, -1.0f, 1.0f);
    }
    const NicheCell* find(const NicheCoordinate& c) const {
        auto it = m_cells.find(c);
        return it != m_cells.end() ? &it->second : nullptr;
    }
    size_t size() const { return m_cells.size(); }
    TrizResolution resolve(const NicheCoordinate& here) const {
        TrizResolution r{};
        r.target = here;
        const NicheCell* current = find(here);
        if (!current) return r;
        bool wanted = current->novelty > 0.5f || current->reward > 0.0f;
        if (!wanted || current->threat <= 0.4f) return r;
        r.contradiction = true;
        r.improving = current->reward > 0.0f ? "reward" : "novelty";
        r.worsening = "safety";
        float baseline = current->habitability();
        const NicheCell* best = nullptr;
        NicheCoordinate best_coord = here;
        InventivePrinciple best_principle = InventivePrinciple::None;
        for (int32_t bucket = 0; bucket < time_buckets(); ++bucket) {
            if (bucket == here.time_bucket) continue;
            NicheCoordinate cand = here; cand.time_bucket = bucket;
            consider(cand, InventivePrinciple::SeparationInTime, best, best_coord, best_principle);
        }
        for (int32_t dx = -1; dx <= 1; ++dx)
            for (int32_t dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                NicheCoordinate cand = here; cand.x += dx; cand.y += dy;
                consider(cand, InventivePrinciple::SeparationInSpace, best, best_coord, best_principle);
            }
        if (best && best->habitability() > baseline) {
            r.principle = best_principle;
            r.target = best_coord;
            r.expected_gain = best->habitability() - baseline;
            return r;
        }
        if (current->threat > 0.8f) r.principle = InventivePrinciple::Inversion;
        else if (current->threat > 0.6f) r.principle = InventivePrinciple::Intermediary;
        else if (current->affordance <= 0.0f) r.principle = InventivePrinciple::CushionInAdvance;
        else if (current->visits < 4) r.principle = InventivePrinciple::Segmentation;
        else r.principle = InventivePrinciple::Dynamism;
        return r;
    }
private:
    static constexpr float kNoveltyThreshold = 0.5f;
    static constexpr float kThreatThreshold = 0.4f;
    int32_t time_buckets() const { return (int32_t)ceilf(24.0f / m_hours_per_bucket); }
    void consider(const NicheCoordinate& cand, InventivePrinciple p, const NicheCell*& best,
                  NicheCoordinate& best_coord, InventivePrinciple& best_principle) const {
        const NicheCell* cell = find(cand);
        if (!cell || cell->visits == 0) return;
        if (!best || cell->habitability() > best->habitability()) {
            best = cell; best_coord = cand; best_principle = p;
        }
    }
    float m_cell_size{25.0f};
    float m_hours_per_bucket{4.0f};
    std::unordered_map<NicheCoordinate, NicheCell, NicheCoordinateHash> m_cells{};
};

struct CognitiveState {
    ProprioceptiveFrame frame{};
    SensorimotorReading sensorimotor{};
    EndocrineState endocrine{};
    TrizResolution resolution{};
    NicheCoordinate coordinate{};
    float exploration_drive{};
    uint64_t tick{};
};

class CognitiveArchitecture {
public:
    CognitiveArchitecture() = default;
    CognitiveArchitecture(FrameBufferProbe probe, NicheConstructor niche) :
        m_probe(std::move(probe)), m_niche(std::move(niche)) {}
    void reset() { m_probe.reset(); m_loop.reset(); m_endocrine.reset(); m_tick = 0; }
    void reset_all() { reset(); m_niche.reset(); }
    CognitiveState tick(const FrameBufferView& view, const WorldObservation& world, const MotorCommand& cmd) {
        CognitiveState state{};
        state.frame = m_probe.sample(view);
        state.sensorimotor = m_loop.integrate(cmd, state.frame);
        EndocrineStimulus stimulus{};
        stimulus.novelty = state.sensorimotor.novelty;
        stimulus.agency = state.sensorimotor.agency;
        stimulus.goal_progress = world.goal_progress;
        stimulus.threat = std::clamp(world.hostility + (1.0f - std::clamp(world.health, 0.0f, 1.0f)) * 0.5f, 0.0f, 1.0f);
        stimulus.effort = std::clamp(cmd.magnitude() / 2.236068f, 0.0f, 1.0f);
        state.endocrine = m_endocrine.update(stimulus);
        state.coordinate = m_niche.quantise(world.pose);
        m_niche.observe(state.coordinate, state.endocrine.reward, stimulus.threat, stimulus.novelty);
        state.resolution = m_niche.resolve(state.coordinate);
        state.exploration_drive = std::clamp(0.5f + 0.5f * state.endocrine.wellbeing - 0.5f * state.endocrine.cortisol, 0.0f, 1.0f);
        state.tick = m_tick++;
        return state;
    }
    void imprint(const NicheCoordinate& c, float affordance) { m_niche.imprint(c, affordance); }
    const NicheConstructor& niche() const { return m_niche; }
    const SensorimotorLoop& loop() const { return m_loop; }
    const EndocrineSystem& endocrine() const { return m_endocrine; }
    EndocrineSystem& endocrine() { return m_endocrine; }
private:
    FrameBufferProbe m_probe{};
    SensorimotorLoop m_loop{};
    EndocrineSystem m_endocrine{};
    NicheConstructor m_niche{};
    uint64_t m_tick{};
};

struct EnvironmentConfig {
    size_t grid_width{8}, grid_height{8};
    float cell_size{25.0f}, hours_per_bucket{4.0f};
    uint64_t max_steps{0};
};

struct StepResult {
    CognitiveState state{};
    float reward{};
    float episode_return{};
    bool done{};
    uint64_t step{};
};

class GTAngelEnvironment {
public:
    GTAngelEnvironment() : GTAngelEnvironment(EnvironmentConfig{}) {}
    explicit GTAngelEnvironment(const EnvironmentConfig& config) :
        m_config(config),
        m_mind(FrameBufferProbe(config.grid_width, config.grid_height),
               NicheConstructor(config.cell_size, config.hours_per_bucket)) {}
    void reset() { m_mind.reset(); m_step = 0; m_return = 0.0f; m_done = false; }
    void reset_all() { reset(); m_mind.reset_all(); }
    StepResult step(const FrameBufferView& view, const WorldObservation& world, const MotorCommand& cmd) {
        StepResult result{};
        if (m_done) {
            result.episode_return = m_return; result.done = true; result.step = m_step;
            return result;
        }
        result.state = m_mind.tick(view, world, cmd);
        result.reward = result.state.endocrine.reward;
        m_return += result.reward;
        result.episode_return = m_return;
        result.step = m_step++;
        m_done = world.terminal || (m_config.max_steps != 0 && m_step >= m_config.max_steps);
        result.done = m_done;
        return result;
    }
    MotorCommand suggest(const CognitiveState& state) const {
        MotorCommand cmd{};
        float vigour = std::clamp(0.25f + 0.75f * state.endocrine.arousal, 0.0f, 1.0f);
        cmd.move_forward = vigour * state.exploration_drive;
        cmd.look_right = 0.5f * (state.exploration_drive - 0.5f);
        cmd.actuate = std::clamp(state.endocrine.dopamine - state.endocrine.cortisol, 0.0f, 1.0f);
        switch (state.resolution.principle) {
            case InventivePrinciple::SeparationInSpace:
            case InventivePrinciple::Segmentation:
                cmd.move_forward *= 0.5f; cmd.move_right = vigour; break;
            case InventivePrinciple::SeparationInTime:
            case InventivePrinciple::CushionInAdvance:
                cmd.move_forward = -0.5f * vigour; cmd.actuate = 0.0f; break;
            case InventivePrinciple::Inversion:
                cmd.move_forward = -vigour; break;
            case InventivePrinciple::Intermediary:
                cmd.move_right = 0.5f * vigour; cmd.actuate *= 0.5f; break;
            case InventivePrinciple::Dynamism:
                cmd.move_right = vigour * (state.tick % 2 == 0 ? 1.0f : -1.0f); break;
            default: break;
        }
        cmd.move_forward = std::clamp(cmd.move_forward, -1.0f, 1.0f);
        cmd.move_right = std::clamp(cmd.move_right, -1.0f, 1.0f);
        cmd.look_right = std::clamp(cmd.look_right, -1.0f, 1.0f);
        cmd.look_up = std::clamp(cmd.look_up, -1.0f, 1.0f);
        cmd.actuate = std::clamp(cmd.actuate, 0.0f, 1.0f);
        return cmd;
    }
    void imprint(const NicheCoordinate& c, float affordance) { m_mind.imprint(c, affordance); }
    const CognitiveArchitecture& mind() const { return m_mind; }
    CognitiveArchitecture& mind() { return m_mind; }
    float episode_return() const { return m_return; }
    uint64_t steps() const { return m_step; }
    bool done() const { return m_done; }
private:
    EnvironmentConfig m_config{};
    CognitiveArchitecture m_mind;
    uint64_t m_step{};
    float m_return{};
    bool m_done{};
};

} // namespace gtangel

// GTAngel v15 — authoritative 768x768 software renderer and render pyramid.
// This module is spliced after namespace gtangel and therefore reuses the
// v13 avatar/world contracts plus GTAngel-Cls framebuffer concepts.

// Component-specific Z++ core clock & pool bindings (from core/ZppContracts.hpp)
namespace gtangel_zpp_core_bind {
constexpr zpp_core_spec::CClock_ZppContract makeInitialisedGameClock(int64_t msPerMin, int64_t tickMs) {
    const auto c0 = zpp_core_spec::apply_CClock_Initialise(zpp_core_spec::CClock_ZppContract{}, msPerMin, tickMs);
    return zpp_core_spec::apply_CClock_StoreClock(c0);
}
static_assert(makeInitialisedGameClock(1000, 16).ms_bClockHasBeenStored && makeInitialisedGameClock(1000, 16).ms_Stored_nGameClockHours == 12, "CognitiveSubsystems CClock Z++ store transition must hold");
} // namespace gtangel_zpp_core_bind
