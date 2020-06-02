// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class UE4_TPS_GAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class ASWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Replicated,Category="Weapon")
	ASWeapon* CurrentWeapon;

	//ÊÇ·ñÒª¿ª¾µ
	bool bWantToZoom;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Player")
	float ZoomedFov;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player",meta=(ClampMin=0.1,ClampMax=100))
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Health")
	class USHealthComponent* HealthComp;

	UPROPERTY(Replicated,BlueprintReadOnly,Category="Health")
	bool bDied;

	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Energy")
	float EnergyPercent;

	UPROPERTY(Replicated)
	float PowerupInterval;

	void CalculateEnergyPercent(float DeltaTime);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Jump() override;
	virtual void StopJumping() override;

	void BeginCrouch();
	void EndCrouch();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartZoom();
	void EndZoom();

	void StartFire();
	void EndFire();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
	UPROPERTY(Replicated)
	bool bGetEnergy;

	UPROPERTY(Replicated)
	float TempEnergy;
};
