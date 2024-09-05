// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grippables/HandSocketComponent.h"
#include "UObject/Object.h"
#include "IrwinoHandSocket.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTION_API UIrwinoHandSocket : public UHandSocketComponent
{
	GENERATED_BODY()

public:
	explicit UIrwinoHandSocket(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	bool     GetBlendedPoseSnapShot(FPoseSnapshot& PoseSnapShot, UPoseableMeshComponent* TargetMesh, bool bSkipRootBone, bool bFlipHand);
	/**
	 * @brief The custom offset to be applied to the grabbing actor. This offset is added on top of the custom motion controller offset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MakeEditWidget=true), Category="Irwino|Socket")
	FTransform CustomOffset;

	/**
	 * @brief The additional offset we're adding to the hand
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MakeEditWidget=true), Category="Irwino|Socket")
	FTransform AdditionalHandOffset;

	/**
	 * @brief Tells if we want to flip and mirror the hands before applying the additional hand offset
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MakeEditWidget=true), Category="Irwino|Socket")
	bool bWantsFlipAxis;

	/**
	 * @brief Get the axis to flip
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Irwino|Socket", meta=(EditCondition="bWantsFlipAxis"))
	TEnumAsByte<EAxis::Type> OffsetFlipAxis;

	/**
	 * @brief The axis to mirror on
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Irwino|Socket", meta=(EditCondition="bWantsFlipAxis"))
	TEnumAsByte<EAxis::Type> OffsetMirrorAxis;
};
