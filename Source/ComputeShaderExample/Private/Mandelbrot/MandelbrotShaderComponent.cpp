// Fill out your copyright notice in the Description page of Project Settings.


#include "Mandelbrot/MandelbrotShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Static/ComputeShaderFunctionLibrary.h"

UMandelbrotShaderComponent::UMandelbrotShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMandelbrotShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	ClearShader();
}

void UMandelbrotShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDoUpdate)
	{
		UpdateShader();
	}
}

void UMandelbrotShaderComponent::ClearShader()
{
	UComputeShaderFunctionLibrary::ClearShader(ComputeShaderOutput, RenderTarget, Width, Height);
}

void UMandelbrotShaderComponent::EnqueueUpdate()
{
	bDoUpdate = true;
}

void UMandelbrotShaderComponent::InitShader()
{
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
	UpdateShader();
}

void UMandelbrotShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		FRandomStream rng;
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *FString("MandelbrotCS_Output_RenderTarget").Append(FString::SanitizeFloat(rng.FRand()));
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("MandelbrotCS_Output_RenderTarget"));
	}
}

void UMandelbrotShaderComponent::UpdateShader() {
	ZoomLevel = 1.f / FMath::Pow(Zoom,Zoom);
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FMandelbrotShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FMandelbrotShaderDeclaration::FParameters Params;
			Params.TrailMap = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.Width = Width;
			Params.Height = Height;
			Params.Center = Center;
			Params.Zoom = ZoomLevel;
			Params.Iterations = Iterations;
			Params.Mode = Mode;

			FComputeShaderUtils::Dispatch(RHICommands, cs, Params, FIntVector(Width, Height, 1));

			RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
		});
}

void UMandelbrotShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}


