#include "SOHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
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
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동 방향으로만 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// 카메라는 자유롭게 회전
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;  // ← 마우스 따라감

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // ← SpringArm만 따라감

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

	// 타이머 시작
	GetWorldTimerManager().SetTimer(StaminaUpdateTimer, this, &ASOHPlayerCharacter::UpdateStamina, 0.1f, true);
	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ASOHPlayerCharacter::TraceForInteractable, 0.1f, true);

	// 숨소리 오디오 컴포넌트 생성
	if (HeavyBreathingSound)
	{
		BreathingAudioComponent = NewObject<UAudioComponent>(this);
		if (BreathingAudioComponent)
		{
			BreathingAudioComponent->SetSound(HeavyBreathingSound);
			BreathingAudioComponent->bAutoActivate = false;
			BreathingAudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			BreathingAudioComponent->RegisterComponent();
		}
	}
}

void ASOHPlayerCharacter::ApplyLoadedData()
{
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("⭐ ApplyLoadedData 시작"));
    
	USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GameInstance null!"));
		return;
	}

	if (!GI->bLoadedFromSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ bLoadedFromSave가 false - 적용 안 함"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("📍 현재 위치: %s"), *GetActorLocation().ToString());
	UE_LOG(LogTemp, Log, TEXT("📍 로드할 위치: %s"), *GI->LoadedPlayerTransform.GetLocation().ToString());

	// ⭐ 위치만 변경 (Transform 전체 말고!)
	SetActorLocation(GI->LoadedPlayerTransform.GetLocation());
	SetActorRotation(GI->LoadedPlayerTransform.GetRotation());

	// 스탯 복원
	Health = GI->LoadedHealth;
	Stamina = GI->LoadedStamina;

	// 인벤토리 복원
	if (USOHInventoryComponent* Inv = FindComponentByClass<USOHInventoryComponent>())
	{
		Inv->LoadInventory(GI->LoadedInventory);
	}

	UpdateOverlay(Health, MaxHealth);

	GI->bLoadedFromSave = false;
    
	UE_LOG(LogTemp, Warning, TEXT("✅ ApplyLoadedData 완료!"));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void ASOHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		AActor* ViewTarget = PC->GetViewTarget();

		bool bIsViewingOtherCamera = false;

		if (ViewTarget)
		{
			FString ViewTargetClassName = ViewTarget->GetClass()->GetName();

			if (!ViewTargetClassName.Contains("SOHPlayerCharacter"))
			{
				bIsViewingOtherCamera = true;
			}
		}

		if (bIsInInteractionCamera != bIsViewingOtherCamera)
		{
			bIsInInteractionCamera = bIsViewingOtherCamera;

			UE_LOG(LogTemp, Error, TEXT("===== CAMERA MODE: %s ====="),
				bIsInInteractionCamera ? TEXT("INTERACTION") : TEXT("NORMAL"));

			if (bIsInInteractionCamera)
			{
				bUIHit = false;
				RemovedWidgets.Empty();

				if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
				{
					TArray<UUserWidget*> AllWidgets;
					for (TObjectIterator<UUserWidget> It; It; ++It)
					{
						UUserWidget* Widget = *It;
						if (Widget && Widget->IsInViewport() && Widget->GetWorld() == GetWorld())
						{
							AllWidgets.Add(Widget);
						}
					}

					for (UUserWidget* Widget : AllWidgets)
					{
						// ⭐ 크로스헤어 UI만 제거 (이름으로 판별)
						FString WidgetName = Widget->GetClass()->GetName();
						if (WidgetName.Contains("Crosshair") || WidgetName.Contains("CrossHair"))
						{
							RemovedWidgets.Add(Widget);
							Widget->RemoveFromParent();
							UE_LOG(LogTemp, Warning, TEXT("Removed Crosshair: %s"), *WidgetName);
						}
					}

					UE_LOG(LogTemp, Error, TEXT("✅ Crosshair REMOVED (saved %d widgets)"), RemovedWidgets.Num());
				}

				// LastHighlightedItem 처리는 그대로 유지
				if (LastHighlightedItem)
				{
					if (LastHighlightedItem->Implements<USOHInteractInterface>())
					{
						ISOHInteractInterface::Execute_CanReceiveTrace(LastHighlightedItem, this, false);
					}
					LastHighlightedItem = nullptr;
				}
			}
			else
			{
				// ⭐ 저장했던 위젯들 모두 복원
				UE_LOG(LogTemp, Error, TEXT("Restoring %d widgets..."), RemovedWidgets.Num());

				for (UUserWidget* Widget : RemovedWidgets)
				{
					if (Widget && !Widget->IsInViewport())
					{
						Widget->AddToViewport();
						UE_LOG(LogTemp, Warning, TEXT("  - Restored: %s"), *Widget->GetName());
					}
				}

				RemovedWidgets.Empty();

				UE_LOG(LogTemp, Error, TEXT("✅ ALL HUD RESTORED"));
			}
		}
	}

	if (bIsUIOpen || bIsInInteractionCamera)
	{
		return;
	}

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float CurrentSpeed = Velocity.Size();

	if (CurrentSpeed < 1.0f)
	{
		if (AController* MyController = GetController())
		{
			FRotator CurrentRotation = GetActorRotation();
			FRotator TargetRotation = FRotator(0.f, MyController->GetControlRotation().Yaw, 0.f);

			FRotator NewRotation = FMath::RInterpTo(
				CurrentRotation,
				TargetRotation,
				DeltaTime,
				StandingRotationSpeed
			);

			SetActorRotation(NewRotation);
		}
	}
}

void ASOHPlayerCharacter::TraceForInteractable()
{
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		UE_LOG(LogTemp, Error, TEXT("Camera Component NOT FOUND!"));
		return;
	}

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * TraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_GameTraceChannel3,
		Params
	);

	AActor* HitActor = nullptr;

	if (bHit)
	{
		HitActor = HitResult.GetActor();

		// ⭐ Interface를 구현한 물체만 인식
		if (HitActor && !HitActor->Implements<USOHInteractInterface>())
		{
			// Interface 없으면 무시
			HitActor = nullptr;
		}
	}

	bUIHit = (HitActor != nullptr);

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

		if (HitActor)
		{
			ISOHInteractInterface::Execute_CanReceiveTrace(HitActor, this, true);
			LastHighlightedItem = HitActor;
		}
	}
}

void ASOHPlayerCharacter::Move(const FInputActionValue& Value)
{
	//if (bIsPlayingInteractMontage)
	//{
		//return;
	//}

	if (bIsUIOpen)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();

	// 현재 이동 방향 저장
	CurrentMoveInput = Axis;

	// 달리는 중에 뒤로 가려고 하면 달리기 취소
	if (bIsRunning && CurrentMoveInput.Y < 0.f)
	{
		StopRun();
		UE_LOG(LogTemp, Warning, TEXT("Sprint stopped - moving backwards"));
	}

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
	//if (bIsPlayingInteractMontage)
	//{
		//return;
	//}

	// ⭐ UI 열려있으면 무조건 리턴
	if (bIsUIOpen)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();

	if (Controller)
	{
		FRotator CurrentRotation = Controller->GetControlRotation();
		float NewPitch = FMath::ClampAngle(CurrentRotation.Pitch - Axis.Y, MinPitchAngle, MaxPitchAngle);

		FRotator NewRotation = CurrentRotation;
		NewRotation.Pitch = NewPitch;
		NewRotation.Yaw = CurrentRotation.Yaw + Axis.X;

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
	if (bIsExhausted || Stamina < MinStaminaToRun)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too exhausted to run!"));
		return;
	}

	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ASOHPlayerCharacter::StopRun()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	TimeSinceLastStaminaUse = 0.f;
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

//스태미너 업데이트
void ASOHPlayerCharacter::UpdateStamina()
{
	if (bIsDead) return;

	// 달리는 중이면 스태미너 소모
	if (bIsRunning)
	{
		Stamina -= StaminaDrainPerSec * 0.1f;
		Stamina = FMath::Clamp(Stamina, 0.f, MaxStamina);
		TimeSinceLastStaminaUse = 0.f;

		// 스태미너가 최소값 이하가 되면 탈진
		if (Stamina < MinStaminaToRun)
		{
			Stamina = 0.f;
			bIsExhausted = true;
			bCanSprint = false;
			StopRun();
			OnExhausted();
		}

		// ============================
		// 낮은 스태미너 상태에서 헥헥대기
		// ============================
		if (Stamina <= LowStaminaThreshold)
		{
			if (BreathingAudioComponent && !BreathingAudioComponent->IsPlaying())
			{
				StartHeavyBreathing();
			}
		}
	}
	// 달리지 않으면 회복
	else
	{
		TimeSinceLastStaminaUse += 0.1f;

		// 대기 시간이 지나면 회복 시작
		if (TimeSinceLastStaminaUse >= StaminaRegenDelay)
		{
			Stamina += StaminaRegenPerSec * 0.1f;
			Stamina = FMath::Clamp(Stamina, 0.f, MaxStamina);

			// 탈진 상태 해제 (50% 회복 시)
			if (bIsExhausted && Stamina >= MaxStamina * 0.2f)
			{
				bIsExhausted = false;
				bCanSprint = true;
				OnRecovered();
			}

			// ============================
			// 스태미너가 충분히 회복되면 숨소리 중지
			// ============================
			if (Stamina > LowStaminaThreshold)
			{
				if (BreathingAudioComponent && BreathingAudioComponent->IsPlaying())
				{
					StopHeavyBreathing();
				}
			}
		}
	}
}

void ASOHPlayerCharacter::OnExhausted()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is exhausted!"));

	// ============================
	// 탈진 사운드 재생
	// ============================
	if (ExhaustedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExhaustedSound, GetActorLocation());
	}
}

void ASOHPlayerCharacter::OnRecovered()
{
	UE_LOG(LogTemp, Log, TEXT("Player recovered from exhaustion"));
}

// ===================================
// 숨소리 관련 함수
// ===================================

void ASOHPlayerCharacter::StartHeavyBreathing()
{
	if (BreathingAudioComponent && !BreathingAudioComponent->IsPlaying())
	{
		BreathingAudioComponent->Play();
		UE_LOG(LogTemp, Log, TEXT("Heavy breathing started"));
	}
}

void ASOHPlayerCharacter::StopHeavyBreathing()
{
	if (BreathingAudioComponent && BreathingAudioComponent->IsPlaying())
	{
		BreathingAudioComponent->FadeOut(1.0f, 0.0f); // 1초에 걸쳐 페이드아웃
		UE_LOG(LogTemp, Log, TEXT("Heavy breathing stopped"));
	}
}

void ASOHPlayerCharacter::PlayFootstepSound()
{
    // 실제 이동 속도 체크
    FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;  // 수직 속도 제외
    float CurrentSpeed = Velocity.Size();
    
    // 실제로 움직이고 있을 때만 사운드 재생
    if (CurrentSpeed > MinFootstepSpeed)
    {
        if (FootstepSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this, 
                FootstepSound, 
                GetActorLocation()
            );
            
            UE_LOG(LogTemp, Log, TEXT("Footstep played (Speed: %.1f)"), CurrentSpeed);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Footstep blocked (Speed: %.1f < %.1f)"), 
            CurrentSpeed, MinFootstepSpeed);
    }
}

void ASOHPlayerCharacter::Interact()
{
	if (LastHighlightedItem)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance)
		{
			return;
		}

		// 이미 재생 중이면 무시
		if (bIsPlayingInteractMontage || AnimInstance->Montage_IsPlaying(InteractMontage))
		{
			UE_LOG(LogTemp, Warning, TEXT("Interact montage is already playing!"));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Interacted with: %s"), *LastHighlightedItem->GetName());

		if (InteractMontage)
		{
			bIsPlayingInteractMontage = true;

			PlayUpperBodyMontage(InteractMontage);

			// 몽타주 종료 시 콜백
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &ASOHPlayerCharacter::OnInteractMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, InteractMontage);
		}

		ISOHInteractInterface::Execute_Interact(LastHighlightedItem, this);
	}
}

void ASOHPlayerCharacter::OnInteractMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsPlayingInteractMontage = false;
	UE_LOG(LogTemp, Warning, TEXT("Interact montage ended, input restored"));
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

	// ⭐ 피격 애니메이션 재생 (죽지 않았을 때만)
	if (Health > 0.f && HitReactionMontage)
	{
		PlayHitReaction();
	}

	if (Health <= 0.f)
	{
		Die();
	}

	return ActualDamage;
}

void ASOHPlayerCharacter::PlayHitReaction()
{
	// 이미 재생 중이거나 죽었으면 무시
	if (bIsPlayingHitReaction || bIsDead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HitReactionMontage) return;

	bIsPlayingHitReaction = true;

	// 상체만 재생 
	PlayUpperBodyMontage(HitReactionMontage);

	// 몽타주 종료 콜백
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASOHPlayerCharacter::OnHitReactionEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactionMontage);

	UE_LOG(LogTemp, Log, TEXT("Hit reaction played!"));
}

void ASOHPlayerCharacter::OnHitReactionEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsPlayingHitReaction = false;
	UE_LOG(LogTemp, Log, TEXT("Hit reaction ended"));
}

void ASOHPlayerCharacter::UseHealthItem()
{
	if (bIsDead) return;

	// 이미 체력이 가득 찬 경우
	if (Health >= MaxHealth)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health is already full!"));
		return;
	}

	if (bIsPlayingHealMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already using health item!"));
		return;
	}

	USOHInventoryComponent* InventoryComp = FindComponentByClass<USOHInventoryComponent>();
	if (!InventoryComp) return;

	TArray<FSOHInventoryItem> Items;
	InventoryComp->GetInventoryContents_BP(Items);

	FName HealthItemID = NAME_None;

	USOHItemManager* ItemManager = GetGameInstance()->GetSubsystem<USOHItemManager>();
	if (!ItemManager) return;

	// 인벤토리에서 회복 아이템 찾기
	for (const FSOHInventoryItem& Item : Items)
	{
		FSOHItemTableRow Row;
		if (ItemManager->GetItemDataByID_BP(Item.itemID, Row))
		{
			// "Health" 또는 "Healing" 태그를 가진 아이템 찾기
			if (Row.itemTags.Contains("Health"))
			{
				HealthItemID = Item.itemID;
				break;
			}
		}
	}

	if (HealthItemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("No health item in inventory!"));
		return;
	}

	// 아이템 데이터 가져오기
	FSOHItemTableRow HealthData;
	if (!ItemManager->GetItemDataByID_BP(HealthItemID, HealthData))
		return;

	if (HealMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			bIsPlayingHealMontage = true;

			PlayUpperBodyMontage(HealMontage);

			// 몽타주 종료 콜백
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &ASOHPlayerCharacter::OnHealMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, HealMontage);

			UE_LOG(LogTemp, Log, TEXT("Heal montage started"));
		}
	}

	// 체력 회복
	float HealAmount = HealthData.value;
	float BeforeHealth = Health;

	Heal(HealAmount);

	float AfterHealth = Health;

	// 실제로 회복되었으면 아이템 소모
	if (AfterHealth > BeforeHealth)
	{
		InventoryComp->ConsumeItem(HealthItemID, 1);

		// 회복 사운드 재생 (있다면)
		if (HealSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HealSound, GetActorLocation());
		}

		UE_LOG(LogTemp, Log, TEXT("Healed %.1f HP (%.1f -> %.1f)"),
			AfterHealth - BeforeHealth, BeforeHealth, AfterHealth);
	}
}

void ASOHPlayerCharacter::OnHealMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsPlayingHealMontage = false;
	UE_LOG(LogTemp, Log, TEXT("Heal montage ended"));
}

void ASOHPlayerCharacter::Heal(float HealAmount)
{
	if (bIsDead) return;

	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);

	// HUD 업데이트 (붉은 테두리 제거)
	UpdateOverlay(Health, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Current Health: %.1f / %.1f"), Health, MaxHealth);
}

// 죽음 처리

void ASOHPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	// 움직임/회전 멈추기
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	// 스태미너 업데이트 타이머 정지
	GetWorldTimerManager().ClearTimer(StaminaUpdateTimer);
	// 숨소리 중지
	StopHeavyBreathing();
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

void ASOHPlayerCharacter::ToggleUI(FName UIType, TSubclassOf<UUserWidget> WidgetClass)
{
	// 같은 UI가 열려있으면 닫기
	if (bIsUIOpen && CurrentUIType == UIType)
	{
		CloseUI();
		return;
	}

	// 다른 UI가 열려있으면 닫기
	if (bIsUIOpen)
	{
		CloseUI();
	}

	// 새 UI 열기
	if (WidgetClass)
	{
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (NewWidget)
		{
			OpenUI(NewWidget, UIType);
		}
	}
}

// OpenUI 
void ASOHPlayerCharacter::OpenUI(UUserWidget* NewUI, FName UIType)
{
	if (bIsUIOpen && CurrentOpenUI && CurrentUIType != UIType)
	{
		CloseUI();
	}
	if (NewUI)
	{
		CurrentOpenUI = NewUI;
		CurrentUIType = UIType;
		CurrentOpenUI->AddToViewport();
		bIsUIOpen = true;

		if (UIOpenSound)
		{
			// 시간 정지 영향 안 받게
			UGameplayStatics::PlaySound2D(this, UIOpenSound, 1.0f, 1.0f, 0.0f, nullptr, nullptr, true);
		}

		//시간 정지
		if (UIType == FName("Pause") || UIType == FName("Inventory") || UIType == FName("Map"))
		{
			UGameplayStatics::SetGlobalTimeDilation(this, 0.01f);
		}

		// 입력 모드 변경
		if (APlayerController* PC = GetController<APlayerController>())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(CurrentOpenUI->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;

			// ⭐ 추가: Look/Move 입력 차단
			//PC->SetIgnoreLookInput(true);
			//PC->SetIgnoreMoveInput(true);
		}
	}
}

// CloseUI 
void ASOHPlayerCharacter::CloseUI()
{
	if (CurrentOpenUI)
	{
		CurrentOpenUI->RemoveFromParent();
		CurrentOpenUI = nullptr;
	}

	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
	if (UICloseSound)
	{
		UGameplayStatics::PlaySound2D(this, UICloseSound);
	}
	CurrentUIType = NAME_None;
	bIsUIOpen = false;

	if (APlayerController* PC = GetController<APlayerController>())
	{
		FInputModeGameOnly GameInputMode;
		PC->SetInputMode(GameInputMode);
		PC->bShowMouseCursor = false;

		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);
		PC->ResetIgnoreInputFlags();
	}
}

void ASOHPlayerCharacter::OnToggleInventory(const FInputActionValue& Value)
{
	// ⭐ 현재 인벤토리가 열려있는지 체크
	if (bIsUIOpen)
	{
		// 인벤토리가 열려있으면 무조건 닫기
		if (CurrentUIType == FName("Inventory"))
		{
			CloseUI();
			return;
		}

		// 다른 UI가 열려있으면 무시 (또는 경고)
		UE_LOG(LogTemp, Warning, TEXT("Another UI is open!"));
		return;
	}

	// UI가 안 열려있으면 인벤토리 열기
	ToggleUI(FName("Inventory"), InventoryMenuClass);
}

void ASOHPlayerCharacter::OnToggleMap(const FInputActionValue& Value)
{
	if (bIsUIOpen)
	{
		if (CurrentUIType == FName("Map"))
		{
			CloseUI();
			return;
		}
		return; // 다른 UI 열려있으면 무시
	}

	ToggleUI(FName("Map"), MapMenuClass);
}

void ASOHPlayerCharacter::OnTogglePause(const FInputActionValue& Value)
{
	if (bIsUIOpen)
	{
		if (CurrentUIType == FName("Pause"))
		{
			CloseUI();
			return;
		}
		return;
	}

	ToggleUI(FName("Pause"), PauseMenuClass);
}