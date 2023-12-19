// Fill out your copyright notice in the Description page of Project Settings.

#include "MoldV2/MoldV2ShaderActor.h"

AMoldV2ShaderActor::AMoldV2ShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMoldV2ShaderComponent>(FName("ShaderComponentV2"));
	AddOwnedComponent(ShaderComponent);
}

void AMoldV2ShaderActor::BeginPlay()
{
	Super::BeginPlay();
}