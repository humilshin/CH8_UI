#include "BaseGameState.h"
#include "BaseGameInstance.h"
#include "CH8_UI/CH8_UICharacter.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ABaseGameState::ABaseGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWave = 0;
	MaxWaves = 3;
	WaveDurations = {40.0f, 30.0f, 20.0f};
	ItemsPerWave = {30, 40, 60};
}

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		return;
	}

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ABaseGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ABaseGameState::GetScore() const
{
	return Score;
}

void ABaseGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UBaseGameInstance* SpartaGameInstance = Cast<UBaseGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ABaseGameState::StartLevel()
{
	if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharacter->ShowGameHUD();
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UBaseGameInstance* SpartaGameInstance = Cast<UBaseGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	CurrentWave = 0;
	StartWave();
}

void ABaseGameState::StartWave()
{
	ClearAllItems();

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UUserWidget* HUDWidget = PlayerCharacter->GetHUDWidget())
		{
			if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveText"))))
			{
				WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Start!"), CurrentWave + 1)));
				WaveText->SetVisibility(ESlateVisibility::Visible);

				GetWorldTimerManager().SetTimer(
					WaveTextTimerHandle,
					this,
					&ABaseGameState::HideWaveText,
					2.0f,
					false
				);
			}
		}
	}

	int32 ItemToSpawn = 40;
	if (ItemsPerWave.IsValidIndex(CurrentWave))
	{
		ItemToSpawn = ItemsPerWave[CurrentWave];
	}

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	float Duration = 30.0f;
	if (WaveDurations.IsValidIndex(CurrentWave))
	{
		Duration = WaveDurations[CurrentWave];
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ABaseGameState::OnWaveTimeUp,
		Duration,
		false
	);
}

void ABaseGameState::OnWaveTimeUp()
{
	OnGameOver();
}

void ABaseGameState::ClearAllItems()
{
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);

	for (AActor* Item : FoundItems)
	{
		if (Item)
		{
			Item->Destroy();
		}
	}
}

void ABaseGameState::NextLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UBaseGameInstance* BaseGameInstance = Cast<UBaseGameInstance>(GameInstance);
		if (BaseGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			BaseGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			if (CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}

			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}

void ABaseGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);

		CurrentWave++;

		if (CurrentWave < MaxWaves)
		{
			StartWave();
		}
		else
		{
			NextLevel();
		}
	}
}

void ABaseGameState::OnGameOver()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTextTimerHandle);

	if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerCharacter->ShowMainMenu(true);
	}
}

void ABaseGameState::HideWaveText()
{
	if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UUserWidget* HUDWidget = PlayerCharacter->GetHUDWidget())
		{
			if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveText"))))
			{
				WaveText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void ABaseGameState::UpdateHUD()
{
	if (ACH8_UICharacter* PlayerCharacter = Cast<ACH8_UICharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UUserWidget* HUDWidget = PlayerCharacter->GetHUDWidget())
		{
			if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
			{
				float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
				TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
			}

			if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
			{
				if (UGameInstance* GameInstance = GetGameInstance())
				{
					UBaseGameInstance* BaseGameInstance = Cast<UBaseGameInstance>(GameInstance);
					if (BaseGameInstance)
					{
						ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), BaseGameInstance->TotalScore)));
					}
				}
			}

			if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
			{
				LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
			}
		}
	}
}
