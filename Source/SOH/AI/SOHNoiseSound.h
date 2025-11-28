#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SOHNoiseSound.generated.h"

UCLASS()
class SOH_API USOHNoiseSound : public UAnimNotify
{
    GENERATED_BODY()

public:
    USOHNoiseSound();

    // 재생할 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise")
    USoundBase* Sound;

    // 소리 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise")
    float Loudness;

    // 최대 감지 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise")
    float MaxRange;

    // 태그
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise")
    FName NoiseTag;

    // 위치 옵션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise|Location")
    bool bUseOwnerLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise|Location")
    FName SocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Noise|Location")
    FVector LocationOffset;

    // 애님 노티파이 호출
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    // 일반 PlaySound용 함수
    UFUNCTION(BlueprintCallable, Category = "AI Noise", meta = (WorldContext = "WorldContextObject"))
    static void PlaySoundWithNoise(
        UObject* WorldContextObject,
        USoundBase* InSound,
        FVector InLocation,
        float InLoudness,
        float InMaxRange,
        FName InNoiseTag,
        AActor* Instigator);
};