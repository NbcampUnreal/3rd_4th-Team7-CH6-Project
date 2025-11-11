// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "ODPaletteSlate.h"
#include "DetailLayoutBuilder.h"
#include "Editor.h"
#include "EditorStyleSet.h"
#include "ODPaletteDataObject.h"
#include "ODToolSettings.h"
#include "ODToolStyle.h"
#include "ODToolSubsystem.h"
#include "Components/WrapBoxSlot.h"
#include "SAssetDropTarget.h"
#include "Components/DetailsView.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSpacer.h"

#define LOCTEXT_NAMESPACE "ObjectDistribution"

UODPaletteSlate::UODPaletteSlate(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer),ToolSubsystem(),PaletteDataObject(),PaletteObjectDataDetails(),PropDistributionDetails()
{
	bIsVariable = false;
	UWidget::SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WrapSize = 500;
	bExplicitWrapSize = false;
	HorizontalAlignment = HAlign_Left;
	Orientation = EOrientation::Orient_Horizontal;
}

TObjectPtr<UODToolSubsystem> UODPaletteSlate::GetToolSubsystem()
{
	if(ToolSubsystem){return ToolSubsystem;}
	ToolSubsystem = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	return ToolSubsystem;
}


bool UODPaletteSlate::IsInPie()
{
	if (GEditor == nullptr){return true;}

	if(GEditor->GetPIEWorldContext())
	{
		return true;

	}
	return false;
}

void UODPaletteSlate::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyWrapBox.Reset();
}

UClass* UODPaletteSlate::GetSlotClass() const
{
	return UWrapBoxSlot::StaticClass();
}

void UODPaletteSlate::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if ( MyWrapBox.IsValid() )
	{
		CastChecked<UWrapBoxSlot>(InSlot)->BuildSlot(MyWrapBox.ToSharedRef());
	}
}

void UODPaletteSlate::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if ( MyWrapBox.IsValid() && InSlot->Content)
	{
		const TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if ( Widget.IsValid() )
		{
			MyWrapBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

UWrapBoxSlot* UODPaletteSlate::AddChildToWrapBox(UWidget* Content)
{
	return Cast<UWrapBoxSlot>(Super::AddChild(Content));
}

TSharedRef<SWidget> UODPaletteSlate::BuildCreationToolbar() const
{
	FSlimHorizontalToolBarBuilder Toolbar(nullptr, FMultiBoxCustomization::None);
	FUIAction CreateBtnAction;
	CreateBtnAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::Create);
	});
	FUIAction ShuffleBtnAction;
	ShuffleBtnAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::Shuffle);
			
	});
	FUIAction SelectCenterAction;
	SelectCenterAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::SelectSpawnCenter);
			
	});
	
	FUIAction SelectAllAction;
	SelectAllAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::SelectAll);
			
	});
	FUIAction DeleteAllAction;
	DeleteAllAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::Delete);
	});
	
	Toolbar.AddToolBarButton(CreateBtnAction, NAME_None, LOCTEXT("CreateBtn", "Create"), FText::FromName("Creates static meshes."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	Toolbar.AddToolBarButton(ShuffleBtnAction, NAME_None, LOCTEXT("ShuffleBtn", "Shuffle"), FText::FromName("Randomizes the positions of created static meshes."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	Toolbar.AddToolBarButton(SelectCenterAction, NAME_None, LOCTEXT("SelectCenterBtn", "Select Center"), FText::FromName("Selects the Spawn Center actor."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	Toolbar.AddToolBarButton(SelectAllAction, NAME_None, LOCTEXT("SelectAllBtn", "Select All"), FText::FromName("Selects all created static meshes."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	Toolbar.AddToolBarButton(DeleteAllAction, NAME_None, LOCTEXT("DeleteAllBtn", "Delete All"), FText::FromName("Deletes all created static meshes."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	return Toolbar.MakeWidget();
}

TSharedRef<SWidget> UODPaletteSlate::BuildSpawnCenterToolbar() const
{
	FSlimHorizontalToolBarBuilder Toolbar(nullptr, FMultiBoxCustomization::None);

	FUIAction MoveToWorldAction;
	MoveToWorldAction.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::MoveToWorldCenter);
	});
	FUIAction MoveToCamera;
	MoveToCamera.ExecuteAction.BindLambda([&]
	{
		OnPaletteButtonPressed.ExecuteIfBound(EPaletteButtonType::MoveToCamera);
	});
	
	Toolbar.AddToolBarButton(MoveToWorldAction, NAME_None, LOCTEXT("MoveToWorldBtn", "Move To World"), FText::FromName("Spawn Center moves to the center of the Level."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	Toolbar.AddToolBarButton(MoveToCamera, NAME_None, LOCTEXT("MoveToCameraBtn", "Move To Camera"), FText::FromName("Spawn Center moves in front of the camera."), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.box-perspective"));
	return Toolbar.MakeWidget();
}


TSharedRef<SWidget> UODPaletteSlate::RebuildWidget()
{
	FinishingTypeNames.Add(MakeShareable(new FString("Keep")));
	FinishingTypeNames.Add(MakeShareable(new FString("Batch")));
	FinishingTypeNames.Add(MakeShareable(new FString("Run Merge Tool")));
	TargetTypeNames.Add(MakeShareable(new FString("SM Component")));
	TargetTypeNames.Add(MakeShareable(new FString("ISM Component")));
	TargetTypeNames.Add(MakeShareable(new FString("HISM Component")));
	
	//Palette Data Object
	PaletteDataObject = Cast<UODPaletteDataObject>(NewObject<UODPaletteDataObject>(this, TEXT("PaletteDataObject")));
	PaletteDataObject->SetupObject();
	PaletteObjectDataDetails = NewObject<UDetailsView>(this);
	PaletteObjectDataDetails->SetObject(PaletteDataObject);
	
	PropDistributionDetails = NewObject<UDetailsView>(this);
	
	const FMargin StandardRightPadding(3.f, 2.f, 6.f, 2.f);

	const FSlateFontInfo StandardFont = FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
	
	auto NewWidget = SNew(SVerticalBox)

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(6.f,0.f,6.f,0.f)
	[
		SNew(SHeader)
		//.Visibility(this, &SFoliageEdit::GetVisibility_Options)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CreationHeader", "Creation"))
			.Font(StandardFont)
			.ColorAndOpacity(FLinearColor::White)
		]
	]

	+SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(6.f,3.f,6.f,2.f))
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				
				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SpawnDensityTextx", "Spawn Density: "))
						.ToolTipText(LOCTEXT("SpawnDensityTextxToolTipText","Increases or decreases the spawn count of static meshes selected in the palette in proportion to their current spawn count."))
						.Font(StandardFont)
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.Padding(StandardRightPadding)
					.FillWidth(1.0f)
					.AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					//.MaxWidth(100.f)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.MinDesiredWidth(100)
						[
							SAssignNew(SpawnDensityBox,SNumericEntryBox<float>)
							.Font(StandardFont)
							.AllowSpin(true)
							.MinValue(0.0f)
							.MaxValue(1.0f)
							.MaxSliderValue(1.0f)
							//.MinDesiredValueWidth(50.0f)
							//.SliderExponent(3.0f)
							.Value_UObject(this,&UODPaletteSlate::GetSpawnDensityValue)
							.OnValueChanged_UObject(this, &UODPaletteSlate::SpawnDensityChanged)
							.IsEnabled_UObject(this, &UODPaletteSlate::IsEnabled_SpawnDensity)
						]
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SSpacer)
					.Size(FVector2D(15.0f,0.0f))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TotalSpawnCountText", "Total Spawn Count: "))
						.ToolTipText(LOCTEXT("TotalSpawnCountToolTipText", "Shows total static mesh count to spawn."))
						.Font(StandardFont)
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.Padding(FMargin(4,0,8,0))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					[
						SNew(SBox)
						.MinDesiredWidth(96)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
							[
								SNew(SBox)
								.Padding(1)
								[
									SAssignNew(TotalSpawnCount,STextBlock)
									.Text(LOCTEXT("TotalCount","0"))
									.Font(StandardFont)
									.ColorAndOpacity(FLinearColor::White)
								]
							]
						]
					]
				]
			]
				
			+ SVerticalBox::Slot()
			.Padding(FMargin(6,2))
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					//.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("KillZText", "Kill Z: "))
						.Font(StandardFont)
						.ToolTipText(LOCTEXT("KillZTooltipText","Simulated static meshes are automatically destroyed if they fall below the specified height. In certain situations, like when the collision of the static mesh is too small, this can be used to prevent the static mesh from going below the ground due to the simulated mesh being detected by the ground."))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.Padding(StandardRightPadding)
					//.FillWidth(1.0f)
					.AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					//.MaxWidth(100.f)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.MinDesiredWidth(100)
						[
							SNew(SNumericEntryBox<float>)
							.Font(StandardFont)
							.AllowSpin(false)
							.MinValue(-50000)
							.MaxValue(50000)
							.Value_UObject(this, &UODPaletteSlate::GetKillZValue)
							.OnValueChanged_UObject(this, &UODPaletteSlate::OnKillZChanged)
							//.IsEnabled_UObject(this, &UODPaletteSlate::IsEnabled_CollisionTest)
						]
					]

				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SSpacer)
					.Size(FVector2D(15.0f,0.0f))
				]
				
				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					//.Padding(StandardLeftPadding)
					.FillWidth(1.0f)
					[
						SNew(SCheckBox)
						.OnCheckStateChanged_Lambda([this] (ECheckBoxState NewCheckState) { SetCollisionTestCheckState(NewCheckState == ECheckBoxState::Checked ? true : false); } )
						.IsChecked_Lambda([this] { return GetCollisionTestCheckState() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
						//.ToolTipText(LOCTEXT("SingleInstantiationModeTooltips", "Paint a single foliage instance at the mouse cursor location (i + Mouse Click)"))
						[
							SNew(SBox)
							.Padding(FMargin(3,0,0,0))
							[
								SNew(STextBlock)
								.Text(LOCTEXT("MaxCollisionTestText", "Collision Test: "))
								.ToolTipText(LOCTEXT("MaxCollisionTestToolTipText","Prevents the initialization of nested static meshes in limited space by collision testing the static meshes until they find a free space."))
								.Font(StandardFont)
							]
						]
					]
					+ SHorizontalBox::Slot()
					.Padding(StandardRightPadding)
					.MaxWidth(100.f)
					.AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					[
						SNew(SBox)
						.MinDesiredWidth(100)
						[
						
							SNew(SNumericEntryBox<int32>)
							.Font(StandardFont)
							.AllowSpin(true)
							.MinValue(1)
							.MaxValue(250)
							.MinSliderValue(1)
							.MaxSliderValue(250)
							.MinDesiredValueWidth(1)
							.SliderExponent(2)
							.Value_UObject(this, &UODPaletteSlate::GetCollisionTestValue)
							.OnValueChanged_UObject(this, &UODPaletteSlate::OnCollisionTestChanged)
							.IsEnabled_UObject(this, &UODPaletteSlate::IsEnabled_CollisionTest)
						]
					]
				]
			]

			+ SVerticalBox::Slot()
			.Padding(FMargin(6,2))
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				//.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("FinishingTypeText", "Finishing Type: "))
						.Font(StandardFont)
						.ToolTipText(LOCTEXT("FinishingTypeToolTipText","Determines the state of simulated static meshes upon completion of the simulation with the Finish button. The Keep option retains each mesh as a static mesh. The Batch option converts to the type specified by the Target Component. The Run Merge Tool executes the built-in static mesh merge tool."))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.Padding(StandardRightPadding)
					//.FillWidth(1.0f)
					.AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					//.MaxWidth(100.f)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.MinDesiredWidth(100)
						.HAlign(HAlign_Fill)
						[
							//Finishing ComboBox
							SAssignNew(FinishingTypeComboBox,STextComboBox)
						   .Font(IDetailLayoutBuilder::GetDetailFont())
						   .OptionsSource(&FinishingTypeNames)
						   .InitiallySelectedItem(GetInitialFinishingTypeName())
						   .OnSelectionChanged_Lambda([this](const TSharedPtr<FString>& String, ESelectInfo::Type)
						   {
							   UE_LOG(LogTemp,Warning,TEXT("Selection Changed %s"),**String.Get());

								 GetToolSubsystem()->GetODToolSettings()->FinishingType = *String.Get();
						   	
								  if(String.Get()->Equals("Batch"))
								  {
										 TargetTypeComboBox->SetEnabled(true);
								  }
								  else
								  {
									  TargetTypeComboBox->SetEnabled(false);
								  }
						   })
						]
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SSpacer)
					.Size(FVector2D(15.0f,0.0f))
				]
				
				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					//.Padding(StandardLeftPadding)
					.FillWidth(1.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TargetComponentText", "Target Component: "))
						.Font(StandardFont)
						.ToolTipText(LOCTEXT("TargetComponentToolTipText","This property is ative when the finish type is set to 'Batch'. All simulated static meshes are converted to the component type specified here and gathered under a single actor."))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.Padding(StandardRightPadding)
					.MaxWidth(100.f)
					.AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					[
						SNew(SBox)
						.MinDesiredWidth(100)
						.HAlign(HAlign_Fill)
						[
							//Target Combobox
							SAssignNew(TargetTypeComboBox,STextComboBox)
						   .Font(IDetailLayoutBuilder::GetDetailFont())
						   .OptionsSource(&TargetTypeNames)
						   .InitiallySelectedItem(GetInitialTargetTypeName())
						   .IsEnabled_Lambda([]()
						   {
							   const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>();
							   if(!ToolSubsystemLocal){return true;}

								   if(ToolSubsystemLocal->GetODToolSettings()->FinishingType.Equals("Batch"))
								   {
									   return true;
								   }
								   return false;
						   	
						   })
						   .OnSelectionChanged_Lambda([this](const TSharedPtr<FString>& String, ESelectInfo::Type)
						   {
								UE_LOG(LogTemp,Warning,TEXT("Selection Changed %s"),**String.Get());

								   GetToolSubsystem()->GetODToolSettings()->TargetType = *String.Get();
						   })
						]
					]
				]
			]
			
			//BUTTONS
			+ SVerticalBox::Slot()
			//.HAlign(EHorizontalAlignment::HAlign_Fill)
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				BuildCreationToolbar()
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				BuildSpawnCenterToolbar()
			]
		]
	]
	
	//PALETTE SETUP
	+ SVerticalBox::Slot()
	.AutoHeight()
	//.Padding(StandardLeftPadding)
	.Padding(6.f,0.f,6.f,3.f)
	[
		SNew(SHeader)
		//.Visibility(this, &SFoliageEdit::GetVisibility_Options)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("PaletteHeader", "Palette"))
			.Font(StandardFont)
			.ColorAndOpacity(FLinearColor::White)
		]
	]
	
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBox)
		.MinDesiredHeight(64.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.OnClicked_Lambda([this]() -> FReply
			{
				OnPaletteBackgroundPressedSignature.ExecuteIfBound();
				return FReply::Handled();
			})
			[
				SNew(SAssetDropTarget)
				.OnAreAssetsAcceptableForDrop_UObject(this, &UODPaletteSlate::OnAreAssetsValidForDrop)
				.OnAssetsDropped_UObject(this,&UODPaletteSlate::HandleAssetsDropped)
				.bSupportsMultiDrop(true)
				[
					SAssignNew(MyWrapBox,SWrapBox)
					.UseAllottedSize(!bExplicitWrapSize)
					.PreferredSize(WrapSize)
					.HAlign(HorizontalAlignment)
					.Orientation(Orientation)
				]
			]
		]	
	]
	
	+SVerticalBox::Slot()
	.AutoHeight()
	[
		PaletteObjectDataDetails->TakeWidget()
	]

	+SVerticalBox::Slot()
	.AutoHeight()
	[
		PropDistributionDetails->TakeWidget()
	];
	
	
	for (UPanelSlot* PanelSlot : Slots)
	{
		if ( UWrapBoxSlot* TypedSlot = Cast<UWrapBoxSlot>(PanelSlot) )
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyWrapBox.ToSharedRef());
		}
	}
	
	return NewWidget;
}



void UODPaletteSlate::SpawnDensityChanged(float InDensity)
{
	GetToolSubsystem()->ChangeSlotObjectDensity(InDensity);

	SpawnDensity = InDensity;
}

bool UODPaletteSlate::IsEnabled_SpawnDensity() const
{
	const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystemLocal){return false;}

	return !ToolSubsystemLocal->SelectedPaletteSlotIndexes.IsEmpty();
}

TOptional<float>  UODPaletteSlate::GetSpawnDensityValue() const
{
	return SpawnDensity;
}

//Collision Test
void UODPaletteSlate::SetCollisionTestCheckState(const bool InNewState)
{
	GetToolSubsystem()->GetODToolSettings()->bTestForCollider = InNewState;
	GetToolSubsystem()->GetODToolSettings()->SaveConfig();
}

bool UODPaletteSlate::GetCollisionTestCheckState()
{
	return GetToolSubsystem()->GetODToolSettings()->bTestForCollider;
}

bool UODPaletteSlate::IsEnabled_CollisionTest() const
{
	const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystemLocal){return false;}
	
	return ToolSubsystemLocal->GetODToolSettings()->bTestForCollider;
}

void UODPaletteSlate::OnCollisionTestChanged(int32 InNewValue)
{
	GetToolSubsystem()->GetODToolSettings()->MaxCollisionTest = InNewValue;
	GetToolSubsystem()->GetODToolSettings()->SaveConfig();
}

TOptional<int32> UODPaletteSlate::GetCollisionTestValue() const
{
	const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystemLocal){return false;}
	
	return ToolSubsystemLocal->GetODToolSettings()->MaxCollisionTest;
}

void UODPaletteSlate::OnKillZChanged(float InNewValue)
{
	GetToolSubsystem()->GetODToolSettings()->KillZ = InNewValue;
	GetToolSubsystem()->GetODToolSettings()->SaveConfig();
}

TOptional<float> UODPaletteSlate::GetKillZValue() const
{
	const auto ToolSubsystemLocal = GEditor->GetEditorSubsystem<UODToolSubsystem>();
	if(!ToolSubsystemLocal){return false;}
	
	return ToolSubsystemLocal->GetODToolSettings()->KillZ;
}

void UODPaletteSlate::OnObjectSelectionChanged()
{
	SpawnDensity = 0.5f;
}

void UODPaletteSlate::OnTotalSpawnCountChanged(const float& InSpawnCount)
{
	TotalSpawnCount->SetText(FText::FromString(FString::FromInt(InSpawnCount)));
}

void UODPaletteSlate::OnDistributionRegenerated(const int32& InCollidingObjects) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Colliding Objects %d"),InCollidingObjects);
}

TSharedPtr<FString> UODPaletteSlate::GetInitialFinishingTypeName()
{
	const auto FinishingType = GetToolSubsystem()->GetODToolSettings()->FinishingType;
	
	if(FinishingType.Equals("Keep"))
	{
		if(FinishingTypeNames.IsValidIndex(0)){return FinishingTypeNames[0];}
	}
	else if(FinishingType.Equals("Batch"))
	{
		if(FinishingTypeNames.IsValidIndex(0)){return FinishingTypeNames[1];}
	}
	else
	{
		if(FinishingTypeNames.IsValidIndex(0)){return FinishingTypeNames[2];}
	}
	return {};
}

bool UODPaletteSlate::OnAreAssetsValidForDrop(TArrayView<FAssetData> DraggedAssets) const //Static Mesh Filter
{
	for (const FAssetData& AssetData : DraggedAssets)
	{
		if (AssetData.GetClass()->GetName().Equals(TEXT("StaticMesh")))
		{
			continue;
		}
		return false;
	}
	return true;
}

void UODPaletteSlate::HandleAssetsDropped(const FDragDropEvent& DragDropEvent, TArrayView<FAssetData> DraggedAssets) const
{
	OnAssetsDropped.ExecuteIfBound(DraggedAssets);
}

void UODPaletteSlate::OnResetParameters()
{
	const auto& FinishingType = GetToolSubsystem()->GetODToolSettings()->FinishingType;
	for(auto CurrentType : FinishingTypeNames)
	{
		if(CurrentType.Get()->Equals(FinishingType))
		{
			FinishingTypeComboBox->SetSelectedItem(CurrentType);
			break;
		}
	}

	const auto& TargetType = GetToolSubsystem()->GetODToolSettings()->TargetType;
	for(auto CurrentType : FinishingTypeNames)
	{
		if(CurrentType.Get()->Equals(TargetType))
		{
			TargetTypeComboBox->SetSelectedItem(CurrentType);
			break;
		}
	}
}


TSharedPtr<FString> UODPaletteSlate::GetInitialTargetTypeName()
{
	const auto FinishingType = GetToolSubsystem()->GetODToolSettings()->TargetType;
	
	if(FinishingType.Equals("SM Component"))
	{
		if(TargetTypeNames.IsValidIndex(0)){return TargetTypeNames[0];}
	}
	else if(FinishingType.Equals("ISM Component"))
	{
		if(TargetTypeNames.IsValidIndex(0)){return TargetTypeNames[1];}
	}
	else
	{
		if(TargetTypeNames.IsValidIndex(0)){return TargetTypeNames[2];}
	}
	return {};
}

void UODPaletteSlate::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyWrapBox->SetInnerSlotPadding(InnerSlotPadding);
	MyWrapBox->SetUseAllottedSize(!bExplicitWrapSize);
	MyWrapBox->SetWrapSize(WrapSize);
	MyWrapBox->SetHorizontalAlignment(HorizontalAlignment);
	MyWrapBox->SetOrientation(Orientation);
}

void UODPaletteSlate::SetInnerSlotPadding(FVector2D InPadding)
{
	InnerSlotPadding = InPadding;

	if ( MyWrapBox.IsValid() )
	{
		MyWrapBox->SetInnerSlotPadding(InPadding);
	}
}

void UODPaletteSlate::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;

	if (MyWrapBox.IsValid())
	{
		MyWrapBox->SetHorizontalAlignment(InHorizontalAlignment);
	}
}

#if WITH_EDITOR

const FText UODPaletteSlate::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
