// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"


/*
	界面数据处理都走事件,单项依赖关系
	Declaring our own custom delegates for the Menu class to bind callbacks to
	MULTICAST的意思是，多个类可以将他们的函数绑定到此委托，一旦broadcast，就会callback所有绑定的函数(这里绑定的函数必须是UFUNCTION!!!不然会绑定失败)
	DYNAMIC的意思是，说明这个委托可以序列化
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/*
		To handle session functionality. The Menu class will call these
	*/
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	/*
		Our own custom delegates for the Menu class to bind callbacks to
	*/
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

protected:
	/*
		Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
		These don't need to be called outside this class.
	*/
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	/*
		To add to the Online Session Interface delegate list.
		We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	*/
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;	// 一旦我们将委托添加到委托列表中，   
	FDelegateHandle CreateSessionCompleteDelegateHandle;			// 用来处理的函数将返回一个FDelegateHandle,并将其存储在CreateSessionCompleteDelegateHandle变量中
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

};
