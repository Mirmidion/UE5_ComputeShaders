// Fill out your copyright notice in the Description page of Project Settings.


#include "LSystem/LSystemShaderComponent.h"

#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Static/ComputeShaderFunctionLibrary.h"
#include "TypeDefinitions/SerializableSettings.h"

// Sets default values for this component's properties
ULSystemShaderComponent::ULSystemShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULSystemShaderComponent::BeginPlay()
{
	Super::BeginPlay();
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), width, height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
	GenerateSystem();
}

void ULSystemShaderComponent::GenerateSystem()
{
	FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();
	Lines.Empty();
	TArray<FVector2D> PositionStack;
	TArray<float> RotationStack;
	FVector2D CurrentLocation = FVector2D(0, 0);
	float Angle = 180;
	
	//UE_LOG(LogTemp, Log, TEXT("First String: %s"), *Start);
	FString Result = GetIteratedString(Start);
	//UE_LOG(LogTemp, Log, TEXT("Final String: %s"), *Result);
	for (int Index = 0; Index < Result.Len(); Index++)
	{
		const FString character = UKismetStringLibrary::GetSubstring(Result, Index, 1);
		const FLSystemRule Rule = Actions.FindRef(character);

		//UE_LOG(LogTemp, Log, TEXT("Doing Action %s for character %s"), *UEnum::GetValueAsString(Rule.Action), *character)
		const float AngleRadians = (Angle + 90.f) / 180.f * PI;
		FVector2D CalculatedDirection = FVector2D(cos(AngleRadians), sin(AngleRadians));
		//UE_LOG(LogTemp, Log, TEXT("Angle: %s"), *CalculatedDirection.ToString())
		
		if (Rule.Action == ELSystemAction::Back)
			{
				const FVector2D StartOfLine = CurrentLocation;
				CurrentLocation -= Rule.ActionValue * CalculatedDirection;
				const FVector2D EndOfLine = CurrentLocation;
				Lines.Add(F2DLine(StartOfLine, EndOfLine));
			}
		if (Rule.Action == ELSystemAction::Forward)
			{
				const FVector2D StartOfLine = CurrentLocation;
				CurrentLocation += Rule.ActionValue * CalculatedDirection;
				const FVector2D EndOfLine = CurrentLocation;
				Lines.Add(F2DLine(StartOfLine, EndOfLine));
			}
			if (Rule.Action == ELSystemAction::PopPosition)
			{
				if (PositionStack.Num())
				{
					CurrentLocation = PositionStack.Last();
					PositionStack.RemoveAt(PositionStack.Num() - 1);
					Angle = RotationStack.Last();
					RotationStack.RemoveAt(RotationStack.Num() - 1);
				}
			}
			if (Rule.Action == ELSystemAction::PushPosition)
			{
				PositionStack.Add(CurrentLocation);
				RotationStack.Add(Angle);
			}
			if (Rule.Action == ELSystemAction::TurnLeft)
			{
				Angle -= Rule.ActionValue;
			}
			if (Rule.Action == ELSystemAction::TurnRight)
			{
				Angle += Rule.ActionValue;
			}
		
	}

	{
		FVector2Float Min = FVector2Float(TNumericLimits<float>::Max()), Max = FVector2Float(TNumericLimits<float>::Min());
		for (const F2DLine& Line : Lines)
		{
			Min = FVector2D(FMath::Min(Line.Start.X, Min.X), FMath::Min(Line.Start.Y, Min.Y));
			Min = FVector2D(FMath::Min(Line.End.X, Min.X), FMath::Min(Line.End.Y, Min.Y));

			Max = FVector2D(FMath::Max(Line.Start.X, Max.X), FMath::Max(Line.Start.Y, Max.Y));
			Max = FVector2D(FMath::Max(Line.End.X, Max.X), FMath::Max(Line.End.Y, Max.Y));
		}

		//UE_LOG(LogTemp, Log, TEXT("Min: %s, Max: %s"), *Min.ToString(), *Max.ToString())

		for (F2DLine& Line : Lines)
		{
			Line.End = Line.End - Min;
			Line.Start = Line.Start - Min;
		}
	}

	{
		FVector2Float Min = FVector2Float(TNumericLimits<float>::Max()), Max = FVector2Float(TNumericLimits<float>::Min());
		for (const F2DLine& Line : Lines)
		{
			Min = FVector2D(FMath::Min(Line.Start.X, Min.X), FMath::Min(Line.Start.Y, Min.Y));
			Min = FVector2D(FMath::Min(Line.End.X, Min.X), FMath::Min(Line.End.Y, Min.Y));

			Max = FVector2D(FMath::Max(Line.Start.X, Max.X), FMath::Max(Line.Start.Y, Max.Y));
			Max = FVector2D(FMath::Max(Line.End.X, Max.X), FMath::Max(Line.End.Y, Max.Y));
		}

		const float FactorX = width / (Max.X - Min.X), FactorY = height / (Max.Y - Min.Y);
		float HighestFactor = FMath::Min(FactorX, FactorY);
		HighestFactor *= 0.8f;
		for (F2DLine& Line : Lines)
		{
			Line.End = Line.End * HighestFactor;
			Line.Start = Line.Start * HighestFactor;
		}
	}

	{
		FVector2Float Min = FVector2Float(TNumericLimits<float>::Max()), Max = FVector2Float(TNumericLimits<float>::Min());
		for (const F2DLine& Line : Lines)
		{
			Min = FVector2D(FMath::Min(Line.Start.X, Min.X), FMath::Min(Line.Start.Y, Min.Y));
			Min = FVector2D(FMath::Min(Line.End.X, Min.X), FMath::Min(Line.End.Y, Min.Y));

			Max = FVector2D(FMath::Max(Line.Start.X, Max.X), FMath::Max(Line.Start.Y, Max.Y));
			Max = FVector2D(FMath::Max(Line.End.X, Max.X), FMath::Max(Line.End.Y, Max.Y));
		}

		float SystemWidth = Max.X - Min.X;
		float SystemHeight = Max.Y - Min.Y;
		FVector2Float Offset = FVector2Float((width - SystemWidth) / 2.f, (height - SystemHeight) / 2.f);
		for (F2DLine& Line : Lines)
		{
			Line.End = Line.End + Offset;
			Line.Start = Line.Start + Offset;
		}
	}

	if (!Lines.Num())
		Lines.Add(F2DLine());

	AmountOfLines = Lines.Num();

	TResourceArray<F2DLine> BufferLines;
	BufferLines.Append(Lines);
	FRHIResourceCreateInfo createInfo{ *FString("") };
	createInfo.ResourceArray = &BufferLines;

	UE_LOG(LogTemp, Log, TEXT("Size: %d"), BufferLines.Num())
	_linesBuffer = RHICreateStructuredBuffer(sizeof(F2DLine), sizeof(F2DLine) * Lines.Num(), BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	_linesBufferUAV = RHICreateUnorderedAccessView(_linesBuffer, false, false);

	Time = 0;
	BufferTime = 0;

	ClearTarget();
}

void ULSystemShaderComponent::SerializeSettings(FString Name)
{
	FLSystemSettings Settings;
	Settings.Actions = Actions;
	Settings.Iterations = iterations;
	Settings.Rules = Rules;
	Settings.Start = Start;
	UComputeShaderFunctionLibrary::SerializeStruct(Settings, FPaths::ProjectDir().Append("/Saves/" + Name + ".txt"));
}

bool ULSystemShaderComponent::DeSerializeSettings(FString Path)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (FileManager.FileExists(*Path))
	{
		FLSystemSettings Settings = UComputeShaderFunctionLibrary::DeSerializeStruct<FLSystemSettings>(Path);
		Actions = Settings.Actions;
		iterations = Settings.Iterations;
		Rules = Settings.Rules;
		Start = Settings.Start;
		GenerateSystem();
		return true;
	}
	return false;
}

TArray<FString> ULSystemShaderComponent::GetAllSettings()
{
	TArray<FString> Out;
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FileManager.FindFiles(Out, *FPaths::ProjectDir().Append("Saves/LSystem/"), *FString(".txt"));
	return Out;
}

FString ULSystemShaderComponent::GetIteratedString(FString ToUse)
{
	FString Previous = ToUse;
	FString New;
	for (int i = 0; i < iterations; i++)
	{
		for (int Index = 0; Index < Previous.Len(); Index++)
		{
			const FString Character = UKismetStringLibrary::GetSubstring(Previous, Index, 1);
			FString ToAppend = Character;
			//UE_LOG(LogTemp, Log, TEXT("Replacing: %s"), *Character)
			for (auto& Rule : Rules)
			{
				if (Character == Rule.Key)
				{
					ToAppend = Rule.Value;
					break;
				}
			}
			//UE_LOG(LogTemp, Log, TEXT("Append: %s"), *ToAppend)
			New.Append(ToAppend);
		}
		//UE_LOG(LogTemp, Log, TEXT("Iteration %d: %s"), i,  *New)
		UE_LOG(LogTemp, Log, TEXT("Iteration length %d"), New.Len())
		Previous = New;
		New = "";
	}
	return Previous;
}

void ULSystemShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		FRandomStream rng;
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *FString("WhiteNoiseCS_Output_RenderTarget").Append(FString::SanitizeFloat(rng.FRand()));
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}
}

void ULSystemShaderComponent::DoUpdate() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FLSystemShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->Lines.GetBaseIndex(), _linesBufferUAV);
			RHICommands.SetUAVParameter(rhiComputeShader, cs->Trailmap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->NumLines.GetBaseIndex(), sizeof(int), &AmountOfLines);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Time.GetBaseIndex(), sizeof(float), &BufferTime);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->PercentagePerSecond.GetBaseIndex(), sizeof(float), &PercentagePerSecond);

			RHICommands.SetComputeShader(rhiComputeShader);

			if (Time > 1 && !Paused)
				DispatchComputeShader(RHICommands, cs, AmountOfLines, 1, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

void ULSystemShaderComponent::ClearTarget()
{
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			TShaderMapRef<FClearShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

			RHICommands.SetUAVParameter(rhiComputeShader, cs->Trailmap.GetBaseIndex(), ComputeShaderOutput->GetRenderTargetItem().UAV);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Width.GetBaseIndex(), sizeof(int), &width);
			RHICommands.SetShaderParameter(rhiComputeShader, cs->ParameterMapInfo.LooseParameterBuffers[0].BaseIndex, cs->Height.GetBaseIndex(), sizeof(int), &height);

			RHICommands.SetComputeShader(rhiComputeShader);

			DispatchComputeShader(RHICommands, cs, width, height, 1);

			{
				RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
			}
		});
}

// Called every frame
void ULSystemShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Paused)
	{
		Delta = DeltaTime;
		Time += Delta;
		if (Time > 1)
			BufferTime += Delta;

		for (int i = 0; i < 10; i++)
			DoUpdate();
	}
}

