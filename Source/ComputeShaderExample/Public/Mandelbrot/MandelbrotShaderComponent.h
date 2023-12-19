// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "MandelbrotShaderComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API UMandelbrotShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

public:
	UMandelbrotShaderComponent();

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
	
	UFUNCTION(BlueprintCallable)
	void EnqueueUpdate();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Width = TEXTURE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
	int Height = TEXTURE_HEIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
	float Zoom = FMath::Pow(5.f,5.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
	FVector2Float Center = FVector2Float(0.5, 0.5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime", meta = (ClampMin = 0))
	int Iterations = 1000;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
	bool bDoUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
	int Mode = 0;

private:
	float ZoomLevel;
};