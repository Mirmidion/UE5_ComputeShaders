// Fill out your copyright notice in the Description page of Project Settings.


#include "Mold/MoldShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"
#include "Static/ComputeShaderFunctionLibrary.h"

UMoldShaderComponent::UMoldShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMoldShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	InitShader();
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

void UMoldShaderComponent::UpdateMold() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			const TShaderMapRef<FMoldShaderDeclaration> Shader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FMoldShaderDeclaration::FParameters Params;
			Params.Agents = AgentsBufferUAV;
			Params.TrailMap = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.NumAgents = AmountOfAgents;
			Params.Width = Width;
			Params.Height = Height;
			Params.MoveSpeed = Speed;
			Params.DeltaTime = CurrentDeltaTime;
			Params.Time = Time;

			if (Time > 3)
			{
				FComputeShaderUtils::Dispatch(RHICommands, Shader, Params, FIntVector(AmountOfAgents, 1, 1));
			}

			RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, BufferShaderOutput->GetRenderTargetItem().ShaderResourceTexture, FRHICopyTextureInfo());
		});
}

void UMoldShaderComponent::DiffuseParticles() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FDiffuseShaderDeclaration> cs2(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FDiffuseShaderDeclaration::FParameters Params;
			Params.TrailMap = BufferShaderOutput->GetRenderTargetItem().UAV;
			Params.Width = Width;
			Params.Height = Height;
			Params.DeltaTime = CurrentDeltaTime;
			Params.DecayRate = DecayRate;
			Params.DiffuseRate = DiffuseRate;
			Params.DiffusedMap = ComputeShaderOutput->GetRenderTargetItem().UAV;

			FComputeShaderUtils::Dispatch(RHICommands, cs2, Params, FIntVector(Width, Height, 1));

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

// Called every frame
void UMoldShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bPaused)
	{
		CurrentDeltaTime = DeltaTime;
		Time += CurrentDeltaTime;
			
		UpdateShader();
	}
}

void UMoldShaderComponent::InitShader()
{
	FRandomStream rng;

	{
		TResourceArray<FAgent> agentsResourceArray;
		agentsResourceArray.Init(FAgent(), AmountOfAgents);

		for (int i = 0; i < AmountOfAgents; i++)
		{
			FAgent& agent = agentsResourceArray[i];

			FVector2f Center = FVector2f(Width / 2.f, Height / 2.f);
			FVector2f Position;
			const float RandomAngle = rng.FRand() * 2 * PI;
			const FVector randomVec = rng.GetUnitVector();
			float Angle = 0.f;

			switch (SpawnMode)
			{
			case ESpawnMode::Random:
			{
				Position = FVector2f(rng.FRandRange(0, Width), rng.FRandRange(0, Height));
				Angle = RandomAngle;
				break;
			}
			case ESpawnMode::Point:
			{
				Position = Center;
				Angle = RandomAngle;
				break;
			}
			case ESpawnMode::InwardCircle:
			{
				Position = Center + FVector2f(randomVec.X, randomVec.Y) * Height * 0.5f;
				FVector2f Normalized = Center - Position;
				Normalized.Normalize();
				Angle = FMath::Atan2(Normalized.Y, Normalized.X);
				break;
			}
			case ESpawnMode::RandomCircle:
			{
				Position = Center + FVector2f(randomVec.X, randomVec.Y) * Height * 0.15f;
				Angle = RandomAngle;
				break;
			}
			}

			agent.Position = Position;
			agent.Angle = Angle;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		AgentsBuffer = RHICreateStructuredBuffer(sizeof(FAgent), sizeof(FAgent) * AmountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		AgentsBufferUAV = RHICreateUnorderedAccessView(AgentsBuffer, false, false);
	}

	Time = 0;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	BufferRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	BufferRenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UMoldShaderComponent::UpdateShader()
{
	UpdateMold();
	DiffuseParticles();
}

void UMoldShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}

void UMoldShaderComponent::ClearShader()
{
	UComputeShaderFunctionLibrary::ClearShader(ComputeShaderOutput, RenderTarget, Width, Height);
	UComputeShaderFunctionLibrary::ClearShader(BufferShaderOutput, BufferRenderTarget, Width, Height);
}

