// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DecayingLineComponent.h"
#include "GameFramework/Actor.h"
#include "DecayingLineActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API ADecayingLineActor : public AActor
{
	GENERATED_BODY()

public:
	ADecayingLineActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = Components)
	UDecayingLineComponent* ShaderComponent;
};
