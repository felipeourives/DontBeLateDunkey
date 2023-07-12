// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"


AInGameHUD::AInGameHUD()
{
	GameOverComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GameOverComponent"));
	GameOverComponent->SetupAttachment(RootComponent);

	GameFinishedComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GameFinishedComponent"));
	GameFinishedComponent->SetupAttachment(RootComponent);
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ScoreWidgetClass)
	{
		ScoreWidget = CreateWidget<UScoreWidget>(GetWorld(), ScoreWidgetClass);

		/** Make sure widget was created */
		if (ScoreWidget)
		{
			/** Add it to the viewport */
			ScoreWidget->AddToViewport();
		}
	}

	if (GameFinishedQueue && GameFinishedComponent)
	{
		GameFinishedComponent->SetSound(GameFinishedQueue);
	}

	if (GameOverQueue && GameOverComponent)
	{
		GameOverComponent->SetSound(GameOverQueue);
	}

}

void AInGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ScoreWidget)
	{

	}
}

void AInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}


void AInGameHUD::ShowRestarScreen()
{

	if (RestartWidgetClass)
	{
		RestartWidget = CreateWidget<UUserWidget>(GetWorld(), RestartWidgetClass);

		/** Make sure widget was created */
		if (RestartWidget)
		{
			/** Add it to the viewport */
			RestartWidget->AddToViewport();

			GameOverComponent->Play(0.0f);
		}
	}

}

void AInGameHUD::ShowCompletedScreen()
{
	if (CompletedWidgetClass)
	{
		CompletedWidget = CreateWidget<UUserWidget>(GetWorld(), CompletedWidgetClass);

		/** Make sure widget was created */
		if (CompletedWidget)
		{
			/** Add it to the viewport */
			CompletedWidget->AddToViewport();

			GameFinishedComponent->Play(0.0f);
		}
	}
}

void AInGameHUD::UpdateGameTimer(FString Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateGameTimer(Value);
	}
}

void AInGameHUD::UpdateLife(int32 Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateLifeCount(Value);
	}
}

void AInGameHUD::UpdateDonut(int32 Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateDonutCount(Value);
	}
}

void AInGameHUD::UpdateCola(int32 Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateColaCount(Value);
	}
}

void AInGameHUD::UpdateSub(int32 Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateSubCount(Value);
	}
}

void AInGameHUD::UpdateCoffe(int32 Value)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateCoffeCount(Value);
	}
}

void AInGameHUD::ResetScores()
{
	if (ScoreWidget)
	{
		ScoreWidget->ResetScores();
	}
}