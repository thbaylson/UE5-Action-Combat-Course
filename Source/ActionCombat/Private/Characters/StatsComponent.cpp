#include "Characters/StatsComponent.h"
#include <Kismet/KismetMathLibrary.h>

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
}

// Note: The check for if we have enough stamina is handled by IMainPlayer. Strange, that we didn't add it to IFighter instead.
void UStatsComponent::ReduceStamina(float Amount)
{
	Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(Stats[EStat::Stamina] - Amount, 0.0f, Stats[EStat::MaxStamina]);
}
