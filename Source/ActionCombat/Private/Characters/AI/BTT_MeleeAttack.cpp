#include "Characters/AI/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interfaces/Fighter.h"
#include "GameFramework/Character.h"
#include "Characters/EEnemyState.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	bNotifyTick = true;
	MoveCompletedDelegate.BindUFunction(this, "FinishAttackTask");

	// If we ever plan on having multiples of this enemy at once, we need to set these settings:
	//bCreateNodeInstance = true;// This property would need to be set in every BTTaskNode constructor that the boss uses.
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
	float PlayerDistance{ OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance")) };

	AAIController* AIControllerRef{ OwnerComp.GetAIOwner() };

	if (PlayerDistance > Cast<IFighter>(AIControllerRef->GetCharacter())->GetBossMeleeRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Range);

		AIControllerRef->StopMovement();
		AIControllerRef->ClearFocus(EAIFocusPriority::Gameplay);
		AIControllerRef->ReceiveMoveCompleted.Remove(MoveCompletedDelegate);

		AbortTask(OwnerComp, NodeMemory);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}

	// Return early if the task is still executing. Everything past here will cleanup the task.
	if (!bIsTaskFinished) { return; }

	AIControllerRef->ReceiveMoveCompleted.Remove(MoveCompletedDelegate);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_MeleeAttack::FinishAttackTask()
{
	bIsTaskFinished = true;
}
