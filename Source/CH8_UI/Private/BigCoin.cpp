// Fill out your copyright notice in the Description page of Project Settings.


#include "BigCoin.h"

ABigCoin::ABigCoin()
{
	PointValue = 50;
	ItemType = "BigCoin";
}

void ABigCoin::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}