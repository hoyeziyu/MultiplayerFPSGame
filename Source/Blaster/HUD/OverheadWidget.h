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
	void SetDisplayText(FString TextToDisplay);
	
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn *InPawn);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> mDisplayTextPtr;

protected:
	// 这个方法应该是失效了
	// virtual void OnLevelRemovedFromWorld(ULevel *InLevel, UWorld *InWorld) override;
	virtual void NativeDestruct() override;
};
