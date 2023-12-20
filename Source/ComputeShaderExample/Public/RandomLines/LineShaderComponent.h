// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "StructuredBufferRW.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "LineShaderComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API ULineShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:
	ULineShaderComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void ClearShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void InitShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void TogglePaused() override;
	virtual void UpdateShader() override;
	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
	int AmountOfAgents = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Width = 480;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Height = 270;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

protected:
	TStructuredBufferRW<FVector2Float> AgentsBuffer;

	float CurrentDeltaTime;
	float Time;
};
