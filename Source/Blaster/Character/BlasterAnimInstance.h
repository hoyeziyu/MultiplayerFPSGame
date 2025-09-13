// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

class ABlasterCharacter;

/**
 *
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABlasterCharacter> mBlasterCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float mSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbIsAccelerating = false;

	/*
		动画蓝图存在于所有机器上的每个角色；
		但是Anim Instance只能访问该机器上的变量
	
	*/
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool mbWeaponEquipped = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;
};
