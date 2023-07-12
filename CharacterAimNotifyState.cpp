// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAimNotifyState.h"
#include "RunningLatePhilipsCharacter.h"
#include "Engine.h"

UCharacterAimNotifyState::UCharacterAimNotifyState()
{
}

// Notify to play after the attacked montage
void UCharacterAimNotifyState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Notify")));
    
    if (MeshComp != NULL && MeshComp->GetOwner() != NULL)
    {
        ARunningLatePhilipsCharacter* Player = Cast<ARunningLatePhilipsCharacter>(MeshComp->GetOwner());
       
        if (Player != NULL) {

            if (Player->bIsClimbingLedge)
            {
                Player->ClimbUpLedgeEnd();
            }

            if (Player->bIsFalledDown)
            {
                Player->bIsFalledDown = false;
            }

            
        }
        
    }
}