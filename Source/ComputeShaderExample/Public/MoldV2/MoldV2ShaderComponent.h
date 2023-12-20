// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "ComputeShaderDeclarations.h"
#include "StructuredBufferRW.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "MoldV2ShaderComponent.generated.h"

UENUM(BlueprintType)
enum class ESpawnMode : uint8
{
	Random,
	Point,
	InwardCircle,
	RandomCircle
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UMoldV2ShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:	
	UMoldV2ShaderComponent();

protected:
	virtual void BeginPlay() override;
	void GenerateAgents();

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitShader() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void ClearShader() override;

	virtual void UpdateShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void TogglePaused() override;
	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
	int StepsPerFrame = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
	int Width = TEXTURE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
	int Height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
	int AmountOfAgents = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
	ESpawnMode SpawnMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trail Settings")
	float TrailWeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
	float DecayRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
	float DiffuseRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings", Meta=(ExposeOnSpawn = true))
	TArray<FSpeciesSettings> Species;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* TrailTarget;
	TRefCountPtr<IPooledRenderTarget> TrailMapOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* DiffuseTarget;
	TRefCountPtr<IPooledRenderTarget> DisplayTrailMapOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* DisplayTarget;
	TRefCountPtr<IPooledRenderTarget> DiffusedTrailMapOutput;

private:
	TStructuredBufferRW<FAgentV2> AgentsBuffer;
	TStructuredBufferRW<FSpeciesSettings> SpeciesBuffer;
	
	void UpdateMold_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder);
	void DiffuseParticles_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder);
	void MapColors_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder);

	float CurrentDeltaTime;
	float Time;
	int NumSpecies;
};
