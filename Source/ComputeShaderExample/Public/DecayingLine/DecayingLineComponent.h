// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "Components/ActorComponent.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "DecayingLineComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDecayingLine, Log, All)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API UDecayingLineComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:
	UDecayingLineComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void InitShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void ClearShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void UpdateShader() override;
	void Diffuse();
	
	UFUNCTION(BlueprintCallable)
	virtual void TogglePaused() override;
	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	FVector3Float Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	float Offset = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	int Octaves = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	FIntVector2d Dimensions = FIntVector2d(1920, 1080);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
	float DecayRate = .7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta=(ClampMin=0))
	float DiffuseRate = .8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* BufferRenderTarget;
	TRefCountPtr<IPooledRenderTarget> BufferShaderOutput;

protected:
	FBufferRHIRef ValuesBuffer;
	FUnorderedAccessViewRHIRef ValuesBufferUAV;

	float Delta;
	float Time;
	FVector2f Start;
};
