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
		//重写了FVector ASCharacter::GetPawnViewLocation() const
		MyOwner->GetActorEyesViewPoint(OUT EyeLocation,OUT EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector EndLocation = EyeLocation + ShotDirection * 10000;

		FHitResult OutHit;
		FCollisionQueryParams ColParams;
		//忽略Actor
		ColParams.AddIgnoredActor(MyOwner);
		ColParams.AddIgnoredActor(this);
		//开启复杂检测
		ColParams.bTraceComplex = true;
		//返回值为是否检测到物体
		bool bIsHitObject = this->GetWorld()->LineTraceSingleByChannel(OUT OutHit, EyeLocation, EndLocation, ECC_Visibility, ColParams);
		if (bIsHitObject)
		{
			//TODO
			AActor* HitActor=OutHit.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("Hit Something"));
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, OutHit, MyOwner->GetInstigatorController(), this, DamageType);
		}
		//辅助线
		DrawDebugLine(this->GetWorld(), EyeLocation, EndLocation,FColor::White,false,1.0f,0,1);
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

