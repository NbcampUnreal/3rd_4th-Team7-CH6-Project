// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODBoundsVisualizer.h"
#include "ODBoundsVisualizerComponent.h"
#include "SceneManagement.h"

void FODBoundsVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	const UODBoundsVisualizerComponent* BoundsVisualizerComponent = Cast<UODBoundsVisualizerComponent>(Component);
	if(!BoundsVisualizerComponent){return;}
	if(BoundsVisualizerComponent->GetBracketCorners().IsEmpty()){return;}
	
	for(int32 BracketCornerIndex=0; BracketCornerIndex<BoundsVisualizerComponent->GetBracketCorners().Num(); ++BracketCornerIndex)
	{
		// Direction corner axis should be pointing based on min/max
		const FVector CORNER = BoundsVisualizerComponent->GetBracketCorners()[BracketCornerIndex];
		const int32 DIR_X = CORNER.X == BoundsVisualizerComponent->MaxVector.X ? -1 : 1;
		const int32 DIR_Y = CORNER.Y == BoundsVisualizerComponent->MaxVector.Y ? -1 : 1;
		const int32 DIR_Z = CORNER.Z == BoundsVisualizerComponent->MaxVector.Z ? -1 : 1;
		
		PDI->DrawLine( CORNER, FVector(CORNER.X + (BoundsVisualizerComponent->BracketOffset * DIR_X), CORNER.Y, CORNER.Z), BoundsVisualizerComponent->BoundsColor, SDPG_Foreground );
		PDI->DrawLine( CORNER, FVector(CORNER.X, CORNER.Y + (BoundsVisualizerComponent->BracketOffset * DIR_Y), CORNER.Z), BoundsVisualizerComponent->BoundsColor, SDPG_Foreground );
		PDI->DrawLine( CORNER, FVector(CORNER.X, CORNER.Y, CORNER.Z + (BoundsVisualizerComponent->BracketOffset * DIR_Z)), BoundsVisualizerComponent->BoundsColor, SDPG_Foreground );
	}
}
