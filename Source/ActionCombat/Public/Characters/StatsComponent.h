#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EStat.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnHealthPercentUpdateSignature,
	UStatsComponent, OnHealthPercentUpdateDelegate,
	float, HealthPercent
);

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnStaminaPercentUpdateSignature,
	UStatsComponent, OnStaminaPercentUpdateDelegate,
	float, StaminaPercent
);

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(
	FOnZeroHealthSignature,
	UStatsComponent, OnZeroHealthDelegate
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bCanRegenerateStamina{ true };

	UPROPERTY(EditAnywhere)
	double StaminaRegenerationRate{ 10 };

	UPROPERTY(EditAnywhere)
	float StaminaRegenerationDelayDuration{ 2.0f };

public:	
	// Sets default values for this component's properties
	UStatsComponent();

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EStat>, float> Stats;

	UPROPERTY(BlueprintAssignable)
	FOnHealthPercentUpdateSignature OnHealthPercentUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnStaminaPercentUpdateSignature OnStaminaPercentUpdateDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnZeroHealthSignature OnZeroHealthDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// This implementation requires us to use the "Damage Causer" pin from "AnyDamage" event in the blueprint.
	// "Instigated By" would be more correct. Find details here: 
	// https://www.gamedev.tv/courses/unreal-action-combat/using-dot-products/6620/a-bit-more-correct-way-to-connect-pins-to-reduce-health/17151
	UFUNCTION(BlueprintCallable)
	void TryReduceHealth(AActor* Opponent, float Amount);

	UFUNCTION(BlueprintCallable)
	void ReduceStamina(float Amount);

	UFUNCTION(BlueprintCallable)
	void RegenerateStamina();

	UFUNCTION(BlueprintCallable)
	void EnableStaminaRegeneration();

	UFUNCTION(BlueprintPure)
	float GetStatPercentage(EStat Current, EStat Max);
		
};
