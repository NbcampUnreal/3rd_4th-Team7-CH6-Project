#include "SOHPlayerController.h"
#include "SOH/Character/SOHPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameMode/SOHGameInstance.h"

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
	if (USOHGameInstance* GI = GetGameInstance<USOHGameInstance>())
	{
		// Save가 있든 없든 여기서 한 번만 적용
		GetWorld()->GetTimerManager().SetTimerForNextTick([GI]()
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[LOAD] ✅ PlayerController -> ApplyWorldState"));

			GI->ApplyWorldState();
		});
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

		if (IA_UseBattery)
		{
			EnhancedInput->BindAction(IA_UseBattery, ETriggerEvent::Triggered, this, &ASOHPlayerController::UseBattery);
		}

		if (IA_TogglePause)
		{
			EnhancedInput->BindAction(IA_TogglePause, ETriggerEvent::Started, this, &ASOHPlayerController::OnTogglePause);
		}

		if (IA_ToggleMap)
		{
			EnhancedInput->BindAction(IA_ToggleMap, ETriggerEvent::Started, this, &ASOHPlayerController::OnToggleMap);
		}

		if (IA_ToggleInventory)
		{
			EnhancedInput->BindAction(IA_ToggleInventory, ETriggerEvent::Started, this, &ASOHPlayerController::OnToggleInventory);
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
		MyChar->StartRun();
	}
}

void ASOHPlayerController::StopRun(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->StopRun();
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

void ASOHPlayerController::UseBattery(const FInputActionValue& Value)
{
	ASOHPlayerCharacter* PC = Cast<ASOHPlayerCharacter>(GetPawn());
	if (PC)
	{
		PC->UseBattery();
	}
}

void ASOHPlayerController::OnTogglePause(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->OnTogglePause(Value);
	}
}

void ASOHPlayerController::OnToggleMap(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->OnToggleMap(Value);
	}
}

void ASOHPlayerController::OnToggleInventory(const FInputActionValue& Value)
{
	if (ASOHPlayerCharacter* MyChar = Cast<ASOHPlayerCharacter>(GetPawn()))
	{
		MyChar->OnToggleInventory(Value);
	}
}