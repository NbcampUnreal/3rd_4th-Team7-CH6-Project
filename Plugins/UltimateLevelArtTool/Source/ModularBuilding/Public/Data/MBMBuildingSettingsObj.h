// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBModularEnum.h"
#include "MBMBuildingSettingsObj.generated.h"

enum class EMBModularSnapType : uint8;

UCLASS()
class MODULARBUILDING_API UMBMBuildingSettingsObj : public UObject
{
	GENERATED_BODY()
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void SetupObjectSettings();
	
	UPROPERTY(EditAnywhere, Category = "Modular Snapping",DisplayName="Enable Modular Snapping",meta=(NoResetToDefault))
	bool bEnableModularSnapping = true;

	UPROPERTY(EditAnywhere, Category = "Modular Snapping",meta=(NoResetToDefault))
	EMBModularSnapType SnappingType;
	
	UPROPERTY(EditAnywhere, Category = "Modular Snapping",meta = (EditCondition= "SnappingType == EMBModularSnapType::BoundCorrection",EditConditionHides,ClampMin = 0, NoResetToDefault,Delta = 50,NoResetToDefault))
	float BoundCorrectionSensitivity;
	
	UPROPERTY(EditAnywhere,Category = "Offset", meta=(InlineEditConditionToggle,NoResetToDefault))
	bool bSnapOffsetCond = false;
	
	UPROPERTY(EditAnywhere, Category = "Offset",meta = (EditCondition="bSnapOffsetCond",ClampMin = 0,ClampMax = 10000.0f, NoResetToDefault,Delta = 50))
	float SnapOffset;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Offset",DisplayName = "Z Offset")
	bool bZOffsetCond = false;
	
	UPROPERTY(EditAnywhere, Category = "Offset",meta = (EditCondition="bZOffsetCond",ClampMin = 0,ClampMax = 10000.0f, NoResetToDefault,Delta = 50))
	float ZOffset;
};