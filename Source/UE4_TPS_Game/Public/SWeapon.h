// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	//枚举转化为字节流
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantizeNormal TraceTo;
};

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TraceEffect;

	//特效插槽
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;

protected:

	UFUNCTION(BlueprintCallable,Category="Weapon")
	void PlayFireEffects(FVector TraceEndPoint);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	//Server在服务器端运行的函数
	//Reliable保证连接到服务器
	//WithValidation检验合法性，通常与Server配套使用
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float TimeBetweenShots;

	float LastFireTime;

	//属性与网络复制有关。复制属性时通知参与者
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();


	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);
public:	

	void StartFire();
	void EndFire();

};
