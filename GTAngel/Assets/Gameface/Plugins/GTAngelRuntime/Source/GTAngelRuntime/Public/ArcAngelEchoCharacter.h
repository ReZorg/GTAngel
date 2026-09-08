#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArcAngelEchoCharacter.generated.h"

class UArcAngelEchoComponent;
class UCameraComponent;
class USpringArmComponent;

/** Concrete in-world host for the Arc Angel Echo skeletal avatar. */
UCLASS(Blueprintable)
class GTANGELRUNTIME_API AArcAngelEchoCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AArcAngelEchoCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GTAngel|Arc Angel")
    TObjectPtr<UArcAngelEchoComponent> ArcAngelComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GTAngel|Arc Angel")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GTAngel|Arc Angel")
    TObjectPtr<UCameraComponent> FollowCamera;
};
