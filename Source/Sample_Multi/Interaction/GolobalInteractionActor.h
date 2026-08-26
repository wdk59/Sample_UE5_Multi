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
	
	// NetMulticast RPC: 서버가 클라이언트에게 실행하라고 전달
	// 서버를 거치지 않고 클라이언트에서 호출될 경우에는 호출한 클라이언트에서만 수행
	// - Server->NetMulticast->일반함수: 서버에서 수행 및 각 클라이언트에게 전달
	// - Client->NetMulticast->일반함수: 호출한 클라이언트에서 수행
	
	// InteractionActorGlobal은 Client가 소유하고 있지 않기 때문에 Server RPC의 입구로 쓰지 않음
	// Server RPC는 Character가 담당

	// RPC는 접미사로 _Implementation 붙여야 함
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayVFX();
	
};
