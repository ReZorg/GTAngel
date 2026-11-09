-- SensorimotorBridge
--
-- Maps a "frame buffer" (a flat array of sampled feature/pixel values, e.g.
-- luminance samples taken from the rendered scene) into the AI's
-- proprioceptive feedback loop:
--
--   motor output -> gameworld change -> rendered frame -> sampled frame
--   buffer -> prediction error / spatial novelty -> endocrine reward -> next
--   motor output
--
-- This module owns no REFramework/game specific state; it only consumes
-- plain Lua values (position, a features array, a threat scalar) so it can
-- be exercised in isolation (see tests/agi_spec.lua) as well as from the
-- live REFramework autorun hook.
local NicheConstructionMap = require("agi/NicheConstructionMap")
local EndocrineRewardSystem = require("agi/EndocrineRewardSystem")

local SensorimotorBridge = {}
SensorimotorBridge.__index = SensorimotorBridge

function SensorimotorBridge.new(opts)
    opts = opts or {}

    return setmetatable({
        niches = NicheConstructionMap.new(opts.cell_size, opts.recovery_seconds),
        endocrine = EndocrineRewardSystem.new(opts.channel_config),
        previous_features = nil,
        history = {},
        history_limit = opts.history_limit or 256,
        time = 0.0,
    }, SensorimotorBridge)
end

-- Mean absolute difference between two equally-sized numeric feature arrays,
-- used as a lightweight stand-in for a learned forward-model prediction
-- error (i.e. "surprise").
local function prediction_error(previous, current)
    if not previous or #previous == 0 or #previous ~= #current then
        return 1.0 -- maximal surprise when there is nothing to compare against
    end

    local sum = 0.0
    for i = 1, #current do
        sum = sum + math.abs(current[i] - previous[i])
    end

    return sum / #current
end

-- position: {x, y, z} world-space position of the observed agent
-- frame_features: flat array of sampled frame buffer values (any scale)
-- threat_level: scalar in [0, 1] describing danger/failure signal this tick
-- dt: elapsed seconds since the previous update
--
-- Returns a telemetry table describing this tick's sensorimotor feedback.
function SensorimotorBridge:update(position, frame_features, threat_level, dt)
    dt = dt or 0
    self.time = self.time + dt

    local novelty = self.niches:visit(position.x, position.y, position.z, self.time)
    local surprise = prediction_error(self.previous_features, frame_features)

    local reward, hormones = self.endocrine:update({
        dopamine = math.min(1.0, (novelty * 0.6) + (surprise * 0.4)),
        cortisol = threat_level or 0.0,
        oxytocin = math.max(0.0, 1.0 - surprise),
    }, dt)

    self.previous_features = frame_features

    local telemetry = {
        time = self.time,
        novelty = novelty,
        surprise = surprise,
        reward = reward,
        hormones = hormones,
        niche_count = self.niches:cell_count(),
    }

    table.insert(self.history, telemetry)
    while #self.history > self.history_limit do
        table.remove(self.history, 1)
    end

    return telemetry
end

function SensorimotorBridge:latest()
    return self.history[#self.history]
end

function SensorimotorBridge:reset()
    self.niches:reset()
    self.endocrine:reset()
    self.previous_features = nil
    self.history = {}
    self.time = 0.0
end

return SensorimotorBridge
