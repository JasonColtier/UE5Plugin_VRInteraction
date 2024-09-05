#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include "IrwinoTouchableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class UIrwinoTouchableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRINTERACTION_API IIrwinoTouchableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief Called LOCALLY AND ON SERVER (unless ShouldNotReplicateTouch==true) when there is an overlapp of the finger collision channel
	 * @param ImpactPoint 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTouch(AActor* TouchInitiatingActor,const FVector ImpactPoint);

	
	/**
	 * @brief Used to avoid relication of touch interaction and only use client side
	 * @return 
	 */
	UFUNCTION(BlueprintImplementableEvent)
	bool ShouldNotReplicateTouch();
};
