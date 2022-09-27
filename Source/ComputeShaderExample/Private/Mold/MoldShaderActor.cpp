// Fill out your copyright notice in the Description page of Project Settings.


#include "Mold/MoldShaderActor.h"

// Sets default values
AMoldShaderActor::AMoldShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMoldShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);

}

// Called when the game starts or when spawned
void AMoldShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMoldShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

