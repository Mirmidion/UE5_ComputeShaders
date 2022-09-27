// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoldV2ShaderComponent.h"
#include "MoldV2TextureComponent.h"
#include "GameFramework/Actor.h"
#include "MoldV2ShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMoldV2ShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoldV2ShaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Components)
		UMoldV2ShaderComponent* ShaderComponent;
};
