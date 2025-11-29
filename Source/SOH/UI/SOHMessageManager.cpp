#include "SOHMessageManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

USOHMessageManager::USOHMessageManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentWidget = nullptr;
}

void USOHMessageManager::ShowMessageText(const FText& Body, float Duration)
{
	if (!MessageWidgetClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return;

	// 기존에 떠 있던 위젯이 있으면 제거
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	CurrentWidget = CreateWidget<USOHMessageWidget>(PC, MessageWidgetClass);
	if (CurrentWidget)
	{
		CurrentWidget->InitMessage(Body, Duration);
		CurrentWidget->AddToViewport();
	}
}