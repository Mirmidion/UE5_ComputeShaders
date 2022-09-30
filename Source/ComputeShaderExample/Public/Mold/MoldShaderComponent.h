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

	void DoUpdate();
	void DoDiffuse();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		int amountOfAgents = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		SpawnMode spawnMode = SpawnMode::Point;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int width = TEXTURE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		float speed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
		float decayRate = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta=(ClampMin=0))
		float diffuseRate = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		bool Paused = false;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	TRefCountPtr<IPooledRenderTarget> BufferShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* BufferRenderTarget;

protected:
	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;

	float Delta;
	float Time;
};
