// Fill out your copyright notice in the Description page of Project Settings.

#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector &HitTarget)
{
    Super::Fire(HitTarget);

    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
        return;
    AController *InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket *MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    // if (MuzzleFlashSocket && InstigatorController)   这里注释掉InstigatorController
    // 因为InstigatorController在所有的模拟代理上都为none
    if (MuzzleFlashSocket)
    {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();
        FVector End = Start + (HitTarget - Start) * 1.25f; // 加系数可能是打到表面不触发hit事件，为了进一步击中

        FHitResult FireHit;
        UWorld *World = GetWorld();
        if (World)
        {
            World->LineTraceSingleByChannel(
                FireHit,
                Start,
                End,
                ECollisionChannel::ECC_Visibility);

            FVector BeamEnd = End;
            if (FireHit.bBlockingHit)
            {
                BeamEnd = FireHit.ImpactPoint;
                ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
                if (BlasterCharacter && HasAuthority() && InstigatorController)
                {
                    UGameplayStatics::ApplyDamage(
                        BlasterCharacter,
                        Damage,
                        InstigatorController,
                        this,
                        UDamageType::StaticClass());
                }

                if (ImpactParticles)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(
                        World,
                        ImpactParticles,
                        FireHit.ImpactPoint,
                        FireHit.ImpactNormal.Rotation() // 撞击法线旋转
                    );
                }

                if (BeamParticles)
                {
                    UParticleSystemComponent *Beam = UGameplayStatics::SpawnEmitterAtLocation(
                        World,
                        BeamParticles,
                        SocketTransform);
                    if (Beam)
                    {
                        Beam->SetVectorParameter(FName("Target"), BeamEnd);
                    }
                }

            }
        }
    }
}