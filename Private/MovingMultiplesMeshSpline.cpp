// Fill out your copyright notice in the Description page of Project Settings.

#include "AIPatrolPoint.h"
#include "PickupActor.h"
#include "MovingSingleMeshSpline.h"
#include "DisappearingMesh.h"
#include "Engine/StaticMeshActor.h" 
#include "Kismet/GameplayStatics.h"
#include "../RunningLatePhilipsCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MovingMultiplesMeshSpline.h"

// Sets default values
AMovingMultiplesMeshSpline::AMovingMultiplesMeshSpline()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	SetRootComponent(SceneRootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(GetRootComponent());

	CameraTrain = CreateDefaultSubobject<AActor>(TEXT("CameraMain"));
	CameraTrain->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform, TEXT("CameraMain"));

	bOutTrain = true;
}

void AMovingMultiplesMeshSpline::OnConstruction(const FTransform& Transform)
{

	//Empty the array and delete all it's components
	if (TrainMeshArray.Num() > 0) {
		for (auto It = TrainMeshArray.CreateIterator(); It; It++)
		{
			//(*It)->DestroyComponent();
		}

		TrainMeshArray.Empty();
	}

	if (TrainBoxTriggerArray.Num()) {
		for (auto It = TrainBoxTriggerArray.CreateIterator(); It; It++)
		{
			//(*It)->DestroyComponent();
		}
		TrainBoxTriggerArray.Empty();
	}

	if (OuTrainBoxTriggerArray.Num()) {
		for (auto It = OuTrainBoxTriggerArray.CreateIterator(); It; It++)
		{
			//(*It)->DestroyComponent();
		}
		OuTrainBoxTriggerArray.Empty();
	}

	if (SplineComponent && SplineMeshMap.Num() > 0)
	{

		/** Creat Trains **/

		// Lookup all pertinent values
		FSplineMeshDetails* BogieMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::BOGIE))
		{
			BogieMeshDetails = SplineMeshMap.Find(ESplineMeshType::BOGIE);
		}

		FSplineMeshDetails* StartMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::START))
		{
			StartMeshDetails = SplineMeshMap.Find(ESplineMeshType::START);
		}
		
		FSplineMeshDetails* EndMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::END))
		{
			EndMeshDetails = SplineMeshMap.Find(ESplineMeshType::END);
		}

		FSplineMeshDetails* DefaultMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::DEFAULT))
		{
			DefaultMeshDetails = SplineMeshMap.Find(ESplineMeshType::DEFAULT);
		}
		else
		{
			// exit if we don't have a default mesh to work with
			return;
		}

		//Register all the components
		RegisterAllComponents();

		//The base name for all our components
		FName InitialName = FName("Train");

		float chainLenght = 0;

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			//Create a new Component

			//Change the name for the next possible item
			FString Str = "Train" + FString::FromInt(i + 1);

			//Convert the FString to FName
			InitialName = (*Str);

			//The first parameter is the "parent" of the our new component
			UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this, InitialName);

			//Add a reference to our array
			TrainMeshArray.Add(NewComp);


			//If the component is valid, set it's static mesh, relative location and attach it to our parent
			UStaticMesh* CTM = NULL;

			if (NewComp && StartMeshDetails->Mesh && EndMeshDetails->Mesh && DefaultMeshDetails->Mesh)
			{
				GLog->Log("Registering Mesh Spline Component");

				//Register the new component 
				NewComp->RegisterComponent();

				// Check first if there is a custom mesh set for the train
				if (CustomTrainMesh.Contains(InitialName.ToString()))
				{
					CTM = *CustomTrainMesh.Find(InitialName.ToString());
					NewComp->SetStaticMesh(CTM);
				}
				
				if(CTM == NULL)
				{

					// Set the static mesh of our component
					if (i == 0)
					{
						NewComp->SetStaticMesh(StartMeshDetails->Mesh);
					}
					else if (i == NumToSpawn - 1)
					{
						NewComp->SetStaticMesh(EndMeshDetails->Mesh);
					}
					else
					{
						NewComp->SetStaticMesh(DefaultMeshDetails->Mesh);
					}

				}
				
				// Set a location for the new component along the spline
				FVector RCBounds = NewComp->GetStaticMesh()->GetBounds().GetBox().GetSize();

				float SplineLength = SplineComponent->GetSplineLength();
				float StartSplineLenght = chainLenght;

				FVector CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(StartSplineLenght, ESplineCoordinateSpace::World);
				FVector EndMeshSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(StartSplineLenght + RCBounds.X, ESplineCoordinateSpace::World);

				FRotator CurrentSplineRotation = UKismetMathLibrary::FindLookAtRotation(CurrentSplineLocation, EndMeshSplineLocation);
				CurrentSplineRotation.Pitch = 0.f;
				CurrentSplineLocation.Z += 30;

				NewComp->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);

				// Create Boggies
				UStaticMeshComponent* NewBogieStart = NewObject<UStaticMeshComponent>(this, *FString::Printf(TEXT("Bogie Start %i"), i + 1));
				UStaticMeshComponent* NewBogieEnd = NewObject<UStaticMeshComponent>(this, *FString::Printf(TEXT("Bogie End %i"), i + 1));

				NewBogieStart->RegisterComponent();
				NewBogieEnd->RegisterComponent();

				NewBogieStart->SetStaticMesh(BogieMeshDetails->Mesh);
				NewBogieEnd->SetStaticMesh(BogieMeshDetails->Mesh);

				FVector NewBogieStartLocation = NewBogieStart->GetRelativeLocation();
				NewBogieStartLocation.Z += -70;
				NewBogieStartLocation.X += 200;
				NewBogieStart->SetRelativeLocation(NewBogieStartLocation);

				FVector NewBogieEndLocation = NewBogieEnd->GetRelativeLocation();
				NewBogieEndLocation.Z += -80;
				NewBogieEndLocation.X += RCBounds.X - 200;
				NewBogieEnd->SetRelativeLocation(NewBogieEndLocation);

				NewBogieStart->AttachToComponent(NewComp, FAttachmentTransformRules::KeepRelativeTransform, *FString::Printf(TEXT("Bogie Start %i"), i + 1));
				NewBogieEnd->AttachToComponent(NewComp, FAttachmentTransformRules::KeepRelativeTransform, *FString::Printf(TEXT("Bogie End %i"), i + 1));

				// Box Component to trigger collision
				UBoxComponent* newCollider = NewObject<UBoxComponent>(this, *FString::Printf(TEXT("Trigger OnTrain %i"), i + 1));
				TrainBoxTriggerArray.Add(newCollider);

				FVector ColliderLocation = newCollider->GetComponentToWorld().GetLocation();
				ColliderLocation.Z += 20;
				ColliderLocation.X += RCBounds.X / 2;
				newCollider->SetWorldLocation(ColliderLocation);
				newCollider->SetBoxExtent(FVector(750, 190.0f, 10.0f), false);


				// Check first if there is a custom mesh set for the train
				FString CustomEventOverlap;
				if (CustomTrainEventOverlap.Contains(InitialName.ToString()))
				{
					CustomEventOverlap = *CustomTrainEventOverlap.Find(InitialName.ToString());
					
				}

				if (i == 0 || CustomEventOverlap == "StartTrain") {

					newCollider->OnComponentBeginOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerBeginOverlap);
					newCollider->OnComponentEndOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerEndOverlap);

				}
				else if (CustomEventOverlap == "MakePlayerSmall") {

					newCollider->OnComponentBeginOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerBeginOverlapCarMakePlayerSmall);
					//newCollider->OnComponentEndOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerEndOverlap);

					GLog->Log(TEXT("On"));

				}
				else if (i == NumToSpawn - 1) {

					newCollider->OnComponentBeginOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerBeginOverlapLastCar);
					//newCollider->OnComponentEndOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerEndOverlap);

				}

				newCollider->SetupAttachment(NewComp);

				chainLenght += RCBounds.X + DistanceBetweenObjs;//DistanceBetweenObjs*(i+1)

				//Attach the component to the root component
				NewComp->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepWorldTransform, InitialName); //FAttachmentTransformRules::KeepRelativeTransform
				NewComp->SetCollisionProfileName(TEXT("ObjectToClip"));
				

				// Attach AAIPatrolPoint to the Train Mesh Component
				TArray<AActor*> PatrolPointActorsToAttach;
				UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AAIPatrolPoint::StaticClass(), InitialName, PatrolPointActorsToAttach);

				for (AActor* PatrolPoint : PatrolPointActorsToAttach)
				{
					PatrolPoint->AttachToComponent(NewComp, FAttachmentTransformRules::KeepWorldTransform);
				}

				TArray<AActor*> PickupActorToAttach;
				UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APickupActor::StaticClass(), InitialName, PickupActorToAttach);

				for (AActor* PickupActor : PickupActorToAttach)
				{
					PickupActor->AttachToComponent(NewComp, FAttachmentTransformRules::KeepWorldTransform);
				}

				TArray<AActor*> DisappearingMeshToAttach;
				UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ADisappearingMesh::StaticClass(), InitialName, DisappearingMeshToAttach);

				for (AActor* DisappearingMesh : DisappearingMeshToAttach)
				{
					DisappearingMesh->AttachToComponent(NewComp, FAttachmentTransformRules::KeepWorldTransform);
				}

				TArray<AActor*> MovingSingleMeshSplineToAttach;
				UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AMovingSingleMeshSpline::StaticClass(), InitialName, MovingSingleMeshSplineToAttach);

				for (AActor* MovingSingleMeshSpline : MovingSingleMeshSplineToAttach)
				{
					MovingSingleMeshSpline->AttachToComponent(NewComp, FAttachmentTransformRules::KeepWorldTransform);
				}

				// Attach Static Mesh to the Train Mesh Component
				TArray<AActor*> StaticMeshsToAttach;
				//UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AStaticMeshActor::StaticClass(), InitialName, StaticMeshsToAttach);

				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), StaticMeshsToAttach);

				GLog->Log(FString::Printf(TEXT("num %i"), StaticMeshsToAttach.Num()));

				for (AActor* StaticMesh : StaticMeshsToAttach)
				{

					if (StaticMesh->Tags.Contains(InitialName))
					{
						StaticMesh->AttachToComponent(NewComp, FAttachmentTransformRules::KeepWorldTransform);
						
					}
				}


			}
		}
	

		/** Create Rails**/

		FSplineMeshDetails* RailMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::RAIL))
		{
			RailMeshDetails = SplineMeshMap.Find(ESplineMeshType::RAIL);
		}
		else
		{
			// exit if we don't have a default mesh to work with
			return;
		}

		if (RailMeshDetails->Mesh) {

			const int32 SplinePoints = SplineComponent->GetNumberOfSplinePoints();

			for (int SplineCount = 0; SplineCount < (SplinePoints ); SplineCount++)
			{
				USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

				UStaticMesh* StaticMesh = DefaultMeshDetails->Mesh;
				UMaterialInterface* Material = nullptr;
				ESplineMeshAxis::Type ForwardAxis = DefaultMeshDetails->ForwardAxis;

				// start mesh
				if (RailMeshDetails )
				{
					StaticMesh = RailMeshDetails->Mesh;
					ForwardAxis = RailMeshDetails->ForwardAxis;

				}

				// update mesh details
				SplineMesh->SetStaticMesh(StaticMesh);
				SplineMesh->SetForwardAxis(ForwardAxis, true);
				SplineMesh->SetMaterial(0, Material);
				 
				// initialize the object
				SplineMesh->RegisterComponentWithWorld(GetWorld());

				SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				SplineMesh->SetMobility(EComponentMobility::Movable);

				FVector SplineLocation = SplineMesh->GetRelativeLocation();
				SplineLocation.Z -= 70;
				SplineMesh->SetRelativeLocation(SplineLocation);

				SplineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

				// define the positions of the points and tangents
				const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Type::Local);
				const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Type::Local);
				const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Type::Local);
				const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Type::Local);
				SplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

				
				// create box colider
				UBoxComponent* newCollider = NewObject<UBoxComponent>(this, *FString::Printf(TEXT("Trigger Out Train %i"), SplineCount));
				OuTrainBoxTriggerArray.Add(newCollider);
				
				FVector ColliderLocation = StartPoint;
				ColliderLocation.Z += 40;

				FVector ColliderLength = EndPoint - StartPoint;

				newCollider->SetWorldLocation(ColliderLocation);
				newCollider->SetBoxExtent(FVector(ColliderLength.Size() + 100, 1000.0f, 40.0f), false);

				newCollider->OnComponentBeginOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerBeginOverlapOutBoundsTrain);
				newCollider->OnComponentEndOverlap.AddDynamic(this, &AMovingMultiplesMeshSpline::TriggerEndOverlapOutBoundsTrain);

				FRotator CurrentSplineRotation = UKismetMathLibrary::FindLookAtRotation(EndPoint, StartPoint);
				CurrentSplineRotation.Pitch = 0.f;

				newCollider->SetWorldRotation(CurrentSplineRotation);
				newCollider->SetupAttachment(SplineMesh);

				// query physics
				SplineMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			}
		}
	}
}

// Called when the game starts or when spawned
void AMovingMultiplesMeshSpline::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);
	

	FOnTimelineEvent OnTimelineFinishedFunction;
	OnTimelineFinishedFunction.BindUFunction(this, TEXT("OnEndMovementTimeline"));
	MovementTimeline.SetTimelineFinishedFunc(OnTimelineFinishedFunction);

	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);


	if (bAutoStart)
	{
		MovementTimeline.PlayFromStart();
	}

}

// Called every frame
void AMovingMultiplesMeshSpline::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);

	}

	if (PlayerController && CameraTrain) {

		FVector NewCameraPos = Player->GetActorLocation();
		//NewCameraPos.Z = CameraTrain->GetActorLocation().Z;
		if(bOutTrain)
		{ 
			if(CameraOffset.X < -200)
				CameraOffset.X += 2;

			if (CameraOffset.Z >= 200)
				CameraOffset.Z -= 2;
		}

		NewCameraPos = NewCameraPos + CameraOffset;
		
		CameraTrain->SetActorLocation(NewCameraPos);
	}

}


void AMovingMultiplesMeshSpline::ProcessMovementTimeline(float Value)
{

	const float SplineLength = SplineComponent->GetSplineLength();
	float chainLenght = DistanceInSpline; // Old:  Value * SplineLength 

	//GLog->Log(*FString::Printf(TEXT("Box Component %i"), TrainMeshArray.Num()));
	
	int i = 1;
	for (UStaticMeshComponent* SplineMesh : TrainMeshArray)
	{
		
		float StartSplineLenght = chainLenght;

		if (chainLenght >= SplineLength)
		{
			if (SplineComponent->IsClosedLoop()) {
				StartSplineLenght = chainLenght - SplineLength;
			}
			else 
			{

				MovementTimeline.Stop();
				break;
			}

		}

		//GLog->Log(*FString::Printf(TEXT("chainLenght= %i"), chainLenght));

		FVector RCBounds = SplineMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
		
		FVector CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(StartSplineLenght, ESplineCoordinateSpace::World);
		FVector EndMeshSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(StartSplineLenght + RCBounds.X, ESplineCoordinateSpace::World);
		
		FRotator CurrentSplineRotation = UKismetMathLibrary::FindLookAtRotation(CurrentSplineLocation, EndMeshSplineLocation);  //  SplineComponent->GetRotationAtDistanceAlongSpline(StartSplineLenght, ESplineCoordinateSpace::World); 
		//CurrentSplineRotation.Pitch = 0.f;
		CurrentSplineLocation.Z += 60;
		SplineMesh->SetWorldLocationAndRotation(CurrentSplineLocation, CurrentSplineRotation);

		chainLenght += RCBounds.X + DistanceBetweenObjs ; // DistanceBetweenObjs *i
		i++;

	}

	DistanceInSpline += GetWorld()->DeltaTimeSeconds * Speed;

}

void AMovingMultiplesMeshSpline::OnEndMovementTimeline()
{
	if (bReverseOnEndTimeline)
	{
		MovementTimeline.Reverse();
	}
	else if (bRestartOnEndTimeline)
	{
		MovementTimeline.PlayFromStart();
	}

}

void AMovingMultiplesMeshSpline::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	// Check first if the actor that overlapped is Player and if the Timeline is already been play
	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID() && !MovementTimeline.IsPlaying()) {
		
		MovementTimeline.PlayFromStart();

		// Lock Player Camera
		Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
		Player->SetCameraTrain(true, CameraTrain, CameraTrain->GetActorRotation());

		PlayerController = UGameplayStatics::GetPlayerController(OtherActor, 0);
		PlayerController->SetViewTargetWithBlend(CameraTrain, 0.1f);

		bOutTrain = false;

	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("On First Train"));
	
}

void AMovingMultiplesMeshSpline::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!MovementTimeline.IsReversing())
	{
		//MovementTimeline.Reverse();
	}
}

void AMovingMultiplesMeshSpline::TriggerBeginOverlapLastCar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID()) {
		
		Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
		Player->LastCar();

		bOutTrain = true;
		
	}
}

void AMovingMultiplesMeshSpline::TriggerBeginOverlapOutBoundsTrain(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Out!"));
		if (!bOutTrain) {

			Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
			Player->FallingOutTrain();

			bOutTrain = true;

		}
	}
}

void AMovingMultiplesMeshSpline::TriggerEndOverlapOutBoundsTrain(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMovingMultiplesMeshSpline::TriggerBeginOverlapCarMakePlayerSmall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!DoOncePlayerSmall.Contains(OverlappedComponent->GetName()) && OtherActor->GetUniqueID() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetUniqueID()) {
		
		Player = Cast<ARunningLatePhilipsCharacter>(OtherActor);
		Player->MakePlayerSmall();

		DoOncePlayerSmall.Add(OverlappedComponent->GetName());

		GLog->Log(FString::Printf(TEXT("On Train to Make Small")));
	}
}