// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RotateCharacterNotify.generated.h"

/**
 *
 */
UCLASS()
class SANDBOX_API URotateCharacterNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
private:
	//max amount of degrees we can rotate in a second
	UPROPERTY(EditAnywhere)
		float MaxDegreesPerSecond = 720;
	//max amount we can rotate
	UPROPERTY(EditAnywhere)
		float MaxPossibleRotation = 180;
};
