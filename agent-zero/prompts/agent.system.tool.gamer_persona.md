## gamer_persona
Interact with "Lucy", the hyper-chaotic hardcore gamer-girl persona engine
(traits, skills, endocrine system, FACS expression, Lorenz chaos).

**Arguments:**
- `action` (string, required): one of
  - `train_step` - run one training iteration (optionally with `event`)
  - `trigger_event` - force a specific gameplay emotion event
  - `get_state` - persona summary; set `full: true` for complete state
  - `get_expression` - current composite expression + MetaHuman morph targets
  - `list_events` - list the 10 valid gameplay events
  - `reset` - reinitialize the persona
- `event` (string, optional): one of CLUTCH_MOMENT, VICTORY_ROYALE, EPIC_PLAY,
  GETTING_TILTED, TRASH_TALKING, FLOW_STATE, SURPRISE_ATTACK, TEAM_CARRY,
  RAGE_QUIT_RESIST, BORED_STOMPING
- `full` (boolean, optional): return full state for `get_state`

**Example:**
~~~json
{
    "thoughts": ["I should see how Lucy reacts to a clutch moment"],
    "tool_name": "gamer_persona",
    "tool_args": {
        "action": "trigger_event",
        "event": "CLUTCH_MOMENT"
    }
}
~~~
