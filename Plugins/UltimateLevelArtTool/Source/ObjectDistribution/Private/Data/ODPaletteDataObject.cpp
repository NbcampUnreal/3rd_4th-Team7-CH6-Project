// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODPaletteDataObject.h"
#include "Editor.h"
#include "ODPresetData.h"
#include "ODToolSubsystem.h"


void UODPaletteDataObject::SetupObject()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->OnPresetLoaded.AddUObject(this,&UODPaletteDataObject::OnPresetLoaded);

		if(!ToolSubsystem->GetLastSelectedPreset().IsNone())
		{
			LoadSelectedObjectData();
		}
	}
}

void UODPaletteDataObject::OnPresetLoaded()
{
	LoadSelectedObjectData();
}

void UODPaletteDataObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	auto SelectedObjectData = ToolSubsystem->GetSelectedDistObjectData();
	
	if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("StaticMesh"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->StaticMesh = StaticMesh;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("bSelectRandomMaterial"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->bSelectRandomMaterial = bSelectRandomMaterial;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("SecondRandomMaterial"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->SecondRandomMaterial = SecondRandomMaterial;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("bSelectMaterialRandomly"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->bSelectMaterialRandomly = bSelectMaterialRandomly;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("SpawnCount"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.SpawnCount = SpawnCount;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("ScaleRange"))) ||
	           PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("X"))) ||
		       PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("Y"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.ScaleRange = ScaleRange;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("LinearDamping"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.LinearDamping = LinearDamping;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("AngularDamping"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.AngularDamping = AngularDamping;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("Mass")))) {
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.Mass = Mass;
	    }
	} else if (PropertyChangedEvent.GetPropertyName().IsEqual(FName(TEXT("OrientationType"))))
	{
	    for (const auto CurrentData : SelectedObjectData) {
	        CurrentData->DistributionProperties.OrientationType = OrientationType;
	    }
	}

	OnPaletteObjectParamChanged.ExecuteIfBound(PropertyChangedEvent.GetPropertyName());
	
}


void UODPaletteDataObject::LoadSelectedObjectData()
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}

	const auto& SlotIndexes = ToolSubsystem->SelectedPaletteSlotIndexes;
	if(SlotIndexes.IsEmpty()){return;}
	
	if(const auto& DistObjectData = &ToolSubsystem->ObjectDistributionData)
	{
		if(SlotIndexes.Num() == 0)
		{
			StaticMesh = nullptr;
			bSelectRandomMaterial = false;
			SecondRandomMaterial = nullptr;
			bSelectMaterialRandomly = false;
			SpawnCount = 5;
			ScaleRange = FVector2D::UnitVector;
			LinearDamping = 0.01;
			AngularDamping = 0.0f;
			Mass = 100.0f;
			OrientationType = EObjectOrientation::Keep;
		}
		else if(SlotIndexes.Num() >= 1)
		{
			if(DistObjectData->IsValidIndex(SlotIndexes[0]))
			{
				const auto& FoundDistData = (*DistObjectData)[SlotIndexes[0]];

				StaticMesh = FoundDistData.StaticMesh;
				bSelectRandomMaterial = FoundDistData.bSelectRandomMaterial;
				SecondRandomMaterial = FoundDistData.SecondRandomMaterial;
				bSelectMaterialRandomly = FoundDistData.bSelectMaterialRandomly;
				SpawnCount = FoundDistData.DistributionProperties.SpawnCount;
				ScaleRange = FoundDistData.DistributionProperties.ScaleRange;
				LinearDamping = FoundDistData.DistributionProperties.LinearDamping;
				AngularDamping = FoundDistData.DistributionProperties.AngularDamping;
				Mass = FoundDistData.DistributionProperties.Mass;
				OrientationType = FoundDistData.DistributionProperties.OrientationType;
			}
		}
	}
}

void UODPaletteDataObject::BeginDestroy()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->OnPresetLoaded.RemoveAll(this);
	}
	
	UObject::BeginDestroy();
}




