// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PickupActor.generated.h"

UCLASS()
class RUNNINGLATEPHILIPS_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float RotationRatio = 0;

	UPROPERTY(EditAnywhere)
		FName ItemType;

	UPROPERTY(EditAnywhere)
		USceneComponent* SceneRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
		UMeshComponent* SaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
		UBoxComponent* BoxCollider;

	UFUNCTION()
		void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* SoundEffectQueue;

private:
	UAudioComponent* SoundEffectComponent;
};
