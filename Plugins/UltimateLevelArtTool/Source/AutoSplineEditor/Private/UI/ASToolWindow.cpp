// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "ASToolWindow.h"
#include "ASSplineSlot.h"
#include "ASToolAssetData.h"
#include "EngineUtils.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "ASCustomClassSpline.h"
#include "ASDeformMeshSpline.h"
#include "ASStaticMeshDistributionSpline.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Framework/Docking/TabManager.h"
#include "LevelEditorViewport.h"
#include "Editor.h"


void UASToolWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(SplineContentBox){SplineContentBox->SetVisibility(ESlateVisibility::Hidden);}
	
	if (!DeformableBtn->OnClicked.IsBound())
	{
		DeformableBtn->OnClicked.AddDynamic(this, &UASToolWindow::DeformableBtnPressed);
	}

	if (!MeshDistributionBtn->OnClicked.IsBound())
	{
		MeshDistributionBtn->OnClicked.AddDynamic(this, &UASToolWindow::MeshDistributionBtnPressed);
	}

	if (!ClassDistributionBtn->OnClicked.IsBound())
	{
		ClassDistributionBtn->OnClicked.AddDynamic(this, &UASToolWindow::ClassDistributionBtnPressed);
	}

	if (!ViewDeformableBtn->OnClicked.IsBound())
	{
		ViewDeformableBtn->OnClicked.AddDynamic(this, &UASToolWindow::ViewDeformableBtnPressed);
	}

	if (!ViewMeshDistBtn->OnClicked.IsBound())
	{
		ViewMeshDistBtn->OnClicked.AddDynamic(this, &UASToolWindow::ViewMeshDistBtnPressed);
	}

	if (!ViewClassDistBtn->OnClicked.IsBound())
	{
		ViewClassDistBtn->OnClicked.AddDynamic(this, &UASToolWindow::ViewClassDistBtnPressed);
	}

	if (!SelectBtn->OnClicked.IsBound())
	{
		SelectBtn->OnClicked.AddDynamic(this, &UASToolWindow::SelectBtnPressed);
	}

	if (!CreateBtn->OnClicked.IsBound())
	{
		CreateBtn->OnClicked.AddDynamic(this, &UASToolWindow::CreateBtnPressed);
	}
	
	if (!SelectAllCheckbox->OnCheckStateChanged.IsBound())
	{
		SelectAllCheckbox->OnCheckStateChanged.AddDynamic(this, &UASToolWindow::SelectAllCheckboxChanged);
	}
	

	DeformableBtn->SetToolTipText(FText::FromName(TEXT("Creates an actor based on a spline mesh for deformable static meshes.")));
	MeshDistributionBtn->SetToolTipText(FText::FromName(TEXT("Creates a customizable spline actor that can distribute multiple static meshes on a spline.")));
	ClassDistributionBtn->SetToolTipText(FText::FromName(TEXT("Creates a customizable spline actor that can distribute selected actors or components over a spline in various ways.")));


	ViewDeformableBtn->SetToolTipText(FText::FromName(TEXT("Lists deformable mesh spline actors.")));
	ViewMeshDistBtn->SetToolTipText(FText::FromName(TEXT("Lists mesh distribution spline actors.")));
	ViewClassDistBtn->SetToolTipText(FText::FromName(TEXT("Lists class distribution spline actors.")));
	
	SelectBtn->SetToolTipText(FText::FromName(TEXT("Selects checked spline actors.")));
	CreateBtn->SetToolTipText(FText::FromName(TEXT("Isolates selected actors from the spline and creates new instances for mesh and class distribution spline actors.\n For deformable mesh spline actors, it runs the merge tool.")));
	
	if(GEditor)
	{
		GEditor->OnLevelActorDeleted().AddUObject(this, &UASToolWindow::HandleOnLevelActorDeletedNative);
	}
	
}



void UASToolWindow::DeformableBtnPressed()
{
	if(const auto SpawnedSplineActor = SpawnSplineActor(AASDeformMeshSpline::StaticClass()))
	{
		AddNewCreatedItemToTheSplineList(EAutoSplineEditorType::Deformable,SpawnedSplineActor);
	}
}

void UASToolWindow::MeshDistributionBtnPressed()
{
	if(const auto SpawnedSplineActor = SpawnSplineActor(AASStaticMeshDistributionSpline::StaticClass()))
	{
		AddNewCreatedItemToTheSplineList(EAutoSplineEditorType::SMDistribution,SpawnedSplineActor);
	}
}

void UASToolWindow::ClassDistributionBtnPressed()
{
	if(const auto SpawnedSplineActor = SpawnSplineActor(AASCustomClassSpline::StaticClass()))
	{
		AddNewCreatedItemToTheSplineList(EAutoSplineEditorType::CCDistribution,SpawnedSplineActor);
	}
}

void UASToolWindow::ViewDeformableBtnPressed()
{
	if(ActiveViewListType == EAutoSplineEditorType::Deformable)
	{
		EjectSplineContentBox();
		return;
	}
	else if(ActiveViewListType != EAutoSplineEditorType::None)
	{
		SelectAllCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
		if(ViewList){ViewList->ClearChildren();}
		SplineSlots.Empty();
	}
	
	if (!GEditor){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(EditorWorld, AASDeformMeshSpline::StaticClass(), FoundActors);

	if(FoundActors.IsEmpty()){return;}

	for(const auto CurrentActor : FoundActors)
	{
		CreateSplineSlotAndAddActorToTheList(EditorWorld,CurrentActor);
	}

	ActiveViewListType = EAutoSplineEditorType::Deformable;
	SplineContentBox->SetVisibility(ESlateVisibility::Visible);
	CheckForViewListButtonsAvailability();
}

void UASToolWindow::ViewMeshDistBtnPressed()
{
	if(ActiveViewListType == EAutoSplineEditorType::SMDistribution)
	{
		EjectSplineContentBox();
		return;
	}
	else if(ActiveViewListType != EAutoSplineEditorType::None)
	{
		SelectAllCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
		if(ViewList){ViewList->ClearChildren();}
		SplineSlots.Empty();
	}
	
	if (!GEditor){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(EditorWorld, AASStaticMeshDistributionSpline::StaticClass(), FoundActors);

	if(FoundActors.IsEmpty()){return;}

	for(const auto CurrentActor : FoundActors)
	{
		CreateSplineSlotAndAddActorToTheList(EditorWorld,CurrentActor);
	}
	
	
	ActiveViewListType = EAutoSplineEditorType::SMDistribution;
	SplineContentBox->SetVisibility(ESlateVisibility::Visible);
	
	CheckForViewListButtonsAvailability();
}

void UASToolWindow::ViewClassDistBtnPressed()
{
	if(ActiveViewListType == EAutoSplineEditorType::CCDistribution)
	{
		EjectSplineContentBox();
		return;
	}
	
	 if(ActiveViewListType != EAutoSplineEditorType::None)
	{
		SelectAllCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
		if(ViewList){ViewList->ClearChildren();}
		SplineSlots.Empty();
	}
	
	if (!GEditor){return;}
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(EditorWorld, AASCustomClassSpline::StaticClass(), FoundActors);

	if(FoundActors.IsEmpty()){return;}

	for(const auto CurrentActor : FoundActors)
	{
		CreateSplineSlotAndAddActorToTheList(EditorWorld,CurrentActor);
	}

	ActiveViewListType = EAutoSplineEditorType::CCDistribution;
	SplineContentBox->SetVisibility(ESlateVisibility::Visible);
	
	CheckForViewListButtonsAvailability();
}

void UASToolWindow::CreateSplineSlotAndAddActorToTheList(UWorld* InWorld, const AActor* InSplineActor)
{
	const TSoftClassPtr<UUserWidget> WidgetClassPtr(ASToolAssetData::ASSplineSlotPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto CreatedSplineSlotWidget = Cast<UASSplineSlot>(CreateWidget(InWorld,ClassRef)))
		{
			if(CreatedSplineSlotWidget)
			{
				CreatedSplineSlotWidget->SetSplineData(InSplineActor->GetActorLabel(),InSplineActor->GetName());
				CreatedSplineSlotWidget->OnSplineSlotCheckboxStateChanged.BindUObject(this,&UASToolWindow::OnSplineSlotCheckboxStateChanged);
				SplineSlots.Add(CreatedSplineSlotWidget);
				ViewList->AddChild(CreatedSplineSlotWidget);
			}
		}
	}
	
}

TArray<AActor*> UASToolWindow::GetAllActorsWithGivenObjectNames(const TArray<FString>& InNames)
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

void UASToolWindow::SelectBtnPressed()
{
	const TArray<AActor*> FoundActors = GetCheckedSplineActors();

	if(FoundActors.IsEmpty()) {return;}
	
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();
	EditorActorSubsystem->SetSelectedLevelActors(FoundActors);
}

void UASToolWindow::CreateBtnPressed()
{
	const auto CheckedActors =  GetCheckedSplineActors();
	if(CheckedActors.IsEmpty()){return;}

	if(ActiveViewListType == EAutoSplineEditorType::CCDistribution && ActiveViewListType == EAutoSplineEditorType::SMDistribution)
	{
		for(const auto CurrentActor : CheckedActors)
		{
			if(const auto CurrentSplineActor = Cast<AASAutoSplineBase>(CurrentActor))
			{
				CurrentSplineActor->CreateActorsFromSpline();
			}
		}
	}
	else if(ActiveViewListType == EAutoSplineEditorType::Deformable)
	{
		if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
		{
			EditorActorSubsystem->SelectNothing();
			EditorActorSubsystem->SetSelectedLevelActors(CheckedActors);
			FGlobalTabmanager::Get()->TryInvokeTab(FName("MergeActors"));
		}
	}
}

void UASToolWindow::SelectAllCheckboxChanged(bool InNewState)
{
	if(SplineSlots.IsEmpty()){return;}

	for(const auto CurrentSlot : SplineSlots)
	{
		CurrentSlot->SetSelectionState(InNewState);
	}
	CheckForViewListButtonsAvailability();
}

void UASToolWindow::HandleOnLevelActorDeletedNative(AActor* InActor)
{
	if(SplineSlots.IsEmpty()){return;}
	
	if(!InActor->IsA(AASAutoSplineBase::StaticClass())){return;}
	const auto ObjectName = InActor->GetName();

	TObjectPtr<UASSplineSlot> MatchingSlot; 
	
	for(const auto CurrentSlot : SplineSlots)
	{
		if(CurrentSlot->GetSplineObjectName().Equals(ObjectName))
		{
			MatchingSlot = CurrentSlot;
			ViewList->RemoveChild(CurrentSlot);
			break;
		}
	}
	
	if(MatchingSlot){return;}

	SplineSlots.Remove(MatchingSlot);

	if(SplineSlots.IsEmpty())
	{
		EjectSplineContentBox();
	}
	else
	{
		CheckForViewListButtonsAvailability();
	}
}

void UASToolWindow::EjectSplineContentBox()
{
	if(ViewList){ViewList->ClearChildren();}
	SplineSlots.Empty();

	ActiveViewListType = EAutoSplineEditorType::None;

	SelectAllCheckbox->SetCheckedState(ECheckBoxState::Unchecked);

	if(SplineContentBox){SplineContentBox->SetVisibility(ESlateVisibility::Hidden);}
}


void UASToolWindow::CheckForViewListButtonsAvailability()
{
	bool bEnableState = false;
	bool bCheckAllBoxState = true;
		
	for(const auto CurrentSlot : SplineSlots)
	{
		//If at least one of them checked then enables buttons
		if(CurrentSlot->GetSelectionState())
		{
			bEnableState = true;
		}
		else //If at least one of them unchecked then uncheck CheckAllBox
		{
			bCheckAllBoxState = false;	
		}
		
	}

	if(bEnableState)
	{
		SelectBtn->SetIsEnabled(true);
		CreateBtn->SetIsEnabled(true);
	}
	else
	{
		SelectBtn->SetIsEnabled(false);
		CreateBtn->SetIsEnabled(false);
	}

	SelectAllCheckbox->SetCheckedState(bCheckAllBoxState ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

AActor* UASToolWindow::SpawnSplineActor(TSubclassOf<AASAutoSplineBase> InSplineClass) const
{
	
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return nullptr;}

	const FVector SpawnLocation = FindSpawnLocationForSpline();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	if(const auto CreatedSplineActor = Cast<AASAutoSplineBase>(EditorWorld->SpawnActor<AActor>(InSplineClass,SpawnLocation,FRotator::ZeroRotator,SpawnParams)))
	{
		CreatedSplineActor->AdjustSplineLabel();
		if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
		{
			EditorActorSubsystem->SelectNothing();
			EditorActorSubsystem->SetActorSelectionState(CreatedSplineActor,true);
		}
		return CreatedSplineActor;
	}
	
	return nullptr;
}


TArray<AActor*> UASToolWindow::GetCheckedSplineActors()
{
	TArray<FString> FoundObjectNames;
	for(const auto CurrentSlot : SplineSlots)
	{
		if(CurrentSlot->GetSelectionState())
		{
			FoundObjectNames.Add(CurrentSlot->GetSplineObjectName());
		}
	}
	if(FoundObjectNames.IsEmpty())
	{
		return TArray<AActor*>();
	}
	
	return GetAllActorsWithGivenObjectNames(FoundObjectNames);
}

void UASToolWindow::AddNewCreatedItemToTheSplineList(const EAutoSplineEditorType InCreatedType,const AActor* InSplineActor)
{

	if(InCreatedType == ActiveViewListType)
	{
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
		if(!EditorWorld){return;}
		CreateSplineSlotAndAddActorToTheList(EditorWorld,InSplineActor);
	}
}

FVector UASToolWindow::FindSpawnLocationForSpline()
{
	const FVector StartLoc = GCurrentLevelEditingViewportClient->GetViewLocation();
	FVector EndLoc = StartLoc + GCurrentLevelEditingViewportClient->GetViewRotation().Vector() * 1500.0f;

	auto EditorWorld = GEditor->GetEditorWorldContext().World();

	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams collisionParams(lineTraceSingleName);
	collisionParams.bTraceComplex = false;
	collisionParams.bReturnPhysicalMaterial = false;
	FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	objectParams.AddObjectTypesToQuery(ECC_Visibility);
	objectParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult HitResult;
	
	EditorWorld->LineTraceSingleByObjectType(HitResult, StartLoc, EndLoc, objectParams, collisionParams);
	if(HitResult.bBlockingHit)
	{
		return HitResult.Location;
	}
	
	EditorWorld->LineTraceSingleByObjectType(HitResult, EndLoc, EndLoc + FVector(0,0,-3000.0f), objectParams, collisionParams);
	if(HitResult.bBlockingHit)
	{
		return HitResult.Location;
	}
	return EndLoc;
}

void UASToolWindow::OnSplineSlotCheckboxStateChanged(bool InState)
{
	CheckForViewListButtonsAvailability();
}

void UASToolWindow::NativeDestruct()
{
	
	if (DeformableBtn) { DeformableBtn->OnClicked.RemoveAll(this); }
	if (MeshDistributionBtn) { MeshDistributionBtn->OnClicked.RemoveAll(this); }
	if (ClassDistributionBtn) { ClassDistributionBtn->OnClicked.RemoveAll(this); }
	if (ViewDeformableBtn) { ViewDeformableBtn->OnClicked.RemoveAll(this); }
	if (ViewMeshDistBtn) { ViewMeshDistBtn->OnClicked.RemoveAll(this); }
	if (ViewClassDistBtn) { ViewClassDistBtn->OnClicked.RemoveAll(this); }
	if (SelectBtn) { SelectBtn->OnClicked.RemoveAll(this); }
	if (CreateBtn) { CreateBtn->OnClicked.RemoveAll(this); }
	if (SelectAllCheckbox) { SelectAllCheckbox->OnCheckStateChanged.RemoveAll(this); }
	
	if(GEditor)
	{
		GEditor->OnLevelActorDeleted().AddUObject(this, &UASToolWindow::HandleOnLevelActorDeletedNative);
	}
	
	Super::NativeDestruct();
}

