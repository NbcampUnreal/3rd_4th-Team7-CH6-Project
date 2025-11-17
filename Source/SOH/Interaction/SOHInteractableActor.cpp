
#include "Interaction/SOHInteractableActor.h"


ASOHInteractableActor::ASOHInteractableActor()
{
	PrimaryActorTick.bCanEverTick = true;

}
void ASOHInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASOHInteractableActor::Interact_Implementation(AActor* Caller)
{
}

bool ASOHInteractableActor::CanReceiveTrace_Implementation(AActor* Caller)
{
	return true;
}

