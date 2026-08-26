// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionActorBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
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
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

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

void AInteractionActorBase::PlayVFX()
{
	if (!VFX)
		return;

	UE_LOG(LogTemp, Log, TEXT("VFX 팡"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), VFX, GetActorLocation());
}

