#include "Characters/BossCharacter.h"

#include "Characters/StatsComponent.h"
#include "Combat/CombatComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Note: The blackboard component comes from the AIController instead of the character itself.
	BlackboardComp = GetController<AAIController>()->GetBlackboardComponent();

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), InitialState);
}

// Called every frame
void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossCharacter::DetectPawn(APawn* PawnToDetect, APawn* PawnDetected)
{
	EEnemyState CurrentState{
		static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(TEXT("CurrentState")))
	};

	if ((CurrentState != EEnemyState::Idle) || (PawnToDetect != PawnDetected)) { return; }

	// TODO: We need a way to go back to Idle if the player is no longer detected.
	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Range);
}

float ABossCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

void ABossCharacter::Attack()
{
	CombatComp->RandomAttack();
}

float ABossCharacter::GetAnimDuration()
{
	return CombatComp->AnimDuration;
}

float ABossCharacter::GetBossMeleeRange()
{
	return StatsComp->Stats[EStat::BossMeleeRange];
}
