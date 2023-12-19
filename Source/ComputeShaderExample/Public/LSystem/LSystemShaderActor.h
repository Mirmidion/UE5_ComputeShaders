// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSystemShaderComponent.h"
#include "GameFramework/Actor.h"
#include "LSystemShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API ALSystemShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALSystemShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Components)
	ULSystemShaderComponent* ShaderComponent;
};
