// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"


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

	//开启下蹲功能
	this->GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponClass)
	{
		FActorSpawnParameters MySpawnPara = FActorSpawnParameters();
		MySpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MySpawnPara.Owner = this;

		this->Weapon = this->GetWorld()->SpawnActor<ASWeapon>(this->WeaponClass, FTransform(FRotator::ZeroRotator, FVector::ZeroVector),MySpawnPara);
		if (Weapon)
		{
			this->Weapon->K2_AttachToComponent(this->GetMesh(), TEXT("WeaponSocket"), EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		}
	}
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

