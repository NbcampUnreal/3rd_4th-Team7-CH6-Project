// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ObjectDistribution/ODDistributionLine.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"

UODDistributionLine::UODDistributionLine(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	DistributionType = EObjectDistributionType::Line;
}

void UODDistributionLine::LoadDistData()
{
	Super::LoadDistData();
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		const auto& LineData = GetToolSubsystem()->GetODToolSettings()->LineDistributionData;
		LineAxis  = LineData.LineAxis;
		LineLength =LineData.LineLength;
		bRandomOffset = LineData.bRandomOffset;
		RandomOffset = LineData.RandomOffset;	
		PivotCentered = LineData.PivotCentered;
	}
}

void UODDistributionLine::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(GetToolSubsystem() && GetToolSubsystem()->GetODToolSettings())
	{
		auto& LineData = GetToolSubsystem()->GetODToolSettings()->LineDistributionData;
		LineData.LineAxis = LineAxis;
		LineData.LineLength = LineLength;
		LineData.bRandomOffset = bRandomOffset;
		LineData.RandomOffset = RandomOffset;
		LineData.PivotCentered = PivotCentered;

		if(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GetToolSubsystem()->GetODToolSettings()->SaveConfig();
		}
	}
}


FVector UODDistributionLine::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	FVector DirectionVector;

	if(LineAxis == EODLineAxis::AxisX)
	{
		DirectionVector = FVector::XAxisVector;
	}
	else if(LineAxis == EODLineAxis::AxisY)
	{
		DirectionVector = FVector::YAxisVector;
	}
	else
	{
		DirectionVector = FVector::ZAxisVector;
	}

	const float InitialOffset  = PivotCentered ? LineLength * -1.0f / 2.0f : 0.0f;

	const float SafeLength = InLength == 1 ? 2 : InLength;
	
	DirectionVector = (InitialOffset * DirectionVector) + (DirectionVector * ((LineLength / ( SafeLength - 1)) * InIndex));

	if(bRandomOffset)
	{
		//Add Random Offset
		return DirectionVector +=
		FVector(
		FMath::FRandRange(-RandomOffset.X, RandomOffset.X),
		FMath::FRandRange(-RandomOffset.Y, RandomOffset.Y),
		FMath::FRandRange(-RandomOffset.Z, RandomOffset.Z)
		);
	}
	return DirectionVector;
}


