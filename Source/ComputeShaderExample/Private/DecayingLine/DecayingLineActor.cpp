// Fill out your copyright notice in the Description page of Project Settings.


#include "DecayingLine/DecayingLineActor.h"


ADecayingLineActor::ADecayingLineActor()
{
	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);

	ShaderComponent = CreateDefaultSubobject<UDecayingLineComponent>("ShaderComponent");
	AddOwnedComponent(ShaderComponent);
}

void ADecayingLineActor::BeginPlay()
{
	Super::BeginPlay();
	
}


