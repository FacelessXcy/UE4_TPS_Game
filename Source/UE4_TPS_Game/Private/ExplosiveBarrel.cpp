// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	this->RootComponent = this->MeshComp;

	//开启物理模拟
	this->MeshComp->SetSimulatePhysics(true);
	this->MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	this->RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	this->RadialForceComp->SetupAttachment(this->GetRootComponent());
	this->RadialForceComp->Radius = 250.0f;
	this->RadialForceComp->bImpulseVelChange = true;
	this->RadialForceComp->bAutoActivate = false;
	this->RadialForceComp->bIgnoreOwningActor = true;

	this->HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	

	this->bExploded = false;
	this->ExplosionImpulse = 400.0f;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	this->HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!this->bExploded && Health <= 0)
	{
		this->bExploded = true;

		FVector BoostIntensity = FVector::UpVector * this->ExplosionImpulse;
		this->MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->ExplosionEffect, this->GetActorLocation());

		//this->MeshComp->SetMaterial(0, this->ExplosiveMaterial);
		OnRep_Exploded();

		this->RadialForceComp->FireImpulse();

	}

}

void AExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->ExplosionEffect, this->GetActorLocation());

	this->MeshComp->SetMaterial(0, this->ExplosiveMaterial);
}

//指定复制Actor的内容和方式
void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//CurrentWeapon复制到AExplosiveBarrel上
	DOREPLIFETIME(AExplosiveBarrel, bExploded);
}
