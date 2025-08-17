// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

    if(NumberOfPlayers == 2)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            /*
                当client走non-SeamlessTravel时，client断开与当前server的连接，然后重连同一个server（断开和重连需要时间），
                （使用non-SeamlessTravel的时候： 第一次map加载时、第一次连接到server时、多人游戏结束和开始新的一个时）

                travel不同的level，最好使用SeamlessTravel，client不必与server断开连接，还避免了重连问题，
                使用SeamlessTravel，需要创建Transition Level或者Transition Map(用它从一个map过渡到另一个map，需要过渡
                的原因是map必须在任何给定时间点加载)
            */
            bUseSeamlessTravel = true;

            /*
                ServerTravel只能server调用，所有连接的client跟随进入new level(通过获取所有连接的client的PlayerController，
                并调用ClientTravel实现)；
                ClientTravel，当client调用时，将前往new server（需要提供地址）；当server调用时，会使玩家travel到server指定的level。
            */
            World->ServerTravel(TEXT("/Game/Maps/BlasterMap?listen"));
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