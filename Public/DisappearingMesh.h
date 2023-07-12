// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "DisappearingMesh.generated.h"

UCLASS()
class RUNNINGLATEPHILIPS_API ADisappearingMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisappearingMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		float DelayToDissapear = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		float DelayToReapear = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bDissapearOnTouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bAutoStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bReappear = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SaticMesh;

	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollider;

	FTimerHandle TimlineHandle;

	UFUNCTION()
		void StartTimeline();

	UFUNCTION()
		void OnEndTimeline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		bool bIsVisible = true;
};
