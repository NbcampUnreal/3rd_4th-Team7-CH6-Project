#include "Level/SOHJumpScare.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

ASOHJumpScare::ASOHJumpScare()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(Root);
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnBox->SetCollisionObjectType(ECC_WorldDynamic);
	SpawnBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpawnBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RemoveBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RemoveBox"));
	RemoveBox->SetupAttachment(Root);
	RemoveBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RemoveBox->SetCollisionObjectType(ECC_WorldDynamic);
	RemoveBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RemoveBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnArrow"));
	SpawnArrow->SetupAttachment(Root);
	SpawnArrow->SetArrowSize(1.0f);
	SpawnArrow->SetHiddenInGame(false);
}

void ASOHJumpScare::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnBox)
	{
		SpawnBox->OnComponentBeginOverlap.AddDynamic(this, &ASOHJumpScare::OnSpawnBoxBeginOverlap);
	}

	if (RemoveBox)
	{
		RemoveBox->OnComponentBeginOverlap.AddDynamic(this, &ASOHJumpScare::OnRemoveBoxBeginOverlap);
	}
}

void ASOHJumpScare::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DashTimerHandle);
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	GetWorldTimerManager().ClearTimer(MontageTimerHandle);

	CleanupAudioComps();
	Super::EndPlay(EndPlayReason);
}

bool ASOHJumpScare::IsPlayer(AActor* OtherActor) const
{
	return OtherActor && (OtherActor == UGameplayStatics::GetPlayerCharacter(this, 0));
}

ACharacter* ASOHJumpScare::GetPlayerChar() const
{
	return UGameplayStatics::GetPlayerCharacter(this, 0);
}

void ASOHJumpScare::OnSpawnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsPlayer(OtherActor))
		return;

	if (bSpawnOnlyOnce && bSpawnTriggered)
		return;

	SpawnCharacter();
}

void ASOHJumpScare::OnRemoveBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsPlayer(OtherActor))
		return;

	if (bRemoveOnlyOnce && bRemoveTriggered)
		return;

	TriggerRemoveAction();
}

void ASOHJumpScare::SpawnCharacter()
{
	bSpawnTriggered = true;

	if (!SpawnClass || !SpawnArrow)
		return;

	if (IsValid(SpawnedCharacter))
		return;

	const FTransform SpawnTM = SpawnArrow->GetComponentTransform();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedCharacter = GetWorld()->SpawnActor<ACharacter>(SpawnClass, SpawnTM, Params);

	StartSpawnSFXAttached();
}

void ASOHJumpScare::StartSpawnSFXAttached()
{
	if (IsValid(SpawnAudioComp))
	{
		SpawnAudioComp->Stop();
		SpawnAudioComp->DestroyComponent();
		SpawnAudioComp = nullptr;
	}

	if (!IsValid(SpawnedCharacter) || !SpawnSFX)
		return;

	SpawnAudioComp = UGameplayStatics::SpawnSoundAttached(
		SpawnSFX,
		SpawnedCharacter->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset,
		true
	);

	if (IsValid(SpawnAudioComp))
	{
		SpawnAudioComp->bAutoDestroy = !bSpawnSFXLoop;
	}
}

void ASOHJumpScare::StopSpawnSFX()
{
	if (IsValid(SpawnAudioComp))
	{
		SpawnAudioComp->Stop();
		SpawnAudioComp->DestroyComponent();
		SpawnAudioComp = nullptr;
	}
}

void ASOHJumpScare::PlayActionSFXAttached()
{
	if (!ActionSFX || !SpawnedCharacter) return;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		ActionSFX,
		SpawnedCharacter->GetActorLocation()
	);
}

void ASOHJumpScare::TriggerRemoveAction()
{
	bRemoveTriggered = true;

	if (!IsValid(SpawnedCharacter))
		return;

	if (bStopSpawnSFXOnRemove)
		StopSpawnSFX();

	PlayActionSFXAttached();

	if (!MontageToPlay)
	{
		DestroySpawned();
		Destroy();
		return;
	}

	GetWorldTimerManager().ClearTimer(DashTimerHandle);
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	GetWorldTimerManager().ClearTimer(MontageTimerHandle);

	if (JumpScareLifetime > 0.f)
	{
		GetWorldTimerManager().SetTimer(LifetimeTimerHandle, [this]()
			{
				GetWorldTimerManager().ClearTimer(DashTimerHandle);
				DestroySpawned();
				Destroy();
			}, JumpScareLifetime, false);
	}

	if (bLaunchTowardPlayer)
	{
		if (ACharacter* Player = GetPlayerChar())
		{
			FVector Dir = Player->GetActorLocation() - SpawnedCharacter->GetActorLocation();
			Dir.Z = 0.f;
			Dir = Dir.GetSafeNormal();

			if (UCharacterMovementComponent* Move = SpawnedCharacter->GetCharacterMovement())
			{
				Move->SetMovementMode(MOVE_Walking);

				const FVector DashVel = Dir * DashSpeed;

				if (DashDuration > 0.f)
				{
					GetWorldTimerManager().SetTimer(DashTimerHandle, [this, DashVel]()
						{
							if (!IsValid(SpawnedCharacter))
								return;

							if (UCharacterMovementComponent* M = SpawnedCharacter->GetCharacterMovement())
							{
								M->Velocity = DashVel;
							}
						}, DashTickInterval, true);

					FTimerHandle DashStopHandle;
					GetWorldTimerManager().SetTimer(DashStopHandle, [this]()
						{
							GetWorldTimerManager().ClearTimer(DashTimerHandle);
						}, DashDuration, false);
				}
				else
				{
					Move->Velocity = DashVel;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(MontageTimerHandle, [this]()
		{
			if (!IsValid(SpawnedCharacter))
				return;

			if (USkeletalMeshComponent* Mesh = SpawnedCharacter->GetMesh())
			{
				if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
				{
					const float Played = AnimInst->Montage_Play(MontageToPlay, 1.0f);
					if (Played > 0.f)
					{
						FOnMontageEnded EndDelegate;
						EndDelegate.BindUObject(this, &ASOHJumpScare::OnMontageEnded);
						AnimInst->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
					}
				}
			}
		}, 0.05f, false);
}

void ASOHJumpScare::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bDestroyOnMontageEnd)
		return;

	if (MontageToPlay && Montage != MontageToPlay)
		return;

	GetWorldTimerManager().ClearTimer(DashTimerHandle);
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	GetWorldTimerManager().ClearTimer(MontageTimerHandle);

	DestroySpawned();
	Destroy();
}

void ASOHJumpScare::CleanupAudioComps()
{
	if (IsValid(ActionAudioComp))
	{
		ActionAudioComp->Stop();
		ActionAudioComp->DestroyComponent();
		ActionAudioComp = nullptr;
	}

	if (IsValid(SpawnAudioComp))
	{
		SpawnAudioComp->Stop();
		SpawnAudioComp->DestroyComponent();
		SpawnAudioComp = nullptr;
	}
}

void ASOHJumpScare::DestroySpawned()
{
	CleanupAudioComps();

	if (IsValid(SpawnedCharacter))
	{
		SpawnedCharacter->Destroy();
	}
	SpawnedCharacter = nullptr;
}