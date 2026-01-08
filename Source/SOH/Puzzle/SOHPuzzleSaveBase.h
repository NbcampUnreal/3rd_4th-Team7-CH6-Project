#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "SOHPuzzleSaveBase.generated.h"

UCLASS(Abstract)
class SOH_API ASOHPuzzleSaveBase
	: public ASOHInteractableActor
	, public ISOHSaveObjectInterface
{
	GENERATED_BODY()

public:
	ASOHPuzzleSaveBase();

protected:
	/** SaveGame에서 이 퍼즐을 식별하기 위한 ID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle|Save")
	FName WorldStateID;

	/** 퍼즐 해결 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle|State")
	bool bIsSolved = false;

public:
	/** 퍼즐 해결 시 BP에서 호출 */
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void MarkPuzzleSolved();
	
	virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
	virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;
};