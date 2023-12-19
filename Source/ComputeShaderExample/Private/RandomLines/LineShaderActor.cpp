// Fill out your copyright notice in the Description page of Project Settings.

#include "RandomLines/LineShaderActor.h"

ALineShaderActor::ALineShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<ULineShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

void ALineShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

