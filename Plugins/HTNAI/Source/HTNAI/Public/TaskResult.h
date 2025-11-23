// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldState.h"
#include "TaskResult.generated.h"

UENUM(BlueprintType)
enum class ETaskState : uint8
{
	Success,
	Failed
};

USTRUCT(Blueprintable)
struct FTaskResult
{
	GENERATED_BODY()
	FTaskResult() : EndState(ETaskState::Success), Effect(FWorldStateContainer::EmptyWorldState), TargetActor(nullptr), PointOfInterest(FVector::Zero()), Message(FString()) { }
	explicit FTaskResult(ETaskState State): EndState(State), Effect(FWorldStateContainer::EmptyWorldState), TargetActor(nullptr), PointOfInterest(FVector::Zero()), Message(FString()) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETaskState EndState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldStateContainer Effect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PointOfInterest;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;
};