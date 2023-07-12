// Fill out your copyright notice in the Description page of Project Settings.


#include "DisappearingMesh.h"

// Sets default values
ADisappearingMesh::ADisappearingMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	SetRootComponent(SceneRootComponent);

	SaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Satic Mesh"));
	SaticMesh->AttachToComponent(SceneRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->AttachToComponent(SaticMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ADisappearingMesh::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ADisappearingMesh::TriggerBeginOverlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ADisappearingMesh::TriggerEndOverlap);

}

// Called every frame
void ADisappearingMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAutoStart) {
		StartTimeline();
	}
}

void ADisappearingMesh::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check first if the actor that overlapped is Player and if the Timeline is already been play
	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID() && bDissapearOnTouch) {
		StartTimeline();
		//GLog->Log(TEXT("On"));
	}
}

void ADisappearingMesh::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}	

void ADisappearingMesh::StartTimeline()
{

	//GLog->Log(TEXT("Start Timeline"));

	if (!bIsVisible) {

		SaticMesh->SetVisibility(false);
		SaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		SaticMesh->SetVisibility(true);
		SaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	}

	GetWorldTimerManager().SetTimer(TimlineHandle, this, &ADisappearingMesh::OnEndTimeline, 0.1f, false, DelayToDissapear);

	bAutoStart = false;
}

void ADisappearingMesh::OnEndTimeline()
{

	//GLog->Log(TEXT("End Timeline"));

	if (bIsVisible) {

		SaticMesh->SetVisibility(false);
		SaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsVisible = false;

		//GLog->Log(TEXT("Invisible"));
	}
	else
	{
		SaticMesh->SetVisibility(true);
		SaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bIsVisible = true;

		//GLog->Log(TEXT("Visible"));
	}
	

	if (bReappear) 
	{
		GetWorldTimerManager().SetTimer(TimlineHandle, this, &ADisappearingMesh::OnEndTimeline, 0.1f, false, DelayToReapear);
	}
}
