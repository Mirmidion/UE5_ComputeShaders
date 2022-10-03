// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoldShaderComponent.h"
#include "GameFramework/Actor.h"
#include "MoldShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMoldShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoldShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void ResetShader();

	void DoUpdate();
	void DoDiffuse();

	void InitAgents(ESpawnMode Mode, int Amount);
	void InitRenderTargets();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands, bool bForceUpdate = false);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init", Meta = (ExposeOnSpawn = true))
		int AmountOfAgents = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init", Meta = (ExposeOnSpawn = true))
		ESpawnMode SpawnMode = ESpawnMode::Point;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials", Meta = (ExposeOnSpawn = true))
		int Width = 1920;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials", Meta=(ExposeOnSpawn = true))
		int Height = 1080;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		float Speed = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
		float DecayRate = .5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
		float DiffuseRate = .5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		bool bPaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* BufferRenderTarget;

protected:
	FBufferRHIRef AgentsBuffer;
	FUnorderedAccessViewRHIRef AgentsBufferUAV;

private:
	float Delta;
	float Time;

	TRefCountPtr<IPooledRenderTarget> RenderShaderOutput;
	TRefCountPtr<IPooledRenderTarget> BufferShaderOutput;
};
