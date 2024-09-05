// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VRInteraction/ActorComponents/IrwinoBaseVRInteractionComp.h"
#include "IrwinoVROverlapInteraction.generated.h"

UCLASS(ClassGroup=(IrwinoInteractionSystem), meta=(BlueprintSpawnableComponent))
class VRINTERACTION_API UIrwinoVROverlapInteraction : public UIrwinoBaseVRInteractionComp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UIrwinoVROverlapInteraction(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	//un délai avant de re-grab un objet
	UPROPERTY(EditAnywhere)
	float MinimalDelayBeforeNextGrab = 0.5f;

	//un offset de rotation à appliquer à l'objet grabbed
	UPROPERTY(EditAnywhere)
	FRotator OffsetRotation = FRotator::ZeroRotator;

	/**
	 * @brief true if we can grab at the moment
	 */
	UPROPERTY(BlueprintReadOnly)
	bool bCanGrab = true;

	/**
	 * @brief The name of the socket to look for
	 */
	UPROPERTY(BlueprintReadOnly, Category="Interaction|Grip Configuration")
	FName ControllerTrackingSocketName;

	/**
	 * @brief Called whenever we attempt to grab an actor
	 */
	UPROPERTY(BlueprintAssignable)
	FOnGripAttempt OnAttemptObjectGrip;

	/**
	 * @brief Called whenever we attempt to grab an actor
	 */
	UPROPERTY(BlueprintAssignable)
	FOnGripAttempt OnAttemptObjectDrop;

	/**
	 * @brief True if the actor should be flipped
	 */
	UPROPERTY(EditAnywhere)
	bool bShouldFlipActorOnGrab = false;

	/**
	 * @brief Request a drop of the current object
	 */
	UFUNCTION(BlueprintCallable, Category="Grip")
	virtual void RequestDrop();

	/**
	 * @brief Handles the dropping of the object currently being held
	 */
	UFUNCTION()
	virtual void HandleDropRequest();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	//this event will only fire on the Locally controlled side
	//we do this to the grabbed object is always matching the local controller location
	virtual void OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) override;

	/**
	 * @brief Tells whether an object is already being held
	 * @param Object The object to check
	 * @return true if the object is already being held
	 */
	bool IsObjectAlreadyGripped(const UObject* Object) const;

	/**
	 * @brief Try to grab an object
	 * @param ObjectToGrab the object we should try to grab. Assumed to be valid.
	 */
	UFUNCTION(BlueprintCallable)
	void TryGrab(UObject* ObjectToGrab);

private:
	UPROPERTY()
	FTimerHandle DelayGrabHandle;

	/**
	 * @brief resets the grab authorization
	 */
	void OnGrabDelayEnd();

	/**
	 * @brief removes the scale of the associated controller from a given transform
	 * @param Other The transform to remove the controller scale from
	 * @return The new transform after removing the controller scale
	 */
	FTransform RemoveControllerScale(const FTransform& Other) const;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
