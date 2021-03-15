// Fill out your copyright notice in the Description page of Project Settings.

#include "TestDummy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"

// Sets default values
ATestDummy::ATestDummy()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = JumpForceZ;
	GetCharacterMovement()->AirControl = 0.2f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Load our attack montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeSwordAttackMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/MeleeSwordAttackMontage.MeleeSwordAttackMontage'"));

	if (MeleeSwordAttackMontageObject.Succeeded())
	{
		MeleeSwordAttackMontage = MeleeSwordAttackMontageObject.Object;
	}
}

// Called when the game starts or when spawned
void ATestDummy::BeginPlay()
{
	Super::BeginPlay();
	//spawn the weapon in world
	MeleeWeapon = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass);

	//attach it in our right hand
	MeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocketR"));
	//claim ownership
	MeleeWeapon->SetOwner(this);
}

// Called every frame
void ATestDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATestDummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATestDummy::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATestDummy::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &ATestDummy::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &ATestDummy::LookRightRate);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ATestDummy::AttackInput);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &ATestDummy::AttackEnd);
}

void ATestDummy::AttackInput()
{
	//Generate a random attack number between 1-4
	int MontageSectionIndex = FMath::RandRange(1, 4);

	//Create a string to reference the start an attack animation sections
	FString MontageSection = "Start_" + FString::FromInt(MontageSectionIndex);

	//Play that animations
	PlayAnimMontage(MeleeSwordAttackMontage, 1.f, FName(*MontageSection));
}

void ATestDummy::AttackStart()
{
	//Log to screen
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack Started"));
	//Activate collider
	//Need to refactor this, causes UE to crash when opening the anim montage
	MeleeWeapon->SetCollider(true);
}

void ATestDummy::AttackEnd()
{
	//Log to screen
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack ended"));
	//Dectivate collider
	//Need to refactor this, causes UE to crash when opening the anim montage
	MeleeWeapon->SetCollider(false);
}

void ATestDummy::MoveForward(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void ATestDummy::MoveRight(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, AxisValue);
	}
}

void ATestDummy::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void ATestDummy::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}