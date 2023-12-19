// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "PerlinShaderComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UPerlinShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:	
	UPerlinShaderComponent();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

protected:
	FBufferRHIRef ValuesBuffer;
	FUnorderedAccessViewRHIRef ValuesBufferUAV;

	float Delta;
	float Time;
	FVector2f Start;
};
