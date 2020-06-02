// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include <SHealthComponent.h>
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	this->MeshComp->SetCanEverAffectNavigation(false);

	this->RootComponent = this->MeshComp;
	this->MeshComp->SetSimulatePhysics(true);

	this->HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	this->HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	this->SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));

	this->MovementForce = 1000.0f;
	this->RequiredDistanceToTarget = 100.0f;
	this->bExploded = false;
	this->ExplosionRadius = 200.0f;
	this->ExplosionDamage = 40.0f;
	this->bStartedSelfDestruction = false;
	this->SphereComp->SetSphereRadius(this->ExplosionRadius);
	this->SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->SphereComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole()==ROLE_Authority)
	{
		GetNextPathPoint();
	}
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole()==ROLE_Authority&&!this->bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - this->NextPathPoint).Size();
		if (DistanceToTarget <= this->RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			this->MeshComp->AddForce(ForceDirection * this->MovementForce, NAME_None, true);
		}
	}
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (this->MatInst==nullptr)
	{
		this->MatInst = this->MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, this->MeshComp->GetMaterial(0));
	}
	if (this->MatInst)
	{
		this->MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	if (Health<=0)
	{
		SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("Current Health is %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return this->GetActorLocation();
}


void ASTrackerBot::SelfDestruct()
{
	if (this->bExploded)
	{
		return;
	}
	this->bExploded = true;
	

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->ExplosionEffect, GetActorLocation());
	//播放声音
	UGameplayStatics::PlaySoundAtLocation(this, this->ExplodedSound, GetActorLocation());

	this->MeshComp->SetVisibility(false, true);
	this->MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetLocalRole()==ROLE_Authority)
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		//爆炸半径伤害
		UGameplayStatics::ApplyRadialDamage(this, this->ExplosionDamage, GetActorLocation(), this->ExplosionRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), this->ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

		//Destroy();
		//防止服务器端Destroy后，客户端还没有播放爆炸特效时就Destroy
		SetLifeSpan(2.0f);
		
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.0f, this->GetInstigatorController(), this, nullptr);
}

// Called to bind functionality to input
void ASTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//重写的父类中的方法
void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (this->bStartedSelfDestruction && !this->bExploded)
	{
		return;
	}
	this->bStartedSelfDestruction = true;
	if (OtherActor)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn)
		{
			if (GetLocalRole()==ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(this->TimeHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, 0.5f, true, 0.0f);
			}
			//播放声音
			UGameplayStatics::SpawnSoundAttached(this->SelfDestructSound, GetRootComponent());
		}

	}
}

