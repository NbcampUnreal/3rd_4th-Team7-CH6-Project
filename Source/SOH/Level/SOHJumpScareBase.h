#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHJumpScareBase.generated.h"

class UBoxComponent;
class UCameraComponent;
class UArrowComponent;

UCLASS()
class SOH_API ASOHJumpScareBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASOHJumpScareBase();

protected:
	virtual void BeginPlay() override;

protected:
    // 트리거 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpScare|Components")
    UBoxComponent* TriggerBox;

    // 점프스퀘어 연출용 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpScare|Components")
    UCameraComponent* JumpScareCamera;

    // 연출용 액터 스폰 위치
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpScare|Components")
    UArrowComponent* ScareSpawnPoint;

    // 재생할 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumpscare|Animation")
    UAnimMontage* JumpScareMontage = nullptr;

public:
    // 트리거 박스 오버랩만으로 자동 발동할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    bool bUseOverlapTrigger = true;

    // Pause
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    bool bPauseGameDuringJumpScare = false;

    // 플레이어 입력 잠글지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    bool bLockPlayerInput = true;

    // 전용 카메라 쓸지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    bool bUseJumpScareCamera = true;

    // 스폰할 액터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    TSubclassOf<AActor> ScareActorClass;

    // SetViewTargetWithBlend 블렌드 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    float CameraBlendTime = 0.2f;

    // 연출 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    float JumpScareDuration = 2.0f;

    // 한 번만 실행할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpScare|Options")
    bool bOneShot = true;

protected:
    // 점프스퀘어 진행 중인지
    UPROPERTY(BlueprintReadOnly, Category = "JumpScare|State")
    bool bIsJumpScarePlaying = false;

    // 한 번이라도 발동했는지
    UPROPERTY(BlueprintReadOnly, Category = "JumpScare|State")
    bool bAlreadyTriggered = false;

    // 플레이어 / 컨트롤러 저장
    TWeakObjectPtr<class APlayerController> CachedPlayerController;
    TWeakObjectPtr<class ACharacter>        CachedPlayerCharacter;

    // 스폰된 연출용 액터
    UPROPERTY(BlueprintReadOnly, Category = "JumpScare|State")
    TWeakObjectPtr<AActor> SpawnedScareActor;

    // 종료 타이머
    FTimerHandle JumpscareTimerHandle;

protected:
    // 트리거 오버랩 콜백
    UFUNCTION()
    void OnTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // C++/BP에서 시작 처리
    void InternalStartJumpScare(AActor* TriggeringActor);

    // C++/BP에서 종료 처리
    void InternalEndJumpScare();

public:
    // BP에서 호출 가능 함수
    UFUNCTION(BlueprintCallable, Category = "JumpScare")
    void StartJumpScare(AActor* TriggeringActor);

    // BP에서 호출 가능 함수
    UFUNCTION(BlueprintCallable, Category = "JumpScare")
    void FinishJumpScare();

public:
    // 점프스퀘어 시작
    UFUNCTION(BlueprintNativeEvent, Category = "JumpScare")
    void OnJumpScareStarted(AActor* TriggeringActor);
    virtual void OnJumpScareStarted_Implementation(AActor* TriggeringActor) {}

    // 점프스퀘어 끝
    UFUNCTION(BlueprintNativeEvent, Category = "JumpScare")
    void OnJumpScareEnded();
    virtual void OnJumpScareEnded_Implementation() {}
};