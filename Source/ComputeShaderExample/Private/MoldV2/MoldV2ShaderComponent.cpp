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


// Called when the game starts
void UMoldV2ShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), TEXTURE_WIDTH, TEXTURE_HEIGHT, ETextureRenderTargetFormat::RTF_RGBA8);
	Reset();
	
}

void UMoldV2ShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	NumSpecies = Species.Num();

	FRandomStream rng;
	{
		TResourceArray<FAgentV2> agentsResourceArray;
		agentsResourceArray.Init(FAgentV2(), amountOfAgents);
		for (FAgentV2& Agent : agentsResourceArray)
		{
			FVector2f Center = FVector2f(width / 2.f, height / 2.f);
			FVector2f Position;
			const float RandomAngle = rng.FRand() * 2 * PI;
			const FVector randomVec = rng.GetUnitVector();
			float Angle = 0.f;

			switch(spawnMode)
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

			FVector3d SpeciesMask;
			int speciesIndex = 0;

			if (NumSpecies == 1)
			{
				SpeciesMask = FVector3d::ForwardVector;
			}
			else
			{
				int species = rng.RandRange(1, NumSpecies);
				speciesIndex = species - 1;
				SpeciesMask = FVector3d((species == 1) ? 1 : 0, (species == 2) ? 1 : 0, (species == 3) ? 1 : 0);
			}

			Agent.position = Position;
			Agent.angle = Angle;
			Agent.speciesIndex = speciesIndex;
			Agent.speciesMask = SpeciesMask;

			/*UE_LOG(LogTemp, Log, TEXT("Position: %s"), *Agent.position.ToString())
			UE_LOG(LogTemp, Log, TEXT("Angle: %s"), *FString::SanitizeFloat(Agent.angle))
			UE_LOG(LogTemp, Log, TEXT("Index: %d"), Agent.speciesIndex)
			UE_LOG(LogTemp, Log, TEXT("Mask: %s"), *Agent.speciesMask.ToString())*/
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(FAgentV2), sizeof(FAgentV2) * amountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}

	{
		TResourceArray<FSpeciesSettings> speciesResourceArray;
		speciesResourceArray.Init(FSpeciesSettings(), NumSpecies);

		for (int Index = 0; Index < NumSpecies; Index++)
		{
			speciesResourceArray[Index] = Species[Index];
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &speciesResourceArray;

		_speciesBuffer = RHICreateStructuredBuffer(sizeof(FSpeciesSettings), sizeof(FSpeciesSettings) * NumSpecies, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_speciesBufferUAV = RHICreateUnorderedAccessView(_speciesBuffer, false, false);
	}
}


// Called every frame
void UMoldV2ShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Delta = DeltaTime;
	Time += Delta;

	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			if (!TrailMapOutput.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
				FPooledRenderTargetDesc TrailMapOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(TrailMap->SizeX, TrailMap->SizeY), TrailMap->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
				TrailMapOutputDesc.DebugName = TEXT("TrailMap_Output_RenderTarget");
				GRenderTargetPool.FindFreeElement(RHICommands, TrailMapOutputDesc, TrailMapOutput, TEXT("TrailMap_Output_RenderTarget"));
			}

			if (!DiffusedTrailMapOutput.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
				FPooledRenderTargetDesc DiffuseOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DiffusedTrailMap->SizeX, DiffusedTrailMap->SizeY), DiffusedTrailMap->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
				DiffuseOutputDesc.DebugName = TEXT("DiffusedTrailMap_Output_RenderTarget");
				GRenderTargetPool.FindFreeElement(RHICommands, DiffuseOutputDesc, DiffusedTrailMapOutput, TEXT("DiffusedTrailMap_Output_RenderTarget"));
			}

			if (!DisplayTrailMapOutput.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
				FPooledRenderTargetDesc DisplayOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(DisplayTrailMap->SizeX, DisplayTrailMap->SizeY), DisplayTrailMap->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
				DisplayOutputDesc.DebugName = TEXT("DisplayTrailMap_Output_RenderTarget");
				GRenderTargetPool.FindFreeElement(RHICommands, DisplayOutputDesc, DisplayTrailMapOutput, TEXT("DisplayTrailMap_Output_RenderTarget"));
			}

			{
				TShaderMapRef<FUpdateShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->speciesSettings.GetBaseIndex(), _speciesBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numSpecies.GetBaseIndex(), sizeof(int), &NumSpecies);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), _agentsBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numAgents.GetBaseIndex(), sizeof(int), &amountOfAgents);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), DisplayTrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->trailWeight.GetBaseIndex(), sizeof(float), &trailWeight);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->time.GetBaseIndex(), sizeof(float), &Time);

				RHICommands.SetComputeShader(rhiComputeShader);

				DispatchComputeShader(RHICommands, cs, amountOfAgents, 1, 1);

				{
					RHICommands.CopyTexture(DisplayTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DisplayTrailMap->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});
		
		/*ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			{
				TShaderMapRef<FDiffuseShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), DisplayTrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->deltaTime.GetBaseIndex(), sizeof(float), &Delta);

				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->decayRate.GetBaseIndex(), sizeof(float), &decayRate);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->diffuseRate.GetBaseIndex(), sizeof(float), &diffuseRate);
				RHICommands.SetUAVParameter(rhiComputeShader, cs->DiffusedTrailMap.GetBaseIndex(), DisplayTrailMapOutput->GetRenderTargetItem().UAV);

				RHICommands.SetComputeShader(rhiComputeShader);

				DispatchComputeShader(RHICommands, cs, width, height, 1);

				{
					RHICommands.CopyTexture(DisplayTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, TrailMap->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});*/


		/*ENQUEUE_RENDER_COMMAND(FComputeShaderRunner3)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			{
				TShaderMapRef<FColorMapShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

				FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

				RHICommands.SetUAVParameter(rhiComputeShader, cs->speciesSettings.GetBaseIndex(), _speciesBufferUAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->numSpecies.GetBaseIndex(), sizeof(int), &NumSpecies);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->TrailMap.GetBaseIndex(), TrailMapOutput->GetRenderTargetItem().UAV);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->width.GetBaseIndex(), sizeof(int), &width);
				RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->height.GetBaseIndex(), sizeof(int), &height);

				RHICommands.SetUAVParameter(rhiComputeShader, cs->ColourMap.GetBaseIndex(), DisplayTrailMapOutput->GetRenderTargetItem().UAV);

				RHICommands.SetComputeShader(rhiComputeShader);

				DispatchComputeShader(RHICommands, cs, width, height, 1);

				{
					RHICommands.CopyTexture(DisplayTrailMapOutput->GetRenderTargetItem().ShaderResourceTexture, DisplayTrailMap->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
				}
			}
		});*/
}

