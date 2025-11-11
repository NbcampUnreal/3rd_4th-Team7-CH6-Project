// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "MBMActionSettingsObj.h"
#include "Editor.h"
#include "MBActorFunctions.h"
#include "MBExtendedMath.h"
#include "MBModularEnum.h"
#include "MBToolData.h"
#include "MBToolSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/BodySetup.h"
#include "Subsystems/EditorActorSubsystem.h"


UMBMActionSettingsObj::UMBMActionSettingsObj(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DuplicationAxis = EMBAxis::AxisZ;
	Offset = 0.0f;
	NumberOfDuplications = 1;
	bPositiveDirection = true;
}

void UMBMActionSettingsObj::Duplicate()
{
	const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if(!EditorActorSubsystem || !ToolSettingsSubsystem){return;}
	
	const TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	if(SelectedActors.IsEmpty()){return;}

	for(const auto SelectedActor : SelectedActors)
	{
		if(!IsValid(SelectedActor)){continue;}
		
		const float SelectedWorldExtent = GetMMHalfLengthOfAxis(SelectedActor,DuplicationAxis); //Pure World Extent
		FVector DirVec = UMBExtendedMath::GetAxisVectorWithEnumValue(DuplicationAxis);

		for(int32 Index = 0; Index < NumberOfDuplications ; ++Index)
		{
			const FVector TotalLength = (DirVec * SelectedWorldExtent * 2 + DirVec * Offset) * (Index + 1) * (bPositiveDirection ? 1.0f : -1.0f);
			EditorActorSubsystem->DuplicateActor(SelectedActor,GEditor->GetEditorWorldContext().World(),TotalLength);
		}
	}
}

float UMBMActionSettingsObj::GetMMHalfLengthOfAxis(const AActor* InActor,const EMBAxis& InAxis) const
{
	if(InActor && UMBActorFunctions::IsActorFromTool(InActor))
	{
		if(const auto MMActor = Cast<AStaticMeshActor>(InActor))
		{
			if(!GEditor){return float();}
			const auto ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
			if(!ToolSettingsSubsystem){return 0.0f;}

			float Extent;
	
			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::SMCollision)
			{
				const FBox CollisionBox = MMActor->GetStaticMeshComponent()->GetStaticMesh()->GetBodySetup()->AggGeom.CalcAABB(FTransform::Identity);
				Extent =  FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetActorRotation().RotateVector(CollisionBox.GetExtent())));
			}
			else
			{
				Extent =  FMath::Abs(UMBExtendedMath::GetAxisOfVector(InAxis,MMActor->GetStaticMeshComponent()->Bounds.BoxExtent)); // Extent
			}
			
			if(ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.SnappingType == EMBModularSnapType::BoundCorrection)
			{
				Extent = FMath::GridSnap(Extent,ToolSettingsSubsystem->GetToolData()->BuildingSettingsData.BoundCorrectionSensitivity / 2.0f);	
			}

			return Extent;
		}
	}
	return 0;
}
