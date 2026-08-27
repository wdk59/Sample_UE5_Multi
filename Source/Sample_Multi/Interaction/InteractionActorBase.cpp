// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionActorBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sample_MultiCharacter.h"

// Sets default values
AInteractionActorBase::AInteractionActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 상호작용 범위 콜리전
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetSphereRadius(150.f);
	SetRootComponent(InteractionCollision);

	// Overlap 델리게이트 바인딩
	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractionActorBase::OnInteractionBeginOverlap);

	// 상호작용 액터의 스태틱 메시
	RootVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RootVFX"));
	RootVFX->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	RootVFX->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AInteractionActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractionActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractionActorBase::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASample_MultiCharacter* Character = Cast<ASample_MultiCharacter>(OtherActor);

	if (!Character)
		return;

	// 로컬에서 내가 조종하는 Character에만 반응
	if (!Character->IsLocallyControlled())
		return;

	// 서버와 클라이언트 양쪽에서 Overlap이 발생 가능하기 때문에 서버 사용 여부 분기
	if (RequiresServer())
	{
		Character->TryInteract(this);
		Interact(Character);	// Client에서는 실행 안 됨
	}
	else
	{
		Interact(Character);
	}
}

void AInteractionActorBase::Interact(ASample_MultiCharacter* InCharacter)
{
	// 상속한 클래스에서 구현
}

// Client에서는 소유하지 않은 Actor를 통해 ServerRPC를 직접 호출하지 못한다는 걸 보여주기 위한 예제
void AInteractionActorBase::Server_Interact_Implementation(ASample_MultiCharacter* InCharacter)
{
	if (!InCharacter)
		return;

	Interact(InCharacter);
}

void AInteractionActorBase::PlayVFX(const int32 PlayerID)
{
	if (!VFX)
		return;

	// PIE 인스턴스 식별 (독립형 게임에서는 알 수 없음)
	const int32 PIEInstanceID = GetWorld()->GetPackage()->GetPIEInstanceID();
	UE_LOG(LogTemp, Log, TEXT("겹친 Player %d, VFX 보일 Instance %d : VFX 팡"), PlayerID, PIEInstanceID);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX, GetActorLocation());
}
