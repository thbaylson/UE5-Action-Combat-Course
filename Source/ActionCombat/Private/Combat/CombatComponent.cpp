#include "Combat/CombatComponent.h"
#include "Interfaces/MainPlayer.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::ComboAttack()
{
	// Attack stamina depends on the combo counter. Each attack gets more expensive.
	float AttackStaminaCost{ (1 + (ComboCounter * 0.1f)) * AttackStaminaBaseCost };
	// Return early if this component is not on the main character or if the main character doesn't have enough stamina.
	if (CharacterRef->Implements<UMainPlayer>())
	{
		IMainPlayer* IPlayerRef{ Cast<IMainPlayer>(CharacterRef) };
		if (IPlayerRef && !IPlayerRef->HasEnoughStamina(AttackStaminaCost))
		{
			return;
		}
	}
	else
	{
		return;
	}

	// Return early if we can't attack.
	if (!bCanAttack) { return; }
	bCanAttack = false;

	// Return early if we don't have any attack animations. This can happen when hot reload fails to re-link member variables.
	if (AttackAnimations.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("No attack animations found on Combat component for %s"), *CharacterRef->GetFName().ToString());
		return;
	}

	// Broadcast the attack performed event. This will deplete the character's stamina.
	OnAttackPerformedDelegate.Broadcast(AttackStaminaCost);

	AnimDuration = CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]);

	// This seems like a lot of extra work when we could just do:
	// ComboCounter = (ComboCounter + 1) % AttackAnimations.Num();
	ComboCounter++;

	int MaxCombo{ AttackAnimations.Num() };
	ComboCounter = UKismetMathLibrary::Wrap(
		ComboCounter,
		-1,// exclusive minimum
		MaxCombo - 1// inclusive maximum
	);
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;
}

void UCombatComponent::RandomAttack()
{
	int RandomIndex{ FMath::RandRange(0, AttackAnimations.Num() - 1) };
	AnimDuration = CharacterRef->PlayAnimMontage(AttackAnimations[RandomIndex]);
}
