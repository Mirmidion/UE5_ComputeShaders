// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoldShaderComponent.h"
#include "GameFramework/Actor.h"
#include "MoldShaderActor.generated.h"

UCLASS()
class COMPUTESHADEREXAMPLE_API AMoldShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMoldShaderActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	UMoldShaderComponent* ShaderComponent;
};
