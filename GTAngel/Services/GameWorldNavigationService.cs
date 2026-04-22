using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

// ── Liberty City World Model ──────────────────────────────────────────────────

/// <summary>
/// KSM Cycle 4: Game World Navigation Service
/// Provides a structured Liberty City world model with:
///   - 3 districts (Portland, Staunton Island, Shoreside Vale)
///   - 45 Points of Interest (POIs) with categories and positions
///   - Road network graph with A* pathfinding
///   - Curiosity-weighted POI selection (prefer unvisited/novel)
///   - Visited-POI memory with recency decay
///   - Human-like route planning (prefer scenic routes, avoid repetition)
///   - District coverage tracking (per-district exploration %)
///
/// Composition: /echo ( /gta3-ue5-wpf ) → /ksm-evolve [Cycle 4]
/// Alexander properties strengthened: P5 Positive Space, P8 Deep Interlock,
/// P10 Gradients, P12 Echoes, P15 Not-Separateness
/// </summary>
public sealed class GameWorldNavigationService
{
    private readonly ILogger _logger;

    // ── World Model ──────────────────────────────────────────────────────────
    public List<District> Districts { get; } = new();
    public List<PointOfInterest> POIs { get; } = new();
    public List<RoadNode> RoadGraph { get; } = new();

    // ── Navigation State ─────────────────────────────────────────────────────
    public PointOfInterest? CurrentPOI { get; private set; }
    public PointOfInterest? NextPOI { get; private set; }
    public District? CurrentDistrict { get; private set; }
    public List<RoadNode> CurrentRoute { get; private set; } = new();
    public int RouteIndex { get; private set; }
    public float RouteProgress => CurrentRoute.Count > 0
        ? (float)RouteIndex / CurrentRoute.Count : 0f;
    public int VisitedPOICount => _visitedPOIs.Count;
    public string NavigationMode { get; private set; } = "Exploring";

    // ── Memory ───────────────────────────────────────────────────────────────
    private readonly Dictionary<string, DateTime> _visitedPOIs = new();
    private readonly Dictionary<string, int> _poiVisitCounts = new();
    private readonly HashSet<(int, int)> _visitedCellsPerDistrict_Portland = new();
    private readonly HashSet<(int, int)> _visitedCellsPerDistrict_Staunton = new();
    private readonly HashSet<(int, int)> _visitedCellsPerDistrict_Shoreside = new();

    // ── Events ───────────────────────────────────────────────────────────────
    public event EventHandler<PointOfInterest>? OnPOIReached;
    public event EventHandler<PointOfInterest>? OnPOISelected;
    public event EventHandler<District>? OnDistrictChanged;
    public event EventHandler<RouteInfo>? OnRouteUpdated;
    public event EventHandler<string>? OnNavigationLog;

    // ── Convenience Properties ─────────────────────────────────────────────────
    public int TotalPOICount => POIs.Count;
    public int DistrictCount => Districts.Count;

    /// <summary>Explicit init for callers that want a clear lifecycle step.</summary>
    public void Initialize() { /* Already initialized in constructor */ }

    // ── Constants ─────────────────────────────────────────────────────────────
    private const float POI_REACH_RADIUS = 80f;     // UU
    private const float CELL_SIZE = 50f;             // UU per grid cell
    private const int CELLS_PER_DISTRICT = 900;      // 30×30 grid per district
    private const float CURIOSITY_DECAY = 0.95f;     // Recency decay per visit
    private const float NOVELTY_BONUS = 2.0f;        // Weight for unvisited POIs

    public GameWorldNavigationService(ILogger<GameWorldNavigationService> logger)
    {
        _logger = logger;
        InitializeWorldModel();
        _logger.LogInformation("GameWorldNavigationService initialized: {Districts} districts, {POIs} POIs, {Nodes} road nodes",
            Districts.Count, POIs.Count, RoadGraph.Count);
    }

    // ── World Model Initialization ───────────────────────────────────────────

    private void InitializeWorldModel()
    {
        // ── Districts ────────────────────────────────────────────────────────
        Districts.Add(new District
        {
            Id = "portland", Name = "Portland",
            BoundsMin = new float[] { -1500, -1500 },
            BoundsMax = new float[] { -200, 500 },
            Description = "Industrial district, Triad territory, docks and warehouses"
        });
        Districts.Add(new District
        {
            Id = "staunton", Name = "Staunton Island",
            BoundsMin = new float[] { -200, -1000 },
            BoundsMax = new float[] { 800, 1000 },
            Description = "Commercial district, Yakuza territory, skyscrapers and shops"
        });
        Districts.Add(new District
        {
            Id = "shoreside", Name = "Shoreside Vale",
            BoundsMin = new float[] { 800, -1000 },
            BoundsMax = new float[] { 1500, 1000 },
            Description = "Suburban district, Cartel territory, mansions and airport"
        });

        // ── Points of Interest (45 POIs across 3 districts) ──────────────────
        // Portland (15 POIs)
        AddPOI("portland", "Salvatore's Mansion",  "landmark",   -1200, -800);
        AddPOI("portland", "Cipriani's Restaurant", "food",       -1100, -600);
        AddPOI("portland", "Ammu-Nation Portland",  "shop",       -900,  -400);
        AddPOI("portland", "Portland Docks",        "industrial", -1400, -200);
        AddPOI("portland", "Red Light District",    "district",   -800,  -300);
        AddPOI("portland", "Triad Fish Factory",    "industrial", -1300, -100);
        AddPOI("portland", "Portland Harbor",       "transport",  -1400, 200);
        AddPOI("portland", "Luigi's Club",          "entertainment", -700, -500);
        AddPOI("portland", "Joey's Garage",         "service",    -600,  -700);
        AddPOI("portland", "Toni's Safehouse",      "safehouse",  -1000, -500);
        AddPOI("portland", "Portland Hospital",     "medical",    -500,  -200);
        AddPOI("portland", "Callahan Bridge (P)",   "bridge",     -250,  -100);
        AddPOI("portland", "Portland Pay'n'Spray",  "service",    -800,  100);
        AddPOI("portland", "Hepburn Heights",       "residential",-600,  200);
        AddPOI("portland", "Portland View",         "scenic",     -400,  400);

        // Staunton Island (15 POIs)
        AddPOI("staunton", "Asuka's Condo",         "landmark",   100,   500);
        AddPOI("staunton", "Kenji's Casino",         "entertainment", 200, 300);
        AddPOI("staunton", "Staunton Hospital",      "medical",    400,   200);
        AddPOI("staunton", "Liberty Campus",         "education",  300,   -200);
        AddPOI("staunton", "Bedford Point",          "commercial", 500,   100);
        AddPOI("staunton", "Belleville Park",        "park",       100,   -400);
        AddPOI("staunton", "Church",                 "landmark",   600,   -100);
        AddPOI("staunton", "Ammu-Nation Staunton",   "shop",       400,   400);
        AddPOI("staunton", "Staunton Safehouse",     "safehouse",  200,   100);
        AddPOI("staunton", "Callahan Bridge (S)",    "bridge",     -100,  -100);
        AddPOI("staunton", "Shoreside Lift Bridge",  "bridge",     750,   0);
        AddPOI("staunton", "Phil Cassidy's Army",    "shop",       600,   600);
        AddPOI("staunton", "Newport",                "residential",300,   700);
        AddPOI("staunton", "Torrington",             "commercial", 500,   -500);
        AddPOI("staunton", "Staunton Pay'n'Spray",   "service",   700,   300);

        // Shoreside Vale (15 POIs)
        AddPOI("shoreside", "Cochrane Dam",           "landmark",   1200,  500);
        AddPOI("shoreside", "Francis Int'l Airport",  "transport",  1300,  -800);
        AddPOI("shoreside", "Cartel Mansion",         "landmark",   1100,  300);
        AddPOI("shoreside", "Cedar Grove",            "residential",900,   200);
        AddPOI("shoreside", "Wichita Gardens",        "residential",1000,  -200);
        AddPOI("shoreside", "Pike Creek",             "industrial", 1100,  -400);
        AddPOI("shoreside", "Shoreside Hospital",     "medical",    1200,  -100);
        AddPOI("shoreside", "Ammu-Nation Shoreside",  "shop",       1000,  100);
        AddPOI("shoreside", "Shoreside Safehouse",    "safehouse",  900,   400);
        AddPOI("shoreside", "Airport Runway",         "transport",  1400,  -600);
        AddPOI("shoreside", "Shoreside Pay'n'Spray",  "service",    1100,  0);
        AddPOI("shoreside", "Catalina's Hideout",     "landmark",   1300,  700);
        AddPOI("shoreside", "Observatory",            "scenic",     1400,  400);
        AddPOI("shoreside", "Shoreside Lift Bridge",  "bridge",     850,   0);
        AddPOI("shoreside", "Import/Export Garage",   "service",    1200,  -300);

        // ── Road Graph (simplified node network) ─────────────────────────────
        BuildRoadGraph();
    }

    private void AddPOI(string districtId, string name, string category, float x, float y)
    {
        POIs.Add(new PointOfInterest
        {
            Id = $"{districtId}_{name.Replace(" ", "_").Replace("'", "").ToLower()}",
            Name = name,
            Category = category,
            DistrictId = districtId,
            Position = new float[] { x, y, 0 }
        });
    }

    private void BuildRoadGraph()
    {
        // Create road nodes at POI locations and key intersections
        var nodeMap = new Dictionary<string, RoadNode>();

        foreach (var poi in POIs)
        {
            var node = new RoadNode
            {
                Id = poi.Id,
                Position = poi.Position,
                DistrictId = poi.DistrictId,
                IsPOI = true,
                POIName = poi.Name
            };
            RoadGraph.Add(node);
            nodeMap[node.Id] = node;
        }

        // Connect nodes within each district (nearest-neighbor with max distance)
        foreach (var district in Districts)
        {
            var districtNodes = RoadGraph.Where(n => n.DistrictId == district.Id).ToList();
            foreach (var node in districtNodes)
            {
                var nearest = districtNodes
                    .Where(n => n.Id != node.Id)
                    .OrderBy(n => Distance2D(node.Position, n.Position))
                    .Take(4); // Connect to 4 nearest neighbors

                foreach (var neighbor in nearest)
                {
                    var dist = Distance2D(node.Position, neighbor.Position);
                    if (dist < 800f) // Max connection distance
                    {
                        node.Neighbors.Add((neighbor.Id, dist));
                    }
                }
            }
        }

        // Connect bridge nodes between districts
        ConnectBridgeNodes(nodeMap);
    }

    private void ConnectBridgeNodes(Dictionary<string, RoadNode> nodeMap)
    {
        // Callahan Bridge: Portland ↔ Staunton
        var calP = nodeMap.GetValueOrDefault("portland_callahan_bridge_p");
        var calS = nodeMap.GetValueOrDefault("staunton_callahan_bridge_s");
        if (calP != null && calS != null)
        {
            var dist = Distance2D(calP.Position, calS.Position);
            calP.Neighbors.Add((calS.Id, dist));
            calS.Neighbors.Add((calP.Id, dist));
        }

        // Shoreside Lift Bridge: Staunton ↔ Shoreside
        var liftS = nodeMap.GetValueOrDefault("staunton_shoreside_lift_bridge");
        var liftSh = nodeMap.GetValueOrDefault("shoreside_shoreside_lift_bridge");
        if (liftS != null && liftSh != null)
        {
            var dist = Distance2D(liftS.Position, liftSh.Position);
            liftS.Neighbors.Add((liftSh.Id, dist));
            liftSh.Neighbors.Add((liftS.Id, dist));
        }
    }

    // ── Navigation API ───────────────────────────────────────────────────────

    /// <summary>
    /// Update the avatar's current position and trigger navigation events.
    /// Called every exploration step from DTE4EAvatarService.
    /// </summary>
    public void UpdatePosition(float[] position)
    {
        // Update district
        var newDistrict = IdentifyDistrict(position);
        if (newDistrict != null && newDistrict.Id != CurrentDistrict?.Id)
        {
            CurrentDistrict = newDistrict;
            OnDistrictChanged?.Invoke(this, newDistrict);
            Log($"Entered district: {newDistrict.Name}");
        }

        // Update district coverage grid
        UpdateDistrictCoverage(position);

        // Check if we reached the current target POI
        if (NextPOI != null)
        {
            var distToPOI = Distance2D(position, NextPOI.Position);
            if (distToPOI < POI_REACH_RADIUS)
            {
                ReachPOI(NextPOI);
            }
        }

        // Check proximity to any POI
        foreach (var poi in POIs)
        {
            var dist = Distance2D(position, poi.Position);
            if (dist < POI_REACH_RADIUS && !IsRecentlyVisited(poi.Id))
            {
                ReachPOI(poi);
            }
        }

        // Update route progress
        if (CurrentRoute.Count > 0 && RouteIndex < CurrentRoute.Count)
        {
            var routeNode = CurrentRoute[RouteIndex];
            if (Distance2D(position, routeNode.Position) < POI_REACH_RADIUS)
            {
                RouteIndex++;
            }
        }

        EmitRouteUpdate();
    }

    /// <summary>
    /// Select the next POI to navigate to, weighted by curiosity and novelty.
    /// Returns the direction vector toward the next waypoint on the route.
    /// </summary>
    public float[] SelectNextDestination(float[] currentPosition, float curiosity)
    {
        // Score all POIs by novelty × curiosity × distance
        var candidates = POIs
            .Select(poi => new
            {
                POI = poi,
                Score = ComputePOIScore(poi, currentPosition, curiosity)
            })
            .OrderByDescending(c => c.Score)
            .ToList();

        if (candidates.Count == 0)
            return new float[] { 0, 1 }; // Default: forward

        // Select from top candidates with some randomness (human-like)
        var rng = new Random();
        var topN = Math.Min(5, candidates.Count);
        var selected = candidates[rng.Next(topN)];

        NextPOI = selected.POI;
        NavigationMode = $"Navigating to {selected.POI.Name}";
        OnPOISelected?.Invoke(this, selected.POI);
        Log($"Selected POI: {selected.POI.Name} (score={selected.Score:F2}, cat={selected.POI.Category})");

        // Plan route using A*
        CurrentRoute = FindRoute(currentPosition, selected.POI.Position);
        RouteIndex = 0;

        // Return direction to first route node (or directly to POI)
        var target = CurrentRoute.Count > 0 ? CurrentRoute[0].Position : selected.POI.Position;
        return DirectionTo(currentPosition, target);
    }

    /// <summary>
    /// Get the next waypoint direction for the current route.
    /// Returns normalized 2D direction vector.
    /// </summary>
    public float[] GetNextWaypointDirection(float[] currentPosition)
    {
        if (CurrentRoute.Count == 0 || RouteIndex >= CurrentRoute.Count)
        {
            // No active route — select new destination
            return SelectNextDestination(currentPosition, 0.5f);
        }

        var target = CurrentRoute[RouteIndex].Position;
        return DirectionTo(currentPosition, target);
    }

    /// <summary>
    /// Get district coverage as a percentage [0,1] for each district.
    /// </summary>
    public Dictionary<string, float> GetDistrictCoverage()
    {
        return new Dictionary<string, float>
        {
            ["portland"]  = Math.Min(_visitedCellsPerDistrict_Portland.Count / (float)CELLS_PER_DISTRICT, 1f),
            ["staunton"]  = Math.Min(_visitedCellsPerDistrict_Staunton.Count / (float)CELLS_PER_DISTRICT, 1f),
            ["shoreside"] = Math.Min(_visitedCellsPerDistrict_Shoreside.Count / (float)CELLS_PER_DISTRICT, 1f),
        };
    }

    /// <summary>
    /// Compute the exploration score (0..1) — weighted average of district coverage.
    /// </summary>
    public float GetExplorationScore()
    {
        var cov = GetDistrictCoverage();
        return (cov["portland"] + cov["staunton"] + cov["shoreside"]) / 3f;
    }

    // ── A* Pathfinding ───────────────────────────────────────────────────────

    private List<RoadNode> FindRoute(float[] from, float[] to)
    {
        // Find nearest road nodes to start and end
        var startNode = FindNearestNode(from);
        var endNode   = FindNearestNode(to);

        if (startNode == null || endNode == null)
            return new List<RoadNode>();

        // A* search
        var openSet = new SortedSet<(float fScore, string nodeId)>(
            Comparer<(float, string)>.Create((a, b) =>
            {
                var cmp = a.Item1.CompareTo(b.Item1);
                return cmp != 0 ? cmp : string.Compare(a.Item2, b.Item2, StringComparison.Ordinal);
            }));
        var cameFrom = new Dictionary<string, string>();
        var gScore   = new Dictionary<string, float>();
        var nodeById = RoadGraph.ToDictionary(n => n.Id);

        gScore[startNode.Id] = 0;
        openSet.Add((Distance2D(startNode.Position, to), startNode.Id));

        while (openSet.Count > 0)
        {
            var current = openSet.Min;
            openSet.Remove(current);
            var currentId = current.nodeId;

            if (currentId == endNode.Id)
            {
                // Reconstruct path
                var path = new List<RoadNode>();
                var id = endNode.Id;
                while (cameFrom.ContainsKey(id))
                {
                    path.Add(nodeById[id]);
                    id = cameFrom[id];
                }
                path.Add(nodeById[id]);
                path.Reverse();
                return path;
            }

            if (!nodeById.TryGetValue(currentId, out var node))
                continue;

            foreach (var (neighborId, edgeCost) in node.Neighbors)
            {
                var tentativeG = gScore.GetValueOrDefault(currentId, float.MaxValue) + edgeCost;
                if (tentativeG < gScore.GetValueOrDefault(neighborId, float.MaxValue))
                {
                    cameFrom[neighborId] = currentId;
                    gScore[neighborId] = tentativeG;
                    var fScore = tentativeG + Distance2D(nodeById[neighborId].Position, to);
                    openSet.Add((fScore, neighborId));
                }
            }
        }

        // No path found — return direct line
        return new List<RoadNode> { endNode };
    }

    private RoadNode? FindNearestNode(float[] position)
    {
        return RoadGraph
            .OrderBy(n => Distance2D(n.Position, position))
            .FirstOrDefault();
    }

    // ── POI Scoring ──────────────────────────────────────────────────────────

    private float ComputePOIScore(PointOfInterest poi, float[] currentPosition, float curiosity)
    {
        var distance = Distance2D(currentPosition, poi.Position);
        var distancePenalty = 1f / (1f + distance / 500f); // Prefer closer POIs slightly

        // Novelty: unvisited POIs get a big bonus
        float novelty;
        if (!_visitedPOIs.ContainsKey(poi.Id))
        {
            novelty = NOVELTY_BONUS;
        }
        else
        {
            // Recency decay: older visits are less penalizing
            var timeSinceVisit = (DateTime.UtcNow - _visitedPOIs[poi.Id]).TotalMinutes;
            var visitCount = _poiVisitCounts.GetValueOrDefault(poi.Id, 1);
            novelty = (float)(Math.Pow(CURIOSITY_DECAY, visitCount) * Math.Min(timeSinceVisit / 10.0, 1.0));
        }

        // Category bonus: prefer diverse categories
        var categoryBonus = GetCategoryBonus(poi.Category);

        // Curiosity amplifies novelty
        var curiosityWeight = 0.5f + curiosity * 1.5f;

        return (novelty * curiosityWeight + distancePenalty * 0.3f + categoryBonus * 0.2f);
    }

    private float GetCategoryBonus(string category)
    {
        // Prefer categories that haven't been visited recently
        var recentCategoryCounts = _visitedPOIs.Keys
            .Where(id => (DateTime.UtcNow - _visitedPOIs[id]).TotalMinutes < 5)
            .Select(id => POIs.FirstOrDefault(p => p.Id == id)?.Category)
            .Where(c => c != null)
            .GroupBy(c => c!)
            .ToDictionary(g => g.Key, g => g.Count());

        return recentCategoryCounts.ContainsKey(category) ? 0.2f : 1.0f;
    }

    // ── POI Visit Tracking ───────────────────────────────────────────────────

    private void ReachPOI(PointOfInterest poi)
    {
        CurrentPOI = poi;
        _visitedPOIs[poi.Id] = DateTime.UtcNow;
        _poiVisitCounts[poi.Id] = _poiVisitCounts.GetValueOrDefault(poi.Id, 0) + 1;

        OnPOIReached?.Invoke(this, poi);
        Log($"Reached POI: {poi.Name} ({poi.Category}) — visit #{_poiVisitCounts[poi.Id]}");

        // If this was the target, clear route
        if (NextPOI?.Id == poi.Id)
        {
            NextPOI = null;
            CurrentRoute.Clear();
            RouteIndex = 0;
            NavigationMode = "Exploring";
        }
    }

    private bool IsRecentlyVisited(string poiId)
    {
        return _visitedPOIs.TryGetValue(poiId, out var lastVisit) &&
               (DateTime.UtcNow - lastVisit).TotalMinutes < 2;
    }

    // ── District Coverage ────────────────────────────────────────────────────

    private void UpdateDistrictCoverage(float[] position)
    {
        var cellX = (int)(position[0] / CELL_SIZE);
        var cellY = (int)(position[1] / CELL_SIZE);
        var cell = (cellX, cellY);

        var district = IdentifyDistrict(position);
        if (district == null) return;

        switch (district.Id)
        {
            case "portland":  _visitedCellsPerDistrict_Portland.Add(cell); break;
            case "staunton":  _visitedCellsPerDistrict_Staunton.Add(cell); break;
            case "shoreside": _visitedCellsPerDistrict_Shoreside.Add(cell); break;
        }
    }

    private District? IdentifyDistrict(float[] position)
    {
        foreach (var d in Districts)
        {
            if (position[0] >= d.BoundsMin[0] && position[0] <= d.BoundsMax[0] &&
                position[1] >= d.BoundsMin[1] && position[1] <= d.BoundsMax[1])
                return d;
        }
        return Districts.FirstOrDefault(); // Default to Portland
    }

    // ── Utility ──────────────────────────────────────────────────────────────

    private static float Distance2D(float[] a, float[] b)
    {
        var dx = a[0] - b[0];
        var dy = a[1] - b[1];
        return MathF.Sqrt(dx * dx + dy * dy);
    }

    private static float[] DirectionTo(float[] from, float[] to)
    {
        var dx = to[0] - from[0];
        var dy = to[1] - from[1];
        var len = MathF.Sqrt(dx * dx + dy * dy);
        if (len < 0.001f) return new float[] { 0, 1 };
        return new float[] { dx / len, dy / len };
    }

    private void Log(string message)
    {
        _logger.LogDebug("[Nav] {Message}", message);
        OnNavigationLog?.Invoke(this, message);
    }

    private void EmitRouteUpdate()
    {
        var cov = GetDistrictCoverage();
        var districtCov = CurrentDistrict != null && cov.TryGetValue(CurrentDistrict.Id, out var dc) ? dc : 0f;
        var info = new RouteInfo
        {
            Status           = CurrentRoute.Count > 0 ? $"Route: {RouteIndex}/{CurrentRoute.Count}" : "No route",
            TargetPOI        = NextPOI?.Name ?? "None",
            TotalWaypoints   = CurrentRoute.Count,
            CurrentWaypoint  = RouteIndex,
            DistanceToTarget = NextPOI != null && CurrentDistrict != null ? Distance2D(CurrentDistrict.BoundsMin, NextPOI.Position) : 0f,
            Mode             = NavigationMode,
            IsActive         = CurrentRoute.Count > 0,
            DistrictCoverage = districtCov
        };
        OnRouteUpdated?.Invoke(this, info);
    }
}

// ── Models ───────────────────────────────────────────────────────────────────

public class District
{
    public string Id { get; set; } = "";
    public string Name { get; set; } = "";
    public string Description { get; set; } = "";
    public float[] BoundsMin { get; set; } = new float[2];
    public float[] BoundsMax { get; set; } = new float[2];
}

public class PointOfInterest
{
    public string Id { get; set; } = "";
    public string Name { get; set; } = "";
    public string Category { get; set; } = "";
    public string DistrictId { get; set; } = "";
    public float[] Position { get; set; } = new float[3];
}

public class RoadNode
{
    public string Id { get; set; } = "";
    public float[] Position { get; set; } = new float[3];
    public string DistrictId { get; set; } = "";
    public bool IsPOI { get; set; }
    public string? POIName { get; set; }
    public List<(string NeighborId, float Cost)> Neighbors { get; } = new();
}

public class RouteInfo
{
    public string Status { get; set; } = "No route";
    public string TargetPOI { get; set; } = "None";
    public int TotalWaypoints { get; set; }
    public int CurrentWaypoint { get; set; }
    public float DistanceToTarget { get; set; }
    public string Mode { get; set; } = "Idle";
    public bool IsActive { get; set; }
    public float DistrictCoverage { get; set; }
}
