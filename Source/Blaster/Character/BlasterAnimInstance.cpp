// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(mBlasterCharacterPtr == nullptr){
        mBlasterCharacterPtr = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if(mBlasterCharacterPtr == nullptr) return;


    FVector velocity = mBlasterCharacterPtr->GetVelocity();
    velocity.Z = 0.f;
    mSpeed = velocity.Size();

    mbIsInAir = mBlasterCharacterPtr->GetCharacterMovement()->IsFalling();
    mbIsAccelerating = mBlasterCharacterPtr->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

}