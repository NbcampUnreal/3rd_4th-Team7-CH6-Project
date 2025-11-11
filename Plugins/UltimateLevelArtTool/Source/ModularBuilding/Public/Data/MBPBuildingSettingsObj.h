// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBPBuildingSettingsObj.generated.h"

UCLASS()
class MODULARBUILDING_API UMBPBuildingSettingsObj : public UObject
{
	GENERATED_BODY()
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void SetupObjectSettings();
	
	UPROPERTY(EditAnywhere, Category = "Prop Placement",DisplayName="Enable Modular Snapping",meta=(NoResetToDefault))
	bool bEnableSurfaceSnapping = false;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle,NoResetToDefault), Category = "Prop Placement",DisplayName = "Custom Rotation Rate")
	bool bCustomRotationRateCond = false;
	
	UPROPERTY(EditAnywhere, Category = "Prop Placement",meta = (EditCondition="bCustomRotationRateCond",ClampMin = 0,ClampMax = 359.0f, NoResetToDefault,Delta = 1))
	float CustomRotationRate = 15.0f;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle,NoResetToDefault), Category = "Prop Placement",DisplayName = "Scale Rate")
	bool bScaleRateCond = false;
	
	UPROPERTY(EditAnywhere, Category = "Prop Placement",meta = (EditCondition="bScaleRateCond",ClampMin = 0.1, NoResetToDefault,Delta = 0.1))
	float ScaleRate = 2.0f;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle,NoResetToDefault), Category = "Prop Placement",DisplayName = "Z Offset")
	bool bZOffsetCond = false;
	
	UPROPERTY(EditAnywhere, Category = "Prop Placement",DisplayName = "Z Offset",meta = (EditCondition="bZOffsetCond",ClampMin = -1000.0f,ClampMax = 50000.0f, NoResetToDefault,Delta = 50))
	float ZOffset = 250.0f;
};