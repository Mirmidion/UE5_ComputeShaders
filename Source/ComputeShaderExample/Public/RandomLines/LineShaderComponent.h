// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "LineShaderComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API ULineShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULineShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void Reset();

	void DoUpdate();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		int amountOfAgents = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int width = 480;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int height = 270;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		bool Paused = false;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;

protected:
	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;

	float Delta;
	float Time;
};
