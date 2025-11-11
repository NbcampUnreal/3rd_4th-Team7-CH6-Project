// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Building/MBPropDistributionBase.h"
#include "Editor.h"
#include "MBToolSubsystem.h"
#include "Libraries/MBActorFunctions.h"
#include "Subsystems/EditorActorSubsystem.h"

void UMBPropDistributionBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	if(!ToolSettingsSubsystem){SetToolSettings();}
	
	if(DistributionActors.Num() == 0)
	{
		SetupDistribution();
	}

	CheckForNumberChanges();

	if(GetDistributionType() != DistributionType)
	{
		if(OnDistributionTypeChangedSignature.ExecuteIfBound(DistributionType))
		{
			return;
		}
	}

	ExecuteDistribution();
}

void UMBPropDistributionBase::SetupDistribution()
{
	if(!ToolSettingsSubsystem){SetToolSettings();}

	if(UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
	{
		auto SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
		UMBActorFunctions::FilterForCategory(EBuildingCategory::Prop,SelectedActors);
		EditorActorSubsystem->SetSelectedLevelActors(SelectedActors);
		DistributionActors.Append(SelectedActors);

		if(!SelectedActors.IsEmpty())
		{
			Location = SelectedActors[0]->GetActorLocation();
		}
	}
}

void UMBPropDistributionBase::ExecuteDistribution()
{
	if(!ToolSettingsSubsystem){SetToolSettings();}
	
	const float Num = DistributionActors.Num();
	if(Num == 0) {return;}
	
	FVector BaseLocation; 
	if(ActorToFollow)
	{
		BaseLocation = ActorToFollow->GetActorLocation();
	}
	else
	{
		BaseLocation = Location;
	}
	
	for(int32 Index = 0; Index < Num ; Index++)
	{
		FVector CalculatedVec = CalculateLocation(Index,Num);
		if(DistributionActors[Index])
		{
			CalculatedVec += BaseLocation;
			DistributionActors[Index]->SetActorLocation(CalculatedVec);
			DistributionActors[Index]->SetActorRotation(CalculateRotation(CalculatedVec));
		}
	}
}

void UMBPropDistributionBase::CheckForNumberChanges()
{
	if(!ToolSettingsSubsystem){SetToolSettings();}
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	const int32 CurrentNum = DistributionActors.Num();
	
	if(CurrentNum < NumberOfDistribution) //Create 
	{
		const int32 CreateNum = NumberOfDistribution - CurrentNum;

		TArray<AActor*> SpawnedActors;
		
		for(int32 Index = 0; Index < CreateNum ; Index++)
		{
			const auto ActorToDuplicate = DistributionActors[FMath::RandRange(0,CurrentNum-1)];
			auto DuplicatedActor = EditorActorSubsystem->DuplicateActor(ActorToDuplicate);
			SpawnedActors.Add(DuplicatedActor);
		}
		DistributionActors.Append(SpawnedActors);
	}
	else if(CurrentNum > NumberOfDistribution) //Delete
	{
		const int32 DeleteNum = CurrentNum - NumberOfDistribution;

		for(int32 Index = 0; Index < DeleteNum ; Index++)
		{
			const int32 RandomIndex = FMath::RandRange(0,DistributionActors.Num() -1);
			const auto ActorToDelete = DistributionActors[RandomIndex];
			DistributionActors.RemoveAt(RandomIndex);
			ActorToDelete->Destroy();
		}
	}
	EditorActorSubsystem->SetSelectedLevelActors(DistributionActors);	
}


FVector UMBPropDistributionBase::CalculateLocation(const int32& InIndex, const int32& InLength)
{
	return FVector::ZeroVector;
}

FRotator UMBPropDistributionBase::CalculateRotation(const FVector& InLocation) const
{
	FVector TargetLoc; 
	if(ActorToFollow)
	{
		TargetLoc = ActorToFollow->GetActorLocation();
	}
	else
	{
		TargetLoc = Location;
	}
	
	if(Orientation == EMBPropOrientation::Inside)
	{
		return FRotationMatrix::MakeFromX(TargetLoc - InLocation).Rotator();

	}
	if(Orientation == EMBPropOrientation::Outside)
	{
		return FRotationMatrix::MakeFromX(InLocation - TargetLoc).Rotator();

	}
	if(Orientation == EMBPropOrientation::RandomZ)
	{
		FRotator RandRot;
		RandRot.Yaw = FMath::FRand() * 360.f;
		RandRot.Pitch = 0.0f;
		RandRot.Roll = 0.0f;
		return RandRot;
	}
	if(Orientation == EMBPropOrientation::Random)
	{
		FRotator RandRot;
		RandRot.Yaw = FMath::FRand() * 360.f;
		RandRot.Pitch = FMath::FRand() * 360.f;
		RandRot.Roll = FMath::FRand() * 360.f;
		return RandRot;
	}
	return FRotator::ZeroRotator;
}

void UMBPropDistributionBase::SetDistributionBaseSettings(const FDistributionBaseData& InDistributionBaseData)
{
	if(!ToolSettingsSubsystem){SetToolSettings();}
	bActorSelected = InDistributionBaseData.bActorSelected;
	bOneActorDistribution = InDistributionBaseData.bOneActorDistribution;
	NumberOfDistribution = DistributionActors.Num();
	DistributionType = InDistributionBaseData.DistributionType;
	ActorToFollow = InDistributionBaseData.ActorToFollow;
	Location = InDistributionBaseData.Location;
	Orientation = InDistributionBaseData.Orientation;
}

FDistributionBaseData UMBPropDistributionBase::GetDistributionBaseSettings() const
{
	FDistributionBaseData LocalDistributionBaseData;
	LocalDistributionBaseData.bActorSelected = bActorSelected;
	LocalDistributionBaseData.bOneActorDistribution = bOneActorDistribution;
	LocalDistributionBaseData.DistributionType = DistributionType;
	LocalDistributionBaseData.ActorToFollow = ActorToFollow;
	LocalDistributionBaseData.Location = Location;
	LocalDistributionBaseData.Orientation = Orientation;
	return LocalDistributionBaseData;
}

void UMBPropDistributionBase::SetToolSettings()
{
	ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
}

