// Copyright Epic Games, Inc. All Rights Reserved.
#define COLLISION_LEDGE ECC_GameTraceChannel1

#include "RunningLatePhilipsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "../UI/InGameHud.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ARunningLatePhilipsCharacter

ARunningLatePhilipsCharacter::ARunningLatePhilipsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(31.f, 94.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	
	RestartCameraSet();

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FollowCamera->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));

	// Face Camera
	OnFaceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OnFaceCamera"));
	//OnFaceCamera->SetupAttachment(CameraBoom);
	OnFaceCamera->SetupAttachment(RootComponent);

	// Speed Setup
	GetCharacterMovement()->MaxWalkSpeed = 100;
	SpeedIncrease = 8.0f;
	MaxSpeed = 600;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	HitSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSoundComponent"));
	HitSoundComponent->SetupAttachment(RootComponent);

	Jump1Component = CreateDefaultSubobject<UAudioComponent>(TEXT("Jump1Component"));
	Jump1Component->SetupAttachment(RootComponent);

	Jump2Component = CreateDefaultSubobject<UAudioComponent>(TEXT("Jump2Component"));
	Jump2Component->SetupAttachment(RootComponent);

	Jump3Component = CreateDefaultSubobject<UAudioComponent>(TEXT("Jump3Component"));
	Jump3Component->SetupAttachment(RootComponent);

	PlayerSmallComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerSmallComponent"));
	PlayerSmallComponent->SetupAttachment(RootComponent);

	PlayerNormalSizeComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerNormalSizeComponent"));
	PlayerNormalSizeComponent->SetupAttachment(RootComponent);

	BackgroundMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusicComponent"));
	BackgroundMusicComponent->SetupAttachment(RootComponent);

	BackgroundMusicPlayerSmallComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusicPlayerSmallComponent"));
	BackgroundMusicPlayerSmallComponent->SetupAttachment(RootComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARunningLatePhilipsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunningLatePhilipsCharacter::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunningLatePhilipsCharacter::PlayerStopJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARunningLatePhilipsCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARunningLatePhilipsCharacter::CrouchEnd);

	PlayerInputComponent->BindAction("ClimbUp", IE_Pressed, this, &ARunningLatePhilipsCharacter::ClimbUpLedgeStart);
	PlayerInputComponent->BindAction("ClimbDown", IE_Pressed, this, &ARunningLatePhilipsCharacter::ClimbDownLedge);
	//PlayerInputComponent->BindAction("ClimbLeft", IE_Pressed, this, &ARunningLatePhilipsCharacter::ClimbLeftLedge);
	//PlayerInputComponent->BindAction("ClimbRight", IE_Pressed, this, &ARunningLatePhilipsCharacter::ClimbRightLedge);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARunningLatePhilipsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARunningLatePhilipsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ARunningLatePhilipsCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARunningLatePhilipsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ARunningLatePhilipsCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARunningLatePhilipsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARunningLatePhilipsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARunningLatePhilipsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARunningLatePhilipsCharacter::OnResetVR);

	// Debug Capsule
	/*GetCapsuleComponent()->SetVisibility(true);
	GetCapsuleComponent()->SetHiddenInGame(false);*/
}

void ARunningLatePhilipsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SeekLedge();

	updateGameTimer();

}


void ARunningLatePhilipsCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetWorldTimerManager().SetTimer(FallCheckHandle, this, &ARunningLatePhilipsCharacter::FallCheckTimer, 0.1f, true, 0.0f);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARunningLatePhilipsCharacter::OnHit);

	if (HitSoundQueue && HitSoundComponent)
	{
		HitSoundComponent->SetSound(HitSoundQueue);
	}

	if (Jump1Queue && Jump1Component)
	{
		Jump1Component->SetSound(Jump1Queue);
	}

	if (Jump2Queue && Jump2Component)
	{
		Jump2Component->SetSound(Jump2Queue);
	}

	if (Jump3Queue && Jump3Component)
	{
		Jump3Component->SetSound(Jump3Queue);
	}

	if (PlayerSmallQueue && PlayerSmallComponent)
	{
		PlayerSmallComponent->SetSound(PlayerSmallQueue);
	}

	if (PlayerNormalSizeQueue && PlayerNormalSizeComponent)
	{
		PlayerNormalSizeComponent->SetSound(PlayerNormalSizeQueue);
	}

	if (BackgroundMusicQueue && BackgroundMusicComponent)
	{
		BackgroundMusicComponent->SetSound(BackgroundMusicQueue);
	}

	if (BackgroundMusicPlayerSmallQueue && BackgroundMusicPlayerSmallComponent)
	{
		BackgroundMusicPlayerSmallComponent->SetSound(BackgroundMusicPlayerSmallQueue);
	}
	
}


void ARunningLatePhilipsCharacter::SetCameraTrain(bool LockCamera, AActor* CT,FRotator CameraDirection)
{
	bLockCamera = LockCamera;
	bOnCameraTrain = true;
	CameraTrain = CT;
	CameraTrainDirection = CameraDirection;

	if (BackgroundMusicQueue)
	{
		BackgroundMusicComponent->Play(0.f);
	}

}

void ARunningLatePhilipsCharacter::SetCameraTrainAgain()
{

	if (CameraTrain)
	{
		bLockCamera = true;
		bOnCameraTrain = true;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		PlayerController->SetViewTargetWithBlend(CameraTrain, 2.0f);

	}
}


void ARunningLatePhilipsCharacter::AddControllerPitchInput(float Val)
{
	
	if (bLockCamera)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		FRotator newRotation = CameraBoom->GetComponentRotation();

		newRotation.Pitch += Val * PC->InputPitchScale;

		CameraBoom->SetWorldRotation(newRotation);
	}
	else {
		Super::AddControllerPitchInput(Val);
	}
}

void ARunningLatePhilipsCharacter::AddControllerYawInput(float Val)
{
	if (bLockCamera)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		FRotator newRotation = CameraBoom->GetComponentRotation();

		newRotation.Yaw += Val * PC->InputYawScale;

		CameraBoom->SetWorldRotation(newRotation);
	}
	else {
		Super::AddControllerYawInput(Val);
	}
}

void ARunningLatePhilipsCharacter::SeekLedge()
{
	ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// WALL AND LEDGE TRACE PARAMS
	const FVector WStart = myCharacter->GetActorLocation();
	FRotator FR = myCharacter->GetActorRotation();
	FVector FV = UKismetMathLibrary::GetForwardVector(FR);
	FVector WL = FV * 70.0f;
	const  FVector WEnd = FVector(FV.X * 150.0f, FV.Y * 150.0f, FV.Z) + WStart;

	const FVector LStart = FVector(WL.X, WL.Y, WL.Z) + FVector(WStart.X, WStart.Y, WStart.Z + 500.0f);
	const  FVector LEnd = FVector(LStart.X, LStart.Y, (LStart.Z - 500.0f));

	// Trace for Wall
	FHitResult WHitOut(ForceInit);
	TArray<AActor*> toignore;
	UKismetSystemLibrary::SphereTraceSingle(this, WStart, WEnd, 20.0f, UEngineTypes::ConvertToTraceType(COLLISION_LEDGE), false, toignore, EDrawDebugTrace::None, WHitOut, true);

	
	// Trace for Ledge
	FHitResult LHitOut(ForceInit);
	UKismetSystemLibrary::SphereTraceSingle(this, LStart, LEnd, 20.0f, UEngineTypes::ConvertToTraceType(COLLISION_LEDGE), true, toignore, EDrawDebugTrace::None, LHitOut, true);

	LedgeLocation = LHitOut.Location;
	FVector SocketLocation = GetMesh()->GetSocketLocation("Spine1Socket");
	float LocalDistanceToLedge = (SocketLocation.Z - LedgeLocation.Z);

	if (LedgeLocation.Z != 0.0f && ((SocketLocation.Z - LedgeLocation.Z) > -50.0f && (SocketLocation.Z - LedgeLocation.Z) < -4.0f) && !bIsClimbingLedgeUp) 
	{
		bCanClimbingLedge = true;

		WallLocation = WHitOut.Location;
		WallNormal = WHitOut.Normal;

		// Fix the Z valur to adjust player Z location to the ledge
		float LedgeLocationZ = LHitOut.GetActor()->ActorToWorld().GetLocation().Z + LHitOut.GetActor()->GetComponentsBoundingBox().GetSize().Z/2;
		LedgeActor = LHitOut.GetActor();
		LedgeActorLocation = LedgeActor->ActorToWorld().GetLocation();
		GrabLedge(LedgeLocationZ);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Ledge: %f ... %f ... %f"), SocketLocation.Z - LedgeLocation.Z, LedgeLocation.Z, SocketLocation.Z));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("WN: %f ... %f"), WallNormal.X, WallNormal.Y));
    
	}
	else if (bIsClimbingLedgeUp)
	{
		ClimbUpLedgeAnim();
	}
	else if (bIsOnLedge)
	{
		OnLedge();
	}
	else {
		bCanClimbingLedge = false;
	}

}

void ARunningLatePhilipsCharacter::GrabLedge(float LedgeLocationZ)
{


	bIsClimbingLedge = true;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	GetCharacterMovement()->GravityScale = 1.0f;
	GetWorldTimerManager().ClearTimer(GravMultiplierHandle);
	bJumping = false;

	FVector WallLedgeHangPosition = WallNormal *10 + WallLocation;
	FRotator WallLedgeHangRotation = UKismetMathLibrary::MakeRotFromX( WallNormal * -1);
	WallLedgeHangRotation.Pitch = 0.0f;

	GetCapsuleComponent()->SetWorldLocationAndRotation(FVector(WallLedgeHangPosition.X, WallLedgeHangPosition.Y, LedgeLocationZ), WallLedgeHangRotation);
	
}

void ARunningLatePhilipsCharacter::ClimbUpLedgeStart()
{
	if (bIsClimbingLedge && !bIsClimbingLedgeUp)
	{
		bIsClimbingLedgeUp = true;

		if (M_ClimbToCrouch) {
			PlayAnimMontage(M_ClimbToCrouch, 1, NAME_None);
		}
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Up")));
	}
}

void ARunningLatePhilipsCharacter::ClimbUpLedgeAnim()
{
	if (bIsClimbingLedgeUp) {

		LedgeLocationZAnimFrame += 1;

		FVector DeltaLedgeActorLocation = LedgeActor->ActorToWorld().GetLocation() - LedgeActorLocation;
		FVector newCapsuleLocation = GetCapsuleComponent()->GetComponentLocation() + DeltaLedgeActorLocation;

		newCapsuleLocation.Z += 1;

		if (LedgeLocationZAnimFrame > 30)
		{
			newCapsuleLocation -= WallNormal * FVector(1, 1, 0) * 2;
		}

		GetCapsuleComponent()->SetWorldLocation(newCapsuleLocation);

		LedgeActorLocation = LedgeActor->ActorToWorld().GetLocation();

		GLog->Log(FString::Printf(TEXT("%i"), LedgeLocationZAnimFrame));
		//GLog->Log(FString::Printf(TEXT("Delta: %f,%f,%f"), WallNormal.X, WallNormal.Y, WallNormal.Z));
	}
}

void ARunningLatePhilipsCharacter::OnLedge()
{
	FVector DeltaLedgeActorLocation = LedgeActor->ActorToWorld().GetLocation() - LedgeActorLocation;
	FVector newCapsuleLocation = GetCapsuleComponent()->GetComponentLocation() + DeltaLedgeActorLocation;

	GetCapsuleComponent()->SetWorldLocation(newCapsuleLocation);

	LedgeActorLocation = LedgeActor->ActorToWorld().GetLocation();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("On Ledge")));
}

void ARunningLatePhilipsCharacter::ClimbUpLedgeEnd()
{
	if (bIsClimbingLedge)
	{
		
		bIsClimbingLedge = false;
		bIsClimbingLedgeUp = false;
		bIsOnLedge = true;
		LedgeLocationZAnimFrame = 0;

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		StopAnimMontage(M_ClimbToCrouch);
		
		GetCharacterMovement()->GravityScale = 1.0f;
		GetWorldTimerManager().ClearTimer(GravMultiplierHandle);

		//FVector CapsuleComponentLocation = GetCapsuleComponent()->GetComponentToWorld().GetLocation() + GetCapsuleComponent()->GetForwardVector()*10.0f + FVector(0.0f,0.0f,0.0f);
		//GetCapsuleComponent()->SetWorldLocationAndRotation(CapsuleComponentLocation, FRotator(0.0f, 0.0f, 0.0f));

		GetCharacterMovement()->GravityScale = 1.0f;
		GetWorldTimerManager().ClearTimer(GravMultiplierHandle);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Climb Up End")));
	}
}

void ARunningLatePhilipsCharacter::ClimbDownLedge()
{
	if (bIsClimbingLedge)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		bIsClimbingLedgeUp = false;

		GetCharacterMovement()->GravityScale = 1.0f;
		GetWorldTimerManager().ClearTimer(GravMultiplierHandle);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Climb Up Down")));

	}
}

void ARunningLatePhilipsCharacter::ClimbLeftLedge()
{
	if (bIsClimbingLedge && M_ClimbToLeft) {
		PlayAnimMontage(M_ClimbToLeft, 1, NAME_None);
	}
}

void ARunningLatePhilipsCharacter::ClimbRightLedge()
{
	if (bIsClimbingLedge && M_ClimbToRight) {
		PlayAnimMontage(M_ClimbToRight, 1, NAME_None);
	}
}

void ARunningLatePhilipsCharacter::Attacked(FVector ForwardDir)
{

	// Run Attack only once
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(M_Hitted))
	{
		//ForwardDir = GetCapsuleComponent()->GetForwardVector();

		const FVector AddForce = ForwardDir * -300 + FVector(0.0f, 0.0f, 1.0F) * 400;

		LaunchCharacter(AddForce, true, true);

		sub1Life();
	}
	
	// Restart when you dont have more life
	if (nLife <= 0) {

		if (M_FallingBackDeath) {

			bIsFalledDown = true;
			PlayAnimMontage(M_FallingBackDeath, 1, NAME_None);
			
			BackgroundMusicComponent->Stop();
			BackgroundMusicPlayerSmallComponent->Stop();

			M_FallingBackDeath->bEnableAutoBlendOut = false;
			AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			InGameHUD->ShowRestarScreen();

			// Restart Game
			bLockCamera = false;
			bPauseGame = true;
		}

	}
	else {

		if (M_Hitted) {
			bIsFalledDown = true;
			PlayAnimMontage(M_Hitted, 1, NAME_None);
		}
	}

	if (HitSoundQueue)
	{
		HitSoundComponent->Play(0.f);
	}

	MaxSpeed = 600;

	GetWorldTimerManager().ClearTimer(CoffeSpeedUpHandler);
}

bool ARunningLatePhilipsCharacter::IsPlayingHitMontage()
{
	return GetMesh()->GetAnimInstance()->Montage_IsPlaying(M_Hitted);;
}

void ARunningLatePhilipsCharacter::LastCar()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.80f);

	FollowCamera->Deactivate();
	OnFaceCamera->Activate();

	BackgroundMusicComponent->Stop();
	BackgroundMusicPlayerSmallComponent->Stop();

	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		bIsFalledDown = true;
		InGameHUD->ShowCompletedScreen();

		// Restart Game
		bLockCamera = false;
		bPauseGame = true;
	}
}

void ARunningLatePhilipsCharacter::FallingOutTrain()
{

	if (M_FallingFlat) {
		bIsFalledDown = true;
		PlayAnimMontage(M_FallingFlat, 1, NAME_None);

		BackgroundMusicComponent->Stop();
		BackgroundMusicPlayerSmallComponent->Stop();

		AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (InGameHUD)
		{
			InGameHUD->ShowRestarScreen();

			// Restart Game
			bLockCamera = false;
			bPauseGame = true;
		}
	}
}

void ARunningLatePhilipsCharacter::OnResetVR()
{
	// If RunningLatePhilips is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in RunningLatePhilips.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARunningLatePhilipsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	ARunningLatePhilipsCharacter::PlayerJump();
}

void ARunningLatePhilipsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	ARunningLatePhilipsCharacter::PlayerStopJump();
}

void ARunningLatePhilipsCharacter::NotifyJumpApex()
{
	GLog->Log("NotifyJumpApex.");

	//GetCharacterMovement()->GravityScale = 5.0f;
	Super::NotifyJumpApex();
}

void ARunningLatePhilipsCharacter::ModifyJumpPower()
{
	switch (JumpCounter) {
		case 1:
			GetCharacterMovement()->JumpZVelocity = 600.0f;
			break;

		case 2:
			GetCharacterMovement()->JumpZVelocity = 1000.0f;
			break;

		default:
			break;
	}

	GLog->Log("ModifyJumpPower.");
}

void ARunningLatePhilipsCharacter::ResetJumpPower()
{
	JumpCounter = 0;
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GLog->Log("Reset Jump.");
}

void ARunningLatePhilipsCharacter::OnJumped_Implementation()
{	
	GLog->Log("OnJumped_Implementation.");

	GetCharacterMovement()->bNotifyApex = true;

	JumpCounter++;
	GetWorldTimerManager().ClearTimer(JumpReset);

	Super::OnJumped_Implementation();
}

void ARunningLatePhilipsCharacter::CrouchStart()
{
	if (!WallSlidingActor) 
	{
		Crouch();
		bIsCrouching = true;
		GLog->Log("Crouch.");
	}

}

void ARunningLatePhilipsCharacter::CrouchEnd()
{
	UnCrouch();
	bIsCrouching = false;
	GLog->Log("UnCrouch.");
}

void ARunningLatePhilipsCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const FVector ForwardDir = HitComponent->GetForwardVector();

	// If is Jumping to another wall
	if (WallSlidingActor != nullptr && WallSlidingActor != OtherActor)
	{
		bWallSliding = false;
	}

	FVector OtherComponentSize = OtherActor->GetComponentsBoundingBox().GetSize();
	float OtherComponentLocationZ = OtherActor->ActorToWorld().GetLocation().Z;
	FVector CapsuleLocation = GetCapsuleComponent()->GetComponentToWorld().GetLocation();
	FVector CapsuleSize = GetCapsuleComponent()->Bounds.GetBox().GetSize();
	float LedgeBorder = 8;

	if (bJumping 
		&& !bWallSliding 
		&& GetCapsuleComponent()->GetRelativeLocation().Z > 8 
		&& Hit.ImpactNormal.Rotation().Pitch == 0.0f 
		&& Hit.ImpactNormal.Rotation().Roll == 0.0f
		&& !bIsClimbingLedge
		&& OtherActor->Tags.Contains("DoWallSlide")
		)
	{

		GetCharacterMovement()->GravityScale = 0.1f;
		
		FVector AddForce = ForwardDir * FVector(0.f, 0.0f, 1.0f) + FVector(0.f, 0.0f, 1.0f);
		LaunchCharacter(AddForce, true, true);
		
		GetCapsuleComponent()->SetWorldRotation(Hit.ImpactNormal.Rotation() + FRotator(180.0f, 180.0f, 180.0f));

		bWallSliding = true;
		bJumping = false;

		MaxSpeed = 600;

		GetWorldTimerManager().ClearTimer(CoffeSpeedUpHandler);
		GetWorldTimerManager().ClearTimer(GravMultiplierHandle);

		WallSlidingActor = OtherActor;

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s %f %f"), *OtherActor->GetName(), Hit.ImpactNormal.Rotation().Roll, Hit.ImpactNormal.Rotation().Pitch));
		
	}
	
}

void ARunningLatePhilipsCharacter::PlayerJump()
{

	Jump();

	// Play Sound
	if(!bIsCrouching && !bIsClimbingLedge && !bIsFalledDown)
	{
		if (JumpCounter == 0) {
			Jump1Component->Play(0.0f);
		}
		else if (JumpCounter == 1) {
			Jump2Component->Play(0.0f);
		}
		else if (JumpCounter == 2) {
			Jump3Component->Play(0.0f);
		}
	}

	// When the player is grapping the wall
	if (bIsClimbingLedge && !bIsFalledDown) {

		bIsClimbingLedge = false;
		
		//GetCapsuleComponent()->SetWorldLocation();
		GetCapsuleComponent()->SetWorldRotation(WallNormal.Rotation() + FRotator(180.0f, 180.0f, 180.0f));

		const FVector ForwardDir = GetCapsuleComponent()->GetForwardVector();
		const FVector AddForce = ForwardDir * 300 + FVector(0.0f, 0.0f, 1.0F) * 800;

		LaunchCharacter(AddForce, true, true);
		
		GetCharacterMovement()->GravityScale = 3.0f;
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Jump out of Grab") ));

	}
	
	// When the player is wall sliding
	if(!bJumping && bWallSliding){
		const FVector ForwardDir = GetCapsuleComponent()->GetForwardVector();
		const FVector AddForce = ForwardDir * 300 + FVector(0.0f, 0.0f, 1.0F) * 800;

		LaunchCharacter(AddForce, true, true);
		GetCharacterMovement()->GravityScale = 3.0f;
	}

	GetWorldTimerManager().SetTimer(GravMultiplierHandle, this, &ARunningLatePhilipsCharacter::GravityMultiplierTimer, 0.1f, true, 0.0f);
	bJumping = true;
}

void ARunningLatePhilipsCharacter::PlayerStopJump()
{
	if (bJumping && !bWallSliding) {
		GetCharacterMovement()->GravityScale = 5.0f;
	}

	GLog->Log("player stop jump.");
}

void ARunningLatePhilipsCharacter::Landed(const FHitResult& Hit)
{
	GetCharacterMovement()->GravityScale = 1.0f;
	GetWorldTimerManager().ClearTimer(GravMultiplierHandle);
	bJumping = false;

	GravMultiplier = 0.1f;

	bIsClimbingLedge = false;
	bIsClimbingLedgeUp = false;

	bWallSliding = false;
	WallSlidingActor = nullptr;

	GLog->Log(*FString::Printf(TEXT("JumpCounter = %i"), JumpCounter));

	if (JumpCounter > 2)
		ResetJumpPower();

	ModifyJumpPower();
	GetWorldTimerManager().SetTimer(JumpReset, this, &ARunningLatePhilipsCharacter::ResetJumpPower, 0.2f, false, 0.3f);

	GLog->Log("Landed.");
	
	Super::Landed(Hit);
}

void ARunningLatePhilipsCharacter::GravityMultiplierTimer()
{

	if (!bWallSliding && !bIsClimbingLedge) {

		GLog->Log(*FString::Printf(TEXT("grav multiplier change. %f %i"), GetCharacterMovement()->GravityScale, bJumping));

		if (GetCharacterMovement()->GravityScale < 5.0f && bJumping == true) {

			GetCharacterMovement()->GravityScale += GravMultiplier;
			GravMultiplier += 0.5f;
			GLog->Log("grav multiplier changed.");
		}

		if (GetCharacterMovement()->GravityScale >= 5.0f && bJumping == true) {
			GravMultiplier = 5.0f;
		}
	}
}

void ARunningLatePhilipsCharacter::FallCheckTimer()
{
	if (GetCharacterMovement()->IsFalling() && !bJumping) {
		GravMultiplier += 5.0f;
	}
}

void ARunningLatePhilipsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARunningLatePhilipsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARunningLatePhilipsCharacter::MoveForward(float Value)
{

	if ((Controller != nullptr) && (Value != 0.0f) && !bWallSliding && !bIsClimbingLedge && !bIsFalledDown && !bIsCrouching)
	{
		if (bIsOnLedge)
		{
			bIsOnLedge = false;
		}

		if (MaxSpeed > GetCharacterMovement()->MaxWalkSpeed) 
		{
			if (Value > 0) 
			{
				GetCharacterMovement()->MaxWalkSpeed += SpeedIncrease * Value;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed -= SpeedIncrease * Value;
			}
		}
		
		// find out which way is forward
		FRotator Rotation = Controller->GetControlRotation();


		// if it is using the train camera
		if (bOnCameraTrain) {
			Rotation = CameraTrainDirection;
		}

		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	else if(Value == 0 && GetCharacterMovement()->Velocity.SizeSquared() < 5) {

		GetCharacterMovement()->MaxWalkSpeed = 100;
	}
}

void ARunningLatePhilipsCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bWallSliding && !bIsClimbingLedge && !bIsFalledDown && !bIsCrouching)
	{
		if (MaxSpeed > GetCharacterMovement()->MaxWalkSpeed)
		{
			if (Value > 0)
			{
				GetCharacterMovement()->MaxWalkSpeed += SpeedIncrease * Value;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed -= SpeedIncrease * Value;
			}
		}

		// find out which way is right
		FRotator Rotation = Controller->GetControlRotation();

		// if it is using the train camera
		if (bOnCameraTrain) {
			Rotation = CameraTrainDirection;
		}

		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	else if (Value == 0 && GetCharacterMovement()->Velocity.SizeSquared() < 5) {

		GetCharacterMovement()->MaxWalkSpeed = 100;
	}
}

void ARunningLatePhilipsCharacter::sub1Life() {

	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nLife -= 1;

		InGameHUD->UpdateLife(nLife);
	}
}

void ARunningLatePhilipsCharacter::add1Life() {
	
	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nLife += 1;

		InGameHUD->UpdateLife(nLife);
	}
}

void ARunningLatePhilipsCharacter::updateGameTimer()
{
	if(!bPauseGame)
	{
		// update hud
		AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (InGameHUD)
		{
			int totalSeconds = GetWorld()->TimeSeconds;
			int32 minutes = totalSeconds / 60;
			int32 seconds = totalSeconds % 60;

			FString time = FString::Printf(TEXT("%02i:%02i"),minutes,seconds % 60);

			InGameHUD->UpdateGameTimer(time);
		}
	}
}

void ARunningLatePhilipsCharacter::add1Donut() {

	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nDonut += 1;

		InGameHUD->UpdateDonut(nDonut);
	}
}

void ARunningLatePhilipsCharacter::add1Cola() {

	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nCola += 1;

		InGameHUD->UpdateCola(nCola);
	}
}

void ARunningLatePhilipsCharacter::add1Sub() {

	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nSub += 1;

		InGameHUD->UpdateSub(nSub);
	}
}

void ARunningLatePhilipsCharacter::add1Coffe() {

	// update hud
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
	{
		nCoffe += 1;

		InGameHUD->UpdateCoffe(nCoffe);

		MaxSpeed += MaxSpeed * CoffeSpeedUpRatio;

		GetWorldTimerManager().SetTimer(CoffeSpeedUpHandler, this, &ARunningLatePhilipsCharacter::CoffeSpeedUpTimer, 0.2f, false, 10.0f);
		GLog->Log(FString::Printf(TEXT("Start coffe")));
	}
}

void ARunningLatePhilipsCharacter::CoffeSpeedUpTimer() {

	MaxSpeed -= MaxSpeed * CoffeSpeedUpRatio;

	if (GetCharacterMovement()->MaxWalkSpeed > MaxSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	}

	GLog->Log(FString::Printf(TEXT("End coffe")));
}

void ARunningLatePhilipsCharacter::MakePlayerSmall()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 2.0f);

	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 10.0f);
	CameraBoom->TargetArmLength = 60.0f;
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 0.0f);
	CameraBoom->CameraLagMaxDistance = 1.0f;

	GetCharacterMovement()->MaxWalkSpeed = 10;
	SpeedIncrease = 8.0f;
	MaxSpeed = 100;

	bIsSmall = true;

	bOnCameraTrain = false;
	bLockCamera = false;

	BackgroundMusicComponent->Stop();

	if (PlayerSmallComponent)
	{
		PlayerSmallComponent->Play(0.f);
	}

	GetWorldTimerManager().SetTimer(MakePlayerSmallHandler, this, &ARunningLatePhilipsCharacter::MakePlayerSmallHanlerTimer, 0.1f, true, 0.0f);

	GLog->Log(FString::Printf(TEXT("Make Small")));
}

void ARunningLatePhilipsCharacter::MakePlayerSmallHanlerTimer()
{
	FVector vScale = GetCapsuleComponent()->GetRelativeScale3D();
	
	vScale -= FVector(0.01f, 0.01f, 0.01f) * MakePlayerSmallCountFrames/3;
	GetCapsuleComponent()->SetRelativeScale3D(vScale);

	MakePlayerSmallCountFrames++;

	if (vScale.Z <= 0.15) {

		PlayerSmallComponent->Stop();

		if (BackgroundMusicPlayerSmallQueue)
		{
			BackgroundMusicPlayerSmallComponent->Play(0.f);
		}

		GetWorldTimerManager().ClearTimer(MakePlayerSmallHandler);
		MakePlayerSmallCountFrames = 0;

		//vScale -= FVector(0.15f, 0.15f, 0.15f);
		//GetCapsuleComponent()->SetRelativeScale3D(vScale);
	}
	
	GLog->Log(FString::Printf(TEXT("Timer Make Small Z: %f"), vScale.Z	));
}

void ARunningLatePhilipsCharacter::MakePlayerNormalSize()
{

	GetCharacterMovement()->MaxWalkSpeed = 100;
	SpeedIncrease = 8.0f;
	MaxSpeed = 600;

	bIsSmall = false;

	BackgroundMusicPlayerSmallComponent->Stop();

	if (PlayerNormalSizeComponent)
	{
		PlayerNormalSizeComponent->Play(0.f);
	}

	GetWorldTimerManager().ClearTimer(CoffeSpeedUpHandler);

	GetWorldTimerManager().SetTimer(MakePlayerNormalSizeHandler, this, &ARunningLatePhilipsCharacter::MakePlayerNormalSizeHanlerTimer, 0.1f, true, 0.0f);

	GLog->Log(FString::Printf(TEXT("Make Normal Size")));
}

void ARunningLatePhilipsCharacter::MakePlayerNormalSizeHanlerTimer()
{
	FVector vScale = GetCapsuleComponent()->GetRelativeScale3D();

	vScale += FVector(0.01f, 0.01f, 0.01f) * MakePlayerSmallCountFrames ;
	GetCapsuleComponent()->SetRelativeScale3D(vScale);

	MakePlayerSmallCountFrames++;

	if (vScale.Z >= 0.7) {

		PlayerNormalSizeComponent->Stop();

		if (BackgroundMusicQueue)
		{
			BackgroundMusicComponent->Play(0.f);
		}

		GetWorldTimerManager().ClearTimer(MakePlayerNormalSizeHandler);
		MakePlayerSmallCountFrames = 0;

		vScale = FVector(0.7f, 0.7f, 0.7f);
		GetCapsuleComponent()->SetRelativeScale3D(vScale);
	}

	GLog->Log(FString::Printf(TEXT("Timer Make Normal Size Z: %f"), vScale.Z));
}


void ARunningLatePhilipsCharacter::RestartCameraSet()
{
	CameraBoom->TargetArmLength = 350.0f; // (old: 300) The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 60.0f);
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagMaxDistance = 100.0f;
	CameraBoom->CameraLagSpeed = 2.0f;
}
