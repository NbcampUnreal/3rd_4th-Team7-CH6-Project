// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#if WITH_EDITOR

#include "Editor/ASDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "Spline/ASCustomClassSpline.h"
#include "Spline/ASDeformMeshSpline.h"
#include "Spline/ASStaticMeshDistributionSpline.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "UltimateLevelArtTool" 

/**************************************************************************************************/
/********************************* Deform Mesh Detail Customization ******************************/
/*************************************************************************************************/

void FASDeformMeshDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(("Tool Settings"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));

	TArray<TWeakObjectPtr<UObject>> CustomizeObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizeObjects);

	Category.AddCustomRow(LOCTEXT("RowSearchName","SnapSurface"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Snapping"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Snap To Surface"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip"," It snaps spline points to the closest surface below them."))
		.OnClicked_Lambda([CustomizeObjects]() -> FReply
		{
			for(auto Object : CustomizeObjects)
			{
				if(Object.IsValid() && Object->IsA(AASDeformMeshSpline::StaticClass()))
				{
					Cast<AASDeformMeshSpline>(Object)->OnSnapPointsToSurface();
				}
			}
			return FReply::Handled();
		})
	];
	
	Category.AddCustomRow(LOCTEXT("RowSearchName","MergeSpline"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Bake The Spline"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Run Merge Tool"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Run the internal merging tool for deformed spline mesh."))
		.OnClicked_Lambda([CustomizeObjects]() -> FReply
		{
			for(auto Object : CustomizeObjects)
			{
				if(Object.IsValid() && Object->IsA(AASDeformMeshSpline::StaticClass()))
				{
					Cast<AASDeformMeshSpline>(Object)->OnRunMergePressed();
				}
			}
			return FReply::Handled();
		})
	];
}

TSharedRef<IDetailCustomization> FASDeformMeshDetailCustomization::MakeInstance()
{
	return MakeShareable(new FASDeformMeshDetailCustomization);
}

/**************************************************************************************************/
/********************************* Custom Class Detail Customization ******************************/
/*************************************************************************************************/

void FASCustomClassDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(("Tool Settings"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
	TArray<TWeakObjectPtr<UObject>> CustomizeObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizeObjects);

	Category.AddCustomRow(LOCTEXT("RowSearchName","IsolateTheSpline"))
		.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Isolation"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SButton)
		.Text(LOCTEXT("ButtonText","Isalote The Spline"))
		.HAlign(HAlign_Center)
		.ToolTipText(LOCTEXT("ButtonTooltip","Isolate the spline and create actor(s) from existing elements."))
		.OnClicked_Lambda([CustomizeObjects]() -> FReply
		{
			for(auto Object : CustomizeObjects)
			{
				if(Object.IsValid() && Object->IsA(AASCustomClassSpline::StaticClass()))
				{
					Cast<AASCustomClassSpline>(Object)->OnIsolatePressed();
				}
			}
			return FReply::Handled();
		})
	];
}

TSharedRef<IDetailCustomization> FASCustomClassDetailCustomization::MakeInstance()
{
	return MakeShareable(new FASCustomClassDetailCustomization);
}

/**************************************************************************************************/
/******************************* Mesh Distribution Detail Customization ***************************/
/*************************************************************************************************/

void FASMeshDistributionDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(("Tool Settings"),FText::GetEmpty(),static_cast<ECategoryPriority::Type>(0));
	TArray<TWeakObjectPtr<UObject>> CustomizeObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizeObjects);

	Category.AddCustomRow(LOCTEXT("RowSearchName","Create"))
		.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DetailName","Create"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Padding(0,0,5,0)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Create Instance"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Creates an actor and copies the meshes into it \n according to the selected instance type."))
			.OnClicked_Lambda([CustomizeObjects]() -> FReply
			{
				for(auto Object : CustomizeObjects)
				{
					if(Object.IsValid() && Object->IsA(AASStaticMeshDistributionSpline::StaticClass()))
					{
						Cast<AASStaticMeshDistributionSpline>(Object)->OnSetFreePressed();
					}
				}
				return FReply::Handled();
			})
		]
		
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Padding(5,0,0,0)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText","Run Merge Tool"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip","Run the internal merging tool for spline mesh."))
			.OnClicked_Lambda([CustomizeObjects]() -> FReply
			{
				for(auto Object : CustomizeObjects)
				{
					if(Object.IsValid() && Object->IsA(AASStaticMeshDistributionSpline::StaticClass()))
					{
						Cast<AASStaticMeshDistributionSpline>(Object)->OnRunMergePressed();
					}
				}
				return FReply::Handled();
			})
		]
	];
}

TSharedRef<IDetailCustomization> FASMeshDistributionDetailCustomization::MakeInstance()
{
	return MakeShareable(new FASMeshDistributionDetailCustomization);
}

#undef LOCTEXT_NAMESPACE

#endif
