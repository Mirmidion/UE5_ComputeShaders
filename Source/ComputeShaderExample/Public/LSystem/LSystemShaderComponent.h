// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderBase.h"
#include "ComputeShaderDeclarations.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "RenderTargetPool.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"
#include "LSystemShaderComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLSystemShader, Log, All)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMPUTESHADEREXAMPLE_API ULSystemShaderComponent : public UActorComponent, public IComputeShaderBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeSerializeDone);

public:
	ULSystemShaderComponent();

protected:
	virtual void BeginPlay() override;
	void CreateRenderTarget();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void InitShader() override;
	void Reset();
	void GenerateLines();
	void CreateLineBuffer();
	
	UFUNCTION(BlueprintCallable)
	virtual void UpdateShader() override;
	UFUNCTION(BlueprintCallable)
	virtual void TogglePaused() override;
	UFUNCTION(BlueprintCallable)
	virtual void ClearShader() override;
	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands) override;

	UFUNCTION(BlueprintCallable)
	void SerializeSettings(FString Name);
	UFUNCTION(BlueprintCallable)
	bool DeSerializeSettings(FString Path);
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAllSettings();

	UFUNCTION(BlueprintCallable)
	FString GetIteratedString(FString ToUse);

	UFUNCTION(BlueprintCallable)
	FString GetStartString() const;
	UFUNCTION(BlueprintCallable)
	void SetStartString(const FString& Value);

	UFUNCTION(BlueprintCallable)
	TMap<FString, FString> GetReplaceRules() const;
	UFUNCTION(BlueprintCallable)
	void SetReplaceRules(const TMap<FString, FString> NewRules);
	
	UFUNCTION(BlueprintCallable)
	TMap<FString, FLSystemRuleList> GetCharacterActions() const;
	UFUNCTION(BlueprintCallable)
	void SetCharacterActions(const TMap<FString, FLSystemRuleList>& NewActions);

	UFUNCTION(BlueprintCallable)
	int GetIterations() const;
	UFUNCTION(BlueprintCallable)
	void SetIterations(const int Value);

	UFUNCTION(BlueprintCallable)
	float GetPercentagePerSecond() const;
	UFUNCTION(BlueprintCallable)
	void SetPercentagePerSecond(const float Value);

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnDeSerializeDone OnDeSerializeDone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "Simulation Materials")
	UTextureRenderTarget2D* RenderTarget;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	
private:
	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Init")
	int SystemIterations = 5;

	UPROPERTY(EditAnywhere, Category = "Simulation Materials")
	int Width = 3840;
	UPROPERTY(EditAnywhere, Category = "Simulation Materials")
	int Height = 2160;

	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Runtime")
	FString Start = "F-G-G";
	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Runtime")
	TMap<FString, FString> Rules = {{"F","F-G+F+G-F"}, {"G","GG"}};
	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Runtime")
	TMap<FString, FLSystemRuleList> Actions = {
		{"F",FLSystemRuleList(FLSystemRule(ELSystemAction::Forward, 40))},
		{"G",FLSystemRuleList(FLSystemRule(ELSystemAction::Forward, 40))},
		{"+",FLSystemRuleList(FLSystemRule(ELSystemAction::TurnLeft, 120))},
		{"-",FLSystemRuleList(FLSystemRule(ELSystemAction::TurnRight, 120))}
	};

	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Runtime")
	int AmountOfLines;

	UPROPERTY(EditAnywhere, Category = "Simulation Settings|Runtime")
	float PercentagePerSecond = 20;

protected:
	TResourceArray<F2DLine> Lines;
	FBufferRHIRef LinesBuffer;
	FUnorderedAccessViewRHIRef LinesBufferUAV;

	float CurrentDeltaTime;
	float Time;
	float BufferTime;
};
