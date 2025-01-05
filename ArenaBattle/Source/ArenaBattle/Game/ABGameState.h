// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// 게임모드가 StartPlay를 통해 게임을 시작하라고 게임스테이트에게 명령을 내리면,
	// 게임 스테이트는 HandleBeginPlay를 통해 현재 월드에 있는 모든 액터들에게 BeginPlay를 호출하고 게임을 시작하라고 명령을 내린다.
	// BUT, 이건 서버 로컬에서만 실행되는 것. 클라이언트의 게임스테이트는 게임모드에게 다이렉트로 명령받지 않음.
	virtual void HandleBeginPlay() override;

	// beReplicatedHasBegunPlay라고 하는 프로퍼티가 서버로부터 클라이언트에게로 전송돼 변경이 감지되면 OnRep_ReplicatedHasBegunPlay가 실행됨
	virtual void OnRep_ReplicatedHasBegunPlay() override;
	
};
