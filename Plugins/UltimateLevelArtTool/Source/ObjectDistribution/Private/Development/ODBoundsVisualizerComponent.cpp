// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODBoundsVisualizerComponent.h"
#include "Editor.h"
#include "ODToolSettings.h"
#include "ODToolSubsystem.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
UODBoundsVisualizerComponent::UODBoundsVisualizerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UODBoundsVisualizerComponent::ReDrawBounds()
{
	if(!GEditor){return;}
	BracketCorners.Empty();

	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSettingsSubsystem || !ToolSettingsSubsystem->GetODToolSettings()){return;}
	if(ToolSettingsSubsystem->CreatedDistObjects.IsEmpty()){return;}
	
	if(!ToolSettingsSubsystem->GetODToolSettings()->bDrawSpawnBounds){return;}

	BoundsColor = ToolSettingsSubsystem->GetODToolSettings()->BoundsColor;

	MinVector = FVector(BIG_NUMBER);
	MaxVector = FVector(-BIG_NUMBER);
	
	for(auto CurrentActor : ToolSettingsSubsystem->CreatedDistObjects)
	{
		if(!IsValid(CurrentActor)){continue;}

		const FBox ActorBox = CurrentActor->GetComponentsBoundingBox( true );

		// MinVector
		MinVector.X = FMath::Min<float>( ActorBox.Min.X, MinVector.X );
		MinVector.Y = FMath::Min<float>( ActorBox.Min.Y, MinVector.Y );
		MinVector.Z = FMath::Min<float>( ActorBox.Min.Z, MinVector.Z );
		// MaxVector
		MaxVector.X = FMath::Max<float>( ActorBox.Max.X, MaxVector.X );
		MaxVector.Y = FMath::Max<float>( ActorBox.Max.Y, MaxVector.Y );
		MaxVector.Z = FMath::Max<float>( ActorBox.Max.Z, MaxVector.Z );
	}

	BracketOffset = FVector::Dist(MinVector, MaxVector) * 0.1f;
	MinVector = MinVector - BracketOffset;
	MaxVector = MaxVector + BracketOffset;
	
	// Calculate bracket corners based on min/max vectors
	
	// Bottom Corners
	BracketCorners.Add(FVector(MinVector.X, MinVector.Y, MinVector.Z));
	BracketCorners.Add(FVector(MinVector.X, MaxVector.Y, MinVector.Z));
	BracketCorners.Add(FVector(MaxVector.X, MaxVector.Y, MinVector.Z));
	BracketCorners.Add(FVector(MaxVector.X, MinVector.Y, MinVector.Z));

	// Top Corners
	BracketCorners.Add(FVector(MinVector.X, MinVector.Y, MaxVector.Z));
	BracketCorners.Add(FVector(MinVector.X, MaxVector.Y, MaxVector.Z));
	BracketCorners.Add(FVector(MaxVector.X, MaxVector.Y, MaxVector.Z));
	BracketCorners.Add(FVector(MaxVector.X, MinVector.Y, MaxVector.Z));
}


