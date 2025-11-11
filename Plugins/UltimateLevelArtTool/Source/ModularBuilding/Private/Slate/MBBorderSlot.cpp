// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Slate/MBBorderSlot.h"
#include "Widgets/SNullWidget.h"
#include "Slate/MBSAssetBorder.h"
#include "Slate/MBAssetBorder.h"
#include "ObjectEditorUtils.h"

/////////////////////////////////////////////////////
// UBorderSlot

UMBBorderSlot::UMBBorderSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Padding = FMargin(4, 2);

	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
}

void UMBBorderSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Border.Reset();
}

void UMBBorderSlot::BuildSlot(TSharedRef<SMBAssetBorder> InBorder)
{
	Border = InBorder;
	Border.Pin()->SetPadding(Padding);
	Border.Pin()->SetHAlign(HorizontalAlignment);
	Border.Pin()->SetVAlign(VerticalAlignment);

	Border.Pin()->SetContent(Content ? Content->TakeWidget() : SNullWidget::NullWidget);
}

void UMBBorderSlot::SetPadding(FMargin InPadding)
{
	CastChecked<UMBAssetBorder>(Parent)->SetPadding(InPadding);
}

void UMBBorderSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	CastChecked<UMBAssetBorder>(Parent)->SetHorizontalAlignment(InHorizontalAlignment);
}

void UMBBorderSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	CastChecked<UMBAssetBorder>(Parent)->SetVerticalAlignment(InVerticalAlignment);
}

void UMBBorderSlot::SynchronizeProperties()
{
	if ( Border.IsValid() )
	{
		SetPadding(Padding);
		SetHorizontalAlignment(HorizontalAlignment);
		SetVerticalAlignment(VerticalAlignment);
	}
}

#if WITH_EDITOR

void UMBBorderSlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static bool IsReentrant = false;

	if ( !IsReentrant )
	{
		IsReentrant = true;

		if ( PropertyChangedEvent.Property )
		{
			static const FName PaddingName("Padding");
			static const FName HorizontalAlignmentName("HorizontalAlignment");
			static const FName VerticalAlignmentName("VerticalAlignment");

			FName PropertyName = PropertyChangedEvent.Property->GetFName();

			if ( UMBAssetBorder* ParentBorder = CastChecked<UMBAssetBorder>(Parent) )
			{
				if ( PropertyName == PaddingName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, PaddingName, ParentBorder, PaddingName);
				}
				else if ( PropertyName == HorizontalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, HorizontalAlignmentName, ParentBorder, HorizontalAlignmentName);
				}
				else if ( PropertyName == VerticalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, VerticalAlignmentName, ParentBorder, VerticalAlignmentName);
				}
			}
		}

		IsReentrant = false;
	}

	
}


#endif
