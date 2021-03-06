// Fill out your copyright notice in the Description page of Project Settings.

#include "TestDummy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "SphereTrace.h"
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

	SphereTrace = CreateDefaultSubobject<USphereTrace>(TEXT("SphereTrace"));
	SphereTrace->SetupAttachment(RootComponent);

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

	// Find out attack montages by their reference from the editor
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryAMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryBMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryB_Montage.Attack_PrimaryB_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackPrimaryCMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/Attack_PrimaryC_Montage.Attack_PrimaryC_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/RollMontage.RollMontage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> StepBackMontageObject(TEXT("AnimMontage'/Game/Blueprints/TestDummy/Animations/StepBackMontage.StepBackMontage'"));

	//if they are found in the editor assign them to the variables we created
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
	if (StepBackMontageObject.Succeeded())
	{
		StepBackMontage = StepBackMontageObject.Object;
	}
}

// Called when the game starts or when spawned
void ATestDummy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
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
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
}

// Called every frame
void ATestDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (bIsTargetLocked) {
	//}
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
	PlayerInputComponent->BindAction(TEXT("LockOnTarget"), EInputEvent::IE_Pressed, this, &ATestDummy::LockTarget);
}

float ATestDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);
	return DamageToApply;
}

void ATestDummy::AttackInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "IsAttacking: " + FString(IsAttacking ? "true" : "false"));
	//is we are already attack save our attack
	if (!bIsRolling) {
		if (IsAttacking) {
			SaveAttack = true;
		}
		//if we aren't attacking set attacking to true and play the next animation
		else {
			IsAttacking = true;
			PlayComboAnimation();
		}
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Save attack: " + FString(SaveAttack ? "true" : "false"));
	//if we have an attack saved clear it and play our animation
	if (SaveAttack) {
		SaveAttack = false;
		PlayComboAnimation();
	}
}

void ATestDummy::ResetAttackCombo()
{//reset our combo by setting the counter to 0 and clearing both ISAttaking and SaveAttack
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Reseting: ");
	AttackCount = 0;
	IsAttacking = false;
	SaveAttack = false;
	//Call attack end to close colliders if the animation has been interrupted and reset manually
	AttackEnd();
}

void ATestDummy::AttackStart()
{
	//Activate collider
	//Need to refactor this, causes UE to crash when opening the anim montage
	if (WeaponCollisionBox != nullptr) {
		WeaponCollisionBox->SetCollisionProfileName("Weapon");
		/*	WeaponCollisionBox->SetNotifyRigidBodyCollision(true);*/
		WeaponCollisionBox->SetGenerateOverlapEvents(true);
	}
}

void ATestDummy::AttackEnd()
{
	//Deactivate collider
	if (WeaponCollisionBox != nullptr) {
		WeaponCollisionBox->SetCollisionProfileName("NoCollision");
		//WeaponCollisionBox->SetNotifyRigidBodyCollision(false);
		WeaponCollisionBox->SetGenerateOverlapEvents(false);
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

bool ATestDummy::IsDead() const
{
	return Health <= 0;
}

bool ATestDummy::IsArmed()
{
	return bIsArmed;
}

bool ATestDummy::IsTargetLocked()
{
	return bIsTargetLocked;
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

bool ATestDummy::IsSteppingBack()
{
	return bIsSteppingBack;
}

void ATestDummy::RollStart()

{
	//Cannot roll in the air or if we are stepping backwards
	if (!GetCharacterMovement()->IsFalling()) {
		if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(StepBackMontage)) {
			//if we have input roll
			if (HasMovementInput()) {
				bIsRolling = true;
				//roll in direction
				SetActorRotation(GetCharacterMovement()->GetLastInputVector().Rotation());
				PlayHighPriorityMontage(RollMontage);
			}
			//else step back
			else {
				bIsSteppingBack = true;
				TryPlayMontage(StepBackMontage, 2);
			}
		}
	}
}

bool ATestDummy::HasMovementInput()
{
	return !GetCharacterMovement()->GetLastInputVector().Equals(FVector(0));
}

FRotator ATestDummy::GetDesiredRotation()
{
	if (HasMovementInput()) {
		return GetCharacterMovement()->GetLastInputVector().Rotation();
	}
	else {
		return GetActorRotation();
	}
}

void ATestDummy::PlayHighPriorityMontage(UAnimMontage* AnimMontage, int PlayRate)
{
	//only check if the animation we want to play is already playing
	//we can interrupt everything ,but our selves
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(AnimMontage)) {
		StopAnimMontage();
		HighPriorityMontage = AnimMontage;

		PlayAnimMontage(HighPriorityMontage, PlayRate, NAME_None);
	}
	//Specifically for our attacks we need to reset the values associated with them if we intrupt them
	//if we don't it breaks the system
	//make some more "elegant"
	TriggerAttackReset(HighPriorityMontage->SequenceLength * 0.8f);
}

void ATestDummy::TryPlayMontage(UAnimMontage* AnimMontage, int PlayRate /*= 1*/)
{
	//we can't intrupt anything so we check before playing
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
		PlayAnimMontage(AnimMontage, PlayRate, NAME_None);
	}
}

void ATestDummy::RollEnd()
{
	if (bIsRolling) {
		bIsRolling = false;
	}
	else {
		bIsSteppingBack = false;
	}
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
	//set bIsArmed to update animations and reset the timer until we get back to idle
	bIsArmed = Value;

	if (!bIsArmed) {
		CountDownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void ATestDummy::LockTarget()
{
	if (!bIsTargetLocked) {
		AActor* HitActor = SphereTrace->BeginTrace();
		if (HitActor) {
			bIsTargetLocked = true;
			//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Hit: %s"), *HitActor->GetName()));

			//targetLockign functionality
		}
	}
	else {
		bIsTargetLocked = false;
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
	//if the countdown has ended we need to reset it and bIsArmed
	if (--CountDownToIdle <= 0) {
		bIsArmed = false;
		CountDownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void ATestDummy::TriggerAttackReset(float delay)
{//reset our attack combo when intrupted after a set delay
	GetWorld()->GetTimerManager().SetTimer(ReTriggerableDelayHandle, this, &ATestDummy::ResetAttackCombo, 1.f, false, delay);
}

void ATestDummy::MoveForward(float AxisValue)
{
	MoveForwardValue = AxisValue;
	if ((Controller != NULL) && (AxisValue != 0.0f)) {
		//if (bIsTargetLocked && !bIsCrouched) {
		//	if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
		//		//point the character in in the direction of input
		//		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//		// do not rotate the character in the direction of movement
		//		//GetCharacterMovement()->bOrientRotationToMovement = false;
		//	}
		//	AddMovementInput(GetActorForwardVector(), MoveForwardValue);
		//}
		//else {
		//	if (GetCharacterMovement()->bUseControllerDesiredRotation) {
		//		//point the character in in the direction of input
		//		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//		// do not rotate the character in the direction of movement
		//		//GetCharacterMovement()->bOrientRotationToMovement = true;
		//	}
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
	MoveRightValue = AxisValue;
	if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		//if (bIsTargetLocked && !bIsCrouched) {
		//	if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
		//		//point the character in in the direction of input
		//		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//		// do not rotate the character in the direction of movement
		//		//GetCharacterMovement()->bOrientRotationToMovement = false;
		//	}
		//	AddMovementInput(GetActorForwardVector(), MoveRightValue);
		//}
		//else {
		//	if (GetCharacterMovement()->bUseControllerDesiredRotation) {
		//		//point the character in in the direction of input
		//		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//		// do not rotate the character in the direction of movement
		//		//GetCharacterMovement()->bOrientRotationToMovement = true;
		//	}
		//	// find out which way is right
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