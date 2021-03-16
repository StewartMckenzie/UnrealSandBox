// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackStartNotifyState.h"
#include "Engine.h"
#include "TestDummy.h"

void UAttackStartNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify start"));
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		//Grab our player
		ATestDummy* Player = Cast<ATestDummy>(MeshComp->GetOwner());
		//Initiate attack when animation starts
		if (Player != NULL) {
			Player->AttackStart();
		}
	}
}

void UAttackStartNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify End"));
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
	{
		//Grab our player
		ATestDummy* Player = Cast<ATestDummy>(MeshComp->GetOwner());
		if (Player != NULL) {
			Player->AttackEnd();
		}
	}
}