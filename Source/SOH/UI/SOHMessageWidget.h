#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SOHMessageWidget.generated.h"

UCLASS()
class SOH_API USOHMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* BodyText;

	UFUNCTION(BlueprintCallable, Category = "InfoMessage")
	void InitMessage(const FText& InBody, float InDuration);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "InfoMessage")
	float Duration = 0.f;

	FTimerHandle DestroyTimer;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void RemoveSelf();

};
