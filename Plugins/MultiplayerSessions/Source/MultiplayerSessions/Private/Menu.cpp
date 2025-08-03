// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    SetIsFocusable(true);

    UWorld *world = GetWorld();
    if (world)
    {
        APlayerController *playerContro = world->GetFirstPlayerController();
        if (playerContro)
        {
            FInputModeUIOnly inputModeData;
            inputModeData.SetWidgetToFocus(TakeWidget());
            inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            playerContro->SetInputMode(inputModeData);
            playerContro->SetShowMouseCursor(true);
        }
    }

    UGameInstance *gameInstance = GetGameInstance();
    if (gameInstance)
    {
        MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }
}

bool UMenu::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (HostButton)
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    if (JoinButton)
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);

    return true;
}

void UMenu::HostButtonClicked()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Blue,
            FString(TEXT("HostButtonClicked!")));
    }

    MultiplayerSessionsSubsystem->CreateSession(4, FString("FreeForAll"));
}

void UMenu::JoinButtonClicked()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("JoinButtonClicked!")));
    }
}
