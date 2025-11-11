// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODToolWindow.h"
#include "Editor.h"
#include "EditorUtilityLibrary.h"
#include "LevelEditorViewport.h"
#include "ODAssetFunctions.h"
#include "ODDebug.h"
#include "ODDiskDistribution.h"
#include "ODDistributionLine.h"
#include "ODPresetObject.h"
#include "ODSpiralDistribution.h"
#include "Components/Button.h"
#include "Components/DetailsView.h"
#include "Components/EditableTextBox.h"
#include "ODSpawnCenter.h"
#include "ODToolSubsystem.h"
#include "Editor/UnrealEdEngine.h"
#include "ODDistributionBase.h"
#include "ODRingDistribution.h"
#include "ODDistributionCube.h"
#include "ODDistributionPlane.h"
#include "ODObjectSlot.h"
#include "ODPaletteDataObject.h"
#include "ODPresetData.h"
#include "ODSimulationWidget.h"
#include "ODSphereDistribution.h"
#include "ODTDDistribution.h"
#include "ODToolAssetData.h"
#include "ODPaletteSlate.h"
#include "ODToolSettings.h"
#include "Components/BillboardComponent.h"
#include "Components/Border.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/ODToolInputProcessor.h"

void UODToolWindow::NativePreConstruct()
{
	Super::NativePreConstruct();
	
#if WITH_EDITOR
	
	if((PresetObject = Cast<UODPresetObject>(NewObject<UODPresetObject>(this, TEXT("PresetObject")))))
	{
		PresetObject->SetupPresets();
		PresetDetails->SetObject(PresetObject);
	}

	if(IsValid(PaletteSlate) && IsValid(PaletteSlate->GetPropDistributionDetails()))
	{
		if((PropDistributionBase = Cast<UODDistributionBase>(NewObject<UODDistributionCube>(this, TEXT("ODDistributionBase")))))
		{
			PropDistributionBase->OnDistributionTypeChangedSignature.BindUObject(this, &UODToolWindow::OnDistributionTypeChanged);
			PropDistributionBase->OnAfterODRegenerated.BindUObject(this, &UODToolWindow::OnAfterODRegenerated);
			PropDistributionBase->OnFinishConditionChangeSignature.BindUObject(this, &UODToolWindow::OnFinishConditionChanged);
			PropDistributionBase->OnTotalSpawnCountChanged.BindUObject(this, &UODToolWindow::OnTotalSpawnCountChanged);
			PaletteSlate->GetPropDistributionDetails()->SetObject(PropDistributionBase);
		}
	}

	if(IsValid(FinishBtn)){FinishBtn->SetVisibility(ESlateVisibility::Collapsed);}
	
	//Tool Tips
	if(IsValid(FinishBtn)){FinishBtn->SetToolTipText(FText::FromName(TEXT("Finish the distribution and perform the operation according to the selected finish type.")));}
	if(IsValid(AddNewPresetBtn)){AddNewPresetBtn->SetToolTipText(FText::FromName(TEXT("Add an empty preset and give it a new name.")));}
	if(IsValid(RenamePresetBtn)){RenamePresetBtn->SetToolTipText(FText::FromName(TEXT("Rename the active preset.")));}
	if(IsValid(RemovePresetBtn)){RemovePresetBtn->SetToolTipText(FText::FromName(TEXT("Remove the active preset.")));}
	if(IsValid(AddSelectedAssetsBtn)){AddSelectedAssetsBtn->SetToolTipText(FText::FromName(TEXT("Create a preset from the selected static meshes in the Content Browser.")));}
	if(IsValid(SavePresetBtn)){SavePresetBtn->SetToolTipText(FText::FromName(TEXT("Save the active preset.")));}
	if(IsValid(SaveAsNewPresetBtn)){SaveAsNewPresetBtn->SetToolTipText(FText::FromName(TEXT("Create a new preset using the existing object distribution data.")));}

	CreateSimulationWidget();
	
#endif // WITH_EDITOR
}

void UODToolWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
#if WITH_EDITOR

	ActivateODInputProcessor();
	
	if (IsValid(AddNewPresetBtn) && !AddNewPresetBtn->OnClicked.IsBound())
	{
	    AddNewPresetBtn->OnClicked.AddDynamic(this, &UODToolWindow::AddNewPresetBtnPressed);
	}

	if (IsValid(AddSelectedAssetsBtn) && !AddSelectedAssetsBtn->OnClicked.IsBound())
	{
	    AddSelectedAssetsBtn->OnClicked.AddDynamic(this, &UODToolWindow::AddSelectedAssetsBtnPressed);
	}

	if (IsValid(RenamePresetBtn) && !RenamePresetBtn->OnClicked.IsBound())
	{
	    RenamePresetBtn->OnClicked.AddDynamic(this, &UODToolWindow::RenamePresetBtnPressed);
	}

	if (IsValid(SavePresetBtn) && !SavePresetBtn->OnClicked.IsBound())
	{
	    SavePresetBtn->OnClicked.AddDynamic(this, &UODToolWindow::SavePresetBtnPressed);
	}

	if (IsValid(SaveAsNewPresetBtn) && !SaveAsNewPresetBtn->OnClicked.IsBound())
	{
	    SaveAsNewPresetBtn->OnClicked.AddDynamic(this, &UODToolWindow::SaveAsNewPresetBtnPressed);
	}

	if (IsValid(RemovePresetBtn) && !RemovePresetBtn->OnClicked.IsBound())
	{
	    RemovePresetBtn->OnClicked.AddDynamic(this, &UODToolWindow::RemovePresetBtnPressed);
	}

	if (IsValid(NewPresetText) && !NewPresetText->OnTextCommitted.IsBound())
	{
	    NewPresetText->OnTextCommitted.AddDynamic(this, &UODToolWindow::OnNewPresetTextCommitted);
	}

	if (IsValid(AddAssetsText) && !AddAssetsText->OnTextCommitted.IsBound())
	{
	    AddAssetsText->OnTextCommitted.AddDynamic(this, &UODToolWindow::OnAddAssetsTextCommitted);
	}

	if (IsValid(SaveAsText) && !SaveAsText->OnTextCommitted.IsBound())
	{
	    SaveAsText->OnTextCommitted.AddDynamic(this, &UODToolWindow::OnSaveAsTextCommitted);
	}

	if (IsValid(RenamePresetText) && !RenamePresetText->OnTextCommitted.IsBound())
	{
	    RenamePresetText->OnTextCommitted.AddDynamic(this, &UODToolWindow::OnRenamePresetTextCommitted);
	}

	if (IsValid(FinishBtn) && !FinishBtn->OnClicked.IsBound())
	{
	    FinishBtn->OnClicked.AddDynamic(this, &UODToolWindow::FinishBtnPressed);
	}
	
	if(IsValid(PaletteSlate))
	{
		PaletteSlate->OnPaletteButtonPressed.BindUObject(this,&UODToolWindow::OnPaletteButtonPressed);
		PaletteSlate->OnAssetsDropped.BindUObject(this,&UODToolWindow::OnAssetsDropped);
		PaletteSlate->OnPaletteBackgroundPressedSignature.BindUObject(this,&UODToolWindow::OnPaletteBackgroundPressed);
		PaletteSlate->GetPaletteDataObject()->OnPaletteObjectParamChanged.BindUObject(this,&UODToolWindow::OnPaletteObjectParamChanged);
	}

	if(IsValid(PaletteSlate) && IsValid(PaletteSlate->GetPaletteObjectDataDetails()))
	{
		PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(PresetObject){PresetObject->OnPresetCategoryHidden.BindUObject(this,&UODToolWindow::OnPresetCategoryHidden);}
	
	if(PropDistributionBase){PropDistributionBase->Setup(this);}
	
	SpawnSpawnCenter();

	if(GEditor)
	{
		GEditor->OnLevelActorDeleted().AddUObject(this, &UODToolWindow::HandleOnLevelActorDeletedNative);
	}
	
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->OnPresetLoaded.AddUObject(this,&UODToolWindow::OnPresetLoaded);
		ToolSubsystem->OnAMixerPresetCheckStatusChanged.BindUObject(this,&UODToolWindow::OnAMixerPresetCheckStatusChanged);
		ToolSubsystem->OnObjectActivateStatusChanged.BindUObject(this,&UODToolWindow::OnObjectActivateStatusChanged);
		ToolSubsystem->OnLocalDensityChanged.BindUObject(this,&UODToolWindow::OnLocalSpawnDensityChanged);
		ToolSubsystem->OnToolReset.BindUObject(this,&UODToolWindow::OnToolReset);
		ToolSubsystem->OnVisualizationParamChanged.BindUObject(this,&UODToolWindow::OnVisualizationParamChanged);
		ToolSubsystem->OnPhysicsParamChanged.BindUObject(this,&UODToolWindow::OnPhysicsParamChanged);

		if(!ToolSubsystem->GetLastSelectedPreset().IsNone() && !ToolSubsystem->IsInMixerMode())
		{
			ToolSubsystem->LoadActivePreset();
		}

		if(PresetSettingsBorder){PresetSettingsBorder->SetVisibility(ToolSubsystem->bInMixerMode ? ESlateVisibility::Collapsed :ESlateVisibility::Visible);}
		
		ToolSubsystem->OnMixerModeChanged.BindUObject(this,&UODToolWindow::OnMixerModeChanged);

		OnDistributionTypeChanged(ToolSubsystem->GetODToolSettings()->LastSelectedDistributionType);
	}

	FEditorDelegates::BeginPIE.AddUObject(this, &UODToolWindow::HandleBeginPIE);
	FEditorDelegates::EndPIE.AddUObject(this, &UODToolWindow::HandleEndPIE);
	FEditorDelegates::PausePIE.AddUObject(this, &UODToolWindow::HandlePausePIE);
	FEditorDelegates::ResumePIE.AddUObject(this, &UODToolWindow::HandleResumePIE);

	
#endif // WITH_EDITOR
}


void UODToolWindow::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(IsValid(SpawnCenterRef))
	{
		//Position Trace
		if(!bTraceForVelocity)
		{
			if(!SpawnCenterRef->GetActorLocation().Equals(LastSpawnCenterLocation))
			{
				bTraceForVelocity = true;
			}
		}
		if(bTraceForVelocity)
		{
			const FVector VelocityChange =  SpawnCenterRef->GetActorLocation() - LastSpawnCenterLocation;
			SpawnCenterVelocity += VelocityChange;

			//Finish 
			if(VelocityChange.SquaredLength() < 1)
			{
				if(PropDistributionBase){PropDistributionBase->AddSpawnCenterMotionDifferences(SpawnCenterVelocity);}
				bTraceForVelocity = false;
				SpawnCenterVelocity = FVector::ZeroVector;
				
				if(SpawnCenterRef)
				{
					SpawnCenterRef->RegenerateBoundsDrawData();
				}
			}
			LastSpawnCenterLocation = SpawnCenterRef->GetActorLocation();
		}

		//Rotation Trace
		if(!bTraceForRotationDiff)
		{
			if(!SpawnCenterRef->GetActorRotation().Equals(LastSpawnCenterRotation))
			{

				bTraceForRotationDiff = true;
			}
		}
		if(bTraceForRotationDiff)
		{
			const FRotator RotChange =  SpawnCenterRef->GetActorRotation() - LastSpawnCenterRotation;
			SpawnCenterRotation += RotChange;

			//Finish 
			//if(RotChange.IsNearlyZero(1))
			if(true)
			{
				if(PropDistributionBase){PropDistributionBase->ReRotateObjectsOnSpawnCenter();}
				bTraceForRotationDiff = false;
				SpawnCenterRotation = FRotator::ZeroRotator;
				
				if(SpawnCenterRef)
				{
					SpawnCenterRef->RegenerateBoundsDrawData();
				}
			}
			LastSpawnCenterRotation = SpawnCenterRef->GetActorRotation();
		}
	}

	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->NatureTick(InDeltaTime);
	}
}

void UODToolWindow::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsMouseOnToolWindow = true;
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UODToolWindow::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	IsMouseOnToolWindow = false;
	
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UODToolWindow::OnDistributionTypeChanged(EObjectDistributionType NewDistributionType)
{
	if(!PropDistributionBase){return;}
	
	if(PropDistributionBase->OnDistributionTypeChangedSignature.IsBound())
	{
		PropDistributionBase->OnDistributionTypeChangedSignature.Unbind();
	}
	if(PropDistributionBase->OnAfterODRegenerated.IsBound())
	{
		PropDistributionBase->OnAfterODRegenerated.Unbind();
	}
	
	PropDistributionBase->OnAfterODRegenerated.Unbind();
	
	if((NewDistributionType == EObjectDistributionType::Cube))
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODDistributionCube::StaticClass(), FName(TEXT("ODDistributionCube")));
		if(const auto LocalDistBase = NewObject<UODDistributionCube>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Ring)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODRingDistribution::StaticClass(), FName(TEXT("ODRingDistribution")));
		if(const auto LocalDistBase = NewObject<UODRingDistribution>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Disk)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODDiskDistribution::StaticClass(), FName(TEXT("ODDiskDistribution")));
		if(const auto LocalDistBase = NewObject<UODDiskDistribution>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Sphere)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODSphereDistribution::StaticClass(), FName(TEXT("ODSphereDistribution")));
		if(const auto LocalDistBase = NewObject<UODSphereDistribution>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Spiral)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODSpiralDistribution::StaticClass(), FName(TEXT("ODSpiralDistribution")));
		if(const auto LocalDistBase = NewObject<UODSpiralDistribution>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Line)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODDistributionLine::StaticClass(), FName(TEXT("ODLineDistribution")));
		if(const auto LocalDistBase = NewObject<UODDistributionLine>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Plane)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODDistributionPlane::StaticClass(), FName(TEXT("ODDistributionPlane")));
		if(const auto LocalDistBase = NewObject<UODDistributionPlane>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	else if(NewDistributionType == EObjectDistributionType::Grid)
	{
		const FName Name = MakeUniqueObjectName(GetWorld(), UODTDDistribution::StaticClass(), FName(TEXT("ODGridDistribution")));
		if(const auto LocalDistBase = NewObject<UODTDDistribution>(this, Name))
		{
			PropDistributionBase = LocalDistBase;
		}
	}
	PaletteSlate->GetPropDistributionDetails()->SetObject(PropDistributionBase);

	PropDistributionBase->Setup(this);

	PropDistributionBase->OnAfterODRegenerated.BindUObject(this, &UODToolWindow::OnAfterODRegenerated);
	PropDistributionBase->OnFinishConditionChangeSignature.BindUObject(this, &UODToolWindow::OnFinishConditionChanged);
	
	PropDistributionBase->ReDesignObjects();
	
	PropDistributionBase->OnDistributionTypeChangedSignature.BindUObject(this, &UODToolWindow::OnDistributionTypeChanged);
	PropDistributionBase->OnTotalSpawnCountChanged.BindUObject(this, &UODToolWindow::OnTotalSpawnCountChanged);

}


/*void UODToolWindow::TestBtnPressed()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->CheckAllSMValidations();
	}
}*/

void UODToolWindow::CreateSimulationWidget()
{
	if((SimulationWidget = NewObject<UODSimulationWidget>(this)))
	{
		SimulationBorder->AddChild(SimulationWidget);
		SimulationWidget->OnStartSimClicked.BindUObject(this,&UODToolWindow::OnStartSimClicked);
		SimulationWidget->OnStopSimClicked.BindUObject(this,&UODToolWindow::OnStopSimClicked);
	}
}

void UODToolWindow::SpawnSpawnCenter()
{
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(SpawnCenterRef && !EditorWorld){return;}

	const FVector SpawnLocation = FindSpawnLocationForSpawnCenter();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnCenterRef = EditorWorld->SpawnActor<AODSpawnCenter>(AODSpawnCenter::StaticClass(),SpawnLocation,FRotator::ZeroRotator,SpawnParams);
	if(SpawnCenterRef)
	{
		const FSoftObjectPath PlaceholderTexturePath("/Engine/EditorResources/S_TargetPoint.S_TargetPoint");
		if(UObject* ToolWindowObject = PlaceholderTexturePath.TryLoad())
		{
			if(const auto PlaceHolderTexture = Cast<UTexture2D>(ToolWindowObject))
			{
				SpawnCenterRef->GetBillboardComp()->SetSprite(PlaceHolderTexture);
			}
		}
		LastSpawnCenterLocation = SpawnCenterRef->GetActorLocation(); //Last SpawnCenter Ref
		if(const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>())
		{
			EditorActorSubsystem->SelectNothing();
			EditorActorSubsystem->SetActorSelectionState(SpawnCenterRef,true);
		}
	}
}

AActor* UODToolWindow::GetSpawnCenterRef() const
{
	return SpawnCenterRef;
}

FVector UODToolWindow::FindSpawnLocationForSpawnCenter()
{
	if(!GCurrentLevelEditingViewportClient || !GEditor){return FVector::ZeroVector;}
	
	const FVector StartLoc = GCurrentLevelEditingViewportClient->GetViewLocation();
	FVector EndLoc = StartLoc + GCurrentLevelEditingViewportClient->GetViewRotation().Vector() * 500.0f;

	auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return FVector::ZeroVector;}

	static const FName lineTraceSingleName(TEXT("LevelEditorLineTrace"));
	FCollisionQueryParams collisionParams(lineTraceSingleName);
	collisionParams.bTraceComplex = false;
	collisionParams.bReturnPhysicalMaterial = true;
	//collisionParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	FCollisionObjectQueryParams objectParams = FCollisionObjectQueryParams(ECC_WorldStatic);
	
	objectParams.AddObjectTypesToQuery(ECC_Visibility);
	objectParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult HitResult;
	EditorWorld->LineTraceSingleByObjectType(HitResult, StartLoc, EndLoc, objectParams, collisionParams);

	if(HitResult.bBlockingHit)
	{
		EndLoc.Z = HitResult.Location.Z;
		return EndLoc;
	}
	return EndLoc;
}

void UODToolWindow::GetSpawnCenterToCameraView() const
{
	SpawnCenterRef->SetActorLocation(FindSpawnLocationForSpawnCenter());
}

void UODToolWindow::AddNewPresetBtnPressed()
{
	AddNewPresetBtn->SetVisibility(ESlateVisibility::Collapsed);
	NewPresetText->SetVisibility(ESlateVisibility::Visible);
	NewPresetText->SetKeyboardFocus();
}

void UODToolWindow::AddSelectedAssetsBtnPressed()
{
	const auto LocalSelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	auto RunWarningMessage =  [] () -> void
	{
		ODDebug::ShowMsgDialog(EAppMsgType::Ok,FString(TEXT("Please select a valid static mesh asset first.")));
	};

	if(LocalSelectedAssets.IsEmpty())
	{
		RunWarningMessage();
		return;
	}
	
	bool bIsContainsAnySM = false;
	for(const auto FoundSM : LocalSelectedAssets)
	{
		if(FoundSM->IsA(UStaticMesh::StaticClass()))
		{
			bIsContainsAnySM = true;
			break;
		}
	}
	if(!bIsContainsAnySM)
	{
		RunWarningMessage();
		return;
	}
	
	AddSelectedAssetsBtn->SetVisibility(ESlateVisibility::Collapsed);
	AddAssetsText->SetVisibility(ESlateVisibility::Visible);
	AddAssetsText->SetKeyboardFocus();
}

void UODToolWindow::OnAssetsDropped(TArrayView<FAssetData> DroppedAssets)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	if(ToolSubsystem->AddAssetsToPalette(DroppedAssets))
	{
		RebuildPalette();

		if(IsValid(PropDistributionBase))
		{
			PropDistributionBase->CalculateTotalSpawnCount();
		}
	}
}

void UODToolWindow::OnVisualizationParamChanged()
{
	if(IsValid(SpawnCenterRef))
	{
		SpawnCenterRef->GetBillboardComp()->SetVisibility(false);
		SpawnCenterRef->GetBillboardComp()->SetVisibility(true);
		SpawnCenterRef->RegenerateBoundsDrawData();
	}
}

void UODToolWindow::OnPhysicsParamChanged()
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->SetSimulatePhysics();
	}
}

void UODToolWindow::OnAddAssetsTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if(InCommitMethod == ETextCommit::Type::Default){return;}
	
	if(InCommitMethod == ETextCommit::Type::OnEnter)
	{
		if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
		{
			if(!InText.IsEmpty() && !InText.ToString().ToLower().Equals(TEXT("no preset")) && !InText.ToString().ToLower().Equals(TEXT("nopreset")) && !ToolSubsystem->IsPresetAvailable(InText.ToString()))
			{
				//Create if at least one of the selected assets is a static mesh.
				if(ToolSubsystem->CreateNewPresetFromSelectedAssets(InText.ToString()))
				{
					ToolSubsystem->SetActivePreset(InText.ToString());

					if(PresetObject)
					{
						PresetObject->SetupPresets();
					}
					ToolSubsystem->LoadActivePreset();
				
					ODDebug::ShowNotifyInfo(FString(TEXT("Preset Created Succesfully")));
				}
			}
		}
	}
	AddAssetsText->SetText(FText());
	AddSelectedAssetsBtn->SetVisibility(ESlateVisibility::Visible);
	AddAssetsText->SetVisibility(ESlateVisibility::Collapsed);
}



void UODToolWindow::RenamePresetBtnPressed()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		if(ToolSubsystem->GetLastSelectedPreset().IsNone())
		{
			ODDebug::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please select a valid preset first"),true);
		}
		else
		{
			RenamePresetBtn->SetVisibility(ESlateVisibility::Collapsed);
			RenamePresetText->SetVisibility(ESlateVisibility::Visible);
			RenamePresetText->SetKeyboardFocus();
		}
	}
}

void UODToolWindow::SavePresetBtnPressed()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		if(ToolSubsystem->GetLastSelectedPreset().IsNone())
		{
			ODDebug::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please select a valid preset first"),true);
		}
		else
		{
			if(ToolSubsystem->SaveCurrentPreset())
			{
				ODDebug::ShowNotifyInfo(FString(TEXT("Preset Saved Succesfully")));
			}
		}
	}
}

void UODToolWindow::SaveAsNewPresetBtnPressed()
{
	SaveAsNewPresetBtn->SetVisibility(ESlateVisibility::Collapsed);
	SaveAsText->SetVisibility(ESlateVisibility::Visible);
	SaveAsText->SetKeyboardFocus();
}



void UODToolWindow::RemovePresetBtnPressed()
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		if(ToolSubsystem->GetLastSelectedPreset().IsNone())
		{
			ODDebug::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please select a valid preset first"),true);
		}
		else
		{
			ToolSubsystem->RemoveCurrentPreset();

			ToolSubsystem->SetLastPresetAsActivePreset(); //V1.3
			
			if(PresetObject)
			{
				PresetObject->SetupPresets();
			}

			ToolSubsystem->LoadActivePreset();

			ODDebug::ShowNotifyInfo(FString(TEXT("Preset removed Succesfully")));
		}
	}
}


void UODToolWindow::OnNewPresetTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if(InCommitMethod == ETextCommit::Type::OnEnter)
	{
		if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
		{
			if(!InText.IsEmpty() && !InText.ToString().ToLower().Equals(TEXT("no preset")) && !InText.ToString().ToLower().Equals(TEXT("nopreset")) && !ToolSubsystem->IsPresetAvailable(InText.ToString()))
			{
				ToolSubsystem->AddNewPreset(InText.ToString());

				//If current preset is none then load newly created one
				if(ToolSubsystem->GetLastSelectedPreset().IsNone())
				{
					ToolSubsystem->SetActivePreset(InText.ToString());

					if (PresetObject)
					{
						PresetObject->SetupPresets();
					}

					ToolSubsystem->LoadActivePreset();
				}
				else
				{
					if (PresetObject)
					{
						PresetObject->SetupPresets();
					}
				}
				
				ODDebug::ShowNotifyInfo(FString(TEXT("Preset Added Succesfully")));
			}
		}
	}
	
	
	NewPresetText->SetText(FText());
	AddNewPresetBtn->SetVisibility(ESlateVisibility::Visible);
	NewPresetText->SetVisibility(ESlateVisibility::Collapsed);
}

void UODToolWindow::OnRenamePresetTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if(InCommitMethod == ETextCommit::Type::OnEnter)
	{
		if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
		{
			if(!InText.IsEmpty() && !InText.ToString().ToLower().Equals(TEXT("no preset")) && !InText.ToString().ToLower().Equals(TEXT("nopreset")) && !ToolSubsystem->IsPresetAvailable(InText.ToString()))
			{
				ToolSubsystem->RenameCurrentPreset(InText.ToString());
				if (PresetObject)
				{
					PresetObject->SetupPresets();
				}
			
				ODDebug::ShowNotifyInfo(FString(TEXT("Preset Renamed Succesfully")));
			}
		}
	}
	RenamePresetText->SetText(FText());
	RenamePresetBtn->SetVisibility(ESlateVisibility::Visible);
	RenamePresetText->SetVisibility(ESlateVisibility::Collapsed);
}

void UODToolWindow::OnSaveAsTextCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if(InCommitMethod == ETextCommit::Type::OnEnter)
	{
		if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
		{
			if(!InText.IsEmpty() && !InText.ToString().ToLower().Equals(TEXT("no preset")) && !InText.ToString().ToLower().Equals(TEXT("nopreset")) && !ToolSubsystem->IsPresetAvailable(InText.ToString()))
			{
				ToolSubsystem->AddNewPreset(InText.ToString());
				ToolSubsystem->SetActivePreset(InText.ToString());

				for(auto& CurrentData : ToolSubsystem->ObjectDistributionData)
				{
					CurrentData.OwnerPreset = ToolSubsystem->GetLastSelectedPreset();
				}
				
				if(ToolSubsystem->SaveCurrentPreset())
				{
					if(PresetObject)
					{
						PresetObject->SetupPresets();
					}

					ODDebug::ShowNotifyInfo(FString(TEXT("Preset Added Succesfully")));
				}
			}
		}
	}

	SaveAsText->SetText(FText());
	SaveAsNewPresetBtn->SetVisibility(ESlateVisibility::Visible);
	SaveAsText->SetVisibility(ESlateVisibility::Collapsed);
}

void UODToolWindow::FinishBtnPressed()
{
	if(PropDistributionBase)
	{
		PropDistributionBase->OnFinishDistributionPressed();
	}
	if(IsValid(SimulationWidget))
	{
		SimulationWidget->SimulationFinished();
	}
}

void UODToolWindow::OnFinishConditionChanged(bool InCanFinish)
{
	if(IsValid(FinishBtn)){FinishBtn->SetVisibility(InCanFinish ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);}
}

void UODToolWindow::OnAfterODRegenerated() const
{
	if(SpawnCenterRef)
	{
		SpawnCenterRef->RegenerateBoundsDrawData();
	}
	if(PaletteSlate && PropDistributionBase)
	{
		PaletteSlate->OnDistributionRegenerated(PropDistributionBase->GetCollidingObjects());
	}
}

void UODToolWindow::OnMixerModeChanged(bool InIsInMixerMode)
{
	ClearPalette();
	
	if(!PropDistributionBase){return;}
	if(PresetSettingsBorder){PresetSettingsBorder->SetVisibility(InIsInMixerMode ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);}
	const auto CurrentDistType = PropDistributionBase->DistributionType;
	
	OnDistributionTypeChanged(CurrentDistType);

	TitleBorder->SetBrushColor(InIsInMixerMode ? 
	FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("1E5428FF")))
		:
	FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("242424FF"))));

	if(!InIsInMixerMode)
	{
		if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
		{
			if(!ToolSubsystem->GetLastSelectedPreset().IsNone())
			{
				ToolSubsystem->LoadActivePreset();
			}
		}
	}
}

void UODToolWindow::OnPresetCategoryHidden(bool InbIsItOpen)
{
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		if(!ToolSubsystem->bInMixerMode)
		{
			PresetSettingsBorder->SetVisibility(InbIsItOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}
	}
}


void UODToolWindow::HandleOnLevelActorDeletedNative(AActor* InActor)
{
	if(!bIsToolDestroying && InActor == SpawnCenterRef)
	{
		SpawnCenterRef = nullptr;
		SpawnSpawnCenter();
	}
	
	if(PropDistributionBase)
	{
		PropDistributionBase->LevelActorDeleted(InActor);
	}
}



void UODToolWindow::NativeDestruct()
{
	DeactivateODInputProcessor();
	
	ClearPalette();

	FEditorDelegates::BeginPIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);
	FEditorDelegates::PausePIE.RemoveAll(this);
	FEditorDelegates::ResumePIE.RemoveAll(this);
	
	if (const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		if(IsValid(ToolSubsystem))
		{
			ToolSubsystem->ObjectDistributionData.Empty();
			ToolSubsystem->OnMixerModeChanged.Unbind();
			ToolSubsystem->OnPresetLoaded.RemoveAll(this);
			ToolSubsystem->OnAMixerPresetCheckStatusChanged.Unbind();
			ToolSubsystem->OnObjectActivateStatusChanged.Unbind();
			ToolSubsystem->OnLocalDensityChanged.Unbind();
			ToolSubsystem->OnToolReset.Unbind();
			ToolSubsystem->OnVisualizationParamChanged.Unbind();
			ToolSubsystem->OnPhysicsParamChanged.Unbind();
			ToolSubsystem->ToolWindowClosed();
		}
	}
	
	if (IsValid(AddNewPresetBtn)) { AddNewPresetBtn->OnClicked.RemoveAll(this); }
	if (IsValid(AddSelectedAssetsBtn)) { AddSelectedAssetsBtn->OnClicked.RemoveAll(this); }
	if (IsValid(RenamePresetBtn)) { RenamePresetBtn->OnClicked.RemoveAll(this); }
	if (IsValid(SavePresetBtn)) { SavePresetBtn->OnClicked.RemoveAll(this); }
	if (IsValid(SaveAsNewPresetBtn)) { SaveAsNewPresetBtn->OnClicked.RemoveAll(this); }
	if (IsValid(RemovePresetBtn)) { RemovePresetBtn->OnClicked.RemoveAll(this); }
	//if (IsValid(PaletteBackgroundBtn)) { PaletteBackgroundBtn->OnClicked.RemoveAll(this); }
	if (IsValid(FinishBtn)) { FinishBtn->OnClicked.RemoveAll(this); }
	if (IsValid(NewPresetText)) { NewPresetText->OnTextCommitted.RemoveAll(this); }
	if (IsValid(AddAssetsText)) { AddAssetsText->OnTextCommitted.RemoveAll(this); }
	if (IsValid(SaveAsText)) { SaveAsText->OnTextCommitted.RemoveAll(this); }
	if (IsValid(RenamePresetText)) { RenamePresetText->OnTextCommitted.RemoveAll(this); }

	if(IsValid(PropDistributionBase))
	{
		if(PropDistributionBase->OnDistributionTypeChangedSignature.IsBound())
		{
			PropDistributionBase->OnDistributionTypeChangedSignature.Unbind();
		}
		if(PropDistributionBase->OnAfterODRegenerated.IsBound())
		{
			PropDistributionBase->OnAfterODRegenerated.Unbind();
		}
		if(PropDistributionBase->OnFinishConditionChangeSignature.IsBound())
		{
			PropDistributionBase->OnFinishConditionChangeSignature.Unbind();
		}
	}

	if(IsValid(PresetObject)){PresetObject->OnPresetCategoryHidden.Unbind();}
	
	bIsToolDestroying = true;
	
	if(IsValid(SpawnCenterRef))
	{
		SpawnCenterRef->Destroy();
	}
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	EditorActorSubsystem->SelectNothing();
	
#if WITH_EDITOR
	if (!GEditor) return;
	GEditor->OnLevelActorDeleted().RemoveAll(this);
#endif
	
	Super::NativeDestruct();
}

void UODToolWindow::OnPresetLoaded()
{
	RebuildPalette();
}

void UODToolWindow::RebuildPalette()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem || !EditorWorld){return;}

	const auto& DistObjectData  =  ToolSubsystem->ObjectDistributionData;
	const int32 Num = DistObjectData.Num();

	ObjectSlots.Empty();
	PaletteSlate->ClearChildren();

	if(Num == 0){return;}
	
	for(int32 Index = 0; Index < Num ; ++Index)
	{
		auto& FoundData = DistObjectData[Index];
		if(!UODToolSubsystem::IsSoftObjectValidToUse(FoundData.StaticMesh)){continue;}
		
		auto LoadedSMData = UODAssetFunctions::GetAssetDataFromObject(FoundData.StaticMesh.LoadSynchronous());
		if(!IsValid(LoadedSMData.GetAsset())){continue;}

		const TSoftClassPtr<UUserWidget> WidgetClassPtr(ODToolAssetData::ObjectSlotPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (auto CreatedObjectSlotWidget = Cast<UODObjectSlot>(CreateWidget(EditorWorld,ClassRef)))
			{
				CreatedObjectSlotWidget->SetSlotParams(LoadedSMData,FoundData.ActiveStatus,Index,FoundData.DistributionProperties.SpawnCount,FoundData.OwnerPreset,ToolSubsystem->GetColorOfPreset(*FoundData.OwnerPreset.ToString()));
				CreatedObjectSlotWidget->OnObjectSlotClickedSignature.BindUObject(this,&UODToolWindow::OnObjectSlotPressed);
				CreatedObjectSlotWidget->OnSelectItems.BindUObject(this,&UODToolWindow::OnSelectSlotItems);
				CreatedObjectSlotWidget->OnRemoveSelectedItems.BindUObject(this,&UODToolWindow::OnRemoveItemsFromPalette);
				CreatedObjectSlotWidget->OnSlotDetailsVisibilityChanged.BindUObject(this,&UODToolWindow::OnSlotDetailsVisibilityChanged);
				PaletteSlate->AddChildToWrapBox(CreatedObjectSlotWidget);
				ObjectSlots.Add(CreatedObjectSlotWidget);
			}
		}
	}
}

void UODToolWindow::OnObjectSlotPressed(const bool InFormalSelectionState,const int32 InSlotIndex)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}

	bool bIsPaletteObjectDetailsVisible = true;
	
	if(bIsLeftCtrlPressed)
	{
		if(InFormalSelectionState)
		{
			if(ToolSubsystem->SelectedPaletteSlotIndexes.Contains(InSlotIndex))
			{
				for(const auto FoundSlot : ObjectSlots)
				{
					if(FoundSlot->GetSlotIndex() == InSlotIndex)
					{
						FoundSlot->SetSelectionState(false);

						ToolSubsystem->SelectedPaletteSlotIndexes.Remove(InSlotIndex);

						if(ToolSubsystem->SelectedPaletteSlotIndexes.IsEmpty())
						{
							bIsPaletteObjectDetailsVisible = false;
						}
						break;
					}
				}
			}
		}
		else
		{
			for(const auto FoundSlot : ObjectSlots)
			{
				if(FoundSlot->GetSlotIndex() == InSlotIndex)
				{
					FoundSlot->SetSelectionState(true);
					ToolSubsystem->SelectedPaletteSlotIndexes.AddUnique(InSlotIndex);
					break;
				}
			}
		}

		ToolSubsystem->LastSelectIndex = InSlotIndex;  
	}
	else if(bIsLeftShiftPressed)
	{
		if (ToolSubsystem->LastSelectIndex <= InSlotIndex)
		{
			for (int Index = ToolSubsystem->LastSelectIndex; Index <= InSlotIndex; ++Index)
			{
				if(ObjectSlots.IsValidIndex(Index))
				{
					ObjectSlots[Index]->SetSelectionState(true);
					ToolSubsystem->SelectedPaletteSlotIndexes.AddUnique(Index);
				}
			}
		}
		else
		{
			for (int Index = InSlotIndex; Index <= ToolSubsystem->LastSelectIndex; ++Index)
			{
				if(ObjectSlots.IsValidIndex(Index))
				{
					ObjectSlots[Index]->SetSelectionState(true);
					ToolSubsystem->SelectedPaletteSlotIndexes.AddUnique(Index);
				}
			}
		}
	}
	else
	{
		ToolSubsystem->SelectedPaletteSlotIndexes.Empty();
		
		for(const auto CurrentSlot : ObjectSlots)
		{
			if(CurrentSlot->GetSlotIndex() == InSlotIndex)
			{
				CurrentSlot->SetSelectionState(true);
				ToolSubsystem->SelectedPaletteSlotIndexes.AddUnique(InSlotIndex);
				ToolSubsystem->LastSelectIndex = InSlotIndex;
			}
			else
			{
				CurrentSlot->SetSelectionState(false);
			}
		}
	}
	
	PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(bIsPaletteObjectDetailsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	NotifyPaletteSlotSelectionChanges();
}

void UODToolWindow::OnSelectSlotItems(const bool InSelect, TArray<int32> InItemIndexes)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	if(!InItemIndexes.IsEmpty())
	{
		for(const auto CurrentSlot : ObjectSlots)
		{
			if(InItemIndexes.Contains(CurrentSlot->GetSlotIndex()))
			{
				CurrentSlot->SetSelectionState(InSelect);
			}
			if(InSelect)
			{
				ToolSubsystem->SelectedPaletteSlotIndexes.AddUnique(CurrentSlot->GetSlotIndex());
			}
			else
			{
				if(InItemIndexes.Contains(CurrentSlot->GetSlotIndex()))
				{
					ToolSubsystem->SelectedPaletteSlotIndexes.Remove(CurrentSlot->GetSlotIndex());
				}
			}
		}
		PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		for(const auto CurrentSlot : ObjectSlots)
		{
			CurrentSlot->SetSelectionState(InSelect);
		}

		//Select All
		if(InSelect)
		{
			const int32 Num = ToolSubsystem->ObjectDistributionData.Num();

			for(int32 CurrentIndex = 0 ; CurrentIndex < Num ;  ++CurrentIndex)
			{
				ToolSubsystem->SelectedPaletteSlotIndexes.Add(CurrentIndex);
			}

			PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(ESlateVisibility::Visible);
		}
		else //Select Nothing
		{
			ToolSubsystem->SelectedPaletteSlotIndexes.Empty();
			
			ToolSubsystem->LastSelectIndex = -1;

			PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	NotifyPaletteSlotSelectionChanges();
}
void UODToolWindow::NotifyPaletteSlotSelectionChanges() const
{
	if(IsValid(PaletteSlate) && IsValid(PaletteSlate->GetPaletteDataObject()))
	{
		PaletteSlate->GetPaletteDataObject()->LoadSelectedObjectData();
	}
	
	if(const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>())
	{
		ToolSubsystem->RestartDensitySession();
	}
	
	if(IsValid(PaletteSlate))
	{
		PaletteSlate->OnObjectSelectionChanged();
	}
}

void UODToolWindow::PaletteBackgroundBtnPressed()
{
	OnSelectSlotItems(false,{});
}
void UODToolWindow::ClearPalette()
{
	if(IsValid(PaletteSlate))
	{
		PaletteSlate->ClearChildren();
	}
	
	ObjectSlots.Empty();
	
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(IsValid(ToolSubsystem))
	{
		ToolSubsystem->SelectedPaletteSlotIndexes.Empty();
		ToolSubsystem->LastSelectIndex = -1;
	}
}

#pragma region  InputPreProcessor



void UODToolWindow::OnPaletteObjectParamChanged(const FName InParamName)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}
	
	if(InParamName.IsEqual("StaticMesh"))
	{
		const auto LoadedSMData = UODAssetFunctions::GetAssetDataFromObject(PaletteSlate->GetPaletteDataObject()->StaticMesh.LoadSynchronous());
		if(!LoadedSMData.IsValid()){return;}
		
		for(const auto& PaletteSlot : ObjectSlots)
		{
			const auto& CurrentIndex = ToolSubsystem->SelectedPaletteSlotIndexes;
			if(CurrentIndex.Contains(PaletteSlot->GetSlotIndex()))
			{
				PaletteSlot->CreateSlotThumbnail(LoadedSMData);
			}
		}
	}
	if(InParamName.IsEqual("SpawnCount"))
	{
		PropDistributionBase->CalculateTotalSpawnCount();

		for(const auto& PaletteSlot : ObjectSlots)
		{
			const auto& CurrentIndex = ToolSubsystem->SelectedPaletteSlotIndexes;
			if(CurrentIndex.Contains(PaletteSlot->GetSlotIndex()))
			{
				if(ToolSubsystem->ObjectDistributionData.IsValidIndex(PaletteSlot->GetSlotIndex()))
				{
					PaletteSlot->SetSpawnCount(ToolSubsystem->ObjectDistributionData[PaletteSlot->GetSlotIndex()].DistributionProperties.SpawnCount);
				}
			}
		}
	}
}

void UODToolWindow::SimulationStoppedWithForcibly()
{
	if(IsValid(SimulationWidget)){SimulationWidget->ResetSimulationInterface();}
}

void UODToolWindow::OnAMixerPresetCheckStatusChanged(bool InNewCheckStatus, FName InPresetName)
{
	RebuildPalette();

	if(PropDistributionBase)
	{
		PropDistributionBase->CalculateTotalSpawnCount();
	}
}

void UODToolWindow::OnObjectActivateStatusChanged(const bool InActivate, const TArray<int32> InIndex)
{
	for(auto CurrentSlot : ObjectSlots)
	{
		if(IsValid(CurrentSlot) && InIndex.Contains(CurrentSlot->GetSlotIndex()))
		{
			CurrentSlot->ChangeActivateStatus(InActivate);
		}
	}

	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->CalculateTotalSpawnCount();
	}
}

void UODToolWindow::OnLocalSpawnDensityChanged()
{
	OnPaletteObjectParamChanged(FName("SpawnCount"));
}

void UODToolWindow::OnPaletteButtonPressed(const EPaletteButtonType InPaletteButtonType)
{
	if(IsValid(PropDistributionBase))
	{
		switch(InPaletteButtonType) {
		case EPaletteButtonType::Create:
			PropDistributionBase->OnCreateDistributionPressed();
			break;
		case EPaletteButtonType::Shuffle:
			PropDistributionBase->OnShuffleDistributionPressed();
			break;
		case EPaletteButtonType::Delete:
			PropDistributionBase->OnDeleteDistributionPressed();
			break;
		case EPaletteButtonType::SelectAll:
			PropDistributionBase->OnSelectObjectsPressed();
			break;
		case EPaletteButtonType::SelectSpawnCenter:
			PropDistributionBase->OnSelectSpawnCenterPressed();
			break;
		case EPaletteButtonType::MoveToWorldCenter:
			PropDistributionBase->OnMoveSpawnCenterToWorldOriginPressed();
			break;
		case EPaletteButtonType::MoveToCamera:
			PropDistributionBase->OnMoveSpawnCenterToCameraPressed();
			break;
		default: ;
		}
	}
}


void UODToolWindow::OnStartSimClicked()
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->OnStartBtnPressed();
	}
}

void UODToolWindow::OnStopSimClicked()
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->OnStopBtnPressed();
	}
}


void UODToolWindow::HandleBeginPIE(const bool bIsSimulating)
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->HandleBeginPIE();
	}
	if(IsValid(SimulationWidget))
	{
		SimulationWidget->OnHandleBeginPie();
	}
}

void UODToolWindow::HandleEndPIE(const bool bIsSimulating)
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->HandleEndPIE();
	}
	if(IsValid(SimulationWidget) && GEditor)
	{
		if(GEditor->GetPIEWorldContext())
		{
			if(IsValid(SimulationWidget))
			{
				SimulationWidget->SimulationFinished();	
			}
		}
	}
}

void UODToolWindow::HandlePausePIE(bool bIsSimulating)
{
	if(IsValid(SimulationWidget))
	{
		SimulationWidget->OnHandlePausePie();
	}
}

void UODToolWindow::HandleResumePIE(bool bIsSimulating)
{
	if(IsValid(SimulationWidget))
	{
		SimulationWidget->OnHandleResumePie();
	}
}

void UODToolWindow::OnToolReset()
{
	if(IsValid(PropDistributionBase))
	{
		PropDistributionBase->LoadDistData();
	}
	if(IsValid(SpawnCenterRef))
	{
		if(const auto SpawnCenter = Cast<AODSpawnCenter>(SpawnCenterRef))
		{
			SpawnCenter->RegenerateBoundsDrawData();
		}
	}
	if(IsValid(PaletteSlate))
	{
		PaletteSlate->OnResetParameters();
	}
}

void UODToolWindow::OnTotalSpawnCountChanged(const float InSpawnCount)
{
	if(IsValid(PaletteSlate))
	{
		PaletteSlate->OnTotalSpawnCountChanged(InSpawnCount);
	}
}

void UODToolWindow::OnPaletteBackgroundPressed()
{
	OnSelectSlotItems(false,{});
}

void UODToolWindow::OnRemoveItemsFromPalette(const TArray<int32> InItemsToRemove)
{
	const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystem){return;}

	TArray<int32> SlotIndexes = InItemsToRemove;
	
	//Delete Selected Object From Palette
	if(!SlotIndexes.IsEmpty())
	{
		if(GEditor && !GEditor->GetActiveViewport()->HasFocus())
		{
			if(SlotIndexes.Num() > 1)
			{
				SlotIndexes.Sort([](const int32& A, const int32& B) {return B < A;});
			}

			ToolSubsystem->SelectedPaletteSlotIndexes = SlotIndexes;
			
			ToolSubsystem->RemoveSelectedAssetsFromPalette();

			if(PropDistributionBase)
			{
				PropDistributionBase->CalculateTotalSpawnCount();
			}
				
			PaletteSlate->GetPaletteObjectDataDetails()->SetVisibility(ESlateVisibility::Collapsed);
				
			RebuildPalette();
		}
	}
}

void UODToolWindow::OnSlotDetailsVisibilityChanged(const bool InbNewVisibility, const TArray<int32> InSlotIndexes)
{
	if(ObjectSlots.IsEmpty()){return;}
		
	if(InbNewVisibility)
	{
		for(const auto CurrentSlot : ObjectSlots)
		{
			if(InSlotIndexes.Contains(CurrentSlot->GetSlotIndex()))
			{
				CurrentSlot->SetDetailsVisibility(true);
			}
			else
			{
				CurrentSlot->SetDetailsVisibility(false);
			}
		}
	}
	else
	{
		for(const auto CurrentSlot : ObjectSlots)
		{
			CurrentSlot->SetDetailsVisibility(false);
		}
	}
}

void UODToolWindow::ActivateODInputProcessor()
{
	ODInputProcessor = MakeShared<FODKeyInputPreProcessor>();
	ODInputProcessor->OnKeySelected.BindUObject(this, &UODToolWindow::HandleKeySelected);
	ODInputProcessor->OnKeyCanceled.BindUObject(this, &UODToolWindow::HandleKeySelectionCanceled);
	FSlateApplication::Get().RegisterInputPreProcessor(ODInputProcessor, 0);
}

void UODToolWindow::DeactivateODInputProcessor() const
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(ODInputProcessor);
	}
}
#pragma endregion  InputPreProcessor

#pragma region InputHandling

bool UODToolWindow::HandleKeySelected(FKey InKey)
{
	if(InKey == EKeys::LeftShift)
	{
		bIsLeftShiftPressed = true;
	}
	else if(InKey == EKeys::LeftControl)
	{
		bIsLeftCtrlPressed = true;
	}
	else if(InKey == EKeys::A)
	{
		if(IsMouseOnToolWindow)
		{
			OnSelectSlotItems(true,{});
			return true;
		}
		
	}
	return false;
}

bool UODToolWindow::HandleKeySelectionCanceled(FKey InKey)
{
	if(InKey == EKeys::LeftShift)
	{
		bIsLeftShiftPressed = false;

	}
	else if(InKey == EKeys::LeftControl)
	{
		bIsLeftCtrlPressed = false;
	}
	else if(InKey == EKeys::Escape)
	{
		OnSelectSlotItems(false,{});
	}
	else if(InKey == EKeys::Delete)
	{
		const auto ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
		if(!ToolSubsystem){return false;}

		if(!ToolSubsystem->SelectedPaletteSlotIndexes.IsEmpty())
		{
			OnRemoveItemsFromPalette(ToolSubsystem->SelectedPaletteSlotIndexes);
		}
	}
	return false;
}
#pragma endregion InputHandling

