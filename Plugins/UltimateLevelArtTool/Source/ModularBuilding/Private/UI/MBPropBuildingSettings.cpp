// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBPropBuildingSettings.h"
#include "MBPBuildingSettingsObj.h"
#include "Components/DetailsView.h"


void UMBPropBuildingSettings::NativePreConstruct()
{
	Super::NativePreConstruct();
#if WITH_EDITOR

	if((PropBuildingSettingsObj = Cast<UMBPBuildingSettingsObj>(NewObject<UMBPBuildingSettingsObj>(this, TEXT("PropBuildingSettingsObj")))))
	{
		PropBuildingSettingsObj->SetupObjectSettings();
		MBPDetails->SetObject(PropBuildingSettingsObj);
	}

#endif
}
