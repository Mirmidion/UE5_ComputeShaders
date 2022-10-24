// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "RHICommandList.h"

#include <atomic>
#include "CoreMinimal.h"

class FComputeShaderDeclaration : public FGlobalShader
{
public:
	FComputeShaderDeclaration(){}
	explicit FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
};

class FBoidShaderDeclaration : public FComputeShaderDeclaration
{
	DECLARE_SHADER_TYPE(FBoidShaderDeclaration, Global);

	FBoidShaderDeclaration() {}

	explicit FBoidShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);


public:

	LAYOUT_FIELD(FShaderResourceParameter, positions);
	LAYOUT_FIELD(FShaderResourceParameter, times);
	LAYOUT_FIELD(FShaderResourceParameter, agents);
};

class FMoldShaderDeclaration : public FComputeShaderDeclaration
{
#define TEXTURE_WIDTH 1920
#define TEXTURE_HEIGHT 1080


	DECLARE_SHADER_TYPE(FMoldShaderDeclaration, Global);

	FMoldShaderDeclaration() {}

	explicit FMoldShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);


public:

	LAYOUT_FIELD(FShaderResourceParameter, agents);
	LAYOUT_FIELD(FShaderResourceParameter, trailmap);
	LAYOUT_FIELD(FShaderResourceParameter, numAgents);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);
	LAYOUT_FIELD(FShaderResourceParameter, moveSpeed);
	LAYOUT_FIELD(FShaderResourceParameter, deltaTime);
	LAYOUT_FIELD(FShaderResourceParameter, Time);
};

class FUpdateShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FUpdateShaderDeclaration, Global);

	FUpdateShaderDeclaration() {}

	explicit FUpdateShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:

	LAYOUT_FIELD(FShaderResourceParameter, speciesSettings);
	LAYOUT_FIELD(FShaderResourceParameter, numSpecies);

	LAYOUT_FIELD(FShaderResourceParameter, agents);
	LAYOUT_FIELD(FShaderResourceParameter, numAgents);

	LAYOUT_FIELD(FShaderResourceParameter, TrailMap);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);

	LAYOUT_FIELD(FShaderResourceParameter, trailWeight);

	LAYOUT_FIELD(FShaderResourceParameter, deltaTime);
	LAYOUT_FIELD(FShaderResourceParameter, time);
};

class FDiffuseShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FDiffuseShaderDeclaration, Global);

	FDiffuseShaderDeclaration() {}

	explicit FDiffuseShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, TrailMap);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);

	LAYOUT_FIELD(FShaderResourceParameter, deltaTime);

	LAYOUT_FIELD(FShaderResourceParameter, decayRate);
	LAYOUT_FIELD(FShaderResourceParameter, diffuseRate);
	LAYOUT_FIELD(FShaderResourceParameter, DiffusedTrailMap);
};

class FColorMapShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FColorMapShaderDeclaration, Global);

	FColorMapShaderDeclaration() {}

	explicit FColorMapShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, speciesSettings);
	LAYOUT_FIELD(FShaderResourceParameter, numSpecies);

	LAYOUT_FIELD(FShaderResourceParameter, TrailMap);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);

	LAYOUT_FIELD(FShaderResourceParameter, ColourMap);
};

class FPerlinShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FPerlinShaderDeclaration, Global);

	FPerlinShaderDeclaration() {}

	explicit FPerlinShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, texture);
	LAYOUT_FIELD(FShaderResourceParameter, start); 
	LAYOUT_FIELD(FShaderResourceParameter, posY);
	LAYOUT_FIELD(FShaderResourceParameter, dimensions);
	LAYOUT_FIELD(FShaderResourceParameter, offset);
	LAYOUT_FIELD(FShaderResourceParameter, octaves);
};

class FLinesShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FLinesShaderDeclaration, Global);

	FLinesShaderDeclaration() {}

	explicit FLinesShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, agents);
	LAYOUT_FIELD(FShaderResourceParameter, trailmap);
	LAYOUT_FIELD(FShaderResourceParameter, numAgents);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);
	LAYOUT_FIELD(FShaderResourceParameter, deltaTime);
	LAYOUT_FIELD(FShaderResourceParameter, Time);
};

class FMandelbrotShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FMandelbrotShaderDeclaration, Global);

	FMandelbrotShaderDeclaration() {}

	explicit FMandelbrotShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, trailmap);
	LAYOUT_FIELD(FShaderResourceParameter, width);
	LAYOUT_FIELD(FShaderResourceParameter, height);
	LAYOUT_FIELD(FShaderResourceParameter, center);
	LAYOUT_FIELD(FShaderResourceParameter, zoom);
	LAYOUT_FIELD(FShaderResourceParameter, iterations);
	LAYOUT_FIELD(FShaderResourceParameter, mode);
};

class FLSystemShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FLSystemShaderDeclaration, Global);

	FLSystemShaderDeclaration() {}

	explicit FLSystemShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, Lines);
	LAYOUT_FIELD(FShaderResourceParameter, Trailmap);
	LAYOUT_FIELD(FShaderResourceParameter, NumLines);
	LAYOUT_FIELD(FShaderResourceParameter, Time);
	LAYOUT_FIELD(FShaderResourceParameter, PercentagePerSecond);
};

class FClearShaderDeclaration : public FComputeShaderDeclaration
{

	DECLARE_SHADER_TYPE(FClearShaderDeclaration, Global);

	FClearShaderDeclaration() {}

	explicit FClearShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

public:
	LAYOUT_FIELD(FShaderResourceParameter, Trailmap);
	LAYOUT_FIELD(FShaderResourceParameter, Width);
	LAYOUT_FIELD(FShaderResourceParameter, Height);
};