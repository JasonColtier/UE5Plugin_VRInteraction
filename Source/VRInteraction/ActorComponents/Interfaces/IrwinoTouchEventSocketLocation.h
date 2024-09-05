#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include "IrwinoTouchEventSocketLocation.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIrwinoTouchEventSocketLocation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRINTERACTION_API IIrwinoTouchEventSocketLocation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCurrentFingerPosition();
};
