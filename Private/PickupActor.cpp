// Fill out your copyright notice in the Description page of Project Settings.

#include "../RunningLatePhilipsCharacter.h"
#include "PickupActor.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	SetRootComponent(SceneRootComponent);

	SaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Satic Mesh"));
	SaticMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	//BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::TriggerBeginOverlap);

	SoundEffectComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundEffectComponent"));
	SoundEffectComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::TriggerBeginOverlap);
	
	if (SoundEffectQueue && SoundEffectComponent)
	{
		SoundEffectComponent->SetSound(SoundEffectQueue);
	}
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	
	SaticMesh->SetWorldRotation(FRotator(0.0f, RotationRatio, 0.0f));
	
	RotationRatio += 0.5;

	Super::Tick(DeltaTime);
}

void APickupActor::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID() ) {
		Destroy();
		ARunningLatePhilipsCharacter* Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
		
		if (SoundEffectQueue)
		{
			SoundEffectComponent->Play(0.f);
		}

		if (ItemType == "life") {
			Player->add1Life();
		}

		if (ItemType == "donut") {
			Player->add1Donut();
		}

		if (ItemType == "cola") {
			Player->add1Cola();
		}

		if (ItemType == "sub") {
			Player->add1Sub();
		}

		if (ItemType == "coffe") {
			Player->add1Coffe();
		}

		if (ItemType == "NormalSizeOnTrain") {
			Player->MakePlayerNormalSize();
			Player->RestartCameraSet();
			Player->SetCameraTrainAgain();
		}
		
	}
}

