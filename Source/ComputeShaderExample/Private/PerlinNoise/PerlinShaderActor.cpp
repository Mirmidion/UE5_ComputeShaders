// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinNoise/PerlinShaderActor.h"

// Sets default values
APerlinShaderActor::APerlinShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UPerlinShaderComponent>(FName("ShaderComponent"));
	AddOwnedComponent(ShaderComponent);

	DrawComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	DrawComponent->AttachToComponent(Component, FAttachmentTransformRules::KeepWorldTransform);

}

// Called when the game starts or when spawned
void APerlinShaderActor::BeginPlay()
{
	Super::BeginPlay();
	if (DrawComponent) {
		UMaterialInstanceDynamic* MID = DrawComponent->CreateAndSetMaterialInstanceDynamic(0);
		if (MID)
			MID->SetTextureParameterValue("InputTexture", (UTexture*)ShaderComponent->RenderTarget);
	}
}

// Called every frame
void APerlinShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

