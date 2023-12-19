// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerlinShaderComponent.h"
#include "GameFramework/Actor.h"
#include "PerlinShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API APerlinShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APerlinShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Components)
	UPerlinShaderComponent* ShaderComponent;
};
