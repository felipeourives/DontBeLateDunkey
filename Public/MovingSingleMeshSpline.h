// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MovingSingleMeshSpline.generated.h"

UCLASS()
class RUNNINGLATEPHILIPS_API AMovingSingleMeshSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingSingleMeshSpline();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void ProcessMovementTimeline(float Value);

	UFUNCTION()
		void OnEndMovementTimeline();

	UFUNCTION()
		void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Construction")
		UCurveFloat* MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bAutoActivate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction", meta = (EditCondition = "!bRestartOnEndTimeline"))
		bool bReverseOnEndTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction", meta = (EditCondition = "!bReverseOnEndTimeline"))
		bool bRestartOnEndTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bNoRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		float WaitTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SaticMesh;

	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, Category = "Construction")
		float Speed = 400;

	UPROPERTY(EditAnywhere)
		float DistanceInSpline = 0;

	bool isReversing = false;

private:
	FTimeline MovementTimeline;

};
