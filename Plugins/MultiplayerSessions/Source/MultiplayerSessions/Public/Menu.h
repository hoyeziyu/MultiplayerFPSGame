// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	/*
		Callbacks for the custom delegates (eg:FMultiplayerOnCreateSessionComplete) on the MultiplayerSessionsSubsystem
	*/
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

private:
	// 如果这样做，C++变量名HostButton必须和widget蓝图中的HostButton变量名一致，且widget蓝图中的HostButton作为C++的HostButton的父亲存在
	UPROPERTY(meta = (BindWidget))
	UButton *HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton *JoinButton;

	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem *MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{"FreeForAll"};
	FString PathToLobby{TEXT("")};
};
