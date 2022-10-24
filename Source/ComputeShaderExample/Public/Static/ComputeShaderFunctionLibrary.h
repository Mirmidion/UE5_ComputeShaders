// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComputeShaderFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMPUTESHADEREXAMPLE_API UComputeShaderFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename T>
	static void SerializeStruct(T Struct, FString Path)
	{
		FString Content;
		FJsonObjectConverter::UStructToJsonObjectString(Struct, Content);
		FFileHelper::SaveStringToFile(Content, *Path);
	}

	template<typename T>
	static T DeSerializeStruct(FString Path)
	{
		FString Content;
		FFileHelper::LoadFileToString(Content, *Path);
		T Struct;
		FJsonObjectConverter::JsonObjectStringToUStruct(Content, &Struct);
		return Struct;
	}
};
