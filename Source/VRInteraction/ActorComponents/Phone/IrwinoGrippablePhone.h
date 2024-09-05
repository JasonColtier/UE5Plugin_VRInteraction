#pragma once

#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>

#include "CoreMinimal.h"
#include "Components/WidgetInteractionComponent.h"
#include "Grippables/GrippableStaticMeshActor.h"
#include "IrwinoGrippablePhone.generated.h"

UCLASS()
class VRINTERACTION_API AIrwinoGrippablePhone : public AGrippableStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIrwinoGrippablePhone(const FObjectInitializer& Initializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* PhoneCollisionHandler;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UChildActorComponent* PhoneTouchScreen;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void PostInitProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
