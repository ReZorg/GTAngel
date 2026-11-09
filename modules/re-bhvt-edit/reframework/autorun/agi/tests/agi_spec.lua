-- Lightweight self-test harness for the pure-Lua AGI testing environment
-- modules (NicheConstructionMap, EndocrineRewardSystem, SensorimotorBridge).
--
-- These modules have no dependency on the REFramework/`sdk`/`imgui`/`re`
-- globals, so they can be exercised directly with a stock Lua interpreter:
--
--   lua5.4 reframework/autorun/agi/tests/agi_spec.lua
--
-- The script exits with a non-zero status and prints the failing assertion
-- if any check fails, and prints "All AGI module tests passed." otherwise.
package.path = package.path
    .. ";" .. (arg and arg[0]:match("(.*/)tests/agi_spec%.lua$") or "./") .. "?.lua"
    .. ";" .. (arg and arg[0]:match("(.*/)tests/agi_spec%.lua$") or "./") .. "../?.lua"

local NicheConstructionMap = require("NicheConstructionMap")
local EndocrineRewardSystem = require("EndocrineRewardSystem")
local SensorimotorBridge = require("SensorimotorBridge")

local checks = 0

local function check(condition, message)
    checks = checks + 1
    if not condition then
        error("FAILED: " .. message, 2)
    end
end

-- NicheConstructionMap: first visit to a cell is maximally novel.
do
    local map = NicheConstructionMap.new(1.0, 10.0)
    local novelty = map:visit(0, 0, 0, 0)
    check(novelty == 1.0, "first visit to a niche should be fully novel")
    check(map:cell_count() == 1, "one niche should have been recorded")
end

-- NicheConstructionMap: immediate re-visit is much less novel (habituation).
do
    local map = NicheConstructionMap.new(1.0, 10.0)
    map:visit(0, 0, 0, 0)
    local novelty = map:visit(0, 0, 0, 0.1)
    check(novelty < 1.0, "immediate revisit should be less novel than first visit")
end

-- NicheConstructionMap: novelty recovers after the recovery window elapses.
do
    local map = NicheConstructionMap.new(1.0, 10.0)
    map:visit(0, 0, 0, 0)
    map:visit(0, 0, 0, 0.1) -- drains novelty
    local recovered = map:visit(0, 0, 0, 100.0) -- long past recovery_seconds
    check(recovered > 0.9, "novelty should substantially recover after the recovery window")
end

-- EndocrineRewardSystem: a novelty stimulus produces positive composite reward.
do
    local endocrine = EndocrineRewardSystem.new()
    local reward = endocrine:update({ dopamine = 1.0 }, 0.1)
    check(reward > 0, "dopamine stimulus should raise composite reward above zero")
end

-- EndocrineRewardSystem: repeated identical stimulation habituates (reward
-- growth slows down / composite reward for the same stimulus decreases).
do
    local endocrine = EndocrineRewardSystem.new()
    local first_reward = endocrine:update({ dopamine = 1.0 }, 1.0)
    local second_reward = endocrine:update({ dopamine = 1.0 }, 1.0)
    check(second_reward <= first_reward, "repeated stimulation should not increase reward due to habituation")
end

-- EndocrineRewardSystem: cortisol (threat) pulls composite reward down.
do
    local endocrine = EndocrineRewardSystem.new()
    local reward = endocrine:update({ cortisol = 1.0 }, 0.5)
    check(reward < 0, "threat stimulus should produce a negative composite reward")
end

-- SensorimotorBridge: integrates niche novelty + prediction error + reward.
do
    local bridge = SensorimotorBridge.new({ cell_size = 1.0, recovery_seconds = 10.0 })

    local t1 = bridge:update({ x = 0, y = 0, z = 0 }, { 0.1, 0.2, 0.3 }, 0.0, 0.1)
    check(t1.novelty == 1.0, "first bridge update should report full novelty")
    check(t1.niche_count == 1, "first bridge update should construct one niche")

    local t2 = bridge:update({ x = 0, y = 0, z = 0 }, { 0.1, 0.2, 0.3 }, 0.0, 0.1)
    check(t2.novelty < t1.novelty, "revisiting the same niche with identical features should lower novelty")
    check(t2.surprise == 0.0, "identical consecutive frame features should produce zero surprise")

    local t3 = bridge:update({ x = 10, y = 10, z = 0 }, { 0.9, 0.9, 0.9 }, 0.8, 0.1)
    check(t3.novelty == 1.0, "moving to a new niche should report full novelty again")
    check(t3.surprise > 0, "differing frame features should produce nonzero surprise")
    check(t3.niche_count == 2, "moving to a new position should construct a second niche")

    bridge:reset()
    check(bridge:latest() == nil, "reset should clear telemetry history")
end

print(string.format("All AGI module tests passed. (%d checks)", checks))
