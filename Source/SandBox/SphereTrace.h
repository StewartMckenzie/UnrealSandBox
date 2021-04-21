// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TestDummy.h"
#include "SphereTrace.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SANDBOX_API USphereTrace : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USphereTrace();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Trace")
		float TraceRadius = 250.0f;

	AActor* BeginTrace();
	ATestDummy* Player;
	//TArray<FHitResult> HitArray;
	//TArray<FHitResult>
};