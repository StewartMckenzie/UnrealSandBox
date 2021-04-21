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

	//Find the Cues we made in editor
	static ConstructorHelpers::FObjectFinder<USoundCue> SwordSlashSoundCueObject(TEXT("SoundCue'/Game/Blueprints/Weapons/Sounds/SwordSwingCue.SwordSwingCue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> SwordHitSoundCueObject(TEXT("SoundCue'/Game/Blueprints/Weapons/Sounds/SwordHitCue.SwordHitCue'"));
	//if found  save them as a variable
	//Also create create an audio component to actual play them
	if (SwordSlashSoundCueObject.Succeeded()) {
		SwordSwingSoundCue = SwordSlashSoundCueObject.Object;
		SwordSwingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SwordSwingAudioComponent"));
		SwordSwingAudioComponent->SetupAttachment((RootComponent));
	}
	if (SwordHitSoundCueObject.Succeeded()) {
		SwordHitSoundCue = SwordHitSoundCueObject.Object;
		SwordHitAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SwordHitAudioComponent"));
		SwordHitAudioComponent->SetupAttachment((RootComponent));
	}
}

// Called when the game starts or when spawned
void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	//Attach box collider to our socket
	BladeCollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, TEXT("BladeSocket"));
	//When the weapon hits something call OnAttackHit on MeleeWeapon
	BladeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnAttackHit);
	//turn off auto play and set our cue as the sound for the component
	//Done in begin play because there are issues with doing it in the constructor. Not to sure of the actual cause what they are gotta research
	if (SwordSwingAudioComponent && !SwordSwingAudioComponent->IsPlaying()) {
		SwordSwingAudioComponent->bAutoActivate = false;
		SwordSwingAudioComponent->SetSound(SwordSwingSoundCue);
	}
	if (SwordHitAudioComponent && !SwordHitAudioComponent->IsPlaying()) {
		SwordHitAudioComponent->bAutoActivate = false;
		SwordHitAudioComponent->SetSound(SwordHitSoundCue);
	}
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeleeWeapon::OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, OtherActor->GetName());
	FVector SlashDirection;
	AController* OwnerController = GetOwnerController();
	SwordHitAudioComponent->Play();
	if (OwnerController != nullptr) {
		SwordHitAudioComponent->Play();

		/*	AActor* HitActor = SweepResult.GetActor();*/

			//FPointDamageEvent DamageEvent(Damage, Hit, SlashDirection, nullptr);

			//HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
	}
}

AController* AMeleeWeapon::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return nullptr;
	return OwnerPawn->GetController();
}