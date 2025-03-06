#include "Characters/AI/BTT_ChargeAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animations/BossAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/EEnemyState.h"

UBTT_ChargeAttack::UBTT_ChargeAttack()
{
	// Note: This must be set in order for the TickTask to ever be called.
	bNotifyTick = true;

	MoveCompletedDelegate.BindUFunction(this, "HandleMoveCompleted");
}

EBTNodeResult::Type UBTT_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO: Move these into the constructor?
	ControllerRef = OwnerComp.GetAIOwner();
	CharacterRef = ControllerRef->GetCharacter();
	BossAnim = Cast<UBossAnimInstance>(CharacterRef->GetMesh()->GetAnimInstance());

	BossAnim->bIsCharging = true;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsReadyToCharge"), false);

	bIsTaskFinished = false;
	
	return EBTNodeResult::InProgress;
}

void UBTT_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	bool bIsReadyToCharge{ OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsReadyToCharge")) };

	if (bIsReadyToCharge)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsReadyToCharge"), false);
		ChargeAtPlayer();
	}

	// Return early if the task is still executing. Everything past here will cleanup the task.
	if (!bIsTaskFinished) { return; }

	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"),EEnemyState::Melee);

	ControllerRef->ReceiveMoveCompleted.Remove(MoveCompletedDelegate);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_ChargeAttack::ChargeAtPlayer()
{
	APawn* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn() };
	FVector PlayerLocation{ PlayerRef->GetActorLocation() };

	// Initialize and configure the move request.
	FAIMoveRequest MoveRequest{ PlayerLocation };
	// Note: We must add the object "Actors > Volumes > NavMeshBoundsVolume" to the level in order for the AI to pathfind.
		// Note: By default, the "p" key toggles the visibility of the NavMeshBoundsVolume.
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	// Initiate the move request.
	ControllerRef->MoveTo(MoveRequest);

	// Make sure the AI is looking at the player.
	ControllerRef->SetFocus(PlayerRef);

	// Register the delegate to be called when the move is completed. This will need to be cleared when the task is finished.
	ControllerRef->ReceiveMoveCompleted.AddUnique(MoveCompletedDelegate);

	// Note: We keep having to re-Get the character movement. Should this be a class member?
	OriginalWalkSpeed = CharacterRef->GetCharacterMovement()->MaxWalkSpeed;
	CharacterRef->GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
}

void UBTT_ChargeAttack::HandleMoveCompleted()
{
	BossAnim->bIsCharging = false;

	// Delay finishing this task because after the charge sequence there is a 1 second long attack animation.
	FTimerHandle AttackTimerHandle;
	CharacterRef->GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&UBTT_ChargeAttack::FinishAttackTask,
		1.0f,
		false
	);
	
	CharacterRef->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
}

void UBTT_ChargeAttack::FinishAttackTask()
{
	bIsTaskFinished = true;
}
