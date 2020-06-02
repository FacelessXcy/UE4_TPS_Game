// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

UCLASS()
class UE4_TPS_GAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

	UPROPERTY(VisibleAnywhere,Category="Components")
	class USphereComponent* SphereComp;

	//Ìù»¨×é¼þ
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly,Category="Pickups")
	float CooldownTime;

	UPROPERTY(EditInstanceOnly,Category="Powerups")
	TSubclassOf<class ASPowerupActor> PowerUpClass;

	ASPowerupActor* PowerupInstance;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
