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
	// Sets default values for this actor's properties
	ALSystemShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		ULSystemShaderComponent* ShaderComponent;
};
