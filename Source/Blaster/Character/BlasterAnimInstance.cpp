// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterTypes/CombatState.h"

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
    bRotateRootBone = mBlasterCharacterPtr->ShouldRotateRootBone();
    bElimmed = mBlasterCharacterPtr->IsElimmed();

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
    { //                                                                                                                        相对的世界空间
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        // 从世界空间转换到骨骼空间                             skeleton上bone的名字       位置                     旋转
        mBlasterCharacterPtr->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        FTransform MuzzleTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
        FVector MuzzleX(FRotationMatrix(MuzzleTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
        // 红色线说明枪口方向
        DrawDebugLine( 
            GetWorld(),
            MuzzleTransform.GetLocation(),
            MuzzleTransform.GetLocation() + MuzzleX * 1000.f,
            FColor::Red);
        
        // 橙色线说明子弹要去哪里
         DrawDebugLine( 
            GetWorld(),
            MuzzleTransform.GetLocation(),
            mBlasterCharacterPtr->GetHitTarget(),
            FColor::Orange);
        
        // 这里考虑性能优化（网络传输，没必要每帧都 复制 击中目标），只在本地玩家计算右手旋转
        if (mBlasterCharacterPtr->IsLocallyControlled())
        {
            bLocallyControlled = true;
            FTransform RightHandTransform = mBlasterCharacterPtr->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            // 返回从起点RightHandTransform.GetLocation()到目标的旋转
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - mBlasterCharacterPtr->GetHitTarget()));
			// 这里解决，当十字准心在命中目标角色后面移动到前面时，会出现跳跃现象，平滑过渡解决这个问题
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
        }
    }

    bUseFABRIK = mBlasterCharacterPtr->GetCombatState() != ECombatState::ECS_Reloading;
    bUseAimOffsets = mBlasterCharacterPtr->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = mBlasterCharacterPtr->GetCombatState() != ECombatState::ECS_Reloading;
}