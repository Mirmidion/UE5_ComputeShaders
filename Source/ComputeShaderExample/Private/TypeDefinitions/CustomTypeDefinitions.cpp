// Fill out your copyright notice in the Description page of Project Settings.


#include "TypeDefinitions/CustomTypeDefinitions.h"

FIntVector4d::FIntVector4d(const int X, const int Y, const int Z, const int W) : X(X), Y(Y), Z(Z), W(W)
{

}

FIntVector4d::FIntVector4d() : X(0), Y(0), Z(0), W(0)
{
}

FVector2Float::FVector2Float(const float X, const float Y) : X(X), Y(Y)
{
}

FVector2Float::FVector2Float() : X(0), Y(0)
{
}

FVector3Float::FVector3Float(const float X, const float Y, const float Z) : X(X), Y(Y), Z(Z)
{
}

FVector3Float::FVector3Float() : X(0), Y(0), Z(0)
{
}

FVector4Float::FVector4Float(const float X, const float Y, const float Z, const float W) : X(X), Y(Y), Z(Z), W(W)
{
}

FVector4Float::FVector4Float() : X(0), Y(0), Z(0), W(0)
{
}

FAgentV2::FAgentV2()
{
	angle = 0;
	speciesIndex = 0;
	speciesMask = FIntVector4d();
	position = FVector2Float();
}
