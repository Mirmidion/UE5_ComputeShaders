// Fill out your copyright notice in the Description page of Project Settings.


#include "Boids/BoidShaderComponent.h"


#include "ComputeShaderDeclarations.h"
#include "ShaderCompilerCore.h"

#include "RHIStaticStates.h"

// Some useful links
// -----------------
// [Enqueue render commands using lambdas](https://github.com/EpicGames/UnrealEngine/commit/41f6b93892dcf626a5acc155f7d71c756a5624b0)
//



// Sets default values for this component's properties
UBoidShaderComponent::UBoidShaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UBoidShaderComponent::BeginPlay()
{
	Super::BeginPlay();

    FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;
	TResourceArray<FVector3f> positionResourceArray;

	{
		positionResourceArray.Init(FVector3f(0,0,0), numBoids);


		for (FVector3f& position : positionResourceArray)
		{
			FVector flat = rng.GetUnitVector();
			flat.Z = 0;
			position = FVector3f(flat) * rng.GetFraction() * spawnRadius;
		}

		FRHIResourceCreateInfo createInfo{*FString("")};
		createInfo.ResourceArray = &positionResourceArray;

		_positionBuffer = RHICreateStructuredBuffer(sizeof(FVector3f), sizeof(FVector3f) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_positionBufferUAV = RHICreateUnorderedAccessView(_positionBuffer, false, false);
	}
    
	{
		TResourceArray<float> timesResourceArray;
		timesResourceArray.Init(0.0f, numBoids);

		for (int i =0; i < timesResourceArray.Num(); i++)
		{
			float& time = timesResourceArray[i];
			float DistanceFactor = positionResourceArray[i].Dist2D(positionResourceArray[i], FVector3f(0, 0, 0)) / spawnRadius;
			time = DistanceFactor * 4 * PI;
			UE_LOG(LogTemp, Log, TEXT("%s"), *FString::SanitizeFloat(time));
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &timesResourceArray;

		_timesBuffer = RHICreateStructuredBuffer(sizeof(float), sizeof(float) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_timesBufferUAV = RHICreateUnorderedAccessView(_timesBuffer, false, false);
	}

	{
		TResourceArray<Agent> agentsResourceArray;
		agentsResourceArray.Init(Agent(), numBoids);

		for (int i = 0; i < positionResourceArray.Num(); i++)
		{
			FVector3f& position = positionResourceArray[i];
			Agent& agent = agentsResourceArray[i];
			agent.position = position;
		}

		FRHIResourceCreateInfo createInfo{ *FString("") };
		createInfo.ResourceArray = &agentsResourceArray;

		_agentsBuffer = RHICreateStructuredBuffer(sizeof(Agent), sizeof(Agent) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_agentsBufferUAV = RHICreateUnorderedAccessView(_agentsBuffer, false, false);
	}

	if (outputPositions.Num() != numBoids)
	{
		const FVector3f zero(0.0f);
		outputPositions.Init(zero, numBoids);
	}
}

// Called every frame
void UBoidShaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
	[&](FRHICommandListImmediate& RHICommands)
	{
		TShaderMapRef<FBoidShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));


		FRHIComputeShader* rhiComputeShader = cs.GetComputeShader();

		RHICommands.SetUAVParameter(rhiComputeShader, cs->positions.GetBaseIndex(), _positionBufferUAV);
		RHICommands.SetUAVParameter(rhiComputeShader, cs->times.GetBaseIndex(), _timesBufferUAV);
		RHICommands.SetUAVParameter(rhiComputeShader, cs->agents.GetBaseIndex(), _agentsBufferUAV);

		RHICommands.SetComputeShader(rhiComputeShader);
		UE_LOG(LogTemp, Log, TEXT("%d:%d:%d"), cs->positions.GetBaseIndex(), cs->times.GetBaseIndex(), cs->agents.GetBaseIndex())

		DispatchComputeShader(RHICommands, cs, 256, 1, 1);

		// read back the data
		{
		/*uint8* data = (uint8*)RHILockBuffer(_positionBuffer, 0, numBoids * sizeof(FVector3f), RLM_ReadOnly);
		FMemory::Memcpy(outputPositions.GetData(), data, numBoids * sizeof(FVector3f));

		RHIUnlockBuffer(_positionBuffer);*/
		}
		{
			uint8* data = (uint8*)RHILockBuffer(_agentsBuffer, 0, numBoids * sizeof(Agent), RLM_ReadOnly);
			FMemory::Memcpy(outputPositions.GetData(), data, numBoids * sizeof(Agent));

			RHIUnlockBuffer(_agentsBuffer);
		}
	});
}


