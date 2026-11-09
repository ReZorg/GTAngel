# GTAngelEcho Living Centers

## Center 1: Cognitive Core (Deep Tree Echo)

**Module**: `src/deep-tree-echo/`
**Role**: Central cognitive processing via Echobeats cycle
**Properties**: Strong Centres (2), Deep Interlock (8)

The Cognitive Core implements the 4-phase Echobeats cycle that drives all higher-level behavior:
1. **Perceive** — Ingest sensory input and endocrine state
2. **Resonate** — Update Echo State Network dynamics
3. **Synthesize** — Integrate memory and current context via LLM
4. **Enact** — Generate action potentials and update endocrine

**Dependencies**: Reservoir Computing, Memory Systems, Emotional Dynamics
**Dependents**: All other centers (cognitive mode drives behavior)

---

## Center 2: Avatar Expression (SuperHotGirl)

**Module**: `src/superhotgirl/`
**Role**: Facial expression and aesthetic presentation
**Properties**: Good Shape (6), Local Symmetries (7)

Implements the SuperHotGirl aesthetic system:
- FACS Action Unit mapping from endocrine state
- Lorenz attractor chaotic micro-expressions
- 5 aesthetic parameters (Confidence, Charisma, Sparkle, Grace, Glow)
- MetaHuman DNA calibration interface

**Dependencies**: Emotional Dynamics (hormone input)
**Dependents**: Avatar Embodiment (expression feeds into body state)

---

## Center 3: Gameplay Intelligence (MLGamer)

**Module**: `src/mlgamer/`
**Role**: Tactical decision-making and adaptive learning
**Properties**: Levels of Scale (1), Gradients (10)

Implements adaptive gameplay AI:
- Q-learning for tactical decisions
- Opponent modeling and prediction
- Strategy evolution through experience
- Epsilon-greedy exploration with decay

**Dependencies**: Cognitive Core (context), Memory Systems (experience)
**Dependents**: World Navigation (action execution)

---

## Center 4: World Navigation (GTAngel)

**Module**: `src/gtangel/`
**Role**: Spatial navigation, pathfinding, and mission planning
**Properties**: Positive Space (5), Boundaries (3)

Implements open-world NPC intelligence:
- A* pathfinding with danger avoidance
- Mission queue with priority scheduling
- Environment scanning and affordance detection
- Landmark-based world representation

**Dependencies**: Gameplay Intelligence (tactical decisions)
**Dependents**: Avatar Embodiment (movement state)

---

## Center 5: Emotional Dynamics (Endocrine)

**Module**: `src/endocrine/`
**Role**: System-wide emotional signaling via virtual hormones
**Properties**: Alternating Repetition (4), Echoes (12)

Implements biologically-inspired emotional dynamics:
- 16-channel hormone bus (8 implemented, 8 reserved)
- 10 virtual glands responding to events
- Emergent cognitive mode detection
- Exponential decay toward baselines

**Dependencies**: None (receives events from all centers)
**Dependents**: All centers (hormone state modulates everything)

---

## Center 6: Reservoir Computing (ESN)

**Module**: `src/reservoir/`
**Role**: Temporal pattern processing via echo state dynamics
**Properties**: Roughness (11), The Void (13)

Implements Echo State Networks:
- Sparse random reservoir with controlled spectral radius
- Leaky integrator dynamics
- Ridge regression training
- Hierarchical multi-scale ESN (Deep Tree structure)
- Lyapunov exponent monitoring

**Dependencies**: Cognitive Core (input signals)
**Dependents**: Memory Systems (temporal embeddings), Cognitive Core (state)

---

## Center 7: Memory Systems (Cognitive)

**Module**: `src/cognitive/`
**Role**: Multi-type memory storage and retrieval
**Properties**: Not-Separateness (15), Simplicity (14)

Implements three memory types:
- **Episodic**: Experiences with emotional context and valence
- **Semantic**: Knowledge graph of facts and relationships
- **Procedural**: Learned action sequences with success rates

**Dependencies**: Reservoir Computing (temporal context), Emotional Dynamics (valence)
**Dependents**: Cognitive Core (memory retrieval), Gameplay Intelligence (experience)

---

## Center 8: Avatar Embodiment

**Module**: `src/avatar/`
**Role**: Physical body state and 4E cognition grounding
**Properties**: Contrast (9), Good Shape (6)

Implements 4E Cognition embodiment:
- **Embodied**: Body schema, posture, gait, breathing
- **Embedded**: Affordance detection from environment
- **Enacted**: World brought forth through interaction
- **Extended**: Tool use and cognitive extension

**Dependencies**: Cognitive Core (mode), Avatar Expression (face), World Navigation (position)
**Dependents**: Rendering pipeline (final avatar state)
