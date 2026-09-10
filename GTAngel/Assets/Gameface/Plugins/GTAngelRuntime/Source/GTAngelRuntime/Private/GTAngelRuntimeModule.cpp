#include "ArcAngelEchoCharacter.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Modules/ModuleManager.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogGTAngelRuntime, Log, All);

class FGTAngelRuntimeModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        WorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddRaw(
            this, &FGTAngelRuntimeModule::OnWorldInitialized);
    }

    virtual void ShutdownModule() override
    {
        if (WorldInitHandle.IsValid())
        {
            FWorldDelegates::OnPostWorldInitialization.Remove(WorldInitHandle);
            WorldInitHandle.Reset();
        }
    }

private:
    FDelegateHandle WorldInitHandle;

    void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues)
    {
        const FString CommandLine(FCommandLine::Get());
        const bool bRequested = FParse::Param(FCommandLine::Get(), TEXT("DTECognitive")) ||
            CommandLine.Contains(TEXT("-GTAngel_DTE_Avatar=1"), ESearchCase::IgnoreCase);
        if (!World || !World->IsGameWorld() || !bRequested)
        {
            return;
        }

        TWeakObjectPtr<UWorld> WeakWorld(World);
        World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakWorld]()
        {
            UWorld* RuntimeWorld = WeakWorld.Get();
            if (!RuntimeWorld)
            {
                return;
            }

            for (TActorIterator<AArcAngelEchoCharacter> It(RuntimeWorld); It; ++It)
            {
                UE_LOG(LogGTAngelRuntime, Display, TEXT("Arc Angel Echo already present: %s"), *It->GetName());
                return;
            }

            FVector SpawnLocation = FVector::ZeroVector;
            FRotator SpawnRotation = FRotator::ZeroRotator;
            if (const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(RuntimeWorld, 0))
            {
                SpawnLocation = PlayerPawn->GetActorLocation() +
                    PlayerPawn->GetActorForwardVector() * 150.f;
                SpawnRotation = PlayerPawn->GetActorRotation();
            }

            FActorSpawnParameters Parameters;
            Parameters.Name = TEXT("ArcAngelEcho");
            Parameters.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AArcAngelEchoCharacter* Angel = RuntimeWorld->SpawnActor<AArcAngelEchoCharacter>(
                AArcAngelEchoCharacter::StaticClass(), SpawnLocation, SpawnRotation, Parameters);
            if (Angel)
            {
                if (APlayerController* Controller = RuntimeWorld->GetFirstPlayerController())
                {
                    Controller->Possess(Angel);
                }
            }
            UE_LOG(LogGTAngelRuntime, Display, TEXT("Arc Angel Echo spawn: %s"), *GetNameSafe(Angel));
        }));
    }
};

IMPLEMENT_MODULE(FGTAngelRuntimeModule, GTAngelRuntime)
