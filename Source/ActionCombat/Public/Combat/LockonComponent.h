// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockonComponent.generated.h"

// This macro declares an event that other classes can listen to.
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	// The name of the event.
	// It is a good practice to use the "FOn" prefix and "Signature" suffix to indicate that it is an event.
	FOnUpdatedTargetSignature,
	// This is the class that contains the event.
	ULockonComponent,
	// The variable name.
	// It is good practice for the variable name to be similar to the event name.
	// It should not start with "F", but it should end with "Delegate".
	OnUpdatedTargetDelegate,
	// The data type of the parameter being passed to the event.
	AActor*,
	// The name of the parameter.
	NewTargetActorRef
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API ULockonComponent : public UActorComponent
{
	GENERATED_BODY()

	ACharacter* OwnerRef;
	APlayerController* Controller;

	// This is an example of forward declaration, a way to inform the compiler about the existence of a class before fully defining it.
	class UCharacterMovementComponent* MovementComp;
	class USpringArmComponent* SpringArmComp;

public:	
	// Sets default values for this component's properties
	ULockonComponent();

	AActor* CurrentTargetActor;

	UPROPERTY(BlueprintAssignable)
	FOnUpdatedTargetSignature OnUpdatedTargetDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ToggleLockon(float Radius= 750.0f);

	void StartLockon(float Radius= 750.0f);

	UPROPERTY(EditAnywhere)
	double BreakDistance{ 1000.0 };

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EndLockon();

};
