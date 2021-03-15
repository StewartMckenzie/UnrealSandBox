// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeWeapon.h"
#include "TestDummy.generated.h"


UCLASS(config = Game)class SANDBOX_API ATestDummy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestDummy();
	//Make the default animation montage editable in our blueprint class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage *MeleeSwordAttackMontage;

protected:
	// Called when the game starts or when spawned
	virtual void
	BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

private:
	// player initiates attack
	void AttackStart();
	//initiated attack ends
	void AttackEnd();
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
	AMeleeWeapon *MeleeWeapon;
};
