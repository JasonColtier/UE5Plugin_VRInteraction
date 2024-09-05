// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GripMotionControllerComponent.h"
#include "Components/ActorComponent.h"
#include "Grippables/HandSocketComponent.h"
#include "IrwinoBaseVRInteractionComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGripAttempt, bool, bSuccess, const UObject*, Other, bool, bWasSlotGrip, UHandSocketComponent*, GripSocket);

UCLASS(Abstract, ClassGroup=(IrwinoInteractionSystem), meta=(BlueprintSpawnableComponent))
class VRINTERACTION_API UIrwinoBaseVRInteractionComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UIrwinoBaseVRInteractionComp(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	/**
	 * @brief Tells this interaction components to bind to the collisions events of the owner
	 */
	UFUNCTION(BlueprintCallable, Category="Initialization")
	virtual void BindCollisionEvents(UGripMotionControllerComponent* AssociatedController);

	/**
	 * @brief A method called whenever the collision of the owner did not match our tags
	 * @param Collision The collision that did not match the required tags
	 */
	UFUNCTION()
	virtual void HandleUnknownCollision(UShapeComponent* Collision);

	/**
	 * @brief A method called whenever the collision of the owner did not match our tags upon destruction of this component
	 * @param Collision The collision that did not match the required tags
	 */
	UFUNCTION()
	virtual void HandleUnknownCollisionOnDestruction(UShapeComponent* Collision);

	/**
	 * @brief If true, allow customization of the collision tags to be considered for subscription
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction|Collision Configuration")
	bool bUseCustomCollisionTags;

	/**
	 * @brief The tags that the collision. If any tag within this array is present within the shape component, it will be considered for binding
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction|Collision Configuration", meta=(EditCondition=bUseCustomCollisionTags))
	TArray<FName> CollisionTags;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	  * @brief 
	  * @param OverlappedComponent the collision component that caused the overlap	
	  * @param OtherActor the other actor we're colliding with
	  * @param OtherComp the component that we overlapped with during the sweep
	  * @param OtherBodyIndex the other body index
	  * @param bBFromSweep true if the overlap was from a sweep
	  * @param SweepResult the sweep hit result
	  */
	UFUNCTION()
	virtual void OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief 
	 * @param HitComponent the collision component that caused the overlap	
	 * @param OtherActor the other actor we're colliding with
	 * @param OtherComp the component that we overlapped with during the sweep
	 * @param NormalImpulse the normal hit impulse
	 * @param Hit the hit result of the simulation
	 */
	UFUNCTION()
	virtual void OnOwnerCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * @brief The associated motion controller, we do not hold hard references to it, so we do not prevent its destruction 
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<class UGripMotionControllerComponent> AssociatedMotionController;

	/**
	 * @brief Reference to the collisions we're bound to
	 */
	UPROPERTY()
	TArray<TWeakObjectPtr<UShapeComponent>> BoundCollisions;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
