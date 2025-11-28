#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHLamp.generated.h"

class UStaticMeshComponent;
class URectLightComponent;

UCLASS()
class SOH_API ASOHLamp : public AActor
{
    GENERATED_BODY()

public:
    ASOHLamp();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Lamp")
    void ToggleLight();

    UFUNCTION(BlueprintCallable, Category = "Lamp")
    void TurnOn();

    UFUNCTION(BlueprintCallable, Category = "Lamp")
    void TurnOff();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* LampMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    URectLightComponent* RectLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lamp")
    bool bIsOn;
};