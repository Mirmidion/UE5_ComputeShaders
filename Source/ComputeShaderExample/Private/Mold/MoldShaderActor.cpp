// Fill out your copyright notice in the Description page of Project Settings.

#include "Mold/MoldShaderActor.h"

AMoldShaderActor::AMoldShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMoldShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

void AMoldShaderActor::BeginPlay()
{
	Super::BeginPlay();
}