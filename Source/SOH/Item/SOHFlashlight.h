#pragma once

#include "CoreMinimal.h"
#include "Item/SOHBaseItem.h"
#include "SOHFlashlight.generated.h"

class USceneComponent;
class USpotLightComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ACharacter;

UCLASS()
class SOH_API ASOHFlashlight : public ASOHBaseItem
{
    GENERATED_BODY()

public:
    ASOHFlashlight();

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void Toggle();

protected:
    virtual void BeginPlay() override;

    virtual void Interact_Implementation(AActor* Caller) override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Pivot;

    UPROPERTY(VisibleAnywhere)
    USpotLightComponent* Spot;

    UPROPERTY()
    ACharacter* OwnerChar;

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    FName HandSocketName = TEXT("FlashlightSocket");

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    float IntensityOn = 4000.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    bool bStartOn = false;

    bool bOn = false;
    bool bEquipped = false;

    void SetOn(bool bEnable);
    void SetEquipped(ACharacter* NewOwner);
};