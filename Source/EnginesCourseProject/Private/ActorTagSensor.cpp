#include "ActorTagSensor.h"

#include "HTNComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UActorTagSensor::Tick()
{
	Super::Tick();
	
	WorldState.Value = false;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> OutActors;

	if (UKismetSystemLibrary::SphereOverlapActors(Owner->GetWorld(), Owner->GetOwner()->GetActorLocation(), SenseRadius,ObjectTypes, AActor::StaticClass(), TArray<AActor*>(),OutActors))
	{
		for (const auto& Actor : OutActors)
		{
			if (Actor->ActorHasTag(SearchTag))
			{
				WorldState.Value = true;
				ReceiveOnSensed();
			}
		}
	}
	Owner->UpdateWorldState(WorldState);
}

