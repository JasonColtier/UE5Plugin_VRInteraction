#include "IrwinoVRTouchInteraction.h"

#include "TraceTool.h"
#include "GameFramework/PlayerController.h"
#include "VRInteraction/ActorComponents/Interfaces/IrwinoTouchableInterface.h"
#include "VRInteraction/ActorComponents/Interfaces/IrwinoTouchEventSocketLocation.h"

// Sets default values for this component's properties
UIrwinoVRTouchInteraction::UIrwinoVRTouchInteraction(const FObjectInitializer& Initializer): Super(Initializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bInteractionsEnabled = true;

	CollisionTags.Empty();
	CollisionTags.Add("FingerInteraction");
}

// Called when the game starts
void UIrwinoVRTouchInteraction::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UIrwinoVRTouchInteraction::OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
	Super::OnOwnerCollisionOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bBFromSweep, SweepResult);

	//quand on détecte une collision on va appeler l'event sur le serveur et localement, ensuite c'est à l'acteur de gérer ces events

	if (IsValid(OtherActor) && bInteractionsEnabled)
	{
		if (OtherActor->Implements<UIrwinoTouchableInterface>())
		{
			TRACE("On Touch %s", *OtherActor->GetName());

			//we replicate touch to server if we are a client and we want to replicate
			if(!IIrwinoTouchableInterface::Execute_ShouldNotReplicateTouch(OtherActor) && !GetOwner()->HasAuthority())
			{
				SR_HandleTouch(OtherActor);
			}
			IIrwinoTouchableInterface::Execute_OnTouch(OtherActor,GetOwner(), IIrwinoTouchEventSocketLocation::Execute_GetCurrentFingerPosition(GetOwner()));
		}
	}
}

void UIrwinoVRTouchInteraction::SR_HandleTouch_Implementation(AActor* TouchedActor)
{
	if (IsValid(TouchedActor))
	{
		TRACE("SR On Touch %s", *TouchedActor->GetName());
		IIrwinoTouchableInterface::Execute_OnTouch(TouchedActor, GetOwner(), IIrwinoTouchEventSocketLocation::Execute_GetCurrentFingerPosition(GetOwner()));
	}
}

void UIrwinoVRTouchInteraction::ToggleInteractionsEnabled(const bool bIsEnabled)
{
	bInteractionsEnabled = bIsEnabled;
	TRACE("Interactions on this component [%s] are [%s]", *GetClass()->GetFName().ToString(), bIsEnabled?TEXT("Enabled"): TEXT("Disabled"));
}

// Called every frame
void UIrwinoVRTouchInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
