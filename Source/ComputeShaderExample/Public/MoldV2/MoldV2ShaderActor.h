// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoldV2ShaderComponent.h"
#include "GameFramework/Actor.h"
#include "MoldV2ShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMoldV2ShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMoldV2ShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Components)
	UMoldV2ShaderComponent* ShaderComponent;
};
