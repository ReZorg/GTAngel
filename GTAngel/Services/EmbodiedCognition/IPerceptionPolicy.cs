using System.Collections.Generic;
using GTAngel.Models.EmbodiedCognition;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// Cognitive policy that maps a perception-limited view of the world to a
/// high-level <see cref="MotorIntent"/>.
///
/// Implementations MUST decide using only the supplied perceptual field and
/// memory snapshot — never the raw <see cref="GTAngel.Interop.AvatarObservation"/>.
/// This is the contract that enforces "limited world knowledge".
/// </summary>
public interface IPerceptionPolicy
{
    /// <summary>
    /// Choose the next motor intent.
    /// </summary>
    /// <param name="field">What the avatar can sense right now.</param>
    /// <param name="memory">What the avatar remembers about previously perceived objects.</param>
    /// <returns>A motor intent, or <c>null</c> for an idle tick.</returns>
    MotorIntent? Decide(PerceptualField field, IReadOnlyList<SpatialMemoryEntry> memory);
}
