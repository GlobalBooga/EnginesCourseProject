// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSensor.h"
#include "HTNComponent.h"
#include "ItemActor.h"
#include "SensorManager.h"

void UItemSensor::Tick()
{
	Super::Tick();
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ItemSensor")

	WorldState.Value = false;

	const auto Result = SensorManager->FindNearestOfType(AItemActor::StaticClass(), Owner->GetOwner()->GetActorLocation());
	if (Result)
	{
		WorldState.Value = true;
		ReceiveOnSensed();
	}
	
	/*
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> OutActors;

	if (UKismetSystemLibrary::SphereOverlapActors(Owner->GetWorld(), Owner->GetOwner()->GetActorLocation(), SenseRadius,ObjectTypes, AItemActor::StaticClass(), TArray<AActor*>(),OutActors))
	{
		for (const auto& Actor : OutActors)
		{
			if (Cast<AItemActor>(Actor)->HasMatchingGameplayTag(ObjectTag))
			{
				WorldState.Value = true;
				ReceiveOnSensed();
			}
		}
	}
	*/
	
	//UE_LOG(LogTemp, Warning, TEXT("UItemSensor::Tick"));
	Owner->UpdateWorldState(WorldState);
}

