-- NicheConstructionMap
--
-- TRIZ-inspired spatio-temporal "niche construction" tracker.
--
-- In niche construction theory, an agent doesn't just adapt to an environment,
-- it actively reshapes it, and its own resulting behavior is shaped by the
-- niches it has already constructed/visited. We model this as a resolved
-- TRIZ contradiction between two opposing requirements:
--   * Exploitation wants familiar (low novelty, "safe") space/time cells.
--   * Exploration wants unfamiliar (high novelty, "interesting") cells.
--
-- Instead of picking a single compromise, the contradiction is separated in
-- time: a cell starts fully novel, quickly loses novelty as it is visited
-- (exploitation phase), and then slowly regains novelty the longer it is left
-- unvisited (a "resource that renews itself", a classic TRIZ ideality
-- pattern). This lets the same spatial location act as both a stable base and
-- a source of fresh reward over the lifetime of the simulation.
local NicheConstructionMap = {}
NicheConstructionMap.__index = NicheConstructionMap

-- cell_size: spatial resolution (world units per grid cell)
-- recovery_seconds: time it takes an exhausted niche to fully regain novelty
function NicheConstructionMap.new(cell_size, recovery_seconds)
    return setmetatable({
        cell_size = cell_size or 1.0,
        recovery_seconds = recovery_seconds or 30.0,
        cells = {},
    }, NicheConstructionMap)
end

function NicheConstructionMap:_key(x, y, z)
    local cs = self.cell_size
    return math.floor(x / cs) .. ":" .. math.floor(y / cs) .. ":" .. math.floor((z or 0) / cs)
end

-- Records a visit to the niche at (x, y, z) at time `now` (seconds) and
-- returns the novelty score in [0, 1] that visit produced (1 = brand new).
function NicheConstructionMap:visit(x, y, z, now)
    now = now or 0

    local key = self:_key(x, y, z)
    local cell = self.cells[key]

    if not cell then
        self.cells[key] = { visits = 1, last_visit = now, novelty = 0.0 }
        return 1.0
    end

    -- Recover novelty the longer the niche has been left alone.
    local elapsed = math.max(0, now - cell.last_visit)
    local recovered = math.min(1.0, elapsed / self.recovery_seconds)
    local novelty = math.min(1.0, cell.novelty + recovered)

    cell.visits = cell.visits + 1
    cell.last_visit = now
    -- Exploiting the niche drains its novelty for next time (habituation).
    cell.novelty = novelty * 0.15

    return novelty
end

function NicheConstructionMap:cell_count()
    local n = 0
    for _ in pairs(self.cells) do
        n = n + 1
    end
    return n
end

function NicheConstructionMap:reset()
    self.cells = {}
end

return NicheConstructionMap
