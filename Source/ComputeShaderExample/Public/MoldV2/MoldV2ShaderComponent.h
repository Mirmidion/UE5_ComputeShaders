// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
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
class COMPUTESHADEREXAMPLE_API UMoldV2ShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoldV2ShaderComponent();
	virtual ~UMoldV2ShaderComponent() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands, bool bForceUpdate);

	UFUNCTION(BlueprintCallable)
		void Reset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int StepsPerFrame = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int width = TEXTURE_WIDTH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int height = TEXTURE_HEIGHT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		int amountOfAgents = 10000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
		ESpawnMode spawnMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trail Settings")
		float trailWeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
		float decayRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings")
		float diffuseRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail Settings", Meta=(ExposeOnSpawn = true))
		TArray<FSpeciesSettings> Species;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Running = true;

	TRefCountPtr<IPooledRenderTarget> TrailMapOutput;
	TRefCountPtr<IPooledRenderTarget> DiffusedTrailMapOutput;
	TRefCountPtr<IPooledRenderTarget> DisplayTrailMapOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextureRenderTarget2D* TrailTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextureRenderTarget2D* DiffuseTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextureRenderTarget2D* DisplayTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Paused = false;

protected:
	FBufferRHIRef _agentsBuffer;
	FUnorderedAccessViewRHIRef _agentsBufferUAV;

	FBufferRHIRef _speciesBuffer;
	FUnorderedAccessViewRHIRef _speciesBufferUAV;

	void DoUpdate();
	void DoDiffuse();
	void DoColorMapping();
	float Delta;
	float Time;
	int NumSpecies;
};
