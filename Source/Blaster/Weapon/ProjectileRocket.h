// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;

/**
 *
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileRocket();
	virtual void Destroyed() override;

protected:
	virtual void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit) override;
	virtual void BeginPlay() override;
	void DestroyTimerFinished();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> TrailSystem;

	/*
		需要material用于Niagara系统的渲染
		“sprite”只不过是一个包含material信息的plane,这个plane会一直旋转面对camera，我们永远不会看到plane的侧面或背面，总是直接把material直接显示在camera上
	*/
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ProjectileLoop;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundAttenuation> LoopingSoundAttenuation;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
};