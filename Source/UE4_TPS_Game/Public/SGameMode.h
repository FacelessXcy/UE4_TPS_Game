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
	

	int32 NumOfBotsToSpawn;//�������ɵ�������
	int32 WaveCount;//����
	float TimeBetweenWaves;//����֮���ʱ����
	FTimerHandle TimerHandle_SpawnInterval;
	FTimerHandle TimerHandle_NextWaveStart;

	//����ͼ��ʵ������һ���ͼ�����Ĺ���
	UFUNCTION(BlueprintImplementableEvent,Category="GameMode")
	void SpawnNewBot();

	//��ʼ����һ������
	void StartWave();

	//һ������
	void EndWave();

	//Ϊ��һ���趨ʱ�䣬ȷ�����������һ��
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
