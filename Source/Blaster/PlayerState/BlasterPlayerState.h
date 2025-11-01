// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

/**
	PlayerState负责持有玩家在游戏中的状态，即使character被销毁重生
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	/**
	 * Replication notifies，复制只发生在server到客户端，所以OnRep函数不会在server上调用
	 */
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
	// client上调用
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private:
	/*
		为防止指针存在未定义行为，可以使用UPROPERTY，或者初始化为nullptr

		所有类属性，必须是 UPROPERTY 宏修饰的或者弱指针类型TWeakObjectPtr，当它们被当做垃圾回收后再次访问不会引起崩溃
		指向UObject的强指针，没有被UPROPERTY宏修饰的话，垃圾回收器不会跟踪它们，对象可以被回收，指针指向内存中无效的数据
	
		跟踪UObject类型的指针，eg:TObjectPtr、TWeakObjectPtr、TSoftObjectPtr、TSoftPtr、TSubclassof、TSoftClassPtr
		跟踪非UObject类型的指针，eg:TUniquePtr、TSharedPtr、TSharedRef、TWeakPtr
	*/
	ABlasterCharacter* Character;
	ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
