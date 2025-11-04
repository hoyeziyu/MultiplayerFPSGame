// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;
class UTexture2D;
class ABlasterCharacter;
class ABlasterPlayerController;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"), // 没有被捡起状态
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	// 这里只有确定好owner之后才能设置HUD的弹药数量
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();

	void ShowPickupWidget(bool bShowWidget);

	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent *GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent *GetWeaponMesh() const { return WeaponMesh; }

	virtual void Fire(const FVector &HitTarget);

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	void Dropped();
	bool IsEmpty();
	void AddAmmo(int32 AmmoToAdd);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }

public:
	/**
	 * Textures for the weapon crosshairs
	 */

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsBottom;

	/**
	 * Zoomed FOV while aiming
	 */

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/** 
	* Automatic fire
	*/
	UPROPERTY(EditAnywhere, Category = CombatComp)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = CombatComp)
	bool bAutomatic = true;

protected:
	virtual void BeginPlay() override;

	// 碰撞检测
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex);

private:
	// WeaponState改变时，走这里的通知
	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	// 声明为变量可复制（复制的工作方式是变量只有在更改时才会被复制）
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass; // 子弹壳
	
	// 弹药放在weapon里更合理，玩家捡起武器时，弹药也会跟随剩下的数量
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterOwnerCharacter;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
};
