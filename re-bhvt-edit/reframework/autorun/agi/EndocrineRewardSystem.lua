-- EndocrineRewardSystem
--
-- A small "adaptive endocrine" reward model. Instead of a single scalar
-- reward, several hormone-like channels are simulated, each with its own
-- decay rate and habituation curve, and combined into a composite reward.
-- This gives the AGI testing harness a reward signal that adapts over time
-- (repeated stimuli produce diminishing reward, matching biological
-- habituation) rather than a flat, stationary reward function.
local EndocrineRewardSystem = {}
EndocrineRewardSystem.__index = EndocrineRewardSystem

local DEFAULT_CHANNELS = {
    -- Rises with novelty/exploration, drives curiosity-seeking behavior.
    dopamine = { decay = 2.0, habituation = 0.35, weight = 1.0 },
    -- Rises with threat/failure signals, suppresses exploration.
    cortisol = { decay = 1.0, habituation = 0.10, weight = -0.6 },
    -- Rises with successful task completion, encourages consistency.
    oxytocin = { decay = 3.0, habituation = 0.05, weight = 0.5 },
}

function EndocrineRewardSystem.new(channel_config)
    local channels = {}

    for name, cfg in pairs(channel_config or DEFAULT_CHANNELS) do
        channels[name] = {
            level = 0.0,
            baseline_sensitivity = 1.0,
            decay = cfg.decay,
            habituation = cfg.habituation,
            weight = cfg.weight,
        }
    end

    return setmetatable({ channels = channels }, EndocrineRewardSystem)
end

-- stimuli: table mapping channel name -> stimulus intensity in [0, 1]
-- dt: elapsed seconds since the last update
-- Returns: composite_reward, per-channel snapshot table
function EndocrineRewardSystem:update(stimuli, dt)
    dt = dt or 0

    local composite = 0.0
    local snapshot = {}

    for name, channel in pairs(self.channels) do
        local stimulus = (stimuli and stimuli[name]) or 0.0

        -- Adaptive sensitivity: repeated stimulation habituates the channel,
        -- rest slowly restores full sensitivity.
        if stimulus > 0 then
            channel.baseline_sensitivity = math.max(0.05,
                channel.baseline_sensitivity - channel.habituation * stimulus * dt)
        else
            channel.baseline_sensitivity = math.min(1.0,
                channel.baseline_sensitivity + channel.habituation * 0.25 * dt)
        end

        local release = stimulus * channel.baseline_sensitivity
        channel.level = math.max(0.0, channel.level + release - channel.level * channel.decay * dt)

        composite = composite + channel.level * channel.weight
        snapshot[name] = {
            level = channel.level,
            sensitivity = channel.baseline_sensitivity,
        }
    end

    return composite, snapshot
end

function EndocrineRewardSystem:reset()
    for _, channel in pairs(self.channels) do
        channel.level = 0.0
        channel.baseline_sensitivity = 1.0
    end
end

return EndocrineRewardSystem
