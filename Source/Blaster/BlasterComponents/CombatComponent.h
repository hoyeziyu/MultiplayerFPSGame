// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	void EquipWeapon(AWeapon *WeaponToEquip);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading(); 

	void FireButtonPressed(bool bPressed);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

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

	/*
		server RPC, 从client调用，server执行
		Multicast RPC, server调用，所有client和server执行
		每次调用rpc，都会有网络发送数据！！！
	*/
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize &TraceHitTarget); // server rpc

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize &TraceHitTarget); // multicast rpc,这里不用复制（因为复制的工作方式是变量只有在更改时才会被复制）

	void TraceUnderCrosshairs(FHitResult &TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void Fire();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();

private:
	void InterpFOV(float DeltaTime);

	// Automatic fire
	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	void InitializeCarriedAmmo();

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();

private:
	ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	ABlasterPlayerController* Controller;
	ABlasterHUD* HUD;

	// HUD and crosshairs
	float CrosshairVelocityFactor; // 速度影响
	float CrosshairInAirFactor;	   // 空中影响
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	/**
	 * Aiming and FOV
	 */

	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = CombatComp)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = CombatComp)
	float ZoomInterpSpeed = 20.f;

	// Automatic fire
	FTimerHandle FireTimer;
	bool bCanFire = true;

	// Carried ammo for the currently-equipped weapon
	/*
		CarriedAmmo放在这里原因是，player state的复制比character更慢
	*/
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
};
