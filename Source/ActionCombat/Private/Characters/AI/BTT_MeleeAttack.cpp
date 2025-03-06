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

	if (PlayerDistance > AttackRadius)
	{
		APawn* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn() };

		FAIMoveRequest MoveRequest{ PlayerRef };
		MoveRequest.SetUsePathfinding(true);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
		OwnerComp.GetAIOwner()->SetFocus(PlayerRef);

		// For UE5.4 and later, this needs to go above the call to MoveTo.
		OwnerComp.GetAIOwner()->ReceiveMoveCompleted.AddUnique(MoveCompletedDelegate);
	}
	else
	{
		IFighter* FighterRef{ Cast<IFighter>(OwnerComp.GetAIOwner()->GetCharacter()) };
		FighterRef->Attack();
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
