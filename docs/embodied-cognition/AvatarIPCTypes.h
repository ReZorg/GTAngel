// Copyright (c) ReZorg / GTAngel.
// SPDX-License-Identifier: same as repository.
//
// =============================================================================
//  AvatarIPCTypes.h
//
//  UE5-side mirror of the .NET IPC contract used by the embodied cognition
//  pipeline. This file is a *contract reference* — it is not built directly
//  by the .NET solution. Copy it into the UE5 plugin module that hosts the
//  GTAngel avatar (e.g. `GTAngelRuntime/Public/AvatarIPCTypes.h`), then keep
//  it in sync with the .NET originals when either side changes:
//
//    .NET                                          UE5 (this file)
//    GTAngel/Interop/AvatarObservation.cs   <-->   FAvatarObservation
//    GTAngel/Interop/AvatarAction.cs        <-->   FAvatarAction
//    GTAngel/Interop/PerceivedObject.cs     <-->   FPerceivedObject
//    GTAngel/Interop/NeurochemicalSnapshot.cs <-->  FNeurochemicalSnapshot
//
//  Both sides serialise via JSON over the named pipe
//  GTAngel_UE5_IPC (see GTAngel/Interop/UE5ProcessManager.cs:33). Field
//  names below MUST match the .NET property names *exactly* — JSON property
//  matching is case-sensitive on the .NET side
//  (System.Text.Json default behaviour).
//
//  Coordinate, unit, and yaw conventions are documented in
//  docs/EMBODIED_COGNITION_UE5_SPEC.md §2. The high-level summary:
//    - Position / Location / Velocity in world UU (≈ cm).
//    - Rotation = (Pitch, Yaw, Roll) in degrees; Rotation[1] = Yaw, 0° = +X.
//    - Distance: engine-reported 3D Euclidean (or 0 to let .NET compute planar).
//    - IsVisible: per-tick line-of-sight result, NOT "is in perception list".
// =============================================================================

#pragma once

#include "CoreMinimal.h"
#include "AvatarIPCTypes.generated.h"

// -----------------------------------------------------------------------------
// FNeurochemicalSnapshot — mirror of NeurochemicalSnapshot.cs
// -----------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FNeurochemicalSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Curiosity = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Endorphin = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float ChaosIntensity = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Homeostasis = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Abundance = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Scarcity = 0.f;
};

// -----------------------------------------------------------------------------
// FPerceivedObject — mirror of PerceivedObject.cs
//
// Tag MUST be stable across ticks for moving objects (use "NPC:<id>",
// "Vehicle:<id>" patterns). IsVisible MUST reflect the engine's actual LoS
// check; do NOT set it true unconditionally.
// -----------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FPerceivedObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FString Tag;

	/** Engine-reported distance in UU (typically 3D Euclidean). 0 lets the .NET side compute planar. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Distance = 0.f;

	/** World-space location (X, Y, Z) in UU. Length must be 3. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<float> Location;

	/** True iff the engine's per-tick line-of-sight check passed for this object. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	bool IsVisible = false;
};

// -----------------------------------------------------------------------------
// FAvatarObservation — mirror of AvatarObservation.cs
//
// Emit at >= 4 Hz (250 ms) — the .NET embodied decision loop polls at this
// cadence. Higher rates are fine; lower rates will starve the cognition
// loop and break SpatialMemory decay timing.
// -----------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FAvatarObservation
{
	GENERATED_BODY()

	/** Engine time in seconds. MUST be monotonically increasing. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	double Timestamp = 0.0;

	/** 768x768 RGB frame as base64 PNG (optional; consumed by ESN, not by cognition). */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FString FrameBase64;

	/** Avatar world position (X, Y, Z) in UU. Length must be 3. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<float> Position;

	/** Avatar rotation (Pitch, Yaw, Roll) in degrees. Length must be 3. Rotation[1] is Yaw. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<float> Rotation;

	/** Avatar linear velocity (X, Y, Z) in UU/s. Length must be 3. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<float> Velocity;

	/** Currently active Enhanced Input Action names. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<FString> ActiveInputActions;

	/** Optional snapshot of the DTE NeurochemicalSystem state. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FNeurochemicalSnapshot NeurochemicalState;

	/** Objects detected by the UE5 AI Perception component this tick. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	TArray<FPerceivedObject> PerceivedObjects;

	/** KSM Cycle 6 arbitration mode: "Human", "AI", or "Arbitrated". */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FString PlayerMode = TEXT("Human");

	/** KSM Cycle 6 arbitration score. 0.0 = AI, 1.0 = Human. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float ArbitrationScore = 1.f;
};

// -----------------------------------------------------------------------------
// FAvatarAction — mirror of AvatarAction.cs
//
// Received from .NET each tick (~4 Hz). InputAction is the name of an
// Enhanced Input Action asset. The eight names the embodied MotorController
// can emit are:
//   IA_Move, IA_Look, IA_Sprint, IA_Crouch, IA_Interact,
//   IA_StrafeR, IA_StrafeL, IA_Jump
//
// IA_Move:    AxisX = strafe right, AxisY = forward (body frame).
// IA_Look:    AxisX = yaw delta in normalised units; AxisY = 0.
// IA_Crouch:  toggle on rising-edge (Magnitude alternates 1/0).
// Others:     Magnitude == 1 ⇒ activate; HoldDuration controls hold time.
// -----------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct FAvatarAction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FString InputAction;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float Magnitude = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float AxisX = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float AxisY = 0.f;

	/** Hold duration in seconds. 0 == single-frame impulse. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	float HoldDuration = 0.f;

	/** Provenance label, e.g. "Reactive:Approach:Pickup". Telemetry only. */
	UPROPERTY(BlueprintReadWrite, Category = "GTAngel|IPC")
	FString Source;
};

// -----------------------------------------------------------------------------
// JSON wire format
// -----------------------------------------------------------------------------
//
// The .NET side reads each line as a UEMessage (see UE5ProcessManager.cs:347).
// AvatarObservations are sent as a UEMessage where:
//   Type   = "AvatarObservation"
//   Extras = [ <FAvatarObservation serialised as JSON> ]
//
// Suggested UE5 emission (pseudocode):
//
//   FAvatarObservation Obs = BuildObservationFromAvatar(Pawn);
//   FString ObsJson;
//   FJsonObjectConverter::UStructToJsonObjectString(Obs, ObsJson);
//
//   TSharedRef<FJsonObject> Msg = MakeShared<FJsonObject>();
//   Msg->SetStringField(TEXT("Type"), TEXT("AvatarObservation"));
//   Msg->SetArrayField(TEXT("Extras"), { MakeShared<FJsonValueString>(ObsJson) });
//
//   FString Line;
//   TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Line);
//   FJsonSerializer::Serialize(Msg, Writer);
//   PipeWriter->WriteLine(Line);
//
// Conversely, when consuming an FAvatarAction:
//
//   FAvatarAction Action;
//   FJsonObjectConverter::JsonObjectStringToUStruct(InExtras[0], &Action, 0, 0);
//   ApplyEnhancedInput(Action);  // dispatch to UInputSubsystem
//
