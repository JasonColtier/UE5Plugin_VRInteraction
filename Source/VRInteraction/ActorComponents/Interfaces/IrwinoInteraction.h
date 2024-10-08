﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IrwinoInteraction.generated.h"

class UIrwinoInteractableSceneComp;

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class UIrwinoInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRINTERACTION_API IIrwinoInteraction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Irwino Interactions")
	UIrwinoInteractableSceneComp* GetInteractionComponent();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Irwino Interactions")
	bool ShouldBeFlippedIfRightHanded();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Irwino Interactions")
	bool SupportsHandTrackingDropGesture();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Irwino Interactions")
	EAxis::Type         GetFlipAxis();
	virtual EAxis::Type GetFlipAxis_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Irwino Interactions")
	EAxis::Type         GetMirrorAxis();
	virtual EAxis::Type GetMirrorAxis_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Irwino Interactions")
	FTransform GetOptionalGripTransformOffset(UGripMotionControllerComponent* QueryController);

    //an objet not interactible should not be grippable, the object should deny the grip
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, Category="Irwino Interactions")
	void ToggleInteractible(bool Interactible);
	
	//does the hand needs to be hidden if we grab this objet ?
	//should be displayed again when any object is dropped
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Irwino Interactions")
	bool ShouldHideHandOnGrip();
};
