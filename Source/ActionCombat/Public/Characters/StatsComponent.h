#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EStat.h"
#include "StatsComponent.generated.h"

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ReduceHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void ReduceStamina(float Amount);

	UFUNCTION(BlueprintCallable)
	void RegenerateStamina();

	UFUNCTION(BlueprintCallable)
	void EnableStaminaRegeneration();

	UFUNCTION(BlueprintPure)
	float GetStatPercentage(EStat Current, EStat Max);
		
};
