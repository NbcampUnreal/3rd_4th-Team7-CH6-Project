#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SOHCombinationLock.generated.h"

class UStaticMeshComponent;
class USoundBase;
class ASOHOpenDoor;

USTRUCT(BlueprintType)
struct FDigit
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int32 CurrentDigit = 0;
};

UCLASS()
class SOH_API ASOHCombinationLock : public ASOHInteractableActor
{
    GENERATED_BODY()

public:
    ASOHCombinationLock();
    virtual void Interact_Implementation(AActor* Caller) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    FGameplayTag PuzzleClearTag;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Components
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Body;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Head;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Gear1;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Gear2;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Gear3;
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* CameraComponent;

    // Gears array
    UPROPERTY()
    TArray<UStaticMeshComponent*> Gears;

    // Gear rotation
    UPROPERTY()
    TArray<FRotator> GearCurrentRot;
    UPROPERTY()
    TArray<FRotator> GearTargetRot;
    UPROPERTY()
    TArray<int32> GearValues;

    int32 CurrentGearIndex = 0;

    // Combination
    UPROPERTY(EditAnywhere)
    TArray<FDigit> CorrectCombination;

    // UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock")
    TSubclassOf<UUserWidget> LockGuideWidgetClass;
    UPROPERTY()
    UUserWidget* LockGuideWidget;

    // Door Class
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock")
    ASOHOpenDoor* LinkedDoor;

    // Materials & Sounds
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* GearRotateSound;
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* UnlockSound;

    // Movement/rotation speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock")
    float GearLerpSpeed = 180.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock")
    float HeadMoveSpeed = 50.f;

    // Head animation
    FVector HeadStartLocation;
    FVector HeadTargetLocation;
    bool bIsHeadMoving = false;

    // Input functions
    void Up();
    void Down();
    void Left();
    void Right();
    void Exit();

    void CheckCombination();
    void PlayUnlockAnimation();
    void UnlockComplete();

    // void UpdateHighlight();
};
