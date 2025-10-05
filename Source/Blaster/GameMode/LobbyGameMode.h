// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 *	GameMode类只存在于server上；如果我们从client上访问GameMode类，只会为nullptr，
 *	所以存在GameState类（存在于server和 all clients），因此来自GameState类中的数据可以通过server向下复制给clients。
 *	存在PlayerState类（存在于server和 all clients）。
 *	server上存在每个character的PlayerController类，但只有拥有client的PlayerController类才会存在于client上，
 *	all characters的Pawn（存在于server和 all clients），我们需要看到我们和其他玩家的Pawn。
 *	HUD和Widget类只存在于拥有的client上。


	GameMode类职责：
		1.Default classes
			eg: DefaultPawnClass, PlayerControllerClass, HUDClass, GameStateClass, PlayerStateClass
		2.管理游戏规则
			eg: 玩家淘汰、重生
		3.Match State
			eg: 热身时间、Match time

	GameState类职责：
		1.存储游戏状态
			eg: 跟踪得分最高球员和领先球队
		2.保持所有Player State的数组
		
	PlayerState类职责：
		1.只负责处理单个玩家的状态
			eg: 玩家得分

	PlayerController类职责：
		1.负责拥有一个pawn
		2.可以访问HUD
			eg: 显示信息、更新hud值


	=============================================server上拥有=============================================
	|   GameMode   |  GameState  | 所有玩家的PlayerState |  所有玩家的PlayerController  |    所有的Pawn    |
	|     		   |             | PlayerState1         |  PlayerController1          |         Pawn1    |
	|              |             | PlayerState2         |  PlayerController2          |         Pawn2    |
	|              |             | PlayerState3         |  PlayerController3          |         Pawn3    |
	|--------------|-------------|----------------------|-----------------------------|------------------|
	============client1拥有===================================client2拥有===================================client3拥有============
	             GameState	                      		       GameState	                       			 GameState
	PlayerState1  PlayerController1  Pawn1	    PlayerState1                     Pawn1	    PlayerState1                     Pawn1
	PlayerState2    			     Pawn2	    PlayerState2  PlayerController2  Pawn2	    PlayerState2                     Pawn2
	PlayerState3    				 Pawn3	    PlayerState3                     Pawn3	    PlayerState3  PlayerController3  Pawn3

	client1上可以访问GameState，也能访问所有的PlayerState（存在于所有client上），client1只能访问与client1相关联的PlayerController，但是能访问所有的Pawn
	client2上可以访问GameState，也能访问所有的PlayerState（存在于所有client上），client2只能访问与client2相关联的PlayerController，但是能访问所有的Pawn
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController *NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
};
