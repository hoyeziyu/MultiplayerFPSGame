// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (mBlasterCharacterPtr == nullptr)
    {
        mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if (mBlasterCharacterPtr == nullptr)
        return;

    FVector velocity = mBlasterCharacterPtr->GetVelocity();
    velocity.Z = 0.f;
    mSpeed = velocity.Size();

    mbIsInAir = mBlasterCharacterPtr->GetCharacterMovement()->IsFalling();
    mbIsAccelerating = mBlasterCharacterPtr->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    mbWeaponEquipped = mBlasterCharacterPtr->IsWeaponEquipped();
    EquippedWeapon = mBlasterCharacterPtr->GetEquippedWeapon();
    bIsCrouched = mBlasterCharacterPtr->bIsCrouched;
    bAiming = mBlasterCharacterPtr->IsAiming();
    TurningInPlace = mBlasterCharacterPtr->GetTurningInPlace();

    // Offset Yaw for Strafing
    /*
    BaseAimRotation是一个全局rotation，并不是角色本地的rotation，范围（-180，180），Yaw的值是角色面向world的方向的X轴夹角；（相当于鼠标左右移动）
    movementRotation对应于GetBaseAimRotation()相同的全局旋转
    */
    FRotator aimRotation = mBlasterCharacterPtr->GetBaseAimRotation();
    FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(mBlasterCharacterPtr->GetVelocity());
    UE_LOG(LogTemp, Warning, TEXT("AimRotation: %s, AimRotation Yaw: %f, movementRotation Yaw: %f,"), *aimRotation.ToString(), aimRotation.Yaw, movementRotation.Yaw);

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

    AO_Yaw = mBlasterCharacterPtr->GetAO_Yaw();
    AO_Pitch = mBlasterCharacterPtr->GetAO_Pitch();

    if (mbWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && mBlasterCharacterPtr->GetMesh())
    {   //                                                                                                                        相对的世界空间
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        // 从世界空间转换到骨骼空间                             skeleton上bone的名字       位置                     旋转
        mBlasterCharacterPtr->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));
    }
}