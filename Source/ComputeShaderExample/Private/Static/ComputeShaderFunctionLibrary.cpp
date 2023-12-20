// Fill out your copyright notice in the Description page of Project Settings.


#include "Static/ComputeShaderFunctionLibrary.h"

#include "ComputeShaderDeclarations.h"
#include "RenderGraphUtils.h"

void UComputeShaderFunctionLibrary::ClearShader(const TRefCountPtr<IPooledRenderTarget>& ComputeShaderOutput,
                                                UTextureRenderTarget2D* RenderTarget, const int& Width, const int& Height)
{
	if (!ComputeShaderOutput.IsValid() || !IsValid(RenderTarget) || !ComputeShaderOutput->GetRHI()->IsValid())
	{
		return;
	}

	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[ComputeShaderOutput, RenderTarget, Width, Height](FRHICommandListImmediate& RHICommands)
		{
			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("ClearShader"));

			const TShaderMapRef<FClearShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(GraphBuilder.RegisterExternalTexture(ComputeShaderOutput), ERDGUnorderedAccessViewFlags::SkipBarrier);

			FClearShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FClearShaderDeclaration::FParameters>();
			Params->TextureToClear = TargetUAV;
			Params->Width = Width;
			Params->Height = Height;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("ClearShaderPass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Width, Height, 1));

			GraphBuilder.Execute();

			RHICommands.CopyTexture(ComputeShaderOutput->GetRHI(), RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
		});
}
