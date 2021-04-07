// Fill out your copyright notice in the Description page of Project Settings.

#include "RotateCharacterNotify.h"
#include "Engine.h"
#include "TestDummy.h"
#include "TestDummyAnimInstance.h"
void URotateCharacterNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetAnimInstance() != nullptr)
	{
		UTestDummyAnimInstance* PlayerAnimInstance = Cast<UTestDummyAnimInstance>(MeshComp->GetAnimInstance());

		if (PlayerAnimInstance) {
			PlayerAnimInstance->StartRotatingWithLimit(MaxDegreesPerSecond, MaxPossibleRotation);
		}
	}
}