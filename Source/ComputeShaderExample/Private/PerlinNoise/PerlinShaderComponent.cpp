// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinNoise/PerlinShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"

UPerlinShaderComponent::UPerlinShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPerlinShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	InitShader();
}

void UPerlinShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateShader();
}

void UPerlinShaderComponent::InitShader()
{
	Time = 0;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Dimensions.X, Dimensions.Y, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UPerlinShaderComponent::ClearShader()
{
	
}

void UPerlinShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}

void UPerlinShaderComponent::UpdateShader() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			const TShaderMapRef<FPerlinShaderDeclaration> Shader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FPerlinShaderDeclaration::FParameters Params;
			Params.NoiseMap = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.Start = Position;
			Params.Dimensions = Dimensions;
			Params.Offset = Offset;
			Params.Octaves = Octaves;

			FComputeShaderUtils::Dispatch(RHICommands, Shader, Params, FIntVector(Dimensions.X, Dimensions.Y, 1));

			RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
		});
}

void UPerlinShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}


