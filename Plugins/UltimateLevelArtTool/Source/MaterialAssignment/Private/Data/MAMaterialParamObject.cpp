// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Data/MAMaterialParamObject.h"


void UMAMaterialParamObject::BeginDestroy()
{
	UObject::BeginDestroy();
	
	OnMaterialParamChanged.Unbind();
}

void UMAMaterialParamObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	OnMaterialParamChanged.ExecuteIfBound();
}
