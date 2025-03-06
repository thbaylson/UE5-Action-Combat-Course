#include "Characters/AI/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interfaces/Fighter.h"
#include "GameFramework/Character.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	bNotifyTick = true;
	MoveCompletedDelegate.BindUFunction(this, "FinishAttackTask");
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bIsTaskFinished = false;
	float PlayerDistance{ OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance")) };

	AAIController* AIControllerRef{ OwnerComp.GetAIOwner() };

	if (PlayerDistance > AttackRadius)
	{
		APawn* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn() };

		FAIMoveRequest MoveRequest{ PlayerRef };
		MoveRequest.SetUsePathfinding(true);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		AIControllerRef->MoveTo(MoveRequest);
		AIControllerRef->SetFocus(PlayerRef);

		// For UE5.4 and later, this needs to go above the call to MoveTo.
		AIControllerRef->ReceiveMoveCompleted.AddUnique(MoveCompletedDelegate);
	}
	else
	{
		IFighter* FighterRef{ Cast<IFighter>(AIControllerRef->GetCharacter()) };
		FighterRef->Attack();

		FTimerHandle AttackTimerHandle;
		AIControllerRef->GetCharacter()->GetWorldTimerManager().SetTimer(
			AttackTimerHandle,
			this,
			&UBTT_MeleeAttack::FinishAttackTask,
			FighterRef->GetAnimDuration(),
			false
		);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Return early if the task is still executing. Everything past here will cleanup the task.
	if (!bIsTaskFinished) { return; }

	OwnerComp.GetAIOwner()->ReceiveMoveCompleted.Remove(MoveCompletedDelegate);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_MeleeAttack::FinishAttackTask()
{
	bIsTaskFinished = true;
}
