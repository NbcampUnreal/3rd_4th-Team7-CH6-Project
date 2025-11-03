#include "SOHPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

ASOHPlayerCharacter::ASOHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//회전 설정
	bUseControllerRotationPitch = false;//컨트롤러의 피치 회전 사용 안함
	bUseControllerRotationYaw = false;//컨트롤러의 요 회전 사용 안함
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
}

void ASOHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_Player)
					Subsystem->AddMappingContext(IMC_Player, 0);
			}
		}
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


void ASOHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Move) EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASOHPlayerCharacter::Move);
		if (IA_Look) EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ASOHPlayerCharacter::Look);
		if (IA_Run)
		{
			EnhancedInput->BindAction(IA_Run, ETriggerEvent::Started, this, &ASOHPlayerCharacter::StartRun);
			EnhancedInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &ASOHPlayerCharacter::StopRun);
		}
	}
}
