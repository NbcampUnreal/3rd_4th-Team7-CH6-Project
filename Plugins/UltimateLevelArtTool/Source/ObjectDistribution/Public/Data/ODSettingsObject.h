// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODSettingsObject.generated.h"

UCLASS()
class OBJECTDISTRIBUTION_API UODSettingsObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Simulation" , meta=(NoResetToDefault,DisplayPriority=1))
	bool  bSimulatePhysics = true;

	UPROPERTY(EditAnywhere,Category = "Simulation" ,DisplayName="Disable Sim After Finishing", meta=(EditCondition = "bSimulatePhysics",ToolTip = "Disable simulate physics option on the actors after finishing the distribution.",NoResetToDefault,DisplayPriority=1))
	bool  bDisableSimAfterFinish = true;

	UPROPERTY(EditAnywhere,Category = "Visualization" ,DisplayName="Draw Spawn Bounds", meta=(NoResetToDefault))
	bool  bDrawSpawnBounds = true;
	
	UPROPERTY(EditAnywhere,Category = "Visualization" , meta=(EditCondition="bDrawSpawnBounds",/*EditConditionHides,*/HideAlphaChannel,NoResetToDefault))
	FLinearColor BoundsColor = FLinearColor( 0.339158f, 0.088386f, 0.953125f);

	//Functions
	void SetupSettingsObject();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
