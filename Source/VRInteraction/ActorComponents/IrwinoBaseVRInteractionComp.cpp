// Fill out your copyright notice in the Description page of Project Settings.

#include "IrwinoBaseVRInteractionComp.h"

#include "BPFL_GenericTools.h"
#include "TraceTool.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

UIrwinoBaseVRInteractionComp::UIrwinoBaseVRInteractionComp(const FObjectInitializer& Initializer): Super(Initializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// default to false
	bUseCustomCollisionTags = false;

	// add default interaction collision tags
	CollisionTags.Add("Interaction");
}

void UIrwinoBaseVRInteractionComp::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
	if (AActor* Owner = GetOwner())
	{
		// if (UPI_FunctionLibrary::IsFirstPawnLocallyControlled(Owner))
		if (GetOwner()->GetInstigator() && GetOwner()->GetInstigator()->IsLocallyControlled())
		{
			TArray<UActorComponent*> Components;
			Owner->GetComponents(UShapeComponent::StaticClass(), Components);

			for (UActorComponent* Component : Components)
			{
				if (Component->ComponentTags.ContainsByPredicate([this](const FName& Tag) { return CollisionTags.Contains(Tag); }))
				{
					if (UBoxComponent* Box = Cast<UBoxComponent>(Component))
					{
						Box->OnComponentBeginOverlap.RemoveAll(this);
						Box->OnComponentHit.RemoveAll(this);
					}
					else if (USphereComponent* Sphere = Cast<USphereComponent>(Component))
					{
						Sphere->OnComponentBeginOverlap.RemoveAll(this);
						Sphere->OnComponentHit.RemoveAll(this);
					}
					else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Component))
					{
						Capsule->OnComponentBeginOverlap.RemoveAll(this);
						Capsule->OnComponentHit.RemoveAll(this);
					}
					else
					{
						HandleUnknownCollisionOnDestruction(Cast<UShapeComponent>(Component));
					}
				}
			}
		}
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UIrwinoBaseVRInteractionComp::BindCollisionEvents(UGripMotionControllerComponent* AssociatedController)
{
	if (const AActor* Owner = GetOwner())
	{
		if (UBPFL_GenericTools::IsFirstPawnLocallyControlled(GetOwner()))
		{
			AssociatedMotionController = AssociatedController;

			TArray<UActorComponent*> Components;
			Owner->GetComponents(UShapeComponent::StaticClass(), Components);

			for (UActorComponent* Component : Components)
			{
				if (bUseCustomCollisionTags)
				{
					if (!Component->ComponentTags.ContainsByPredicate([this](const FName& Tag) { return CollisionTags.Contains(Tag); }))
						continue;
				}

				if (UBoxComponent* Box = Cast<UBoxComponent>(Component))
				{
					Box->OnComponentBeginOverlap.RemoveAll(this);
					Box->OnComponentBeginOverlap.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionOverlapped);
					Box->OnComponentHit.RemoveAll(this);
					Box->OnComponentHit.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionHit);
					BoundCollisions.Add(Cast<UShapeComponent>(Component));
					TRACE("binding set with UBoxComponent");
				}
				else if (USphereComponent* Sphere = Cast<USphereComponent>(Component))
				{
					Sphere->OnComponentBeginOverlap.RemoveAll(this);
					Sphere->OnComponentBeginOverlap.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionOverlapped);
					Sphere->OnComponentHit.RemoveAll(this);
					Sphere->OnComponentHit.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionHit);
					BoundCollisions.Add(Cast<UShapeComponent>(Component));
					TRACE("binding set with USphereComponent");
				}
				else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Component))
				{
					Capsule->OnComponentBeginOverlap.RemoveAll(this);
					Capsule->OnComponentBeginOverlap.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionOverlapped);
					Capsule->OnComponentHit.RemoveAll(this);
					Capsule->OnComponentHit.AddDynamic(this, &UIrwinoBaseVRInteractionComp::OnOwnerCollisionHit);
					BoundCollisions.Add(Cast<UShapeComponent>(Component));
					TRACE("binding set with UCapsuleComponent");
				}
				else
				{
					HandleUnknownCollision(Cast<UShapeComponent>(Component));
				}
			}
		}
		else
		{
			TRACE("the pawn is not locally controlled, we only set binding on local player")
		}
	}
}

void UIrwinoBaseVRInteractionComp::HandleUnknownCollision(UShapeComponent* Collision)
{
}

void UIrwinoBaseVRInteractionComp::HandleUnknownCollisionOnDestruction(UShapeComponent* Collision)
{
}

// Called when the game starts
void UIrwinoBaseVRInteractionComp::BeginPlay()
{
	Super::BeginPlay();
}

void UIrwinoBaseVRInteractionComp::OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
}

void UIrwinoBaseVRInteractionComp::OnOwnerCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

// Called every frame
void UIrwinoBaseVRInteractionComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
