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
		// again check pointers
		if (PlayerCharacter)
		{
			bIsInAir = PlayerCharacter->GetMovementComponent()->IsFalling();
			bIsAnimationBlended = PlayerCharacter->IsAnimationBlended();
			Speed = PlayerCharacter->GetVelocity().Size();
			bIsCrouching = PlayerCharacter->GetMovementComponent()->IsCrouching();
			bIsArmed = PlayerCharacter->IsArmed();
			bIsMoving = Speed > 0 ? true : false;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "IsInAir: " + FString(IsInAir ? "true" : "false"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "IsAnimationBlended: " + FString(IsAnimationBlended ? "true" : "false"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Speed: " + FString::SanitizeFloat(Speed));
		}
	}
}