namespace GTAngel.Models;

/// <summary>
/// Deep Tree Echo cognitive state — mirrors the angelclaw DeepTreeEcho/Core architecture.
/// Tracks the 12-step cognitive cycle, 3 consciousness streams, and 4E cognition state.
/// </summary>
public class CognitiveState
{
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;

    // 12-Step Cognitive Cycle (Echobeats)
    public int CurrentCycleStep { get; set; } // 0-11
    public string CycleStepName => CycleStepNames[CurrentCycleStep % 12];
    public double CycleProgress => CurrentCycleStep / 12.0;

    // 3 Consciousness Streams
    public ConsciousnessStream SensoryStream { get; set; } = new() { Name = "Sensory" };
    public ConsciousnessStream CognitiveStream { get; set; } = new() { Name = "Cognitive" };
    public ConsciousnessStream AffectiveStream { get; set; } = new() { Name = "Affective" };

    // 4E Cognition State
    public double Embodied { get; set; }    // Motor readiness
    public double Embedded { get; set; }    // Niche coupling
    public double Enacted { get; set; }     // Sensorimotor engagement
    public double Extended { get; set; }    // Tool/environment extension

    // Emotional State (Valence-Arousal)
    public double Valence { get; set; }     // -1.0 (negative) to 1.0 (positive)
    public double Arousal { get; set; }     // 0.0 (calm) to 1.0 (excited)
    public double Stability { get; set; }   // 0.0 (unstable) to 1.0 (stable)

    // Cognitive Mode
    public CognitiveMode Mode { get; set; } = CognitiveMode.Exploration;

    // Wisdom Level (logarithmic growth)
    public double WisdomLevel { get; set; }

    // Introspection Depth
    public double IntrospectionDepth { get; set; }

    public static readonly string[] CycleStepNames =
    {
        "Perception",       // 0: Sensory intake
        "Attention",        // 1: Salience filtering
        "Recognition",      // 2: Pattern matching
        "Comprehension",    // 3: Meaning construction
        "Evaluation",       // 4: Value assessment
        "Planning",         // 5: Action selection
        "Intention",        // 6: Goal commitment
        "Execution",        // 7: Motor output
        "Monitoring",       // 8: Feedback loop
        "Learning",         // 9: Weight update
        "Consolidation",    // 10: Memory integration
        "Reflection"        // 11: Meta-cognitive review
    };
}

public class ConsciousnessStream
{
    public string Name { get; set; } = string.Empty;
    public double Coherence { get; set; }       // 0.0-1.0
    public double Activation { get; set; }      // 0.0-1.0
    public double[] ReservoirState { get; set; } = Array.Empty<double>();
    public List<double> CoherenceHistory { get; set; } = new();
}

public enum CognitiveMode
{
    Exploration,    // Seeking new information
    Exploitation,   // Using known strategies
    Combat,         // Reactive combat mode
    Navigation,     // Wayfinding
    Social,         // NPC interaction
    Introspection,  // Self-reflection
    Flow            // Optimal performance
}
