// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

class ABlasterPlayerState;

/**
 * AGameState 类可能是server和client之间共享信息最重要的类；
 * AGameState 类会被复制到所有client
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;
private:

	float TopScore = 0.f;
};