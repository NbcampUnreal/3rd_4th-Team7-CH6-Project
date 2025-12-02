#include "SOHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "SOH/Item/SOHFlashlight.h"
#include "Item/SOHBattery.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/SOHInteractInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SOHGameModeBase.h"
#include "GameMode/SOHGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Item/SOHInventoryComponent.h"
#include "Item/SOHItemDataStructs.h"
#include "Item/SOHItemManager.h"

ASOHPlayerCharacter::ASOHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;  // 추가!
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);  // 속도 조절
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Tags.Add(FName("Player"));

	Health = MaxHealth;
	Stamina = MaxStamina;
}

void ASOHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDClass)
	{
		PlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();

			UpdateOverlay(Health, MaxHealth);
		}
	}

	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ASOHPlayerCharacter::TraceForInteractable, 0.1f, true);

	// ============================
	// LOAD GAME RESTORE (추가)
	// ============================
	if (USOHGameInstance* GI = GetGameInstance<USOHGameInstance>())
	{
		if (GI->bLoadedFromSave)   // 로드한 상태인지 체크
		{
			SetActorTransform(GI->LoadedPlayerTransform);
			Health = GI->LoadedHealth;
			Stamina = GI->LoadedStamina;

			UpdateOverlay(Health, MaxHealth);  // UI도 업데이트
		}
	}
}

void ASOHPlayerCharacter::TraceForInteractable()
{
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	if (!Camera) return;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * TraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	AActor* HitActor = nullptr;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel3, Params);
	if (bHit)
	{
		HitActor = HitResult.GetActor();

		bUIHit = bHit;
	}
	else
	{
		bUIHit = false;
	}

	if (LastHighlightedItem != HitActor)
	{
		if (LastHighlightedItem)
		{
			if (LastHighlightedItem->Implements<USOHInteractInterface>())
			{
				ISOHInteractInterface::Execute_CanReceiveTrace(LastHighlightedItem, this, false);
			}
		}
		LastHighlightedItem = nullptr; 
		if (HitActor && HitActor->Implements<USOHInteractInterface>())
		{
			ISOHInteractInterface::Execute_CanReceiveTrace(HitActor, this, true);
			LastHighlightedItem = HitActor;
		}
	} 
}

void ASOHPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	// 현재 이동 방향 저장 (헤더에 변수 추가 필요)
	CurrentMoveInput = Axis;

	if (Controller)
	{
		// Forward
		if (Axis.Y != 0.f)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(FRotator(0.f, Rotation.Yaw, 0.f)).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Axis.Y);
		}
		// Right
		if (Axis.X != 0.f)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(FRotator(0.f, Rotation.Yaw, 0.f)).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Axis.X);
		}
	}
}

void ASOHPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	if (Controller)
	{
		// Pitch만 제한
		FRotator CurrentRotation = Controller->GetControlRotation();
		float NewPitch = FMath::ClampAngle(CurrentRotation.Pitch - Axis.Y, MinPitchAngle, MaxPitchAngle);

		FRotator NewRotation = CurrentRotation;
		NewRotation.Pitch = NewPitch;
		NewRotation.Yaw = CurrentRotation.Yaw + Axis.X;  // Yaw는 제한 없음

		Controller->SetControlRotation(NewRotation);
	}
}

void ASOHPlayerCharacter::StartRun()
{
	// 뒤로 가는 중이면 달리기 불가
	if (CurrentMoveInput.Y < 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't run backwards!"));
		return;
	}

	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ASOHPlayerCharacter::StopRun()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASOHPlayerCharacter::ToggleCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();

		// 일어섰을 때
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		// 타이머 정지
		GetWorldTimerManager().ClearTimer(CrouchMovementCheckTimer);

		UE_LOG(LogTemp, Warning, TEXT("UnCrouch Called"));
	}
	else
	{
		Crouch();

		// 앉았을 때
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		// 타이머 시작 (0.1초마다 체크)
		GetWorldTimerManager().SetTimer(CrouchMovementCheckTimer, this, &ASOHPlayerCharacter::CheckCrouchMovement, 0.1f, true);

		UE_LOG(LogTemp, Warning, TEXT("Crouch Called"));
	}
}

void ASOHPlayerCharacter::CheckCrouchMovement()
{
	if (!bIsCrouched) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;  // 수직 속도 제외

	// 움직이고 있으면 회전 활성화
	if (Velocity.SizeSquared() > 1.f)
	{
		if (!bUseControllerRotationYaw)
		{
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	// 멈춰있으면 회전 비활성화
	else
	{
		if (bUseControllerRotationYaw)
		{
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}
	}
}

void ASOHPlayerCharacter::Interact()
{
	if (LastHighlightedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interacted with: %s"), *LastHighlightedItem->GetName());
		ISOHInteractInterface::Execute_Interact(LastHighlightedItem, this);
	}
}

void ASOHPlayerCharacter::ToggleFlashlight()
{
	if (Flashlight)
	{
		Flashlight->Toggle();

		if (bFlashlightOn)
		{
			PlayUpperBodyMontage(FlashlightMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Flashlight assigned!"));
	}
}

void ASOHPlayerCharacter::UseBattery()
{
	if (!Flashlight) return;

	USOHInventoryComponent* InventoryComp = FindComponentByClass<USOHInventoryComponent>();
	if (!InventoryComp) return;

	TArray<FSOHInventoryItem> Items;
	InventoryComp->GetInventoryContents_BP(Items);

	FName BatteryItemID = NAME_None;

	USOHItemManager* ItemManager = GetGameInstance()->GetSubsystem<USOHItemManager>();
	if (!ItemManager) return;

	for (const FSOHInventoryItem& Item : Items)
	{
		FSOHItemTableRow Row;
		if (ItemManager->GetItemDataByID_BP(Item.itemID, Row))
		{
			if (Row.itemTags.Contains("Battery"))
			{
				BatteryItemID = Item.itemID;
				break;
			}
		}
	}

	if (BatteryItemID.IsNone()) return;

	FSOHItemTableRow BatteryData;
	if (!ItemManager->GetItemDataByID_BP(BatteryItemID, BatteryData))
		return;

	float Charge = BatteryData.value;
	float BeforePercent = Flashlight->GetBatteryPercent();
	Flashlight->UseBatteryItem(Charge);
	float AfterPercent = Flashlight->GetBatteryPercent();

	if (AfterPercent > BeforePercent)
	{
		InventoryComp->ConsumeItem(BatteryItemID, 1);
	}
}

void ASOHPlayerCharacter::PlayUpperBodyMontage(UAnimMontage* Montage)
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			if (Montage)
			{
				AnimInstance->Montage_Play(Montage);
			}
		}
	}
}

// 데미지 처리

float ASOHPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.f) return 0.f;

	Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Overlay Call: %f / %f"), Health, MaxHealth);

	UpdateOverlay(Health, MaxHealth);

	if (Health <= 0.f)
	{
		Die();
	}

	return ActualDamage;
}

// 죽음 처리

void ASOHPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	// 움직임/회전 멈추기
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 입력 막기
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);
	}

	// 죽음 몽타주 재생
	if (DeathMontage)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{
				Anim->Montage_Play(DeathMontage);
			}
		}
	}

	// 사운드 재생
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	FTimerHandle DeathDelayHandle;
	GetWorldTimerManager().SetTimer(
		DeathDelayHandle,
		this,
		&ASOHPlayerCharacter::CallGameModeOnPlayerDied,
		3.0f,
		false
	);
}

void ASOHPlayerCharacter::CallGameModeOnPlayerDied()
{
	if (UWorld* World = GetWorld())
	{
		if (ASOHGameModeBase* GM = Cast<ASOHGameModeBase>(UGameplayStatics::GetGameMode(World)))
		{
			GM->OnPlayerDied();
		}
	}
}