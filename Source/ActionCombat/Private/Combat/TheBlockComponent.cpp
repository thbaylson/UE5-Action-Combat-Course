// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/TheBlockComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/MainPlayer.h"

// Sets default values for this component's properties
UTheBlockComponent::UTheBlockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTheBlockComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTheBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTheBlockComponent::IsBlockSuccessful(AActor* Opponent)
{
	// Check if the owner of this component inherits from MainPlayer.
	ACharacter* CharacterRef{ GetOwner<ACharacter>() };
	if (!CharacterRef->Implements<UMainPlayer>()) { return false; }

	// Check if we have enough stamina to block.
	IMainPlayer* PlayerRef{ Cast<IMainPlayer>(CharacterRef) };
	if (!PlayerRef->HasEnoughStamina(StaminaCost)) { return false; }

	// Check if the opponent is facing towards us.
	FVector OpponentForward{ Opponent->GetActorForwardVector() };
	FVector PlayerForward{ CharacterRef->GetActorForwardVector() };
	// Returns a negative number if the two actors are facing towards each other, otherwise a positive number.
	double DotProduct{ FVector::DotProduct(OpponentForward, PlayerForward) };
	if (DotProduct > 0) { return false; }

	// By this point, all checks have passed, so the block is successful.
	return true;
}

