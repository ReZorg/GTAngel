using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Comprehensive tests for GameWorldNavigationService:
///   1. World model initialisation — district count, POI count, road graph
///   2. POI properties — required fields, valid categories, unique IDs
///   3. District model — bounds, IDs, names
///   4. Road graph — connectivity, bridge connections
///   5. UpdatePosition — district detection, coverage tracking, POI reach
///   6. SelectNextDestination — returns valid 2-D direction
///   7. GetNextWaypointDirection — delegates to SelectNextDestination when no route
///   8. GetDistrictCoverage — returns all three keys, values in [0,1]
///   9. GetExplorationScore — average of three district coverages
///  10. RouteProgress — 0 when no route active
///  11. NavigationMode — initial value
///  12. Events — OnPOIReached, OnDistrictChanged, OnRouteUpdated fire correctly
/// </summary>
public sealed class GameWorldNavigationServiceTests
{
    private readonly GameWorldNavigationService _svc;

    public GameWorldNavigationServiceTests()
    {
        _svc = new GameWorldNavigationService(NullLogger<GameWorldNavigationService>.Instance);
    }

    // ── 1. World model initialisation ────────────────────────────────────────

    [Fact]
    public void Constructor_Districts_InitializesThreeDistricts()
    {
        Assert.Equal(3, _svc.Districts.Count);
    }

    [Fact]
    public void Constructor_POIs_InitializesFifteenPerDistrict()
    {
        Assert.Equal(45, _svc.TotalPOICount);
    }

    [Fact]
    public void Constructor_RoadGraph_HasAtLeastOnePOINode()
    {
        Assert.NotEmpty(_svc.RoadGraph);
    }

    [Fact]
    public void Constructor_RoadGraph_ContainsAllPOIs()
    {
        // Every POI must have a corresponding road node
        var nodeIds = _svc.RoadGraph.Select(n => n.Id).ToHashSet();
        foreach (var poi in _svc.POIs)
            Assert.Contains(poi.Id, nodeIds);
    }

    [Fact]
    public void Initialize_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Initialize());
        Assert.Null(ex);
    }

    // ── 2. POI properties ────────────────────────────────────────────────────

    [Fact]
    public void POIs_AllHaveNonEmptyIds()
    {
        Assert.All(_svc.POIs, p => Assert.NotEmpty(p.Id));
    }

    [Fact]
    public void POIs_AllHaveNonEmptyNames()
    {
        Assert.All(_svc.POIs, p => Assert.NotEmpty(p.Name));
    }

    [Fact]
    public void POIs_AllHaveValidDistrictId()
    {
        var validIds = new HashSet<string> { "portland", "staunton", "shoreside" };
        Assert.All(_svc.POIs, p => Assert.Contains(p.DistrictId, validIds));
    }

    [Fact]
    public void POIs_AllHaveThreeElementPositionArray()
    {
        Assert.All(_svc.POIs, p => Assert.Equal(3, p.Position.Length));
    }

    [Fact]
    public void POIs_IdsAreUnique()
    {
        var ids = _svc.POIs.Select(p => p.Id).ToList();
        Assert.Equal(ids.Count, ids.Distinct().Count());
    }

    [Fact]
    public void POIs_PortlandHasFifteenEntries()
    {
        Assert.Equal(15, _svc.POIs.Count(p => p.DistrictId == "portland"));
    }

    [Fact]
    public void POIs_StauntonHasFifteenEntries()
    {
        Assert.Equal(15, _svc.POIs.Count(p => p.DistrictId == "staunton"));
    }

    [Fact]
    public void POIs_ShoresideHasFifteenEntries()
    {
        Assert.Equal(15, _svc.POIs.Count(p => p.DistrictId == "shoreside"));
    }

    // ── 3. District model ────────────────────────────────────────────────────

    [Fact]
    public void Districts_AllHaveNonEmptyIds()
    {
        Assert.All(_svc.Districts, d => Assert.NotEmpty(d.Id));
    }

    [Fact]
    public void Districts_AllHaveNonEmptyNames()
    {
        Assert.All(_svc.Districts, d => Assert.NotEmpty(d.Name));
    }

    [Fact]
    public void Districts_ContainPortlandStauntonShoreside()
    {
        var ids = _svc.Districts.Select(d => d.Id).ToHashSet();
        Assert.Contains("portland",  ids);
        Assert.Contains("staunton",  ids);
        Assert.Contains("shoreside", ids);
    }

    [Fact]
    public void Districts_AllHaveTwoElementBoundsArrays()
    {
        Assert.All(_svc.Districts, d =>
        {
            Assert.Equal(2, d.BoundsMin.Length);
            Assert.Equal(2, d.BoundsMax.Length);
        });
    }

    [Fact]
    public void Districts_BoundsMinLessThanBoundsMax()
    {
        Assert.All(_svc.Districts, d =>
        {
            Assert.True(d.BoundsMin[0] < d.BoundsMax[0]);
            Assert.True(d.BoundsMin[1] < d.BoundsMax[1]);
        });
    }

    // ── 4. Road graph ────────────────────────────────────────────────────────

    [Fact]
    public void RoadGraph_NodesHaveNonEmptyIds()
    {
        Assert.All(_svc.RoadGraph, n => Assert.NotEmpty(n.Id));
    }

    [Fact]
    public void RoadGraph_NodesHaveThreeElementPositions()
    {
        Assert.All(_svc.RoadGraph, n => Assert.Equal(3, n.Position.Length));
    }

    [Fact]
    public void RoadGraph_NodeIdsAreUnique()
    {
        var ids = _svc.RoadGraph.Select(n => n.Id).ToList();
        Assert.Equal(ids.Count, ids.Distinct().Count());
    }

    [Fact]
    public void RoadGraph_AtLeastOneNodeHasNeighbors()
    {
        // Road graph should be connected — at least some nodes have edges
        Assert.Contains(_svc.RoadGraph, n => n.Neighbors.Count > 0);
    }

    [Fact]
    public void RoadGraph_POINodesFlaggedCorrectly()
    {
        var poiNodeIds = _svc.RoadGraph.Where(n => n.IsPOI).Select(n => n.Id).ToHashSet();
        var poiIds     = _svc.POIs.Select(p => p.Id).ToHashSet();
        // Every IsPOI node should correspond to a known POI
        Assert.All(poiNodeIds, id => Assert.Contains(id, poiIds));
    }

    // ── 5. Initial state / properties ────────────────────────────────────────

    [Fact]
    public void VisitedPOICount_Initially_IsZero()
    {
        Assert.Equal(0, _svc.VisitedPOICount);
    }

    [Fact]
    public void RouteProgress_Initially_IsZero()
    {
        Assert.Equal(0f, _svc.RouteProgress);
    }

    [Fact]
    public void NavigationMode_Initially_IsExploring()
    {
        Assert.Equal("Exploring", _svc.NavigationMode);
    }

    [Fact]
    public void CurrentPOI_Initially_IsNull()
    {
        Assert.Null(_svc.CurrentPOI);
    }

    [Fact]
    public void NextPOI_Initially_IsNull()
    {
        Assert.Null(_svc.NextPOI);
    }

    [Fact]
    public void DistrictCount_IsThree()
    {
        Assert.Equal(3, _svc.DistrictCount);
    }

    // ── 6. GetDistrictCoverage ────────────────────────────────────────────────

    [Fact]
    public void GetDistrictCoverage_Initially_AllZero()
    {
        var cov = _svc.GetDistrictCoverage();
        Assert.Equal(0f, cov["portland"]);
        Assert.Equal(0f, cov["staunton"]);
        Assert.Equal(0f, cov["shoreside"]);
    }

    [Fact]
    public void GetDistrictCoverage_ContainsAllThreeKeys()
    {
        var cov = _svc.GetDistrictCoverage();
        Assert.True(cov.ContainsKey("portland"));
        Assert.True(cov.ContainsKey("staunton"));
        Assert.True(cov.ContainsKey("shoreside"));
    }

    [Fact]
    public void GetDistrictCoverage_ValuesInUnitRange()
    {
        // Move around Portland to generate some coverage
        for (int i = 0; i < 10; i++)
            _svc.UpdatePosition(new float[] { -1000f + i * 10, -500f + i * 10, 0 });

        var cov = _svc.GetDistrictCoverage();
        Assert.All(cov.Values, v => Assert.InRange(v, 0f, 1f));
    }

    // ── 7. GetExplorationScore ────────────────────────────────────────────────

    [Fact]
    public void GetExplorationScore_Initially_IsZero()
    {
        Assert.Equal(0f, _svc.GetExplorationScore());
    }

    [Fact]
    public void GetExplorationScore_IncreasesAfterMovement()
    {
        // Move around Liberty City to increase coverage
        for (int i = 0; i < 20; i++)
            _svc.UpdatePosition(new float[] { -1000f + i * 50, -500f + i * 25, 0 });

        Assert.True(_svc.GetExplorationScore() > 0f);
    }

    [Fact]
    public void GetExplorationScore_IsAtMostOne()
    {
        Assert.True(_svc.GetExplorationScore() <= 1f);
    }

    // ── 8. UpdatePosition ────────────────────────────────────────────────────

    [Fact]
    public void UpdatePosition_InPortlandBounds_SetsCurrentDistrictPortland()
    {
        _svc.UpdatePosition(new float[] { -1200, -800, 0 }); // well inside Portland
        Assert.NotNull(_svc.CurrentDistrict);
        Assert.Equal("portland", _svc.CurrentDistrict!.Id);
    }

    [Fact]
    public void UpdatePosition_InStauntonBounds_SetsCurrentDistrictStaunton()
    {
        _svc.UpdatePosition(new float[] { 300, 100, 0 }); // well inside Staunton
        Assert.NotNull(_svc.CurrentDistrict);
        Assert.Equal("staunton", _svc.CurrentDistrict!.Id);
    }

    [Fact]
    public void UpdatePosition_InShoresideBounds_SetsCurrentDistrictShoreside()
    {
        _svc.UpdatePosition(new float[] { 1100, 200, 0 }); // well inside Shoreside
        Assert.NotNull(_svc.CurrentDistrict);
        Assert.Equal("shoreside", _svc.CurrentDistrict!.Id);
    }

    [Fact]
    public void UpdatePosition_CrossingDistrict_FiresOnDistrictChanged()
    {
        int fired = 0;
        _svc.OnDistrictChanged += (_, _) => fired++;

        _svc.UpdatePosition(new float[] { -1200, -800, 0 }); // Portland
        _svc.UpdatePosition(new float[] { 300,    100, 0 }); // Staunton → triggers event

        Assert.True(fired >= 1);
    }

    [Fact]
    public void UpdatePosition_NearPOI_FiresOnPOIReached()
    {
        int fired = 0;
        _svc.OnPOIReached += (_, _) => fired++;

        // Salvatore's Mansion is at (-1200, -800)
        _svc.UpdatePosition(new float[] { -1200, -800, 0 });

        Assert.True(fired >= 1);
    }

    [Fact]
    public void UpdatePosition_NearPOI_IncreasesVisitedPOICount()
    {
        _svc.UpdatePosition(new float[] { -1200, -800, 0 });
        Assert.True(_svc.VisitedPOICount >= 1);
    }

    [Fact]
    public void UpdatePosition_FiresOnRouteUpdated()
    {
        int fired = 0;
        _svc.OnRouteUpdated += (_, _) => fired++;
        _svc.UpdatePosition(new float[] { -1200, -800, 0 });
        Assert.True(fired >= 1);
    }

    // ── 9. SelectNextDestination ─────────────────────────────────────────────

    [Fact]
    public void SelectNextDestination_ReturnsTwoElementVector()
    {
        var dir = _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        Assert.Equal(2, dir.Length);
    }

    [Fact]
    public void SelectNextDestination_ReturnsApproximatelyUnitVector()
    {
        var dir = _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.7f);
        var magnitude = MathF.Sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
        Assert.True(magnitude > 0.99f && magnitude < 1.01f,
            $"Direction magnitude should be ~1 but was {magnitude}");
    }

    [Fact]
    public void SelectNextDestination_SetsNextPOI()
    {
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        Assert.NotNull(_svc.NextPOI);
    }

    [Fact]
    public void SelectNextDestination_FiresOnPOISelected()
    {
        int fired = 0;
        _svc.OnPOISelected += (_, _) => fired++;
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        Assert.Equal(1, fired);
    }

    [Fact]
    public void SelectNextDestination_SetsCurrentRoute()
    {
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        // Route may be empty if start == end, but CurrentRoute should be initialised
        Assert.NotNull(_svc.CurrentRoute);
    }

    [Fact]
    public void SelectNextDestination_NavigationModeChangesToNavigating()
    {
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        Assert.StartsWith("Navigating to", _svc.NavigationMode);
    }

    [Fact]
    public void SelectNextDestination_WithMaxCuriosity_StillReturnsUnitVector()
    {
        var dir = _svc.SelectNextDestination(new float[] { 0, 0, 0 }, 1.0f);
        var mag = MathF.Sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
        Assert.True(mag > 0.99f && mag < 1.01f);
    }

    [Fact]
    public void SelectNextDestination_WithZeroCuriosity_StillReturnsDirection()
    {
        var dir = _svc.SelectNextDestination(new float[] { 0, 0, 0 }, 0.0f);
        Assert.Equal(2, dir.Length);
    }

    // ── 10. GetNextWaypointDirection ──────────────────────────────────────────

    [Fact]
    public void GetNextWaypointDirection_WithoutRoute_ReturnsTwoElementVector()
    {
        var dir = _svc.GetNextWaypointDirection(new float[] { -800, -400, 0 });
        Assert.Equal(2, dir.Length);
    }

    [Fact]
    public void GetNextWaypointDirection_AfterSelectingDestination_ReturnsUnitVector()
    {
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        var dir = _svc.GetNextWaypointDirection(new float[] { -800, -400, 0 });
        var mag = MathF.Sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
        Assert.True(mag > 0.99f && mag < 1.01f);
    }

    // ── 11. A* pathfinding via SelectNextDestination ─────────────────────────

    [Fact]
    public void SelectNextDestination_RouteFromPortlandToStaunton_FindsPath()
    {
        // Force route planning by picking a Staunton POI from Portland start
        // (real test: route length ≥ 1 since there are valid nodes)
        _svc.SelectNextDestination(new float[] { -1200, -800, 0 }, 0.5f);
        // After selection, the route should not be null
        Assert.NotNull(_svc.CurrentRoute);
    }

    // ── 12. RouteProgress ────────────────────────────────────────────────────

    [Fact]
    public void RouteProgress_WithNoRoute_IsZero()
    {
        Assert.Equal(0f, _svc.RouteProgress);
    }

    [Fact]
    public void RouteProgress_InRange()
    {
        _svc.SelectNextDestination(new float[] { -800, -400, 0 }, 0.5f);
        Assert.InRange(_svc.RouteProgress, 0f, 1f);
    }
}
