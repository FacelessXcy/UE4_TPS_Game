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
	float PowerupInterval;//调用时间间隔

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerups")
	int32 TotalNumOfTicks;//总调用次数

	UPROPERTY(BlueprintReadOnly, Category = "Powerups")
	int32 TicksProcessed;//当前已经调用多少次

	//调用行为
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

	//激活
	void ActivePowerup(AActor* OtherActor);

	//激活后的行为
	UFUNCTION(BlueprintImplementableEvent,Category="Powerups")
	void OnActived(AActor* OtherActor);

	//失效后的行为
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	//调用后的行为
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();
};
