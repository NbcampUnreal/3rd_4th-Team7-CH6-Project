#include "Puzzle/SOHComputer.h"
#include "Interaction/SOHInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h" 
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

ASOHComputer::ASOHComputer()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComputerMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
}

void ASOHComputer::Interact_Implementation(AActor* Caller)
{
	Super::Interact_Implementation(Caller);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetOverlayMaterial(nullptr);
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;
	
	PC->SetViewTargetWithBlend(this, 0.5);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&ASOHComputer::ShowComputerWidget,0.5f,false);
}

void ASOHComputer::ShowComputerWidget()
{
	if (!ComputerWidgetClass) return;
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), ComputerWidgetClass);
	if (!Widget) return;

	FProperty* Property = Widget->GetClass()->FindPropertyByName(TEXT("ComputerActorRef"));

	if (FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
	{
		ObjectProp->SetObjectPropertyValue_InContainer(Widget, this);
	}
	
	Widget->AddToViewport();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, Widget, EMouseLockMode::DoNotLock, false);
	PC->bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);

}
void ASOHComputer::SetPasswordSolved(bool bSolved)
{
	bIsPasswordSolved = bSolved;
}