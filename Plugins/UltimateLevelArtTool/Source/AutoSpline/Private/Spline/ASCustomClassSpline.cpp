// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Spline/ASCustomClassSpline.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

#if WITH_EDITOR

#include "Editor.h"
#include "Subsystems/EditorActorSubsystem.h"

#endif

void AASCustomClassSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	RedesignTheSplineElements();
}

void AASCustomClassSpline::BeginPlay()
{
	Super::BeginPlay();

	CheckForSurfaceSnapping();
}

void AASCustomClassSpline::CreateSpline()
{
	TArray<TSubclassOf<AActor>> FilteredActorClasses = SplineActorClasses; 
	TArray<TSubclassOf<USceneComponent>> FilteredComponentClasses = SplineComponentClasses; 
	
	if(InstanceType == ESplineClassInstType::Actor)
	{
		if(FilteredActorClasses.Num() == 0){return;}
	}
	else
	{
		if(FilteredComponentClasses.Num() == 0){return;}
	}

	if(InstanceType == ESplineClassInstType::Actor)
	{
		//Remove Empty Actor Class Slots
		const int32 SMNum = FilteredActorClasses.Num();
		for(int32 Index = 0 ; Index < SMNum ; Index++)
		{
			if(!FilteredActorClasses[SMNum - Index - 1]){FilteredActorClasses.RemoveAt(SMNum - Index - 1);}
		}
		if(FilteredActorClasses.IsEmpty()){return;}
	}
	else
	{
		//Remove Empty Component Class Slots
		const int32 SMNum = FilteredComponentClasses.Num();
		for(int32 Index = 0 ; Index < SMNum ; Index++)
		{
			if(!FilteredComponentClasses[SMNum - Index - 1]){FilteredComponentClasses.RemoveAt(SMNum - Index - 1);}
		}
		if(FilteredComponentClasses.IsEmpty()){return;}
	}
	
	const int32 ObjectVariety = InstanceType == ESplineClassInstType::Actor ? FilteredActorClasses.Num() : FilteredComponentClasses.Num();

	
	int32 TotalIndex = 0;  
	int32 CurrentObjectIndex = bSelectRandom ? UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,ObjectVariety - 1) : 0;
	GetSplineComp()->SetSplinePointType(TotalIndex,ESplinePointType::Type::Curve,true);
	FTransform Transform;
	
	for(int32 Index = 0 ; Index < NumberOfInstance ; Index++)
	{
		//Spawn Here

		TObjectPtr<USceneComponent> CreatedObject;
		
		if(InstanceType == ESplineClassInstType::Actor)
		{
			CreatedObject = CreateChildActorComponent(SplineActorClasses[CurrentObjectIndex]);
		}
		else
		{
			CreatedObject = CreateComponentWithClass(SplineComponentClasses[CurrentObjectIndex]);
		}
		if(!CreatedObject){continue;}

		const float UnitLength = (GetSplineComp()->GetSplineLength())/(NumberOfInstance - 1);

		FVector LocationOnSpline = GetSplineComp()->GetLocationAtDistanceAlongSpline(Index * UnitLength,ESplineCoordinateSpace::Type::Local);

		if(bInnerSplineOffset)
		{
			FVector SplineTangent = GetSplineComp()->GetRightVectorAtDistanceAlongSpline(Index * UnitLength,ESplineCoordinateSpace::Type::Local);
			SplineTangent = SplineTangent.GetSafeNormal();
			SplineTangent *= UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,InnerSplineOffset.X,InnerSplineOffset.Y);
			SplineTangent *= FMath::RandBool() ? 1.0 : -1.0f;
			LocationOnSpline += SplineTangent;
		}

		FRotator ObjectRotation = FRotator::ZeroRotator;
		if(RotationType != ESplineObjectRotationType::Keep)
		{
			//Rotation Follows The Spline
			float ObjectLength = 0.0f;
			if(InstanceType == ESplineClassInstType::Actor)
			{
				if(const auto ChildActorComp = Cast<UChildActorComponent>(CreatedObject))
				{
					if(const auto ChildActor = ChildActorComp->GetChildActor())
					{
						ObjectLength = GetActorLength(ChildActor);
					}
				}
			}
			else
			{
				ObjectLength = GetComponentLength(CreatedObject);
			}
			
			FVector DirToNextMesh = GetSplineComp()->GetLocationAtDistanceAlongSpline((Index * UnitLength) + ObjectLength,ESplineCoordinateSpace::Type::Local) - LocationOnSpline;
			ObjectRotation = FRotationMatrix::MakeFromX(DirToNextMesh).Rotator();

			if(bRandomZ)
			{
				ObjectRotation.Yaw += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationRate * -1,RandomRotationRate);	
			}
			
			if(RotationType == ESplineObjectRotationType::FollowSplineAndSurface)
			{
				//Rotation Follows The Spline & Surface
				FVector SurfaceNormal = GetSurfaceNormalAtLocation(GetActorTransform().TransformPosition(LocationOnSpline));
				FVector ObjectWorldForward  = GetActorTransform().TransformRotation(ObjectRotation.Quaternion()).Rotator().Vector();
				FRotator MakeRot = FRotationMatrix::MakeFromZX(SurfaceNormal,ObjectWorldForward).Rotator();
				ObjectRotation = GetActorTransform().InverseTransformRotation(MakeRot.Quaternion()).Rotator();
			}
		}

		ObjectRotation =  FRotator(FQuat(ObjectRotation)*FQuat(InitialRotationOffset));

		
		if(bRandomPositionOffset)
		{
			AddRandomPositionOffset(LocationOnSpline);
		}

		if(bRandomRotationOffset)
		{
			AddRandomRotationOffset(ObjectRotation);
		}
		
		Transform.SetRotation(ObjectRotation.Quaternion());

		Transform.SetLocation(LocationOnSpline);
		
		CreatedObject->SetRelativeTransform(Transform);
		DynamicSplineObjects.Add(CreatedObject);
		
		//Select Next Object
		if(bSelectRandom)
		{
			CurrentObjectIndex = UKismetMathLibrary::RandomIntegerInRangeFromStream(RandomStream,0,ObjectVariety - 1);
		}
		else
		{
			if(++CurrentObjectIndex == ObjectVariety)
			{
				CurrentObjectIndex = 0;
			}
		}
		++TotalIndex;
	}
}

UChildActorComponent* AASCustomClassSpline::CreateChildActorComponent(TSubclassOf<AActor> InClass)
{
	UChildActorComponent* CreatedChildActorComponent = NewObject<UChildActorComponent>(this);
	CreatedChildActorComponent->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
	CreatedChildActorComponent->bAutoRegister = true;
	CreatedChildActorComponent->SetupAttachment(RootComponent);
	CreatedChildActorComponent->SetNetAddressable();
	CreatedChildActorComponent->CreateChildActor();
	CreatedChildActorComponent->SetChildActorClass(InClass);
	CreatedChildActorComponent->SetMobility(EComponentMobility::Movable);
	FinishAndRegisterComponent(CreatedChildActorComponent);
	return CreatedChildActorComponent;
}

USceneComponent* AASCustomClassSpline::CreateComponentWithClass(TSubclassOf<USceneComponent> InClass)
{
	USceneComponent* CreatedChildActorComponent = NewObject<USceneComponent>(this,InClass);
	CreatedChildActorComponent->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
	CreatedChildActorComponent->bAutoRegister = true;
	CreatedChildActorComponent->SetupAttachment(RootComponent);
	CreatedChildActorComponent->SetNetAddressable();
	CreatedChildActorComponent->SetMobility(EComponentMobility::Movable);
	FinishAndRegisterComponent(CreatedChildActorComponent);
	return CreatedChildActorComponent;
	
}

void AASCustomClassSpline::ResetFormerSpline()
{
	if(!DynamicSplineObjects.IsEmpty())
	{
		const auto Num = DynamicSplineObjects.Num();

		for(int32 Index = 0 ; Index < Num ; Index++)
		{
			if(DynamicSplineObjects[Num - Index - 1])
			{
				DynamicSplineObjects[Num - Index - 1]->DestroyComponent();
			}
		}
		DynamicSplineObjects.Empty();
	}

	//ClassSelectionIndexes.Empty();
}

void AASCustomClassSpline::RedesignTheSplineElements()
{
#if WITH_EDITOR
	
	if(bCopyASpline)
	{
		if(CopyASpline)
		{
			if(const auto AutoSplineActor = Cast<AASAutoSplineBase>(CopyASpline))
			{
				AutoSplineActor->CopyTargetSpline(GetSplineComp());
				
				SetActorRotation(CopyASpline->GetActorRotation());
			}
			CopyASpline = nullptr;
		}
	}

#endif
	
	ResetFormerSpline();
	
#if WITH_EDITOR
	
	CheckForClosedStatus();

#endif
	
	
	
	UKismetMathLibrary::SetRandomStreamSeed(RandomStream,Seed);

	CreateSpline();

	CheckForSurfaceSnapping();
	
}

void AASCustomClassSpline::CheckForClosedStatus()
{
	if(!bAutoCloseTheSpline)
	{
		GetSplineComp()->SetClosedLoop(false,true);
	}
	else
	{
		const float  Result = FVector::DistSquared(GetSplineComp()->GetLocationAtDistanceAlongSpline(0,ESplineCoordinateSpace::Local),GetSplineComp()->GetLocationAtDistanceAlongSpline(GetSplineComp()->GetSplineLength(),ESplineCoordinateSpace::Local));

		if(Result < AutoCloseTheSpline * AutoCloseTheSpline)
		{
			GetSplineComp()->SetClosedLoop(true);
		}
		else
		{
			GetSplineComp()->SetClosedLoop(false);
		}
	}
	
}



void AASCustomClassSpline::AddRandomPositionOffset(FVector& InVector) const
{
	InVector += FVector(
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.X * -1,RandomPositionOffset.X),	
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.Y * -1,RandomPositionOffset.Y),	
UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomPositionOffset.Z * -1,RandomPositionOffset.Z));
}

void AASCustomClassSpline::AddRandomRotationOffset(FRotator& InRotator) const
{
	InRotator.Pitch += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Pitch * -1,RandomRotationOffset.Pitch);
	InRotator.Yaw += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Yaw * -1,RandomRotationOffset.Yaw);
	InRotator.Roll += UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream,RandomRotationOffset.Roll * -1,RandomRotationOffset.Roll);
}

void AASCustomClassSpline::CheckForSurfaceSnapping() const
{
		if(SnapToSurface)
		{
			UWorld* LocalWorld = GetWorld();
			if(!LocalWorld){return;}
			
			static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
			FCollisionQueryParams collisionParams(lineTraceSingleName);
			collisionParams.bTraceComplex = false;
			collisionParams.bReturnPhysicalMaterial = false;
			FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
			objectParams.AddObjectTypesToQuery(ECC_Visibility);
			objectParams.AddObjectTypesToQuery(ECC_WorldStatic);
			FHitResult HitResult;
    
			TArray<UActorComponent*> FoundMesComps;
			GetComponents(USceneComponent::StaticClass(),FoundMesComps);
			if(FoundMesComps.IsEmpty()){return;}
    
			for(const auto CurrentSMComp : FoundMesComps)
			{
				if( CurrentSMComp == GetRootComponent() || CurrentSMComp == GetSplineComp()){continue;}
    				
				if(USceneComponent* LocalSmComp = Cast<USceneComponent>(CurrentSMComp))
				{
					FVector CompLoc = LocalSmComp->GetComponentLocation();

					LocalWorld->LineTraceSingleByObjectType(HitResult, CompLoc, CompLoc + FVector(0,0,-15000.0f), objectParams, collisionParams);
					if(HitResult.bBlockingHit)
					{
						LocalSmComp->SetWorldLocation(HitResult.Location);
					}
    					
					if(RotationType == ESplineObjectRotationType::FollowSplineAndSurface)
					{
						//Rotation Follows The Spline & Surface
						FVector ObjectWorldForward  = GetActorTransform().TransformRotation(LocalSmComp->GetComponentRotation().Quaternion()).Rotator().Vector();
						FRotator MakeRot = FRotationMatrix::MakeFromZX(HitResult.Normal,ObjectWorldForward).Rotator();
						FRotator ObjectRotation = GetActorTransform().InverseTransformRotation(MakeRot.Quaternion()).Rotator();
						LocalSmComp->SetWorldRotation(ObjectRotation);
					}
				}
			}
		}
}


#if WITH_EDITOR

FReply AASCustomClassSpline::OnIsolatePressed()
{

	CreateActorsFromSpline();
	
	return FReply::Handled();
}

TObjectPtr<AActor> AASCustomClassSpline::SpawnAndGetAnActor(const FTransform& InTransform, const FString& InActorLabel,UWorld* InWorld)
{
	//Create Actor
	const auto SpawnedActor = InWorld->SpawnActor<AActor>(AActor::StaticClass(),InTransform);
	if(!SpawnedActor){return nullptr;}

	//Crate Root Component
	USceneComponent* CreatedRootComponent = NewObject<USceneComponent>(SpawnedActor,FName(TEXT("RootComponent")));
	if(!CreatedRootComponent){return SpawnedActor;}
	
	CreatedRootComponent->bAutoRegister = true;
	SpawnedActor->SetRootComponent(CreatedRootComponent);
	SpawnedActor->AddInstanceComponent(CreatedRootComponent);
	SpawnedActor->AddOwnedComponent(CreatedRootComponent);
	CreatedRootComponent->SetFlags(RF_Transactional);
	SpawnedActor->SetActorTransform(InTransform);
	SpawnedActor->RegisterAllComponents();
	
	return SpawnedActor;
}

void AASCustomClassSpline::CreateActorsFromSpline(bool bInSelectAfter)
{
	if(DynamicSplineObjects.IsEmpty()){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	if(InstanceType == ESplineClassInstType::Actor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.bNoFail = true;


		TArray<TObjectPtr<AActor>> CreatedActors;
		
		for(auto LocalComp : DynamicSplineObjects)
		{
			if(const auto LocalChildComp = Cast<UChildActorComponent>(LocalComp))
			{
				const auto CreatedActor = EditorWorld->SpawnActor<AActor>(LocalChildComp->GetChildActorClass(),LocalComp->GetComponentTransform(),SpawnParams);

				const FName NewActorName = MakeUniqueObjectName(this, LocalChildComp->GetChildActorClass(), FName(*LocalChildComp->GetChildActorClass()->GetName()));
				CreatedActor->SetActorLabel(NewActorName.ToString());
				CreatedActors.Add(CreatedActor);
			}
		}

		if(bInSelectAfter)
		{
			if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
			{
				EditorActorSubsystem->SelectNothing();
				EditorActorSubsystem->SetSelectedLevelActors(CreatedActors);
			}
		}
	}
	else
	{
		const auto SpawnedActor = SpawnAndGetAnActor(DynamicSplineObjects[0]->GetComponentTransform(),FString(TEXT("IsolatedCollectionActor")),EditorWorld);

		for(const auto LocalComp : DynamicSplineObjects)
		{
			const auto CreatedComp = NewObject<USceneComponent>(SpawnedActor,LocalComp->GetClass());
			if(!CreatedComp){continue;}
			
			CreatedComp->SetupAttachment(SpawnedActor->GetRootComponent());
			SpawnedActor->AddInstanceComponent(CreatedComp);
			SpawnedActor->AddOwnedComponent(CreatedComp);
			CreatedComp->RegisterComponent();
			CreatedComp->SetWorldTransform(LocalComp->GetComponentTransform());
			CreatedComp->SetMobility(EComponentMobility::Movable);
		}

		if(bInSelectAfter)
		{
			if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
			{
				EditorActorSubsystem->SelectNothing();
				EditorActorSubsystem->SetActorSelectionState(SpawnedActor,true);
			}
		}
	}
}

#endif



