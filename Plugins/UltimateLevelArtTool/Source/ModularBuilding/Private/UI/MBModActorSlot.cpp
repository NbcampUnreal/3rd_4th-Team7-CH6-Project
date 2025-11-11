// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.


#include "UI/MBModActorSlot.h"
#include "MBAssetFunctions.h"
#include "AssetRegistry/AssetData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Modules/ModuleManager.h"

void UMBModActorSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TabSlotSelectBtn->OnClicked.IsBound())
	{
		TabSlotSelectBtn->OnClicked.AddDynamic(this, &UMBModActorSlot::TabSlotSelectBtnPressed);
	}
}

void UMBModActorSlot::NativeDestruct()
{
	if(TabSlotSelectBtn){TabSlotSelectBtn->OnClicked.RemoveAll(this);}
	
	Super::NativeDestruct();
}


void UMBModActorSlot::TabSlotSelectBtnPressed()
{
	OnModActorSlotClickedSignature.ExecuteIfBound(SlotIndex,Section);
	
}

void UMBModActorSlot::SetSlotParams(const FAssetData& AssetData, const int32 InSlotIndex,const uint8 InSection)
{
	SlotIndex = InSlotIndex;
	Section = InSection;
	
	MaterialInterface = Cast<UMaterialInterface>(AssetData.GetAsset());

	if(const auto GeneratedTexture = UMBAssetFunctions::GenerateThumbnailForAsset(AssetData,EMBAssetThumbnailFormant::PNG))
	{
		SlotImage->SetBrushFromTexture(GeneratedTexture);
	}
	TabSlotSelectBtn->SetToolTipText(FText::FromName(AssetData.AssetName));
}
