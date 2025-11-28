#include "SOHNoiseSound.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/Actor.h"

USOHNoiseSound::USOHNoiseSound()
{
    Sound = nullptr;

    Loudness = 1.0f;
    MaxRange = 0.0f;

    NoiseTag = "GenericNoise";

    bUseOwnerLocation = true;

    SocketName = NAME_None;
    LocationOffset = FVector::ZeroVector;
}

void USOHNoiseSound::PlaySoundWithNoise(UObject* WorldContextObject, USoundBase* InSound, FVector InLocation, float InLoudness, float InMaxRange, FName InNoiseTag, AActor* Instigator)
{
    if (!WorldContextObject) return;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    if (InSound)
    {
        UGameplayStatics::PlaySoundAtLocation(WorldContextObject, InSound, InLocation);
    }

    UAISense_Hearing::ReportNoiseEvent(World, InLocation, InLoudness, Instigator, InMaxRange, InNoiseTag);
}

void USOHNoiseSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    if (!MeshComp) return;

    UWorld* World = MeshComp->GetWorld();
    if (!World) return;

    AActor* Owner = MeshComp->GetOwner();

    // 위치 계산
    FVector NoiseLocation = FVector::ZeroVector;

    if (!SocketName.IsNone() && MeshComp->DoesSocketExist(SocketName))
    {
        NoiseLocation = MeshComp->GetSocketLocation(SocketName);
    }
    else
    {
        if (bUseOwnerLocation && Owner)
        {
            NoiseLocation = Owner->GetActorLocation();
        }
        else
        {
            NoiseLocation = MeshComp->GetComponentLocation();
        }
    }

    NoiseLocation += LocationOffset;

    USOHNoiseSound::PlaySoundWithNoise(MeshComp, Sound, NoiseLocation, Loudness, MaxRange, NoiseTag, Owner);
}
