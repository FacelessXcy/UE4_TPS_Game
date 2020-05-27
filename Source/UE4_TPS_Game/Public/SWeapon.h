// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

UCLASS()
class UE4_TPS_GAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USkeletalMeshComponent* MeshComp;


	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	TSubclassOf<UDamageType> DamageType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,Category="Weapon")
	void Fire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
