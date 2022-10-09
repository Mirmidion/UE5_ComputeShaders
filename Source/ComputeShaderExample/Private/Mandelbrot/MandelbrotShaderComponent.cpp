// Fill out your copyright notice in the Description page of Project Settings.


#include "Mandelbrot/MandelbrotShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UMandelbrotShaderComponent::UMandelbrotShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMandelbrotShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void UMandelbrotShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget);
	DoUpdate();
}

void UMandelbrotShaderComponent::EnqueueUpdate()
{
	bDoUpdate = true;
}

void UMandelbrotShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		FRandomStream rng;
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *FString("WhiteNoiseCS_Output_RenderTarget").Append(FString::SanitizeFloat(rng.FRand()));
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}

void UMandelbrotShaderComponent::DoUpdate() {
	ZoomLevel = 1.f / FMath::Pow(zoom,zoom);
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FMandelbrotShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->trailmap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->center.GetBaseIndex(), sizeof(FVector2Float), &center);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->zoom.GetBaseIndex(), sizeof(float), &ZoomLevel);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->iterations.GetBaseIndex(), sizeof(int), &iterations);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->mode.GetBaseIndex(), sizeof(int), &Mode);

			RHICommands.SetComputeShader(rhiComputeShader);

			DispatchComputeShader(RHICommands, cs, width, height, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

// Called every frame
void UMandelbrotShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bDoUpdate)
	{
		DoUpdate();
	}
}

