// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Sample_MultiWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Sample_MultiGameState.h"
#include "Sample_MultiCharacter.h"

void USample_MultiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트
	if (ChangeTextButton)
	{
		// Button Click 이벤트 델리게이트 구독
		ChangeTextButton->OnClicked.AddDynamic(
			this,
			&USample_MultiWidget::OnChangeTextButtonClicked
		);
	}
	
	// GameState 가져오기
	ASample_MultiGameState* GameState = GetWorld()->GetGameState<ASample_MultiGameState>();

	if (GameState)
	{
		// GlobalText 값 변경 델리게이트 구독
		GameState->OnGlobalTextChanged.AddUObject(
			this,
			&USample_MultiWidget::OnGlobalTextChanged
		);

		// Widget 생성 전에 이미 복제되어 있었을 수도 있으므로
		// 현재 값으로 한 번 초기화
		OnGlobalTextChanged(GameState->GetGlobalText());
	}

}

void USample_MultiWidget::OnChangeTextButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Button Clicked"));

	// Widget은 복제되지 않으므로 소유 Pawn을 통해 Server RPC를 요청한다.
	ASample_MultiCharacter* Character = GetOwningPlayerPawn<ASample_MultiCharacter>();
	
	if (!Character)
		return;

	Character->Server_SetGlobalText();

}

void USample_MultiWidget::OnGlobalTextChanged(const FString& NewText)
{
	UE_LOG(LogTemp, Log, TEXT("Update Text"));

	if (GlobalTextBlock)
	{
		GlobalTextBlock->SetText(FText::FromString(NewText));
	}
}
