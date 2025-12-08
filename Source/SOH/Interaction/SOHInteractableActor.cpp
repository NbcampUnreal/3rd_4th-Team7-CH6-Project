#include "Interaction/SOHInteractableActor.h"
#include "Components/MeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ASOHInteractableActor::ASOHInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	InteractionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionRange"));
	InteractionRange->SetupAttachment(RootComponent);
	InteractionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionRange->InitBoxExtent(FVector(50.f));

	UIAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("UIAnchor"));
	UIAnchor->SetupAttachment(RootComponent);
	UIAnchor->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(UIAnchor);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);
}
void ASOHInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionWidget)
	{
		if (bEnableProximityText && InteractionWidgetClass)
		{
			InteractionWidget->SetWidgetClass(InteractionWidgetClass);
			InteractionWidget->SetDrawAtDesiredSize(true);
			InteractionWidget->SetDrawSize(WidgetSize);
			InteractionWidget->SetVisibility(false);
		}
		else
		{
			InteractionWidget->SetVisibility(false);
		}
	}

	if (InteractionRange)
	{
		InteractionRange->OnComponentBeginOverlap.AddDynamic(this, &ASOHInteractableActor::OnPlayerEnter);
		InteractionRange->OnComponentEndOverlap.AddDynamic(this, &ASOHInteractableActor::OnPlayerExit);
	}
}

void ASOHInteractableActor::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerEnter fired. OtherActor = %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	if (!bEnableProximityText) return;

	if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;

	ShowInteractWidget();
}

void ASOHInteractableActor::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(ACharacter::StaticClass())) return;

	HideInteractWidget();
}

void ASOHInteractableActor::Interact_Implementation(AActor* Caller)
{
	if (Caller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Base Interact called on %s by %s. No specific action defined in C++."), *GetName(), *Caller->GetName());
	}
}
bool ASOHInteractableActor::CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract)
{
	if (bCanInteract)
	{
		ApplyOverlayMaterial(OutlineMaterial);
		return true;
	}
	else
	{
		ApplyOverlayMaterial(nullptr);
		return false;
	}
}
void ASOHInteractableActor::ShowInteractWidget()
{
	 if (InteractionWidget)
	 {
	 	InteractionWidget->SetVisibility(true);
	 }
}
void ASOHInteractableActor::HideInteractWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}
}
void ASOHInteractableActor::ApplyOverlayMaterial(UMaterialInterface* Material)
{
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* Mesh : MeshComponents)
	{
		if (Mesh)
		{
			Mesh->SetOverlayMaterial(Material);
			Mesh->bDisallowNanite = true;
		}
	}
}
