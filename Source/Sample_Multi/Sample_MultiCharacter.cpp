// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sample_MultiCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Sample_Multi.h"

#include "Interaction/InteractionActorBase.h"
#include "GameFramework/PlayerState.h"
#include "Sample_MultiGameState.h"

ASample_MultiCharacter::ASample_MultiCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASample_MultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASample_MultiCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASample_MultiCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASample_MultiCharacter::Look);
	}
	else
	{
		UE_LOG(LogSample_Multi, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	UE_LOG(LogTemp, Warning,
		TEXT("MovementMode = %d"),
		static_cast<int32>(GetCharacterMovement()->MovementMode));
}

void ASample_MultiCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASample_MultiCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ASample_MultiCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ASample_MultiCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASample_MultiCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ASample_MultiCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}





void ASample_MultiCharacter::TryInteract(AInteractionActorBase* TargetActor)
{
	if (!TargetActor)
		return;

	// 이 Character는 호출 클라이언트 연결이 소유하므로 Server RPC를 서버로 보낼 수 있다.
	Server_Interact(TargetActor);
	
}

// UFUNCTION(Server)의 실제 서버 실행부다.
void ASample_MultiCharacter::Server_SetGlobalText_Implementation()
{
	APlayerState* PS = GetPlayerState();

	if (!PS)
		return;

	// 클라이언트가 ID를 보내지 않고 서버가 이 Character의 PlayerState에서 ID를 읽는다.
	const int32 PlayerID = PS->GetPlayerId();

	// GameState는 모든 플레이어가 관찰해야 하는 공유 상태를 보관한다.
	if (ASample_MultiGameState* GS = GetWorld()->GetGameState<ASample_MultiGameState>())
	{
		GS->SetGlobalText(FString::FromInt(PlayerID));
	}

}

// 실제 프로젝트에서는 유효성뿐 아니라 거리와 상호작용 가능 상태도 서버에서 검증해야 한다.
void ASample_MultiCharacter::Server_Interact_Implementation(AInteractionActorBase* TargetActor)
{
	if (!IsValid(TargetActor) || !TargetActor->RequiresServer())
		return;

	// 클라이언트의 TargetActor 포인터를 그대로 신뢰하지 않고 서버 충돌 상태로 재검증한다.
	/*if (!TargetActor->CanInteract(this))
		return;*/

	TargetActor->Interact(this);

}
