// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABlasterCharacter;

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	/*
		server RPC, RPC可以有参数; OnRep_ 通知函数不能有参数；
		在复制变量之前，将自动获取复制变量的最新值(last value)，
	*/
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	/*
		server RPC, 从client调用，server执行
		Multicast RPC, server调用，所有client和server执行
		每次调用rpc，都会有网络发送数据！！！
	*/
	UFUNCTION(Server, Reliable)
	void ServerFire();	// server rpc

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();	// multicast rpc,这里不用复制（因为复制的工作方式是变量只有在更改时才会被复制）

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:
	TObjectPtr<ABlasterCharacter> Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
		
	bool bFireButtonPressed;
};
