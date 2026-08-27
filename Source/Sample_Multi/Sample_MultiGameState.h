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
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	FOnGlobalTextChanged OnGlobalTextChanged;

	// Replicated: 서버 권위 프로퍼티의 값을 클라이언트에 복제한다.
	// ReplicatedUsing(RepNotify): 클라이언트에 복제된 값의 변경이 적용될 때 지정한 OnRep 함수를 호출한다.
	// OnRep 함수는 서버에서는 자동으로 호출되지 않으며, 두 방식 모두 GetLifetimeReplicatedProps에 등록해야 한다.
	
	// 지속 상태는 Multicast RPC 대신 프로퍼티 복제를 사용한다.
	// 클라이언트에 GlobalText의 복제된 변경이 적용되면 OnRep_GlobalText()가 호출되므로
	// 늦게 참가한 클라이언트도 서버의 최신 값을 받을 수 있다.
	UPROPERTY(ReplicatedUsing = OnRep_GlobalText)
	FString GlobalText;

public :

	void SetGlobalText(const FString& NewText);

	const FString& GetGlobalText() const { return GlobalText; }

protected :
	
	// 클라이언트의 복제 알림 함수. UI가 구독한 델리게이트를 Broadcast한다.
	UFUNCTION()
	void OnRep_GlobalText();

};
