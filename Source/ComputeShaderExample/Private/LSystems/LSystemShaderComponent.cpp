// Fill out your copyright notice in the Description page of Project Settings.


#include "LSystem/LSystemShaderComponent.h"
#include "ComputeShaderDeclarations.h"
#include <Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h>

#include "RenderGraphUtils.h"
#include "Kismet/KismetStringLibrary.h"
#include "Static/ComputeShaderFunctionLibrary.h"
#include "TypeDefinitions/SerializableSettings.h"

DEFINE_LOG_CATEGORY(LogLSystemShader)

namespace LSystemConstants
{
	static constexpr int StartDelay = 1;
	static constexpr int UpdatesPerTick = 10;
}

ULSystemShaderComponent::ULSystemShaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULSystemShaderComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CreateRenderTarget();
	
	InitShader();
}

void ULSystemShaderComponent::CreateRenderTarget()
{
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, RTF_RGBA8);
	RenderTarget->LODGroup = TEXTUREGROUP_EffectsNotFiltered;
}

void ULSystemShaderComponent::InitShader()
{
	Reset();
	
	GenerateLines();

	CreateLineBuffer();
	
	ClearShader();
}

void ULSystemShaderComponent::Reset()
{
	Lines.Empty();

	Time = 0;
	BufferTime = 0;
}

void ULSystemShaderComponent::GenerateLines()
{
	TArray<FVector2D> PositionStack;
	TArray<float> RotationStack;
	FVector2D CurrentLocation = FVector2D(0, 0);
	float Angle = 180;
	
	FString Result = GetIteratedString(Start);
	for (int Index = 0; Index < Result.Len(); Index++)
	{
		const FString character = UKismetStringLibrary::GetSubstring(Result, Index, 1);
		const FLSystemRuleList Rule = Actions.FindRef(character);

		const float AngleRadians = (Angle + 90.f) / 180.f * PI;
		FVector2D CalculatedDirection = FVector2D(cos(AngleRadians), sin(AngleRadians));
		for (FLSystemRule SingleRule : Rule.List)
		{
			switch (SingleRule.Action)
			{
			case ELSystemAction::Back:
				{
					const FVector2D StartOfLine = CurrentLocation;
					CurrentLocation -= SingleRule.ActionValue * CalculatedDirection;
					const FVector2D EndOfLine = CurrentLocation;
					Lines.Add(F2DLine(StartOfLine, EndOfLine));
				}
				break;
			case ELSystemAction::Forward:
				{
					const FVector2D StartOfLine = CurrentLocation;
					CurrentLocation += SingleRule.ActionValue * CalculatedDirection;
					const FVector2D EndOfLine = CurrentLocation;
					Lines.Add(F2DLine(StartOfLine, EndOfLine));
				}
				break;
			case ELSystemAction::PopPosition:
				{
					if (PositionStack.Num())
					{
						CurrentLocation = PositionStack.Last();
						PositionStack.RemoveAt(PositionStack.Num() - 1);
						Angle = RotationStack.Last();
						RotationStack.RemoveAt(RotationStack.Num() - 1);
					}
				}
				break;
			case ELSystemAction::PushPosition:
				{
					PositionStack.Add(CurrentLocation);
					RotationStack.Add(Angle);
				}
				break;
			case ELSystemAction::TurnLeft:
				{
					Angle -= SingleRule.ActionValue;
				}
				break;
			case ELSystemAction::TurnRight:
				{
					Angle += SingleRule.ActionValue;
				}
				break;
			default:
				{

				}
				break;
			}
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

		const float FactorX = Width / (Max.X - Min.X), FactorY = Height / (Max.Y - Min.Y);
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
		FVector2Float Offset = FVector2Float((Width - SystemWidth) / 2.f, (Height - SystemHeight) / 2.f);
		for (F2DLine& Line : Lines)
		{
			Line.End = Line.End + Offset;
			Line.Start = Line.Start + Offset;
		}
	}

	if (!Lines.Num())
		Lines.Add(F2DLine());

	AmountOfLines = Lines.Num();
}

void ULSystemShaderComponent::CreateLineBuffer()
{
	if (LinesBuffer.IsValid())
	{
		LinesBuffer.SafeRelease();
	}

	if (LinesBufferUAV.IsValid())
	{
		LinesBufferUAV.SafeRelease();
	}
	
	UE_LOG(LogLSystemShader, Log, TEXT("Creating lines buffer with %d lines"), Lines.Num())
	
	FRHIResourceCreateInfo CreateInfo{ *FString("LineBufferInfo") };
	CreateInfo.ResourceArray = &Lines;

	LinesBuffer = RHICreateStructuredBuffer(sizeof(F2DLine), sizeof(F2DLine) * Lines.Num(), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
	LinesBufferUAV = RHICreateUnorderedAccessView(LinesBuffer, false, false);
}

void ULSystemShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bPaused)
	{
		Time += CurrentDeltaTime = DeltaTime;

		if (Time > LSystemConstants::StartDelay)
		{
			BufferTime += CurrentDeltaTime;
		}

		for (int i = 0; i < LSystemConstants::UpdatesPerTick; i++)
		{
			UpdateShader();
		}
	}
}

void ULSystemShaderComponent::SerializeSettings(const FString Name)
{
	FLSystemSettings Settings;
	Settings.Actions = Actions;
	Settings.Iterations = SystemIterations;
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
		SystemIterations = Settings.Iterations;
		Rules = Settings.Rules;
		Start = Settings.Start;
		
		InitShader();

		if (OnDeSerializeDone.IsBound())
		{
			OnDeSerializeDone.Broadcast();
		}
		
		return true;
	}
	return false;
}

TArray<FString> ULSystemShaderComponent::GetAllSettings()
{
	const FString Directory = FPaths::ProjectDir() / "Saves/LSystem/";
	
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	
	TArray<FString> Out;
	FileManager.FindFiles(Out, *Directory, *FString(".txt"));
	
	return Out;
}

FString ULSystemShaderComponent::GetIteratedString(FString ToUse)
{
	FString Previous = ToUse;
	FString New;
	for (int i = 0; i < SystemIterations; i++)
	{
		for (int Index = 0; Index < Previous.Len(); Index++)
		{
			const FString Character = UKismetStringLibrary::GetSubstring(Previous, Index, 1);
			FString ToAppend = Character;
			for (auto& Rule : Rules)
			{
				if (Character == Rule.Key)
				{
					ToAppend = Rule.Value;
					break;
				}
			}
			New.Append(ToAppend);
		}

		UE_LOG(LogLSystemShader, Log, TEXT("Iteration length %d"), New.Len())
		Previous = New;
		New = "";
	}
	return Previous;
}

FString ULSystemShaderComponent::GetStartString() const
{
	return Start;
}

void ULSystemShaderComponent::SetStartString(const FString& Value)
{
	Start = Value;
}

TMap<FString, FString> ULSystemShaderComponent::GetReplaceRules() const
{
	return Rules;
}

void ULSystemShaderComponent::SetReplaceRules(const TMap<FString, FString> NewRules)
{
	Rules = NewRules;
}

TMap<FString, FLSystemRuleList> ULSystemShaderComponent::GetCharacterActions() const
{
	return Actions;
}

void ULSystemShaderComponent::SetCharacterActions(const TMap<FString, FLSystemRuleList>& NewActions)
{
	Actions = NewActions;
}

int ULSystemShaderComponent::GetIterations() const
{
	return SystemIterations;
}

void ULSystemShaderComponent::SetIterations(const int Value)
{
	SystemIterations = Value;
}

float ULSystemShaderComponent::GetPercentagePerSecond() const
{
	return PercentagePerSecond;
}

void ULSystemShaderComponent::SetPercentagePerSecond(const float Value)
{
	PercentagePerSecond = Value;
}

void ULSystemShaderComponent::CheckRenderBuffers(FRHICommandListImmediate& RHICommands)
{
	if (!ComputeShaderOutput.IsValid())
	{
		const FRandomStream Random;

		const FString TextureBufferName = FString("LSystemCS_Output_RenderTarget").Append(FString::SanitizeFloat(Random.FRand()));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, true));
		ComputeShaderOutputDesc.DebugName = *TextureBufferName;
		GRenderTargetPool.FindFreeElement(RHICommands, ComputeShaderOutputDesc, ComputeShaderOutput, *TextureBufferName);
	}
}

void ULSystemShaderComponent::UpdateShader() {
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			CheckRenderBuffers(RHICommands);

			const TShaderMapRef<FLSystemShaderDeclaration> Shader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FLSystemShaderDeclaration::FParameters Params;
			Params.Lines = LinesBufferUAV.GetReference();
			Params.TrailMap = ComputeShaderOutput->GetRenderTargetItem().UAV;
			Params.NumLines = AmountOfLines;
			Params.Time = BufferTime;
			Params.PercentPerSecond = PercentagePerSecond;

			if (Time > LSystemConstants::StartDelay)
			{
				FComputeShaderUtils::Dispatch(RHICommands, Shader, Params, FIntVector(AmountOfLines, 1, 1));
			}

			RHICommands.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
		});
}

void ULSystemShaderComponent::TogglePaused()
{
	IComputeShaderBase::TogglePaused();
}

void ULSystemShaderComponent::ClearShader()
{
	if (ComputeShaderOutput.IsValid() && IsValid(RenderTarget))
	{
		UComputeShaderFunctionLibrary::ClearShader(ComputeShaderOutput, RenderTarget, Width, Height);
	}
}


