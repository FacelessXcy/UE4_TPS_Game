// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../UE4_TPS_Game.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

//自定义命令行命令
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("Shoot.DebugWeapons"),//命令名
	DebugWeaponDrawing,//参数值
	TEXT("Draw Debug Lines For Weapons"),//命令行描述
	ECVF_Cheat//在打包后不会显示
);
// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	this->RootComponent = this->MeshComp;

	this->MuzzleSocketName = "MuzzleSocket";
	this->BaseDamage = 20.0f;
	this->TimeBetweenShots = 0.1f;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ME(TEXT("ParticleSystem'/Game/WeaponEffects/Muzzle/P_Muzzle_Large.P_Muzzle_Large'"));
	if (ME.Succeeded())
	{
		this->MuzzleEffect = ME.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FIE(TEXT("ParticleSystem'/Game/WeaponEffects/BloodImpact/P_blood_splash_02.P_blood_splash_02'"));
	if (FIE.Succeeded())
	{
		this->FleshImpactEffect = FIE.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DIE(TEXT("ParticleSystem'/Game/WeaponEffects/GenericImpact/P_RifleImpact.P_RifleImpact'"));
	if (DIE.Succeeded())
	{
		this->DefaultImpactEffect = DIE.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> TE(TEXT("ParticleSystem'/Game/WeaponEffects/BasicTracer/P_SmokeTrail.P_SmokeTrail'"));
	if (TE.Succeeded())
	{
		this->TraceEffect = TE.Object;
	}
}	


void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		//重写了FVector ASCharacter::GetPawnViewLocation() const
		MyOwner->GetActorEyesViewPoint(OUT EyeLocation,OUT EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector EndLocation = EyeLocation + ShotDirection * 10000;

		FVector TracerEndPoint=EndLocation;

		FHitResult OutHit;


		FCollisionQueryParams ColParams;
		//忽略Actor
		ColParams.AddIgnoredActor(MyOwner);
		ColParams.AddIgnoredActor(this);
		//开启复杂检测
		ColParams.bTraceComplex = true;
		//是否返回物理材质
		ColParams.bReturnPhysicalMaterial = true;
		//返回值为是否检测到物体
		bool bIsHitObject = this->GetWorld()->LineTraceSingleByChannel(OUT OutHit, EyeLocation, EndLocation, ECC_Visibility, ColParams);
		if (bIsHitObject)
		{
			TracerEndPoint = OutHit.ImpactPoint;
			//TODO
			AActor* HitActor=OutHit.GetActor();

			//UE_LOG(LogTemp, Warning, TEXT("Hit Something"));

			//获取返回的材质的类型
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());
			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACETYPE_FleshDefault:
				UGameplayStatics::ApplyPointDamage(HitActor,this->BaseDamage, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);
				SelectedEffect = this->FleshImpactEffect;
				break;
			case SURFACETYPE_FleshVulnerable:
				UGameplayStatics::ApplyPointDamage(HitActor, this->BaseDamage*4.0f, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);
				SelectedEffect = this->FleshImpactEffect;
				break;
			default:
				SelectedEffect = this->DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
			{
				//生成打击特效
				UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), SelectedEffect, OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation());
			}
		}
		if (DebugWeaponDrawing>0)
		{
			//辅助线
			DrawDebugLine(this->GetWorld(), EyeLocation, EndLocation,FColor::White,false,1.0f,0,1);
		}
		PlayFireEffects(TracerEndPoint);
		//获取最后一次的开火时间
		LastFireTime = this->GetWorld()->TimeSeconds;
	}
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max<float>(LastFireTime + TimeBetweenShots - this->GetWorld()->TimeSeconds,0.0f);
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire,this->TimeBetweenShots,true, FirstDelay);
}

void ASWeapon::EndFire()
{
	this->GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	//枪口特效
	if (this->MuzzleEffect)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Muzzle"));
		UGameplayStatics::SpawnEmitterAttached(this->MuzzleEffect, this->MeshComp, this->MuzzleSocketName);
	}

	//枪线弹道特效
	if (this->TraceEffect)
	{
		FVector MuzzleSocketLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), this->TraceEffect, MuzzleSocketLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TEXT("BeamEnd"), TracerEndPoint);
		}
	}

	//摄像机抖动
	APawn* MyOwner = Cast<APawn>(this->GetOwner());
	if (MyOwner)
	{
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(this->FireCameraShake);
		}
	}
}
