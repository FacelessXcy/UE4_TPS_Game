// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include <SPowerupActor.h>
#include <SCharacter.h>
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	this->SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	this->RootComponent = this->SphereComp;
	this->SphereComp->SetSphereRadius(75.0f);
	this->SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	this->DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	this->DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	this->DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	this->DecalComp->SetupAttachment(GetRootComponent());
	
	this->CooldownTime = 5.0f;
}

void ASPickupActor::Respawn()
{
	if (this->PowerUpClass==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s"), *GetName());
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	this->PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(this->PowerUpClass, GetTransform(), SpawnParams);

}


// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}

// Called every frame
void ASPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{
			if (this->PowerupInstance&&GetLocalRole()==ROLE_Authority)
			{
				//判断生成的物体是不是加速道具
				if (this->PowerupInstance->GetFName().ToString().Contains("Speed"))
				{
					PlayerPawn->bGetEnergy=true;
					PlayerPawn->PowerupInterval = PowerupInstance->PowerupInterval;
					PlayerPawn->TempEnergy= PowerupInstance->PowerupInterval;
				}
				this->PowerupInstance->ActivePowerup(OtherActor);
				PowerupInstance = nullptr;
				GetWorldTimerManager().SetTimer(this->TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, this->CooldownTime);
			}

		}
	}
}
