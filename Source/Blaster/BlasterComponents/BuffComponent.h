// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ABlasterCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UBuffComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    UBuffComponent();

    void Heal(float HealAmount, float HealingTime);
    void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

public:
    friend ABlasterCharacter;

protected:
    virtual void BeginPlay() override;

    void HealRampUp(float DeltaTime);

private:
    UFUNCTION(NetMulticast, Reliable)
    void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

private:
    UPROPERTY()
    TObjectPtr<ABlasterCharacter> Character;

    // Heal buff
    bool bHealing = false;
    float HealingRate = 0; // 目前的治愈率，每秒治愈多少个点
    float AmountToHeal = 0.f;

    // Speed buff
    FTimerHandle SpeedBuffTimer;
    void ResetSpeeds();
    float InitialBaseSpeed;
    float InitialCrouchSpeed;
};