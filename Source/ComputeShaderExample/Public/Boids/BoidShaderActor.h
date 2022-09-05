// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoidShaderComponent.h"
#include "DrawPositionsComponent.h"
#include "GameFramework/Actor.h"
#include "BoidShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API ABoidShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		UBoidShaderComponent* ShaderComponent;
	UPROPERTY(EditAnywhere, Category = Components)
		UDrawPositionsComponent* DrawComponent;
	UPROPERTY(EditAnywhere, Category = Components)
		UInstancedStaticMeshComponent* MeshComponent;

};
