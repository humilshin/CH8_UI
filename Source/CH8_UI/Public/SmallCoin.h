// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "SmallCoin.generated.h"

/**
 * 
 */
UCLASS()
class CH8_UI_API ASmallCoin : public ACoinItem
{
	GENERATED_BODY()
public:
	ASmallCoin();

	virtual void ActivateItem(AActor* Activator) override;
};
