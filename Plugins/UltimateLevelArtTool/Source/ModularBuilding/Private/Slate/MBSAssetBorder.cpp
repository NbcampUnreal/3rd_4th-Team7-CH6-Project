// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#include "Slate/MBSAssetBorder.h"

#include "Engine/StaticMesh.h"
#include "Rendering/DrawElements.h"

static FName SBorderTypeName("SMBAssetBorder");

SLATE_IMPLEMENT_WIDGET(SMBAssetBorder)
void SMBAssetBorder::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "BorderImage", BorderImageAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "BorderBackgroundColor", BorderBackgroundColorAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "DesiredSizeScale", DesiredSizeScaleAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ShowDisabledEffect", ShowDisabledEffectAttribute, EInvalidateWidgetReason::Paint);
}

SMBAssetBorder::SMBAssetBorder()
	: BorderImageAttribute(*this, FCoreStyle::Get().GetBrush( "Border" ))
	, BorderBackgroundColorAttribute(*this, FLinearColor::White)
	, DesiredSizeScaleAttribute(*this, FVector2D(1,1))
	, ShowDisabledEffectAttribute(*this, true)
	, bFlipForRightToLeftFlowDirection(false)
{
}

void SMBAssetBorder::Construct( const SMBAssetBorder::FArguments& InArgs )
{
	// Only do this if we're exactly an SBorder
	if ( GetType() == SBorderTypeName )
	{
		SetCanTick(false);
		bCanSupportFocus = false;
	}
	
	SetContentScale(InArgs._ContentScale);
	SetColorAndOpacity(InArgs._ColorAndOpacity);
	SetDesiredSizeScale(InArgs._DesiredSizeScale);
	SetShowEffectWhenDisabled(InArgs._ShowEffectWhenDisabled);

	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;

	SetBorderImage(InArgs._BorderImage);
	SetBorderBackgroundColor(InArgs._BorderBackgroundColor);
	SetForegroundColor(InArgs._ForegroundColor);

	if (InArgs._OnMouseButtonDown.IsBound())
	{
		SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
	}

	if (InArgs._OnMouseButtonUp.IsBound())
	{
		SetOnMouseButtonUp(InArgs._OnMouseButtonUp);
	}

	if (InArgs._OnMouseMove.IsBound())
	{
		SetOnMouseMove(InArgs._OnMouseMove);
	}

	if (InArgs._OnMouseDoubleClick.IsBound())
	{
		SetOnMouseDoubleClick(InArgs._OnMouseDoubleClick);
	}

	ChildSlot
	.HAlign(InArgs._HAlign)
	.VAlign(InArgs._VAlign)
	.Padding(InArgs._Padding)
	[
		SNew(SAssetDropTarget)
		.OnAreAssetsAcceptableForDrop(this, &SMBAssetBorder::OnAreAssetsValidForDrop)
		.OnAssetsDropped(this, &SMBAssetBorder::HandlePlacementDropped)
		.bSupportsMultiDrop(true)
		[
			InArgs._Content.Widget
		]
	];
}

void SMBAssetBorder::SetContent( TSharedRef< SWidget > InContent )
{
	ChildSlot
	[
		SNew(SAssetDropTarget)
		.OnAreAssetsAcceptableForDrop(this, &SMBAssetBorder::OnAreAssetsValidForDrop)
		.OnAssetsDropped(this, &SMBAssetBorder::HandlePlacementDropped)
		.bSupportsMultiDrop(true)
		[
			InContent
		]
	];
}

const TSharedRef< SWidget >& SMBAssetBorder::GetContent() const
{
	return ChildSlot.GetWidget();
}

void SMBAssetBorder::ClearContent()
{
	ChildSlot.DetachWidget();
}

int32 SMBAssetBorder::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
	const FSlateBrush* BrushResource = BorderImageAttribute.Get();
		
	const bool bEnabled = ShouldBeEnabled(bParentEnabled);

	if ( BrushResource && BrushResource->DrawAs != ESlateBrushDrawType::NoDrawType )
	{
		const bool bShowDisabledEffect = GetShowDisabledEffect();
		const ESlateDrawEffect DrawEffects = (bShowDisabledEffect && !bEnabled) ? ESlateDrawEffect::DisabledEffect : ESlateDrawEffect::None;

		if (bFlipForRightToLeftFlowDirection && GSlateFlowDirection == EFlowDirection::RightToLeft)
		{
			const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(-1, 1)));
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				FlippedGeometry.ToPaintGeometry(),
				BrushResource,
				DrawEffects,
				BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * BorderBackgroundColorAttribute.Get().GetColor(InWidgetStyle)
			);
		}
		else
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				BrushResource,
				DrawEffects,
				BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * BorderBackgroundColorAttribute.Get().GetColor(InWidgetStyle)
			);
		}
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bEnabled );
}

FVector2D SMBAssetBorder::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return DesiredSizeScaleAttribute.Get() * Super::ComputeDesiredSize(LayoutScaleMultiplier);
}

bool SMBAssetBorder::OnAreAssetsValidForDrop(TArrayView<FAssetData> DraggedAssets) const //Static Mesh Filter
{
	for (const FAssetData& AssetData : DraggedAssets)
	{
		if (AssetData.GetClass() == UStaticMesh::StaticClass())
		{
			continue;
		}
		return false;
	}
	
	return true;
}

void SMBAssetBorder::HandlePlacementDropped(const FDragDropEvent& DragDropEvent, TArrayView<FAssetData> DraggedAssets) const
{
	AssetDroppedSignature.ExecuteIfBound(DraggedAssets);
		
}

void SMBAssetBorder::SetBorderBackgroundColor(TAttribute<FSlateColor> InColorAndOpacity)
{
	BorderBackgroundColorAttribute.Assign(*this, InColorAndOpacity);
}

void SMBAssetBorder::SetDesiredSizeScale(TAttribute<FVector2D> InDesiredSizeScale)
{
	DesiredSizeScaleAttribute.Assign(*this, InDesiredSizeScale);
}

void SMBAssetBorder::SetHAlign(EHorizontalAlignment HAlign)
{
	ChildSlot.SetHorizontalAlignment(HAlign);
}

void SMBAssetBorder::SetVAlign(EVerticalAlignment VAlign)
{
	ChildSlot.SetVerticalAlignment(VAlign);
}

void SMBAssetBorder::SetPadding(TAttribute<FMargin> InPadding)
{
	ChildSlot.SetPadding(MoveTemp(InPadding));
}

void SMBAssetBorder::SetShowEffectWhenDisabled(TAttribute<bool> InShowEffectWhenDisabled)
{
	ShowDisabledEffectAttribute.Assign(*this, InShowEffectWhenDisabled);
}

void SMBAssetBorder::SetBorderImage(TAttribute<const FSlateBrush*> InBorderImage)
{
	BorderImageAttribute.Assign(*this, InBorderImage);
}
