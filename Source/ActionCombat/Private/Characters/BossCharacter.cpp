#include "Characters/BossCharacter.h"

#include "Characters/StatsComponent.h"
#include "Combat/CombatComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/MainCharacter.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/MainPlayer.h"

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

	ControllerRef = GetController<AAIController>();
	
	// Note: The blackboard component comes from the AIController instead of the character itself.
	BlackboardComp = ControllerRef->GetBlackboardComponent();

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), InitialState);

	GetWorld()->GetFirstPlayerController()->GetPawn<AMainCharacter>()
		->StatsComp->OnZeroHealthDelegate.AddDynamic(this, &ABossCharacter::HandlePlayerDeath);
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

void ABossCharacter::HandlePlayerDeath()
{
	ControllerRef->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::GameOver);
}

void ABossCharacter::HandleDeath()
{
	float AnimDuration = PlayAnimMontage(DeathAnimMontage);
	
	// Stop the AI from running.
	ControllerRef->GetBrainComponent()->StopLogic("Defeated");

	// Disable collision.
	FindComponentByClass<UCapsuleComponent>()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// End lockon.
	IMainPlayer* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn<IMainPlayer>() };
	if (PlayerRef)
	{
		PlayerRef->EndLockonWithActor(this);
	}

	// Destroy the character after the animation is done.
	FTimerHandle DeathAnimTimerHandle;
	ControllerRef->GetWorldTimerManager().SetTimer(
		DeathAnimTimerHandle,
		this,
		&ABossCharacter::FinishDeathAnim,
		AnimDuration,
		false
	);
}

void ABossCharacter::FinishDeathAnim()
{
	Destroy();
}
