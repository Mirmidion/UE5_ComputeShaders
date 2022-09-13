// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "PerlinShaderComponent.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UPerlinShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPerlinShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void Reset();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		FVector Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		float Offset = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		FIntVector2d Dimensions = FIntVector2d(TEXTURE_WIDTH, TEXTURE_HEIGHT);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int width = TEXTURE_WIDTH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int height = TEXTURE_HEIGHT;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;

protected:
	FBufferRHIRef _valuesBuffer;
	FUnorderedAccessViewRHIRef _valuesBufferUAV;

	float Delta;
	float Time;
};
