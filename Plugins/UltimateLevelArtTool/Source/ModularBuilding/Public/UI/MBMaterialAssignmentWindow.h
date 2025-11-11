// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBMaterialAssignmentWindow.generated.h"

UCLASS()
class MODULARBUILDING_API UMBMaterialAssignmentWindow : public UUserWidget
{
	GENERATED_BODY()

public:	
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> MaterialMainBox;
	
	bool RedesignTheMaterialWindow() const;

private:



};
