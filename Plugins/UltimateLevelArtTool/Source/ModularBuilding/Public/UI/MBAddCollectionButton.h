// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBAddCollectionButton.generated.h"

class UButton;

UCLASS()
class MODULARBUILDING_API UMBAddCollectionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddCollectionBtn;

	UFUNCTION()
	void AddCollectionBtnPressed();
	
	
};
