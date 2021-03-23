// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeWeaponSoundNotify.h"
#include "Engine.h"
#include "TestDummy.h"

void UMeleeWeaponSoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	{
		if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
		{
			//Grab our player
			ATestDummy* Player = Cast<ATestDummy>(MeshComp->GetOwner());
			//Save our combo
			if (Player) {
				Player->AttackSound();
			}
		}
	}
}