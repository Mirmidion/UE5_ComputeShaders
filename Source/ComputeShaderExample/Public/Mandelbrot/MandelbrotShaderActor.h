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
	// Sets default values for this actor's properties
	AMandelbrotShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		UMandelbrotShaderComponent* ShaderComponent;
};
