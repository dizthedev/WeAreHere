// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = true; //default is true, unlike other rotations

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Cam->SetupAttachment(GetCapsuleComponent());
	Cam->AddRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	Cam->bUsePawnControlRotation = true;

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->AddRelativeLocation(FVector(50.0f, 0, 0));
	HoldingComponent->SetupAttachment(RootComponent);

	LeftHandComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandComponent"));
	LeftHandComponent->AddRelativeLocation(FVector(100.0f, -60.0f, 48.0f));
	LeftHandComponent->SetupAttachment(RootComponent);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CurrentItem = NULL;
	bCanMove = true;
	//bCanRotate = true;
	bInspecting = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PitchMax = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax;
	PitchMin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin;
	//bUseControllerRotationYaw = true;
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Start = Cam->GetComponentLocation();
	ForwardVector = Cam->GetForwardVector();
	End = ((ForwardVector * 200.0f) + Start);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue.WithAlpha(60), false, 1, 0, .5f);

	if (!bHoldingItem)
	{
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParams))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(AInteractableObject::StaticClass()))
			{
				CurrentItem = Cast<AInteractableObject>(Hit.GetActor());
			}
		}
		else
		{
			CurrentItem = NULL;
		}
	}

	if (bInspecting)
	{
		if (bHoldingItem)
		{
			Cam->SetFieldOfView(FMath::Lerp(Cam->FieldOfView, 90.0f, 0.1f));
			HoldingComponent->SetRelativeLocation(FVector(150.0f, 0.0f, 0.0f));
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 179.900002f;
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = -179.900002f;
			CurrentItem->RotateActor();
		}
		else
		{
			Cam->SetFieldOfView(FMath::Lerp(Cam->FieldOfView, 45.0f, 0.1f));
		}
	}
	else
	{
		Cam->SetFieldOfView(FMath::Lerp(Cam->FieldOfView, 90.0f, 0.1f));

		if (bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation(FVector(110.0f, 70.0f, -24.0f));
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Horizontal", this, &APlayerCharacter::HorizMove);
	InputComponent->BindAxis("Vertical", this, &APlayerCharacter::VertMove);
	InputComponent->BindAxis("Vertical", this, &APlayerCharacter::VertMove);
	//InputComponent->BindAxis("HorizontalRotation", this, &APlayerCharacter::HorizRot);
	//InputComponent->BindAxis("VerticalRotation", this, &APlayerCharacter::VertRot);
	InputComponent->BindAxis("HorizontalRotation", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("VerticalRotation", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("Pickup", IE_Pressed, this, &APlayerCharacter::OnPickup);
	InputComponent->BindAction("Inspect", IE_Pressed, this, &APlayerCharacter::OnInspect);
	InputComponent->BindAction("Inspect", IE_Released, this, &APlayerCharacter::OnInspectReleased);

}

void APlayerCharacter::HorizMove(float value)
{
	if (value && bCanMove)
	{
		AddMovementInput(GetActorRightVector(), value);
	}
}

void APlayerCharacter::VertMove(float value)
{
	if (value && bCanMove)
	{
		AddMovementInput(GetActorForwardVector(), value);
	}
}

//void APlayerCharacter::HorizRot(float value)
//{
//	if (value && bCanRotate)
//	{
//		AddActorLocalRotation(FRotator(0, value, 0));
//	}
//}
//
//void APlayerCharacter::VertRot(float value)
//{
//	if (value && bCanRotate)
//	{
//		float temp = Cam->GetRelativeRotation().Pitch + value;
//
//		if (temp <65 && temp > -65)
//		{
//			Cam->AddLocalRotation(FRotator(value, 0, 0));
//		}
//	}
//}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::OnPickup()
{
	//if our tracer is hitting an item, and we're not currently inspecting something, toggle pickup
	if (CurrentItem && !bInspecting)
	{
		ToggleItemPickup();
	}
}

void APlayerCharacter::OnInspect()
{
	if (bHoldingItem)
	{
		LastCamRotation = GetControlRotation();
		ToggleMovement();
	}
	else
	{
		bInspecting = true;
	}
}

void APlayerCharacter::OnInspectReleased()
{
	if (bInspecting && bHoldingItem)
	{
		GetController()->SetControlRotation(LastCamRotation);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
		ToggleMovement();
	}
	else
	{
		bInspecting = false;
	}
}

void APlayerCharacter::ToggleMovement()
{
	bCanMove = !bCanMove;
	bInspecting = !bInspecting;
	Cam->bUsePawnControlRotation = !Cam->bUsePawnControlRotation;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}

void APlayerCharacter::ToggleItemPickup()
{
	if (CurrentItem)
	{
		bHoldingItem = !bHoldingItem;
		CurrentItem->Pickup();

		if (!bHoldingItem)
		{
			CurrentItem = NULL;
		}
	}
}
