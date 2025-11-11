// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBMainScreen.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Building/MBBuildingManager.h"
#include "Development/MBDebug.h"
#include "MBToolAssetData.h"
#include "UI/MBCategoryWindow.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "MBActorFunctions.h"
#include "MBUserSettings.h"
#include "Selection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/TextBlock.h"
#include "Data/MBToolData.h"
#include "MBToolSubsystem.h"
#include "Components/Viewport.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "UI/MBSettingMenu.h"
#include "Framework/Application/IInputProcessor.h"
#include "UI/MBAddCollectionButton.h"
#include "UI/MBCollectionButton.h"
#include "UI/MBCollectionWindow.h"
#include "Data/MBModularEnum.h"
#include "Framework/Application/SlateApplication.h"

#define MarketplaceLink "https://www.unrealengine.com/marketplace/en-US/profile/Leartes+Studios"


#pragma region  InputPreProcessor

class FMBKeyInputPreProcessor : public IInputProcessor
{
public:
	FMBKeyInputPreProcessor()
	{
		
	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		const bool KeyReturn = HandleKey(InKeyEvent.GetKey(),false);
		return KeyReturn;
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		const bool KeyReturn = HandleKey(InKeyEvent.GetKey(),true);
		return KeyReturn;
	}

	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return false;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		const bool KeyReturn = HandleKey(MouseEvent.GetEffectingButton(),true);
		return KeyReturn;
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		const bool KeyReturn = HandleKey(MouseEvent.GetEffectingButton(),false);
		return KeyReturn;
	}

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
	{
		if (InWheelEvent.GetWheelDelta() != 0)
		{
			const FKey Key = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
			const bool KeyReturn = HandleKey(Key,true);
			return KeyReturn;
		}
		return false;
	}

	DECLARE_DELEGATE_RetVal_OneParam(bool,FSettingsPressAnyKeyInputPreProcessorKeySelected, FKey);
	FSettingsPressAnyKeyInputPreProcessorKeySelected OnKeySelected;

	DECLARE_DELEGATE_RetVal_OneParam(bool,FSettingsPressAnyKeyInputPreProcessorCanceled, FKey);
	FSettingsPressAnyKeyInputPreProcessorCanceled OnKeyCanceled;

private:
	bool HandleKey(const FKey& Key, const bool bIsPressed) const
	{
		if(bIsPressed)
		{
			return OnKeySelected.Execute(Key);
		}
		else
		{
			return OnKeyCanceled.Execute(Key);
		}
	}
};
/*---------------------------------------------------------------------------------------------------------------------*/


//Activate Custom Input Class
void UMBMainScreen::ActivateInputProcessor()
{
	//bKeySelected = false;

	InputProcessor = MakeShared<FMBKeyInputPreProcessor>();
	InputProcessor->OnKeySelected.BindUObject(this, &UMBMainScreen::HandleKeySelected);
	InputProcessor->OnKeyCanceled.BindUObject(this, &UMBMainScreen::HandleKeySelectionCanceled);
	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
}

//Deactivate Custom Input Class
void UMBMainScreen::DeactivateInputProcessor() const
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}
}
#pragma endregion  InputPreProcessor

#pragma region InputHandling

bool UMBMainScreen::HandleKeySelected(FKey InKey)
{
	if(InKey == EKeys::LeftMouseButton)
	{
	}
	else if(InKey == EKeys::RightMouseButton)
	{
		
	}
	else if(InKey == EKeys::MiddleMouseButton)
	{
	}
	else if(InKey == EKeys::MouseScrollUp)
	{
		if(ToolSettingsSubsystem && ToolSettingsSubsystem->bIsCtrlPressed)
		{
			AddFreePlacementDistance(true);
		}
	}
	else if(InKey == EKeys::MouseScrollDown)
	{
		if(ToolSettingsSubsystem && ToolSettingsSubsystem->bIsCtrlPressed)
		{
			AddFreePlacementDistance(false);
		}
	}
	else if(InKey == EKeys::Z)
	{
		if (ToolSettingsSubsystem && !ToolSettingsSubsystem->bIsCtrlPressed && BuildingManager)
		{
			if (BuildingManager){BuildingManager->RotateActor(false);}
		}
	} 
	else if(InKey == EKeys::C)
	{
		if (BuildingManager)
		{
			if (BuildingManager){BuildingManager->RotateActor(true);}
		}
	}
	else if(InKey == EKeys::B)
	{
		if (ToolSettingsSubsystem)
		{
			if(ToolSettingsSubsystem->bIsCtrlPressed)
			{
				StartSyncingAssetOnBrowser();
			}
			else if(!ToolSettingsSubsystem->ActiveCollectionWindow.IsNone())
			{
				SelectCollectionPivot();
			}
		}
	}
	else if(InKey == EKeys::LeftShift)
	{
		if(ToolSettingsSubsystem)
		{
			ToolSettingsSubsystem->bIsShiftPressed = true;
		}
	}
	else if(InKey == EKeys::LeftControl)
	{
		if(ToolSettingsSubsystem)
		{
			ToolSettingsSubsystem->bIsCtrlPressed = true;
		}
	}
	else if(InKey == EKeys::LeftAlt)
	{
		if(ToolSettingsSubsystem)
		{
			if(BuildingManager){return BuildingManager->AltPressed(true);}
		}
	}
	return false;
}

bool UMBMainScreen::HandleKeySelectionCanceled(FKey InKey)
{
	if(InKey == EKeys::LeftMouseButton)
	{
		if(BuildingManager){return BuildingManager->LeftClickPressed();}
	}
	else if(InKey == EKeys::RightMouseButton)
	{
		
	}
	else if(InKey == EKeys::N)
	{
		if (BuildingManager)
		{
			if (BuildingManager){BuildingManager->ToggleSecondAxisPositionState();}
		}
	}
	else if(InKey == EKeys::B)
	{
		if (BuildingManager)
		{
			if (BuildingManager){BuildingManager->ToggleThirdAxisPositionState();}
		}
	}
	else if(InKey == EKeys::LeftShift)
	{
		if(ToolSettingsSubsystem)
		{
			ToolSettingsSubsystem->bIsShiftPressed = false;

			if(BuildingManager->GetIsPlacementInProgress())
			{
				ToolSettingsSubsystem->ChangePlacementType();
			}
		}
	}
	else if(InKey == EKeys::LeftControl)
	{
		if(ToolSettingsSubsystem)
		{
			if(BuildingManager && BuildingManager->GetIsPlacementInProgress()){BuildingManager->ToggleRepositionMainAxis();}
			ToolSettingsSubsystem->bIsCtrlPressed = false;
		}
	}
	else if(InKey == EKeys::LeftAlt)
	{
		if(BuildingManager){return BuildingManager->AltPressed(false);}
	}
	else if(InKey == EKeys::Escape)
	{
		if(ToolSettingsSubsystem)
		{
			if (BuildingManager)
			{
				if(CategoryMenu)
                {
                	CategoryMenu->ResetSlotSelectionStates();
                }
				ResetLastAssetSelectionFromMemory();
				return BuildingManager->EscPressed();
			}
	
		}
	}
	return false;
}

void UMBMainScreen::HandleOnAssetRenamed(const FAssetData& InAsset, const FString& InOldObjectPath)
{
	bIsAnAssetChanged = true;
}

void UMBMainScreen::HandleOnAssetUpdated(const FAssetData& InAsset)
{
	bIsAnAssetChanged = true;
}


#pragma endregion InputHandling



void UMBMainScreen::NativeConstruct()
{
	Super::NativeConstruct();
#if WITH_EDITOR
	
	CreateAndInitializeTheBuildingManager();
	
	if((ToolSettingsSubsystem = GEditor->GetEditorSubsystem<UMBToolSubsystem>()))
	{
		ToolSettingsSubsystem->IsTheSettingsOn = false;

		ToolSettingsSubsystem->SetMBToolMainScreen(this);
	}
	else
	{
		MBDebug::PrintLog(TEXT("ToolSettingsSubsystem not found"));
	}
	
	if (IsValid(SettingsBtn) && !SettingsBtn->OnClicked.IsBound())
	{
		SettingsBtn->OnClicked.AddDynamic(this, &UMBMainScreen::SettingsBtnPressed);

		SettingsBtn->SetToolTipText(FText::FromString(TEXT("Open Modular Building Settings")));

	}	

	if (IsValid(LeartesBtn) && !LeartesBtn->OnClicked.IsBound())
	{
		LeartesBtn->OnClicked.AddDynamic(this, &UMBMainScreen::LeartesBtnPressed);

		LeartesBtn->SetToolTipText(FText::FromString(TEXT("Visit Leartes Studios on Marketplace")));
	}


	if (IsValid(ModularCategoryBtn) && !ModularCategoryBtn->OnClicked.IsBound())
	{
		ModularCategoryBtn->OnClicked.AddDynamic(this, &UMBMainScreen::ModularCategoryBtnPressed);

		ModularCategoryBtn->SetToolTipText(FText::FromString(TEXT("Open building category")));
	}

	if(IsValid(ModularCategoryBtn) && IsValid(ToolSettingsSubsystem) && ToolSettingsSubsystem->GetToolData())
	{
		if(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
		{
			ModularCategoryBtn->SetStyle(CategorySelectedBtnStyle);
		}
		else
		{
			ModularCategoryBtn->SetStyle(CategoryNotSelectedBtnStyle);
		}
	}

	if (IsValid(PropCategoryBtn) && !PropCategoryBtn->OnClicked.IsBound())
	{
		PropCategoryBtn->OnClicked.AddDynamic(this, &UMBMainScreen::PropCategoryBtnPressed);

		PropCategoryBtn->SetToolTipText(FText::FromString(TEXT("Open prop category")));
	}

	if(IsValid(PropCategoryBtn) && IsValid(ToolSettingsSubsystem) && ToolSettingsSubsystem->GetToolData())
	{
		if(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Prop)
		{
			PropCategoryBtn->SetStyle(CategorySelectedBtnStyle);
		}
		else
		{
			PropCategoryBtn->SetStyle(CategoryNotSelectedBtnStyle);
		}
	}
	
	SetupBindings();
	
	InitializeTheAssetMenu();

	RegenerateCollectionBox();
	
	ActivateInputProcessor();

	if(IsValid(MultiplePlacementText)){MultiplePlacementText->SetVisibility(ESlateVisibility::Hidden);}

#endif
}
void UMBMainScreen::SetupBindings()
{
	if (IsValid(ToolSettingsSubsystem))
	{
		ToolSettingsSubsystem->OnNewAssetsAdded.AddDynamic(this,&UMBMainScreen::OnNewAssetsAddedToTool);
	}

	if(GEditor)
	{
		GEditor->OnEditorClose().AddUObject(this,&UMBMainScreen::OnEditorShutdown);
		USelection::SelectionChangedEvent.AddUObject(this, &UMBMainScreen::OnSelectionChanged);
	}

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	AssetRegistry.OnAssetRenamed().AddUObject(this, &UMBMainScreen::HandleOnAssetRenamed);
	AssetRegistry.OnAssetUpdated().AddUObject(this, &UMBMainScreen::HandleOnAssetUpdated);
}

void UMBMainScreen::ResetLastAssetSelectionFromMemory() const
{
	if(IsValid(ToolSettingsSubsystem))
	{
		ToolSettingsSubsystem->LastCreatedAsset.Empty();
	}
}

void UMBMainScreen::OnEditorShutdown()
{
	bCanWindowTick = false;

	if(IsValid(BuildingManager))
	{
		BuildingManager->ShutDownBuildingManager();
		
		BuildingManager->ConditionalBeginDestroy();
	}

	EjectAllBindings();
}

void UMBMainScreen::NativeDestruct()
{
	bCanWindowTick = false;

	if(IsValid(BuildingManager))
	{
		BuildingManager->ShutDownBuildingManager();
		
		BuildingManager->ConditionalBeginDestroy();
	}
	
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->LastActiveCollectionIndex = -1;
		ToolSettingsSubsystem->WorkingMode = EMBWorkingMode::None;
		ToolSettingsSubsystem->ActiveCollectionWindow = FName();
		ToolSettingsSubsystem->bIsDuplicationInprogress = false;
		ToolSettingsSubsystem->OnNewAssetsAdded.RemoveAll(this);
		ResetLastAssetSelectionFromMemory();
		ToolSettingsSubsystem->ReleaseMBToolMainScreenRef();
		ToolSettingsSubsystem = nullptr;}
	
		EjectAllBindings();
	
	Super::NativeDestruct();
}

void UMBMainScreen::EjectAllBindings()
{
	FLevelEditorModule& levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	levelEditor.OnActorSelectionChanged().RemoveAll(this);
	
	DeactivateInputProcessor();

	if(InputProcessor)
	{
		InputProcessor->OnKeySelected.Unbind();
		InputProcessor->OnKeyCanceled.Unbind();
		InputProcessor = nullptr;
	}
	
	if(IsValid(SettingsBtn)){SettingsBtn->OnClicked.RemoveAll(this);}
	if(IsValid(LeartesBtn)){LeartesBtn->OnClicked.RemoveAll(this);}
	if(IsValid(ModularCategoryBtn)){ModularCategoryBtn->OnClicked.RemoveAll(this);}
	if(IsValid(PropCategoryBtn)){PropCategoryBtn->OnClicked.RemoveAll(this);}
	
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry"))) {
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		if (AssetRegistryModule.GetPtr()) {
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
			AssetRegistry.OnAssetRenamed().RemoveAll(this);
			AssetRegistry.OnAssetUpdated().RemoveAll(this);
		}
	}

	if(GEditor)
	{
		GEditor->OnEditorClose().AddUObject(this,&UMBMainScreen::OnEditorShutdown);
		USelection::SelectionChangedEvent.RemoveAll(this);
	}
}

void UMBMainScreen::CreateAndInitializeTheBuildingManager()
{
	if(!BuildingManager)
	{
		BuildingManager = NewObject<UMBBuildingManager>();
	}
	if(BuildingManager)
	{
		BuildingManager->InitializeManager();
	}
}

//Create The Categories
void UMBMainScreen::InitializeTheAssetMenu()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	
	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::CategoryMenuPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if ((CategoryMenu = Cast<UMBCategoryWindow>(CreateWidget(EditorWorld, ClassRef))))
		{
			MainContentBox->AddChild(CategoryMenu);
			CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory); //Add Last opened Category index later
		}
	}
}
	
#pragma region Collections

void UMBMainScreen::RegenerateCollectionBox()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!EditorWorld){return;}
	
	CollectionBox->ClearChildren();
	CollectionButtons.Empty();

	const TArray<FName>& Collections = ToolSettingsSubsystem->GetToolData()->Collections;
	
	if(Collections.Num() > 0)
	{
		for(const auto Collection : Collections)
		{

			const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::CollectionButtonPath);
			if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
			{
				if (const auto CollectionBtnWidget  = Cast<UMBCollectionButton>(CreateWidget(EditorWorld, ClassRef)))
				{
					CollectionButtons.Add(CollectionBtnWidget);
					CollectionBtnWidget->SetCollection(Collection);
					CollectionBox->AddChild(CollectionBtnWidget);
				}
			}
		}
	}

	const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::AddCollectionButtonPath);
	if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
	{
		if (const auto AddCollectionBtn = Cast<UMBAddCollectionButton>(CreateWidget(EditorWorld, ClassRef)))
		{
			CollectionBox->AddChild(AddCollectionBtn);
		}
	}
}

void UMBMainScreen::StartSyncingAssetOnBrowser() const
{
	if(BuildingManager)
	{
		if(BuildingManager->GetIsPlacementInProgress())
		{
			ToolSettingsSubsystem->SyncAssetInBrowser(ToolSettingsSubsystem->LastCreatedAsset);
						
			BuildingManager->CancelPlacement();

			if(CategoryMenu)
			{
				ResetLastAssetSelectionFromMemory();
				CategoryMenu->ResetSlotSelectionStates();
			}
		}
	}
}

void UMBMainScreen::SelectCollectionPivot() const
{
	if(MainContentBox->HasAnyChildren())
	{
		if(const auto CollectionWindow = Cast<UMBCollectionWindow>(MainContentBox->GetChildAt(0)))
		{
			CollectionWindow->SelectCollectionTransportBtnPressed();
		}
	}
}


void UMBMainScreen::AddNewCollection()
{
	TurnOffCollectionState(ToolSettingsSubsystem->LastActiveCollectionIndex);
	ToolSettingsSubsystem->LastActiveCollectionIndex = ToolSettingsSubsystem->CreateANewCollection();
	
	if(!ToolSettingsSubsystem->ActiveCollectionWindow.IsNone())
	{
		ToolSettingsSubsystem->ActiveCollectionWindow = FName();
		UpdateTheCollectionTabState();
	}
	RegenerateCollectionBox();
}

void UMBMainScreen::TurnOffCollectionState(const int32 InIndex)
{
	if(InIndex < 0){return;}
	
	if(const auto FoundCollectionWidget = CollectionButtons[InIndex])
	{
		FoundCollectionWidget->SetSelectionState(false);
	}
}

void UMBMainScreen::AssetTypeRemoved()
{
	RegenerateCollectionBox();
	
	if(CategoryMenu)
	{
		CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory);
	}
}

void UMBMainScreen::CollectionRemoved()
{
	UpdateTheCollectionTabState();

	RegenerateCollectionBox();
}

void UMBMainScreen::CollectionRestored()
{
	RegenerateCollectionBox();
}

void UMBMainScreen::CollectionNameChanged(const FName& InOldName, const FName& InNewName)
{
	if(CollectionButtons.IsEmpty()){return;}
	for(const auto Collection : CollectionButtons)
	{
		if(Collection->GetCollectionName() == InOldName)
		{
			Collection->ChangeCollectionName(InNewName);
			return;
		}
	}
}

void UMBMainScreen::UpdateTheCollectionTabState()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if(!ToolSettingsSubsystem || !EditorWorld){return;}

	if(ToolSettingsSubsystem->ActiveCollectionWindow.IsNone())
	{
		MainContentBox->ClearChildren();
		InitializeTheAssetMenu();
	}
	else
	{
		MainContentBox->ClearChildren();
		if(ToolSettingsSubsystem->IsTheSettingsOn)
		{
			OpenSettingsMenu(false);
		}

		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::CollectionWindowPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto CollectionWindow  = Cast<UMBCollectionWindow>(CreateWidget(EditorWorld, ClassRef)))
			{
				if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
				{
					BuildingManager->CancelPlacement();
				}
				
				MainContentBox->AddChild(CollectionWindow);
			}
		}
	}
}

void UMBMainScreen::UpdateCategoryWindow()
{
	if(CategoryMenu)
	{
		CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory); //Add Last opened Category index later
	}
}

#pragma endregion Collections

void UMBMainScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!bCanWindowTick){return;}
	
	if(bListeningForAssetDoubleClickEvent)
	{
		AssetDoubleClickCounter+=InDeltaTime;

		//Normal Asset Click Event
		if(AssetDoubleClickCounter >= 0.35f)
		{
			ResetDoubleClickVariables();
		}
	}
}

void UMBMainScreen::ModularCategoryBtnPressed()
{
	if(IsValid(ToolSettingsSubsystem) && ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory != EBuildingCategory::Modular)
	{
		ChangeTheCategory();

		ModularCategoryBtn->SetStyle(CategorySelectedBtnStyle);
		PropCategoryBtn->SetStyle(CategoryNotSelectedBtnStyle);
	}
}

void UMBMainScreen::PropCategoryBtnPressed()
{
	if(IsValid(ToolSettingsSubsystem) && ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory != EBuildingCategory::Prop)
	{
		ChangeTheCategory();

		PropCategoryBtn->SetStyle(CategorySelectedBtnStyle);
		ModularCategoryBtn->SetStyle(CategoryNotSelectedBtnStyle);
	}
}

void UMBMainScreen::SettingsBtnPressed()
{
	if(!bIsUserSettingsOn)
	{
		if(SettingsMenu)
		{
			OpenSettingsMenu(false);
		}
		SettingsBox->ClearChildren();
	
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
		
		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ToolUserSettingsPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto UserSettings  = Cast<UUserWidget>(CreateWidget(EditorWorld, ClassRef)))
			{
				SettingsBox->AddChild(UserSettings);
			}
		}

		bIsUserSettingsOn = true;
	}
	else
	{
		SettingsBox->ClearChildren();
		bIsUserSettingsOn = false;
	}
}

void UMBMainScreen::LeartesBtnPressed()
{
	FPlatformProcess::LaunchURL(TEXT(MarketplaceLink),nullptr,nullptr);
}


void UMBMainScreen::OpenSettingsMenu(const bool bInOpen)
{
	if(!ToolSettingsSubsystem->IsTheSettingsOn && bInOpen && ToolSettingsSubsystem->ActiveCollectionWindow.IsNone())
	{
		UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();

		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::ToolSettingsPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if ((SettingsMenu  = Cast<UMBSettingMenu>(CreateWidget(EditorWorld, ClassRef))))
			{
				SettingsBox->AddChild(SettingsMenu);
				ToolSettingsSubsystem->IsTheSettingsOn = true;
				SettingsMenu->RegenerateTheSettings();
			}
		}
	}
	else if(ToolSettingsSubsystem->IsTheSettingsOn && !bInOpen)
	{
		SettingsBox->ClearChildren();
		SettingsMenu = nullptr;
		ToolSettingsSubsystem->IsTheSettingsOn = false;
	}
}

void UMBMainScreen::SetMultiplePlacementAmountText(const FString& InText) const
{
	if(InText.IsEmpty())
	{
		MultiplePlacementText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		MultiplePlacementText->SetText(FText::FromString(InText));
		MultiplePlacementText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMBMainScreen::ResetToolWindowInterface()
{
	if(MainContentBox)
	{
		MainContentBox->ClearChildren();
		InitializeTheAssetMenu();
	}
	
	RegenerateCollectionBox();

	if(CategoryMenu && ToolSettingsSubsystem)
	{
		CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory);
	}
	
	if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
	{
		BuildingManager->CancelPlacement();
	}
}

void UMBMainScreen::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	ToolSettingsSubsystem->bIsMouseOnToolWindow = true;

	if(bIsAnAssetChanged)
	{
		RegenerateCollectionBox();

		if(CategoryMenu)
		{
			CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory);
		}
		bIsAnAssetChanged = false;
	}
}

void UMBMainScreen::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	ToolSettingsSubsystem->bIsMouseOnToolWindow = false;
}


UObject* UMBMainScreen::GetBuildingManager()
{
	return BuildingManager;
}

void UMBMainScreen::ChangeTheCategory()
{
	ResetLastAssetSelectionFromMemory();
	
	if(BuildingManager)
	{
		BuildingManager->CancelPlacement();
	}
	if(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory == EBuildingCategory::Modular)
	{
		ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory = EBuildingCategory::Prop;
		CategoryMenu->RegenerateTheCategoryMenu(EBuildingCategory::Prop);

		if(SettingsMenu)
		{
			SettingsMenu->RegenerateTheSettings();
		}
	}
	else
	{
		ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory = EBuildingCategory::Modular;
		CategoryMenu->RegenerateTheCategoryMenu(EBuildingCategory::Modular);

		if(SettingsMenu)
		{
			SettingsMenu->RegenerateTheSettings();
		}
	}
}


void UMBMainScreen::CreateAssetClicked(const FString AssetName)
{
	if(AssetName.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("Asset Name Is Empty!"));return;}
	if(!ToolSettingsSubsystem){return;}
	
	if(bListeningForAssetDoubleClickEvent)
	{
		if(CategoryMenu && !ToolSettingsSubsystem->LastCreatedAsset.Equals(AssetName))
		{
			if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
			{
				CategoryMenu->ResetSlotSelectionStates();
				
				if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
				{
					BuildingManager->CancelPlacement();
				}
			}
		}
		
		if(AssetToCreate.Equals(AssetName))
		{
			CreateAssetDoubleClickClickEvent();
			return;
		}
		ResetDoubleClickVariables();
	}
	else
	{
		if(ToolSettingsSubsystem->LastCreatedAsset.IsEmpty() || !ToolSettingsSubsystem->LastCreatedAsset.Equals(AssetName))
		{
			if(CategoryMenu && !ToolSettingsSubsystem->LastCreatedAsset.Equals(AssetName))
			{
				CategoryMenu->ResetSlotSelectionStates();
				CreateAssetOneClickEvent(AssetName);
			}
		}
		else
		{
			ResetLastAssetSelectionFromMemory();
			CategoryMenu->ResetSlotSelectionStates();
				
			if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
			{
				BuildingManager->CancelPlacement();
			}
		}
		AssetToCreate = AssetName;
		StartAssetDoubleClickListener();
	}
}

void UMBMainScreen::StartAssetDoubleClickListener()
{
	bListeningForAssetDoubleClickEvent = true;
}

void UMBMainScreen::CreateAssetOneClickEvent(const FString& InAssetName)
{
	if(ToolSettingsSubsystem->WorkingMode == EMBWorkingMode::ModActorAction && ToolSettingsSubsystem->bIsDuplicationInprogress)
	{
		if(BuildingManager->HasTheDuplicationReset())
		{
			BuildingManager->StopModularDuplication(false);
		}
		else
		{
			const auto ReturnType = MBDebug::ShowMsgDialog(EAppMsgType::OkCancel,TEXT("You have an ongoing modular actor action. \n Would yo like to save the old action before committing a new one?"));
			if(ReturnType == EAppReturnType::Ok)
			{
				BuildingManager->StopModularDuplication(true);
				//return;
			}
			else if(ReturnType == EAppReturnType::Cancel)
			{
				BuildingManager->StopModularDuplication(false);
			}
		}
	}
	
	ToolSettingsSubsystem->WorkingMode = EMBWorkingMode::Placement;
	if (BuildingManager){BuildingManager->CreateAssetAndStartPlacementProgress(InAssetName);}

	ToolSettingsSubsystem->LastCreatedAsset = InAssetName;

	if(bIsUserSettingsOn)
	{
		SettingsBox->ClearChildren();
		bIsUserSettingsOn = false;
	}
	
	OpenSettingsMenu(true);
	if(SettingsMenu){SettingsMenu->RegenerateTheSettings();}
}

void UMBMainScreen::CreateAssetDoubleClickClickEvent()
{
	//Firstly Cancel Current Building
	if(BuildingManager)
	{
		BuildingManager->CancelPlacement();
	}
	
	if(ToolSettingsSubsystem)
	{
		ToolSettingsSubsystem->OpenAsset(ToolSettingsSubsystem->LastCreatedAsset);
	}
	
	ResetDoubleClickVariables();
}

void UMBMainScreen::ResetDoubleClickVariables()
{
	bListeningForAssetDoubleClickEvent = false;
	AssetDoubleClickCounter = 0.0f;
	AssetToCreate.Empty();
}





void UMBMainScreen::StartModularDuplicationFromSettings(const FDuplicationData& InDuplicationData)
{
	if (BuildingManager){BuildingManager->SetNewDuplicationDataInBm(InDuplicationData);}
}

void UMBMainScreen::ApplyModularDuplicationPressed()
{
	if (BuildingManager){BuildingManager->StopModularDuplication(true);}
	OpenSettingsMenu(false);
}

TArray<FDuplicationData*> UMBMainScreen::GetExistingDuplicationData()
{
	return BuildingManager->GetDuplicationData();
}

void UMBMainScreen::ApplyModularDuplicationFilter(const FDuplicationFilters& InDuplicationFilters)
{
	BuildingManager->ApplyModularDuplicationFilterInBM(InDuplicationFilters);
	
}

FDuplicationFilters* UMBMainScreen::GetExistingDuplicationFilter()
{
	return BuildingManager->GetExistingDuplicationFilterInBM();
}


void UMBMainScreen::OnNewAssetsAddedToTool()
{
	CategoryMenu->RegenerateTheCategoryMenu(ToolSettingsSubsystem->GetToolData()->LastActiveBuildingCategory);
}

#pragma region ActorSelection

void UMBMainScreen::OnSelectionChanged(UObject* Object)
{
	if(!GEditor){return;}
	const auto EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if(!EditorActorSubsystem){return;}
	
	//Close Settings
	if(bIsUserSettingsOn)
	{
		SettingsBox->ClearChildren();
		bIsUserSettingsOn = false;
	}

	//Dont do anything if building is in progress
	if(BuildingManager->GetIsPlacementInProgress())
	{
		EditorActorSubsystem->SelectNothing();
		return;
	}
	
	//Shut down first
	const TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	if(SelectedActors.IsEmpty()){return;}
	
	EMBWorkingMode WorkingMode = EMBWorkingMode::None;
	for(const auto CurrentActor : SelectedActors)
	{
		const auto CurrentBuildingCategory = UMBActorFunctions::GetBuildingCategory(CurrentActor);
		if(CurrentBuildingCategory == EBuildingCategory::None)
		{
			WorkingMode = EMBWorkingMode::None;
			break;
		}
		if(CurrentBuildingCategory == EBuildingCategory::Modular)
		{
			if(WorkingMode == EMBWorkingMode::ModActorAction || WorkingMode == EMBWorkingMode::None)
			{
				WorkingMode = EMBWorkingMode::ModActorAction;
			}
			else
			{
				WorkingMode = EMBWorkingMode::None;
				break;
			}
		}
		else if(CurrentBuildingCategory == EBuildingCategory::Prop)
		{
			if(WorkingMode == EMBWorkingMode::PropActorAction || WorkingMode == EMBWorkingMode::None)
			{
				WorkingMode = EMBWorkingMode::PropActorAction;
			}
			else
			{
				WorkingMode = EMBWorkingMode::None;
				break;
			}
		}
	}
	
	if(!ToolSettingsSubsystem){return;}

	if(ToolSettingsSubsystem->WorkingMode != WorkingMode)
	{
		if(ToolSettingsSubsystem->bIsDuplicationInprogress && WorkingMode != EMBWorkingMode::ModActorAction)
		{
			if(BuildingManager)
			{
				BuildingManager->StopModularDuplication(false);
			}
		}
		
		ToolSettingsSubsystem->WorkingMode = WorkingMode;
		
		OpenSettingsMenu(true);
		
		if(SettingsMenu)
		{
			SettingsMenu->RegenerateTheSettings();
		}
	}
	else
	{
		ToolSettingsSubsystem->WorkingMode = WorkingMode;
	}
}

void UMBMainScreen::AddFreePlacementDistance(const bool bIsAdding) const
{
	if(BuildingManager && BuildingManager->GetIsPlacementInProgress())
	{
		if(GEditor && GEditor->GetActiveViewport()->HasFocus())
		{
			ToolSettingsSubsystem->GetToolUserSettings()->FreePlacementDistance = FMath::Clamp(ToolSettingsSubsystem->GetToolUserSettings()->FreePlacementDistance + (bIsAdding ? 200.0f : -200.0f),500.0f,50000.0f);
			ToolSettingsSubsystem->GetToolUserSettings()->SaveConfig();
		}
	}
}

#pragma endregion ActorSelection

#pragma region FastClick


