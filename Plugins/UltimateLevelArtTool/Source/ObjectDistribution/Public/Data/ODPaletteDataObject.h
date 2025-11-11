// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODMeshData.h"
#include "ODPaletteDataObject.generated.h"

DECLARE_DELEGATE_OneParam(FOnPaletteObjectParamChanged,FName);

UCLASS()
class OBJECTDISTRIBUTION_API UODPaletteDataObject : public UObject
{
	GENERATED_BODY()

public:
	void SetupObject();
	
	virtual void BeginDestroy() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#pragma region ObjectData
	
	UPROPERTY(EditAnywhere, Category = "Object Data",NoClear,meta=(NoResetToDefault))
	TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Object Data",DisplayName="Override Material",meta=(ToolTip = "Assign new material for first material slot"),meta=(NoResetToDefault))
	bool bSelectRandomMaterial = false;
	
	UPROPERTY(EditAnywhere, DisplayName="New Material", meta=(EditCondition="bSelectRandomMaterial",EditConditionHides,NoResetToDefault),Category = "Object Data")
	TSoftObjectPtr<UMaterialInterface> SecondRandomMaterial = nullptr;

	UPROPERTY(EditAnywhere, DisplayName="Select Material Randomly", meta=(EditCondition="bSelectRandomMaterial",EditConditionHides,ToolTip = "It randomly changes the assigned material or doesn't change it at all.", NoResetToDefault),Category = "Object Data")
	bool bSelectMaterialRandomly = false;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0,NoResetToDefault),Category = "Object Data")
	int32 SpawnCount = 5;

	UPROPERTY(EditAnywhere, Category = "Object Data",meta=(NoResetToDefault))
	FVector2D  ScaleRange = FVector2D::UnitVector;

	UPROPERTY(EditAnywhere, Category = "Object Data",meta=(NoResetToDefault))
	float  LinearDamping = 0.01;

	UPROPERTY(EditAnywhere, Category = "Object Data",meta=(NoResetToDefault))
	float  AngularDamping = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Object Data",meta=(NoResetToDefault))
	float  Mass = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Object Data",meta=(NoResetToDefault))
	EObjectOrientation OrientationType = EObjectOrientation::Keep;

#pragma endregion ObjectData

private:
	UFUNCTION()
	void OnPresetLoaded();

public:
	void LoadSelectedObjectData();

	FOnPaletteObjectParamChanged OnPaletteObjectParamChanged;
};


