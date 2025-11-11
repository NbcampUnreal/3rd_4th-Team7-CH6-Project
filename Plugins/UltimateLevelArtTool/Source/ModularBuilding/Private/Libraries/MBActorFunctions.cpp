// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "Libraries/MBActorFunctions.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Data/MBModularAssetData.h"
#include "MBToolSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"

void UMBActorFunctions::FilterForCategory(const EBuildingCategory InCategory,TArray<AActor*>& InActors)
{
	const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(InActors.IsEmpty() || !ToolSettings){return;}

	TArray<AActor*> TempActors;
	for(AActor* Local : InActors)
	{
		if(IsValid(Local) &&IsActorFromTool(Local) && Local->IsA(AStaticMeshActor::StaticClass()))
		{
			auto Name = Cast<AStaticMeshActor>(Local)->GetStaticMeshComponent()->GetStaticMesh()->GetName();
			if(const auto ToolData = ToolSettings->GetModularAssetDataByName(Name))
			{
				if(ToolData->MeshCategory == InCategory)
				{
					TempActors.Add(Local);
				}
			}
		}
	}
	InActors.Empty();
	InActors.Append(TempActors);
	InActors = TempActors;
}

TArray<AActor*> UMBActorFunctions::GetAllActorsUnderTheFolderPath(const FName& InFolderPath)
{
	TArray<AActor*> CollectionActors;
	if(const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;

			if(Actor->GetFolderPath().IsEqual(InFolderPath))
			{
				CollectionActors.Add(Actor);
			}
		}
	}
	return {CollectionActors};
}

FBox UMBActorFunctions::GetAllActorsTransportPoint(const TArray<AActor*>& InActors)
{
	if(InActors.IsEmpty()){return FBox();}

	FVector PositiveExtent = FVector();
	FVector NegativeExtent = FVector();

	int32 Index = 0;
	for(const auto Actor : InActors)
	{
		if(Index == 0)
		{
			PositiveExtent = Actor->GetActorLocation();
			NegativeExtent = Actor->GetActorLocation();
			++Index;
			continue;	
		}
		
		if(!Actor){continue;}
		const FVector ActorLocation = Actor->GetActorLocation();
		if(ActorLocation.X > PositiveExtent.X)
		{
			PositiveExtent.X = ActorLocation.X;
		}
		if(ActorLocation.Y > PositiveExtent.Y)
		{
			PositiveExtent.Y = ActorLocation.Y;
		}
		if(ActorLocation.Z > PositiveExtent.Z)
		{
			PositiveExtent.Z = ActorLocation.Z;
		}
		if(ActorLocation.X < NegativeExtent.X)
		{
			NegativeExtent.X = ActorLocation.X;
		}
		if(ActorLocation.Y < NegativeExtent.Y)
		{
			NegativeExtent.Y = ActorLocation.Y;
		}
		if(ActorLocation.Z < NegativeExtent.Z)
		{
			NegativeExtent.Z = ActorLocation.Z;
		}
		
		++Index;
	}
	return FBox(NegativeExtent,PositiveExtent);
}

FBox UMBActorFunctions::GetAllActorsTransportPointWithExtents(const TArray<AActor*>& InActors)
{
	if(InActors.IsEmpty()){return FBox();}

	FVector PositiveExtent = FVector();
	FVector NegativeExtent = FVector();
	
	int32 Index = 0;
	for(const auto Actor : InActors)
	{
		if(!IsValid(Actor)){continue;}
		
		FVector ActorOrigin;
		FVector ActorExtent;
		Actor->GetActorBounds(false,ActorOrigin,ActorExtent,false);
		const FVector ActorPositiveExtent = ActorOrigin + ActorExtent;
		const FVector ActorNegativeExtent = ActorOrigin - ActorExtent;
		
		if(Index == 0)
		{
			PositiveExtent = ActorPositiveExtent;
			NegativeExtent = ActorNegativeExtent;
			++Index;
			continue;	
		}
		
		if(ActorPositiveExtent.X > PositiveExtent.X)
		{
			PositiveExtent.X = ActorPositiveExtent.X;
		}
		if(ActorPositiveExtent.Y > PositiveExtent.Y)
		{
			PositiveExtent.Y = ActorPositiveExtent.Y;
		}
		if(ActorPositiveExtent.Z > PositiveExtent.Z)
		{
			PositiveExtent.Z = ActorPositiveExtent.Z;
		}
		if(ActorNegativeExtent.X < NegativeExtent.X)
		{
			NegativeExtent.X = ActorNegativeExtent.X;
		}
		if(ActorNegativeExtent.Y < NegativeExtent.Y)
		{
			NegativeExtent.Y = ActorNegativeExtent.Y;
		}
		if(ActorNegativeExtent.Z < NegativeExtent.Z)
		{
			NegativeExtent.Z = ActorNegativeExtent.Z;
		}
		++Index;
	}
	return FBox(NegativeExtent,PositiveExtent);
}

void UMBActorFunctions::FilterModularActors(TArray<AActor*>& InActors)
{
	if(InActors.IsEmpty()){return;}
	
	TArray<AActor*> ActorsToRemove;

	for(const auto LocalActor : InActors)
	{
		if(IsActorFromTool(LocalActor))
		{
			continue;
		}
		ActorsToRemove.Add(LocalActor);
	}
	
	if(ActorsToRemove.IsEmpty()){return;}
	
	for(auto Actor : ActorsToRemove)
	{
		InActors.Remove(Actor);
	}
}

TArray<AActor*> UMBActorFunctions::GetAllModularActorsInWorld()
{
	TArray<AActor*> ModularActors;
	if(const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
		{
			AActor* LocalActor = *ActorItr;

			if(IsActorFromTool(LocalActor))
			{
				ModularActors.Add(LocalActor);
			}
		}
	}
	return ModularActors;
}

bool UMBActorFunctions::IsActorFromTool(const AActor* InActor)
{
	if(IsValid(InActor))
	{
		return InActor->ActorHasTag(MODULAR_ACTOR_TAG);
	}
	return false;
}

AActor* UMBActorFunctions::GetActorWithObjectName(const FString& InName)
{
	if(const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
		{
			AActor* LocalActor = *ActorItr;

			if(IsValid(LocalActor) && LocalActor->GetName().Equals(InName))
			{
				return LocalActor;
			}
		}
	}
	return nullptr;
}

TArray<AActor*> UMBActorFunctions::GetAllActorsWithGivenObjectNames(const TArray<FString>& InNames)
{
	TArray<AActor*> FoundActors;
	if(const UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> ActorItr(EditorWorld); ActorItr; ++ActorItr)
		{
			AActor* LocalActor = *ActorItr;

			if(InNames.Contains(LocalActor->GetName()))
			{
				FoundActors.Add(LocalActor);

				if(FoundActors.Num() == InNames.Num())
				{
					return FoundActors;
				}
			}
		}
	}
	return FoundActors;
}

EBuildingCategory UMBActorFunctions::GetBuildingCategory(const AActor* InActor)
{
	if(IsActorFromTool(InActor))
	{
		if(InActor->ActorHasTag(MODULAR_TAG))
		{
			return EBuildingCategory::Modular;
		}
		if(InActor->ActorHasTag(PROP_TAG))
		{
			return EBuildingCategory::Prop;
		}
	}
	return EBuildingCategory::None;
}
