// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionActorBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ASampleCharacter;
class UNiagaraSystem;
class ASample_MultiCharacter;

UCLASS()
class SAMPLE_MULTI_API AInteractionActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionActorBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> InteractionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Overlap에 대한 델리게이트 바인등을 위한 콜백 함수
	UFUNCTION()
	void OnInteractionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	

protected :

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> VFX = nullptr;

public :
	
	// 서버 RPC: 서버에서 실행할 상호작용
	virtual void Interact(ASample_MultiCharacter* InCharacter);

	// 유효성 검사용 함수: 이 상호작용이 서버를 거쳐야 하는지
	virtual bool RequiresServer() const
	{
		return false;
	}

protected :

	// 일반 함수: VFX 활성
	void PlayVFX();

};
