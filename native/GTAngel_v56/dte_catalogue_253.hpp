// Generated from the reviewed DTE catalogue. Advisory ONLY; no commit capability.
// APL190..196 have extra typed, fail-closed checks; all other English constraints
// remain unimplemented except the separately qualified APL160 beacon proposal.
namespace v25rules {
struct Rule { unsigned short id; const char* signal; const char* effect; float threshold; float intensity; unsigned short ttl; bool lessEqual; };
static constexpr Rule Catalogue[] = {
 {1, "memory_relevance", "allocate_cognition", 0.120000f, 0.850000f, 144, true}, // INDEPENDENT REGIONS
 {2, "novelty", "propose_niche", 0.600000f, 0.600000f, 120, false}, // DISTRIBUTION OF TOWNS
 {3, "route_affordance", "propose_route", 0.300000f, 0.700000f, 72, false}, // CITY COUNTRY FINGERS
 {4, "telemetry_freshness", "preserve_boundary", 0.020000f, 0.900000f, 168, true}, // AGRICULTURAL VALLEYS
 {5, "route_affordance", "propose_route", 0.050000f, 0.500000f, 48, true}, // LACE OF COUNTRY STREETS
 {6, "world_budget", "propose_niche", 0.700000f, 0.750000f, 240, false}, // COUNTRY TOWNS
 {7, "collision_risk", "stage_learning", 0.200000f, 0.600000f, 96, false}, // THE COUNTRYSIDE
 {8, "identity_coherence", "emit_reflection", 0.400000f, 0.650000f, 168, false}, // MOSAIC OF SUBCULTURES
 {9, "world_budget", "propose_motor", 0.100000f, 0.700000f, 36, true}, // SCATTERED WORK
 {10, "arousal", "allocate_cognition", 0.750000f, 0.900000f, 72, false}, // MAGIC OF THE CITY
 {11, "route_affordance", "propose_route", 0.700000f, 0.750000f, 180, false}, // LOCAL TRANSPORT AREAS
 {12, "memory_relevance", "allocate_cognition", 0.030000f, 0.800000f, 200, true}, // COMMUNITY OF 7000
 {13, "social_proximity", "preserve_boundary", 0.200000f, 0.650000f, 150, true}, // SUBCULTURE BOUNDARY
 {14, "identity_coherence", "emit_reflection", 0.750000f, 0.850000f, 120, false}, // IDENTIFIABLE NEIGHBORHOOD
 {15, "consent", "preserve_boundary", 0.600000f, 0.700000f, 160, false}, // NEIGHBORHOOD BOUNDARY
 {16, "route_affordance", "propose_route", 0.800000f, 0.780000f, 200, false}, // WEB OF PUBLIC TRANSPORTATION
 {17, "collision_risk", "propose_route", 0.700000f, 0.700000f, 140, false}, // RING ROADS
 {18, "novelty", "stage_learning", 0.600000f, 0.820000f, 180, false}, // NETWORK OF LEARNING
 {19, "world_budget", "propose_niche", 0.750000f, 0.680000f, 150, false}, // WEB OF SHOPPING
 {20, "fatigue", "propose_motor", 0.200000f, 0.800000f, 160, true}, // MINI-BUSES
 {21, "attention_load", "preserve_boundary", 0.800000f, 0.800000f, 180, true}, // FOUR-STORY LIMIT
 {22, "visual_salience", "allocate_cognition", 0.090000f, 0.600000f, 120, true}, // NINE PERCENT PARKING
 {23, "route_affordance", "propose_route", 0.200000f, 0.900000f, 60, true}, // PARALLEL ROADS
 {24, "memory_relevance", "preserve_boundary", 0.600000f, 0.950000f, 300, false}, // SACRED SITES
 {25, "world_version_match", "preserve_boundary", 0.300000f, 0.850000f, 240, false}, // ACCESS TO WATER
 {26, "ritual_readiness", "stage_learning", 0.750000f, 0.700000f, 200, false}, // LIFE CYCLE
 {27, "social_proximity", "preserve_boundary", 0.100000f, 0.900000f, 120, true}, // MEN AND WOMEN
 {28, "novelty", "propose_niche", 0.450000f, 0.650000f, 150, true}, // ECCENTRIC NUCLEUS
 {29, "world_budget", "allocate_cognition", 0.700000f, 0.800000f, 180, true}, // DENSITY RINGS
 {30, "collision_risk", "propose_motor", 0.400000f, 0.850000f, 90, false}, // ACTIVITY NODES
 {31, "social_proximity", "focus_attention", 0.120000f, 0.650000f, 180, false}, // PROMENADE
 {32, "memory_relevance", "propose_niche", 0.300000f, 0.550000f, 120, false}, // SHOPPING STREET
 {33, "arousal", "tune_ambient", 0.600000f, 0.600000f, 90, true}, // NIGHT LIFE
 {34, "route_affordance", "propose_route", 0.250000f, 0.800000f, 30, true}, // INTERCHANGE
 {35, "identity_coherence", "propose_niche", 0.600000f, 0.700000f, 240, false}, // HOUSEHOLD MIX
 {36, "perceptual_confidence", "allocate_cognition", 0.150000f, 0.500000f, 100, false}, // DEGREES OF PUBLICNESS
 {37, "trust", "link_memory", 0.300000f, 0.750000f, 300, false}, // HOUSE CLUSTER
 {38, "consent", "propose_niche", 0.250000f, 0.500000f, 160, false}, // ROW HOUSES
 {39, "route_affordance", "preserve_boundary", 0.200000f, 0.600000f, 200, false}, // HOUSING HILL
 {40, "memory_relevance", "stage_learning", 0.120000f, 0.850000f, 220, false}, // OLD PEOPLE EVERYWHERE
 {41, "social_proximity", "propose_niche", 0.500000f, 0.750000f, 180, false}, // WORK COMMUNITY
 {42, "route_affordance", "allocate_cognition", 0.600000f, 0.680000f, 120, true}, // INDUSTRIAL RIBBON
 {43, "memory_relevance", "stage_learning", 0.400000f, 0.800000f, 200, false}, // UNIVERSITY AS MARKETPLACE
 {44, "perceptual_confidence", "issue_dialogue_cue", 0.200000f, 0.700000f, 150, true}, // LOCAL TOWN HALL
 {45, "attention_load", "propose_niche", 0.350000f, 0.600000f, 90, true}, // NECKLACE OF COMMUNITY PROJECTS
 {46, "world_budget", "propose_niche", 0.100000f, 0.650000f, 120, true}, // MARKET OF MANY SHOPS
 {47, "fatigue", "schedule_recovery", 0.300000f, 0.900000f, 30, true}, // HEALTH CENTER
 {48, "attention_load", "allocate_cognition", 0.800000f, 0.600000f, 60, true}, // HOUSING IN BETWEEN
 {49, "collision_risk", "suggest_expression", 0.250000f, 0.550000f, 100, true}, // LOOPED LOCAL ROADS
 {50, "prediction_error", "issue_dialogue_cue", 0.150000f, 0.700000f, 40, true}, // T JUNCTIONS
 {51, "memory_relevance", "propose_route", 0.600000f, 0.450000f, 180, false}, // GREEN STREETS
 {52, "collision_risk", "propose_niche", 0.250000f, 0.700000f, 200, false}, // NETWORK OF PATHS AND CARS
 {53, "social_proximity", "suggest_expression", 0.300000f, 0.350000f, 120, false}, // MAIN GATEWAYS
 {54, "attention_load", "allocate_cognition", 0.200000f, 0.600000f, 40, false}, // ROAD CROSSING
 {55, "collision_risk", "propose_route", 0.400000f, 0.600000f, 220, false}, // RAISED WALK
 {56, "route_affordance", "propose_niche", 0.150000f, 0.500000f, 160, false}, // BIKE PATHS AND RACKS
 {57, "social_proximity", "tune_ambient", 0.100000f, 0.850000f, 240, false}, // CHILDREN IN THE CITY
 {58, "ritual_readiness", "stage_learning", 0.400000f, 0.400000f, 30, false}, // CARNIVAL
 {59, "arousal", "preserve_boundary", 0.500000f, 0.700000f, 200, false}, // QUIET BACKS
 {60, "route_affordance", "propose_niche", 0.150000f, 0.900000f, 300, false}, // ACCESSIBLE GREEN
 {61, "social_proximity", "focus_attention", 0.020000f, 0.600000f, 120, false}, // SMALL PUBLIC SQUARES
 {62, "perceptual_confidence", "allocate_cognition", 0.800000f, 0.700000f, 150, false}, // HIGH PLACES
 {63, "arousal", "issue_dialogue_cue", 0.600000f, 0.650000f, 48, true}, // DANCING IN THE STREET
 {64, "memory_relevance", "link_memory", 0.300000f, 0.500000f, 240, false}, // POOLS AND STREAMS
 {65, "consent", "request_reobservation", 0.900000f, 0.850000f, 300, false}, // BIRTH PLACES
 {66, "identity_coherence", "emit_reflection", 0.950000f, 0.400000f, 200, false}, // HOLY GROUND
 {67, "trust", "propose_niche", 0.600000f, 0.600000f, 180, false}, // COMMON LAND
 {68, "collision_risk", "propose_route", 0.050000f, 0.700000f, 120, true}, // CONNECTED PLAY
 {69, "attention_load", "stage_learning", 0.400000f, 0.600000f, 90, true}, // PUBLIC OUTDOOR ROOM
 {70, "telemetry_freshness", "register_witness", 0.000000f, 0.500000f, 300, false}, // GRAVE SITES
 {71, "visual_salience", "focus_attention", 0.920000f, 0.450000f, 200, false}, // STILL WATER
 {72, "route_affordance", "propose_route", 0.300000f, 0.600000f, 160, true}, // LOCAL SPORTS
 {73, "novelty", "propose_motor", 0.550000f, 0.700000f, 240, false}, // ADVENTURE PLAYGROUND
 {74, "social_proximity", "link_memory", 0.700000f, 0.350000f, 180, true}, // ANIMALS
 {75, "trust", "allocate_cognition", 0.600000f, 0.850000f, 240, false}, // THE FAMILY
 {76, "world_budget", "propose_niche", 0.250000f, 0.700000f, 200, false}, // HOUSE FOR A SMALL FAMILY
 {77, "social_proximity", "stage_learning", 0.500000f, 0.600000f, 180, false}, // HOUSE FOR A COUPLE
 {78, "agency_confidence", "preserve_boundary", 0.600000f, 0.500000f, 160, false}, // HOUSE FOR ONE PERSON
 {79, "identity_coherence", "link_memory", 0.100000f, 0.900000f, 300, false}, // YOUR OWN HOME
 {80, "consent", "request_reobservation", 0.700000f, 0.800000f, 220, false}, // SELF-GOVERNING WORKSHOPS AND OFFICES
 {81, "world_budget", "propose_niche", 0.050000f, 0.450000f, 120, true}, // SMALL SERVICES WITHOUT RED TAPE
 {82, "route_affordance", "propose_route", 0.900000f, 0.600000f, 180, false}, // OFFICE CONNECTIONS
 {83, "memory_relevance", "stage_learning", 0.700000f, 0.720000f, 200, false}, // MASTER AND APPRENTICES
 {84, "social_proximity", "register_witness", 0.600000f, 0.550000f, 160, false}, // TEENAGE SOCIETY
 {85, "visual_salience", "propose_niche", 0.250000f, 0.500000f, 140, false}, // SHOPFRONT SCHOOLS
 {86, "somatic_safety", "preserve_boundary", 0.950000f, 0.850000f, 300, false}, // CHILDREN'S HOME
 {87, "identity_coherence", "link_memory", 0.600000f, 0.600000f, 180, false}, // INDIVIDUALLY OWNED SHOPS
 {88, "visual_salience", "focus_attention", 0.900000f, 0.400000f, 120, false}, // STREET CAFE
 {89, "route_affordance", "propose_niche", 0.200000f, 0.780000f, 150, true}, // CORNER GROCERY
 {90, "social_proximity", "emit_reflection", 0.450000f, 0.650000f, 220, false}, // BEER HALL
 {91, "memory_relevance", "register_witness", 0.700000f, 0.450000f, 120, false}, // TRAVELER'S INN
 {92, "attention_load", "focus_attention", 0.650000f, 0.500000f, 60, false}, // BUS STOP
 {93, "novelty", "propose_niche", 0.500000f, 0.550000f, 90, false}, // FOOD STANDS
 {94, "somatic_safety", "stage_learning", 0.800000f, 0.600000f, 180, false}, // SLEEPING IN PUBLIC
 {95, "world_version_match", "allocate_cognition", 0.750000f, 0.700000f, 200, false}, // BUILDING COMPLEX
 {96, "attention_load", "allocate_cognition", 0.850000f, 0.500000f, 150, true}, // NUMBER OF STORIES
 {97, "visual_salience", "preserve_boundary", 0.250000f, 0.400000f, 100, true}, // SHIELDED PARKING
 {98, "route_affordance", "propose_route", 0.350000f, 0.650000f, 140, false}, // CIRCULATION REALMS
 {99, "agency_confidence", "emit_reflection", 0.600000f, 0.800000f, 220, false}, // MAIN BUILDING
 {100, "collision_risk", "tune_ambient", 0.150000f, 0.750000f, 160, true}, // PEDESTRIAN STREET
 {101, "route_affordance", "propose_route", 0.950000f, 0.700000f, 120, false}, // BUILDING THOROUGHFARE
 {102, "agency_confidence", "ask_consent", 0.700000f, 0.600000f, 200, false}, // FAMILY OF ENTRANCES
 {103, "world_budget", "allocate_cognition", 0.200000f, 0.500000f, 90, true}, // SMALL PARKING LOTS
 {104, "memory_relevance", "register_witness", 0.150000f, 0.800000f, 150, false}, // SITE REPAIR
 {105, "perceptual_confidence", "tune_ambient", 0.600000f, 0.650000f, 100, false}, // SOUTH FACING OUTDOORS
 {106, "social_proximity", "issue_dialogue_cue", 0.300000f, 0.700000f, 180, false}, // POSITIVE OUTDOOR SPACE
 {107, "telemetry_freshness", "allocate_cognition", 0.200000f, 0.750000f, 80, true}, // WINGS OF LIGHT
 {108, "perceptual_confidence", "propose_route", 0.900000f, 0.850000f, 140, false}, // CONNECTED BUILDINGS
 {109, "attention_load", "propose_niche", 0.150000f, 0.600000f, 110, true}, // LONG THIN HOUSE
 {110, "trust", "stage_learning", 0.400000f, 0.900000f, 220, true}, // MAIN ENTRANCE
 {111, "visual_salience", "focus_attention", 0.200000f, 0.550000f, 120, false}, // HALF-HIDDEN GARDEN
 {112, "social_proximity", "request_reobservation", 0.600000f, 0.680000f, 80, false}, // ENTRANCE TRANSITION
 {113, "route_affordance", "propose_route", 0.500000f, 0.450000f, 100, false}, // CAR CONNECTION
 {114, "trust", "preserve_boundary", 0.400000f, 0.720000f, 200, true}, // HIERARCHY OF OPEN SPACE
 {115, "social_proximity", "stage_learning", 0.200000f, 0.780000f, 150, false}, // COURTYARDS WHICH LIVE
 {116, "arousal", "tune_ambient", 0.100000f, 0.400000f, 90, false}, // CASCADE OF ROOFS
 {117, "somatic_safety", "preserve_boundary", 0.250000f, 0.620000f, 140, false}, // SHELTERING ROOF
 {118, "novelty", "allocate_cognition", 0.300000f, 0.660000f, 180, false}, // ROOF GARDEN
 {119, "perceptual_confidence", "stage_learning", 0.800000f, 0.500000f, 120, false}, // ARCADES
 {120, "route_affordance", "propose_route", 0.650000f, 0.850000f, 160, false}, // PATHS AND GOALS
 {121, "route_affordance", "propose_route", 0.080000f, 0.450000f, 180, true}, // PATH SHAPE
 {122, "social_proximity", "suggest_expression", 0.600000f, 0.700000f, 120, false}, // BUILDING FRONTS
 {123, "novelty", "allocate_cognition", 0.150000f, 0.600000f, 200, false}, // PEDESTRIAN DENSITY
 {124, "memory_relevance", "propose_niche", 0.050000f, 0.500000f, 150, true}, // ACTIVITY POCKETS
 {125, "fatigue", "stage_learning", 0.200000f, 0.400000f, 90, false}, // STAIR SEATS
 {126, "visual_salience", "focus_attention", 0.400000f, 0.800000f, 240, true}, // SOMETHING ROUGHLY IN THE MIDDLE
 {127, "consent", "preserve_boundary", 0.600000f, 0.750000f, 300, false}, // INTIMACY GRADIENT
 {128, "perceptual_confidence", "emit_reflection", 0.700000f, 0.650000f, 220, false}, // INDOOR SUNLIGHT
 {129, "telemetry_freshness", "link_memory", 0.080000f, 0.850000f, 260, true}, // COMMON AREAS AT THE HEART
 {130, "perceptual_confidence", "issue_dialogue_cue", 0.600000f, 0.600000f, 140, true}, // ENTRANCE ROOM
 {131, "route_affordance", "propose_route", 0.600000f, 0.700000f, 180, false}, // THE FLOW THROUGH ROOMS
 {132, "visual_salience", "tune_ambient", 0.500000f, 0.500000f, 120, true}, // SHORT PASSAGES
 {133, "social_proximity", "stage_learning", 0.400000f, 0.800000f, 200, false}, // STAIRCASE AS A STAGE
 {134, "visual_salience", "emit_reflection", 0.700000f, 0.600000f, 150, false}, // ZEN VIEW
 {135, "attention_load", "tune_ambient", 0.450000f, 0.400000f, 100, false}, // TAPESTRY OF LIGHT AND DARK
 {136, "social_proximity", "preserve_boundary", 0.600000f, 0.900000f, 300, false}, // COUPLE'S REALM
 {137, "novelty", "propose_niche", 0.350000f, 0.850000f, 240, true}, // CHILDREN'S REALM
 {138, "fatigue", "schedule_recovery", 0.500000f, 0.700000f, 160, false}, // SLEEPING TO THE EAST
 {139, "memory_relevance", "allocate_cognition", 0.500000f, 0.900000f, 220, false}, // FARMHOUSE KITCHEN
 {140, "consent", "preserve_boundary", 0.600000f, 0.600000f, 140, false}, // PRIVATE TERRACE ON THE STREET
 {141, "identity_coherence", "preserve_boundary", 0.900000f, 0.850000f, 180, false}, // A ROOM OF ONE'S OWN
 {142, "attention_load", "allocate_cognition", 0.600000f, 0.700000f, 120, false}, // SEQUENCE OF SITTING SPACES
 {143, "fatigue", "preserve_boundary", 0.300000f, 0.800000f, 200, true}, // BED CLUSTER
 {144, "perceptual_confidence", "stage_learning", 0.800000f, 0.600000f, 60, false}, // BATHING ROOM
 {145, "memory_relevance", "link_memory", 0.200000f, 0.750000f, 240, true}, // BULK STORAGE
 {146, "novelty", "propose_niche", 0.500000f, 0.800000f, 150, true}, // FLEXIBLE OFFICE SPACE
 {147, "social_proximity", "stage_learning", 0.700000f, 0.650000f, 100, false}, // COMMUNAL EATING
 {148, "trust", "propose_niche", 0.600000f, 0.900000f, 140, false}, // SMALL WORK GROUPS
 {149, "perceptual_confidence", "stage_learning", 0.700000f, 0.700000f, 80, true}, // RECEPTION WELCOMES YOU
 {150, "attention_load", "slow_down", 0.400000f, 0.550000f, 90, true}, // A PLACE TO WAIT
 {151, "social_proximity", "propose_niche", 0.250000f, 0.700000f, 120, true}, // SMALL MEETING ROOMS
 {152, "identity_coherence", "allocate_cognition", 0.600000f, 0.650000f, 200, false}, // HALF-PRIVATE OFFICE
 {153, "route_affordance", "propose_route", 0.200000f, 0.550000f, 80, true}, // ROOMS TO RENT
 {154, "agency_confidence", "stage_learning", 0.400000f, 0.450000f, 160, false}, // TEENAGER'S COTTAGE
 {155, "fatigue", "preserve_boundary", 0.300000f, 0.600000f, 240, true}, // OLD AGE COTTAGE
 {156, "memory_relevance", "stage_learning", 0.700000f, 0.800000f, 300, false}, // SETTLED WORK
 {157, "collision_risk", "propose_motor", 0.250000f, 0.600000f, 100, true}, // HOME WORKSHOP
 {158, "perceptual_confidence", "request_reobservation", 0.500000f, 0.500000f, 60, false}, // OPEN STAIRS
 {159, "perceptual_confidence", "tune_ambient", 0.660000f, 0.850000f, 180, false}, // LIGHT ON TWO SIDES OF EVERY ROOM
 {160, "niche_need", "propose_niche", 0.400000f, 0.750000f, 140, false}, // BUILDING EDGE
 {161, "visual_salience", "propose_niche", 0.600000f, 0.700000f, 120, false}, // SUNNY PLACE
 {162, "agency_confidence", "preserve_boundary", 0.700000f, 0.850000f, 180, false}, // NORTH FACE
 {163, "route_affordance", "propose_route", 0.650000f, 0.800000f, 150, false}, // OUTDOOR ROOM
 {164, "social_proximity", "register_witness", 0.500000f, 0.600000f, 100, false}, // STREET WINDOWS
 {165, "trust", "link_memory", 0.400000f, 0.750000f, 90, true}, // OPENING TO THE STREET
 {166, "route_affordance", "propose_route", 0.600000f, 0.650000f, 140, false}, // GALLERY SURROUND
 {167, "attention_load", "propose_motor", 0.600000f, 0.700000f, 130, false}, // SIX-FOOT BALCONY
 {168, "niche_need", "propose_niche", 0.700000f, 0.800000f, 200, false}, // CONNECTION TO THE EARTH
 {169, "route_affordance", "propose_route", 0.550000f, 0.600000f, 160, false}, // TERRACED SLOPE
 {170, "ritual_readiness", "propose_niche", 0.600000f, 0.700000f, 220, false}, // FRUIT TREES
 {171, "social_proximity", "propose_niche", 0.800000f, 0.620000f, 120, false}, // TREE PLACES
 {172, "world_budget", "link_memory", 0.750000f, 0.450000f, 200, true}, // GARDEN GROWING WILD
 {173, "perceptual_confidence", "preserve_boundary", 0.400000f, 0.700000f, 180, true}, // GARDEN WALL
 {174, "visual_salience", "focus_attention", 0.150000f, 0.550000f, 90, false}, // TRELLISED WALK
 {175, "novelty", "stage_learning", 0.500000f, 0.680000f, 150, false}, // GREENHOUSE
 {176, "arousal", "propose_niche", 0.400000f, 0.500000f, 72, true}, // GARDEN SEAT
 {177, "quest_priority", "propose_route", 0.600000f, 0.600000f, 240, true}, // VEGETABLE GARDEN
 {178, "memory_relevance", "link_memory", 0.300000f, 0.580000f, 96, false}, // COMPOST
 {179, "consent", "issue_dialogue_cue", 0.500000f, 0.720000f, 60, true}, // ALCOVES
 {180, "visual_salience", "emit_reflection", 0.500000f, 0.470000f, 48, false}, // WINDOW PLACE
 {181, "social_proximity", "focus_attention", 0.600000f, 0.900000f, 120, false}, // THE FIRE
 {182, "attention_load", "tune_ambient", 0.400000f, 0.600000f, 48, false}, // EATING ATMOSPHERE
 {183, "identity_coherence", "allocate_cognition", 0.250000f, 0.750000f, 84, false}, // WORKSPACE ENCLOSURE
 {184, "route_affordance", "propose_route", 0.500000f, 0.700000f, 36, false}, // COOKING LAYOUT
 {185, "trust", "issue_dialogue_cue", 0.600000f, 0.650000f, 72, false}, // SITTING CIRCLE
 {186, "fatigue", "schedule_recovery", 0.600000f, 0.800000f, 240, false}, // COMMUNAL SLEEPING
 {187, "ritual_readiness", "emit_reflection", 0.700000f, 0.900000f, 300, false}, // MARRIAGE BED
 {188, "somatic_safety", "preserve_boundary", 0.250000f, 0.700000f, 96, true}, // BED ALCOVE
 {189, "world_version_match", "stage_learning", 0.800000f, 0.550000f, 12, false}, // DRESSING ROOM
 {190, "attention_load", "tune_ambient", 0.200000f, 0.600000f, 60, false}, // CEILING HEIGHT VARIETY
 {191, "visual_salience", "focus_attention", 1.000000f, 0.750000f, 120, false}, // THE SHAPE OF INDOOR SPACE
 {192, "social_proximity", "register_witness", 0.900000f, 0.650000f, 200, false}, // WINDOWS OVERLOOKING LIFE
 {193, "memory_relevance", "link_memory", 0.200000f, 0.600000f, 150, true}, // HALF-OPEN WALL
 {194, "perceptual_confidence", "propose_route", 0.700000f, 0.550000f, 80, false}, // INTERIOR WINDOWS
 {195, "world_budget", "allocate_cognition", 0.750000f, 0.800000f, 180, false}, // STAIRCASE VOLUME
 {196, "collision_risk", "propose_motor", 0.150000f, 0.600000f, 90, true}, // CORNER DOORS
 {197, "world_budget", "preserve_boundary", 0.500000f, 0.700000f, 140, false}, // THICK WALLS
 {198, "novelty", "slow_down", 0.600000f, 0.500000f, 100, false}, // CLOSETS BETWEEN ROOMS
 {199, "visual_salience", "tune_ambient", 0.800000f, 0.700000f, 110, false}, // SUNNY COUNTER
 {200, "trust", "link_memory", 0.600000f, 0.600000f, 130, false}, // OPEN SHELVES
 {201, "visual_salience", "focus_attention", 0.250000f, 0.650000f, 120, false}, // WAIST-HIGH SHELF
 {202, "route_affordance", "propose_niche", 0.300000f, 0.600000f, 90, false}, // BUILT-IN SEATS
 {203, "social_proximity", "suggest_expression", 0.350000f, 0.500000f, 60, false}, // CHILD CAVES
 {204, "trust", "preserve_boundary", 0.600000f, 0.700000f, 180, false}, // SECRET PLACE
 {205, "social_proximity", "allocate_cognition", 0.550000f, 0.800000f, 200, false}, // STRUCTURE FOLLOWS SOCIAL SPACES
 {206, "world_budget", "preserve_boundary", 0.400000f, 0.750000f, 240, true}, // EFFICIENT STRUCTURE
 {207, "perceptual_confidence", "register_witness", 0.500000f, 0.700000f, 180, false}, // GOOD MATERIALS
 {208, "fatigue", "slow_down", 0.500000f, 0.600000f, 150, false}, // GRADUAL STIFFENING
 {209, "collision_risk", "tune_ambient", 0.300000f, 0.550000f, 140, false}, // ROOF LAYOUT
 {210, "memory_relevance", "checkpoint_state", 0.450000f, 0.650000f, 160, false}, // FLOOR AND CEILING LAYOUT
 {211, "memory_relevance", "preserve_boundary", 0.650000f, 0.720000f, 180, false}, // THICKENING THE OUTER WALLS
 {212, "agency_confidence", "allocate_cognition", 0.700000f, 0.600000f, 160, false}, // COLUMNS AT THE CORNERS
 {213, "world_budget", "allocate_cognition", 0.120000f, 0.680000f, 200, true}, // FINAL COLUMN DISTRIBUTION
 {214, "identity_coherence", "preserve_boundary", 0.800000f, 0.700000f, 140, false}, // ROOT FOUNDATIONS
 {215, "telemetry_freshness", "checkpoint_state", 0.500000f, 0.620000f, 120, true}, // GROUND FLOOR SLAB
 {216, "telemetry_freshness", "register_witness", 0.800000f, 0.550000f, 150, false}, // BOX COLUMNS
 {217, "perceptual_confidence", "link_memory", 0.800000f, 0.660000f, 170, true}, // PERIMETER BEAMS
 {218, "prediction_error", "tune_ambient", 0.150000f, 0.580000f, 130, true}, // WALL MEMBRANES
 {219, "perceptual_confidence", "link_memory", 0.050000f, 0.640000f, 190, true}, // FLOOR-CEILING VAULTS
 {220, "trust", "emit_reflection", 0.700000f, 0.700000f, 180, false}, // ROOF VAULTS
 {221, "attention_load", "focus_attention", 0.350000f, 0.750000f, 120, false}, // NATURAL DOORS AND WINDOWS
 {222, "perceptual_confidence", "recall_episode", 0.700000f, 0.600000f, 90, false}, // LOW SILL
 {223, "visual_salience", "focus_attention", 0.450000f, 0.500000f, 100, true}, // DEEP REVEALS
 {224, "collision_risk", "propose_route", 0.250000f, 0.400000f, 60, true}, // LOW DOORWAY
 {225, "identity_coherence", "allocate_cognition", 0.600000f, 0.800000f, 180, true}, // FRAMES AS THICKENED EDGES
 {226, "memory_relevance", "link_memory", 0.500000f, 0.700000f, 200, false}, // COLUMN PLACE
 {227, "ritual_readiness", "suggest_expression", 0.400000f, 0.450000f, 120, false}, // COLUMN CONNECTION
 {228, "route_affordance", "propose_niche", 0.550000f, 0.600000f, 150, false}, // STAIR VAULT
 {229, "telemetry_freshness", "allocate_cognition", 0.400000f, 0.850000f, 240, false}, // DUCT SPACE
 {230, "somatic_safety", "tune_ambient", 0.600000f, 0.700000f, 180, false}, // RADIANT HEAT
 {231, "visual_salience", "focus_attention", 0.450000f, 0.600000f, 120, true}, // DORMER WINDOWS
 {232, "arousal", "allocate_cognition", 0.550000f, 0.450000f, 80, true}, // ROOF CAPS
 {233, "memory_relevance", "preserve_boundary", 0.120000f, 0.700000f, 200, true}, // FLOOR SURFACE
 {234, "world_version_match", "stage_learning", 0.100000f, 0.500000f, 150, false}, // LAPPED OUTSIDE WALLS
 {235, "social_proximity", "tune_ambient", 0.400000f, 0.650000f, 160, false}, // SOFT INSIDE WALLS
 {236, "agency_confidence", "propose_route", 0.600000f, 0.800000f, 40, false}, // WINDOWS WHICH OPEN WIDE
 {237, "trust", "preserve_boundary", 0.700000f, 0.600000f, 100, false}, // SOLID DOORS WITH GLASS
 {238, "prediction_error", "tune_ambient", 0.350000f, 0.550000f, 90, false}, // FILTERED LIGHT
 {239, "attention_load", "focus_attention", 0.600000f, 0.700000f, 70, false}, // SMALL PANES
 {240, "perceptual_confidence", "preserve_boundary", 0.030000f, 0.400000f, 140, false}, // HALF-INCH TRIM
 {241, "route_affordance", "focus_attention", 0.050000f, 0.450000f, 120, false}, // SEAT SPOTS
 {242, "social_proximity", "issue_dialogue_cue", 0.300000f, 0.500000f, 200, true}, // FRONT DOOR BENCH
 {243, "attention_load", "allocate_cognition", 0.600000f, 0.600000f, 150, false}, // SITTING WALL
 {244, "novelty", "tune_ambient", 0.700000f, 0.550000f, 60, false}, // CANVAS ROOFS
 {245, "perceptual_confidence", "focus_attention", 0.500000f, 0.500000f, 80, true}, // RAISED FLOWERS
 {246, "memory_relevance", "link_memory", 0.100000f, 0.650000f, 300, false}, // CLIMBING PLANTS
 {247, "world_budget", "propose_niche", 0.050000f, 0.350000f, 200, false}, // PAVING WITH CRACKS BETWEEN
 {248, "perceptual_confidence", "suggest_expression", 0.700000f, 0.600000f, 180, false}, // SOFT TILE AND BRICK
 {249, "visual_salience", "emit_reflection", 0.150000f, 0.400000f, 140, true}, // ORNAMENT
 {250, "trust", "tune_ambient", 0.500000f, 0.500000f, 100, false}, // WARM COLORS
 {251, "novelty", "propose_niche", 0.600000f, 0.550000f, 120, false}, // DIFFERENT CHAIRS
 {252, "visual_salience", "focus_attention", 0.500000f, 0.720000f, 100, false}, // POOLS OF LIGHT
 {253, "identity_coherence", "recall_episode", 0.700000f, 0.850000f, 200, false}, // THINGS FROM YOUR LIFE
};
static_assert(sizeof(Catalogue)/sizeof(Rule)==253,"DTE catalogue incomplete");
static constexpr const char* CatalogueSHA256="0bc516efaa58eabfd56d82d5c932d86243f644bc3822cb14b3b1e98fa0c03c7d";
} // namespace v25rules
