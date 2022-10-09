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
	// Sets default values for this actor's properties
	ALineShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		ULineShaderComponent* ShaderComponent;
};
