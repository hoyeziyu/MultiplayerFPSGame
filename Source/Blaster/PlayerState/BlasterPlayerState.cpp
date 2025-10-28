// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
    SetScore(GetScore() + ScoreAmount);
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character.Get();
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller.Get();
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

void ABlasterPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
    // player controller存在内置访问player state的函数，player state不存在访问player controller的内置函数，但可以通过pawn访问controller
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character.Get();
    // Character 如果未初始化，可能存在旧的数据值（垃圾值），可是可以通过if判断
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller.Get();
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
    Defeats += DefeatsAmount;
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character.Get();
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller.Get();
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}

void ABlasterPlayerState::OnRep_Defeats()
{
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character.Get();
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller.Get();
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}