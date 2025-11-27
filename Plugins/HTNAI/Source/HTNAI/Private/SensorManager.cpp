// Fill out your copyright notice in the Description page of Project Settings.
#include "SensorManager.h"

USensorManager* USensorManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			const UGameInstance* GameInstance = World->GetGameInstance();
			if (GameInstance) return GameInstance->GetSubsystem<USensorManager>();
		}
	}
	return nullptr;
}

AActor* USensorManager::FindNearestOfType(UClass* Type, FVector FromLocation)
{
	if (Objects.IsEmpty()) return nullptr;
	
	// sort objects array 
	Objects.Sort([FromLocation](const AActor& A, const AActor& B)
	{
		const float FastDistA = FVector::DistSquared(FromLocation, A.GetActorLocation());
		const float FastDistB = FVector::DistSquared(FromLocation, B.GetActorLocation());
		return FastDistA > FastDistB;
	});
	
	return Objects[0];
}

void USensorManager::AddObject(AActor* Object)
{
	Objects.Add(Object);
}

void USensorManager::RemoveObject(AActor* Object)
{
	Objects.Remove(Object);
}
