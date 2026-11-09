--[[
    GTAngel AGI Gameworld Testing Environment
    ==========================================

    An experimental UEVR script that turns the game world into a testbed for
    cognitive-architecture style agents. It wires the per-frame engine
    callbacks into a simple sensorimotor loop:

        frame buffer / world state -> proprioceptive feedback -> niche map
                                                                -> endocrine reward
                                                                -> (agent action)

    This is intentionally engine-agnostic and dependency-free: it only relies
    on the UEVR Lua API (engine tick callbacks, ImGui bindings and the JSON
    helpers), so it can run against any UE4/UE5 title without extra tooling.

    Three cooperating subsystems are implemented below:

    1. Proprioceptive feedback (sensorimotor loop)
       Every post-engine-tick, the local pawn's transform/velocity is sampled
       and treated as the "frame buffer" signal that would normally come from
       rendered pixels - i.e. the raw sensory sample an embodied agent would
       receive on each simulation step.

    2. TRIZ-inspired spatio-temporal niche construction
       The world is quantized into a coarse spatio-temporal grid (space x
       time-of-day bucket). Each niche accumulates a visitation count and a
       decaying "pressure" value. TRIZ's ideality/contradiction framing is
       used to decide, each tick, whether to bias the agent toward
       exploration (unresolved contradiction: novelty is high, pressure is
       low) or exploitation (contradiction resolved: pressure has built up
       and a nearby, well-understood niche is preferable).

    3. Adaptive endocrine reward system
       Two hormone-like scalars - "dopamine" (short-term novelty reward) and
       "cortisol" (long-term stress/stagnation signal) - rise and decay every
       tick based on niche novelty and revisitation. Their ratio produces an
       adaptive scalar reward that a downstream RL/cognitive-architecture
       agent can consume via `get_reward()`.

    None of this requires a specific game; the sampled fields degrade
    gracefully to zero if a pawn/controller isn't available yet.
--]]

local gtangel = {}

-- ------------------------------------------------------------------------
-- Configuration
-- ------------------------------------------------------------------------

gtangel.config = {
    niche_cell_size = 200.0,      -- spatial quantization (UE units) per niche cell
    niche_time_buckets = 8,       -- temporal quantization buckets per "day" cycle
    niche_seconds_per_cycle = 120.0, -- how long a full time cycle takes (seconds)

    dopamine_decay = 0.98,        -- per-tick multiplicative decay
    cortisol_decay = 0.995,       -- cortisol decays slower than dopamine
    dopamine_novelty_gain = 1.0,  -- reward gain for visiting a new/rare niche
    cortisol_stagnation_gain = 0.05, -- stress gain for revisiting a saturated niche
    pressure_saturation = 25.0,   -- visits at which a niche is considered "saturated"

    exploration_bias_threshold = 0.35, -- contradiction resolution threshold
}

-- ------------------------------------------------------------------------
-- State
-- ------------------------------------------------------------------------

gtangel.state = {
    elapsed_time = 0.0,
    niches = {},               -- key -> { visits = n, pressure = p, last_seen = t }
    dopamine = 0.0,
    cortisol = 0.0,
    reward = 0.0,
    mode = "explore",          -- "explore" | "exploit" (TRIZ contradiction resolution)
    last_proprioception = {
        position = { x = 0.0, y = 0.0, z = 0.0 },
        velocity = { x = 0.0, y = 0.0, z = 0.0 },
        speed = 0.0,
    },
}

-- ------------------------------------------------------------------------
-- Helpers
-- ------------------------------------------------------------------------

local function niche_key(cell_x, cell_y, time_bucket)
    return cell_x .. ":" .. cell_y .. ":" .. time_bucket
end

local function get_time_bucket(elapsed_time)
    local cfg = gtangel.config
    local phase = (elapsed_time % cfg.niche_seconds_per_cycle) / cfg.niche_seconds_per_cycle
    return math.floor(phase * cfg.niche_time_buckets)
end

--- Samples the "frame buffer" proprioceptive signal for the local pawn.
--- In a full cognitive-architecture integration this is where rendered
--- frame data / depth buffer would be fused with kinematic state; here we
--- use the pawn transform + velocity as a lightweight stand-in that is
--- available on every UEVR-supported title.
local function sample_proprioception()
    local position = { x = 0.0, y = 0.0, z = 0.0 }
    local velocity = { x = 0.0, y = 0.0, z = 0.0 }

    local ok, game_engine_class = pcall(function()
        return uevr.api:find_uobject("Class /Script/Engine.GameEngine")
    end)

    if not ok or game_engine_class == nil then
        return { position = position, velocity = velocity, speed = 0.0 }
    end

    local game_engine = UEVR_UObjectHook.get_first_object_by_class(game_engine_class)
    if game_engine == nil then
        return { position = position, velocity = velocity, speed = 0.0 }
    end

    local viewport = game_engine.GameViewport
    local world = viewport and viewport.World or nil
    local game_instance = game_engine.GameInstance
    local local_players = game_instance and game_instance.LocalPlayers or nil

    if world == nil or local_players == nil then
        return { position = position, velocity = velocity, speed = 0.0 }
    end

    for i in ipairs(local_players) do
        local player = local_players[i]
        local controller = player and player.PlayerController or nil
        local pawn = controller and controller.Pawn or nil

        if pawn ~= nil then
            local ok_pos, root_location = pcall(function() return pawn:K2_GetActorLocation() end)
            if ok_pos and root_location ~= nil then
                position = { x = root_location.x, y = root_location.y, z = root_location.z }
            end

            local ok_vel, vel = pcall(function() return pawn:GetVelocity() end)
            if ok_vel and vel ~= nil then
                velocity = { x = vel.x, y = vel.y, z = vel.z }
            end

            break
        end
    end

    local speed = math.sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z)

    return { position = position, velocity = velocity, speed = speed }
end

--- TRIZ-inspired spatio-temporal niche construction: quantizes the current
--- proprioceptive sample into a niche cell, records the visit, and decays
--- the "pressure" (accumulated exploitation) of every other niche.
local function update_niche(proprioception, delta)
    local cfg = gtangel.config
    local st = gtangel.state

    local cell_x = math.floor(proprioception.position.x / cfg.niche_cell_size)
    local cell_y = math.floor(proprioception.position.y / cfg.niche_cell_size)
    local time_bucket = get_time_bucket(st.elapsed_time)
    local key = niche_key(cell_x, cell_y, time_bucket)

    local niche = st.niches[key]
    local is_novel = niche == nil

    if is_novel then
        niche = { visits = 0, pressure = 0.0, last_seen = st.elapsed_time }
        st.niches[key] = niche
    end

    niche.visits = niche.visits + 1
    niche.pressure = niche.pressure + 1.0
    niche.last_seen = st.elapsed_time

    -- Ideality/contradiction resolution: as pressure builds on the current
    -- niche without resolution, bias toward exploiting known-good niches;
    -- otherwise keep exploring for new ones. This is a lightweight stand-in
    -- for TRIZ's "resolve the contradiction rather than compromise" idea -
    -- the agent doesn't average explore/exploit, it switches modes outright.
    local saturation = math.min(niche.pressure / cfg.pressure_saturation, 1.0)
    if saturation >= cfg.exploration_bias_threshold then
        st.mode = "exploit"
    else
        st.mode = "explore"
    end

    return key, is_novel, saturation
end

--- Adaptive endocrine reward system: dopamine spikes on novelty, cortisol
--- rises with stagnation/over-exploitation. Both decay each tick, and the
--- net reward is dopamine minus cortisol, giving the agent an adaptive
--- scalar signal that reflects both curiosity and homeostatic stress.
local function update_endocrine(is_novel, saturation, delta)
    local cfg = gtangel.config
    local st = gtangel.state

    st.dopamine = st.dopamine * cfg.dopamine_decay
    st.cortisol = st.cortisol * cfg.cortisol_decay

    if is_novel then
        st.dopamine = st.dopamine + cfg.dopamine_novelty_gain
    end

    if saturation >= 1.0 then
        st.cortisol = st.cortisol + cfg.cortisol_stagnation_gain
    end

    st.reward = st.dopamine - st.cortisol
end

-- ------------------------------------------------------------------------
-- Public API (usable from other scripts / a cognitive-architecture bridge)
-- ------------------------------------------------------------------------

--- Returns the current adaptive scalar reward for an external agent.
function gtangel.get_reward()
    return gtangel.state.reward
end

--- Returns the current explore/exploit mode chosen by niche construction.
function gtangel.get_mode()
    return gtangel.state.mode
end

--- Returns the most recent sampled proprioceptive feedback sample.
function gtangel.get_proprioception()
    return gtangel.state.last_proprioception
end

--- Returns a snapshot table describing the full sensorimotor/reward state,
--- suitable for serialization to an external cognitive-architecture process.
function gtangel.get_snapshot()
    local st = gtangel.state
    local niche_count = 0
    for _ in pairs(st.niches) do
        niche_count = niche_count + 1
    end

    return {
        elapsed_time = st.elapsed_time,
        proprioception = st.last_proprioception,
        mode = st.mode,
        dopamine = st.dopamine,
        cortisol = st.cortisol,
        reward = st.reward,
        niche_count = niche_count,
    }
end

--- Persists the learned niche map and hormone levels to disk so that a run
--- can be resumed across sessions.
function gtangel.save_state(path)
    path = path or "gtangel_agi_state.json"
    json.dump_file(path, {
        niches = gtangel.state.niches,
        dopamine = gtangel.state.dopamine,
        cortisol = gtangel.state.cortisol,
    }, 4)
end

--- Restores a previously persisted niche map and hormone levels.
function gtangel.load_state(path)
    path = path or "gtangel_agi_state.json"
    local ok, data = pcall(function() return json.load_file(path) end)
    if ok and data ~= nil then
        gtangel.state.niches = data.niches or {}
        gtangel.state.dopamine = data.dopamine or 0.0
        gtangel.state.cortisol = data.cortisol or 0.0
    end
end

-- ------------------------------------------------------------------------
-- Engine wiring
-- ------------------------------------------------------------------------

uevr.sdk.callbacks.on_post_engine_tick(function(engine, delta)
    delta = delta or 0.0
    gtangel.state.elapsed_time = gtangel.state.elapsed_time + delta

    local proprioception = sample_proprioception()
    gtangel.state.last_proprioception = proprioception

    local _, is_novel, saturation = update_niche(proprioception, delta)
    update_endocrine(is_novel, saturation, delta)
end)

uevr.sdk.callbacks.on_draw_ui(function()
    local st = gtangel.state
    local snapshot = gtangel.get_snapshot()

    imgui.text("GTAngel AGI Gameworld Testing Environment")
    imgui.text(string.format("Mode: %s", snapshot.mode))
    imgui.text(string.format("Reward: %.3f (dopamine %.3f / cortisol %.3f)",
        snapshot.reward, snapshot.dopamine, snapshot.cortisol))
    imgui.text(string.format("Niches discovered: %d", snapshot.niche_count))
    imgui.text(string.format("Speed: %.2f", st.last_proprioception.speed))

    if imgui.button("Save GTAngel state") then
        gtangel.save_state()
    end
end)

uevr.sdk.callbacks.on_script_reset(function()
    print("Resetting gtangel_agi_testbed.lua")
    gtangel.state.niches = {}
    gtangel.state.dopamine = 0.0
    gtangel.state.cortisol = 0.0
    gtangel.state.reward = 0.0
    gtangel.state.elapsed_time = 0.0
end)

-- Expose the module globally so other scripts (or a future native bridge)
-- can query reward/state without re-implementing the sensorimotor loop.
_G.GTAngelAGI = gtangel

return gtangel
