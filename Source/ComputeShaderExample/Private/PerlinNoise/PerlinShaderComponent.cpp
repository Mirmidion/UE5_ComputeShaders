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

void UPerlinShaderComponent::Generate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FPerlinShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->texture.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->start.GetBaseIndex(), sizeof(FVector3Float), &Position);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->dimensions.GetBaseIndex(), sizeof(FIntVector2d), &Dimensions);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->offset.GetBaseIndex(), sizeof(float), &Offset);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->octaves.GetBaseIndex(), sizeof(int), &Octaves);

			RHICommands.SetComputeShader(rhiComputeShader);

			DispatchComputeShader(RHICommands, cs, Dimensions.X, Dimensions.Y, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}


// Called every frame
void UPerlinShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Generate();
}

