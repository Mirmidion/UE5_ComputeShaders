// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomTypeDefinitions.h"
#include "SerializableSettings.generated.h"

USTRUCT(BlueprintType)
struct FLSystemSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Start;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Iterations;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, FString> Rules;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, FLSystemRuleList> Actions;

};
