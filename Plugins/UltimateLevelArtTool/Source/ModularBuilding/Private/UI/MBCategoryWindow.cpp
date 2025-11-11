// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "UI/MBCategoryWindow.h"
#include "Components/VerticalBox.h"
#include "Data/MBModularAssetData.h"
#include "UI/MBTypeWindow.h"
#include "Editor.h"
#include "Components/Button.h"
#include "MBToolAssetData.h"
#include "MBToolSubsystem.h"
#include "Development/MBDebug.h"
#include "Components/Spacer.h"
#include "Data/MBToolData.h"
#include "Engine/DataTable.h"
#include "Components/EditableTextBox.h"

void UMBCategoryWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(!AddNewTypeText->OnTextCommitted.IsBound())
	{
		AddNewTypeText->OnTextCommitted.AddDynamic(this,&UMBCategoryWindow::OnAddNewTypeTextCommitted);
	}
}

void UMBCategoryWindow::CreateTypeMenus()
{
	TypeWindows.Empty();
	
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();

	TArray<FName> Types; 
	if(Category == EBuildingCategory::Modular)
	{
		Types  = ToolSettings->GetToolData()->ModularMeshTypes;
	}
	else
	{
		Types  = ToolSettings->GetToolData()->PropMeshTypes;
	}
	
	for(auto Type : Types)
	{
		//Create New TypeMenu & Add To TypeMenu
		const TSoftClassPtr<UUserWidget> WidgetClassPtr(MBToolAssetData::TypeMenuPath);
		if(const auto ClassRef = WidgetClassPtr.LoadSynchronous())
		{
			if (const auto NewTypeMenu = Cast<UMBTypeWindow>(CreateWidget(EditorWorld, ClassRef)))
			{
				NewTypeMenu->SetMenuType(Type);
				CategoryMenuBox->AddChildToVerticalBox(NewTypeMenu);
				TypeWindows.Add(Type,NewTypeMenu);
				AddASpacer();
			}
		}
	}
}


void UMBCategoryWindow::RegenerateTheCategoryMenu(const EBuildingCategory BuildingCategory)
{
	const UMBToolSubsystem* ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();
	if(!ToolSettings || !ToolSettings->GetModularAssetData()){return;}
	
	Category = BuildingCategory;
	CategoryMenuBox->ClearChildren();
	
	CreateTypeMenus();
	AddASpacer();
	
	TArray<FName> ModRowNames = ToolSettings->GetModularAssetData()->GetRowNames();

	if(ModRowNames.Num() != 0 || TypeWindows.Num() != 0)
	{
		for (const auto ModRowName : ModRowNames)
		{
			if(const auto FoundRow = ToolSettings->GetModularAssetData()->FindRow<FModularBuildingAssetData>(ModRowName, "", true))
			{
				if(FoundRow->AssetReference.IsNull() || !FoundRow->AssetReference.LoadSynchronous())
				{
					//ToolSettings->RemoveNullAssetRow(ModRowName);
					continue;
				}
				
				if(FoundRow->MeshCategory == BuildingCategory)
				{
					if(TypeWindows.Contains(FoundRow->MeshType))
					{
						if(const auto TypeWindow = TypeWindows.FindChecked(FoundRow->MeshType))
						{
							TypeWindow->AddSlotToMenu(FoundRow->AssetReference.LoadSynchronous());
						}
					}
				}
			}
		}
	}
}

void UMBCategoryWindow::ResetSlotSelectionStates()
{
	if(TypeWindows.IsEmpty()){return;}

	TArray<FName> Keys;
	TypeWindows.GetKeys(Keys);
	
	for(auto LocalKey : Keys)
	{
		TypeWindows[LocalKey]->ResetAllSelectionStates();
	}
}

void UMBCategoryWindow::OnAddNewTypeTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(Text.IsEmpty()){return;}
	
	const auto ToolSettings = GEditor->GetEditorSubsystem<UMBToolSubsystem>();

	if(ToolSettings->AddNewMeshType(FName(*Text.ToString()),Category))
	{
		const FString Dialog = FString::Printf(TEXT("%s added as a new asset type succesfully."),*Text.ToString());
		MBDebug::ShowNotifyInfo(Dialog);
	}
	RegenerateTheCategoryMenu(Category);
	AddNewTypeText->SetText(FText());
}

void UMBCategoryWindow::AddASpacer()
{
	const FName NewSpacerName = MakeUniqueObjectName(this, USpacer::StaticClass(), FName(TEXT("Spacer")));
	USpacer* NewSpacer = Cast<USpacer>(NewObject<USpacer>(this, NewSpacerName));
	NewSpacer->SetSize(FVector2D(0.0f,5.0f));
	CategoryMenuBox->AddChildToVerticalBox(NewSpacer);
}


void UMBCategoryWindow::NativeDestruct()
{
	if (AddNewTypeText) { AddNewTypeText->OnTextCommitted.RemoveAll(this); }
	
	Super::NativeDestruct();
}