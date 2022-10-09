// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "MandelbrotShaderComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API UMandelbrotShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMandelbrotShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void Reset();
	UFUNCTION(BlueprintCallable)
		void EnqueueUpdate();

	void DoUpdate();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int width = TEXTURE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		float zoom = FMath::Pow(5.f,5.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
		FVector2Float center = FVector2Float(0.5, 0.5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
		int iterations = 1000;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		bool bDoUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		int Mode = 0;

private:
	float ZoomLevel;
};