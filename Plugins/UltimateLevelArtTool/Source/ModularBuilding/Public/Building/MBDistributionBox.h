// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/MBPropDistributionBase.h"
#include "MBDistributionBox.generated.h"

UCLASS()
class MODULARBUILDING_API UMBDistributionBox : public UMBPropDistributionBase
{
	GENERATED_BODY()

public:
	UMBDistributionBox(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,Category = "Distribution|Box")
	FVector SpawnRange = FVector(500.0f,500.0f,0.0f);
	
	virtual EMBDistributionType GetDistributionType() override {return EMBDistributionType::Box;}

private:
	virtual FVector CalculateLocation(const int32& InIndex,const int32& InLength) override;
	
};
