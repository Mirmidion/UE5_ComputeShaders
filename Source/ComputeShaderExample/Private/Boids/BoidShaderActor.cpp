// Fill out your copyright notice in the Description page of Project Settings.


#include "Boids/BoidShaderActor.h"

// Sets default values
ABoidShaderActor::ABoidShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UBoidShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);

	DrawComponent = CreateDefaultSubobject<UDrawPositionsComponent>(FName("DrawComponent"));
	AddOwnedComponent(DrawComponent);

	MeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	
}

// Called when the game starts or when spawned
void ABoidShaderActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoidShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

