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

	const auto Result = SensorManager->FindNearestWithTag(ObjectTag, Owner->GetOwner()->GetActorLocation());
	if (Result)
	{
		WorldState.Value = true;
		ReceiveOnSensed();
	}
	
	Owner->UpdateWorldState(WorldState);
}

