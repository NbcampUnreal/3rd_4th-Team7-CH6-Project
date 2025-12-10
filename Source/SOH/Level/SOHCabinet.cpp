#include "SOHCabinet.h"
#include "SOH/Character/SOHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "SOHJumpScareBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ASOHCabinet::ASOHCabinet()
{
	PrimaryActorTick.bCanEverTick = false;

	HidePoint = CreateDefaultSubobject<USceneComponent>(TEXT("HidePoint"));
	HidePoint->SetupAttachment(RootComponent);

	if (InteractionRange)
		InteractionRange->SetupAttachment(HidePoint);

	if (UIAnchor)
	{
		UIAnchor->SetupAttachment(DoorFrame);
		UIAnchor->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	}

	HideCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HideCamera"));
	HideCamera->SetupAttachment(HidePoint);
}

void ASOHCabinet::Interact_Implementation(AActor* Caller)
{
	ASOHPlayerCharacter* Player = Cast<ASOHPlayerCharacter>(Caller);
	if (!Player || bIsMoving) return;

	if (bIsHidden)
	{
		if (!bIsOpen)
		{
			bPendingExit = true;
			Super::Interact_Implementation(Caller);
		}
		else
		{
			ExitCabinet(Player);
		}
		return;
	}

	if (!bIsOpen)
	{
		Super::Interact_Implementation(Caller);

		if (bUseCabinetJumpScare && !bJumpScareTriggered && CabinetJumpScare)
		{
			CabinetJumpScare->StartJumpScare(Player);
			bJumpScareTriggered = true;
			return;
		}

		bPendingEnter = true;
		return;
	}

	bPendingEnter = true;
}

void ASOHCabinet::NotifyDoorMoveFinished(bool bNowOpen)
{
	Super::NotifyDoorMoveFinished(bNowOpen);

	bIsOpen = bNowOpen;
	bIsMoving = false;

	if (!bNowOpen)
	{
		bPendingEnter = false;
		bPendingExit = false;
		return;
	}

	if (bPendingExit)
	{
		bPendingExit = false;

		ASOHPlayerCharacter* Player =
			Cast<ASOHPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		if (Player) ExitCabinet(Player);
		return;
	}

	if (bPendingEnter)
	{
		bPendingEnter = false;

		ASOHPlayerCharacter* Player =
			Cast<ASOHPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		if (Player) EnterCabinet(Player);
	}
}

void ASOHCabinet::EnterCabinet(ASOHPlayerCharacter* Player)
{
	if (!Player || !HidePoint) return;

	CachedPlayerLocation = Player->GetActorLocation();
	CachedPlayerRotation = Player->GetActorRotation();

	Player->SetActorLocation(HidePoint->GetComponentLocation());
	Player->SetActorRotation(HidePoint->GetComponentRotation());

	if (UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}

	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		FViewTargetTransitionParams Params;
		Params.BlendTime = 0.3f;
		PC->SetViewTarget(this, Params);
	}

	bIsHidden = true;
}

void ASOHCabinet::ExitCabinet(ASOHPlayerCharacter* Player)
{
	Player->SetActorLocation(CachedPlayerLocation);
	Player->SetActorRotation(CachedPlayerRotation);

	if (UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		FViewTargetTransitionParams Params;
		Params.BlendTime = 0.3f;
		PC->SetViewTarget(Player, Params);
	}

	bIsHidden = false;
	bIsOpen = false;
}