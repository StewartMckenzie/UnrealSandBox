// Fill out your copyright notice in the Description page of Project Settings.

#include "TestDummyAnimInstance.h"

#include "TestDummy.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Engine.h"

UTestDummyAnimInstance::UTestDummyAnimInstance()
{
	bIsInAir = false;
	bIsAnimationBlended = true;
	Speed = 0.f;
	RootMotionMode = ERootMotionMode::RootMotionFromEverything;
}

void UTestDummyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// cache the pawn
	Owner = TryGetPawnOwner();
}

void UTestDummyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	// double check our pointers make sure nothing is empty
	if (!Owner)
	{
		return;
	}

	if (Owner->IsA(ATestDummy::StaticClass()))
	{
		ATestDummy* PlayerCharacter = Cast<ATestDummy>(Owner);
		bIsInAir = PlayerCharacter->GetMovementComponent()->IsFalling();
		bIsAnimationBlended = PlayerCharacter->IsAnimationBlended();
		Speed = PlayerCharacter->GetVelocity().Size();
		bIsCrouching = PlayerCharacter->GetMovementComponent()->IsCrouching();
		bIsArmed = PlayerCharacter->IsArmed();
		bIsRolling = PlayerCharacter->IsRolling();
		bIsMoving = Speed > 0 ? true : false;
		//We need to rotate the character smoothly when we roll into a direction
		//
		if (PlayerCharacter)
		{
			if (ShouldRotate) {
				TimeElapsed = TimeElapsed + GetWorld()->GetDeltaSeconds();
				if (TimeElapsed <= RotateTime) {
					PlayerCharacter->SetActorRotation(FMath::RInterpConstantTo(PlayerCharacter->GetActorRotation(), PlayerCharacter->GetDesiredRotation(), GetWorld()->GetDeltaSeconds(), MaxDegreesPerSecond));
				}
			}
			else {
				StopRotating();
			}
		}
	}
}

void UTestDummyAnimInstance::StartRotatingWithLimit(float MaxPossibleRotation, float MaxDegreesPS)
{
	RotateTime = MaxPossibleRotation / MaxDegreesPS;
	MaxDegreesPerSecond = MaxDegreesPS;
	TimeElapsed = 0.f;
	ShouldRotate = true;
}

void UTestDummyAnimInstance::StopRotating()
{
	ShouldRotate = false;
}