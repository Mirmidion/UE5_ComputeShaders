// Fill out your copyright notice in the Description page of Project Settings.

#include "Mandelbrot/MandelbrotShaderActor.h"

AMandelbrotShaderActor::AMandelbrotShaderActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMandelbrotShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

void AMandelbrotShaderActor::BeginPlay()
{
	Super::BeginPlay();
}