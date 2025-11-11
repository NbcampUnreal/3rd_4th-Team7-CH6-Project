// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBPropBuildingSettings.generated.h"


UCLASS()
class MODULARBUILDING_API UMBPropBuildingSettings : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UMBPBuildingSettingsObj> PropBuildingSettingsObj;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UDetailsView> MBPDetails;


public:
		virtual void NativePreConstruct() override;
};
