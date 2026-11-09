# GTAngelEcho Extension Architecture

**KSM Cycle 2 — Strengthening The Void (Property 13)**

## Overview

The Extension Architecture provides intentional space for growth within the GTAngelEcho cognitive architecture. It consists of four interconnected systems that allow external code to observe, modify, and extend the core pipeline without modifying it.

## Components

### 1. Event Bus (`extensions/event_bus.py`)

A decoupled inter-module communication system using typed events with dot-notation namespacing.

**Features:**
- Priority-based handler ordering (SYSTEM → HIGH → NORMAL → LOW → MONITOR)
- Wildcard subscriptions (`"tick.*"` matches `"tick.pre"` and `"tick.post"`)
- One-shot subscriptions (auto-unsubscribe after first fire)
- Event cancellation and propagation control
- Event history for debugging (last 200 events)
- Source filtering

**Built-in Events:**
- `tick.pre` / `tick.post` — Emitted at the start and end of each game tick
- `world.*` — World events (e.g., `world.threat_detected`)
- `endocrine.mode_change` — Cognitive mode transitions
- `mission.complete` — Mission completion

### 2. Plugin Registry (`extensions/plugin_registry.py`)

Discovery, loading, and lifecycle management for plugins.

**Features:**
- File-based plugin discovery from a directory
- Class-based plugin registration (for built-in plugins)
- Dependency resolution (topological sort)
- Lifecycle management (enable/disable)
- Per-tick dispatch to enabled plugins
- Plugin configuration

**Plugin Lifecycle:**
```
DISCOVERED → LOADED → ENABLED ↔ DISABLED
                         ↓
                       ERROR
```

**Creating a Plugin:**
```python
from extensions.plugin_registry import PluginBase, PluginMeta

class MyPlugin(PluginBase):
    def meta(self):
        return PluginMeta(
            name="my_plugin",
            version="1.0.0",
            description="My custom plugin",
            events=["tick.*"],
            hooks=["post_tick"],
        )

    def on_enable(self, context):
        self.event_bus = context["event_bus"]
        self.hooks = context["hooks"]

    def on_disable(self):
        pass

    def on_tick(self, tick_result):
        # React to each game tick
        pass

def create_plugin():
    return MyPlugin()
```

### 3. Hook System (`extensions/hooks.py`)

Named interception points in the GTAngelEcho pipeline that allow plugins to modify data flowing between centers.

**20 Hook Points:**

| Hook Point | Location | Purpose |
|---|---|---|
| `PRE_TICK` | Before tick processing | Modify tick context |
| `POST_TICK` | After tick result compiled | Modify final result |
| `PRE_ENDOCRINE_TICK` | Before hormone decay | Inject hormones |
| `POST_ENDOCRINE_TICK` | After mode detection | React to mode |
| `PRE_PERCEIVE` | Before perception | Filter sensory input |
| `POST_PERCEIVE` | After perception | Modify percepts |
| `PRE_SYNTHESIZE` | Before action generation | Bias action space |
| `POST_SYNTHESIZE` | After synthesis | Modify potentials |
| `POST_ENACT` | After action selection | Override action |
| `PRE_EXPRESSION` | Before FACS update | Inject expressions |
| `POST_EXPRESSION` | After expression state | Modify expression |
| `PRE_EMBODIMENT` | Before embodiment | Modify body state |
| `POST_EMBODIMENT` | After embodiment | React to posture |
| `PRE_NAVIGATION` | Before nav tick | Modify waypoints |
| `POST_NAVIGATION` | After nav tick | React to position |
| `MISSION_ASSIGNED` | New mission starts | Modify mission |
| `MISSION_COMPLETE` | Mission completes | Reward/log |
| `PRE_MEMORY_STORE` | Before storing memory | Filter memories |
| `POST_MEMORY_RECALL` | After memory retrieval | Modify recall |
| `ENDOCRINE_EVENT` | When event signaled | Intercept events |

### 4. API Adapter (`extensions/api_adapter.py`)

Exposes GTAngelEcho internals via REST endpoints for external tools, dashboards, and AI agents.

**14 Built-in Routes:**

| Method | Path | Description |
|---|---|---|
| GET | `/api/state` | Full system state |
| GET | `/api/metrics` | Performance metrics |
| GET | `/api/hormones` | Current hormone levels |
| GET | `/api/valence-arousal` | Valence-arousal coordinates |
| POST | `/api/command` | Inject a command |
| POST | `/api/event` | Signal a world event |
| GET | `/api/plugins` | List all plugins |
| POST | `/api/plugins/enable` | Enable a plugin |
| POST | `/api/plugins/disable` | Disable a plugin |
| GET | `/api/events` | Recent event stream |
| GET | `/api/events/stats` | Event statistics |
| GET | `/api/hooks` | Hook point handler counts |
| GET | `/api/routes` | List all API routes |
| GET | `/api/memory/recent` | Recent episodic memories |

## Example Plugins

Three example plugins are provided in `plugins/examples/`:

1. **Telemetry Logger** — Logs tick data and mode transitions to a JSON-lines file
2. **Aggression Modulator** — Modifies action potentials with configurable aggression/pacifism bias
3. **Expression Snapshot** — Captures periodic snapshots of avatar expression state

## Directory Structure

```
plugins/
├── examples/           # Example plugins (shipped with repo)
│   ├── telemetry_logger.py
│   ├── aggression_modulator.py
│   └── expression_snapshot.py
└── contrib/            # Community-contributed plugins (The Void)
```
