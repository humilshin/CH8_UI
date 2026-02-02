// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "BigCoin.generated.h"

/**
 * 
 */
UCLASS()
class CH8_UI_API ABigCoin : public ACoinItem
{
	GENERATED_BODY()

public:
	ABigCoin();
	
	virtual void ActivateItem(AActor* Activator) override;
};
