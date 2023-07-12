// Fill out your copyright notice in the Description page of Project Settings.
#include "AIPatrolController.h"
#include "AIController.h"
#include "AIPatrol.h"
#include "AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"


AAIPatrolController::AAIPatrolController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BBPlayerKey = "Enemy";
	BBLocationToGoKey = "PatrolPoint";
	BBCanSeePlayerKey = "CanSeePlayer";

	CurrentPatrolPoint = 0;

}

void AAIPatrolController::SetPlayerCaught(bool CanSeePlayer, APawn* InPawn)
{
	if (BlackboardComp)
	{

		PlayerPawn = InPawn;
		BlackboardComp->SetValueAsBool(BBCanSeePlayerKey, CanSeePlayer);
		BlackboardComp->SetValueAsObject(BBPlayerKey, InPawn);
		//BlackboardComp->SetValueAsVector(PlayerKey, InPawn->GetActorLocation());

	}
}

void AAIPatrolController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAIPatrol* AICharacter = Cast<AAIPatrol>(InPawn);

	if (AICharacter) 
	{
		if (AICharacter->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
		}

		/* Populate patrol point array*/
		if (!AICharacter->TargetPointsTag.IsNone()) 
		{
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AAIPatrolPoint::StaticClass(), AICharacter->TargetPointsTag, PatrolPoints);

		}
		
		BehaviorComp->StartTree(*AICharacter->BehaviorTree);
	}
}
