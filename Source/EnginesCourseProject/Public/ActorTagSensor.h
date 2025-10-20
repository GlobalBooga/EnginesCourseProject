// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sensor.h"
#include "ActorTagSensor.generated.h"

/**
 * 
 */
UCLASS()
class ENGINESCOURSEPROJECT_API UActorTagSensor : public USensor
{
	GENERATED_BODY()

public:
	virtual void Tick() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnSensed"))
	void ReceiveOnSensed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SenseRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SearchTag;
};
