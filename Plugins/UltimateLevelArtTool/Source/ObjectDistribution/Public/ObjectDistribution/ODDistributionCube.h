// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ODDistributionBase.h"
#include "ODDistributionCube.generated.h"


UCLASS()
class OBJECTDISTRIBUTION_API UODDistributionCube : public UODDistributionBase
{
	GENERATED_BODY()

public:
	UODDistributionCube(const FObjectInitializer& ObjectInitializer);

	virtual void LoadDistData() override; 

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere,Category = "Distribution")
	float ScaleDistance = 250.0f;
	
	UPROPERTY(EditAnywhere, meta=(InlineEditConditionToggle), Category = "Distribution")
	bool bRandomOffset = false;
	
	UPROPERTY(EditAnywhere,meta=(EditCondition="bRandomOffset"),Category = "Distribution")
	FVector RandomOffset = FVector::ZeroVector;
	
	virtual EObjectDistributionType GetDistributionType() override {return EObjectDistributionType::Cube;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
