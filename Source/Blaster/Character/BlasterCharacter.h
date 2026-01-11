// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Equip捡枪动作 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *EquipAction;

public:
	ABlasterCharacter();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// 配置哪些变量需要从服务器同步（复制）到客户端
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(AWeapon* Weapon);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	/*
		OnRep_OverlappingWeapon中OverlappingWeapon是复制的变量名（约定），会自动调用
		OnRep_OverlappingWeapon的参数只能是是被复制的变量类型（eg:AWeapon*）
	*/
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);	//  LastWeapon是发生复制之前的旧值

	void EquipButtonPressed();
	/*
		RPC函数，必须指定rpc是否可靠（信息从client发送到server，不可靠信息可能会丢包），server RPC只会在server上执行
		--- 远程过程调用rpc 是 可以在一台机器（client）上调用的函数，并在另一台机器（server）上执行 ---
	*/
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

protected:
	/*
		Replicated只在变量改变时才起作用，并不会复制每一帧或每次网络更新；一旦其值在server上更改立即复制到所有client;
		ReplicatedUsing = OnRep_OverlappingWeapon表示当变量被replicated时会调用OnRep_OverlappingWeapon函数(代表通知)，
		但是在server中不会被调用(Replicated的工作方式只有一种：就是从server复制到client，所以永远不会复制到server【不会自我复制】)

		a. 声明Replicated
		b. 在GetLifetimeReplicatedProps函数中注册变量
	*/
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> CombatComp;

// 								                      意味我们将private变量暴露在蓝图中
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;
};

