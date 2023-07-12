// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Components/AudioComponent.h"
#include "RunningLatePhilipsCharacter.generated.h"

UCLASS(config=Game)
class ARunningLatePhilipsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* OnFaceCamera;

public:
	ARunningLatePhilipsCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Triple Jump**/
	virtual void NotifyJumpApex() override;

	void ModifyJumpPower();
	void ResetJumpPower();

	virtual void OnJumped_Implementation() override;

	/** Crouch **/
	void CrouchStart();
	void CrouchEnd();

	/** Hit **/
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitSoundComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	/** Varible Jump**/
	void PlayerJump();
	void PlayerStopJump();

	virtual void Landed(const FHitResult& Hit) override;

	void GravityMultiplierTimer();
	void FallCheckTimer();

	/** Climbing Ledge**/
	void SeekLedge();
	void GrabLedge(float LedgeLocationZ);
	
public:	
	void ClimbUpLedgeStart();
	void ClimbUpLedgeAnim();
	void OnLedge();
	void ClimbUpLedgeEnd();
	void ClimbDownLedge();
	void ClimbLeftLedge();
	void ClimbRightLedge();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsFalledDown = false;

	void Attacked(FVector ForwardDir);
	bool IsPlayingHitMontage();
	
	void LastCar();
	void FallingOutTrain();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Construction", meta = (EditCondition = "!bReverseOnEndTimeline"))
		bool bLockCamera = false;

	AActor* CameraTrain;
	bool bOnCameraTrain = false;
	FRotator CameraTrainDirection;
		
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	virtual void Tick(float DeltaTime) override;

	void RestartCameraSet();

	UFUNCTION()
		void SetCameraTrain(bool LockCamera, AActor* CT, FRotator CameraDirection);
	
	UFUNCTION()
		void SetCameraTrainAgain();

	virtual void AddControllerPitchInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;

	FVector WallLocation;
	FVector WallNormal;
	FVector LedgeLocation;
	int LedgeLocationZAnimFrame = 0;
	AActor* LedgeActor;
	FVector LedgeActorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_ClimbToCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_ClimbToLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_ClimbToRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_Hitted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_FallingFlat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_FallingBackDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsClimbingLedgeUp = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bCanClimbingLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsClimbingLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsOnLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsSmall = false;

	/** Crouch **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsCrouching = false;

	/** Wall Slide**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bWallSliding = false;

	AActor* WallSlidingActor;

	/** Triple Jump**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int JumpCounter = 0;

	FTimerHandle JumpReset;

	/** Varible Jump**/
	float GravMultiplier = 0.1F;
	bool bJumping;
	FTimerHandle GravMultiplierHandle;
	FTimerHandle FallCheckHandle;

	/** Speed Aceleration**/
	float SpeedIncrease;
	float MaxSpeed;

	/* Game Timer*/
	bool bPauseGame = false;
	void updateGameTimer();

	/** Score **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int nLife = 3;

	int nDonut = 0;
	int nCola = 0;
	int nSub = 0;
	int nCoffe = 0;

	void sub1Life();
	void add1Life();
	void add1Donut();
	void add1Cola();
	void add1Sub();
	void add1Coffe();

	/* Coffe Speed up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
		float CoffeSpeedUpRatio = 1.5;
	
	FTimerHandle CoffeSpeedUpHandler;
	void CoffeSpeedUpTimer();

	/* Make Player Small*/
	int MakePlayerSmallCountFrames = 0;
	FTimerHandle MakePlayerSmallHandler;
	void MakePlayerSmall();
	void MakePlayerSmallHanlerTimer();

	/* Make Player Normal Size*/
	int MakePlayerNormalSizeCountFrames = 0;
	FTimerHandle MakePlayerNormalSizeHandler;
	void MakePlayerNormalSize();
	void MakePlayerNormalSizeHanlerTimer();

	// Sound Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* HitSoundQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* Jump1Queue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* Jump2Queue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* Jump3Queue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* PlayerSmallQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* PlayerNormalSizeQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* BackgroundMusicQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* BackgroundMusicPlayerSmallQueue;

private:
	UAudioComponent* HitSoundComponent;
	UAudioComponent* Jump1Component;
	UAudioComponent* Jump2Component;
	UAudioComponent* Jump3Component;
	UAudioComponent* PlayerSmallComponent;
	UAudioComponent* PlayerNormalSizeComponent;
	UAudioComponent* BackgroundMusicComponent;
	UAudioComponent* BackgroundMusicPlayerSmallComponent;
};

