// Copyright 2023 - 2024 Leartes Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "MAToolSubsystem.generated.h"

UCLASS()
class MATERIALASSIGNMENT_API UMAToolSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TWeakObjectPtr<UObject> MAToolWindowRef;

public:
	FORCEINLINE void SetMAToolMainScreen(UObject* InMainScreenPtr) {MAToolWindowRef = InMainScreenPtr;}
	FORCEINLINE TWeakObjectPtr<UObject> GetMAMainScreen() const {return MAToolWindowRef;}
	FORCEINLINE void ReleaseMAToolMainScreenRef() {MAToolWindowRef = nullptr;}
	
};
