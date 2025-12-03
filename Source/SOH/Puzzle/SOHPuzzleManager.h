#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHBust.h"
#include "SOHPuzzleManager.generated.h"

UCLASS()
class SOH_API ASOHPuzzleManager : public AActor
{
	GENERATED_BODY()
    
public:
	ASOHPuzzleManager();

	// Tick에서 퍼즐 상태 체크
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 퍼즐 완료 시 호출
	void CheckPuzzleSolved();

	// 퍼즐이 완성되면 true
	bool bPuzzleSolved = false;

	// 목표 Position (예: 0,0,0,0)
	UPROPERTY(EditAnywhere, Category="Puzzle")
	int32 TargetPosition = 0;

public:
	// 퍼즐에 포함될 조각상 4개
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	TArray<ASOHBust*> BustPieces;
};
