// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "MoldV2/MoldV2ShaderComponent.h"
#include "MoldShaderComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UMoldShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:	
	UMoldShaderComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void ClearShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void InitShader() override;
	virtual void UpdateShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void TogglePaused() override;
	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	int AmountOfAgents = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	ESpawnMode SpawnMode = ESpawnMode::Point;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Width = TEXTURE_WIDTH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
	float Speed = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
	float DecayRate = .5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta=(ClampMin=0))
	float DiffuseRate = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* BufferRenderTarget;
	TRefCountPtr<IPooledRenderTarget> BufferShaderOutput;

private:
	void UpdateMold();
	void DiffuseParticles();

	FBufferRHIRef AgentsBuffer;
	FUnorderedAccessViewRHIRef AgentsBufferUAV;

	float CurrentDeltaTime;
	float Time;
};
