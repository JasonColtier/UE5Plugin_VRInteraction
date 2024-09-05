// Fill out your copyright notice in the Description page of Project Settings.

#include "IrwinoInteraction.h"

// Add default functionality here for any IIrwinoInteraction functions that are not pure virtual.
EAxis::Type IIrwinoInteraction::GetFlipAxis_Implementation()
{
	return EAxis::Y;
}

EAxis::Type IIrwinoInteraction::GetMirrorAxis_Implementation()
{
	return EAxis::X;
}
