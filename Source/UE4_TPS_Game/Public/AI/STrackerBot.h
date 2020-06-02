// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class UE4_TPS_GAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere,Category="Components")
	class USHealthComponent* HealthComp;

	UMaterialInstanceDynamic* MatInst;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;

	bool bExploded;

	UPROPERTY(VisibleAnywhere,Category="Components")
	class USphereComponent* SphereComp;

	void DamageSelf();

	FTimerHandle TimeHandle_SelfDamage;

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly,Category="Sound")
	class USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* ExplodedSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
