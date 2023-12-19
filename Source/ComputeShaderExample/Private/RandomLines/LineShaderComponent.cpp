// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomLines/LineShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"
#include "Static/ComputeShaderFunctionLibrary.h"

ULineShaderComponent::ULineShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULineShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	InitShader();
}

void ULineShaderComponent::ClearShader()
{
	Time = 0;

	UComputeShaderFunctionLibrary::ClearShader(ComputeShaderOutput, RenderTarget, Width, Height);
}

void ULineShaderComponent::InitShader()
{
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	{
		TResourceArray<FVector2Float> AgentsResourceArray;
		AgentsResourceArray.Init(FVector2Float(), AmountOfAgents);

		for (FVector2Float& Agent : AgentsResourceArray)
			Agent = FVector2Float(0, 0);

		FRHIResourceCreateInfo CreateInfo{ *FString("") };
		CreateInfo.ResourceArray = &AgentsResourceArray;

		AgentsBuffer = RHICreateStructuredBuffer(sizeof(FVector2Float), sizeof(FVector2Float) * AmountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
		AgentsBufferUAV = RHICreateUnorderedAccessView(AgentsBuffer, false, false);
	}
}

void ULineShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}

void ULineShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		const FRandomStream Random;
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *FString("WhiteNoiseCS_Output_RenderTarget").Append(FString::SanitizeFloat(Random.FRand()));
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}

void ULineShaderComponent::UpdateShader() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			const TShaderMapRef<FLinesShaderDeclaration> Shader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FLinesShaderDeclaration::FParameters Params;
			Params.Agents = AgentsBufferUAV;
			Params.TrailMap = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.NumAgents = AmountOfAgents;
			Params.Width = Width;
			Params.Height = Height;
			Params.DeltaTime = CurrentDeltaTime;
			Params.Time = Time;

			if (Time > 3)
			{
				FComputeShaderUtils::Dispatch(RHICommands, Shader, Params, FIntVector(AmountOfAgents, 1, 1));
			}

			RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
		});
}

// Called every frame
void ULineShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bPaused)
	{
		Time += CurrentDeltaTime = DeltaTime;

		for (int i = 0; i < 10; i++)
			UpdateShader();
	}
}

