#pragma once

#include "CoreMinimal.h"
#include "Async/Future.h"
#include "Containers/Queue.h"
#include "Components/ActorComponent.h"
#include "ArcAngelEchoComponent.generated.h"

class UAnimSequence;
class UMaterialInstanceDynamic;
class USkeletalMesh;
class USkeletalMeshComponent;
class FJsonObject;

/**
 * Runtime endpoint for the Arc Angel Echo profile imported by
 * Content/Python/import_arc_angel_echo.py.
 *
 * The component consumes newline-delimited GTAngel_Embodiment_IPC command
 * batches, resolves the imported UE assets, and projects cognitive expression
 * into skeletal head/gaze targets plus PBR neon-aura parameters. Head/gaze
 * values are Blueprint-readable so an AnimBP or Control Rig can consume them.
 */
UCLASS(ClassGroup=(GTAngel), meta=(BlueprintSpawnableComponent))
class GTANGELRUNTIME_API UArcAngelEchoComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UArcAngelEchoComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /** Load the editor-imported Arc Angel assets from DestinationPath. */
    UFUNCTION(BlueprintCallable, Category="GTAngel|Arc Angel")
    bool ActivateImportedProfile(const FString& InDestinationPath);

    /** Apply one JSON command batch received from the WPF embodiment service. */
    UFUNCTION(BlueprintCallable, Category="GTAngel|Arc Angel")
    bool ApplyCommandBatchJson(const FString& Json);

    UFUNCTION(BlueprintPure, Category="GTAngel|Arc Angel")
    USkeletalMeshComponent* GetAvatarMesh() const { return AvatarMesh; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GTAngel|Arc Angel")
    FString DestinationPath = TEXT("/Game/GTAngel/Avatars/ArcAngelEcho");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GTAngel|Arc Angel")
    bool bAutoActivateProfile = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GTAngel|IPC")
    bool bEnableEmbodimentPipe = true;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float HeadPitch = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float HeadYaw = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float GazeIntensity = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float AuraIntensity = 0.35f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float AuraValence = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel|Expression")
    float AuraArousal = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="GTAngel|Arc Angel")
    bool bProfileActive = false;

private:
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> AvatarMesh;

    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> WalkAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> RunAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

    TQueue<FString, EQueueMode::Mpsc> PendingCommandBatches;
    TQueue<FString, EQueueMode::Mpsc> PendingMainCommands;
    TQueue<FString, EQueueMode::Mpsc> PendingMainMessages;
    TAtomic<bool> bStopPipeThread{false};
    TFuture<void> PipeFuture;
    TFuture<void> MainPipeFuture;

    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> CurrentLocomotionAnimation;
    float ObservationAccumulator = 0.f;
    FVector2D DesiredMove = FVector2D::ZeroVector;
    float DesiredMoveSeconds = 0.f;
    bool bSprintRequested = false;
    FRotator BaseMeshRelativeRotation = FRotator::ZeroRotator;
    float TorsoLean = 0.f;
    FString PlayerMode = TEXT("AI");
    FVector NavigationTarget = FVector::ZeroVector;
    bool bHasNavigationTarget = false;

    void StartEmbodimentPipe();
    void StopEmbodimentPipe();
    void RunEmbodimentPipe();
    void RunMainPipeClient();
    void ApplyMainMessageJson(const FString& Json);
    void ApplyAvatarAction(const TSharedPtr<FJsonObject>& ActionObject);
    void BuildAndQueueObservation();
    void ApplyCommandObject(const TSharedPtr<FJsonObject>& CommandObject);
    void ApplySkeletalAuraExpression(const TSharedPtr<FJsonObject>& Parameters);
    void ApplyMaterialParameters();
    void UpdateLocomotion();
};
