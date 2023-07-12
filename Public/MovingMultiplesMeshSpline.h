#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AudioComponent.h"
#include "../RunningLatePhilipsCharacter.h"
#include "MovingMultiplesMeshSpline.generated.h"

UENUM(BlueprintType)
enum class ESplineMeshType : uint8 {
	DEFAULT		UMETA(DisplayName = "Default Mesh"),
	START		UMETA(DisplayName = "Starting Mesh"),
	END			UMETA(DisplayName = "Ending Mesh"),
	RAIL		UMETA(DisplayName = "Rail Mesh"),
	BOGIE		UMETA(DisplayName = "Bogie"),
};

USTRUCT(BlueprintType)
struct FSplineMeshDetails : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	FSplineMeshDetails() : ForwardAxis(ESplineMeshAxis::Type::X)
	{
	}
};

UCLASS()
class RUNNINGLATEPHILIPS_API AMovingMultiplesMeshSpline  : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingMultiplesMeshSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
		void ProcessMovementTimeline(float Value);

	UFUNCTION()
		void OnEndMovementTimeline();

	UFUNCTION()
		void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
		void TriggerBeginOverlapCarMakePlayerSmall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerBeginOverlapLastCar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerBeginOverlapOutBoundsTrain(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerEndOverlapOutBoundsTrain(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Construction")
		TMap<ESplineMeshType, FSplineMeshDetails> SplineMeshMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Construction")
		UCurveFloat* MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction")
		bool bAutoStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction", meta = (EditCondition = "!bRestartOnEndTimeline"))
		bool bReverseOnEndTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction", meta = (EditCondition = "!bReverseOnEndTimeline"))
		bool bRestartOnEndTimeline;

	/*The number of meshes/components that will get spawned*/
	UPROPERTY(EditAnywhere, Category = "Construction")
		int32 NumToSpawn;
	
	UPROPERTY(EditAnywhere, Category = "Construction")
		float DistanceBetweenObjs = -10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MainMesh;

	UPROPERTY(EditAnywhere)
		AActor* CameraTrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		FVector CameraOffset = FVector(-220, 0, 350);

	UPROPERTY(EditAnywhere)
		APlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
		ARunningLatePhilipsCharacter* Player;

	/*An array containing references of the spawned components
	This will be used in order to delete old components in case we decide to
	tinker with the NumToSpawn parameter*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction", meta = (AllowPrivateAccess = "true"))
		TArray<UStaticMeshComponent*> TrainMeshArray;

	UPROPERTY(EditAnywhere)
		TArray<UBoxComponent*> TrainBoxTriggerArray;

	UPROPERTY(EditAnywhere)
		TArray<UBoxComponent*> OuTrainBoxTriggerArray;

	UPROPERTY(EditAnywhere)
		TArray<UStaticMeshComponent*> RailMeshArray;

	UPROPERTY(EditAnywhere, Category = "Construction")
		float Speed = 350;

	UPROPERTY(EditAnywhere)
		float DistanceInSpline = 0;

	UPROPERTY(EditAnywhere)
		bool bOutTrain = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
		TMap<FString, UStaticMesh*> CustomTrainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
		TMap<FString, FString> CustomTrainEventOverlap;


	UPROPERTY(EditAnywhere)
		TArray<FString> DoOncePlayerSmall;

private:
	FTimeline MovementTimeline;

};
