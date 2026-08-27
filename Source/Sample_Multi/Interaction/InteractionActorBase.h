// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionActorBase.generated.h"

class USphereComponent;
class ASampleCharacter;
class UNiagaraSystem;
class UNiagaraComponent;
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
	TObjectPtr<UNiagaraComponent> RootVFX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Overlap 델리게이트 바인딩을 위한 콜백 함수
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
	
	// 상호작용의 실제 동작을 파생 클래스에서 구현하기 위한 일반 가상 함수
	virtual void Interact(ASample_MultiCharacter* InCharacter);

	// 이 상호작용이 서버 권위 처리를 필요로 하는지 구분한다.
	virtual bool RequiresServer() const
	{
		return false;
	}

	// Character의 Server RPC 실행부에서 서버가 관찰한 실제 Overlap 상태를 다시 확인할 때 사용한다.
	bool CanInteract(const ASample_MultiCharacter* InCharacter) const;

protected :
	// 소유권 규칙을 설명하기 위한 비교용 RPC다.
	// 레벨 배치 Actor는 보통 클라이언트 연결이 소유하지 않으므로 클라이언트가 여기서 호출한
	// Server RPC는 서버에서 실행되지 않는다.
	// 실제 요청 진입점은 클라이언트가 소유한 Character의 Server_Interact다.
	UFUNCTION(Server, Reliable)
	void Server_Interact(ASample_MultiCharacter* InCharacter);

	// RPC가 아닌 로컬 함수다. 호출된 각 프로세스에서 VFX를 생성한다.
	void PlayVFX(const int32 PlayerID);

};
