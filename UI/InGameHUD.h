#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Components/WidgetComponent.h"

#include "ScoreWidget.h"

#include "InGameHUD.generated.h"

/**
 *
 */
UCLASS()
class RUNNINGLATEPHILIPS_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInGameHUD();

	// Primary draw call for the HUD.
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void ShowRestarScreen();

	UFUNCTION()
		void ShowCompletedScreen();

	UFUNCTION()
		void UpdateGameTimer(FString Value);

	UFUNCTION()
		void UpdateLife(int32 Value);

	UFUNCTION()
		void UpdateDonut(int32 Value);

	UFUNCTION()
		void UpdateCola(int32 Value);

	UFUNCTION()
		void UpdateSub(int32 Value);

	UFUNCTION()
		void UpdateCoffe(int32 Value);

	UFUNCTION()
		void ResetScores();

	UPROPERTY(EditDefaultsOnly, Category = "Interactive")
		TSubclassOf<UUserWidget> ScoreWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Interactive")
		TSubclassOf<UUserWidget> RestartWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Interactive")
		TSubclassOf<UUserWidget> CompletedWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* GameFinishedQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* GameOverQueue;

private:
	UAudioComponent* GameFinishedComponent;
	UAudioComponent* GameOverComponent;

	UScoreWidget* ScoreWidget;
	UUserWidget* RestartWidget;
	UUserWidget* CompletedWidget;
};