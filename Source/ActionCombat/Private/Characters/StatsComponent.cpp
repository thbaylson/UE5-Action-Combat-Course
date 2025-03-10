#include "Characters/StatsComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatsComponent::ReduceHealth(float Amount)
{
	if (Stats[EStat::Health] <= 0) { return; }

	Stats[EStat::Health] = UKismetMathLibrary::FClamp(Stats[EStat::Health] - Amount, 0.0f, Stats[EStat::MaxHealth]);

	OnHealthPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Health, EStat::MaxHealth));
}

// Note: The check for if we have enough stamina is handled by IMainPlayer. Strange, that we didn't add it to IFighter instead.
void UStatsComponent::ReduceStamina(float Amount)
{
	bCanRegenerateStamina = false;

	Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(Stats[EStat::Stamina] - Amount, 0.0f, Stats[EStat::MaxStamina]);

	FLatentActionInfo EnableStaminaRegenActionInfo{
		// Linkage. Other values help with resuming a function that's been interupted, but we don't need that functionality.
		0,
		// A unique ID. It is unlikely 100 has already been taken.
		100,
		// The function name.
		TEXT("EnableStaminaRegeneration"),
		// The class that contains the function.
		this	
	};

	UKismetSystemLibrary::RetriggerableDelay(
		GetWorld(),
		StaminaRegenerationDelayDuration,
		// This function will be called after the delay timer runs out.
		EnableStaminaRegenActionInfo
	);

	OnStaminaPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Stamina, EStat::MaxStamina));
}

void UStatsComponent::RegenerateStamina()
{
	// Return early if we can't regenerate stamina.
	if (!bCanRegenerateStamina) { return; }

	Stats[EStat::Stamina] = UKismetMathLibrary::FInterpTo_Constant(
		Stats[EStat::Stamina],
		Stats[EStat::MaxStamina],
		GetWorld()->DeltaTimeSeconds,
		StaminaRegenerationRate
	);

	OnStaminaPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Stamina, EStat::MaxStamina));
}

void UStatsComponent::EnableStaminaRegeneration()
{
	bCanRegenerateStamina = true;
}

float UStatsComponent::GetStatPercentage(EStat Current, EStat Max)
{
	return Stats[Current] / Stats[Max];
}
