#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBAT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsBlocking{ false };

// Variables that will be exposed to Blueprints should be protected or public.
protected:
	// This is an example of direct initialization (as opposed to copy initialization).
	// The distinction between the two is a C++ specific concept.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentSpeed{ 0.0f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentDirection{ 0.0f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsInCombat{ false };

	UFUNCTION(BlueprintCallable)
	void UpdateSpeed();

public:
	UFUNCTION(BlueprintCallable)
	void HandleUpdatedTarget(AActor* TargetActorRef);

	UFUNCTION(BlueprintCallable)
	void UpdateDirection();


};
