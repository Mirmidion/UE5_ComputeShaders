// Fill out your copyright notice in the Description page of Project Settings.


#include "Mold/MoldShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"

// Sets default values for this component's properties
UMoldShaderComponent::UMoldShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoldShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void UMoldShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;
	
	{
		TResourceArray<FAgent> agentsResourceArray;
		agentsResourceArray.Init(FAgent(), amountOfAgents);

		for (int i = 0; i < amountOfAgents; i++)
		{
			FAgent& agent = agentsResourceArray[i];

			FVector2f Center = FVector2f(width / 2.f, height / 2.f);
			FVector2f Position;
			const float RandomAngle = rng.FRand() * 2 * PI;
			const FVector randomVec = rng.GetUnitVector();
			float Angle = 0.f;

			switch (spawnMode)
			{
			case SpawnMode::Random:
			{
				Position = FVector2f(rng.FRandRange(0, width), rng.FRandRange(0, height));
				Angle = RandomAngle;
				break;
			}
			case SpawnMode::Point:
			{
				Position = Center;
				Angle = RandomAngle;
				break;
			}
			case SpawnMode::InwardCircle:
			{
				Position = Center + FVector2f(randomVec.X, randomVec.Y) * height * 0.5f;
				FVector2f Normalized = Center - Position;
				Normalized.Normalize();
				Angle = FMath::Atan2(Normalized.Y, Normalized.X);
				break;
			}
			case SpawnMode::RandomCircle:
			{
				Position = Center + FVector2f(randomVec.X, randomVec.Y) * height * 0.15f;
				Angle = RandomAngle;
				break;
			}
			}

			agent.position = Position;
			agent.angle = Angle;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(FAgent), sizeof(FAgent) * amountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}

	Time = 0;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	BufferRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	BufferRenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UMoldShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}

	if (!BufferShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(BufferRenderTarget->SizeX, BufferRenderTarget->SizeY), BufferRenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget2");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, BufferShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget2"));
	}
}

void UMoldShaderComponent::DoUpdate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FMoldShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), _agentsBufferUAV);
			RHICommands.SetUAVParameter(rhiComputeShader, cs->trailmap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numAgents.GetBaseIndex(), sizeof(UINT), &amountOfAgents);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->moveSpeed.GetBaseIndex(), sizeof(float), &speed);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Time.GetBaseIndex(), sizeof(float), &Time);

			RHICommands.SetComputeShader(rhiComputeShader);

			if (Time > 3)
				DispatchComputeShader(RHICommands, cs, amountOfAgents, 1, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, BufferShaderOutput->GetRenderTargetItem().ShaderResourceTexture, FRHICopyTextureInfo());
			}
		});
}

void UMoldShaderComponent::DoDiffuse() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FDiffuseShaderDeclaration> cs2(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader2 = cs2.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader2, cs2->TrailMap.GetBaseIndex(), BufferShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->width.GetBaseIndex(), sizeof(int), &width);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->height.GetBaseIndex(), sizeof(int), &height);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->decayRate.GetBaseIndex(), sizeof(float), &decayRate);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->diffuseRate.GetBaseIndex(), sizeof(float), &diffuseRate);
			RHICommands.SetUAVParameter(rhiComputeShader2, cs2->DiffusedTrailMap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);

			RHICommands.SetComputeShader(rhiComputeShader2);

			DispatchComputeShader(RHICommands, cs2, width, height, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

// Called every frame
void UMoldShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Paused)
	{
		Delta = DeltaTime;
		Time += Delta;
			
		DoUpdate();
		DoDiffuse();
	}
}

