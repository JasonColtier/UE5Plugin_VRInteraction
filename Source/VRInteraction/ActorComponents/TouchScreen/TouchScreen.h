#pragma once

#include "CoreMinimal.h"
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>

#include "CoreMinimal.h"
#include "Components/WidgetInteractionComponent.h"
#include "VRInteraction/ActorComponents/Interfaces/IrwinoTouchableInterface.h"
#include "TouchScreen.generated.h"

UCLASS(ClassGroup=(Touchable))
class VRINTERACTION_API ATouchScreen final : public AActor, public IIrwinoTouchableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ATouchScreen(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* TouchScreenSurface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey PointerKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReleaseTimeInSeconds = 1.0f;

	UPROPERTY()
	FTimerHandle ReleaseHandle;

	virtual void OnTouch_Implementation(AActor* InitiatingTouchActor,const FVector ImpactPoint) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};
