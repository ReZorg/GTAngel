# GTAngelEcho Architecture

## Composition Algebra

```
GTAngelEcho = GTAngel ⊗ DeepTreeEcho(SuperHotGirl ⊕ MLGamer)

Expanded:
  = (CognitiveCore ⊗ ReservoirComputing ⊗ MemorySystems)    [Deep Integration Pipeline]
  ⊕ (AvatarExpression ⊗ AvatarEmbodiment)                   [Visual Channel]
  ⊕ (GameplayIntelligence ⊗ WorldNavigation)                 [Action Channel]
  ⊕ EmotionalDynamics                                        [Affective Bus]
```

## Data Flow

```
                    ┌─────────────────────────────────────────────┐
                    │           GTAngelEcho Integration            │
                    └─────────────────────────────────────────────┘
                                        │
                    ┌───────────────────┼───────────────────┐
                    │                   │                   │
              ┌─────▼─────┐     ┌──────▼──────┐    ┌──────▼──────┐
              │  Cognitive │     │  Emotional  │    │   Action    │
              │   Core     │     │  Dynamics   │    │   Channel   │
              │ (Echobeats)│     │ (Endocrine) │    │ (MLGamer +  │
              └─────┬─────┘     └──────┬──────┘    │  GTAngel)   │
                    │                   │           └──────┬──────┘
              ┌─────▼─────┐            │                   │
              │ Reservoir  │◄───────────┘                   │
              │ Computing  │                                │
              │   (ESN)    │────────────────────────────────┘
              └─────┬─────┘
                    │
              ┌─────▼─────┐     ┌─────────────┐
              │  Memory    │     │   Avatar    │
              │  Systems   │     │  Channel    │
              │ (Episodic/ │     │(SuperHotGirl│
              │  Semantic/ │     │+ Embodiment)│
              │ Procedural)│     └─────────────┘
              └───────────┘
```

## Per-Tick Pipeline

1. **World Events** → Endocrine System signals events
2. **Endocrine Tick** → Hormone decay + mode detection
3. **Reservoir Step** → ESN processes temporal patterns
4. **Cognitive Cycle** → Echobeats 4-phase (Perceive → Resonate → Synthesize → Enact)
5. **Memory Update** → Store/retrieve episodic + semantic + procedural
6. **MLGamer Decision** → Q-learning action selection
7. **GTAngel Navigation** → Pathfinding + mission progress
8. **Avatar Expression** → FACS mapping + chaos + aesthetics
9. **Avatar Embodiment** → 4E cognition body state update

## Module Interfaces

### Endocrine → All Modules

```python
hormone_state = endocrine.get_state()
# Returns: Dict[str, float] with 16 hormone concentrations
# + current_mode: str (RESTING, STRESSED, REWARD, SOCIAL, etc.)
```

### Reservoir → Cognitive

```python
temporal_embedding = esn.step(input_signal)
# Returns: np.ndarray of shape (output_dim,)
# Encodes temporal context for memory and decision-making
```

### MLGamer → GTAngel

```python
action = mlgamer.select_action(game_state)
# Returns: str action name
# GTAngel navigation executes the selected tactical action
```

### Cognitive → Avatar

```python
avatar.update_from_cognitive_mode(endocrine.current_mode, hormone_state)
expression.update_from_endocrine(hormone_state)
expression.apply_chaotic_micro_expressions()
expression.apply_aesthetic_bias()
```

## Autonomy Level Progression

| Level | What Changes | Key Indicator |
|-------|-------------|---------------|
| 2→3 | Mock → Real API calls | Actual LLM inference in Synthesize phase |
| 3→4 | Flat → Hierarchical ESN | Multi-scale temporal processing |
| 4→5 | Fixed → Self-modifying | Config mutation based on performance |
| 5→6 | Single → Recursive | Agent spawns sub-agents, self-trains |
