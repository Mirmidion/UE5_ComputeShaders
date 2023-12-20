// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "RHICommandList.h"

#include <atomic>
#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"
#include "TypeDefinitions/CustomTypeDefinitions.h"


class FComputeShaderDeclaration : public FGlobalShader
{
public:
	FComputeShaderDeclaration(){}
	explicit FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
};

class FBoidShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FBoidShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FBoidShaderDeclaration, FComputeShaderDeclaration);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FAgent>, Agents)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Positions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float>, Times)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FMoldShaderDeclaration : public FComputeShaderDeclaration
{
#define TEXTURE_WIDTH 1920
#define TEXTURE_HEIGHT 1080

	DECLARE_SHADER_TYPE(FMoldShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FMoldShaderDeclaration, FComputeShaderDeclaration);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FAgentV2>, Agents)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(UINT, NumAgents) 
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
		SHADER_PARAMETER(float, MoveSpeed) 
		SHADER_PARAMETER(float, DeltaTime) 
		SHADER_PARAMETER(float, Time) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FUpdateShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FUpdateShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FUpdateShaderDeclaration, FComputeShaderDeclaration);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FSpeciesSettings>, SpeciesSettings)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FAgentV2>, Agents)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(UINT, NumSpecies) 
		SHADER_PARAMETER(UINT, NumAgents) 
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
		SHADER_PARAMETER(float, TrailWeight) 
		SHADER_PARAMETER(float, DeltaTime) 
		SHADER_PARAMETER(float, Time) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FDiffuseShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FDiffuseShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FDiffuseShaderDeclaration, FComputeShaderDeclaration);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, DiffusedMap)
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
		SHADER_PARAMETER(float, DeltaTime) 
		SHADER_PARAMETER(float, DecayRate) 
		SHADER_PARAMETER(float, DiffuseRate) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FColorMapShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FColorMapShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FColorMapShaderDeclaration, FComputeShaderDeclaration);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FSpeciesSettings>, SpeciesSettings)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, ColorMap)
		SHADER_PARAMETER(UINT, NumSpecies) 
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

};

class FPerlinShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_GLOBAL_SHADER(FPerlinShaderDeclaration);

	SHADER_USE_PARAMETER_STRUCT(FPerlinShaderDeclaration, FComputeShaderDeclaration);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, NoiseMap)
		SHADER_PARAMETER(FIntVector2d, Dimensions) 
		SHADER_PARAMETER(FVector3Float, Start) 
		SHADER_PARAMETER(float, Offset) 
		SHADER_PARAMETER(int, Octaves) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FLinesShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FLinesShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FLinesShaderDeclaration, FComputeShaderDeclaration);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float2>, Agents)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(int, NumAgents) 
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
		SHADER_PARAMETER(float, DeltaTime) 
		SHADER_PARAMETER(float, Time) 
	END_SHADER_PARAMETER_STRUCT()
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FMandelbrotShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_GLOBAL_SHADER(FMandelbrotShaderDeclaration);

	SHADER_USE_PARAMETER_STRUCT(FMandelbrotShaderDeclaration, FComputeShaderDeclaration);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
		SHADER_PARAMETER(FVector2Float, Center) 
		SHADER_PARAMETER(float, Zoom) 
		SHADER_PARAMETER(int, Iterations) 
		SHADER_PARAMETER(int, Mode) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FLSystemShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FLSystemShaderDeclaration, Global);
	
	SHADER_USE_PARAMETER_STRUCT(FLSystemShaderDeclaration, FComputeShaderDeclaration);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<F2DLine>, Lines)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TrailMap)
		SHADER_PARAMETER(int, NumLines) 
		SHADER_PARAMETER(float, Time)
		SHADER_PARAMETER(float, PercentPerSecond)
	END_SHADER_PARAMETER_STRUCT()
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FClearShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FClearShaderDeclaration, Global);

	SHADER_USE_PARAMETER_STRUCT(FClearShaderDeclaration, FComputeShaderDeclaration);
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TextureToClear)
		SHADER_PARAMETER(int, Width) 
		SHADER_PARAMETER(int, Height) 
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};