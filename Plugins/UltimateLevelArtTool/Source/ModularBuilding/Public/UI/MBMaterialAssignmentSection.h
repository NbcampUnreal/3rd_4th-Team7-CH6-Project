// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBMaterialAssignmentSection.generated.h"

class UMaterial;
class UWrapBox;
class UMaterialInstance;
class UTextBlock;
UCLASS()
class MODULARBUILDING_API UMBMaterialAssignmentSection : public UUserWidget
{
	GENERATED_BODY()

	static bool CheckForEligibility(const FString& InCroppedAssetName);
	
public:
	bool SetupMaterialAssignmentSlots(UMaterialInstance* InMaterial,const int32 InSlotIndex) const;

	virtual void NativeDestruct() override;
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> MaterialSlotContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotNameText;
};
