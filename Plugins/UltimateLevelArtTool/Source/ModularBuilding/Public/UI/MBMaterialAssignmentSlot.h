// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBMaterialAssignmentSlot.generated.h"

class UMaterialInstance;
class UButton;
class UImage;
class UMaterialInterface;

UCLASS()
class MODULARBUILDING_API UMBMaterialAssignmentSlot : public UUserWidget
{
	GENERATED_BODY()

	int32 SlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialInterface;
	
public:
	virtual void NativeConstruct() override;
virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MaterialSelectionBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MaterialImage;

	UFUNCTION()
	void MaterialSelectionBtnPressed();

public:
	void SetSlotParams(const FAssetData& AssetData,const int32 InSlotIndex);
	
};
