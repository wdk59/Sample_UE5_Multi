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

	// 상호작용 액터에 부착할 Niagara Component
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

	// 충돌이 활성화된 복제 Actor의 Overlap은 서버와 각 클라이언트에서 각각 발생할 수 있다.
	// 로컬 효과는 즉시 실행하고, 전역 효과는 소유 중인 Character를 RPC 통로로 사용한다.
	if (RequiresServer())
	{
		Character->TryInteract(this);

		// 소유권 학습용 비교 호출이다. 원격 클라이언트에서는 Authority 검사로 종료되지만,
		// Listen Server 호스트에서는 Authority도 가지므로 중복 실행될 수 있다.
		Server_Interact(Character);

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

bool AInteractionActorBase::CanInteract(const ASample_MultiCharacter* InCharacter) const
{
	return IsValid(InCharacter)
		&& IsValid(InteractionCollision)
		&& InteractionCollision->IsOverlappingActor(InCharacter);
}

// Server RPC 소유권 규칙을 비교하기 위한 예제다. 정상 상호작용 흐름에서는 사용하지 않는다.
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

	// PIE 창을 구분하기 위한 에디터 전용 값이다. 실제 플레이어 식별자로 사용하면 안 된다.
	const int32 PIEInstanceID = GetWorld()->GetPackage()->GetPIEInstanceID();
	UE_LOG(LogTemp, Log, TEXT("겹친 Player %d, VFX 보일 Instance %d : VFX 팡"), PlayerID, PIEInstanceID);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX, GetActorLocation());
}
