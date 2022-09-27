// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomTypeDefinitions.generated.h"


USTRUCT()
struct FAgent
{
	GENERATED_USTRUCT_BODY()

		FVector2f position;
	float angle;
};

template<>
struct TShaderParameterTypeInfo<FAgent>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 3;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = 16;
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FAgent, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FIntVector4d
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Z;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int W;

public:
	FIntVector4d(const int X, const int Y, const int Z, const int W);
	FIntVector4d();
};

template<>
struct TShaderParameterTypeInfo<FIntVector4d>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_INT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 4;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FIntVector4d);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FIntVector4d, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FVector2Float
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Y;


public:
	FVector2Float(const float X, const float Y);
	FVector2Float();
};

template<>
struct TShaderParameterTypeInfo<FVector2Float>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 2;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FVector2Float);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FVector2Float, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FVector4Float
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Z;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int W;

public:
	FVector4Float(const float X, const float Y, const float Z, const float W);
	FVector4Float();
};

template<>
struct TShaderParameterTypeInfo<FVector4Float>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 4;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FVector4Float);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FVector4Float, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FAgentV2
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2Float position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float angle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FIntVector4d speciesMask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int speciesIndex;

public:
	FAgentV2();
};

template<>
struct TShaderParameterTypeInfo<FAgentV2>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 8;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = 16;
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FAgentV2, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FSpeciesSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float moveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float turnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float sensorAngleDegrees;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float sensorOffsetDst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int sensorSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector4Float colour;
};

template<>
struct TShaderParameterTypeInfo<FSpeciesSettings>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 9;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FSpeciesSettings);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FSpeciesSettings, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FIntVector2d
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere)
		int X;
	UPROPERTY(EditAnywhere)
		int Y;

public:
	FIntVector2d(const int X, const int Y);
	FIntVector2d();
};

template<>
struct TShaderParameterTypeInfo<FIntVector2d>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_INT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 2;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FIntVector2d);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FIntVector2d, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};








