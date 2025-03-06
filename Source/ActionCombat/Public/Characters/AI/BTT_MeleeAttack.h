#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBAT_API UBTT_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackRadius{ 200.0f };

	UPROPERTY(EditAnywhere)
	float AcceptanceRadius{ 100.0f };

	UPROPERTY(VisibleAnywhere)
	bool bIsTaskFinished{ false };

	FScriptDelegate MoveCompletedDelegate;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
public:
	UBTT_MeleeAttack();

	UFUNCTION()
	void FinishAttackTask();
};
