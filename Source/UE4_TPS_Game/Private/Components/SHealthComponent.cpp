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
	//���Ƶ��ͻ���
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (this->GetOwnerRole()==ROLE_Authority)
	{
		this->CurrentHealth = this->DefaultHealth;
		AActor* MyOwner = this->GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage<=0)
	{
		return;
	}
	this->CurrentHealth = FMath::Clamp(this->CurrentHealth - Damage, 0.0f, this->DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s :"), *FString::SanitizeFloat(this->CurrentHealth));
	this->OnHealthChanged.Broadcast(this, this->CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

//ָ������Actor�����ݺͷ�ʽ
void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//CurrentHealth���Ƶ�USHealthComponent�� 
	//COND_SkipOwner ���������ڷַ�ʱ�����Է�����
	DOREPLIFETIME(USHealthComponent, CurrentHealth);

}