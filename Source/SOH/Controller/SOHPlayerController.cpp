// Fill out your copyright notice in the Description page of Project Settings.


#include "SOHPlayerController.h"
#include "SOH/Character/SOHPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ASOHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Player)
				Subsystem->AddMappingContext(IMC_Player, 0);
		}

	}
}

void ASOHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_Move)
			EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASOHPlayerController::Move);

		if (IA_Look)
			EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ASOHPlayerController::Look);

		if (IA_Run)
		{
			EnhancedInput->BindAction(IA_Run, ETriggerEvent::Started, this, &ASOHPlayerController::StartRun);
			EnhancedInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &ASOHPlayerController::StopRun);
		}

		if (IA_Crouch)
		{
			EnhancedInput->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ASOHPlayerController::ToggleCrouch);
		}

		if (IA_Interact)
		{
			EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASOHPlayerController::Interact);
		}
		
		if (IA_ToggleFlashlight)	
		{
			EnhancedInput->BindAction(IA_ToggleFlashlight, ETriggerEvent::Started, this, &ASOHPlayerController::ToggleFlashlight);
		}

		
	}
}

void ASOHPlayerController::Move(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->Move(Value);
	}
}

void ASOHPlayerController::Look(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->Look(Value);
	}
}

void ASOHPlayerController::StartRun(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->StartRun(Value);
	}
}

void ASOHPlayerController::StopRun(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->StopRun(Value);
	}
}

void ASOHPlayerController::ToggleCrouch(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->ToggleCrouch();
	}
}

void ASOHPlayerController::Interact(const FInputActionValue& Value)
{
	if(ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->Interact();
	}
}

void ASOHPlayerController::ToggleFlashlight(const FInputActionValue& Value)
{
	if(ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->ToggleFlashlight();
	}
	
}
