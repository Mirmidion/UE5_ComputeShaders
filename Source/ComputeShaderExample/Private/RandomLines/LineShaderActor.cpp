// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomLines/LineShaderActor.h"

// Sets default values
ALineShaderActor::ALineShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<ULineShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

// Called when the game starts or when spawned
void ALineShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALineShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

