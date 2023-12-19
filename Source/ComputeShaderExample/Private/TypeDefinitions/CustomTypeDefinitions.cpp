// Fill out your copyright notice in the Description page of Project Settings.


#include "TypeDefinitions/CustomTypeDefinitions.h"

FIntVector2d::FIntVector2d(const int X, const int Y) : X(X), Y(Y)
{
}

FIntVector2d::FIntVector2d() : X(0), Y(0)
{
}

FLSystemRule::FLSystemRule() : Action(ELSystemAction::Forward), ActionValue(40)
{
}

FLSystemRule::FLSystemRule(const ELSystemAction Action, const int Value) : Action(Action), ActionValue(Value)
{
}

FLSystemRuleList::FLSystemRuleList()
{
}

FLSystemRuleList::FLSystemRuleList(FLSystemRule Action)
{
	List.Add(Action);
}

F2DLine::F2DLine()
{
}

F2DLine::F2DLine(const FVector2Float Start, const FVector2Float End) : Start(Start), End(End)
{

}

F2DLine::F2DLine(const FVector2D Start, const FVector2D End) : Start(Start), End(End)
{
}

FIntVector4d::FIntVector4d(const int X, const int Y, const int Z, const int W) : X(X), Y(Y), Z(Z), W(W)
{
}

FIntVector4d::FIntVector4d() : X(0), Y(0), Z(0), W(0)
{
}

FVector2Float::FVector2Float(const FVector2D Location) : X(Location.X), Y(Location.Y)
{
}

FVector2Float::FVector2Float(const float XY) : X(XY), Y(XY)
{
}

FVector2Float::FVector2Float(const float X, const float Y) : X(X), Y(Y)
{
}

FVector2Float::FVector2Float() : X(0), Y(0)
{
}

FString FVector2Float::ToString() 
{
	return FString("X:").Append(FString::SanitizeFloat(X)).Append(",Y:").Append(FString::SanitizeFloat(Y));
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
	Angle = 0;
	SpeciesIndex = 0;
	SpeciesMask = FIntVector4d();
	Position = FVector2Float();
}
