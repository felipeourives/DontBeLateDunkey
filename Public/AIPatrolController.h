// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIPatrol.h"
#include "AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIPatrolController.generated.h"

/**
 * 
 */
UCLASS()
class RUNNINGLATEPHILIPS_API AAIPatrolController : public AAIController
{
	GENERATED_BODY()

public:
	AAIPatrolController();

private:
	TArray<AActor*> PatrolPoints;
	
	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	void OnPossess(APawn* InPawn) override;


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		FName BBLocationToGoKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		FName BBPlayerKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
		FName BBCanSeePlayerKey;

	void SetPlayerCaught(bool CanSeePlayer, APawn* InPawn);

	int32 CurrentPatrolPoint;

	APawn* PlayerPawn;

	FORCEINLINE UBlackboardComponent* GetBackboardComp() const { return BlackboardComp; }
	FORCEINLINE TArray<AActor*> GetPatrolPoints() const { return PatrolPoints; }

};