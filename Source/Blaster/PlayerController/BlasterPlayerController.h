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
	virtual void OnPossess(APawn *InPawn) override;

	virtual float GetServerTime();			// Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible 向server请求时间同步

protected:
	virtual void BeginPlay() override;

	void SetHUDTime();

	// Sync time between client and server
	/*
		请求当前服务器时间，传入客户端发送请求时的时间戳；
		这是一个Server RPC，从client发送到server的请求(从client调用，server上执行)
	*/
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	/*
		在收到 ServerRequestServerTime 请求时，向客户端报告当前服务器时间。
		这是一个Client RPC，从server发送到client的响应(从server调用，client上执行)
	*/
	UFUNCTION(Client, Reliable)		// 客户端请求的时间戳		   服务器接收到客户端请求的当时时间戳
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	void CheckTimeSync(float DeltaTime);

protected:
	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;	// client向server请求时间同步的频率

	float TimeSyncRunningTime = 0.f; // 距离上次时间同步过去多久

private:
	ABlasterHUD *BlasterHUD;

	/*
		game mode应该负责这个，这里为了把比赛时间显示在屏幕上，目前暂时放在player controller里。
		需要确保客户端和服务器时间同步！！！
	*/
	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
};
