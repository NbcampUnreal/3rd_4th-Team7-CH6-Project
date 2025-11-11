// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "ASSplineComponent.h"


#if WITH_EDITOR
void UASSplineComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnComponentChanged.ExecuteIfBound();
}
#endif
