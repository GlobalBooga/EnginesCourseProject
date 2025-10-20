// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListItemObject.generated.h"

/**
 * 
 */
UCLASS()
class ENGINESCOURSEPROJECT_API UListItemObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	FText DisplayText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	bool bState;
};