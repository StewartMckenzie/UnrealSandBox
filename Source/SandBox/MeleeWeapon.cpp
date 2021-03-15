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
}

// Called when the game starts or when spawned
void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	//Attach box collider to our socket
	BladeCollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("BladeSocket"));
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
