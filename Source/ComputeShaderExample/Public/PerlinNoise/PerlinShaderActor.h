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
	// Sets default values for this actor's properties
	APerlinShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		UPerlinShaderComponent* ShaderComponent;
};
