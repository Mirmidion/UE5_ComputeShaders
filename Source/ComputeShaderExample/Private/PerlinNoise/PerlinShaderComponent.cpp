// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinNoise/PerlinShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "Kismet/KismetMathLibrary.h"
#include "MoldV2/MoldV2ShaderComponent.h"

FIntVector2d::FIntVector2d(const int X, const int Y) : X(X), Y(Y)
{
}

FIntVector2d::FIntVector2d() : X(0), Y(0)
{
}

// Sets default values for this component's properties
UPerlinShaderComponent::UPerlinShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPerlinShaderComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), TEXTURE_WIDTH, TEXTURE_HEIGHT, ETextureRenderTargetFormat::RTF_RGBA8);
	Reset();
	
}

void UPerlinShaderComponent::Reset()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;
	TResourceArray<FVector2f> positionResourceArray;

	Time = 0;
}

void UPerlinShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}


// Called every frame
void UPerlinShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);
			
			TShaderMapRef<FPerlinShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();
			
			RHICommands.SetUAVParameter(rhiComputeShader, cs->texture.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->start.GetBaseIndex(), sizeof(FVector), &Position);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->dimensions.GetBaseIndex(), sizeof(FIntVector2d), &Dimensions);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->offset.GetBaseIndex(), sizeof(float), &Offset);

			RHICommands.SetComputeShader(rhiComputeShader);

			DispatchComputeShader(RHICommands, cs, width, height, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
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

				if (Paused)
				{
					for (int32 Index = 0; Index < agentsResourceArray.Num(); Index++)
					{
						FAgent& agent = agentsResourceArray[Index];
						FString ToPrint = FString("agent position: ").Append(agent.position.ToString());
						ToPrint = ToPrint.Append(" agent angle: ").Append(FString::SanitizeFloat(agent.angle));
						UE_LOG(LogTemp, Log, TEXT("%d"), Index);
						UE_LOG(LogTemp, Log, TEXT("%s"), *ToPrint);
					}
					Paused = false;
				}
			}*/
		});
}

