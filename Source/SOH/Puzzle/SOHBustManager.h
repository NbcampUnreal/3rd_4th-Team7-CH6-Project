#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHBust.h"
#include "SOHBustManager.generated.h"

UCLASS()
class SOH_API ASOHBustManager : public AActor
{
	GENERATED_BODY()
    
public:
	ASOHBustManager();

	// Tick에서 퍼즐 상태 체크
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Puzzle")
	bool bIsLocked = false;

protected:
	virtual void BeginPlay() override;

	// 퍼즐 완료 시 호출
	void CheckPuzzleSolved();

	// 퍼즐이 완성되면 true
	bool bPuzzleSolved = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<ASOHBust*> BustPieces;

	// 정답 위치 배열 (예: 1,2,3,0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<int32> TargetPositions;	

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* SolvedSound;
	
};
