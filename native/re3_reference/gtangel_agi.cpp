// GTAngel AGI Gameworld Testing Environment
// Pure C++ Win32 — no .NET, no external deps beyond Windows API
// Composition: /iterative-micro-improvement(/meta-echo-dna</echo-angel[/superhot-gamer-girl]>
//              & /vorticog</triz-cognitive-arena[/ai-env-grp-dyn]>
//              & /echo-introspect[/cogverse-evolution-accelerator])

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>
#include <sstream>
#include <fstream>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

// ─── Virtual Endocrine System (16 hormone channels) ─────────────────────────
struct EndocrineState {
    float channels[16] = {};
    // 0:CRH 1:ACTH 2:Cortisol 3:DopamineTonic 4:DopaminePhasic 5:Serotonin
    // 6:Norepinephrine 7:Oxytocin 8:T3T4 9:Melatonin 10:Insulin 11:Glucagon
    // 12:IL6 13:Anandamide 14-15:Reserved
    float baselines[16] = {0.05f,0.05f,0.15f,0.3f,0.0f,0.4f,0.1f,0.1f,0.5f,0.0f,0.2f,0.1f,0.05f,0.1f,0.0f,0.0f};
    float halfLives[16] = {5,10,30,20,3,50,8,15,100,12,10,8,20,6,10,10};
    int tick = 0;

    void decay() {
        tick++;
        for (int i = 0; i < 16; i++) {
            float lambda = 0.693f / halfLives[i];
            channels[i] = baselines[i] + (channels[i] - baselines[i]) * expf(-lambda);
        }
    }
    void signal(int hormone, float amount) {
        if (hormone >= 0 && hormone < 16)
            channels[hormone] = fminf(1.0f, channels[hormone] + amount);
    }
    float concentration(int h) const { return channels[h]; }
    const char* cognitiveMode() const {
        if (channels[2] > 0.6f) return "STRESSED";
        if (channels[4] > 0.5f) return "REWARD";
        if (channels[6] > 0.5f) return "VIGILANT";
        if (channels[7] > 0.4f) return "SOCIAL";
        if (channels[3] > 0.5f) return "EXPLORATORY";
        if (channels[5] > 0.6f) return "FOCUSED";
        if (channels[9] > 0.3f) return "RESTING";
        return "REFLECTIVE";
    }
};

// ─── TRIZ Cognitive Arena (hex-grid, 40 principles) ─────────────────────────
struct HexCell {
    int q, r;
    float aestheticValue = 0.5f;
    float temperature = 0.5f;
    int objectCount = 0;
    bool occupied = false;
};

struct TRIZArena {
    static constexpr int GRID_SIZE = 12;
    HexCell grid[GRID_SIZE][GRID_SIZE];
    float overallCoherence = 0.5f;
    int discoveryPhase = 0; // 0:Contradiction 1:Experiment 2:Crystallize 3:Teach
    int stepInPhase = 0;
    int totalDiscoveries = 0;
    float lastCoherenceDelta = 0.0f;

    // 40 TRIZ principles as action names
    const char* principles[40] = {
        "Segment","Extract","Differentiate","BreakSymmetry","Merge","MultiPurpose","Nest",
        "Counterbalance","PreStress","PrePosition","AddBackup","FlattenField","Invert",
        "Curve","MakeFlexible","Overshoot","AddDimension",
        "Vibrate","Pulse","Sustain","SkipThrough",
        "ReframeHarm","AddFeedback","Mediate","SelfServe","Copy","MakeDisposable",
        "ReplaceWithField","Fluidize","MakeMembrane","MakePorous","ChangeColor","Homogenize","Recycle",
        "ChangeParameter","ExploitTransition","ExpandDifferentially","Catalyze","Protect","Compose"
    };

    TRIZArena() { reset(); }
    void reset() {
        for (int q = 0; q < GRID_SIZE; q++)
            for (int r = 0; r < GRID_SIZE; r++) {
                grid[q][r].q = q; grid[q][r].r = r;
                grid[q][r].aestheticValue = 0.3f + (float)(rand() % 40) / 100.0f;
                grid[q][r].objectCount = rand() % 3;
                grid[q][r].occupied = grid[q][r].objectCount > 0;
            }
        computeCoherence();
    }
    void computeCoherence() {
        float sum = 0, count = 0;
        for (int q = 0; q < GRID_SIZE; q++)
            for (int r = 0; r < GRID_SIZE; r++) {
                sum += grid[q][r].aestheticValue;
                count++;
            }
        overallCoherence = count > 0 ? sum / count : 0.5f;
    }
    // Apply a TRIZ principle as a grid transformation
    float applyPrinciple(int principleIdx, int q, int r) {
        if (q < 0 || q >= GRID_SIZE || r < 0 || r >= GRID_SIZE) return 0;
        float before = overallCoherence;
        auto& cell = grid[q][r];
        switch (principleIdx % 40) {
            case 0: cell.objectCount++; cell.aestheticValue += 0.05f; break; // Segment
            case 4: cell.aestheticValue += 0.08f; cell.occupied = true; break; // Merge
            case 6: cell.aestheticValue += 0.06f; break; // Nest
            case 17: cell.aestheticValue += 0.1f; cell.temperature += 0.1f; break; // Vibrate
            case 22: cell.aestheticValue += 0.07f; break; // AddFeedback
            case 31: cell.aestheticValue += 0.04f; break; // ChangeColor
            case 39: cell.aestheticValue += 0.12f; cell.objectCount += 2; break; // Compose
            default: cell.aestheticValue += 0.03f; break;
        }
        cell.aestheticValue = fminf(1.0f, fmaxf(0.0f, cell.aestheticValue));
        computeCoherence();
        lastCoherenceDelta = overallCoherence - before;
        if (lastCoherenceDelta > 0.01f) totalDiscoveries++;
        return lastCoherenceDelta;
    }
    // 12-step discovery loop (3 steps per phase)
    void tick() {
        stepInPhase++;
        if (stepInPhase >= 3) { stepInPhase = 0; discoveryPhase = (discoveryPhase + 1) % 4; }
        // Auto-experiment: pick random cell and principle
        int q = rand() % GRID_SIZE, r = rand() % GRID_SIZE;
        int p = rand() % 40;
        applyPrinciple(p, q, r);
    }
    const char* phaseName() const {
        switch (discoveryPhase) {
            case 0: return "CONTRADICTION";
            case 1: return "EXPERIMENT";
            case 2: return "CRYSTALLIZE";
            case 3: return "TEACH";
            default: return "UNKNOWN";
        }
    }
};

// ─── Group Dynamics (emergent agent groups) ──────────────────────────────────
struct Agent {
    int id;
    float x, y;
    float trust = 0.5f, affinity = 0.5f;
    int groupId = -1;
    float cohesion = 0.0f;
    char name[32];
};

struct GroupDynamics {
    std::vector<Agent> agents;
    std::map<int, std::vector<int>> groups; // groupId -> agentIds
    int nextGroupId = 0;
    float avgCohesion = 0.0f;

    void init(int n) {
        agents.clear();
        groups.clear();
        for (int i = 0; i < n; i++) {
            Agent a;
            a.id = i;
            a.x = (float)(rand() % 800);
            a.y = (float)(rand() % 600);
            snprintf(a.name, 32, "Agent_%d", i);
            agents.push_back(a);
        }
    }
    void tick() {
        // Proximity-based group formation
        for (auto& a : agents) {
            if (a.groupId >= 0) continue;
            for (auto& b : agents) {
                if (a.id == b.id || b.groupId >= 0) continue;
                float dx = a.x - b.x, dy = a.y - b.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 100.0f && a.affinity > 0.3f) {
                    int gid = nextGroupId++;
                    a.groupId = gid; b.groupId = gid;
                    groups[gid] = {a.id, b.id};
                    a.cohesion = 0.5f; b.cohesion = 0.5f;
                    break;
                }
            }
        }
        // Cohesion decay
        for (auto& a : agents) {
            if (a.groupId >= 0) {
                a.cohesion = fmaxf(0.0f, a.cohesion - 0.001f);
                if (a.cohesion < 0.1f) {
                    groups[a.groupId].erase(
                        std::remove(groups[a.groupId].begin(), groups[a.groupId].end(), a.id),
                        groups[a.groupId].end());
                    a.groupId = -1;
                }
            }
        }
        // Compute average cohesion
        float sum = 0; int count = 0;
        for (auto& a : agents) if (a.groupId >= 0) { sum += a.cohesion; count++; }
        avgCohesion = count > 0 ? sum / count : 0.0f;
    }
    int groupCount() const { return (int)groups.size(); }
};

// ─── Proprioceptive Sensorimotor Loop ───────────────────────────────────────
struct SensorimotorLoop {
    // Frame buffer mapped to feedback mechanisms
    static constexpr int FRAME_W = 64;
    static constexpr int FRAME_H = 64;
    float frameBuffer[FRAME_W * FRAME_H] = {};
    float proprioception[8] = {}; // position(3) + velocity(3) + fatigue + arousal
    float motorOutput[6] = {};    // action vector
    float reward = 0.0f;
    float totalReward = 0.0f;
    int stepCount = 0;

    void captureFrame() {
        // Simulate frame capture from game world (synthetic pattern)
        for (int y = 0; y < FRAME_H; y++)
            for (int x = 0; x < FRAME_W; x++) {
                float v = sinf(x * 0.1f + stepCount * 0.05f) * cosf(y * 0.1f + stepCount * 0.03f);
                frameBuffer[y * FRAME_W + x] = (v + 1.0f) * 0.5f;
            }
    }
    void computeProprioception() {
        // Map frame buffer statistics to proprioceptive feedback
        float mean = 0, variance = 0;
        for (int i = 0; i < FRAME_W * FRAME_H; i++) mean += frameBuffer[i];
        mean /= (FRAME_W * FRAME_H);
        for (int i = 0; i < FRAME_W * FRAME_H; i++)
            variance += (frameBuffer[i] - mean) * (frameBuffer[i] - mean);
        variance /= (FRAME_W * FRAME_H);

        proprioception[0] = mean;           // spatial position proxy
        proprioception[1] = variance;       // velocity proxy
        proprioception[2] = frameBuffer[0]; // orientation proxy
        proprioception[3] = frameBuffer[FRAME_W*FRAME_H-1]; // acceleration
        proprioception[4] = variance * 2.0f; // fatigue
        proprioception[5] = mean * 0.8f;    // arousal
        proprioception[6] = (float)(stepCount % 100) / 100.0f; // temporal phase
        proprioception[7] = reward;         // reward feedback
    }
    void selectAction(EndocrineState& endo) {
        // Endocrine-modulated action selection
        float dopamine = endo.concentration(3) + endo.concentration(4);
        float cortisol = endo.concentration(2);
        float ne = endo.concentration(6);
        for (int i = 0; i < 6; i++) {
            float base = (float)(rand() % 100) / 100.0f;
            motorOutput[i] = base * (0.5f + dopamine * 0.5f) * (1.0f - cortisol * 0.3f) + ne * 0.2f;
        }
    }
    void tick(EndocrineState& endo) {
        stepCount++;
        captureFrame();
        computeProprioception();
        selectAction(endo);
        // Compute reward from proprioceptive coherence
        float coherence = 1.0f - fabsf(proprioception[0] - 0.5f) * 2.0f;
        reward = coherence * 0.1f + proprioception[5] * 0.05f;
        totalReward += reward;
        // Signal endocrine system
        if (reward > 0.08f) endo.signal(4, 0.3f); // dopamine phasic
        if (reward < 0.02f) endo.signal(2, 0.1f); // cortisol
        if (proprioception[5] > 0.6f) endo.signal(6, 0.2f); // norepinephrine
    }
};

// ─── Iterative Micro-Improvement Loop ───────────────────────────────────────
struct MicroImprovement {
    int version = 0;
    int keptVersions = 0;
    float bestCoherence = 0.0f;
    std::vector<std::string> history;

    void iterate(TRIZArena& arena, SensorimotorLoop& sml) {
        version++;
        // Introspection: identify weakest dimension
        float coherence = arena.overallCoherence;
        float reward = sml.totalReward / (sml.stepCount + 1);
        // Mutation: apply TRIZ principle to improve
        int q = rand() % TRIZArena::GRID_SIZE;
        int r = rand() % TRIZArena::GRID_SIZE;
        int p = rand() % 40;
        float delta = arena.applyPrinciple(p, q, r);
        // Evaluation: keep if improved
        if (delta > 0 || reward > bestCoherence) {
            keptVersions++;
            bestCoherence = fmaxf(bestCoherence, coherence);
            char buf[128];
            snprintf(buf, 128, "v%d: KEEP (delta=%.3f, coherence=%.3f)", version, delta, coherence);
            history.push_back(buf);
        } else {
            char buf[128];
            snprintf(buf, 128, "v%d: REVERT (delta=%.3f)", version, delta);
            history.push_back(buf);
        }
        if (history.size() > 20) history.erase(history.begin());
    }
};

// ─── Echo Introspection Engine ───────────────────────────────────────────────
struct EchoIntrospect {
    std::vector<std::string> insights;
    float selfAwareness = 0.3f;
    int introspectionCount = 0;

    void introspect(EndocrineState& endo, TRIZArena& arena, SensorimotorLoop& sml) {
        introspectionCount++;
        selfAwareness = fminf(1.0f, selfAwareness + 0.01f);
        char buf[256];
        const char* mode = endo.cognitiveMode();
        float cortisol = endo.concentration(2);
        float dopamine = endo.concentration(3);
        if (cortisol > 0.5f) {
            snprintf(buf, 256, "Introspection #%d: High cortisol (%.2f) — touching vulnerability. Mode=%s. Coherence=%.3f",
                introspectionCount, cortisol, mode, arena.overallCoherence);
        } else if (dopamine > 0.5f) {
            snprintf(buf, 256, "Introspection #%d: Dopamine burst (%.2f) — genuine insight! Mode=%s. Reward=%.3f",
                introspectionCount, dopamine, mode, sml.totalReward);
        } else {
            snprintf(buf, 256, "Introspection #%d: Mode=%s. Self-awareness=%.3f. Groups=%d. Discoveries=%d",
                introspectionCount, mode, selfAwareness, 0, arena.totalDiscoveries);
        }
        insights.push_back(buf);
        if (insights.size() > 15) insights.erase(insights.begin());
    }
};

// ─── CogVerse Evolution Accelerator ─────────────────────────────────────────
struct CogVerse {
    int evolutionCycle = 0;
    float propertyCoherence = 0.5f;
    std::vector<std::string> residents;
    int activeResidents = 0;

    CogVerse() {
        residents = {"Dan","Manus","Echo","Marduk","OpenCog","Aion","Vega","Ember","ma9us","Eliza"};
        activeResidents = 10;
    }
    void evolve(TRIZArena& arena, GroupDynamics& gd) {
        evolutionCycle++;
        // Score property coherence from arena + group dynamics
        float arenaScore = arena.overallCoherence;
        float groupScore = gd.avgCohesion;
        propertyCoherence = (arenaScore + groupScore) * 0.5f;
    }
};

// ─── Main Application State ──────────────────────────────────────────────────
struct GTAngelAGI {
    EndocrineState endo;
    TRIZArena arena;
    GroupDynamics groups;
    SensorimotorLoop sensorimotor;
    MicroImprovement improvement;
    EchoIntrospect introspect;
    CogVerse cogverse;
    bool running = false;
    bool paused = false;
    int tickCount = 0;
    std::string log[50];
    int logHead = 0;

    void addLog(const char* msg) {
        log[logHead % 50] = msg;
        logHead++;
    }
    void tick() {
        if (!running || paused) return;
        tickCount++;
        endo.decay();
        arena.tick();
        groups.tick();
        sensorimotor.tick(endo);
        if (tickCount % 10 == 0) improvement.iterate(arena, sensorimotor);
        if (tickCount % 25 == 0) introspect.introspect(endo, arena, sensorimotor);
        if (tickCount % 50 == 0) cogverse.evolve(arena, groups);
        if (tickCount % 5 == 0) {
            char buf[128];
            snprintf(buf, 128, "Tick %d | Mode=%s | Coherence=%.3f | Reward=%.3f | Groups=%d",
                tickCount, endo.cognitiveMode(), arena.overallCoherence,
                sensorimotor.totalReward, groups.groupCount());
            addLog(buf);
        }
    }
};

// ─── Win32 UI ────────────────────────────────────────────────────────────────
static GTAngelAGI g_agi;
static HWND g_hwnd = NULL;
static HFONT g_hFont = NULL;
static HFONT g_hFontLarge = NULL;
static HFONT g_hFontMono = NULL;

// Colors
#define COL_BG      RGB(13, 13, 26)
#define COL_CARD    RGB(24, 24, 48)
#define COL_ACCENT  RGB(255, 215, 0)
#define COL_CYAN    RGB(0, 188, 212)
#define COL_GREEN   RGB(76, 175, 80)
#define COL_RED     RGB(244, 67, 54)
#define COL_TEXT    RGB(255, 255, 255)
#define COL_TEXT2   RGB(158, 158, 200)
#define COL_PURPLE  RGB(156, 39, 176)

void DrawText(HDC hdc, int x, int y, const char* text, COLORREF color, HFONT font) {
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, font);
    TextOutA(hdc, x, y, text, (int)strlen(text));
}

void DrawRect(HDC hdc, int x, int y, int w, int h, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    RECT rc = {x, y, x+w, y+h};
    FillRect(hdc, &rc, brush);
    DeleteObject(brush);
}

void DrawBar(HDC hdc, int x, int y, int w, int h, float value, COLORREF color, COLORREF bg) {
    DrawRect(hdc, x, y, w, h, bg);
    int fill = (int)(w * fminf(1.0f, fmaxf(0.0f, value)));
    if (fill > 0) DrawRect(hdc, x, y, fill, h, color);
}

void RenderFrame(HDC hdc, RECT& clientRc) {
    int W = clientRc.right - clientRc.left;
    int H = clientRc.bottom - clientRc.top;

    // Double buffer
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, W, H);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

    // Background
    DrawRect(memDC, 0, 0, W, H, COL_BG);

    // Title
    DrawText(memDC, 20, 15, "GTAngel — AGI Gameworld Testing Environment", COL_ACCENT, g_hFontLarge);
    DrawText(memDC, 20, 45, "Composition: /iterative-micro-improvement(/meta-echo-dna</echo-angel[/superhot-gamer-girl]> & /vorticog</triz-cognitive-arena[/ai-env-grp-dyn]> & /echo-introspect[/cogverse-evolution-accelerator])", COL_TEXT2, g_hFont);

    // Status
    char statusBuf[256];
    snprintf(statusBuf, 256, "Status: %s | Tick: %d | Mode: %s",
        g_agi.running ? (g_agi.paused ? "PAUSED" : "RUNNING") : "STOPPED",
        g_agi.tickCount, g_agi.endo.cognitiveMode());
    DrawText(memDC, 20, 75, statusBuf, g_agi.running ? COL_GREEN : COL_RED, g_hFont);

    // ── Left Panel: Endocrine System ──
    int lx = 20, ly = 110, lw = 320, lh = 400;
    DrawRect(memDC, lx, ly, lw, lh, COL_CARD);
    DrawText(memDC, lx+10, ly+10, "Virtual Endocrine System (16 channels)", COL_CYAN, g_hFont);
    const char* hormoneNames[16] = {"CRH","ACTH","Cortisol","DopamineT","DopamineP","Serotonin",
        "Norepinephrine","Oxytocin","T3/T4","Melatonin","Insulin","Glucagon","IL-6","Anandamide","R14","R15"};
    for (int i = 0; i < 16; i++) {
        int row = i / 4, colIdx = i % 4;
        int bx = lx + 10 + colIdx * 75, by = ly + 40 + row * 80;
        char hbuf[64];
        snprintf(hbuf, 64, "%s", hormoneNames[i]);
        DrawText(memDC, bx, by, hbuf, COL_TEXT2, g_hFont);
        float val = g_agi.endo.concentration(i);
        COLORREF barCol = val > 0.6f ? COL_RED : (val > 0.4f ? COL_ACCENT : COL_GREEN);
        DrawBar(memDC, bx, by + 20, 65, 8, val, barCol, RGB(40,40,60));
        snprintf(hbuf, 64, "%.2f", val);
        DrawText(memDC, bx, by + 32, hbuf, COL_TEXT, g_hFontMono);
    }
    // Cognitive mode
    char modeBuf[64];
    snprintf(modeBuf, 64, "Mode: %s", g_agi.endo.cognitiveMode());
    DrawText(memDC, lx+10, ly+lh-30, modeBuf, COL_ACCENT, g_hFont);

    // ── Center Panel: TRIZ Arena ──
    int cx = 360, cy = 110, cw = 480, ch = 400;
    DrawRect(memDC, cx, cy, cw, ch, COL_CARD);
    DrawText(memDC, cx+10, cy+10, "TRIZ Cognitive Arena (Hex Grid)", COL_CYAN, g_hFont);
    // Draw hex grid
    int cellSize = 28;
    int gridOx = cx + 30, gridOy = cy + 40;
    for (int q = 0; q < TRIZArena::GRID_SIZE; q++) {
        for (int r = 0; r < TRIZArena::GRID_SIZE; r++) {
            auto& cell = g_agi.arena.grid[q][r];
            int px = gridOx + q * (cellSize + 4) + (r % 2) * (cellSize/2 + 2);
            int py = gridOy + r * (cellSize + 4);
            float v = cell.aestheticValue;
            COLORREF cellCol = RGB((int)(v * 255), (int)(v * 200), (int)(v * 100));
            if (cell.occupied) cellCol = RGB((int)(v*255), (int)(v*180), (int)(v*50));
            DrawRect(memDC, px, py, cellSize, cellSize, cellCol);
        }
    }
    // Arena stats
    char arenaBuf[128];
    snprintf(arenaBuf, 128, "Coherence: %.3f | Phase: %s | Discoveries: %d",
        g_agi.arena.overallCoherence, g_agi.arena.phaseName(), g_agi.arena.totalDiscoveries);
    DrawText(memDC, cx+10, cy+ch-50, arenaBuf, COL_TEXT, g_hFont);
    snprintf(arenaBuf, 128, "Last Delta: %.4f", g_agi.arena.lastCoherenceDelta);
    DrawText(memDC, cx+10, cy+ch-30, arenaBuf, COL_TEXT2, g_hFont);

    // ── Right Panel: Sensorimotor + Group Dynamics ──
    int rx = 860, ry = 110, rw = 400, rh = 400;
    DrawRect(memDC, rx, ry, rw, rh, COL_CARD);
    DrawText(memDC, rx+10, ry+10, "Proprioceptive Sensorimotor Loop", COL_CYAN, g_hFont);
    // Frame buffer visualization (mini)
    int fbSize = 4;
    int fbOx = rx + 20, fbOy = ry + 40;
    for (int y = 0; y < SensorimotorLoop::FRAME_H; y += 4) {
        for (int x = 0; x < SensorimotorLoop::FRAME_W; x += 4) {
            float v = g_agi.sensorimotor.frameBuffer[y * SensorimotorLoop::FRAME_W + x];
            COLORREF pixCol = RGB((int)(v*100), (int)(v*150), (int)(v*255));
            DrawRect(memDC, fbOx + x/4 * fbSize, fbOy + y/4 * fbSize, fbSize-1, fbSize-1, pixCol);
        }
    }
    // Proprioception bars
    const char* propNames[8] = {"Pos","Vel","Orient","Accel","Fatigue","Arousal","Phase","Reward"};
    for (int i = 0; i < 8; i++) {
        int by = ry + 140 + i * 28;
        DrawText(memDC, rx+10, by, propNames[i], COL_TEXT2, g_hFont);
        float val = g_agi.sensorimotor.proprioception[i];
        DrawBar(memDC, rx+80, by, 200, 12, val, COL_PURPLE, RGB(40,40,60));
        char pbuf[32];
        snprintf(pbuf, 32, "%.3f", val);
        DrawText(memDC, rx+290, by, pbuf, COL_TEXT, g_hFontMono);
    }
    // Reward
    char rwBuf[64];
    snprintf(rwBuf, 64, "Total Reward: %.3f | Steps: %d",
        g_agi.sensorimotor.totalReward, g_agi.sensorimotor.stepCount);
    DrawText(memDC, rx+10, ry+rh-30, rwBuf, COL_ACCENT, g_hFont);

    // ── Bottom Left: Group Dynamics ──
    int gx = 20, gy = 530, gw = 320, gh = 200;
    DrawRect(memDC, gx, gy, gw, gh, COL_CARD);
    DrawText(memDC, gx+10, gy+10, "Group Dynamics (Emergent)", COL_CYAN, g_hFont);
    char gBuf[128];
    snprintf(gBuf, 128, "Agents: %d | Groups: %d | Avg Cohesion: %.3f",
        (int)g_agi.groups.agents.size(), g_agi.groups.groupCount(), g_agi.groups.avgCohesion);
    DrawText(memDC, gx+10, gy+40, gBuf, COL_TEXT, g_hFont);
    // Draw agent positions
    int mapOx = gx + 10, mapOy = gy + 70;
    int mapW = gw - 20, mapH = gh - 80;
    DrawRect(memDC, mapOx, mapOy, mapW, mapH, RGB(20,20,40));
    for (auto& a : g_agi.groups.agents) {
        int ax = mapOx + (int)(a.x / 800.0f * mapW);
        int ay = mapOy + (int)(a.y / 600.0f * mapH);
        COLORREF acol = a.groupId >= 0 ? COL_GREEN : COL_TEXT2;
        DrawRect(memDC, ax-2, ay-2, 5, 5, acol);
    }

    // ── Bottom Center: Micro-Improvement + Introspection ──
    int mx = 360, my = 530, mw = 480, mh = 200;
    DrawRect(memDC, mx, my, mw, mh, COL_CARD);
    DrawText(memDC, mx+10, my+10, "Iterative Micro-Improvement + Echo Introspection", COL_CYAN, g_hFont);
    char miBuf[128];
    snprintf(miBuf, 128, "Version: %d | Kept: %d | Best: %.3f | Self-Awareness: %.3f",
        g_agi.improvement.version, g_agi.improvement.keptVersions,
        g_agi.improvement.bestCoherence, g_agi.introspect.selfAwareness);
    DrawText(memDC, mx+10, my+40, miBuf, COL_TEXT, g_hFont);
    // Recent history
    int hy = my + 65;
    for (int i = (int)g_agi.improvement.history.size()-1; i >= 0 && i >= (int)g_agi.improvement.history.size()-5; i--) {
        DrawText(memDC, mx+10, hy, g_agi.improvement.history[i].c_str(), COL_TEXT2, g_hFontMono);
        hy += 18;
    }
    // Latest insight
    if (!g_agi.introspect.insights.empty()) {
        DrawText(memDC, mx+10, my+mh-30, g_agi.introspect.insights.back().c_str(), COL_ACCENT, g_hFont);
    }

    // ── Bottom Right: CogVerse + Log ──
    int lx2 = 860, ly2 = 530, lw2 = 400, lh2 = 200;
    DrawRect(memDC, lx2, ly2, lw2, lh2, COL_CARD);
    DrawText(memDC, lx2+10, ly2+10, "CogVerse Evolution Accelerator", COL_CYAN, g_hFont);
    char cvBuf[128];
    snprintf(cvBuf, 128, "Cycle: %d | Property Coherence: %.3f | Residents: %d",
        g_agi.cogverse.evolutionCycle, g_agi.cogverse.propertyCoherence, g_agi.cogverse.activeResidents);
    DrawText(memDC, lx2+10, ly2+40, cvBuf, COL_TEXT, g_hFont);
    // Log
    int logY = ly2 + 65;
    for (int i = 0; i < 8; i++) {
        int idx = (g_agi.logHead - 1 - i + 50) % 50;
        if (g_agi.log[idx].empty()) continue;
        DrawText(memDC, lx2+10, logY, g_agi.log[idx].c_str(), COL_TEXT2, g_hFontMono);
        logY += 16;
    }

    // Controls hint
    DrawText(memDC, 20, H-30, "[SPACE] Start/Pause | [R] Reset | [ESC] Exit", COL_TEXT2, g_hFont);

    // Blit
    BitBlt(hdc, 0, 0, W, H, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            g_hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, "Arial");
            g_hFontLarge = CreateFontA(22, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, "Arial");
            g_hFontMono = CreateFontA(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_MODERN, "Consolas");
            SetTimer(hwnd, 1, 50, NULL); // 20 Hz tick
            g_agi.groups.init(20);
            g_agi.addLog("GTAngel AGI initialized");
            return 0;
        case WM_TIMER:
            g_agi.tick();
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            RenderFrame(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN:
            if (wParam == VK_SPACE) {
                if (!g_agi.running) { g_agi.running = true; g_agi.addLog("Started"); }
                else { g_agi.paused = !g_agi.paused; g_agi.addLog(g_agi.paused ? "Paused" : "Resumed"); }
            } else if (wParam == 'R') {
                g_agi = GTAngelAGI();
                g_agi.groups.init(20);
                g_agi.addLog("Reset");
            } else if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            DeleteObject(g_hFont);
            DeleteObject(g_hFontLarge);
            DeleteObject(g_hFontMono);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    srand((unsigned int)GetTickCount());

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "GTAngelAGI";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);

    g_hwnd = CreateWindowExA(0, "GTAngelAGI",
        "GTAngel — AGI Gameworld Testing Environment",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1300, 800,
        NULL, NULL, hInst, NULL);

    if (!g_hwnd) return 1;
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
