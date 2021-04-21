// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereTrace.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"

class UCameraComponent;

// Sets default values for this component's properties
USphereTrace::USphereTrace()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void USphereTrace::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ATestDummy>(GetOwner());
}

// Called every frame
void USphereTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AActor* USphereTrace::BeginTrace()
{
	if (Player) {
		const FVector	Start = Player->GetActorLocation();
		const FVector	End = Player->FollowCamera->GetForwardVector() * 1000.0f + Start;;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetOwner());
		TArray<FHitResult> HitArray;
		TArray<TEnumAsByte<enum EObjectTypeQuery>> objectTypes;

		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

		//bool UKismetSystemLibrary::SphereTraceMultiForObjects(const UObject * WorldContextObject, const FVector Start, const FVector End, float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > &ObjectTypes, bool bTraceComplex, const TArray<AActor*>&ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>&OutHits, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)

		const bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius, objectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitArray, true, FLinearColor::Gray, FLinearColor::Blue, 60.0f);
		if (Hit) {
			//for (const FHitResult HitResult : HitArray) {
			//	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Hit: %s"), *HitResult.Actor->GetName()));
			//}
			return HitArray[0].GetActor();
		}
	}
	return nullptr;
}