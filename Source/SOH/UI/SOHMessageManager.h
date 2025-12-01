#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SOHMessageWidget.h"
#include "SOHMessageManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOH_API USOHMessageManager : public UActorComponent
{
	GENERATED_BODY()


public:
	USOHMessageManager();

	// 생성할 메시지 위젯 클래스 (UMG에서 만든 BP를 여기로 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InfoMessage")
	TSubclassOf<USOHMessageWidget> MessageWidgetClass;

	// FText 버전 (UI/Text용으로 쓰기 좋음)
	UFUNCTION(BlueprintCallable, Category = "InfoMessage")
	void ShowMessageText(const FText& Body, float Duration = 1.5f);

protected:
	// 현재 화면에 떠 있는 메시지 위젯 (하나만 유지)
	UPROPERTY()
	USOHMessageWidget* CurrentWidget;
		
};
