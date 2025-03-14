#include "Characters/PlayerActionsComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/MainPlayer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPlayerActionsComponent::UPlayerActionsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPlayerActionsComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
	MovementComp = CharacterRef->GetCharacterMovement();
	
	if (!CharacterRef->Implements<UMainPlayer>()) { return; }
	
	IPlayerRef = Cast<IMainPlayer>(CharacterRef);
}

// Called every frame
void UPlayerActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerActionsComponent::Sprint()
{
	// Return early if the player doesn't have enough stamina. And walk instead.
	if (!IPlayerRef->HasEnoughStamina(SprintCost))
	{
		Walk();
		return;
	}

	// Return early if the player is not moving.
	if (MovementComp->Velocity.Equals(FVector::ZeroVector, 1)) { return; }

	MovementComp->MaxWalkSpeed = SprintSpeed;

	OnSprintDelegate.Broadcast(SprintCost);
}

void UPlayerActionsComponent::Walk()
{
	MovementComp->MaxWalkSpeed = WalkSpeed;
}

void UPlayerActionsComponent::Roll()
{
	// Return early if the player is already rolling.
	if (bIsRollActive) { return; }

	// Return early if the player doesn't have enough stamina.
	if (!IPlayerRef->HasEnoughStamina(RollStaminaCost)) { return; }

	bIsRollActive = true;

	OnRollDelegate.Broadcast(RollStaminaCost);

	// If the player is not moving, roll in the direction the player is facing, otherwise roll in the direction of the movement.
	FVector Direction{ CharacterRef->GetCharacterMovement()->Velocity.Length() < 1 ?
		CharacterRef->GetActorForwardVector() :
		CharacterRef->GetLastMovementInputVector() 
	};

	// Note: The rotation is only on the XAxis.
	FRotator NewRot{ UKismetMathLibrary::MakeRotFromX(Direction) };

	CharacterRef->SetActorRotation(NewRot);

	float AnimationDuration = CharacterRef->PlayAnimMontage(RollAnimMontage);

	FTimerHandle RollTimerHandle;
	CharacterRef->GetWorldTimerManager().SetTimer(
		RollTimerHandle,
		this,
		&UPlayerActionsComponent::FinishRollAnim,
		AnimationDuration,
		false
	);
}

void UPlayerActionsComponent::FinishRollAnim()
{
	bIsRollActive = false;
}
