#include "ArcAngelEchoComponent.h"

#include "Animation/AnimSequence.h"
#include "Async/Async.h"
#include "Components/SkeletalMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformProcess.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if GTANGEL_WITH_NAMED_PIPES
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogArcAngelEcho, Log, All);

namespace ArcAngelPaths
{
    static FString ObjectPath(const FString& Root, const TCHAR* Name)
    {
        return FString::Printf(TEXT("%s/%s.%s"), *Root, Name, Name);
    }
}

UArcAngelEchoComponent::UArcAngelEchoComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 1.f / 30.f;
}

void UArcAngelEchoComponent::BeginPlay()
{
    Super::BeginPlay();

    AvatarMesh = GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
    if (!AvatarMesh && GetOwner())
    {
        AvatarMesh = NewObject<USkeletalMeshComponent>(GetOwner(), TEXT("ArcAngelEchoMesh"));
        AvatarMesh->SetupAttachment(GetOwner()->GetRootComponent());
        AvatarMesh->RegisterComponent();
        GetOwner()->AddInstanceComponent(AvatarMesh);
    }
    if (AvatarMesh)
    {
        BaseMeshRelativeRotation = AvatarMesh->GetRelativeRotation();
    }

    if (bAutoActivateProfile)
    {
        ActivateImportedProfile(DestinationPath);
    }

    if (bEnableEmbodimentPipe)
    {
        StartEmbodimentPipe();
    }
}

void UArcAngelEchoComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopEmbodimentPipe();
    Super::EndPlay(EndPlayReason);
}

void UArcAngelEchoComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    FString Json;
    int32 Processed = 0;
    while (Processed < 32 && PendingCommandBatches.Dequeue(Json))
    {
        ApplyCommandBatchJson(Json);
        ++Processed;
    }

    Processed = 0;
    while (Processed < 32 && PendingMainCommands.Dequeue(Json))
    {
        ApplyMainMessageJson(Json);
        ++Processed;
    }

    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        if (DesiredMoveSeconds > 0.f)
        {
            Character->AddMovementInput(Character->GetActorForwardVector(), DesiredMove.Y);
            Character->AddMovementInput(Character->GetActorRightVector(), DesiredMove.X);
            DesiredMoveSeconds = FMath::Max(0.f, DesiredMoveSeconds - DeltaTime);
        }
        else
        {
            DesiredMove = FVector2D::ZeroVector;
        }

        if (bHasNavigationTarget)
        {
            const FVector Delta = NavigationTarget - Character->GetActorLocation();
            if (Delta.Size2D() <= 100.f)
            {
                bHasNavigationTarget = false;
            }
            else
            {
                Character->AddMovementInput(Delta.GetSafeNormal2D(), 1.f);
            }
        }

        Character->GetCharacterMovement()->MaxWalkSpeed = bSprintRequested ? 650.f : 350.f;
    }

    ObservationAccumulator += DeltaTime;
    if (ObservationAccumulator >= 0.25f)
    {
        ObservationAccumulator = FMath::Fmod(ObservationAccumulator, 0.25f);
        BuildAndQueueObservation();
    }

    ApplyMaterialParameters();
    if (AvatarMesh)
    {
        AvatarMesh->SetRelativeRotation(BaseMeshRelativeRotation + FRotator(
            HeadPitch * 0.18f,
            HeadYaw * 0.18f,
            TorsoLean));
    }
    UpdateLocomotion();
}

bool UArcAngelEchoComponent::ActivateImportedProfile(const FString& InDestinationPath)
{
    if (!AvatarMesh)
    {
        UE_LOG(LogArcAngelEcho, Error, TEXT("Arc Angel activation failed: no skeletal mesh component"));
        return false;
    }

    DestinationPath = InDestinationPath.IsEmpty() ? DestinationPath : InDestinationPath;
    USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(
        nullptr, *ArcAngelPaths::ObjectPath(DestinationPath, TEXT("SK_ArcAngelEcho")));
    WalkAnimation = LoadObject<UAnimSequence>(
        nullptr, *ArcAngelPaths::ObjectPath(DestinationPath, TEXT("A_ArcAngelEcho_Walk")));
    RunAnimation = LoadObject<UAnimSequence>(
        nullptr, *ArcAngelPaths::ObjectPath(DestinationPath, TEXT("A_ArcAngelEcho_Run")));

    if (!Mesh)
    {
        UE_LOG(LogArcAngelEcho, Warning,
            TEXT("Arc Angel skeletal mesh not found under %s. Run import_arc_angel_echo.py in the editor."),
            *DestinationPath);
        bProfileActive = false;
        return false;
    }

    AvatarMesh->SetSkeletalMesh(Mesh);
    AvatarMesh->SetCollisionProfileName(TEXT("CharacterMesh"));
    AvatarMesh->SetGenerateOverlapEvents(false);

    if (AvatarMesh->GetNumMaterials() > 0)
    {
        DynamicMaterial = AvatarMesh->CreateAndSetMaterialInstanceDynamic(0);
        ApplyMaterialParameters();
    }

    bProfileActive = true;
    UE_LOG(LogArcAngelEcho, Display,
        TEXT("Arc Angel Echo activated: mesh=%s walk=%s run=%s"),
        *GetNameSafe(Mesh), *GetNameSafe(WalkAnimation), *GetNameSafe(RunAnimation));
    return true;
}

bool UArcAngelEchoComponent::ApplyCommandBatchJson(const FString& Json)
{
    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogArcAngelEcho, Warning, TEXT("Ignoring malformed GTAngel command batch"));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Commands = nullptr;
    if (!Root->TryGetArrayField(TEXT("Commands"), Commands) || Commands == nullptr)
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& Value : *Commands)
    {
        const TSharedPtr<FJsonObject> Object = Value.IsValid() ? Value->AsObject() : nullptr;
        if (Object.IsValid())
        {
            ApplyCommandObject(Object);
        }
    }
    return true;
}

void UArcAngelEchoComponent::ApplyCommandObject(const TSharedPtr<FJsonObject>& CommandObject)
{
    FString Module;
    FString Command;
    CommandObject->TryGetStringField(TEXT("Module"), Module);
    CommandObject->TryGetStringField(TEXT("Command"), Command);

    if (Module != TEXT("Avatar3DComponent"))
    {
        return;
    }

    const TSharedPtr<FJsonObject>* ParametersPtr = nullptr;
    CommandObject->TryGetObjectField(TEXT("Parameters"), ParametersPtr);
    const TSharedPtr<FJsonObject> Parameters = ParametersPtr ? *ParametersPtr : CommandObject;

    if (Command == TEXT("LoadAssetProfile"))
    {
        const TSharedPtr<FJsonObject>* ImportPtr = nullptr;
        FString RequestedDestination = DestinationPath;
        if (Parameters->TryGetObjectField(TEXT("Import"), ImportPtr) && ImportPtr && ImportPtr->IsValid())
        {
            (*ImportPtr)->TryGetStringField(TEXT("DestinationPath"), RequestedDestination);
        }
        ActivateImportedProfile(RequestedDestination);
    }
    else if (Command == TEXT("SetSkeletalAuraExpression"))
    {
        ApplySkeletalAuraExpression(Parameters);
    }
    else if (Command == TEXT("SetIKTargets"))
    {
        const TSharedPtr<FJsonObject>* TargetsPtr = nullptr;
        const TSharedPtr<FJsonObject> Targets =
            Parameters->TryGetObjectField(TEXT("IKTargets"), TargetsPtr) && TargetsPtr
                ? *TargetsPtr
                : Parameters;
        double Value = 0.0;
        if (Targets->TryGetNumberField(TEXT("HeadPitch"), Value)) HeadPitch = static_cast<float>(Value);
        if (Targets->TryGetNumberField(TEXT("HeadYaw"), Value)) HeadYaw = static_cast<float>(Value);
        if (Targets->TryGetNumberField(TEXT("TorsoLean"), Value)) TorsoLean = static_cast<float>(Value);
        if (Targets->TryGetNumberField(TEXT("GazeTarget"), Value)) GazeIntensity = FMath::Clamp(static_cast<float>(Value), 0.f, 1.f);
    }
    else if (Command == TEXT("ConfigureLocomotion"))
    {
        WalkAnimation = LoadObject<UAnimSequence>(
            nullptr, *ArcAngelPaths::ObjectPath(DestinationPath, TEXT("A_ArcAngelEcho_Walk")));
        RunAnimation = LoadObject<UAnimSequence>(
            nullptr, *ArcAngelPaths::ObjectPath(DestinationPath, TEXT("A_ArcAngelEcho_Run")));
        UE_LOG(LogArcAngelEcho, Display, TEXT("Arc Angel locomotion configured"));
    }
    else if (Command == TEXT("ConfigurePbrMaterial"))
    {
        double Value = 0.0;
        if (Parameters->TryGetNumberField(TEXT("EmissiveIntensity"), Value))
        {
            AuraIntensity = FMath::Clamp(static_cast<float>(Value), 0.f, 2.f);
        }
        ApplyMaterialParameters();
        UE_LOG(LogArcAngelEcho, Display, TEXT("Arc Angel PBR material configured"));
    }
    else if (Command == TEXT("ApplyPersonality"))
    {
        const TSharedPtr<FJsonObject>* TraitsPtr = nullptr;
        if (Parameters->TryGetObjectField(TEXT("Traits"), TraitsPtr) && TraitsPtr && TraitsPtr->IsValid())
        {
            double Confidence = 0.5;
            double Charm = 0.5;
            double Playfulness = 0.5;
            double Sass = 0.5;
            (*TraitsPtr)->TryGetNumberField(TEXT("Confidence"), Confidence);
            (*TraitsPtr)->TryGetNumberField(TEXT("Charm"), Charm);
            (*TraitsPtr)->TryGetNumberField(TEXT("Playfulness"), Playfulness);
            (*TraitsPtr)->TryGetNumberField(TEXT("Sass"), Sass);
            TorsoLean = FMath::Lerp(-2.f, 3.f, static_cast<float>(Confidence));
            AuraValence = FMath::Clamp(static_cast<float>((Charm + Playfulness) * 0.5), 0.f, 1.f);
            AuraIntensity = FMath::Clamp(
                AuraIntensity + static_cast<float>(Sass) * 0.08f, 0.f, 1.5f);
        }
    }
    else if (Command == TEXT("ConfigureExpressionDriver"))
    {
        UE_LOG(LogArcAngelEcho, Display, TEXT("Arc Angel expression driver configured"));
    }
}

void UArcAngelEchoComponent::ApplySkeletalAuraExpression(const TSharedPtr<FJsonObject>& Parameters)
{
    double Value = 0.0;
    if (Parameters->TryGetNumberField(TEXT("HeadPitch"), Value)) HeadPitch = static_cast<float>(Value);
    if (Parameters->TryGetNumberField(TEXT("HeadYaw"), Value)) HeadYaw = static_cast<float>(Value);
    if (Parameters->TryGetNumberField(TEXT("GazeIntensity"), Value)) GazeIntensity = FMath::Clamp(static_cast<float>(Value), 0.f, 1.f);
    if (Parameters->TryGetNumberField(TEXT("AuraIntensity"), Value)) AuraIntensity = FMath::Clamp(static_cast<float>(Value), 0.f, 1.f);
    if (Parameters->TryGetNumberField(TEXT("AuraValence"), Value)) AuraValence = FMath::Clamp(static_cast<float>(Value), 0.f, 1.f);
    if (Parameters->TryGetNumberField(TEXT("AuraArousal"), Value)) AuraArousal = FMath::Clamp(static_cast<float>(Value), 0.f, 1.f);
}

void UArcAngelEchoComponent::ApplyMaterialParameters()
{
    if (!DynamicMaterial)
    {
        return;
    }

    DynamicMaterial->SetScalarParameterValue(TEXT("AuraIntensity"), AuraIntensity);
    DynamicMaterial->SetVectorParameterValue(
        TEXT("AuraColor"),
        FLinearColor::LerpUsingHSV(
            FLinearColor(0.25f, 0.4f, 1.0f),
            FLinearColor(1.0f, 0.25f, 0.8f),
            AuraValence));
}

void UArcAngelEchoComponent::UpdateLocomotion()
{
    if (!AvatarMesh || AvatarMesh->GetAnimInstance() || !bProfileActive)
    {
        return;
    }

    const AActor* Owner = GetOwner();
    const float Speed = Owner ? Owner->GetVelocity().Size2D() : 0.f;
    UAnimSequence* Desired = Speed > 420.f ? RunAnimation : (Speed > 10.f ? WalkAnimation : nullptr);

    if (Desired && CurrentLocomotionAnimation != Desired)
    {
        AvatarMesh->PlayAnimation(Desired, true);
        CurrentLocomotionAnimation = Desired;
    }
    else if (!Desired && CurrentLocomotionAnimation)
    {
        AvatarMesh->Stop();
        CurrentLocomotionAnimation = nullptr;
    }
}

void UArcAngelEchoComponent::ApplyMainMessageJson(const FString& Json)
{
    TSharedPtr<FJsonObject> Message;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Message) || !Message.IsValid())
    {
        UE_LOG(LogArcAngelEcho, Warning, TEXT("Ignoring malformed GTAngel main-pipe message"));
        return;
    }

    FString Action;
    Message->TryGetStringField(TEXT("Action"), Action);
    if (Action == TEXT("AvatarAction"))
    {
        const TArray<TSharedPtr<FJsonValue>>* Extras = nullptr;
        if (Message->TryGetArrayField(TEXT("Extras"), Extras) && Extras && Extras->Num() > 0)
        {
            TSharedPtr<FJsonObject> ActionObject;
            const TSharedRef<TJsonReader<>> ActionReader =
                TJsonReaderFactory<>::Create((*Extras)[0]->AsString());
            if (FJsonSerializer::Deserialize(ActionReader, ActionObject) && ActionObject.IsValid())
            {
                ApplyAvatarAction(ActionObject);
            }
        }
    }
    else if (Action == TEXT("Shutdown"))
    {
        if (UWorld* World = GetWorld())
        {
            if (APlayerController* Controller = World->GetFirstPlayerController())
            {
                Controller->ConsoleCommand(TEXT("quit"));
            }
        }
    }
    else if (Action == TEXT("SetPlayerAiMode"))
    {
        Message->TryGetStringField(TEXT("Key"), PlayerMode);
    }
    else if (Action == TEXT("NavigateTo"))
    {
        FString Value;
        Message->TryGetStringField(TEXT("Value"), Value);
        TArray<FString> Parts;
        Value.ParseIntoArray(Parts, TEXT(","), true);
        if (Parts.Num() == 3)
        {
            NavigationTarget = FVector(
                FCString::Atof(*Parts[0]),
                FCString::Atof(*Parts[1]),
                FCString::Atof(*Parts[2]));
            bHasNavigationTarget = true;
        }
    }
    else if (Action == TEXT("CaptureMLVisionFrame"))
    {
        BuildAndQueueObservation();
    }
    else if (Action == TEXT("Pause"))
    {
        if (UWorld* World = GetWorld())
        {
            UGameplayStatics::SetGamePaused(World, true);
        }
    }
    else if (Action == TEXT("Resume"))
    {
        if (UWorld* World = GetWorld())
        {
            UGameplayStatics::SetGamePaused(World, false);
        }
    }
}

void UArcAngelEchoComponent::ApplyAvatarAction(const TSharedPtr<FJsonObject>& ActionObject)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    FString InputAction;
    ActionObject->TryGetStringField(TEXT("InputAction"), InputAction);
    double AxisX = 0.0;
    double AxisY = 0.0;
    double Magnitude = 1.0;
    double HoldDuration = 0.0;
    ActionObject->TryGetNumberField(TEXT("AxisX"), AxisX);
    ActionObject->TryGetNumberField(TEXT("AxisY"), AxisY);
    ActionObject->TryGetNumberField(TEXT("Magnitude"), Magnitude);
    ActionObject->TryGetNumberField(TEXT("HoldDuration"), HoldDuration);

    if (InputAction == TEXT("IA_Move") || InputAction == TEXT("IA_StrafeR") || InputAction == TEXT("IA_StrafeL"))
    {
        DesiredMove = FVector2D(
            static_cast<float>(AxisX * Magnitude),
            static_cast<float>(AxisY * Magnitude));
        if (InputAction == TEXT("IA_StrafeR")) DesiredMove.X = static_cast<float>(Magnitude);
        if (InputAction == TEXT("IA_StrafeL")) DesiredMove.X = -static_cast<float>(Magnitude);
        DesiredMoveSeconds = FMath::Max(0.30f, static_cast<float>(HoldDuration));
    }
    else if (InputAction == TEXT("IA_Look"))
    {
        Character->AddControllerYawInput(static_cast<float>(AxisX * Magnitude * 5.0));
        Character->AddControllerPitchInput(static_cast<float>(-AxisY * Magnitude * 5.0));
    }
    else if (InputAction == TEXT("IA_Jump") && Magnitude > 0.5)
    {
        Character->Jump();
    }
    else if (InputAction == TEXT("IA_Crouch"))
    {
        Magnitude > 0.5 ? Character->Crouch() : Character->UnCrouch();
    }
    else if (InputAction == TEXT("IA_Sprint"))
    {
        bSprintRequested = Magnitude > 0.5;
    }
}

void UArcAngelEchoComponent::BuildAndQueueObservation()
{
    const AActor* Owner = GetOwner();
    const UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return;
    }

    auto VectorToJson = [](const FVector& Value)
    {
        TArray<TSharedPtr<FJsonValue>> Result;
        Result.Add(MakeShared<FJsonValueNumber>(Value.X));
        Result.Add(MakeShared<FJsonValueNumber>(Value.Y));
        Result.Add(MakeShared<FJsonValueNumber>(Value.Z));
        return Result;
    };

    const FVector Position = Owner->GetActorLocation();
    const FRotator Rotation = Owner->GetActorRotation();
    const FVector Velocity = Owner->GetVelocity();

    TSharedRef<FJsonObject> Observation = MakeShared<FJsonObject>();
    Observation->SetNumberField(TEXT("Timestamp"), World->GetTimeSeconds());
    Observation->SetStringField(TEXT("FrameBase64"), TEXT(""));
    Observation->SetArrayField(TEXT("Position"), VectorToJson(Position));
    Observation->SetArrayField(TEXT("Rotation"), VectorToJson(FVector(Rotation.Pitch, Rotation.Yaw, Rotation.Roll)));
    Observation->SetArrayField(TEXT("Velocity"), VectorToJson(Velocity));

    TArray<TSharedPtr<FJsonValue>> ActiveActions;
    if (!DesiredMove.IsNearlyZero()) ActiveActions.Add(MakeShared<FJsonValueString>(TEXT("IA_Move")));
    if (bSprintRequested) ActiveActions.Add(MakeShared<FJsonValueString>(TEXT("IA_Sprint")));
    Observation->SetArrayField(TEXT("ActiveInputActions"), ActiveActions);

    TSharedRef<FJsonObject> Neuro = MakeShared<FJsonObject>();
    Neuro->SetNumberField(TEXT("Curiosity"), GazeIntensity);
    Neuro->SetNumberField(TEXT("Endorphin"), AuraValence);
    Neuro->SetNumberField(TEXT("ChaosIntensity"), AuraArousal);
    Neuro->SetNumberField(TEXT("Homeostasis"), 1.f - AuraArousal * 0.5f);
    Neuro->SetNumberField(TEXT("Abundance"), AuraIntensity);
    Neuro->SetNumberField(TEXT("Scarcity"), 1.f - AuraIntensity);
    Observation->SetObjectField(TEXT("NeurochemicalState"), Neuro);
    const TArray<TSharedPtr<FJsonValue>> PerceivedObjects;
    Observation->SetArrayField(TEXT("PerceivedObjects"), PerceivedObjects);
    Observation->SetStringField(TEXT("PlayerMode"), PlayerMode);
    Observation->SetNumberField(TEXT("ArbitrationScore"), 0.f);

    FString ObservationJson;
    const TSharedRef<TJsonWriter<>> ObservationWriter = TJsonWriterFactory<>::Create(&ObservationJson);
    FJsonSerializer::Serialize(Observation, ObservationWriter);

    TSharedRef<FJsonObject> Message = MakeShared<FJsonObject>();
    Message->SetStringField(TEXT("Action"), TEXT("MLVisionFrame"));
    Message->SetStringField(TEXT("Key"), TEXT("ArcAngelEcho"));
    Message->SetStringField(TEXT("Value"), TEXT("AvatarObservation"));
    TArray<TSharedPtr<FJsonValue>> Extras;
    Extras.Add(MakeShared<FJsonValueString>(ObservationJson));
    Message->SetArrayField(TEXT("Extras"), Extras);

    FString Line;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Line);
    FJsonSerializer::Serialize(Message, Writer);
    PendingMainMessages.Enqueue(Line + TEXT("\n"));
}

void UArcAngelEchoComponent::StartEmbodimentPipe()
{
#if GTANGEL_WITH_NAMED_PIPES
    bStopPipeThread = false;
    PipeFuture = Async(EAsyncExecution::Thread, [this]() { RunEmbodimentPipe(); });
    MainPipeFuture = Async(EAsyncExecution::Thread, [this]() { RunMainPipeClient(); });
#else
    UE_LOG(LogArcAngelEcho, Warning, TEXT("Named-pipe embodiment bridge is Win64-only"));
#endif
}

void UArcAngelEchoComponent::StopEmbodimentPipe()
{
    bStopPipeThread = true;
#if GTANGEL_WITH_NAMED_PIPES
    // Wake a blocking ConnectNamedPipe call, if present.
    HANDLE Wake = CreateFileW(
        L"\\\\.\\pipe\\GTAngel_Embodiment_IPC", GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (Wake != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Wake);
    }
#endif
    if (PipeFuture.IsValid())
    {
        PipeFuture.Wait();
    }
    if (MainPipeFuture.IsValid())
    {
        MainPipeFuture.Wait();
    }
}

void UArcAngelEchoComponent::RunEmbodimentPipe()
{
#if GTANGEL_WITH_NAMED_PIPES
    constexpr DWORD BufferSize = 64 * 1024;
    TArray<ANSICHAR> Buffer;
    Buffer.SetNumUninitialized(BufferSize);

    while (!bStopPipeThread)
    {
        HANDLE Pipe = CreateNamedPipeW(
            L"\\\\.\\pipe\\GTAngel_Embodiment_IPC",
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, BufferSize, BufferSize, 0, nullptr);
        if (Pipe == INVALID_HANDLE_VALUE)
        {
            FPlatformProcess::Sleep(1.f);
            continue;
        }

        const BOOL Connected = ConnectNamedPipe(Pipe, nullptr)
            ? TRUE
            : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!Connected || bStopPipeThread)
        {
            CloseHandle(Pipe);
            continue;
        }

        DWORD ReadMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
        SetNamedPipeHandleState(Pipe, &ReadMode, nullptr, nullptr);

        FString Accumulator;
        DWORD BytesRead = 0;
        while (!bStopPipeThread)
        {
            if (ReadFile(Pipe, Buffer.GetData(), BufferSize - 1, &BytesRead, nullptr) && BytesRead > 0)
            {
                Buffer[BytesRead] = '\0';
                Accumulator += UTF8_TO_TCHAR(Buffer.GetData());

                int32 Newline = INDEX_NONE;
                while (Accumulator.FindChar(TEXT('\n'), Newline))
                {
                    const FString Line = Accumulator.Left(Newline).TrimStartAndEnd();
                    Accumulator.RightChopInline(Newline + 1, EAllowShrinking::No);
                    if (!Line.IsEmpty())
                    {
                        PendingCommandBatches.Enqueue(Line);
                    }
                }
            }
            else
            {
                const DWORD Error = GetLastError();
                if (Error == ERROR_BROKEN_PIPE || Error == ERROR_PIPE_NOT_CONNECTED)
                {
                    break;
                }
                FPlatformProcess::Sleep(0.01f);
            }
        }

        DisconnectNamedPipe(Pipe);
        CloseHandle(Pipe);
    }
#endif
}

void UArcAngelEchoComponent::RunMainPipeClient()
{
#if GTANGEL_WITH_NAMED_PIPES
    constexpr DWORD BufferSize = 64 * 1024;
    TArray<ANSICHAR> Buffer;
    Buffer.SetNumUninitialized(BufferSize);

    while (!bStopPipeThread)
    {
        if (!WaitNamedPipeW(L"\\\\.\\pipe\\GTAngel_UE5_IPC", 250))
        {
            FPlatformProcess::Sleep(0.10f);
            continue;
        }

        HANDLE Pipe = CreateFileW(
            L"\\\\.\\pipe\\GTAngel_UE5_IPC",
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (Pipe == INVALID_HANDLE_VALUE)
        {
            FPlatformProcess::Sleep(0.10f);
            continue;
        }

        DWORD ReadMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
        SetNamedPipeHandleState(Pipe, &ReadMode, nullptr, nullptr);
        FString Accumulator;

        while (!bStopPipeThread)
        {
            FString Outgoing;
            while (PendingMainMessages.Dequeue(Outgoing))
            {
                FTCHARToUTF8 Utf8(*Outgoing);
                DWORD BytesWritten = 0;
                if (!WriteFile(Pipe, Utf8.Get(), Utf8.Length(), &BytesWritten, nullptr))
                {
                    break;
                }
            }

            DWORD BytesRead = 0;
            if (ReadFile(Pipe, Buffer.GetData(), BufferSize - 1, &BytesRead, nullptr) && BytesRead > 0)
            {
                Buffer[BytesRead] = '\0';
                Accumulator += UTF8_TO_TCHAR(Buffer.GetData());

                int32 Newline = INDEX_NONE;
                while (Accumulator.FindChar(TEXT('\n'), Newline))
                {
                    FString Line = Accumulator.Left(Newline).TrimStartAndEnd();
                    Accumulator.RightChopInline(Newline + 1, EAllowShrinking::No);
                    Line.RemoveFromStart(TEXT("\xFEFF"));
                    if (!Line.IsEmpty())
                    {
                        PendingMainCommands.Enqueue(Line);
                    }
                }
            }
            else
            {
                const DWORD Error = GetLastError();
                if (Error == ERROR_BROKEN_PIPE || Error == ERROR_PIPE_NOT_CONNECTED)
                {
                    break;
                }
            }

            FPlatformProcess::Sleep(0.01f);
        }

        CloseHandle(Pipe);
    }
#endif
}
