// Fill out your copyright notice in the Description page of Project Settings.


#include "ComputeShaderDeclarations.h"

#include "ShaderCompilerCore.h"

FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
	
}

FBoidShaderDeclaration::FBoidShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	positions.Bind(Initializer.ParameterMap, TEXT("positions"));
	times.Bind(Initializer.ParameterMap, TEXT("times"));
	agents.Bind(Initializer.ParameterMap, TEXT("agents"));
}

void FBoidShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

IMPLEMENT_SHADER_TYPE(, FBoidShaderDeclaration, TEXT("/ComputeShaderPlugin/Boid.usf"), TEXT("MainComputeShader"), SF_Compute);

FMoldShaderDeclaration::FMoldShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	agents.Bind(Initializer.ParameterMap, TEXT("agents"));
	trailmap.Bind(Initializer.ParameterMap, TEXT("trailmap"));
	numAgents.Bind(Initializer.ParameterMap, TEXT("numAgents"));
	width.Bind(Initializer.ParameterMap, TEXT("width"));
	height.Bind(Initializer.ParameterMap, TEXT("height"));
	moveSpeed.Bind(Initializer.ParameterMap, TEXT("moveSpeed"));
	deltaTime.Bind(Initializer.ParameterMap, TEXT("deltaTime"));
	Time.Bind(Initializer.ParameterMap, TEXT("Time"));
}



void FMoldShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.SetDefine(TEXT("TEXTURE_WIDTH"), TEXTURE_WIDTH);
	OutEnvironment.SetDefine(TEXT("TEXTRE_HEIGHT"), TEXTURE_HEIGHT);
}

IMPLEMENT_SHADER_TYPE(, FMoldShaderDeclaration, TEXT("/ComputeShaderPlugin/Mold.usf"), TEXT("MainComputeShader"), SF_Compute);


FOldDiffuseShaderDeclaration::FOldDiffuseShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	trailmap.Bind(Initializer.ParameterMap, TEXT("trailmap"));
	width.Bind(Initializer.ParameterMap, TEXT("width"));
	height.Bind(Initializer.ParameterMap, TEXT("height"));
	deltaTime.Bind(Initializer.ParameterMap, TEXT("deltaTime"));
	decayRate.Bind(Initializer.ParameterMap, TEXT("decayRate"));
	diffuseRate.Bind(Initializer.ParameterMap, TEXT("diffuseRate"));
	DiffusedTrailMap.Bind(Initializer.ParameterMap, TEXT("DiffusedTrailMap"));
}



void FOldDiffuseShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
	OutEnvironment.SetDefine(TEXT("TEXTURE_WIDTH"), TEXTURE_WIDTH);
	OutEnvironment.SetDefine(TEXT("TEXTRE_HEIGHT"), TEXTURE_HEIGHT);
}

IMPLEMENT_SHADER_TYPE(, FOldDiffuseShaderDeclaration, TEXT("/ComputeShaderPlugin/DiffuseCS.usf"), TEXT("Diffuse"), SF_Compute);

FUpdateShaderDeclaration::FUpdateShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	speciesSettings.Bind(Initializer.ParameterMap, TEXT("speciesSettings"));
	numSpecies.Bind(Initializer.ParameterMap, TEXT("numSpecies"));

	agents.Bind(Initializer.ParameterMap, TEXT("agents"));
	numAgents.Bind(Initializer.ParameterMap, TEXT("numAgents"));

	TrailMap.Bind(Initializer.ParameterMap, TEXT("TrailMap"));
	width.Bind(Initializer.ParameterMap, TEXT("width"));
	height.Bind(Initializer.ParameterMap, TEXT("height"));

	trailWeight.Bind(Initializer.ParameterMap, TEXT("trailWeight"));

	deltaTime.Bind(Initializer.ParameterMap, TEXT("deltaTime"));
	time.Bind(Initializer.ParameterMap, TEXT("time"));
}

void FUpdateShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FUpdateShaderDeclaration, TEXT("/ComputeShaderPlugin/MoldV2.usf"), TEXT("Update"), SF_Compute);

FDiffuseShaderDeclaration::FDiffuseShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	TrailMap.Bind(Initializer.ParameterMap, TEXT("TrailMap"));
	width.Bind(Initializer.ParameterMap, TEXT("width"));
	height.Bind(Initializer.ParameterMap, TEXT("height"));

	deltaTime.Bind(Initializer.ParameterMap, TEXT("deltaTime"));

	decayRate.Bind(Initializer.ParameterMap, TEXT("decayRate"));
	diffuseRate.Bind(Initializer.ParameterMap, TEXT("diffuseRate"));
	DiffusedTrailMap.Bind(Initializer.ParameterMap, TEXT("DiffusedTrailMap"));
}

void FDiffuseShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FDiffuseShaderDeclaration, TEXT("/ComputeShaderPlugin/DiffuseV2.usf"), TEXT("Diffuse"), SF_Compute);

FColorMapShaderDeclaration::FColorMapShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	speciesSettings.Bind(Initializer.ParameterMap, TEXT("speciesSettings"));
	numSpecies.Bind(Initializer.ParameterMap, TEXT("numSpecies"));

	TrailMap.Bind(Initializer.ParameterMap, TEXT("TrailMap"));
	width.Bind(Initializer.ParameterMap, TEXT("width"));
	height.Bind(Initializer.ParameterMap, TEXT("height"));

	ColourMap.Bind(Initializer.ParameterMap, TEXT("ColourMap"));
}

void FColorMapShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FColorMapShaderDeclaration, TEXT("/ComputeShaderPlugin/ColorV2.usf"), TEXT("UpdateColourMap"), SF_Compute);

FPerlinShaderDeclaration::FPerlinShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FComputeShaderDeclaration(Initializer)
{
	texture.Bind(Initializer.ParameterMap, TEXT("noisemap"));
	start.Bind(Initializer.ParameterMap, TEXT("start"));
	dimensions.Bind(Initializer.ParameterMap, TEXT("dimensions"));
	offset.Bind(Initializer.ParameterMap, TEXT("offset"));
}

void FPerlinShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FPerlinShaderDeclaration, TEXT("/ComputeShaderPlugin/PerlinNoise.usf"), TEXT("GenerateNoise"), SF_Compute);

