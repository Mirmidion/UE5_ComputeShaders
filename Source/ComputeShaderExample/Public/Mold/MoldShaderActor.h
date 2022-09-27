// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoldShaderComponent.h"
#include "MoldTextureComponent.h"
#include "GameFramework/Actor.h"
#include "MoldShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMoldShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoldShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		UMoldShaderComponent* ShaderComponent;
};
