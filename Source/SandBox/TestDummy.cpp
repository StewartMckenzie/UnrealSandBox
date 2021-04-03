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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Find out attack montages by their refrence from the editor
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryAMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryBMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryB_Montage.Attack_PrimaryB_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryCMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryC_Montage.Attack_PrimaryC_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/RollMontage.RollMontage'"));

	//if they are found in the editor asign them to the variables we created
	if (AttackPrimaryAMontageObject.Succeeded())
	{
		AttackPrimaryAMontage = AttackPrimaryAMontageObject.Object;
	}
	if (AttackPrimaryBMontageObject.Succeeded())
	{
		AttackPrimaryBMontage = AttackPrimaryBMontageObject.Object;
	}
	if (AttackPrimaryCMontageObject.Succeeded())
	{
		AttackPrimaryCMontage = AttackPrimaryCMontageObject.Object;
	}
	if (RollMontageObject.Succeeded())
	{
		RollMontage = RollMontageObject.Object;
	}
}

// Called when the game starts or when spawned
void ATestDummy::BeginPlay()
{
	Super::BeginPlay();
	//spawn the weapon in world
	MeleeWeapon = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass);

	if (MeleeWeapon) {	//attach it in our right hand
		MeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocketR"));
		//claim ownership
		MeleeWeapon->SetOwner(this);
		//Grab your weapons box collision
		WeaponCollisionBox = MeleeWeapon->FindComponentByClass<UBoxComponent>();
	}
	bIsAnimationBlended = true;
	MaxCountdownToIdle = 5;
	MaxCrouchSpeed = 175.f;
	MaxWalkSpeed = 350.0f;
	MaxSprintSpeed = 650.0f;
	//MaxArmedSpeed = 200.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
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
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ATestDummy::CrouchStart);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ATestDummy::CrouchEnd);
	PlayerInputComponent->BindAction(TEXT("ArmPlayer"), EInputEvent::IE_Released, this, &ATestDummy::ArmPlayerImmediately);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ATestDummy::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ATestDummy::SprintEnd);
	PlayerInputComponent->BindAction(TEXT("Roll"), EInputEvent::IE_Pressed, this, &ATestDummy::RollStart);
}

void ATestDummy::AttackInput()
{
	//is we are already attack save our attack
	if (IsAttacking) {
		SaveAttack = true;
	}
	//if we aren't attacking set attacking to true and play the next animation
	else {
		IsAttacking = true;
		PlayComboAnimation();
	}
}

void ATestDummy::PlayComboAnimation()
{
	//A switch to ply out combo attacks in order
	switch (AttackCount) {
	case 0:
		AttackCount = 1;
		PlayAnimMontage(AttackPrimaryAMontage, AttackSpeed, NAME_None);

		break;
	case 1:
		AttackCount = 2;
		PlayAnimMontage(AttackPrimaryBMontage, AttackSpeed, NAME_None);

		break;
	case 2:
		AttackCount = 0;
		PlayAnimMontage(AttackPrimaryCMontage, AttackSpeed, NAME_None);
		break;
	}
	if (!bIsArmed) {
		bIsArmed = true;
	}
}

void ATestDummy::SaveAttackCombo()
{
	//if we have an attack saved clear it and play our animation
	if (SaveAttack) {
		SaveAttack = false;
		PlayComboAnimation();
	}
}

void ATestDummy::ResetAttackCombo()
{//reset our combo by seting the counter to 0 and clearing both ISAttaking and SaveAttack
	AttackCount = 0;
	IsAttacking = false;
	SaveAttack = false;
}

void ATestDummy::AttackStart()
{
	//Activate collider
	//Need to refactor this, causes UE to crash when opening the anim montage
	if (WeaponCollisionBox != nullptr) {
		WeaponCollisionBox->SetCollisionProfileName("Weapon");
		WeaponCollisionBox->SetNotifyRigidBodyCollision(true);
		/*	WeaponCollisionBox->SetGenerateOverlapEvents(true);*/
	}
}

void ATestDummy::AttackEnd()
{
	//Deactivate collider
	if (WeaponCollisionBox != nullptr) {
		WeaponCollisionBox->SetCollisionProfileName("NoCollision");
		WeaponCollisionBox->SetNotifyRigidBodyCollision(false);
		/*	WeaponCollisionBox->SetGenerateOverlapEvents(false);*/
	}
	//Check if our timer is still going
	bool bIsCountdownActive = GetWorld()->GetTimerManager().IsTimerActive(ArmedToIdleTimerHandle);

	if (bIsCountdownActive) {
		//reset timer
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
	//Start timer again

	CountDownToIdle = MaxCountdownToIdle;
	GetWorld()->GetTimerManager().SetTimer(ArmedToIdleTimerHandle, this, &ATestDummy::TriggerCountdownToIdle, 1.f, true);
}

void ATestDummy::AttackSound()
{
	//Plays "Woosh" Sound at the correct time
	MeleeWeapon->SwordSwingAudioComponent->Play();
}

bool ATestDummy::IsAnimationBlended()
{
	return bIsAnimationBlended;
}

bool ATestDummy::IsArmed()
{
	return bIsArmed;
}

float ATestDummy::GetMoveRight()
{
	return MoveRightValue;
}

float ATestDummy::GetMoveForward()
{
	return MoveForwardValue;
}

bool ATestDummy::IsSprintning()
{
	return bIsSprintning;
}

bool ATestDummy::IsRolling()
{
	return bIsRolling;
}

void ATestDummy::RollStart()

{
	if (!GetCharacterMovement()->GetLastInputVector().Equals(FVector(0))) {
		bIsRolling = true;
		PlayRollAnimation();
	}
}

void ATestDummy::PlayRollAnimation()
{
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(RollMontage)) {
		SetActorRotation(GetCharacterMovement()->GetLastInputVector().Rotation());
		PlayAnimMontage(RollMontage, 1, NAME_None);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, bIsRolling ? TEXT("Roll") : TEXT("DOnT ROLL"));
	}
}

void ATestDummy::RollEnd()
{
	bIsRolling = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("Roll END"));
}

void ATestDummy::CrouchStart()
{
	Crouch();
}

void ATestDummy::CrouchEnd()
{
	UnCrouch();
}

void ATestDummy::ArmPlayer(bool Value)
{
	bIsArmed = Value;

	if (!bIsArmed) {
		CountDownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void ATestDummy::ArmPlayerImmediately()
{
	ArmPlayer(!bIsArmed);
}

void ATestDummy::SprintStart()
{
	if (!bIsCrouched)
	{
		bIsSprintning = true;
		GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
	}
}

void ATestDummy::SprintEnd()
{
	if (!bIsCrouched)
	{
		bIsSprintning = false;
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}

void ATestDummy::TriggerCountdownToIdle()
{
	if (--CountDownToIdle <= 0) {
		bIsArmed = false;
		CountDownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void ATestDummy::MoveForward(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f)) {
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