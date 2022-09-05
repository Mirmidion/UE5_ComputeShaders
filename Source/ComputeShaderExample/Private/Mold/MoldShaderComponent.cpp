// Fill out your copyright notice in the Description page of Project Settings.


#include "Mold/MoldShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include "RenderTargetPool.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

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

	
	//RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), TEXTURE_WIDTH, TEXTURE_HEIGHT, ETextureRenderTargetFormat::RTF_RGBA8);
	Reset();
	
}

void UMoldShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;
	TResourceArray<FVector2f> positionResourceArray;

	{
		positionResourceArray.Init(FVector2f(0, 0), amountOfAgents);


		for (FVector2f& position : positionResourceArray)
		{
			FVector rand = rng.GetUnitVector();
			FVector2f flat = FVector2f(rand.X, rand.Y);
			position = FVector2f(960, 540);
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &positionResourceArray;
	}



	{
		TResourceArray<FAgent> agentsResourceArray;
		agentsResourceArray.Init(FAgent(), amountOfAgents);

		for (int i = 0; i < positionResourceArray.Num(); i++)
		{
			FVector2f& position = positionResourceArray[i];
			FAgent& agent = agentsResourceArray[i];
			agent.position = position;
			agent.angle = rng.FRand() * 2 * PI;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(FAgent), sizeof(FAgent) * amountOfAgents, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}
}


// Called every frame
void UMoldShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Delta = DeltaTime;
	Time += Delta;
	
	
	
	
		
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{

			if (!ComputeShaderOutput.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
				FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
				ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
				GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
			}

			if (!BufferShaderOutput.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
				FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(BufferRenderTarget->SizeX, BufferRenderTarget->SizeY), BufferRenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
				ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget2");
				GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, BufferShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget2"));
			}

			
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

			// read back the data
			/*{
				TResourceArray<FAgent> agentsResourceArray;
				agentsResourceArray.SetAllowCPUAccess(true);
				agentsResourceArray.Init(FAgent(), amountOfAgents);
				uint8* data = (uint8*)RHILockBuffer(_agentsBuffer, 0, amountOfAgents * sizeof(FAgent), RLM_ReadOnly);
				FMemory::Memcpy(agentsResourceArray.GetData(), data, amountOfAgents * sizeof(FAgent));

				RHIUnlockBuffer(_agentsBuffer);
				if (agentsResourceArray.GetAllowCPUAccess())
				{
					FString ToPrint = FString("Delta: ").Append(FString::SanitizeFloat(Delta)).Append(" speed: ").Append(FString::SanitizeFloat(speed));
					ToPrint = ToPrint.Append(" height: ").Append(FString::FromInt(height));
					ToPrint = ToPrint.Append(" width: ").Append(FString::FromInt(width));
					ToPrint = ToPrint.Append(" num agents: ").Append(FString::FromInt(amountOfAgents));
					ToPrint = ToPrint.Append(" agent1angle: ").Append(FString::SanitizeFloat(agentsResourceArray[0].angle));
					UE_LOG(LogTemp, Log, TEXT("%s"), *agentsResourceArray[0].position.ToString());
					UE_LOG(LogTemp, Log, TEXT("%s"), *ToPrint);

				}

				if (Running)
				{
					for (int32 Index = 0; Index < agentsResourceArray.Num(); Index++)
					{
						FAgent& agent = agentsResourceArray[Index];
						FString ToPrint = FString("agent position: ").Append(agent.position.ToString());
						ToPrint = ToPrint.Append(" agent angle: ").Append(FString::SanitizeFloat(agent.angle));
						UE_LOG(LogTemp, Log, TEXT("%d"), Index);
						UE_LOG(LogTemp, Log, TEXT("%s"), *ToPrint);
					}
					Running = false;
				}
			}*/
		});

	
		ENQUEUE_RENDER_COMMAND(FComputeShaderRunner2)(
			[&](FRHICommandListImmediate& RHICommands)
			{

			TShaderMapRef<FOldDiffuseShaderDeclaration> cs2(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader2 = cs2.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader2, cs2->trailmap.GetBaseIndex(), BufferShaderOutput->GetRenderTargetItem().UAV);
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
				RHICommands.CopyTexture(BufferShaderOutput->GetRenderTargetItem().ShaderResourceTexture, BufferRenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}


			
		});
}

