// Fill out your copyright notice in the Description page of Project Settings.

#include "ResetAttackNotify.h"
#include "Engine.h"
#include "TestDummy.h"

void UResetAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	{
		if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
		{
			//Grab our player
			ATestDummy* Player = Cast<ATestDummy>(MeshComp->GetOwner());
			//reset our combo
			if (Player) {
				Player->ResetAttackCombo();
			}
		}
	}
}