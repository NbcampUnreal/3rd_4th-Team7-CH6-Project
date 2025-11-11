// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBModActorSlot.generated.h"

class UImage;
class UButton;


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnModActorSlotClickedSignature, int32, InSlotIndex,uint8,InSection);



UCLASS()
class MODULARBUILDING_API UMBModActorSlot : public UUserWidget
{
	GENERATED_BODY()
	
	uint8 Section = 0;
	int32 SlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialInterface;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TabSlotSelectBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotImage;

	UFUNCTION()
	void TabSlotSelectBtnPressed();

public:
	void SetSlotParams(const FAssetData& AssetData,const int32 InSlotIndex,const uint8 InSection);

	FOnModActorSlotClickedSignature OnModActorSlotClickedSignature;
	
};
