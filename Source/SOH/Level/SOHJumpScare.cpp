#include "Level/SOHJumpScare.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
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

void ASOHJumpScare::OnSpawnBoxBeginOverlap(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32,
	bool,
	const FHitResult&)
{
	if (!IsPlayer(OtherActor))
	{
		return;
	}

	if (bSpawnOnlyOnce && bSpawnTriggered)
	{
		return;
	}

	SpawnCharacter();
}

void ASOHJumpScare::OnRemoveBoxBeginOverlap(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32,
	bool,
	const FHitResult&)
{
	if (!IsPlayer(OtherActor))
	{
		return;
	}

	if (bRemoveOnlyOnce && bRemoveTriggered)
	{
		return;
	}

	TriggerRemoveAction();
}

void ASOHJumpScare::SpawnCharacter()
{
	bSpawnTriggered = true;

	if (!SpawnClass || !SpawnArrow) return;

	if (IsValid(SpawnedCharacter)) return;

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
	{
		return;
	}

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
	if (IsValid(ActionAudioComp))
	{
		ActionAudioComp->Stop();
		ActionAudioComp->DestroyComponent();
		ActionAudioComp = nullptr;
	}

	if (!IsValid(SpawnedCharacter) || !ActionSFX)
	{
		return;
	}

	ActionAudioComp = UGameplayStatics::SpawnSoundAttached(
		ActionSFX,
		SpawnedCharacter->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset,
		true
	);

	if (IsValid(ActionAudioComp))
	{
		ActionAudioComp->bAutoDestroy = true;
	}
}

void ASOHJumpScare::TriggerRemoveAction()
{
	bRemoveTriggered = true;

	if (!IsValid(SpawnedCharacter))
	{
		return;
	}

	if (bStopSpawnSFXOnRemove)
	{
		StopSpawnSFX();
	}

	PlayActionSFXAttached();

	if (!MontageToPlay)
	{
		DestroySpawned();
		return;
	}

	if (bLaunchTowardPlayer)
	{
		if (ACharacter* Player = GetPlayerChar())
		{
			FVector Dir = (Player->GetActorLocation() - SpawnedCharacter->GetActorLocation());
			Dir.Z = 0.f;
			Dir = Dir.GetSafeNormal();

			const FVector LaunchVel = Dir * LaunchStrength + FVector(0.f, 0.f, LaunchUpStrength);
			SpawnedCharacter->LaunchCharacter(LaunchVel, true, true);
		}
	}

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
				return;
			}
		}
	}

	DestroySpawned();
}

void ASOHJumpScare::OnMontageEnded(UAnimMontage* Montage, bool)
{
	if (MontageToPlay && Montage != MontageToPlay)
	{
		return;
	}

	DestroySpawned();
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