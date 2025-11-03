#include "SOHCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

ASOHCharacter::ASOHCharacter()
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

void ASOHCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASOHCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)//컨트롤러가 있고 입력값이 0이 아닐 때
	{
		const FRotator Rotation = Controller->GetControlRotation();//컨트롤러의 회전값 가져오기
		const FRotator YawRotation(0, Rotation.Yaw, 0);//피치와 롤을 0으로 설정하여 요 회전만 사용
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASOHCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ASOHCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);//컨트롤러의 요 회전 입력 추가
}

void ASOHCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASOHCharacter::StartRun()
{
	bIsRunning = true;//달리기 상태로 변경
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ASOHCharacter::StopRun()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASOHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)//입력 컴포넌트 설정
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASOHCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASOHCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ASOHCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASOHCharacter::LookUp);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASOHCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASOHCharacter::StopRun);
}
