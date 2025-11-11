// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBUserSettingsObject.generated.h"


UCLASS()
class MODULARBUILDING_API UMBUserSettingsObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Placement",meta = (ClampMin = 5000.0f,ClampMax = 100000.0f,ShowOnlyInnerProperties, NoResetToDefault), DisplayName="Object Tracing Distance")
	float ObjectTracingDistance;
	
	UPROPERTY(EditAnywhere, Category = "Placement",meta = (ClampMin = 500.0f,ClampMax = 50000.0f,ShowOnlyInnerProperties, NoResetToDefault), DisplayName="Free Placement Distance")
	float FreePlacementDistance;

	UPROPERTY(EditAnywhere, Category = "Debugging", meta=(NoResetToDefault))
	bool UsePreviewShader = false;
	
	UPROPERTY(EditAnywhere, Category = "Debugging", DisplayName="Mesh Placeable Color", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	FLinearColor PlaceableColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modular Placement", DisplayName="Mesh Not Placeable Color", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	FLinearColor NotPlaceableColor;
	
	UPROPERTY(EditAnywhere, Category = "Debugging", DisplayName="Mesh Replaceable Color", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	FLinearColor ReplaceableColor;

	UPROPERTY(EditAnywhere, Category = "Debugging", DisplayName="Enable Direction Debugger", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	bool EnableDirectionDebugger;

	UPROPERTY(EditAnywhere, Category = "Debugging", DisplayName="Direction Debug Color",meta=(NoResetToDefault))
	FLinearColor DirectionDebugColor;

	UPROPERTY(EditAnywhere, Category = "Debugging",meta = (ClampMin = 0.1,ClampMax = 50.0), DisplayName="Debug Thickness", meta=(ShowOnlyInnerProperties, NoResetToDefault))
	float DebugThickness;
	
	UPROPERTY(EditAnywhere, Category = "User Interface", DisplayName="Asset Selection Color",meta=(NoResetToDefault))
	FLinearColor AssetSelectionColor;

	UPROPERTY(EditAnywhere, Category = "User Interface", DisplayName="Collection Selection Color",meta=(NoResetToDefault))
	FLinearColor CollectionSelectionColor;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void InitializeSettings();
};
