// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"

void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UScoreWidget::UpdateGameTimer(FString Value)
{
	if (TXTGameTimer)
	{
		if (TXTGameTimer->Visibility == ESlateVisibility::Hidden)
		{
			TXTGameTimer->SetVisibility(ESlateVisibility::Visible);
		}
		TXTGameTimer->SetText(FText::FromString(Value));

	}
}

void UScoreWidget::UpdateLifeCount(int32 Value)
{
	if (TXTLife)
	{
		if (TXTLife->Visibility == ESlateVisibility::Hidden)
		{
			TXTLife->SetVisibility(ESlateVisibility::Visible);
		}
		TXTLife->SetText(FText::FromString((FString::FromInt(Value))));
		
	}
	
}

void UScoreWidget::UpdateDonutCount(int32 Value)
{

	if (TXTDonut)
	{
		if (TXTDonut->Visibility == ESlateVisibility::Hidden)
		{
			TXTDonut->SetVisibility(ESlateVisibility::Visible);
		}
		TXTDonut->SetText(FText::FromString((FString::FromInt(Value))));

	}

}

void UScoreWidget::UpdateColaCount(int32 Value)
{
	// only update if more than one hit
	if (TXTCola)
	{
		if (TXTCola->Visibility == ESlateVisibility::Hidden)
		{
			TXTCola->SetVisibility(ESlateVisibility::Visible);
		}
		TXTCola->SetText(FText::FromString((FString::FromInt(Value))));

	}

}

void UScoreWidget::UpdateSubCount(int32 Value)
{
	// only update if more than one hit
	if (TXTSub)
	{
		if (TXTSub->Visibility == ESlateVisibility::Hidden)
		{
			TXTSub->SetVisibility(ESlateVisibility::Visible);
		}
		TXTSub->SetText(FText::FromString((FString::FromInt(Value))));

	}

}

void UScoreWidget::UpdateCoffeCount(int32 Value)
{
	// only update if more than one hit
	if (TXTCoffe)
	{
		if (TXTCoffe->Visibility == ESlateVisibility::Hidden)
		{
			TXTCoffe->SetVisibility(ESlateVisibility::Visible);
		}
		TXTCoffe->SetText(FText::FromString((FString::FromInt(Value))));

	}

}


void UScoreWidget::ResetScores()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, __FUNCTION__);

	if (TXTLife)
	{
		TXTLife->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TXTDonut)
	{
		TXTDonut->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TXTCola)
	{
		TXTCola->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TXTSub)
	{
		TXTSub->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TXTCoffe)
	{
		TXTCoffe->SetVisibility(ESlateVisibility::Hidden);
	}
}