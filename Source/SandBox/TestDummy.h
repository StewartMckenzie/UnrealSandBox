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
	// Sets default values for this character's properties
	ATestDummy();

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
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	UPROPERTY(EditAnywhere)
	float RotationRate = 10;
	UPROPERTY(EditAnywhere)
	float JumpForceZ = 600;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMeleeWeapon> MeleeWeaponClass;
	UPROPERTY()
	AMeleeWeapon *MeleeWeapon;
};
