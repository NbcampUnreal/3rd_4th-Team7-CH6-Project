// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ODBoundsVisualizerComponent.generated.h"

UCLASS(ClassGroup=(Custom)) 
	
class OBJECTDISTRIBUTION_API UODBoundsVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	TArray<FVector> BracketCorners;
	FVector MinVector;
	FVector MaxVector;
	float BracketOffset;
	FLinearColor BoundsColor;
	
	const TArray<FVector>& GetBracketCorners()const {return BracketCorners;}
	void SetBracketCorners(const TArray<FVector>& InNewBracketCorners) {BracketCorners = InNewBracketCorners;}
	
	// Sets default values for this component's properties
	UODBoundsVisualizerComponent();

	void ReDrawBounds();
};
