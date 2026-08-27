// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sample_MultiWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class SAMPLE_MULTI_API USample_MultiWidget : public UUserWidget
{
	GENERATED_BODY()

protected :
	
	virtual void NativeConstruct() override;

protected :
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GlobalTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ChangeTextButton;

private :
	
	UFUNCTION()
	void OnChangeTextButtonClicked();

	void OnGlobalTextChanged(const FString& NewText);
	
};
