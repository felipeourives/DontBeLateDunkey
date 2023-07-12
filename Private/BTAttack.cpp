// Fill out your copyright notice in the Description page of Project Settings.

#include "AIPatrolController.h"
#include "../RunningLatePhilipsCharacter.h"
#include "BTAttack.h"

EBTNodeResult::Type UBTAttack::ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attack!"));
	//AAIPatrolController* AICon = Cast<AAIPatrolController>(OwerComp.GetAIOwner());

	return EBTNodeResult::Succeeded;
}