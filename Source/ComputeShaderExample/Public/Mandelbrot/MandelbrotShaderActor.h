// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MandelbrotShaderComponent.h"
#include "GameFramework/Actor.h"
#include "MandelbrotShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMandelbrotShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMandelbrotShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Components)
	UMandelbrotShaderComponent* ShaderComponent;
};
