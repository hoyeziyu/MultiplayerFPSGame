// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

class ABlasterCharacter;

UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	// AllowPrivateAccess 是一个元数据属性，用于控制在蓝图编辑器中是否允许访问私有成员变量
	UPROPERTY(BlueprintReadOnly, Category = "Blaster", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABlasterCharacter> mBlasterCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float mSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbIsAccelerating = false;

	/*
		动画蓝图存在于所有机器上的每个角色；（每台机器上都有一个该角色的动画蓝图在独立运行）
		但是Anim Instance只能访问该机器上的变量（不要在动画蓝图里做逻辑判断， 角色负责同步，动画负责表现）
	*/
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbWeaponEquipped = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// 用来驱动blendspace的两个轴， Offset Yaw for Strafing(扫射)	Yaw（偏航角）对应的是左右移动
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float YawOffset;

	// 倾斜
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Lean;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;
};
