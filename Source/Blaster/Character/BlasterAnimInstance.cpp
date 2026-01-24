// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());

}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (mBlasterCharacterPtr == nullptr)
        mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());
    if (!mBlasterCharacterPtr) return;

    FVector velocity = mBlasterCharacterPtr->GetVelocity();
    velocity.Z = 0.0f;
    mSpeed = velocity.Size();

    mbIsInAir = mBlasterCharacterPtr->GetCharacterMovement()->IsFalling();
    mbIsAccelerating = mBlasterCharacterPtr->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    mbWeaponEquipped = mBlasterCharacterPtr->IsWeaponEquipped();
    bIsCrouched = mBlasterCharacterPtr->bIsCrouched;
    bAiming = mBlasterCharacterPtr->IsAiming();

   // GetBaseAimRotation()获取的是“控制器（Controller）”的旋转角度，而不是“角色身体（Mesh/Capsule）”的旋转角度; 玩家镜头（Camera）的朝向；GetActorRotation()获取的是角色身体(胶囊体)的旋转角度
    FRotator aimRotation = mBlasterCharacterPtr->GetBaseAimRotation();  
    FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(mBlasterCharacterPtr->GetVelocity());
    UE_LOG(LogTemp, Log, TEXT("AimRotation: %s, AimRotation Yaw: %f, movementRotation Yaw: %f,"), *aimRotation.ToString(), aimRotation.Yaw, movementRotation.Yaw);
   
    /*
        使blendspace中的动画切换更平滑，为解决当yaw值从-180度跳到+180度时，会经过0度，（-180 -> 0 -> 180）导致动画播放异常的问题
        使用RInterpTo函数计算两个旋转之间的最短差值旋转
    */
    // YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation).Yaw;
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
    
    // Character Lean两帧（当前帧和上一帧）之间的Delta Yaw
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = mBlasterCharacterPtr->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);
}