// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// __FUNCTION__ : 로그를 call한 시점에서 어떤 함수의 몇 번째 줄인지 정보를 알아올 수 있는 예약어
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
// 기존에 사용했던 UE_LOG를 대신할 우리만의 로그 매크로 지정
#define AB_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %s"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);