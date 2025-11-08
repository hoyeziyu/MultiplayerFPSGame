// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;

/**
  	Player controler只存在于拥有的client和server上。
	对于网络我们要遵循所有权概念，只有player controler拥有的actor才可以向server发送数据，server再将数据发送给每个client。

	作用: 1.管理，访问HUD（ GetHUD()返回正在使用的当前HUD ）
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);

	void SetHUDMatchCountdown(float CountdownTime);
	virtual void Tick(float DeltaTime) override;

	/*
		一旦控制器拥有一个pawn，就可以在这个函数访问Pawn
	*/
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	void SetHUDTime();

private:
	ABlasterHUD* BlasterHUD;

	/*
		game mode应该负责这个，这里为了把比赛时间显示在屏幕上，目前暂时放在player controller里。
		需要确保客户端和服务器时间同步！！！
	*/
	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
};
