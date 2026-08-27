// Fill out your copyright notice in the Description page of Project Settings.


#include "Sample_MultiGameState.h"

#include "Net/UnrealNetwork.h"

void ASample_MultiGameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(
		ASample_MultiGameState,
		GlobalText
	);

}

void ASample_MultiGameState::SetGlobalText(const FString& NewText)
{
	if (!HasAuthority())
		return;
	
	GlobalText = NewText;

	// 모든 Client UI 갱신
	OnRep_GlobalText();

}

void ASample_MultiGameState::OnRep_GlobalText()
{
	OnGlobalTextChanged.Broadcast(GlobalText);
}
