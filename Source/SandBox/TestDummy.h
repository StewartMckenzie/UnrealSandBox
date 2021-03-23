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
	//Make the default animation montage editable in our blueprint class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryAMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryBMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AttackPrimaryCMontage;
	// Returns IsAnimationBlended for branching animations in blue prints
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsArmed();

	UFUNCTION()
		void CrouchStart();
	UFUNCTION()
		void CrouchEnd();

	UFUNCTION()
		void ArmPlayer();

	UFUNCTION()
		void TriggerCountdownToIdle();
	//how long it take you to un arm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
		int MaxCountdownToIdle;

	//Trigger attack animations based on user input
	void AttackInput();
	//Plays the next animation in out AttackCombo
	void PlayComboAnimation();

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
	UPROPERTY(VisibleAnywhere)
		bool IsAttacking = false;
	UPROPERTY(VisibleAnywhere)
		bool SaveAttack = false;
	UPROPERTY(VisibleAnywhere)
		int AttackCount = 0;
	UPROPERTY(VisibleAnywhere)
		int AttackSpeed = 1.7;
	UBoxComponent* WeaponCollisionBox;
	bool bIsAnimationBlended;

	bool bIsArmed;
	FTimerHandle ArmedToIdleTimerHandle;
	int CountDownToIdle;
};
