// Fill out your copyright notice in the Description page of Project Settings.


#include "MoldV2/MoldV2ShaderActor.h"

// Sets default values
AMoldV2ShaderActor::AMoldV2ShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UMoldV2ShaderComponent>(FName("ShaderComponentV2"));
	AddOwnedComponent(ShaderComponent);

}

// Called when the game starts or when spawned
void AMoldV2ShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMoldV2ShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

