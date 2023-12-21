// Fill out your copyright notice in the Description page of Project Settings.


#include "DecayingLine/DecayingLineComponent.h"

#include "ComputeShaderDeclarations.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "Kismet/KismetRenderingLibrary.h"


UDecayingLineComponent::UDecayingLineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDecayingLineComponent::BeginPlay()
{
	Super::BeginPlay();

	InitShader();
}

void UDecayingLineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Time += DeltaTime;
	Delta = DeltaTime;
	
	UpdateShader();
	Diffuse();
}

void UDecayingLineComponent::InitShader()
{
	Time = 0;

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Dimensions.X, Dimensions.Y, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;

	BufferRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Dimensions.X, Dimensions.Y, RTF_RGBA8);
	BufferRenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void UDecayingLineComponent::ClearShader()
{
	
}

void UDecayingLineComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("DecayingLineCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("DecayingLineCS_Output_RenderTarget"));
	}

	if (!BufferShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(BufferRenderTarget->SizeX, BufferRenderTarget->SizeY), BufferRenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = TEXT("DiffuseCS_Output_RenderTarget2");
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, BufferShaderOutput, TEXT("DiffuseCS_Output_RenderTarget2"));
	}
}

void UDecayingLineComponent::UpdateShader() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("GenerateLine"));

			const TShaderMapRef<FDecayingLineShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
			const FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(RenderTarget->GetRenderTargetResource()->TextureRHI, TEXT("DecayingLineRenderTarget")));
			
			FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

			FDecayingLineShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FDecayingLineShaderDeclaration::FParameters>();
			Params->Texture = TargetUAV;
			Params->Start = Position;
			Params->Dimensions = Dimensions;
			Params->Time = Time;
			Params->Offset = Offset;
			Params->Octaves = Octaves;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("GeneratePerlinPass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Dimensions.X, 1, 1));

			AddCopyTexturePass(GraphBuilder, TargetTexture, RenderTargetTexture);
			
			GraphBuilder.Execute();
		});
}

void UDecayingLineComponent::Diffuse()
{
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);
			
			FRDGBuilder GraphBuilder(RHICommands, RDG_EVENT_NAME("DiffuseLine"));

			const TShaderMapRef<FDiffuseUpShaderDeclaration> Shader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			const FRDGTextureRef TargetTexture = GraphBuilder.RegisterExternalTexture(ComputeShaderOutput);
			const FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(BufferShaderOutput);
			const FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(RenderTarget->GetRenderTargetResource()->TextureRHI, TEXT("DiffuseRenderTarget")));

			FRDGTextureUAV* SourceUAV = GraphBuilder.CreateUAV(SourceTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);
			FRDGTextureUAV* TargetUAV = GraphBuilder.CreateUAV(TargetTexture, ERDGUnorderedAccessViewFlags::SkipBarrier);

			FDiffuseUpShaderDeclaration::FParameters* Params = GraphBuilder.AllocParameters<FDiffuseUpShaderDeclaration::FParameters>();
			Params->TrailMap = SourceUAV;
			Params->Width = Dimensions.X;
			Params->Height = Dimensions.Y;
			Params->DeltaTime = Delta;
			Params->DecayRate = DecayRate;
			Params->DiffuseRate = DiffuseRate;
			Params->DiffusedMap = TargetUAV;

			AddCopyTexturePass(GraphBuilder, TargetTexture, SourceTexture);

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("DiffuseLinePass"), ERDGPassFlags::Compute, Shader, Params, FIntVector(Dimensions.X, Dimensions.Y, 1));

			AddCopyTexturePass(GraphBuilder, TargetTexture, RenderTargetTexture);

			GraphBuilder.Execute();
		});
}

void UDecayingLineComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}


