// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "LSystemShaderComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API ULSystemShaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULSystemShaderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
		void GenerateSystem();
	UFUNCTION(BlueprintCallable)
		void SerializeSettings(FString Name);
	UFUNCTION(BlueprintCallable)
		bool DeSerializeSettings(FString Path);
	UFUNCTION(BlueprintCallable)
		TArray<FString> GetAllSettings();

	FString GetIteratedString(FString ToUse);

	void DoUpdate();
	void ClearTarget();

	void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Init")
		int iterations = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int width = 3840;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Materials")
		int height = 2160;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		FString Start = "F-G-G";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		TMap<FString, FString> Rules = {{"F","F-G+F+G-F"}, {"G","GG"}};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		TMap<FString, FLSystemRule> Actions = {{"F",FLSystemRule(ELSystemAction::Forward, 40)},{"G",FLSystemRule(ELSystemAction::Forward, 40)},{"+",FLSystemRule(ELSystemAction::TurnLeft, 120)},{"-",FLSystemRule(ELSystemAction::TurnRight, 120)}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		TArray<F2DLine> Lines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		int AmountOfLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		float PercentagePerSecond = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings|Runtime")
		bool Paused = false;

	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
		UTextureRenderTarget2D* RenderTarget;

protected:
	FBufferRHIRef _linesBuffer;
	FUnorderedAccessViewRHIRef _linesBufferUAV;

	float Delta;
	float Time;
	float BufferTime;
};
