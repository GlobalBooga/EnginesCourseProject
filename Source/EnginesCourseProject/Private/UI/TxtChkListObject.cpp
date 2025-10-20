// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TxtChkListObject.h"
#include "UI/ListItemObject.h"
#include "Components/CheckBox.h"

void UTxtChkListObject::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);
	
	const UListItemObject* ListItem = Cast<UListItemObject>(ListItemObject);
	if (!ListItem || !State) return;

	State->SetIsChecked(ListItem->bState);
}
