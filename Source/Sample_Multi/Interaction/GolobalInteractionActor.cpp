// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/GolobalInteractionActor.h"

#include "GameFramework/PlayerState.h"
#include "Sample_MultiCharacter.h"

AGolobalInteractionActor::AGolobalInteractionActor()
{
	bReplicates = true;	// 리플리케이션 설정
}

void AGolobalInteractionActor::Interact(ASample_MultiCharacter* InCharacter)
{
	if (!HasAuthority() || !IsValid(InCharacter))
	{
		// 서버가 호출한 게 아니면 return
		return;
	}

	const APlayerState* PlayerState = InCharacter->GetPlayerState();
	if (!IsValid(PlayerState))
		return;

	// 서버에서 호출했으면 NetMulticast RPC 호출
	Multicast_PlayVFX(PlayerState->GetPlayerId());
}

// RPC는 접미사로 _Implementation 붙여야 함
void AGolobalInteractionActor::Multicast_PlayVFX_Implementation(const int32 PlayerID)
{
	// 실제로 수행할 함수를 NetMulticast RPC를 통해 실행
	PlayVFX(PlayerID);
}
