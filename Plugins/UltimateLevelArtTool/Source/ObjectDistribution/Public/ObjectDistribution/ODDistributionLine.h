// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODMeshData.h"
#include "ODDistributionLine.generated.h"

UCLASS()
class OBJECTDISTRIBUTION_API UODDistributionLine : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODDistributionLine(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void LoadDistData() override; 
	
	UPROPERTY(EditAnywhere,Category = "Distribution")
	EODLineAxis LineAxis  = EODLineAxis::AxisX;
	
	UPROPERTY(EditAnywhere,meta=(ClampMin = 1.0f),Category = "Distribution")
	float LineLength = 5000.0f;

	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Distribution")
	bool bRandomOffset = false;
	
	UPROPERTY(EditAnywhere,meta=(EditCondition="bRandomOffset"),Category = "Distribution")
	FVector RandomOffset = FVector::ZeroVector;	

	UPROPERTY(EditAnywhere,DisplayName="Pivot Centered Distribution",Category = "Distribution")
	bool PivotCentered = true;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Line;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
