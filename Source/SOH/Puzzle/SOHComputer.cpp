#include "Puzzle/SOHComputer.h"
#include "Interaction/SOHInteractableActor.h"

void ASOHComputer::Interact_Implementation(AActor* Caller)
{
	Super::Interact_Implementation(Caller);
	
	// if (InteractionWidgetClass)
	// {
	// 	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), InteractionWidgetClass);
	// 	if (Widget)
	// 	{
	// 		Widget->AddToViewport();
	// 	}
	// }
}
