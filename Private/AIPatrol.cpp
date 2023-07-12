// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "AIPatrol.h"
#include "AIPatrolController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../RunningLatePhilipsCharacter.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAIPatrol::AAIPatrol()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	//SetRootComponent(SceneRootComponent);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	NavigationComp = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavComp"));

	GetCharacterMovement()->MaxWalkSpeed = 100;

	//SplineRouteComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Route"));
	//SplineRouteComponent->SetupAttachment(SceneRootComponent);

}

// Called when the game starts or when spawned
void AAIPatrol::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AAIPatrol::OnHit);

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIPatrol::OnPlayerCaught);

	}


}

// Called to bind functionality to input
void AAIPatrol::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIPatrol::SetWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 100;
}

void AAIPatrol::OnPlayerCaught(APawn* Pawn)
{
	/* Get a Reference to the player controller*/
	SetCanSeePlayer(true, Pawn);
	RunRetriggerableTimer();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Seen!"));
}


void AAIPatrol::SetCanSeePlayer(bool SeePlayer, APawn* Player)
{

	AAIPatrolController* AIController = Cast<AAIPatrolController>(GetController());
	ARunningLatePhilipsCharacter* PlayerController = Cast<ARunningLatePhilipsCharacter>(Player);
	
	if (AIController)
	{	
		// It ignores player when: the hit montage is been play, it heas no life, it is crouching
		if (SeePlayer && !PlayerController->IsPlayingHitMontage() && PlayerController->nLife > 0 && !PlayerController->bIsCrouching)
		{
			AIController->SetPlayerCaught(SeePlayer, Player);

			GetCharacterMovement()->MaxWalkSpeed = 300;

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("You have been caught! %i"), !PlayerController->bIsFalledDown));
		}
		else
		{
			AIController->SetPlayerCaught(SeePlayer, Player);

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Not seen!"));
		}
	}
}

void AAIPatrol::RunRetriggerableTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(RetriggerableTimerHandle);

	FunctionDelegate.BindUFunction(this, FName("SetCanSeePlayer"), false, GetController()->GetPawn());

	GetWorld()->GetTimerManager().SetTimer(RetriggerableTimerHandle, FunctionDelegate, PawnSensingComp->SensingInterval * 2.0f, false);
}

void AAIPatrol::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ARunningLatePhilipsCharacter* Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
	if (Player) {

		Player->Attacked(Hit.ImpactNormal);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Made contact with player"));
	}
}
