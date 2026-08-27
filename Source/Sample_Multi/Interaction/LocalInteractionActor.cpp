// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/LocalInteractionActor.h"

#include "Sample_MultiCharacter.h"
#include "GameFramework/PlayerState.h"

void ALocalInteractionActor::Interact(ASample_MultiCharacter* InCharacter)
{
	if (!IsValid(InCharacter))
		return;

	// ID를 별도 멤버로 복사하지 않고 사용 직전에 PlayerState에서 읽는다.
	// 클라이언트에서 PlayerState 복제가 아직 적용되지 않았다면 nullptr일 수 있다.
	const APlayerState* PlayerState = InCharacter->GetPlayerState();
	if (!IsValid(PlayerState))
		return;

	// RPC를 사용하지 않으므로 Overlap을 감지한 로컬 플레이어 화면에만 보인다.
	PlayVFX(PlayerState->GetPlayerId());
}
