// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "MoldV2ShaderComponent.generated.h"

UENUM()
enum class SpawnMode : uint8
{
	Random,
	Point,
	InwardCircle,
	RandomCircle
};

USTRUCT()
struct FIntVector4d
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
		int X;
	UPROPERTY(EditAnywhere)
		int Y;
	UPROPERTY(EditAnywhere)
		int Z;
	UPROPERTY(EditAnywhere)
		int W;

public:
	FIntVector4d(const int X, const int Y, const int Z, const int W);
	FIntVector4d();
};

USTRUCT()
struct FAgentV2
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
		FVector2f position;
	UPROPERTY(EditAnywhere)
		float angle;
	UPROPERTY(EditAnywhere)
		FIntVector4d speciesMask;
	UPROPERTY(EditAnywhere)
		int speciesIndex;

public:
	FAgentV2();
};

USTRUCT()
struct FSpeciesSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
		float moveSpeed;
	UPROPERTY(EditAnywhere)
		float turnSpeed;

	UPROPERTY(EditAnywhere)
		float sensorAngleDegrees;
	UPROPERTY(EditAnywhere)
		float sensorOffsetDst;
	UPROPERTY(EditAnywhere)
		int sensorSize;
	UPROPERTY(EditAnywhere)
		FVector4f colour;
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UMoldV2ShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoldV2ShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UFUNCTION(BlueprintCallable)
		void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int StepsPerFrame = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int width = 1920;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int height = 1080;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int amountOfAgents = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		SpawnMode spawnMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trail Settings")
		float trailWeight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
		float decayRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
		float diffuseRate = 1;

	UPROPERTY(EditAnywhere, Category = "Trail Settings")
		TArray<FSpeciesSettings> Species = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Running = true;

	TRefCountPtr<IPooledRenderTarget> TrailMapOutput;
	TRefCountPtr<IPooledRenderTarget> DiffusedTrailMapOutput;
	TRefCountPtr<IPooledRenderTarget> DisplayTrailMapOutput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* TrailTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* DiffuseTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* DisplayTarget;

	bool Paused = true;

protected:
	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;

	FBufferRHIRef _speciesBuffer;
	FUnorderedAccessViewRHIRef _speciesBufferUAV;

	float Delta;
	float Time;
	int NumSpecies;
};
