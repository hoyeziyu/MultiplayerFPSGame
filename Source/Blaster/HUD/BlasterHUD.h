// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UTexture2D;
class UCharacterOverlay;
class UUserWidget;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UTexture2D *CrosshairsCenter;
	UTexture2D *CrosshairsLeft;
	UTexture2D *CrosshairsRight;
	UTexture2D *CrosshairsTop;
	UTexture2D *CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};


/*
	HUD类可以用作一个隔离区域，用于管理UserWidgets的创建、销毁、显示、隐藏等。
*/
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage &Package) { HUDPackage = Package; }

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UCharacterOverlay *CharacterOverlay;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();

private:
	void DrawCrosshair(UTexture2D *Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
};
