// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/GolobalInteractionActor.h"

AGolobalInteractionActor::AGolobalInteractionActor()
{
	bReplicates = true;	// 리플리케이션 설정
}

void AGolobalInteractionActor::Interact(ASample_MultiCharacter* InCharacter)
{
	if (!HasAuthority())
	{
		// 서버가 호출한 게 아니면 return
		return;
	}

	// 서버에서 호출했으면 NetMulticast RPC 호출
	Multicast_PlayVFX();
}

// RPC는 접미사로 _Implementation 붙여야 함
void AGolobalInteractionActor::Multicast_PlayVFX_Implementation()
{
	// 실제로 수행할 함수를 NetMulticast RPC를 통해 실행
	PlayVFX();
}
