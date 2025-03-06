#include "Characters/AI/BTT_RangeAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EEnemyState.h"
#include "Interfaces/Fighter.h"

EBTNodeResult::Type UBTT_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* CharacterRef{ OwnerComp.GetAIOwner()->GetPawn<ACharacter>() };
	if (!IsValid(CharacterRef))
	{
		return EBTNodeResult::Failed;
	}

	float PlayerDistance{ OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance")) };
	if (PlayerDistance < Cast<IFighter>(CharacterRef)->GetBossMeleeRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Melee);

		// This task is supposed to perform a ranged attack, which we aren't doing if the player is in 
		// melee range, so we abort and return EBTNodeResult::Aborted to make sure memory cleanup happens.
		AbortTask(OwnerComp, NodeMemory);
		return EBTNodeResult::Aborted;
	}

	CharacterRef->PlayAnimMontage(AnimMontage);

	double RandomValue{ UKismetMathLibrary::RandomFloat() };
	if (RandomValue > Threshold)
	{
		Threshold = 0.9;
		
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Charge);
	}
	else
	{
		Threshold -= 0.1;
	}

	return EBTNodeResult::Succeeded;
}
