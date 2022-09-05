// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "MoldShaderComponent.generated.h"

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



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UMoldShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoldShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int amountOfAgents = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int width = TEXTURE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float speed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float decayRate = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float diffuseRate = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Running = true;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	TRefCountPtr<IPooledRenderTarget> BufferShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTextureRenderTarget2D* BufferRenderTarget;

protected:
	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;

	float Delta;
	float Time;
};
