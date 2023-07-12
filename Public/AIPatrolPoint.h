// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AIPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class RUNNINGLATEPHILIPS_API AAIPatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ParentMesh;
};
