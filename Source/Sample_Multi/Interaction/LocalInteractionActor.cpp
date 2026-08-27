// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/LocalInteractionActor.h"

#include "Sample_MultiCharacter.h"
#include "GameFramework/PlayerState.h"

void ALocalInteractionActor::Interact(ASample_MultiCharacter* InCharacter)
{
	if (!IsValid(InCharacter))
		return;

	const APlayerState* PlayerState = InCharacter->GetPlayerState();
	if (!IsValid(PlayerState))
		return;

	// Interact가 호출된 클라이언트에서만 실행되면 되기 때문에 일반 함수 호출
	PlayVFX(PlayerState->GetPlayerId());
}
