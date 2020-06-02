// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState :uint8;
/**
 * 
 */
UCLASS()
class UE4_TPS_GAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	ASGameMode();
	

	int32 NumOfBotsToSpawn;//本波生成敌人数量
	int32 WaveCount;//波次
	float TimeBetweenWaves;//两波之间的时间间隔
	FTimerHandle TimerHandle_SpawnInterval;
	FTimerHandle TimerHandle_NextWaveStart;

	//在蓝图中实现生成一个低级怪物的功能
	UFUNCTION(BlueprintImplementableEvent,Category="GameMode")
	void SpawnNewBot();

	//开始生成一波怪物
	void StartWave();

	//一波结束
	void EndWave();

	//为下一波设定时间，确定多久生成下一波
	void PrepareForNextWave();

	void SpawnBotsTimerElapsed();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();
public:
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;
	 
};
