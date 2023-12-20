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
		TArray<FVector2Float> AgentsResourceArray;
		AgentsResourceArray.Init(FVector2Float(), AmountOfAgents);

		for (FVector2Float& Agent : AgentsResourceArray)
			Agent = FVector2Float(0, 0);

		AgentsBuffer.Write(AgentsResourceArray);
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

			if (Time > 3)
			{
				FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("UpdateRandomLines"));

				const TShaderMapRef<FLinesShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

				const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
				const FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(RenderTarget->GetRenderTargetResource()->TextureRHI, TEXT("LineRenderTarget")));
				
				FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

				FLinesShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FLinesShaderDeclaration::FParameters>();
				Params->Agents = AgentsBuffer.GetUAV();
				Params->TrailMap = TargetUAV;
				Params->NumAgents = AmountOfAgents;
				Params->Width = Width;
				Params->Height = Height;
				Params->DeltaTime = CurrentDeltaTime;
				Params->Time = Time;

				FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("UpdateRandomLinesPass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(AmountOfAgents, 1, 1));

				AddCopyTexturePass(GraphBuilder, TargetTexture,RenderTargetTexture);
				
				GraphBuilder.Execute();
			}
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

