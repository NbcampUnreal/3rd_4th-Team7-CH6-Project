#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHWarpMirror.generated.h"

class UBoxComponent;

UCLASS()
class SOH_API ASOHWarpMirror : public ASOHInteractableActor
{
	GENERATED_BODY()
	
public:
    ASOHWarpMirror();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Warp")
    UBoxComponent* WarpTrigger;

    UPROPERTY(VisibleAnywhere, Category = "Warp")
    USceneComponent* DestinationPoint;

    UPROPERTY(EditAnywhere, Category = "Warp")
    ASOHWarpMirror* TargetWarp;

    UPROPERTY(VisibleAnywhere, Category = "Warp")
    bool bCanWarp;

    FTimerHandle TimerHandle_ResetWarp;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void ResetWarp();
};
