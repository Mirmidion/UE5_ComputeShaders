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

void UMoldShaderComponent::UpdateMold_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder) {
	
	CheckRenderBuffers(RHICommands);

	if (Time > 3)
	{
		const TShaderMapRef<FMoldShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
		const FRDGTextureRef BufferTargetTexture = GraphBuilder.RegisterExternalTexture(BufferShaderOutput);

		FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

		FMoldShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FMoldShaderDeclaration::FParameters>();
		Params->Agents = AgentsBuffer.GetUAV();
		Params->TrailMap = TargetUAV;
		Params->NumAgents = AmountOfAgents;
		Params->Width = Width;
		Params->Height = Height;
		Params->MoveSpeed = Speed;
		Params->DeltaTime = CurrentDeltaTime;
		Params->Time = Time;
	
		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("UpdateMoldV1Pass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(AmountOfAgents, 1, 1));

		AddCopyTexturePass(GraphBuilder, TargetTexture, BufferTargetTexture);
	}
}

void UMoldShaderComponent::DiffuseParticles_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder) {
	CheckRenderBuffers(RHICommands);

	const TShaderMapRef<FDiffuseShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
	const FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(RenderTarget->GetRenderTargetResource()->TextureRHI, TEXT("DiffuseRenderTarget")));

	FRDGTextureUAV* SourceUAV = GraphBuilder.CreateUAV(GraphBuilder.RegisterExternalTexture(BufferShaderOutput), ERDGUnorderedAccessViewFlags::SkipBarrier);
	FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

	FDiffuseShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FDiffuseShaderDeclaration::FParameters>();
	Params->TrailMap = SourceUAV;
	Params->Width = Width;
	Params->Height = Height;
	Params->DeltaTime = CurrentDeltaTime;
	Params->DecayRate = DecayRate;
	Params->DiffuseRate = DiffuseRate;
	Params->DiffusedMap = TargetUAV;

	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("DiffuseMoldV1Pass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Width, Height, 1));

	AddCopyTexturePass(GraphBuilder, TargetTexture, RenderTargetTexture);

	GraphBuilder.Execute();
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
		TArray<FAgent> agentsResourceArray;
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
		
		AgentsBuffer.Write(agentsResourceArray);
	}

	Time = 0;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	BufferRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	BufferRenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UMoldShaderComponent::UpdateShader()
{
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("Mold"));

			UpdateMold_RenderThread(RHICommands, GraphBuilder);
			DiffuseParticles_RenderThread(RHICommands, GraphBuilder);
		});
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

