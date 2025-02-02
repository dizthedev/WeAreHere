// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

class UCameraComponent;
class ACharacter;

UCLASS()
class WEAREHERE_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* HoldingComp;

	UFUNCTION()
		void RotateActor();

	UFUNCTION()
		void Pickup();

	bool bHolding;
	bool bGravity;

	FRotator ControlRotation;
	ACharacter* MyCharacter;
	UCameraComponent* PlayerCamera;
	FVector ForwardVector;
};
