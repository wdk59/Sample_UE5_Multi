// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractionActorBase.h"
#include "GolobalInteractionActor.generated.h"

/**
 * 
 */
UCLASS()
class SAMPLE_MULTI_API AGolobalInteractionActor : public AInteractionActorBase
{
	GENERATED_BODY()

public :
	
	AGolobalInteractionActor();

	
	virtual void Interact(ASample_MultiCharacter* InCharacter) override;

	// 서버에서 실행해야 하는지 검증하는 함수
	virtual bool RequiresServer() const override
	{
		return true;
	}

protected :
	
	// NetMulticast RPC: 서버에서 실행되고, 이 Actor가 복제되며 네트워크 관련성이 있는 클라이언트에 전달된다.
	// 클라이언트에서 직접 호출하면 서버나 다른 클라이언트로 전송되지 않고 호출한 로컬 인스턴스에서만 실행된다.
	// - Server -> NetMulticast -> 일반 함수: 서버와 관련성 있는 클라이언트에서 실행
	// - Client -> NetMulticast -> 일반 함수: 호출한 클라이언트의 로컬 인스턴스에서만 실행
	
	// 레벨 배치 Interaction Actor는 클라이언트 연결이 소유하지 않으므로 Server RPC 진입점으로 사용하지 않는다.
	// 클라이언트가 소유한 Character가 Server RPC 진입점을 담당한다.

	// UFUNCTION으로 선언한 RPC의 C++ 구현 함수에는 _Implementation 접미사를 붙인다.
	// 일회성 연출은 손실되어도 게임 상태가 깨지지 않으므로 Reliable 큐를 점유하지 않는다.
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayVFX(const int32 PlayerID);
	
};
