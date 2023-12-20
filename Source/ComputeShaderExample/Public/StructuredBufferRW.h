// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template<typename T>
class TStructuredBufferRW 
{
public:
	TStructuredBufferRW()
	{
		bSafeToUse = false;
		BufferSize = 0;
	}
	
	FRHIUnorderedAccessView* GetUAV() const
	{
		return BufferUAV.GetReference();	
	}
	
	bool GetData(TArray<T>& Data)
	{
		if (bSafeToUse)
		{
			Data = BufferData;
		}

		return bSafeToUse;
	}
	
	void Read() const
	{
		ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			bSafeToUse = false;

			const uint8* Source = static_cast<uint8*>(RHILockBuffer(Buffer,0,BufferSize,RLM_ReadOnly));
			
			uint8* Destination = static_cast<uint8*>(BufferData.GetData());
			
			FMemory::Memcpy(Destination, Source, BufferSize);
			
			RHIUnlockBuffer(Buffer);

			bSafeToUse = true;
		});

		FRHIResourceInfo Info;
		Buffer->GetResourceInfo(Info);
	}

	void Write(const TArray<T>& Data)
	{
		TResourceArray<T> ResourceData;

		ResourceData.Append(Data);
		
		Write(ResourceData);
	}
	
	void Write(const TResourceArray<T>& Data)
	{
		BufferData = Data;

		BufferSize = sizeof(T) * BufferData.Num();

		if (Buffer.IsValid())
			Buffer.SafeRelease();

		if (BufferUAV.IsValid())
			BufferUAV.SafeRelease();
		
		ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&](FRHICommandListImmediate& RHICommands)
		{
			TResourceArray<T> ResourceArray;
			ResourceArray.Append(BufferData);
			
			FRHIResourceCreateInfo CreateInfo{ *FString("") };
			CreateInfo.ResourceArray = &ResourceArray;
			
			Buffer = RHICreateStructuredBuffer(sizeof(T), BufferSize, BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
			BufferUAV = RHICreateUnorderedAccessView(Buffer, false, false);
			
			bSafeToUse = true;
		});
		
	}

	template<typename U>
	friend class TStructuredBufferRW;

protected:
	TArray<T> BufferData;
	bool bSafeToUse;

	uint64 BufferSize;
	FBufferRHIRef Buffer;
	FUnorderedAccessViewRHIRef BufferUAV;

};




