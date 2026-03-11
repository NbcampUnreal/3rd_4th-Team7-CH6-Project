#include "GameMode/SOHCutscenePlayerBase.h"

#include "Blueprint/UserWidget.h"

// Sets default values
void ACutscenePlayerBase::PlayCutscene_Implementation()
{
	
}

void ACutscenePlayerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}