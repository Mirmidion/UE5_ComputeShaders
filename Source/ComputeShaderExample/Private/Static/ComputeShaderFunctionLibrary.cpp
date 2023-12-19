// Fill out your copyright notice in the Description page of Project Settings.


#include "Static/ComputeShaderFunctionLibrary.h"

#include "ComputeShaderDeclarations.h"
#include "RenderGraphUtils.h"

void UComputeShaderFunctionLibrary::ClearShader(const TRefCountPtr<IPooledRenderTarget>& ComputeShaderOutput,
                                                const UTextureRenderTarget2D* RenderTarget, const int& Width, const int& Height)
{
	if (!ComputeShaderOutput.IsValid() || !IsValid(RenderTarget) || !ComputeShaderOutput->GetRenderTargetItem().IsValid())
	{
		return;
	}

	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			const TShaderMapRef<FClearShaderDeclaration> Shader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FClearShaderDeclaration::FParameters Params;
			Params.TextureToClear = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.Width = Width;
			Params.Height = Height;

			FComputeShaderUtils::Dispatch(RHICommands, Shader, Params, FIntVector(Width, Height, 1));
		});
}
