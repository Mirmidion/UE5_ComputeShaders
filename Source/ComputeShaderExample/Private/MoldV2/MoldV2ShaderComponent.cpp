// Fill out your copyright notice in the Description page of Project Settings.


#include "MoldV2/MoldV2ShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include "RenderTargetPool.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>



// Sets default values for this component's properties
UMoldV2ShaderComponent::UMoldV2ShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UMoldV2ShaderComponent::~UMoldV2ShaderComponent()
{

}


// Called when the game starts
void UMoldV2ShaderComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FSpeciesSettings Data;
	Data.colour = FVector4Float(1,0,0,1);
	Data.moveSpeed = 100;
	Data.sensorAngleDegrees = 45;
	Data.sensorOffsetDst = 75;
	Data.sensorSize = 10;
	Data.turnSpeed = 10;
	Species.Add(Data);

	Reset();
}

void UMoldV2ShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	NumSpecies = Species.Num();
	UE_LOG(LogTemp, Log, TEXT("Amount of species: %d"), NumSpecies)

	FRandomStream rng;
	{
		TResourceArray<FAgentV2> agentsResourceArray;
		agentsResourceArray.Init(FAgentV2(), amountOfAgents);
		for (int index = 0; index < amountOfAgents; index++)
		{
			FAgentV2& Agent = agentsResourceArray[index];
			FVector2f Center = FVector2f(width / 2.f, height / 2.f);
			FVector2Float Position;
			const float RandomAngle = rng.FRand() * 2 * PI;
			const FVector randomVec = rng.GetUnitVector();
			float Angle = 0.f;

			switch(spawnMode)
			{
			case ESpawnMode::Random:
			{
				Position = FVector2Float(rng.FRandRange(0, width), rng.FRandRange(0, height));
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
					FVector2f Value = Center + FVector2f(randomVec.X, randomVec.Y) * height * 0.5f;
					Position = FVector2Float(Value.X, Value.Y);
					FVector2f Normalized = Center - FVector2f(Position.X, Position.Y);
					Normalized.Normalize();
					Angle = FMath::Atan2(Normalized.Y, Normalized.X);
					break;
				}
			case ESpawnMode::RandomCircle:
			{
				FVector2f Value = Center + FVector2f(randomVec.X, randomVec.Y) * height * 0.15f;
				Position = FVector2Float(Value.X, Value.Y);
				Angle = RandomAngle;
				break;
			}
			}

			FIntVector4d SpeciesMask;
			int speciesIndex = 0;

			if (NumSpecies == 1)
			{
				SpeciesMask = FIntVector4d(1,0,0,0);
			}
			else
			{
				int species = rng.RandRange(1, NumSpecies);
				speciesIndex = species - 1;
				SpeciesMask = FIntVector4d((species == 1) ? 1 : 0, (species == 2) ? 1 : 0, (species == 3) ? 1 : 0, (species == 4) ? 1 : 0);
			}

			Agent.position = Position;
			Agent.angle = Angle;
			Agent.speciesIndex = speciesIndex;
			Agent.speciesMask = SpeciesMask;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(FAgentV2), sizeof(FAgentV2) * amountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}

	{
		TResourceArray<FSpeciesSettings> speciesResourceArray;
		speciesResourceArray.Append(Species);
		
		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &speciesResourceArray;

		_speciesBuffer = RHICreateStructuredBuffer(sizeof(FSpeciesSettings), sizeof(FSpeciesSettings) * NumSpecies, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_speciesBufferUAV = RHICreateUnorderedAccessView(_speciesBuffer, false, false);
	}

	Time = 0;

	DisplayTarget = nullptr;
	DisplayTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	DisplayTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	DiffuseTarget = nullptr;
	DiffuseTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	DiffuseTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	TrailTarget = nullptr;
	TrailTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	TrailTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UMoldV2ShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands, bool bForceUpdate)
{
	if (!TrailMapOutput.IsValid() || bForceUpdate)
	{
		FPooledRenderTargetDesc TrailMapOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(TrailTarget->SizeX, TrailTarget->SizeY), TrailTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		TrailMapOutputDesc.DebugName = TEXT("TrailMap_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, TrailMapOutputDesc, TrailMapOutput, TEXT("TrailMap_Output_RenderTarget"));
	}

	if (!DiffusedTrailMapOutput.IsValid() || bForceUpdate)
	{
		FPooledRenderTargetDesc DiffuseOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DiffuseTarget->SizeX, DiffuseTarget->SizeY), DiffuseTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		DiffuseOutputDesc.DebugName = TEXT("DiffusedTrailMap_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, DiffuseOutputDesc, DiffusedTrailMapOutput, TEXT("DiffusedTrailMap_Output_RenderTarget"));
	}

	if (!DisplayTrailMapOutput.IsValid() || bForceUpdate)
	{
		FPooledRenderTargetDesc DisplayOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DisplayTarget->SizeX, DisplayTarget->SizeY), DisplayTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		DisplayOutputDesc.DebugName = TEXT("DisplayTrailMap_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, DisplayOutputDesc, DisplayTrailMapOutput, TEXT("DisplayTrailMap_Output_RenderTarget"));
	}
}

void UMoldV2ShaderComponent::DoUpdate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands, Time == Delta);

			{
				TShaderMapRef<FUpdateShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->speciesSettings.GetBaseIndex(), _speciesBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numSpecies.GetBaseIndex(), sizeof(int), &NumSpecies);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), _agentsBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numAgents.GetBaseIndex(), sizeof(int), &amountOfAgents);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), TrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->trailWeight.GetBaseIndex(), sizeof(float), &trailWeight);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->time.GetBaseIndex(), sizeof(float), &Time);

				RHICommands.SetComputeShader(rhiComputeShader);

				if (Time > 3 )
					DispatchComputeShader(RHICommands, cs, amountOfAgents, 1, 1);

				{
					RHICommands.CopyTexture(TrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DiffusedTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, FRHICopyTextureInfo());
					RHICommands.CopyTexture(TrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, TrailTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});
}

void UMoldV2ShaderComponent::DoDiffuse() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands, false);

			{
				TShaderMapRef<FDiffuseShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), DiffusedTrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->decayRate.GetBaseIndex(), sizeof(float), &decayRate);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->diffuseRate.GetBaseIndex(), sizeof(float), &diffuseRate);
				RHICommands.SetUAVParameter(rhiComputeShader, cs->DiffusedTrailMap.GetBaseIndex(), TrailMapOutput->GetRenderTargetItem().UAV);

				RHICommands.SetComputeShader(rhiComputeShader);

				DispatchComputeShader(RHICommands, cs, width, height, 1);

				{
					RHICommands.CopyTexture(TrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DisplayTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
					RHICommands.CopyTexture(DiffusedTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DiffuseTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
					RHICommands.CopyTexture(TrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, TrailTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});
}

void UMoldV2ShaderComponent::DoColorMapping() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner3)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands, false);
			{
				TShaderMapRef<FColorMapShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->speciesSettings.GetBaseIndex(), _speciesBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numSpecies.GetBaseIndex(), sizeof(UINT), &NumSpecies);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), TrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->ColourMap.GetBaseIndex(), DisplayTrailMapOutput->GetRenderTargetItem().UAV);

				RHICommands.SetComputeShader(rhiComputeShader);

				DispatchComputeShader(RHICommands, cs, width, height, 1);

				{
					RHICommands.CopyTexture(DisplayTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DisplayTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});
}

// Called every frame
void UMoldV2ShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Paused)
		return;

	Delta = DeltaTime;
	Time += Delta;

	for (int i = 0; i < StepsPerFrame; i++) {
		DoUpdate();
		DoDiffuse();
		//continue;
		DoColorMapping();
	}
}

