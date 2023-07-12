// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingSingleMeshSpline.h"

// Sets default values
AMovingSingleMeshSpline::AMovingSingleMeshSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	SetRootComponent(SceneRootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(GetRootComponent());

	SaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Satic Mesh"));
	SaticMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->AttachToComponent(SaticMesh, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AMovingSingleMeshSpline::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AMovingSingleMeshSpline::TriggerBeginOverlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AMovingSingleMeshSpline::TriggerEndOverlap);

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);


	FOnTimelineEvent OnTimelineFinishedFunction;
	OnTimelineFinishedFunction.BindUFunction(this, TEXT("OnEndMovementTimeline"));
	MovementTimeline.SetTimelineFinishedFunc(OnTimelineFinishedFunction);

	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	if (bAutoActivate)
	{
		MovementTimeline.PlayFromStart();
	}
	
}

// Called every frame
void AMovingSingleMeshSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);

	}

}

void AMovingSingleMeshSpline::ProcessMovementTimeline(float Value)
{
	const float SplineLength = SplineComponent->GetSplineLength(); // DistanceInSpline;

	if (DistanceInSpline > SplineLength)
	{
		DistanceInSpline = SplineLength;
	}
	else if (DistanceInSpline < 0) {
		DistanceInSpline = 0;
	}

	if (Speed == 0) {
		DistanceInSpline = Value * SplineLength;
	}

	const FVector CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceInSpline, ESplineCoordinateSpace::World);
	FRotator CurrentSplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceInSpline, ESplineCoordinateSpace::World);

	CurrentSplineRotation.Pitch = 0.f;
	
	if (!bNoRotation)
	{
		SaticMesh->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);
	}
	else {
		SaticMesh->SetWorldLocation(CurrentSplineLocation);
	}
	
	if (Speed > 0) {
		if (!isReversing) {
			DistanceInSpline += GetWorld()->DeltaTimeSeconds * Speed;
		}
		else {
			DistanceInSpline -= GetWorld()->DeltaTimeSeconds * Speed;
		}
	}

}

void AMovingSingleMeshSpline::OnEndMovementTimeline()
{

	if (isReversing == false) {
		isReversing = true;
	}
	else {
		isReversing = false;
	}

	if (bReverseOnEndTimeline)
	{

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
			{

				if (isReversing)
				{
					MovementTimeline.Reverse();
				}
				else {
					MovementTimeline.PlayFromStart();

				}
			});

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.01f, false, WaitTime);

		
	}
	else if (bRestartOnEndTimeline)
	{
		MovementTimeline.PlayFromStart();
	}

}

void AMovingSingleMeshSpline::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check first if the actor that overlapped is Player and if the Timeline is already been play
	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID() && !MovementTimeline.IsPlaying()) {

		MovementTimeline.PlayFromStart();

	}
}

void AMovingSingleMeshSpline::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!MovementTimeline.IsReversing())
	{
		MovementTimeline.Reverse();
	}
}

