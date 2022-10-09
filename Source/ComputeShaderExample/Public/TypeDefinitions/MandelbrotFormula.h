// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMandelbrotFormula : uint8
{
	Minus2 UMETA(DisplayName="Z^-2"),
	Minus1 UMETA(DisplayName = "Z^-1"),
	Plus2 UMETA(DisplayName = "Z^2"),
	Plus3 UMETA(DisplayName = "Z^3"),
	Plus4 UMETA(DisplayName = "Z^4"),
	Plus2And3 UMETA(DisplayName = "Z^2 + Z^3"),
	Plus2And4 UMETA(DisplayName = "Z^2 + Z^4"),
	Plus3And4 UMETA(DisplayName = "Z^3 + Z^4"),
	Plus2And3And4 UMETA(DisplayName = "Z^2 + Z^3 + Z^4"),
	Tricorn,
	BurningShip,
	MandelDiamond
};

