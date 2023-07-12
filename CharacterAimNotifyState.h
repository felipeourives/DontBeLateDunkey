// Fill out your copyright notice in the Description page of Project Settings.

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RunningLatePhilipsCharacter.h"
#include "CharacterAimNotifyState.generated.h"


UCLASS()
class RUNNINGLATEPHILIPS_API UCharacterAimNotifyState : public UAnimNotify
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterAimNotifyState();

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};

