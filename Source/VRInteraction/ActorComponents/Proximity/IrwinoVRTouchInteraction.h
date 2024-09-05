#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>

#include "VRInteraction/ActorComponents/IrwinoBaseVRInteractionComp.h"
#include "IrwinoVRTouchInteraction.generated.h"

UCLASS(ClassGroup=(IrwinoInteractionSystem), meta=(BlueprintSpawnableComponent))
class VRINTERACTION_API UIrwinoVRTouchInteraction : public UIrwinoBaseVRInteractionComp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UIrwinoVRTouchInteraction(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	/**
	 * @brief Toggle on/off the interactions
	 * @param bIsEnabled if true, enables interactions
	 */
	UFUNCTION(BlueprintCallable, Category="Interactions|Settings")
	virtual void ToggleInteractionsEnabled(bool bIsEnabled);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnOwnerCollisionOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(Server, Reliable)
	void SR_HandleTouch(AActor* TouchedActor);

	UPROPERTY(BlueprintReadOnly)
	bool bInteractionsEnabled;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
