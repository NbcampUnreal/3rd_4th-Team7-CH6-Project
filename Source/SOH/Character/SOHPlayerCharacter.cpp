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

	// ============================
	// LOAD GAME RESTORE (추가)
	// ============================
	if (USOHGameInstance* GI = GetGameInstance<USOHGameInstance>())
	{
		if (GI->bLoadedFromSave)
		{
			SetActorTransform(GI->LoadedPlayerTransform);
			Health = GI->LoadedHealth;
			Stamina = GI->LoadedStamina;
			UpdateOverlay(Health, MaxHealth);
		}
	}
}

void ASOHPlayerCharacter::TraceForInteractable()
{
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	if (!Camera) return;

	// ========== 하이브리드: 캐릭터 앞에서 시작, 카메라 방향 사용 ==========
	FVector CameraForward = Camera->GetForwardVector();
	FVector Start = GetActorLocation() + (CameraForward * 100.f) + FVector(0, 0, 90.f);
	FVector End = Start + (CameraForward * TraceDistance);
	// ====================================================================

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(50.f),
		Params
	);
	
	//디버그
	//DrawDebugSphere(GetWorld(), Start, 50.f, 12, FColor::Green, false, 0.1f);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f);

	AActor* BestActor = nullptr;
	float BestScore = -999999.f;

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || !HitActor->Implements<USOHInteractInterface>())
				continue;

			FVector ToActor = (HitActor->GetActorLocation() - Start).GetSafeNormal();
			float DotProduct = FVector::DotProduct(CameraForward, ToActor);
			float Distance = FVector::Dist(Start, HitActor->GetActorLocation());
			float Score = DotProduct / (Distance * 0.001f);

			if (Score > BestScore)
			{
				BestScore = Score;
				BestActor = HitActor;
			}
		}
	}

	bUIHit = (BestActor != nullptr);

	if (LastHighlightedItem != BestActor)
	{
		if (LastHighlightedItem)
		{
			if (LastHighlightedItem->Implements<USOHInteractInterface>())
			{
				ISOHInteractInterface::Execute_CanReceiveTrace(LastHighlightedItem, this, false);
			}
		}

		LastHighlightedItem = nullptr;

		if (BestActor)
		{
			ISOHInteractInterface::Execute_CanReceiveTrace(BestActor, this, true);
			LastHighlightedItem = BestActor;
		}
	}
}

void ASOHPlayerCharacter::Move(const FInputActionValue& Value)
{
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
			if (bIsExhausted && Stamina >= MaxStamina * 0.5f)
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

void ASOHPlayerCharacter::OpenUI(UUserWidget* NewUI, FName UIType)
{
	if (bIsUIOpen && CurrentOpenUI && CurrentUIType != UIType)
	{
		// 기존 UI 닫기
		CloseUI();
	}

	if (NewUI)
	{
		CurrentOpenUI = NewUI;
		CurrentUIType = UIType;
		CurrentOpenUI->AddToViewport();
		bIsUIOpen = true;

		// Pause UI일 때만 시간 정지
		if (UIType == FName("Pause") || UIType == FName("Inventory") || UIType == FName("Map"))
		{
			UGameplayStatics::SetGlobalTimeDilation(this, 0.0f);
		}

		// 입력 모드 변경
		if (APlayerController* PC = GetController<APlayerController>())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(CurrentOpenUI->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void ASOHPlayerCharacter::CloseUI()
{
	if (CurrentOpenUI)
	{
		CurrentOpenUI->RemoveFromParent();
		CurrentOpenUI = nullptr;
	}

	// 시간 재개
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

	CurrentUIType = NAME_None;
	bIsUIOpen = false;

	// 입력 모드 복구
	if (APlayerController* PC = GetController<APlayerController>())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

// 특정 UI만 닫기
bool ASOHPlayerCharacter::CloseSpecificUI(FName UIType)
{
	if (CurrentUIType == UIType)
	{
		CloseUI();
		return true;
	}
	return false;
}