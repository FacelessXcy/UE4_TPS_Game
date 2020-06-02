// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class UE4_TPS_GAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

	UPROPERTY(EditDefaultsOnly,Replicated, BlueprintReadOnly, Category = "Powerups")
	float PowerupInterval;//����ʱ����

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerups")
	int32 TotalNumOfTicks;//�ܵ��ô���

	UPROPERTY(BlueprintReadOnly, Category = "Powerups")
	int32 TicksProcessed;//��ǰ�Ѿ����ö��ٴ�

	//������Ϊ
	UFUNCTION()
	void OnTickPowerup();

	FTimerHandle TimerHandle_PowerupTick;

	UPROPERTY(ReplicatedUsing=OnRep_PowerUpActive)
	bool bIsPowerupActive;

	UFUNCTION()
	void OnRep_PowerUpActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool bNewActive);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//����
	void ActivePowerup(AActor* OtherActor);

	//��������Ϊ
	UFUNCTION(BlueprintImplementableEvent,Category="Powerups")
	void OnActived(AActor* OtherActor);

	//ʧЧ�����Ϊ
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	//���ú����Ϊ
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();
};
