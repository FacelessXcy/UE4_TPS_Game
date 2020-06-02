// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	this->DefaultHealth = 100.0f;
	//复制到客户端
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (this->GetOwnerRole()==ROLE_Authority)
	{
		AActor* MyOwner = this->GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
	this->CurrentHealth = this->DefaultHealth;
	this->HealthPercent = CurrentHealth / this->DefaultHealth;
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	/*if (GetOwner()->Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Health Current Health is %s of %s in Server"), *FString::SanitizeFloat(this->CurrentHealth), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Health Current Health is %s of %s in Client"), *FString::SanitizeFloat(this->CurrentHealth), *GetOwner()->GetName());
	}*/
	float Damage = CurrentHealth - OldHealth;
	this->HealthPercent = CurrentHealth / this->DefaultHealth;
	this->OnHealthChanged.Broadcast(this, this->CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage<=0)
	{
		return;
	}
	this->CurrentHealth = FMath::Clamp(this->CurrentHealth - Damage, 0.0f, this->DefaultHealth);
	this->HealthPercent = CurrentHealth / this->DefaultHealth;
	/*if (GetOwner()->Role==ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Health is %s of %s in Server"), *FString::SanitizeFloat(this->CurrentHealth), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Health is %s of %s in Client"), *FString::SanitizeFloat(this->CurrentHealth), *GetOwner()->GetName());
	}*/
	
	this->OnHealthChanged.Broadcast(this, this->CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount<=0.0f||CurrentHealth<=0.0f)
	{
		return;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, 100.0f);
	this->HealthPercent = CurrentHealth / this->DefaultHealth;
	/*if (GetOwner()->Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Health Changed :%s(+%s) in Server"), *FString::SanitizeFloat(this->CurrentHealth), *FString::SanitizeFloat(HealAmount));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Health Changed :%s(+%s) in Client"), *FString::SanitizeFloat(this->CurrentHealth), *FString::SanitizeFloat(HealAmount));
	}*/
	this->OnHealthChanged.Broadcast(this, this->CurrentHealth, -HealAmount, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetCurrentHealth() const
{
	return this->CurrentHealth;
}

//指定复制Actor的内容和方式
void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//CurrentHealth复制到USHealthComponent上 
	DOREPLIFETIME(USHealthComponent, CurrentHealth);

}