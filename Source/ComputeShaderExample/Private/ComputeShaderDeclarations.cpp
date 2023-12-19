// Fill out your copyright notice in the Description page of Project Settings.


#include "ComputeShaderDeclarations.h"

#include "ShaderCompilerCore.h"


FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
	
}

void FBoidShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

IMPLEMENT_SHADER_TYPE(, FBoidShaderDeclaration, TEXT("/ComputeShaderPlugin/Boid.usf"), TEXT("MainComputeShader"), SF_Compute);

void FMoldShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.SetDefine(TEXT("TEXTURE_WIDTH"), TEXTURE_WIDTH);
	OutEnvironment.SetDefine(TEXT("TEXTRE_HEIGHT"), TEXTURE_HEIGHT);
}

IMPLEMENT_SHADER_TYPE(, FMoldShaderDeclaration, TEXT("/ComputeShaderPlugin/Mold.usf"), TEXT("MainComputeShader"), SF_Compute);

void FUpdateShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FUpdateShaderDeclaration, TEXT("/ComputeShaderPlugin/MoldV2.usf"), TEXT("Update"), SF_Compute);

void FDiffuseShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FDiffuseShaderDeclaration, TEXT("/ComputeShaderPlugin/DiffuseV2.usf"), TEXT("Diffuse"), SF_Compute);

void FColorMapShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FColorMapShaderDeclaration, TEXT("/ComputeShaderPlugin/ColorV2.usf"), TEXT("UpdateColorMap"), SF_Compute);

void FPerlinShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_GLOBAL_SHADER(FPerlinShaderDeclaration, "/ComputeShaderPlugin/PerlinNoise.usf", "GenerateNoise", SF_Compute);

void FLinesShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FLinesShaderDeclaration, TEXT("/ComputeShaderPlugin/RandomLines.usf"), TEXT("MainComputeShader"), SF_Compute);

void FMandelbrotShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_GLOBAL_SHADER(FMandelbrotShaderDeclaration, "/ComputeShaderPlugin/MandelbrotSet.usf", "MainComputeShader", SF_Compute);

void FLSystemShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FLSystemShaderDeclaration, TEXT("/ComputeShaderPlugin/LSystems.usf"), TEXT("MainComputeShader"), SF_Compute);

void FClearShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
}

IMPLEMENT_SHADER_TYPE(, FClearShaderDeclaration, TEXT("/ComputeShaderPlugin/ClearCS.usf"), TEXT("ClearTexture"), SF_Compute);