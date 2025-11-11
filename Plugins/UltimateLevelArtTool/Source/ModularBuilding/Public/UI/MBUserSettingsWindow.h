// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MBUserSettingsWindow.generated.h"

class UMBUserSettingsObject;
class UDetailsView;
class UButton;

UCLASS()
class MODULARBUILDING_API UMBUserSettingsWindow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UMBUserSettingsObject> UserSettingsObject;
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetToolBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearTheSceneBtn;
	
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget),Category="Modular Building")
	TObjectPtr<UDetailsView> UserSettingsDetails;

	UFUNCTION()
	void ResetToolBtnPressed();

	UFUNCTION()
	void ClearTheSceneBtnPressed();
	
};
