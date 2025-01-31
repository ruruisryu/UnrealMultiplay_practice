// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Water;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
	float ServerRotationYaw;

	UPROPERTY(ReplicatedUsing = OnRep_ServerLightColor)
	FLinearColor ServerLightColor;

	// property replication function
	UFUNCTION()
	void OnRep_ServerLightColor();
	
	UFUNCTION()
	void OnRep_ServerRotationYaw();

	// RPC function
	// Client RPC

	// NetMulticast RPC
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRPCChangeLigthColor(const FLinearColor& NewColor);
	
	float RotationRate = 30.0f;
	float ClientTimeSinceUpdate = 0.0f;			// 서버로부터 데이터를 받고 시간이 얼마나 경과했는지를 기록
	float ClientTimeBetweenLastUpdate = 0.0f;	// 서버로부터 데이터를 받고 그 다음 데이터를 받았을 때 걸린 시간을 기록

	bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
};
