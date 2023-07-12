// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSelectPatrolPoint.h"
#include "AIPatrolPoint.h"
#include "AIPatrol.h"
#include "AIPatrolController.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h" 
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTSelectPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory)
{
	AAIPatrolController* AICon = Cast<AAIPatrolController>(OwerComp.GetAIOwner());
	AAIPatrol* AICharacter = Cast<AAIPatrol>(OwerComp.GetAIOwner()->GetPawn());

	if (AICon && AICharacter)
	{
		/* Get BB Component*/
		UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComponent();
	
		AICharacter->SetWalk();

		/*USplineComponent* SplineRouteComponent = AICharacter->GetSplineRouteComponent();
		const int32 SplinePoints = SplineRouteComponent->GetNumberOfSplinePoints();

		if (SplinePoints > 1)
		{

			FVector NextPatrolPoint;

			if (AICon->CurrentPatrolPoint != SplinePoints -1)
			{
				NextPatrolPoint = SplineRouteComponent->GetLocationAtSplinePoint(++AICon->CurrentPatrolPoint, ESplineCoordinateSpace::Type::World);
			}
			else // If there not any more point to go to
			{
				NextPatrolPoint = SplineRouteComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Type::World);
				AICon->CurrentPatrolPoint = 0;
			}

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("GO TO %i > %f, %f, %f"), SplinePoints, NextPatrolPoint.X, NextPatrolPoint.Y, NextPatrolPoint.Z));
			GLog->Log(FString::Printf(TEXT("GO TO %i > %f, %f, %f"), AICon->CurrentPatrolPoint, NextPatrolPoint.X, NextPatrolPoint.Y, NextPatrolPoint.Z));

			BlackboardComp->SetValueAsVector(AICon->BBLocationToGoKey, NextPatrolPoint);

			DrawDebugPoint(GetWorld(), NextPatrolPoint, 10, FColor(255, 0, 0), true);
			
			//AICon->CurrentPatrolPoint += 1;

			return EBTNodeResult::Succeeded;
		}*/

		/*OLD */
		
		AAIPatrolPoint* CurrentPoint = Cast<AAIPatrolPoint>(BlackboardComp->GetValueAsObject(AICon->BBLocationToGoKey));

		TArray<AActor*> AvailablePatrolPoints = AICon->GetPatrolPoints();
		
		if (AvailablePatrolPoints.Num() > 0)
		{ 
			AAIPatrolPoint* NextPatrolPoint = nullptr;

			if (AICon->CurrentPatrolPoint != AvailablePatrolPoints.Num() - 1)
			{
				NextPatrolPoint = Cast<AAIPatrolPoint>(AvailablePatrolPoints[++AICon->CurrentPatrolPoint]);
			}
			else // If there not any more point to go to
			{
				NextPatrolPoint = Cast<AAIPatrolPoint>(AvailablePatrolPoints[0]);
				AICon->CurrentPatrolPoint = 0;
			}

			//NextPatrolPoint->DrawDebugComponents(FColor::Green);
			//GLog->Log(FString::Printf(TEXT("GO TO %i > %f, %f, %f"), AICon->CurrentPatrolPoint, NextPatrolPoint->GetActorLocation().X, NextPatrolPoint->GetActorLocation().Y, NextPatrolPoint->GetActorLocation().Z));

			BlackboardComp->SetValueAsObject(AICon->BBLocationToGoKey, NextPatrolPoint);

			return EBTNodeResult::Succeeded;
		}
		else 
		{

			FNavLocation ResultLocation;
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
			navSys->GetRandomPointInNavigableRadius(AICon->GetPawn()->GetActorLocation(), 300, ResultLocation);

			BlackboardComp->SetValueAsVector(AICon->BBLocationToGoKey, ResultLocation.Location);

			//GLog->Log(TEXT("Random Point"));

			return EBTNodeResult::Succeeded;

		}

	}

	return EBTNodeResult::Failed;
}
