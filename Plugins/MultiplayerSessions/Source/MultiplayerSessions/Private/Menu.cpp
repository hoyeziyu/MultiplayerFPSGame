// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{

    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
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

    if (MultiplayerSessionsSubsystem)
    {
        // 将我们的回调绑定在委托上面
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
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

void UMenu::NativeDestruct()
{
    MenuTearDown();
    Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Green,
            FString::Printf(TEXT("UMenu::OnCreateSession: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed")));
    }

    if (bWasSuccessful)
    {
        UWorld *world = GetWorld();
        if (world)
        {
            world->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
        }
    }
}

void UMenu::HostButtonClicked()
{
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
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

void UMenu::MenuTearDown()
{
    RemoveFromParent();

    APlayerController *playerController = GetWorld()->GetFirstPlayerController();
    if (playerController)
    {

        FInputModeGameOnly inputModeData;
        playerController->SetInputMode(inputModeData);
        playerController->SetShowMouseCursor(false);
    }
}
