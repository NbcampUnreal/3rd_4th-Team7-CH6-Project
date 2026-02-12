#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHLamp.generated.h"

class UStaticMeshComponent;
class URectLightComponent;
class UMaterialInstanceDynamic;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    bool bStartOn = false;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* LampMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    URectLightComponent* RectLight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
    UMaterialInstanceDynamic* LampMaterialInstance;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lamp")
    bool bIsOn;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    float EmissiveOn = 0.09f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    float EmissiveOff = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    float LightIntensity = 5000.0f;
    
    void UpdateLightState();
};