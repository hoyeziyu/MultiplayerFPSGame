// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);	// 所有机器上需要完成的事情

	if (!HasAuthority()) return;	// 希望子弹是Authority的，由server负责，确保只在server上生成子弹，然后复制到clients

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        // 从枪口焰套筒(muzzle flash socket)到命中位置（HitTarget）的“准星下追踪”
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
        // TargetRotation取决于命中的目标
		FRotator TargetRotation = ToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),  // spawn的位置
					TargetRotation,
					SpawnParams
					);
			}
		}
	}
}