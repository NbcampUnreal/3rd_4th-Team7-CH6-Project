#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHCutscenePlayerBase.generated.h"

UCLASS(Abstract)
class SOH_API ACutscenePlayerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	TSoftClassPtr<UUserWidget> CutScenePlayerWidgetClass;

	// 생성된 위젯을 추적하기 위한 변수
	UPROPERTY(Transient)
	UUserWidget* ActiveWidget;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Cutscene")
	void PlayCutscene();

	// 레벨이 언로드 될 때 위젯을 정리하기 위한 오버라이드
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

