// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	this->RootComponent = this->MeshComp;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		//��д��FVector ASCharacter::GetPawnViewLocation() const
		MyOwner->GetActorEyesViewPoint(OUT EyeLocation,OUT EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector EndLocation = EyeLocation + ShotDirection * 10000;

		FHitResult OutHit;
		FCollisionQueryParams ColParams;
		//����Actor
		ColParams.AddIgnoredActor(MyOwner);
		ColParams.AddIgnoredActor(this);
		//�������Ӽ��
		ColParams.bTraceComplex = true;
		//����ֵΪ�Ƿ��⵽����
		bool bIsHitObject = this->GetWorld()->LineTraceSingleByChannel(OUT OutHit, EyeLocation, EndLocation, ECC_Visibility, ColParams);
		if (bIsHitObject)
		{
			//TODO
			AActor* HitActor=OutHit.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("Hit Something"));
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);
		}
		//������
		DrawDebugLine(this->GetWorld(), EyeLocation, EndLocation,FColor::White,false,1.0f,0,1);
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

