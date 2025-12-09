#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHBust.h"
#include "SOHBustManager.generated.h"

class ASOHSlidingDoor;
class UBoxComponent;

UCLASS()
class SOH_API ASOHBustManager : public AActor
{
	GENERATED_BODY()
    
public:
	ASOHBustManager();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Puzzle")
	bool bIsLocked = false;

protected:
	virtual void BeginPlay() override;

	void CheckPuzzleSolved();

	bool bPuzzleSolved = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<ASOHBust*> BustPieces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<int32> TargetPositions;	

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* SolvedSound;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* EnterTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<ASOHSlidingDoor*> TargetDoors;

	UFUNCTION()
	void OnEnterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
