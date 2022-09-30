// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "PerlinShaderComponent.generated.h"



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

	void Generate();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		FVector3Float Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		float Offset = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		int Octaves = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		FIntVector2d Dimensions = FIntVector2d(1920, 1080);

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;

protected:
	FBufferRHIRef _valuesBuffer;
	FUnorderedAccessViewRHIRef _valuesBufferUAV;

	float Delta;
	float Time;
	FVector2f Start;
};
