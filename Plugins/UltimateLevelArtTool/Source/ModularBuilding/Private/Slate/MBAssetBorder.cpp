// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Slate/MBAssetBorder.h"
#include "Slate/SlateBrushAsset.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Slate/MBSAssetBorder.h"
#include "Slate/MBBorderSlot.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UBorder

UMBAssetBorder::UMBAssetBorder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = false;

	ContentColorAndOpacity = FLinearColor::White;
	BrushColor = FLinearColor::White;

	Padding = FMargin(4, 2);

	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;

	DesiredSizeScale = FVector2D(1, 1);

	bShowEffectWhenDisabled = true;
}

void UMBAssetBorder::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyBorder.Reset();
}

TSharedRef<SWidget> UMBAssetBorder::RebuildWidget()
{
	MyBorder = SNew(SMBAssetBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection);
	
	if ( GetChildrenCount() > 0 )
	{
		Cast<UMBBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}

void UMBAssetBorder::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	TAttribute<FLinearColor> ContentColorAndOpacityBinding = PROPERTY_BINDING(FLinearColor, ContentColorAndOpacity);
	TAttribute<FSlateColor> BrushColorBinding = OPTIONAL_BINDING_CONVERT(FLinearColor, BrushColor, FSlateColor, ConvertLinearColorToSlateColor);
	TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Background, const FSlateBrush*, ConvertImage);
	
	MyBorder->SetPadding(Padding);
	MyBorder->SetBorderBackgroundColor(BrushColorBinding);
	MyBorder->SetColorAndOpacity(ContentColorAndOpacityBinding);

	MyBorder->SetBorderImage(ImageBinding);
	
	MyBorder->SetDesiredSizeScale(DesiredSizeScale);
	MyBorder->SetShowEffectWhenDisabled(bShowEffectWhenDisabled != 0);
	
	MyBorder->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
	MyBorder->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
	MyBorder->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
	MyBorder->SetOnMouseDoubleClick(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseDoubleClick));
}

UClass* UMBAssetBorder::GetSlotClass() const
{
	return UMBBorderSlot::StaticClass();
}

void UMBAssetBorder::OnSlotAdded(UPanelSlot* InSlot)
{
	// Copy the content properties into the new slot so that it matches what has been setup
	// so far by the user.
	UMBBorderSlot* BorderSlot = CastChecked<UMBBorderSlot>(InSlot);
	BorderSlot->Padding = Padding;
	BorderSlot->HorizontalAlignment = HorizontalAlignment;
	BorderSlot->VerticalAlignment = VerticalAlignment;

	// Add the child to the live slot if it already exists
	if ( MyBorder.IsValid() )
	{
		// Construct the underlying slot.
		BorderSlot->BuildSlot(MyBorder.ToSharedRef());
	}
}

void UMBAssetBorder::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetContent(SNullWidget::NullWidget);
	}
}

void UMBAssetBorder::SetContentColorAndOpacity(FLinearColor Color)
{
	ContentColorAndOpacity = Color;
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetColorAndOpacity(Color);
	}
}

void UMBAssetBorder::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetPadding(InPadding);
	}
}

void UMBAssetBorder::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetHAlign(InHorizontalAlignment);
	}
}

void UMBAssetBorder::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetVAlign(InVerticalAlignment);
	}
}

void UMBAssetBorder::SetBrushColor(FLinearColor Color)
{
	BrushColor = Color;
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetBorderBackgroundColor(Color);
	}
}

FReply UMBAssetBorder::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( OnMouseButtonDownEvent.IsBound() )
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UMBAssetBorder::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( OnMouseButtonUpEvent.IsBound() )
	{
		return OnMouseButtonUpEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UMBAssetBorder::HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( OnMouseMoveEvent.IsBound() )
	{
		return OnMouseMoveEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UMBAssetBorder::HandleMouseDoubleClick(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( OnMouseDoubleClickEvent.IsBound() )
	{
		return OnMouseDoubleClickEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

void UMBAssetBorder::SetBrush(const FSlateBrush& Brush)
{
	Background = Brush;

	if ( MyBorder.IsValid() )
	{
		MyBorder->SetBorderImage(&Background);
	}
}

void UMBAssetBorder::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	Background = Asset ? Asset->Brush : FSlateBrush();

	if ( MyBorder.IsValid() )
	{
		MyBorder->SetBorderImage(&Background);
	}
}

void UMBAssetBorder::SetBrushFromTexture(UTexture2D* Texture)
{
	Background.SetResourceObject(Texture);

	if ( MyBorder.IsValid() )
	{
		MyBorder->SetBorderImage(&Background);
	}
}

void UMBAssetBorder::SetBrushFromMaterial(UMaterialInterface* Material)
{
	if (!Material)
	{
		UE_LOG(LogSlate, Log, TEXT("UBorder::SetBrushFromMaterial.  Incoming material is null"));
	}

	Background.SetResourceObject(Material);
	
	if ( MyBorder.IsValid() )
	{
		MyBorder->SetBorderImage(&Background);
	}
}

UMaterialInstanceDynamic* UMBAssetBorder::GetDynamicMaterial()
{
	UMaterialInterface* Material = nullptr;

	UObject* Resource = Background.GetResourceObject();
	Material = Cast<UMaterialInterface>(Resource);

	if ( Material )
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);

		if ( !DynamicMaterial )
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			Background.SetResourceObject(DynamicMaterial);

			if ( MyBorder.IsValid() )
			{
				MyBorder->SetBorderImage(&Background);
			}
		}

		return DynamicMaterial;
	}
	return nullptr;
}

void UMBAssetBorder::SetDesiredSizeScale(FVector2D InScale)
{
	DesiredSizeScale = InScale;
	if (MyBorder.IsValid())
	{
		MyBorder->SetDesiredSizeScale(InScale);
	}
}

const FSlateBrush* UMBAssetBorder::ConvertImage(TAttribute<FSlateBrush> InImageAsset) const
{
	UMBAssetBorder* MutableThis = const_cast<UMBAssetBorder*>( this );
	MutableThis->Background = InImageAsset.Get();

	return &Background;
}

void UMBAssetBorder::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	if ( GetLinkerUEVersion() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS && Brush_DEPRECATED != nullptr )
	{
		Background = Brush_DEPRECATED->Brush;
		Brush_DEPRECATED = nullptr;
	}
#endif

	if ( GetChildrenCount() > 0 )
	{
		if ( UPanelSlot* PanelSlot = GetContentSlot() )
		{
			UMBBorderSlot* BorderSlot = Cast<UMBBorderSlot>(PanelSlot);
			if ( BorderSlot == NULL )
			{
				BorderSlot = NewObject<UMBBorderSlot>(this);
				BorderSlot->Content = GetContentSlot()->Content;
				BorderSlot->Content->Slot = BorderSlot;
				Slots[0] = BorderSlot;
			}
		}
	}
}

#if WITH_EDITOR

void UMBAssetBorder::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
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

			if ( UMBBorderSlot* BorderSlot = Cast<UMBBorderSlot>(GetContentSlot()) )
			{
				if (PropertyName == PaddingName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, PaddingName, BorderSlot, PaddingName);
				}
				else if (PropertyName == HorizontalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, HorizontalAlignmentName, BorderSlot, HorizontalAlignmentName);
				}
				else if (PropertyName == VerticalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, VerticalAlignmentName, BorderSlot, VerticalAlignmentName);
				}
			}
		}

		IsReentrant = false;
	}
}

const FText UMBAssetBorder::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif


#undef LOCTEXT_NAMESPACE


