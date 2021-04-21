// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TestDummyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class SANDBOX_API UTestDummyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsAnimationBlended;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsRolling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsDead;

public:
	UTestDummyAnimInstance();
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	void StartRotatingWithLimit(float MaxPossibleRotation, float MaxDegreesPerSecond);
	void StopRotating();
	bool ShouldRotate = false;
	float TimeElapsed = 0;
	float RotateTime;

	//maximum we can rotate in a second
	float MaxDegreesPerSecond = 720;

private:
	APawn* Owner;
};
