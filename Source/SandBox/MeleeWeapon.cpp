// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeWeapon.h"

// Sets default values
AMeleeWeapon::AMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//create a root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// assign a mesh for our weapon and attach it to the root
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	//Initialize the box collider for our blade
	BladeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BladeCollisionBox"));
	BladeCollisionBox->SetCollisionProfileName("NoCollision");
	BladeCollisionBox->SetHiddenInGame(false);
	BladeCollisionBox->SetNotifyRigidBodyCollision(false);
}

// Called when the game starts or when spawned
void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	//Attach box collider to our socket
	BladeCollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, TEXT("BladeSocket"));
	//When the weapon hits something call OnAttackHit on MeleeWeapon
	BladeCollisionBox->OnComponentHit.AddDynamic(this, &AMeleeWeapon::OnAttackHit);
	//BladeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnAttackOverlapBegin);
	//BladeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMeleeWeapon::OnAttackOverlapEnd);
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeleeWeapon::SetCollider(bool Status)
{
	//If true activate else deactivate colliders
	if (Status) {
		BladeCollisionBox->SetCollisionProfileName("Weapon");
	}
	else {
		BladeCollisionBox->SetCollisionProfileName("NoCollision");
	}
}

void AMeleeWeapon::OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, Hit.GetActor()->GetName());
}

//void AMeleeWeapon::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, OtherActor->GetName());
//}
//
//void AMeleeWeapon::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, OtherActor->GetName());
//}