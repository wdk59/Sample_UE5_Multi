// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractionActorBase.h"
#include "LocalInteractionActor.generated.h"

/**
 * 
 */
UCLASS()
class SAMPLE_MULTI_API ALocalInteractionActor : public AInteractionActorBase
{
	GENERATED_BODY()

public :

	virtual void Interact(ASample_MultiCharacter* InCharacter) override;
	
};
