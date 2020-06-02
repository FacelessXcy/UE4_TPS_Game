// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include <SHealthComponent.h>
#include "SGameState.h"

ASGameMode::ASGameMode()
{
	GameStateClass = ASGameState::StaticClass();
	this->TimeBetweenWaves = 2.0f;
	this->WaveCount = 0;
	//设置Tick调用频率
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

}

void ASGameMode::StartWave()
{
	this->WaveCount++;
	this->NumOfBotsToSpawn = 2 * this->WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnInterval, this, &ASGameMode::SpawnBotsTimerElapsed, 1.0f, true, 0.0f);
	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(this->TimerHandle_SpawnInterval);
	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, this->TimeBetweenWaves, false);
	SetWaveState(EWaveState::WaitingToStart);
	RestartDeadPlayers();
}

void ASGameMode::SpawnBotsTimerElapsed()
{
	SpawnNewBot();
	this->NumOfBotsToSpawn--;
	if (this->NumOfBotsToSpawn<=0)
	{
		EndWave();
	}
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(this->TimerHandle_NextWaveStart);

	if (this->NumOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}
	bool bIsAnyBotAlive = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn->GetFName().ToString().Contains("Bot"))
		{
			USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (HealthComp&&HealthComp->GetCurrentHealth()>0.0f)
			{
				bIsAnyBotAlive = true;
				break;
			}
		}
	}
	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();
		if (PC&&PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (HealthComp&&HealthComp->GetCurrentHealth()>0.0f)
			{
				return;
			}
		}
	}

	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);
	UE_LOG(LogTemp, Warning, TEXT("Player are all Died"));
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	if (GS)
	{
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::RestartDeadPlayers()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();
		if (PC&&!PC->GetPawn())
		{
			RestartPlayer(PC);
		}
	}
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckWaveState();
	CheckAnyPlayerAlive();
}
