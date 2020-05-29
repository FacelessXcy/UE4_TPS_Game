// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "Components/SHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	this->SpringArmComp->SetupAttachment(this->GetRootComponent());
	this->SpringArmComp->TargetArmLength = 300.0f;
	this->SpringArmComp->bUsePawnControlRotation = true;

	this->CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	this->CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);

	this->HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));


	//开启下蹲功能
	this->GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;


	this->DefaultFOV = this->CameraComp->FieldOfView;
	this->ZoomedFov = 65.0f;
	this->bWantToZoom = false;
	this->ZoomInterpSpeed = 20.0f;
	this->bDied = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	if (WeaponClass)
	{
		FActorSpawnParameters MySpawnPara = FActorSpawnParameters();
		MySpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MySpawnPara.Owner = this;

		//在服务器端运行
		if (this->GetLocalRole()==ROLE_Authority)
		{
			this->CurrentWeapon = this->GetWorld()->SpawnActor<ASWeapon>(this->WeaponClass, FTransform(FRotator::ZeroRotator, FVector::ZeroVector), MySpawnPara);
			if (CurrentWeapon)
			{
				/*this->CurrentWeapon->K2_AttachToComponent(this->GetMesh(), TEXT("WeaponSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);*/

				//this->CurrentWeapon->SetOwner(this);

				this->CurrentWeapon->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
			}
		}
	}
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = this->bWantToZoom ? this->ZoomedFov : this->DefaultFOV;
	float CurrentFOV = FMath::FInterpTo(this->CameraComp->FieldOfView, TargetFOV, DeltaTime, this->ZoomInterpSpeed);
	this->CameraComp->FieldOfView = CurrentFOV;
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Axis
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	//Action
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed,this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ASCharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Pressed, this, &ASCharacter::StartZoom);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ASCharacter::EndFire);
}	

FVector ASCharacter::GetPawnViewLocation() const
{
	if (this->CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASCharacter::Jump()
{
	Super::Jump();
}

void ASCharacter::StopJumping()
{
	Super::StopJumping();
}

void ASCharacter::BeginCrouch()
{
	this->Crouch();
}

void ASCharacter::EndCrouch()
{
	this->UnCrouch();
}

void ASCharacter::MoveForward(float Value)
{
	this->AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	this->AddMovementInput(GetActorRightVector(), Value);
}

void ASCharacter::StartZoom()
{
	this->bWantToZoom = true;
}

void ASCharacter::EndZoom()
{
	this->bWantToZoom = false;
}

void ASCharacter::StartFire()
{
	if (this->CurrentWeapon)
	{
		this->CurrentWeapon->StartFire();
	}
}

void ASCharacter::EndFire()
{
	if (this->CurrentWeapon)
	{
		this->CurrentWeapon->EndFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!this->bDied&&Health<=0.0f)
	{
		this->bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		//解开控制器对Pawn的控制
		DetachFromControllerPendingDestroy();

		//设置Pawn存活时间，到期后，销毁
		SetLifeSpan(10.0f);
	}
}

//指定复制Actor的内容和方式
void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//CurrentWeapon复制到ASCharacter上
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}
