// Fill out your copyright notice in the Description page of Project Settings.

#include "IrwinoVROverlapInteraction.h"

#include <IXRTrackingSystem.h>

#include "CoreMinimal.h"
#include "BPFL_GenericTools.h"
#include "IXRInput.h"
#include "TraceTool.h"
#include "VRGripInterface.h"
#include "VRInteraction/ActorComponents/Comps/IrwinoInteractableSceneComp.h"
#include "VRInteraction/ActorComponents/Interfaces/IrwinoInteraction.h"
#include "VRInteraction/ActorComponents/Sockets/IrwinoHandSocket.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UIrwinoVROverlapInteraction::UIrwinoVROverlapInteraction(const FObjectInitializer& Initializer): Super(Initializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ControllerTrackingSocketName      = FName(TEXT("ControllerSocket"));

	// ...
}

void UIrwinoVROverlapInteraction::RequestDrop()
{
	HandleDropRequest();
}

void UIrwinoVROverlapInteraction::HandleDropRequest()
{
	if (AActor* Owner = GetOwner())
	{
		if (!UBPFL_GenericTools::IsFirstPawnLocallyControlled(Owner))
		{
			TRACE_ERROR("called drop on a non locally controlled char !")
			return;
		}

		if (AssociatedMotionController.Get())
		{
			TArray<UObject*> GrippedActors;
			AssociatedMotionController->GetGrippedObjects(GrippedActors);

			for (UObject* GrippedActor : GrippedActors)
			{
				FBPActorGripInformation GripInfo;
				EBPVRResultSwitch       Result;
				AssociatedMotionController->GetGripByObject(GripInfo, GrippedActor, Result);
				const bool bDropActor = AssociatedMotionController->DropObject(GrippedActor, 0,IVRGripInterface::Execute_SimulateOnDrop(GrippedActor));
				TRACE("Drop tried on %s, success ? %i", *GrippedActor->GetName(), bDropActor);

				UHandSocketComponent* Socket = nullptr;
				if (const AActor* AsActor = Cast<AActor>(GrippedActor))
				{
					TArray<UActorComponent*> NestedChildren;
					AsActor->GetComponents(UHandSocketComponent::StaticClass(), NestedChildren, true); // query all components

					for (UActorComponent* NestedChild : NestedChildren)
					{
						if (ControllerTrackingSocketName == NestedChild->GetFName()) // attempt to find the named component
						{
							Socket = Cast<UHandSocketComponent>(NestedChild);
							break;
						}
					}
				}

				const bool bIsSlotGrip = Result == EBPVRResultSwitch::OnSucceeded;
				OnAttemptObjectDrop.Broadcast(bDropActor, GrippedActor, bIsSlotGrip ? GripInfo.bIsSlotGrip : false, bIsSlotGrip ? Socket : nullptr);
			}
		}
	}
}

// Called when the game starts
void UIrwinoVROverlapInteraction::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

bool UIrwinoVROverlapInteraction::IsObjectAlreadyGripped(const UObject* Object) const
{
	if (AssociatedMotionController->HasGrippedObjects()) // are we grabbing any object?
	{
		return AssociatedMotionController->GetIsObjectHeld(Object); // return if it is being held
	}

	return false;
}

void UIrwinoVROverlapInteraction::TryGrab(UObject* ObjectToGrab)
{
	TRACE("try grabbing %s",*ObjectToGrab->GetName())
	// Halt execution if the object is invalid
	checkfSlow(IsValid(ObjectToGrab), TEXT("The assumed valid object to grab is invalid, null or pending kill."));
	checkfSlow(IsValid(AssociatedMotionController), TEXT("The assumed valid attached motion controller is invalid, null or pending kill."));

	// grab parameters
	bool                          bWorldOffsetIsRelative       = true;                 // tells whether or not the grab transform is relative to the motion controller
	bool                          bHadSlotInRange              = false;                // tells whether we have a slot in range 
	FName                         SlotName                     = EName::None;          // the slot name whenever we have a slot in range
	AActor*                       OwnerOfGrippingObject        = nullptr;              // the outer actor of the object we want to grip, or the object itself if it's an actor
	UObject*                      TheObjectThatHasTheInterface = nullptr;              // The object that owns the interface
	FTransform                    GrabTransform                = FTransform::Identity; // the relative transform that will applied to object we want to grip 
	FTransform                    SlotTransform                = FTransform::Identity; // the transform of the slot 
	constexpr EName               Prefix                       = EName::None;          // the slot prefix override
	UHandSocketComponent*         Socket                       = nullptr;              // the socket of our slot, if any in range
	UIrwinoInteractableSceneComp* CustomGripComponent          = nullptr;              // valid if we have a custom component to grab 

	// the transform of the object we want to grab. Defaults to FTransform::Identity 
	FTransform ObjectToGrabCurrentTransform = FTransform::Identity;

	if (ObjectToGrab->IsA<AActor>()) // is it an actor ?
	{
		AActor* OwnerActor           = Cast<AActor>(ObjectToGrab);
		CustomGripComponent          = OwnerActor->Implements<UIrwinoInteraction>() ? IIrwinoInteraction::Execute_GetInteractionComponent(OwnerActor) : nullptr;
		ObjectToGrabCurrentTransform = CustomGripComponent ? CustomGripComponent->GetComponentTransform() : OwnerActor->GetActorTransform();
		OwnerOfGrippingObject        = OwnerActor;
		TheObjectThatHasTheInterface = OwnerActor->Implements<UVRGripInterface>() ? OwnerActor : nullptr;
	}
	else if (ObjectToGrab->IsA<USceneComponent>()) // is it a component ?
	{
		USceneComponent* SceneComponent = Cast<USceneComponent>(ObjectToGrab);
		CustomGripComponent             = SceneComponent->Implements<UIrwinoInteraction>() ? IIrwinoInteraction::Execute_GetInteractionComponent(SceneComponent->GetOwner()) : nullptr;
		ObjectToGrabCurrentTransform    = CustomGripComponent ? CustomGripComponent->GetComponentTransform() : SceneComponent->GetComponentTransform();
		OwnerOfGrippingObject           = SceneComponent->GetOwner();

		if (SceneComponent->Implements<UVRGripInterface>())
		{
			TheObjectThatHasTheInterface = SceneComponent;
		}
		else
		{
			TheObjectThatHasTheInterface = OwnerOfGrippingObject->Implements<UVRGripInterface>() ? OwnerOfGrippingObject : nullptr;
			ObjectToGrabCurrentTransform = OwnerOfGrippingObject->GetTransform();
		}

		if (!CustomGripComponent) // edge case
		{
			if (OwnerOfGrippingObject->Implements<UIrwinoInteraction>())
			{
				CustomGripComponent          = IIrwinoInteraction::Execute_GetInteractionComponent(OwnerOfGrippingObject);
				ObjectToGrabCurrentTransform = CustomGripComponent ? CustomGripComponent->GetComponentTransform() : SceneComponent->GetComponentTransform();
			}
		}
	}
	else
	{
		TRACE_ERROR("Cannot grab [ %s ], invalid grippable object", *ObjectToGrab->GetFName().ToString())
		return;
	}

	// Halt execution, we assumed that the object we're grabbing has the interface or the owning actor if its a component
	checkfSlow(IsValid(TheObjectThatHasTheInterface), TEXT("We're trying to grab an object without interface."));


	// check whether we have a slot in range
	IVRGripInterface::Execute_ClosestGripSlotInRange(TheObjectThatHasTheInterface,
	                                                 AssociatedMotionController->GetComponentLocation(),
	                                                 false, // TODO: should we handle 2 handed gripping on the same object ?
	                                                 /* out */bHadSlotInRange,
	                                                 /* out */SlotTransform,
	                                                 /* out */SlotName,
	                                                 AssociatedMotionController.Get(),
	                                                 Prefix);

	// do we have a slot in rage ?
	if (bHadSlotInRange)
	{
		// at this point we have a slot in range, so we're going to snap to it

		// get the relative Transform of the slot from the owner
		FTransform RelativeTransform = RemoveControllerScale(ObjectToGrabCurrentTransform.GetRelativeTransform(SlotTransform));

		// we could compress the transform with FTransform_NetQuantize(RelativeTransform)
		GrabTransform = RelativeTransform;

		// attempt to query to hand controller from the direct children
		Socket = UHandSocketComponent::GetHandSocketComponentFromObject(ObjectToGrab, ControllerTrackingSocketName);

		if (!Socket) // the component is nowhere to be seen, but we have a slot grip, so we check for nested components
		{
			TArray<UActorComponent*> NestedChildren;
			OwnerOfGrippingObject->GetComponents(UHandSocketComponent::StaticClass(), NestedChildren, true); // query all components

			for (UActorComponent* NestedChild : NestedChildren)
			{
				if (ControllerTrackingSocketName == NestedChild->GetFName()) // attempt to find the named component
				{
					Socket = Cast<UHandSocketComponent>(NestedChild);
					break;
				}
			}
		}

		if (!Socket)
		{
			TRACE_WARN("Grip slot in range but couldn't find socket component named [ %s ]", *ControllerTrackingSocketName.ToString())
		}
	}
	else // we did not have a slot
	{
		// do we have a custom grip component ?
		if (IsValid(CustomGripComponent))
		{
			// at this point we want to grab our custom component

			FTransform T = CustomGripComponent->GetRelativeTransform();

			// add the local transform offset before performing any calculations
			T.Accumulate(IIrwinoInteraction::Execute_GetOptionalGripTransformOffset(OwnerOfGrippingObject, AssociatedMotionController.Get()));

			// inverse the transform
			FTransform RelativeTransform = T.Inverse();

			// default rotation
			FRotator Rotation = FRotator::ZeroRotator;

			// we try to flip our actor (like the phone)
			// maybe we should scale it in -1 on one axis instead
			if (IIrwinoInteraction::Execute_ShouldBeFlippedIfRightHanded(OwnerOfGrippingObject) && bShouldFlipActorOnGrab)
			{
				Rotation = UKismetMathLibrary::ComposeRotators(Rotation, FRotator(180, 0, 180));
				// RelativeTransform.Mirror(IIrwinoInteraction::Execute_GetMirrorAxis(OwnerOfGrippingObject), IIrwinoInteraction::Execute_GetFlipAxis(OwnerOfGrippingObject));
				TRACE("Flipping actor !")
			}

			// we apply the relative rotation of the scene component
			Rotation = UKismetMathLibrary::ComposeRotators(Rotation, RelativeTransform.GetRotation().Rotator());

			// we apply the offset rotation of this actor component
			Rotation = UKismetMathLibrary::ComposeRotators(Rotation, OffsetRotation);

			// finalize transform
			GrabTransform = FTransform(Rotation, RelativeTransform.GetLocation(), OwnerOfGrippingObject->GetActorRelativeScale3D());

			TRACE("Gripping with custom transform : %s", *GrabTransform.ToString());
		}
		else // we do not have any special logic to handle this, thus sue the default logic
		{
			// convert the world transform of the object we want to grip to a local transform relative to the controller 
			GrabTransform.SetScale3D(ObjectToGrabCurrentTransform.GetScale3D());

			// if we wanted to keep the object relative to the grab location
			// GrabTransform = AssociatedMotionController->ConvertToControllerRelativeTransform(ObjectToGrabCurrentTransform);
		}
	}

	// add the custom offset
	// TRACE_WARN("Applying custom offset from controller of [%s]", *OffsetRotation.ToString())
	// GrabTransform.SetRotation(GrabTransform.GetRotation() * OffsetRotation.Quaternion());

	if (const UIrwinoHandSocket* IrwinoHandSocket = Cast<UIrwinoHandSocket>(Socket))
	{
		// apply the custom socket offset
		GrabTransform *= IrwinoHandSocket->CustomOffset;
		TRACE_WARN("Applying custom offset from handsocket of [%s]", *IrwinoHandSocket->CustomOffset.ToString())
	}

	// query the stiffness and damping
	float Stiffness;
	float Damping;
	IVRGripInterface::Execute_GetGripStiffnessAndDamping(TheObjectThatHasTheInterface, Stiffness, Damping);

	// attempt to grip the given object
	bool bGripSuccess = AssociatedMotionController->GripObject(
	                                                           TheObjectThatHasTheInterface,                                                                // the object
	                                                           GrabTransform,                                                                               // the compressed transform of the grip relative to our controller 
	                                                           bWorldOffsetIsRelative,                                                                      // always true, the transform we pass it will always be relative to our motion controller
	                                                           EName::None,                                                                                 // no custom socket name
	                                                           EName::None,                                                                                 // no bone gripping
	                                                           IVRGripInterface::Execute_GetPrimaryGripType(TheObjectThatHasTheInterface, bHadSlotInRange), // get the collision type
	                                                           IVRGripInterface::Execute_GripLateUpdateSetting(TheObjectThatHasTheInterface),               // get late update settings
	                                                           IVRGripInterface::Execute_GripMovementReplicationType(TheObjectThatHasTheInterface),         // get replication movement type 
	                                                           Stiffness,                                                                                   // queried stiffness 
	                                                           Damping,                                                                                     // queried damping
	                                                           bHadSlotInRange                                                                              // is this a slot grip 
	                                                          );

	if (bGripSuccess) // if we successfully grabbed an object, delay the next grab attempt 
	{
		TRACE("Successfully grabbed [ %s ], applying grab cooldown", *ObjectToGrab->GetFName().ToString())

		bCanGrab = false;
		GetOwner()->GetWorldTimerManager().SetTimer(DelayGrabHandle,
		                                            this,
		                                            &UIrwinoVROverlapInteraction::OnGrabDelayEnd,
		                                            MinimalDelayBeforeNextGrab,
		                                            false);
	}
	else
	{
		TRACE("Failed to grab [ %s ]", *ObjectToGrab->GetFName().ToString())
	}

	// broadcast the grab event
	OnAttemptObjectGrip.Broadcast(bGripSuccess, ObjectToGrab, bHadSlotInRange, Socket);
}

/*
 * This method assumes that the object is valid and will proceed to attempt to grab the object.
 *
 * First we try to grab the object by its slot if any is in range.
 * If the preceding attempt fails, we try to grab the object as it it were a component.
 * If the preceding attempt fails, attempt to grab it as an actor.
 * The fallback method is to attempt to grab the whole actor and keep it relative to the controller or center it in the controller
 */
void UIrwinoVROverlapInteraction::OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bBFromSweep, const FHitResult& SweepResult)
{
	Super::OnOwnerCollisionOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bBFromSweep, SweepResult);

	TRACE("Overlapping %s",*OtherActor->GetName())

	if (!AssociatedMotionController.IsValid())
	{
		TRACE_ERROR("no associated motion controller");
		return;
	}

	// if we can't grab at the moment, abord
	if (!bCanGrab)
		return;

	// the object wa wish to grab
	UObject* TheObjectWeWantToGrab = nullptr;

	// first check whether the component is grippable
	if (OtherComp->Implements<UVRGripInterface>())
	{
		// we want to grab this component
		TheObjectWeWantToGrab = OtherComp;
	}
	// does the other actor implement the VRGrip interface ?
	else if (OtherActor->Implements<UVRGripInterface>())
	{
		// is it a custom scene component ? ...
		if (OtherActor->Implements<UIrwinoInteraction>())
		{
			// get the custom scene component
			TheObjectWeWantToGrab = IIrwinoInteraction::Execute_GetInteractionComponent(OtherActor);
			checkfSlow(TheObjectWeWantToGrab, "Attempting to grab an invalid custom component")
		}

		if(!IsValid(TheObjectWeWantToGrab))
		{
			// ...no, grab the whole actor 
			TheObjectWeWantToGrab = OtherActor;
		}
	}


	// attempt to grab the object we want to grab
	if (IsValid(TheObjectWeWantToGrab))
	{
		// check if we can grab the actor
		if (!AssociatedMotionController->HasGrippedObjects())
		{
			TryGrab(TheObjectWeWantToGrab);
		}
	}
}

void UIrwinoVROverlapInteraction::OnGrabDelayEnd()
{
	bCanGrab = true;
}

FTransform UIrwinoVROverlapInteraction::RemoveControllerScale(const FTransform& Other) const
{
	if (AssociatedMotionController.IsValid())
	{
		return Other * FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector / AssociatedMotionController->GetPivotTransform().GetScale3D());
	}

	return FTransform::Identity;
}

// Called every frame
void UIrwinoVROverlapInteraction::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
