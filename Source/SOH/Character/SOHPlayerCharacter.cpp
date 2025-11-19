#include "SOHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "SOH/Item/SOHFlashlight.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASOHPlayerCharacter::ASOHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//회전 설정
	bUseControllerRotationPitch = false;//컨트롤러의 피치 회전 사용 안함
	bUseControllerRotationYaw = true;//컨트롤러의 요 회전 사용 안함
	bUseControllerRotationRoll = false;//컨트롤러의 롤 회전 사용 안함

	GetCharacterMovement()->bOrientRotationToMovement = true;                //이동 방향으로 회전
	GetCharacterMovement()->RotationRate = FRotator(0.f, RotationRate, 0.f); //회전 속도 및 이동 속도 설정
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;                        //걷기 속도 설정

	//카메라 컴포넌트
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;            // 카메라 거리
	SpringArm->bUsePawnControlRotation = true;     // 캐릭터 회전과 따로 회전
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 SpringArm 회전을 따름

	Tags.Add(FName("Player"));//플레이어 태그 
}

void ASOHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 0.1초마다 감지 실행 (초당 10회)
	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ASOHPlayerCharacter::TraceForInteractable, 0.1f, true);
}

void ASOHPlayerCharacter::TraceForInteractable()
{
	// 카메라 위치와 방향 가져오기
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	if (!Camera) return;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * 30.0f); // 감지 거리 30cm

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	AActor* HitActor = nullptr;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		HitActor = HitResult.GetActor();
	}
	// 디버그 라인 그리기
	
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		bHit ? FColor::Red : FColor::Green,
		false,   // 지속 시간 유지 여부
		0.05f,   // 지속 시간 (초)
		0,       // Depth priority
		1.5f     // 두께
	);
	

	// 이전 아이템과 다르면 이전 아웃라인 해제
	if (LastHighlightedItem && LastHighlightedItem != HitActor)
	{
		if (UStaticMeshComponent* ItemMesh = LastHighlightedItem->FindComponentByClass<UStaticMeshComponent>())
		{
			ItemMesh->SetRenderCustomDepth(false);
		}
		LastHighlightedItem = nullptr;
	}

	// 새로운 아이템이면 아웃라인 표시
	if (HitActor && HitActor->ActorHasTag("Item"))
	{
		if (UStaticMeshComponent* ItemMesh = HitActor->FindComponentByClass<UStaticMeshComponent>())
		{
			ItemMesh->SetRenderCustomDepth(true);
			ItemMesh->SetCustomDepthStencilValue(1); // 머티리얼에서 사용하는 스텐실 값
		}
		LastHighlightedItem = HitActor;
	}
}

void ASOHPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
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
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ASOHPlayerCharacter::StartRun(const FInputActionValue& Value)
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ASOHPlayerCharacter::StopRun(const FInputActionValue& Value)
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASOHPlayerCharacter::ToggleCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
		UE_LOG(LogTemp, Warning, TEXT("UnCrouch Called"));
	}
	else
	{
		Crouch();
		UE_LOG(LogTemp, Warning, TEXT("Crouch Called"));
	}
}

void ASOHPlayerCharacter::Interact()
{
	if (LastHighlightedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interacted with: %s"), *LastHighlightedItem->GetName());
		// 아이템과 상호작용 로직 추가
	}
}

void ASOHPlayerCharacter::ToggleFlashlight()
{
	if (Flashlight)
	{
		Flashlight->Toggle();

		// 손전등 켤 때 상체 몽타주 재생
		if (bFlashlightOn) // Toggle() 내부에서 상태가 바뀌는 걸 확인해야 함
		{
			PlayUpperBodyMontage(FlashlightMontage); // UpperBody 슬롯 사용
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Flashlight assigned!"));
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
