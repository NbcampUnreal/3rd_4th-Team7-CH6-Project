#include "SOHMessageWidget.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"

void USOHMessageWidget::InitMessage(const FText& InBody, float InDuration)
{
	Duration = InDuration;

	if (BodyText)
	{
		BodyText->SetText(InBody);
	}
}

void USOHMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Duration > 0.f)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				DestroyTimer,
				this,
				&USOHMessageWidget::RemoveSelf,
				Duration,
				false
			);
		}
	}
}

void USOHMessageWidget::RemoveSelf()
{
	RemoveFromParent();
}
