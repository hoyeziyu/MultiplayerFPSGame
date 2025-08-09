// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString::Printf(TEXT("play in game: %d"), NumberOfPlayers));

        APlayerState *playerState = NewPlayer->GetPlayerState<APlayerState>();
        if (playerState)
        {
            FString playerName = playerState->GetPlayerName();
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Cyan,
                FString::Printf(TEXT("Player Name: %s joining game!"), *playerName));
        }
    }
}

void ALobbyGameMode::Logout(AController *Exiting)
{
    Super::Logout(Exiting);

    APlayerState *playerState = Exiting->GetPlayerState<APlayerState>();
    if (playerState)
    {
        int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString::Printf(TEXT("play in game: %d"), NumberOfPlayers - 1));

        FString playerName = playerState->GetPlayerName();
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("Player Name: %s leaving game!"), *playerName));
    }
}