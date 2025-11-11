// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MBModularEnum.h"
#include "MBPropActionSettings.generated.h"

class UDetailsView;
class UMBPropDistributionBase;

UCLASS()
class MODULARBUILDING_API UMBPropActionSettings : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	
	void OnDistributionTypeChanged(EMBDistributionType NewDistributionType);

protected:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget),Category="Prop Distribution")
	TObjectPtr<UDetailsView> PropDistributionDetails;
	
	UPROPERTY()
	TObjectPtr<UMBPropDistributionBase> PropDistributionBase;
	
	UPROPERTY()
	FDistributionBaseData DistributionBaseData;
};
