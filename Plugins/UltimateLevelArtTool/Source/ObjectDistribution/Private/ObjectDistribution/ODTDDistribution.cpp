// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODTDDistribution.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODTDDistribution::UODTDDistribution(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Grid;
}
void UODTDDistribution::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& GridData = GetToolSubsystem()->GetODToolSettings()->GridDistributionData;

		GridSize = GridData.GridSize;
		GridLength = GridData.GridLength;
		Chaos = GridData.Chaos;
	}
}

void UODTDDistribution::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& GridData = GetToolSubsystem()->GetODToolSettings()->GridDistributionData;

		GridData.GridSize = GridSize;
		GridData.GridLength = GridLength;
		GridData.Chaos = Chaos;

		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}

FVector UODTDDistribution::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	if(GridSize.X <= 0 || GridSize.Y <= 0){return FVector::ZeroVector;}
	
	const int32 ZValue = InIndex / (GridSize.X * GridSize.Y);
	const int32 XYMultiplier = InIndex - (ZValue * GridSize.X * GridSize.Y);

	const float XValue = static_cast<float>(XYMultiplier % GridSize.X);
	const float YValue = static_cast<float>(XYMultiplier / GridSize.Y);

	if(!Chaos)
	{
		return FVector(XValue,YValue,ZValue)  * GridLength;

	}
	return FVector(XValue + FMath::RandRange(-0.4,0.4),YValue + FMath::RandRange(-0.4,0.4),ZValue + FMath::RandRange(-0.4,0.4))  * GridLength;

}
