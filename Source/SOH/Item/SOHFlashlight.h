#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHFlashlight.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USpotLightComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ACharacter;

UCLASS()
class SOH_API ASOHFlashlight : public AActor
{
	GENERATED_BODY()
	
public:
    ASOHFlashlight();

    UFUNCTION(BlueprintCallable)
    void Toggle();

    UFUNCTION(BlueprintCallable)
    void InteractPickup(ACharacter* Picker);

protected:
    virtual void Tick(float DeltaSeconds) override;

private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Pivot;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    USpotLightComponent* Spot;


    UPROPERTY()
    ACharacter* OwnerChar;

    UPROPERTY()
    UCameraComponent* OwnerCam;

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY(EditAnywhere)
    FName HandSocketName;

    UPROPERTY(EditAnywhere)
    float IntensityOn;

    bool bOn;
    bool bEquipped;

    void SetOn(bool bEnable);
    void UpdateToCamera();
};
