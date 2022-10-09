// Fill out your copyright notice in the Description page of Project Settings.


#include "Mandelbrot/MandelbrotShaderActor.h"

// Sets default values
AMandelbrotShaderActor::AMandelbrotShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMandelbrotShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

// Called when the game starts or when spawned
void AMandelbrotShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMandelbrotShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

