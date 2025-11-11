// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBPropActionSettings.h"
#include "MBDistributionBox.h"
#include "MBToolSubsystem.h"
#include "Building/MBCircleDistribution.h"
#include "Components/DetailsView.h"
#include "Building/MBPropDistributionBase.h"
#include "Building/MBSphereDistribution.h"
#include "Building/MBTDDistribution.h"

void UMBPropActionSettings::NativePreConstruct()
{
	Super::NativePreConstruct();

	if((PropDistributionBase = Cast<UMBPropDistributionBase>(NewObject<UMBDistributionBox>(this, TEXT("MBPropDistributionBase")))))
	{
		PropDistributionBase->OnDistributionTypeChangedSignature.BindUObject(this, &UMBPropActionSettings::OnDistributionTypeChanged);
		PropDistributionDetails->SetObject(PropDistributionBase);
	}
}

void UMBPropActionSettings::NativeDestruct()
{
	if(PropDistributionBase){PropDistributionBase->OnDistributionTypeChangedSignature.Unbind();}
	
	Super::NativeDestruct();
}

void UMBPropActionSettings::OnDistributionTypeChanged(EMBDistributionType NewDistributionType)
{
	DistributionBaseData = PropDistributionBase->GetDistributionBaseSettings();
	
	if((NewDistributionType == EMBDistributionType::Box))
	{
		if((PropDistributionBase = NewObject<UMBDistributionBox>(this, TEXT("MBBoxDistribution"))))
		{
			PropDistributionDetails->SetObject(PropDistributionBase);
		}
	}
	if(NewDistributionType == EMBDistributionType::Circle)
	{
		if((PropDistributionBase = NewObject<UMBCircleDistribution>(this, TEXT("MBCircleDistribution"))))
		{
			PropDistributionDetails->SetObject(PropDistributionBase);
		}
	}
	else if(NewDistributionType == EMBDistributionType::Sphere)
	{
		if((PropDistributionBase = NewObject<UMBSphereDistribution>(this, TEXT("MBSphereDistribution"))))
		{
			PropDistributionDetails->SetObject(PropDistributionBase);
		}
	}
	else if(NewDistributionType == EMBDistributionType::TDGrid)
	{
		if((PropDistributionBase = NewObject<UMBTDDistribution>(this, TEXT("MBTDDistribution"))))
		{
			PropDistributionDetails->SetObject(PropDistributionBase);
		}
	}
	PropDistributionBase->SetDistributionBaseSettings(DistributionBaseData);
	PropDistributionBase->ExecuteDistribution();
	Cast<UMBPropDistributionBase>(PropDistributionBase)->OnDistributionTypeChangedSignature.BindUObject(this, &UMBPropActionSettings::OnDistributionTypeChanged);
}
