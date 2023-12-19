// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LineShaderComponent.h"
#include "GameFramework/Actor.h"
#include "LineShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API ALineShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALineShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Components)
	ULineShaderComponent* ShaderComponent;
};
