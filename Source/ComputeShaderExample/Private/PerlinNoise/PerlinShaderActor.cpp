// Fill out your copyright notice in the Description page of Project Settings.

#include "PerlinNoise/PerlinShaderActor.h"

APerlinShaderActor::APerlinShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UPerlinShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

void APerlinShaderActor::BeginPlay()
{
	Super::BeginPlay();
}