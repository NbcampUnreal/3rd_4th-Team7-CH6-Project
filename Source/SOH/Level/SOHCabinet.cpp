#include "SOHCabinet.h"
#include "SOH/Character/SOHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ASOHCabinet::ASOHCabinet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASOHCabinet::Interact_Implementation(AActor* Caller)
{
	if (bIsMoving) return;

	ASOHPlayerCharacter* Player = Cast<ASOHPlayerCharacter>(Caller);
	if (!Player)
	{
		Super::Interact_Implementation(Caller);
		return;
	}

	if (bIsHidden)
	{
		if (!bIsOpen)
		{
			bIsMoving = true;

			if (OpenSound)
				UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());

			BP_OpenDoor(Caller);
		}

		if (HideExitMontage)
		{
			Player->PlayUpperBodyMontage(HideExitMontage);
		}

		bIsHidden = false;
		return;
	}

	const bool bWasOpen = bIsOpen;

	Super::Interact_Implementation(Caller);

	if (!bWasOpen)
	{
		if (HideEnterMontage)
			Player->PlayUpperBodyMontage(HideEnterMontage);

		bIsHidden = true;
	}
}