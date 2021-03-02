// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "InteractableObject.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class AInteractableObject;

UCLASS()
class WEAREHERE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere)
		class AInteractableObject* CurrentItem;

	bool bCanMove;
	bool bHoldingItem;
	bool bInspecting;

	float PitchMax;
	float PitchMin;

	FVector HoldingComp;
	FRotator LastCamRotation;

	FVector Start;
	FVector ForwardVector;
	FVector End;

	FHitResult Hit;
	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParams;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void HorizMove(float value);
	void VertMove(float value);
	//void HorizRot(float value);
	//void VertRot(float value);

	void OnPickup();
	void OnInspect();
	void OnInspectReleased();

	void ToggleMovement();
	void ToggleItemPickup();

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* Cam;

	UPROPERTY(EditAnywhere)
		class USceneComponent* HoldingComponent;

};
