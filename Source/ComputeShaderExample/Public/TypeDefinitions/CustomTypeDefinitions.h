// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomTypeDefinitions.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API UFileShower : public UObject
{
	GENERATED_BODY()
};

USTRUCT()
struct COMPUTESHADEREXAMPLE_API FAgent
{
	GENERATED_USTRUCT_BODY()

	FVector2f Position;

	float Angle;
};

template<>
struct TShaderParameterTypeInfo<FAgent>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 3;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FAgent);
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
	FVector2Float(const FVector2D Location);
	FVector2Float(const float XY);
	FVector2Float(const float X, const float Y);
	FVector2Float();

	FString ToString();

	FVector2Float operator*(const float Input) const
	{
		return FVector2Float(X * Input, Y * Input);
	}

	FVector2Float operator+(const float Input) const
	{
		return FVector2Float(X + Input, Y + Input);
	}

	FVector2Float operator+(const FVector2Float Input) const
	{
		return FVector2Float(X + Input.X, Y + Input.Y);
	}

	FVector2Float operator-(const FVector2Float Input) const
	{
		return FVector2Float(X - Input.X, Y - Input.Y);
	}
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
struct FVector3Float
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z;

public:
	FVector3Float(const float X, const float Y, const float Z);
	FVector3Float();
};

template<>
struct TShaderParameterTypeInfo<FVector3Float>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 3;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = alignof(FVector3Float);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FVector3Float, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};


USTRUCT(BlueprintType)
struct FVector4Float
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float W;

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
	FVector2Float Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector4d SpeciesMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpeciesIndex;

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
	static constexpr int32 Alignment = alignof(FAgentV2);
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<FAgentV2, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

USTRUCT(BlueprintType)
struct FSpeciesSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SensorAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SensorOffsetDst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SensorSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4Float Color;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	static constexpr bool bIsStoredInConstantBuffer = false;

	using TAlignedType = TAlignedTypedef<FIntVector2d, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

UENUM(BlueprintType)
enum class ELSystemAction : uint8
{
	Forward,
	Back,

	PushPosition,
	PopPosition,

	TurnLeft,
	TurnRight,
};

USTRUCT(BlueprintType)
struct FLSystemRule
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELSystemAction Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ActionValue;

	FLSystemRule();
	FLSystemRule(ELSystemAction Action, int Value);
};

USTRUCT(BlueprintType)
struct FLSystemRuleList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLSystemRule> List;

	FLSystemRuleList();
	FLSystemRuleList(FLSystemRule Action);
};

USTRUCT(BlueprintType)
struct F2DLine
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2Float Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2Float End;

	F2DLine();
	F2DLine(const FVector2Float Start, const FVector2Float End);
	F2DLine(const FVector2D Start, const FVector2D End);
};












