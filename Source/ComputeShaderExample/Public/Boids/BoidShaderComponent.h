// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"

#include "UniformBuffer.h"


#include "BoidShaderComponent.generated.h"



struct Agent
{
	FVector3f position;
};

template<>
struct TShaderParameterTypeInfo<Agent>
{
	static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
	static constexpr int32 NumRows = 1;
	static constexpr int32 NumColumns = 3;
	static constexpr int32 NumElements = 0;
	static constexpr int32 Alignment = 16;
	static constexpr bool bIsStoredInConstantBuffer = true;

	using TAlignedType = TAlignedTypedef<Agent, Alignment>::Type;

	static const FShaderParametersMetadata* GetStructMetadata() { return nullptr; }
};

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidPosition, )
SHADER_PARAMETER(FVector3f, position)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidVelocity, )
SHADER_PARAMETER(FVector3f, velocity)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidAgent, )
SHADER_PARAMETER(Agent, agent)
END_GLOBAL_SHADER_PARAMETER_STRUCT()



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UBoidShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBoidShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int numBoids = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float spawnRadius = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVector3f> outputPositions;

protected:
	// GPU side
	FBufferRHIRef _positionBuffer;
	FUnorderedAccessViewRHIRef _positionBufferUAV;     // we need a UAV for writing

	FBufferRHIRef _timesBuffer;
	FUnorderedAccessViewRHIRef _timesBufferUAV;

	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;
};
