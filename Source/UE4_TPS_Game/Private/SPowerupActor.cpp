// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->PowerupInterval = 0.0f;
	this->TotalNumOfTicks = 0.0f;
	this->bIsPowerupActive = false;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::OnTickPowerup()
{
	this->TicksProcessed++;
	this->OnPowerupTicked();
	if (this->TicksProcessed>=this->TotalNumOfTicks)
	{
		this->bIsPowerupActive = false;
		OnPowerupStateChanged(this->bIsPowerupActive);
		GetWorldTimerManager().ClearTimer(this->TimerHandle_PowerupTick);
		this->OnExpired();
	}
}

void ASPowerupActor::OnRep_PowerUpActive()
{
	OnPowerupStateChanged(this->bIsPowerupActive);
}

// Called every frame
void ASPowerupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//ASPickupActor中，在Overlap到角色时调用
void ASPowerupActor::ActivePowerup(AActor* OtherActor)
{
	this->OnActived(OtherActor);
	this->bIsPowerupActive = true;
	OnPowerupStateChanged(this->bIsPowerupActive);

	if (this->PowerupInterval>0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, this->PowerupInterval, true);
	}
	else
	{
		this->OnTickPowerup();
	}
}

//指定复制Actor的内容和方式
void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//CurrentWeapon复制到ASCharacter上
	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
	DOREPLIFETIME(ASPowerupActor, PowerupInterval);
}
