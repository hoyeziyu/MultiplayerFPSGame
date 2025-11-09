// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

/*
		AGameModeBase					VS					AGameMode
		   default classes										AGameMode是AGameModeBase的子类
		   spawns player's pawns								match state;handling match states;custom match states;
		   restarts players
		   restarts the game

*/

/*
	namespace MatchState
	{
		EnteringMap;					// We are entering this map, actors are not yet ticking
		WaitingToStart;					// Actors are ticking, but the match has not yet started
		InProgress;		【只能这里加custom match states】	// Normal gameplay is occurring. Specific games will have their own state machine inside this state
		WaitingPostMatch;				// Match has ended so we aren't accepting new players, but actors are still ticking
		LeavingMap;						// We are transitioning out of the map to another location
		Aborted;						// Match has failed due to network issues or other problems, cannot continue

		如果游戏需要添加新的状态，您可能需要重写 HasMatchStarted 和 HasMatchEnded 方法来处理这些新状态。
		在 WaitingToStart 之前和 WaitingPostMatch 之后均不得添加任何状态。
	}

*/

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;

	// 处理玩家被淘汰的逻辑
	virtual void PlayerEliminated(ABlasterCharacter *ElimmedCharacter, ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController);
	virtual void RequestRespawn(ACharacter *ElimmedCharacter, AController *ElimmedController);

public:
	/*
		Stay in the WaitingToStart State
			• WarmupTime （这里game mode为每个玩家spawn默认的pawn，并没有character，我们也看不到其他的默认pawn）
		Call StartMatch()
			• Transition to the InProgress State
			• Spawn Characters
	*/

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f; // 预热时间,在 WaitingToStart状态 和 调用StartMatch() 之间的 自定义时间段

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;

private:
	float CountdownTime = 0.f;
};
