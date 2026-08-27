// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/GolobalInteractionActor.h"

#include "GameFramework/PlayerState.h"
#include "Sample_MultiCharacter.h"

AGolobalInteractionActor::AGolobalInteractionActor()
{
	// Multicast RPC를 관련성 있는 원격 클라이언트에 전달하려면 Actor가 복제 대상이어야 한다.
	bReplicates = true;
}

void AGolobalInteractionActor::Interact(ASample_MultiCharacter* InCharacter)
{
	if (!HasAuthority() || !IsValid(InCharacter))
	{
		// 서버가 호출한 게 아니면 return
		return;
	}

	// 클라이언트는 ID를 보내지 않는다. 서버가 RPC 호출 Character의 PlayerState에서 식별자를 조회한다.
	const APlayerState* PlayerState = InCharacter->GetPlayerState();
	if (!IsValid(PlayerState))
		return;

	// 연출에 필요한 최소 값만 Multicast 매개변수로 전달한다.
	Multicast_PlayVFX(PlayerState->GetPlayerId());
}

// UFUNCTION으로 선언한 RPC의 C++ 구현 함수에는 _Implementation 접미사를 붙인다.
void AGolobalInteractionActor::Multicast_PlayVFX_Implementation(const int32 PlayerID)
{
	// 실제로 수행할 함수를 NetMulticast RPC를 통해 실행
	PlayVFX(PlayerID);
}
