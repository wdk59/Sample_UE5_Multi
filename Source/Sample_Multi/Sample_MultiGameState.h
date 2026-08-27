// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Sample_MultiGameState.generated.h"

/**
 * 
 */
// 텍스트 변경 알림용 델리게이트: 위젯이 구독
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalTextChanged, const FString&);
UCLASS()
class SAMPLE_MULTI_API ASample_MultiGameState : public AGameStateBase
{
	GENERATED_BODY()

public :
	
	FOnGlobalTextChanged OnGlobalTextChanged;

	// Replicated: 단순 리플리케이트(복제)
	// ReplicatedUsing: 클라이언트가 서버로부터 값을 받은 직후 특정 함수를 자동으로 호출하고 싶을 때
	
	// 변수는 MulticastRPC를 쏘기보다 Replicate 설정을 통해 업데이트
	// 클라이언트가 서버로분터 GlobalText의 변경된 값을 받으면 OnRep_GlobalTexT()를 호출
	UPROPERTY(ReplicatedUsing = OnRep_GlobalText)
	FString GlobalText;

public :

	void SetGlobalText(const FString& NewText);

	const FString& GetGlobalText() const { return GlobalText; }

protected :
	
	// OnGlobalTextChanged 발동 함수
	UFUNCTION()
	void OnRep_GlobalText();

};
