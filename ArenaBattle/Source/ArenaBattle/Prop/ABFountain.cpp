// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/ABFountain.h"

#include "ArenaBattle.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	RootComponent = Body;
	Water->SetupAttachment(Body);
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if (BodyMeshRef.Object)
	{
		Body->SetStaticMesh(BodyMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (WaterMeshRef.Object)
	{
		Water->SetStaticMesh(WaterMeshRef.Object);
	}
	
	bReplicates = true;
	NetUpdateFrequency = 100.f;
	NetDormancy = DORM_Initial;
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FTimerHandle ColorTimer;
		GetWorld()->GetTimerManager().SetTimer(ColorTimer, FTimerDelegate::CreateLambda([&]
		{
			// 서버에서 ServerLightColor 랜덤값으로 업데이트
			ServerLightColor = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f);
			// OnRep 함수의 경우 서버에서는 동작을 하지 않기 때문에 서버쪽에서도 관련된 액션을 취할 수 있도록 명시적으로 호출
			// 요거 호출안하면 호스트 클라에서는 분수대 색 안 바뀜
			OnRep_ServerLightColor();
		}
		), 1.0f, true, 0.0f);

		FTimerHandle WakeUpTimer;
		GetWorld()->GetTimerManager().SetTimer(WakeUpTimer, FTimerDelegate::CreateLambda([&]
		{
			FlushNetDormancy();
		}), 10.0f, false, -1.0f);
	}
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())		// Server에서만 원본 속성을 수정
	{
		// 1초에 RotationRate만큼 돌아가도록 Local Rotation 값에 더해줌
		AddActorLocalRotation(FRotator(0, RotationRate * DeltaTime, 0));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else
	{
		ClientTimeSinceUpdate += DeltaTime;

		// 값이 업데이트되는 속도가 매우 빠르다면 굳이 예측할 필요없이 업데이트되는대로 반영만하면 되기에,
		// ClientTimeBetweenLastUpdate의 값이 매우 작다면 바로 return
		if (ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER)
		{
			return;
		}
		// 서버로부터 받을 다음 Yaw값 추정치 = 현재 서버로 받은 Yaw 회전값 + (회전률 * 다음 패킷이 받아질 것으로 예상되는 시간)
		const float EstimateRotationYaw = ServerRotationYaw + RotationRate * ClientTimeBetweenLastUpdate;
		const float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;

		FRotator ClientRotator = RootComponent->GetComponentRotation();
		const float ClientNewYaw = FMath::Lerp(ServerRotationYaw, EstimateRotationYaw, LerpRatio);
		ClientRotator.Yaw = ClientNewYaw;
		RootComponent->SetWorldRotation(ClientRotator);
	}
}

void AABFountain::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABFountain, ServerRotationYaw);
	DOREPLIFETIME_CONDITION(AABFountain, ServerLightColor, COND_InitialOnly);
}

void AABFountain::OnRep_ServerLightColor()
{
	if (!HasAuthority())
	{
		AB_LOG(LogABNetwork, Log, TEXT("ServerLightColor: %s"), *ServerLightColor.ToString());
	}
	TObjectPtr<UPointLightComponent> PointLight = Cast<UPointLightComponent>(GetComponentByClass(UPointLightComponent::StaticClass()));
	if (PointLight)
	{
		PointLight->SetLightColor(ServerLightColor);
	}
}

void AABFountain::OnRep_ServerRotationYaw()
{
	// AB_LOG(LogABNetwork, Log, TEXT("Yaw: %f"), ServerRotationYaw);
	// Tick에서 진행했던 로직 복붙
	FRotator NewRotator = RootComponent->GetComponentRotation();
	NewRotator.Yaw = ServerRotationYaw;
	RootComponent->SetWorldRotation(NewRotator);

	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0.0f;
}

bool AABFountain::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool NetRelevantResult = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	if (!NetRelevantResult)
	{
		// RealViewer : 클라이언트의 화면을 담당하는 뷰어 -> 플레이어 컨트롤러
		// SrcLocation : 리얼 뷰어의 위치
		AB_LOG(LogABNetwork, Log, TEXT("Not Relvant: [%s] %s"), *RealViewer->GetName(), *SrcLocation.ToCompactString());
	}
	return NetRelevantResult;
}