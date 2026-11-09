-- GTAngel AGI gameworld testing environment
--
-- Experimental integration point for external cognitive-architecture testing
-- of an AI agent inside the gameworld. This bridges the rendered frame
-- buffer to a sensorimotor feedback loop (see agi/SensorimotorBridge.lua)
-- built from a TRIZ-inspired spatio-temporal niche construction model
-- (agi/NicheConstructionMap.lua) and an adaptive endocrine reward system
-- (agi/EndocrineRewardSystem.lua).
--
-- This file only wires the above pure-Lua modules into REFramework's
-- callbacks (re.on_frame / re.on_draw_ui). It intentionally does nothing if
-- loaded outside of REFramework (e.g. under a plain `lua` interpreter for
-- testing), since the `re`/`sdk`/`imgui` globals won't exist in that case.
local SensorimotorBridge = require("agi/SensorimotorBridge")

local bridge = SensorimotorBridge.new({
    cell_size = 1.5,        -- world units per spatio-temporal niche cell
    recovery_seconds = 30,  -- how long an exhausted niche takes to renew
})

local enabled = false
local last_time = os.clock()

-- Samples a coarse "frame buffer" feature vector for the current tick.
-- Real integration would pull this from REFramework's renderer/back buffer;
-- here we fall back to the local player's transform so the loop still has a
-- meaningful, cheap signal to react to when no renderer sample is wired up.
local function sample_frame_features(player)
    if not player then
        return { 0.0, 0.0, 0.0 }
    end

    local transform = player:call("get_Transform")
    if not transform then
        return { 0.0, 0.0, 0.0 }
    end

    local pos = transform:call("get_Position")
    return { pos.x, pos.y, pos.z }
end

local function get_player_position(player)
    if not player then
        return { x = 0, y = 0, z = 0 }
    end

    local transform = player:call("get_Transform")
    if not transform then
        return { x = 0, y = 0, z = 0 }
    end

    return transform:call("get_Position")
end

if type(re) == "table" and type(re.on_frame) == "function" then
    re.on_frame(function()
        if not enabled then
            return
        end

        local now = os.clock()
        local dt = now - last_time
        last_time = now

        local player = type(sdk) == "table" and sdk.get_native_singleton
            and (function()
                local ok, mgr = pcall(function()
                    return sdk.get_managed_singleton("app.CharacterManager")
                end)
                if not ok or not mgr then return nil end
                return mgr:call("get_ManualPlayer")
            end)()
            or nil

        local position = get_player_position(player)
        local features = sample_frame_features(player)

        bridge:update(position, features, 0.0, dt)
    end)
end

if type(re) == "table" and type(re.on_draw_ui) == "function" then
    re.on_draw_ui(function()
        if not imgui.tree_node("GTAngel AGI Testing Environment") then
            return
        end

        local changed
        changed, enabled = imgui.checkbox("Enable sensorimotor feedback loop", enabled)

        local telemetry = bridge:latest()

        if telemetry then
            imgui.text(string.format("Reward: %.3f", telemetry.reward))
            imgui.text(string.format("Novelty: %.3f  Surprise: %.3f", telemetry.novelty, telemetry.surprise))
            imgui.text(string.format("Niches constructed: %d", telemetry.niche_count))

            for name, hormone in pairs(telemetry.hormones) do
                imgui.text(string.format("%s: level=%.3f sensitivity=%.3f", name, hormone.level, hormone.sensitivity))
            end
        else
            imgui.text("No telemetry yet.")
        end

        if imgui.button("Reset niches / hormones") then
            bridge:reset()
        end

        imgui.tree_pop()
    end)
end

return bridge
