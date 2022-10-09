// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomLines/LineShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"

// Sets default values for this component's properties
ULineShaderComponent::ULineShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULineShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void ULineShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;

	{
		TResourceArray<FVector2Float> agentsResourceArray;
		agentsResourceArray.Init(FVector2Float(), amountOfAgents);

		for (FVector2Float& agent : agentsResourceArray)
			agent = FVector2Float(0, 0);

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(FVector2Float), sizeof(FVector2Float) * amountOfAgents , BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}

	Time = 0;

	
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget);
}

void ULineShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		FRandomStream rng;
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *FString("WhiteNoiseCS_Output_RenderTarget").Append(FString::SanitizeFloat(rng.FRand()));
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}

void ULineShaderComponent::DoUpdate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FLinesShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), _agentsBufferUAV);
			RHICommands.SetUAVParameter(rhiComputeShader, cs->trailmap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numAgents.GetBaseIndex(), sizeof(UINT), &amountOfAgents);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Time.GetBaseIndex(), sizeof(float), &Time);

			RHICommands.SetComputeShader(rhiComputeShader);

			if (Time > 3)
				DispatchComputeShader(RHICommands, cs, amountOfAgents, 1, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

// Called every frame
void ULineShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Paused)
	{
		Delta = DeltaTime;
		Time += Delta;

		for (int i = 0; i < 10; i++)
			DoUpdate();
	}
}

