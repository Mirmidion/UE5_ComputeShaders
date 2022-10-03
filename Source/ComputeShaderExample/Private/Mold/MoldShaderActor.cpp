// Fill out your copyright notice in the Description page of Project Settings.


#include "Mold/MoldShaderActor.h"

#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
AMoldShaderActor::AMoldShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Component = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Component);
}

// Called when the game starts
void AMoldShaderActor::BeginPlay()
{
	Super::BeginPlay();

	Reset();
}

void AMoldShaderActor::ResetShader()
{
	InitAgents(SpawnMode, AmountOfAgents);

	Time = 0;

	InitRenderTargets();
}

void AMoldShaderActor::CheckRenderBuffers(FRHICommandListImmediate& RHICommands, bool bForceUpdate)
{
	if (!RenderShaderOutput.IsValid())
	{
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, RenderShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}

	if (!BufferShaderOutput.IsValid())
	{
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(BufferRenderTarget->SizeX, BufferRenderTarget->SizeY), BufferRenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget2");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, BufferShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget2"));
	}
}

void AMoldShaderActor::DoUpdate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FMoldShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), AgentsBufferUAV);
			RHICommands.SetUAVParameter(rhiComputeShader, cs->trailmap.GetBaseIndex(), RenderShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numAgents.GetBaseIndex(), sizeof(UINT), &AmountOfAgents);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &Width);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &Height);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->moveSpeed.GetBaseIndex(), sizeof(float), &Speed);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Time.GetBaseIndex(), sizeof(float), &Time);

			RHICommands.SetComputeShader(rhiComputeShader);

			if (Time > 3)
				DispatchComputeShader(RHICommands, cs, AmountOfAgents, 1, 1);

			{
				RHICommands.CopyTexture(RenderShaderOutput->GetRenderTargetItem().ShaderResourceTexture, BufferShaderOutput->GetRenderTargetItem().ShaderResourceTexture, FRHICopyTextureInfo());
			}
		});
}

void AMoldShaderActor::DoDiffuse() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FDiffuseShaderDeclaration> cs2(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader2 = cs2.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader2, cs2->TrailMap.GetBaseIndex(), BufferShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->width.GetBaseIndex(), sizeof(int), &Width);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->height.GetBaseIndex(), sizeof(int), &Height);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->decayRate.GetBaseIndex(), sizeof(float), &DecayRate);
			RHICommands.SetShaderParameter(rhiComputeShader2, cs2->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs2->diffuseRate.GetBaseIndex(), sizeof(float), &DiffuseRate);
			RHICommands.SetUAVParameter(rhiComputeShader2, cs2->DiffusedTrailMap.GetBaseIndex(), RenderShaderOutput->GetRenderTargetItem().UAV);

			RHICommands.SetComputeShader(rhiComputeShader2);

			DispatchComputeShader(RHICommands, cs2, Width, Height, 1);

			{
				RHICommands.CopyTexture(RenderShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

void AMoldShaderActor::InitAgents(ESpawnMode spawnMode, int Amount)
{
	{
		FRandomStream RandomStream;
		TResourceArray<FAgent> agentsResourceArray;
		agentsResourceArray.Init(FAgent(), AmountOfAgents);

		for (int i = 0; i < AmountOfAgents; i++)
		{
			FAgent& Agent = agentsResourceArray[i];

			FVector2f Center = FVector2f(Width / 2.f, Height / 2.f);
			FVector2f Position;
			const float RandomAngle = RandomStream.FRand() * 2 * PI;
			const FVector RandomVec = RandomStream.GetUnitVector();
			float Angle = 0.f;

			switch (spawnMode)
			{
			case ESpawnMode::Random:
			{
				Position = FVector2f(RandomStream.FRandRange(0, Width), RandomStream.FRandRange(0, Height));
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
				Position = Center + FVector2f(RandomVec.X, RandomVec.Y) * Height * 0.5f;
				FVector2f Normalized = Center - Position;
				Normalized.Normalize();
				Angle = FMath::Atan2(Normalized.Y, Normalized.X);
				break;
			}
			case ESpawnMode::RandomCircle:
			{
				Position = Center + FVector2f(RandomVec.X, RandomVec.Y) * Height * 0.15f;
				Angle = RandomAngle;
				break;
			}
			}

			Agent.position = Position;
			Agent.angle = Angle;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		AgentsBuffer = RHICreateStructuredBuffer(sizeof(FAgent), sizeof(FAgent) * AmountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		AgentsBufferUAV = RHICreateUnorderedAccessView(AgentsBuffer, false, false);
	}
}

void AMoldShaderActor::InitRenderTargets()
{
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	BufferRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	BufferRenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

// Called every frame
void AMoldShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPaused)
	{
		Delta = DeltaTime;
		Time += Delta;

		DoUpdate();
		DoDiffuse();
	}
}