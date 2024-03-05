// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputConfigData.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(25.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	CrouchEyeOffset = FVector(0.f);
	CrouchSpeed = 12.f;

}

void AFPSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AFPSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AFPSCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSCharacter::MoveForward(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, MoveValue.Y);
	}
}

void AFPSCharacter::MoveBackward(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, MoveValue.Y);
	}
}

void AFPSCharacter::MoveRight(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		const FVector RightDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, MoveValue.X);
	}
}

void AFPSCharacter::MoveLeft(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		const FVector RightDirection = FRotationMatrix(MovementRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, MoveValue.X);
	}
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();

		if (LookValue.X != 0.f) {
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void AFPSCharacter::Sprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
}

void AFPSCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AFPSCharacter::StartCrouch()
{
	Crouch();
}

void AFPSCharacter::StopCrouch()
{
	UnCrouch();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(InputActions->InputMoveForward, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveForward);
	PEI->BindAction(InputActions->InputMoveBackward, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveBackward);
	PEI->BindAction(InputActions->InputMoveRight, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveRight);
	PEI->BindAction(InputActions->InputMoveLeft, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveLeft);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started, this, &AFPSCharacter::Sprint);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed, this, &AFPSCharacter::StopSprint);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &ACharacter::Jump);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Started, this, &AFPSCharacter::StartCrouch);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Completed, this, &AFPSCharacter::StopCrouch);
}

