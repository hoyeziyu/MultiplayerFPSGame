// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 确保Projectile在server生成，生成动作被复制到clients上，server维护Projectile位置的authority

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	// 这里不用ECollisionChannel::ECC_Pawn的原因是：如果用Pawn通道，打到的是UCapsuleComponent, 而不是Character Mesh，
	// 我们在mesh上追踪，实际上也是追踪物理asset
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox, // 附加在什么场景组件上
			FName(),	  // 附加点的socket name(可以加在skeleton mesh component的bone上),不想附加就填空
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
		/*
		忽略发射者的碰撞,防止自爆；
		火箭弹的自定义移动组件URocketMovementComponent存在一个问题。当火箭弹与目标重叠时，
		它不会击中owner（这很好），但同时也会导致火箭弹完全停止飞行。当目标owner移开后，火箭弹才会再次移动
	*/
		CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
	}

	// 放在这里 和owner的碰撞太早了（应该指的是初始化生成子弹的时候），还是会自爆
	// CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
}

void AProjectile::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	// ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	// if (BlasterCharacter)
	// {
	// 	BlasterCharacter->MulticastHit();
	// }

	Destroy(); // 这里destroy服务器端的actor,会传播到所有clients
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 对复制的actor进行Destroyed，会传播到所有clients
void AProjectile::Destroyed()
{
	Super::Destroyed();
	// 放在这里，是为了client也有机会看到粒子特效
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}
