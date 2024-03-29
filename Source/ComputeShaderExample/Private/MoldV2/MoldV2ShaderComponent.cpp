// Fill out your copyright notice in the Description page of Project Settings.


#include "MoldV2/MoldV2ShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include "RenderTargetPool.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Static/ComputeShaderFunctionLibrary.h"

namespace MoldV2Constants
{
	static constexpr int StartDelay = 3;
}

UMoldV2ShaderComponent::UMoldV2ShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMoldV2ShaderComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FSpeciesSettings Data;
	Data.Color = FVector4Float(1,1,0,1);
	Data.MoveSpeed = 100;
	Data.SensorAngleDegrees = 45;
	Data.SensorOffsetDst = 75;
	Data.SensorSize = 10;
	Data.TurnSpeed = 10;
	Species.Add(Data);

	InitShader();
}

void UMoldV2ShaderComponent::GenerateAgents()
{
	FRandomStream rng;
	{
		TResourceArray<FAgentV2> agentsResourceArray;
		agentsResourceArray.Init(FAgentV2(), AmountOfAgents);
		for (int index = 0; index < AmountOfAgents; index++)
		{
			FAgentV2& Agent = agentsResourceArray[index];
			FVector2f Center = FVector2f(Width / 2.f, Height / 2.f);
			FVector2Float Position;
			const float RandomAngle = rng.FRand() * 2 * PI;
			const FVector randomVec = rng.GetUnitVector();
			float Angle = 0.f;

			switch (SpawnMode)
			{
			case ESpawnMode::Random:
				{
					Position = FVector2Float(rng.FRandRange(0, Width), rng.FRandRange(0, Height));
					Angle = RandomAngle;
					break;
				}
			case ESpawnMode::Point:
				{
					Position = FVector2Float(Center.X, Center.Y);
					Angle = RandomAngle;
					break;
				}
			case ESpawnMode::InwardCircle:
				{
					FVector2f Value = Center + FVector2f(randomVec.X, randomVec.Y) * Height * 0.5f;
					Position = FVector2Float(Value.X, Value.Y);
					FVector2f Normalized = Center - FVector2f(Position.X, Position.Y);
					Normalized.Normalize();
					Angle = FMath::Atan2(Normalized.Y, Normalized.X);
					break;
				}
			case ESpawnMode::RandomCircle:
				{
					FVector2f Value = Center + FVector2f(randomVec.X, randomVec.Y) * Height * 0.15f;
					Position = FVector2Float(Value.X, Value.Y);
					Angle = RandomAngle;
					break;
				}
			}

			FIntVector4d SpeciesMask;
			int speciesIndex = 0;

			if (NumSpecies == 1)
			{
				SpeciesMask = FIntVector4d(1, 0, 0, 0);
			}
			else
			{
				int species = rng.RandRange(1, NumSpecies);
				speciesIndex = species - 1;
				SpeciesMask = FIntVector4d((species == 1) ? 1 : 0, (species == 2) ? 1 : 0, (species == 3) ? 1 : 0, (species == 4) ? 1 : 0);
			}

			Agent.Position = Position;
			Agent.Angle = Angle;
			Agent.SpeciesIndex = speciesIndex;
			Agent.SpeciesMask = SpeciesMask;
		}

		AgentsBuffer.Write(agentsResourceArray);
	}
}

void UMoldV2ShaderComponent::InitShader()
{
	NumSpecies = Species.Num();
	UE_LOG(LogTemp, Log, TEXT("Amount of species: %d"), NumSpecies)

	GenerateAgents();

	SpeciesBuffer.Write(Species);

	Time = 0;

	DisplayTarget = nullptr;
	DisplayTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	DisplayTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	DiffuseTarget = nullptr;
	DiffuseTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	DiffuseTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	TrailTarget = nullptr;
	TrailTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	TrailTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}


void UMoldV2ShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!TrailMapOutput.IsValid())
	{
		const FRandomStream Random;

		const FString TextureBufferName = FString("TrailMap_Output_RenderTarget").Append(FString::SanitizeFloat(Random.FRand()));
		
		FPooledRenderTargetDesc TrailMapOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(TrailTarget->SizeX, TrailTarget->SizeY), TrailTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		TrailMapOutputDesc.DebugName = *TextureBufferName;
		GRenderTargetPool.FindFreeElement(RHICommands, TrailMapOutputDesc, TrailMapOutput, *TextureBufferName);
	}

	if (!DiffusedTrailMapOutput.IsValid())
	{
		const FRandomStream Random;

		const FString TextureBufferName = FString("DiffusedTrailMap_Output_RenderTarget").Append(FString::SanitizeFloat(Random.FRand()));
		
		FPooledRenderTargetDesc DiffuseOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DiffuseTarget->SizeX, DiffuseTarget->SizeY), DiffuseTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_NoTiling, TexCreate_ShaderResource | TexCreate_UAV, false));
		DiffuseOutputDesc.DebugName = *TextureBufferName;
		GRenderTargetPool.FindFreeElement(RHICommands, DiffuseOutputDesc, DiffusedTrailMapOutput, *TextureBufferName);
	}

	if (!DisplayTrailMapOutput.IsValid())
	{
		const FRandomStream Random;

		const FString TextureBufferName = FString("DisplayTrailMap_Output_RenderTarget").Append(FString::SanitizeFloat(Random.FRand()));
		
		FPooledRenderTargetDesc DisplayOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DisplayTarget->SizeX, DisplayTarget->SizeY), DisplayTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_NoTiling, TexCreate_ShaderResource | TexCreate_UAV, false));
		DisplayOutputDesc.DebugName = *TextureBufferName;
		GRenderTargetPool.FindFreeElement(RHICommands, DisplayOutputDesc, DisplayTrailMapOutput, *TextureBufferName);
	}
}

// Called every frame
void UMoldV2ShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bPaused)
		return;

	CurrentDeltaTime = DeltaTime;
	Time += CurrentDeltaTime;

	UpdateShader();
}

void UMoldV2ShaderComponent::ClearShader()
{
	UComputeShaderFunctionLibrary::ClearShader(TrailMapOutput, TrailTarget, Width, Height);
	UComputeShaderFunctionLibrary::ClearShader(DiffusedTrailMapOutput, DiffuseTarget, Width, Height);
	UComputeShaderFunctionLibrary::ClearShader(DisplayTrailMapOutput, DisplayTarget, Width, Height);
}

void UMoldV2ShaderComponent::UpdateShader()
{
	for (int i = 0; i < StepsPerFrame; i++) {
		ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("MoldV2"));

			UpdateMold_RenderThread(RHICommands, GraphBuilder);
			DiffuseParticles_RenderThread(RHICommands, GraphBuilder);
			MapColors_RenderThread(RHICommands, GraphBuilder);
		});
	}
}

void UMoldV2ShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}

void UMoldV2ShaderComponent::UpdateMold_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder) {
		CheckRenderBuffers(RHICommands);

		if (Time > MoldV2Constants::StartDelay)
		{
			const TShaderMapRef<FUpdateShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(GraphBuilder.RegisterExternalTexture(TrailMapOutput), ERDGUnorderedAccessViewFlags::SkipBarrier);

			FUpdateShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FUpdateShaderDeclaration::FParameters>();
			Params->SpeciesSettings = SpeciesBuffer.GetUAV();
			Params->NumSpecies = NumSpecies;
			Params->Agents = AgentsBuffer.GetUAV();
			Params->NumAgents = AmountOfAgents;
			Params->TrailMap = TargetUAV;
			Params->Width = Width;
			Params->Height = Height;
			Params->TrailWeight = TrailWeight;
			Params->DeltaTime = CurrentDeltaTime;
			Params->Time = Time;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("UpdateMoldV2Pass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(AmountOfAgents, 1, 1));
		}
}

void UMoldV2ShaderComponent::DiffuseParticles_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder) {
	CheckRenderBuffers(RHICommands);

	{
		const TShaderMapRef<FDiffuseShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		const FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(TrailMapOutput);
		const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(DiffusedTrailMapOutput);
		
		FRDGTextureUAV* SourceUAV = GraphBuilder.CreateUAV(SourceTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);
		FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

		FDiffuseShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FDiffuseShaderDeclaration::FParameters>();
		Params->TrailMap = SourceUAV;
		Params->DiffusedMap = TargetUAV;
		Params->Width = Width;
		Params->Height = Height;
		Params->DeltaTime = CurrentDeltaTime;
		Params->DecayRate = DecayRate;
		Params->DiffuseRate = DiffuseRate;

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("DiffuseMoldV2Pass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Width, Height, 1));

		AddCopyTexturePass(GraphBuilder, TargetTexture, SourceTexture);
	}
}

void UMoldV2ShaderComponent::MapColors_RenderThread(FRHICommandListImmediate& RHICommands, FRDGBuilder& GraphBuilder) {
	CheckRenderBuffers(RHICommands);
	
	{
		const TShaderMapRef<FColorMapShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		const FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(TrailMapOutput);
		const FRDGTextureRef DisplayTexture = GraphBuilder.RegisterExternalTexture(DisplayTrailMapOutput);
		const FRDGTextureRef DisplayTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(DisplayTarget->GetRenderTargetResource()->TextureRHI, TEXT("PerlinRenderTarget")));
		
		FRDGTextureUAV* SourceUAV = GraphBuilder.CreateUAV(SourceTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);
		FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(DisplayTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

		FColorMapShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FColorMapShaderDeclaration::FParameters>();
		Params->SpeciesSettings = SpeciesBuffer.GetUAV();
		Params->NumSpecies = NumSpecies;
		Params->TrailMap = SourceUAV;
		Params->Width = Width;
		Params->Height = Height;
		Params->ColorMap = TargetUAV;

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("MapColorsPass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Width, Height, 1));

		AddCopyTexturePass(GraphBuilder, DisplayTexture, DisplayTargetTexture);
		
		GraphBuilder.Execute();
	}
}