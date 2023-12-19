// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComputeShaderBase.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UComputeShaderBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMPUTESHADEREXAMPLE_API IComputeShaderBase
{
	GENERATED_BODY()

public:
	virtual void ClearShader();

	virtual void UpdateShader();

	virtual void TogglePaused();

	bool bPaused = false;
protected:
	virtual void InitShader();

	virtual void CheckRenderBuffers(FRHICommandListImmediate& RHICommands);
};

