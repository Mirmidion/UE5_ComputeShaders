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

			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("GeneratePerlin"));

			const TShaderMapRef<FPerlinShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
			const FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(RenderTarget->GetRenderTargetResource()->TextureRHI, TEXT("PerlinRenderTarget")));
			
			FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

			FPerlinShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FPerlinShaderDeclaration::FParameters>();
			Params->NoiseMap = TargetUAV;
			Params->Start = Position;
			Params->Dimensions = Dimensions;
			Params->Offset = Offset;
			Params->Octaves = Octaves;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("GeneratePerlinPass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Dimensions.X, Dimensions.Y, 1));

			AddCopyTexturePass(GraphBuilder, TargetTexture, RenderTargetTexture);
			
			GraphBuilder.Execute();
		});
}

void UPerlinShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}


