#include "HealingItem.h"
#include "CH8_UI/CH8_UICharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(Activator))
		{
			// 캐릭터의 체력을 회복
			PlayerCharacter->AddHealth(HealAmount);
		}
			
		DestroyItem();
	}
}