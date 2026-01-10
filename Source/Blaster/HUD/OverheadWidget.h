// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void setDisplayText(FString &textToDisplay);

	UFUNCTION(BlueprintCallable)
	void showPlayerNetRole(APawn *InPawn);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> mDisplayTextPtr;

protected:
	virtual void NativeDestruct() override;
};
