// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MBModularEnum.h"
#include "MBSettingMenu.generated.h"

class UContentWidget;

UCLASS()
class MODULARBUILDING_API UMBSettingMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	ESettingsMenuType ActiveSettingsMenu = ESettingsMenuType::None;

	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveWidget;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UContentWidget> ContentBox;

public:
	void RegenerateTheSettings() const;


};
