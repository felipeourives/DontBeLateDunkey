// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class RUNNINGLATEPHILIPS_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	//UScoreWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTGameTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTLife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTDonut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTCola;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTSub;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTCoffe;

	
	void UpdateGameTimer(FString Value);
	void UpdateLifeCount(int32 Value);
	void UpdateDonutCount(int32 Value);
	void UpdateColaCount(int32 Value);
	void UpdateSubCount(int32 Value);
	void UpdateCoffeCount(int32 Value);
	
	void ResetScores();

};
