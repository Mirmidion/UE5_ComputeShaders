// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystem/LSystemShaderActor.h"

ALSystemShaderActor::ALSystemShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<ULSystemShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

void ALSystemShaderActor::BeginPlay()
{
	Super::BeginPlay();
}