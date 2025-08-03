// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

private:
	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

private:
	// 如果这样做，C++变量名HostButton必须和widget蓝图中的HostButton变量名一致，且widget蓝图中的HostButton作为C++的HostButton的父亲存在
	UPROPERTY(meta = (BindWidget))
	UButton *HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton *JoinButton;


	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
};
