// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeWeapon.h"
#include "TestDummy.generated.h"

UCLASS(config = Game)
class SANDBOX_API ATestDummy : public ACharacter
{
	GENERATED_BODY()

public:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Sets default values for this character's properties
	ATestDummy();
	//Make the default animation montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryAMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryBMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryCMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* StepBackMontage;

	// Returns IsAnimationBlended for branching animations in blue prints
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();

	//Getter for class bools
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsArmed();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsTargetLocked();

	UFUNCTION(BlueprintCallable, Category = Animation)
		float GetMoveRight();

	UFUNCTION(BlueprintCallable, Category = Animation)
		float GetMoveForward();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsSprintning();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsRolling();
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsSteppingBack();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasMovementInput();

	UFUNCTION()
		void RollStart();

	//Returns the direction of the last input vector
	UFUNCTION()
		FRotator GetDesiredRotation();

	UFUNCTION()
		void RollEnd();

	UFUNCTION()
		void CrouchStart();
	//end crouching
	UFUNCTION()
		void CrouchEnd();
	//"Arms" the player changing animations for idle and movement
	UFUNCTION()
		void ArmPlayer(bool Value);

	UFUNCTION()
		void LockTarget();

	//Arms player on a key press
	UFUNCTION()
		void ArmPlayerImmediately();

	UFUNCTION()
		void SprintStart();

	UFUNCTION()
		void SprintEnd();
	//Once the player is arm start a timer until the return to "normal"
	UFUNCTION()
		void TriggerCountdownToIdle();
	//Helper function that resets our attacks when interrupted
	void TriggerAttackReset(float delay);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
		int MaxCountdownToIdle;
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		float LockOnRadius;
	UPROPERTY(VisibleAnywhere)
		float MaxCrouchSpeed;
	UPROPERTY(VisibleAnywhere)
		float MaxWalkSpeed;
	UPROPERTY(VisibleAnywhere)
		float MaxSprintSpeed;

	//Trigger attack
	void AttackInput();
	//Helper function to play attack animation combos
	void PlayComboAnimation();
	//Helper function to play uninterpretable animations
	void PlayHighPriorityMontage(UAnimMontage* AnimMontage, int PlayRate = 1);
	////Helper function to play "low priority animations"
	void TryPlayMontage(UAnimMontage* AnimMontage, int PlayRate = 1);

	//Allows the current attack combo to continue
	void SaveAttackCombo();
	//Resets our combo after some time
	void ResetAttackCombo();

	// player initiates attack
	void AttackStart();
	//initiated attack ends
	void AttackEnd();
	//Play Attack sound
	void AttackSound();
	//Timer handle for delayed actions
	FTimerHandle ReTriggerableDelayHandle;
protected:
	// Called when the game starts or when spawned
	virtual void
		BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//x and y movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	//x and y camera rotations for controller
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	//How fast our camera rotates
	UPROPERTY(EditAnywhere)
		float RotationRate = 10;
	//How high we jump
	UPROPERTY(EditAnywhere)
		float JumpForceZ = 600;
	//The class of our weapon
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AMeleeWeapon> MeleeWeaponClass;
	//A pointer to our actual weapon
	UPROPERTY()
		AMeleeWeapon* MeleeWeapon;

	//class bools
	UPROPERTY(VisibleAnywhere)
		bool IsAttacking = false;
	UPROPERTY(VisibleAnywhere)
		bool SaveAttack = false;
	UPROPERTY(VisibleAnywhere)
		int AttackCount = 0;
	UPROPERTY(VisibleAnywhere)
		int AttackSpeed = 1.7;
	UAnimMontage* HighPriorityMontage;

	bool bIsSprintning;
	float MoveForwardValue;
	float MoveRightValue;
	UBoxComponent* WeaponCollisionBox;
	bool bIsAnimationBlended;
	bool bIsRolling;
	bool bIsSteppingBack;
	bool bIsArmed;
	bool bIsTargetLocked = false;
	FTimerHandle ArmedToIdleTimerHandle;
	int CountDownToIdle;
};
