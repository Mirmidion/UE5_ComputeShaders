// Fill out your copyright notice in the Description page of Project Settings.


#include "LSystem/LSystemShaderActor.h"

// Sets default values
ALSystemShaderActor::ALSystemShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<ULSystemShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);
}

// Called when the game starts or when spawned
void ALSystemShaderActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALSystemShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

