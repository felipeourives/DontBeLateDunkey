// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "NavigationInvokerComponent.h"
#include "AIPatrol.generated.h"

UCLASS()
class RUNNINGLATEPHILIPS_API AAIPatrol : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIPatrol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "AI")
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneRootComponent;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineRouteComponent;
		*/

	UPROPERTY(Visibleanywhere, Category = "AI")
		class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		FName TargetPointsTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UNavigationInvokerComponent* NavigationComp;

	UFUNCTION()
		void SetWalk();

private:

	UFUNCTION()
		void OnPlayerCaught(APawn* Pawn);

public:
	UFUNCTION()
		void SetCanSeePlayer(bool SeePlayer, APawn* Player);

	FTimerHandle RetriggerableTimerHandle;

	FTimerDelegate FunctionDelegate;

	void RunRetriggerableTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UBoxComponent* ContactTrigger;


	//FORCEINLINE USplineComponent* GetSplineRouteComponent() const { return SplineRouteComponent; }
	

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
		
};
