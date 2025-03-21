#include "Characters/MainCharacter.h"
#include "Animations/PlayerAnimInstance.h"

#include "Characters/PlayerActionsComponent.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"

#include "Combat/CombatComponent.h"
#include "Combat/LockonComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/TheBlockComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	LockonComp = CreateDefaultSubobject<ULockonComponent>(TEXT("Lockon Component"));
	TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	TheBlockComp = CreateDefaultSubobject<UTheBlockComponent>(TEXT("The Final Block Component"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AMainCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

bool AMainCharacter::HasEnoughStamina(float StaminaCost)
{
	return StaminaCost <= StatsComp->Stats[EStat::Stamina];
}

void AMainCharacter::EndLockonWithActor(AActor* ActorRef)
{
	if (ActorRef != LockonComp->CurrentTargetActor) { return; }
	LockonComp->EndLockon();
}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{
	// If we're rolling, then we cannot take damage.
	if (PlayerActionsComp->bIsRollActive) { return false; }

	// If the block is successful, then we cannot take damage.
	if (PlayerAnim->bIsBlocking) { return !TheBlockComp->IsBlockSuccessful(Opponent); }
	
	// Assume the main character can always take damage, unless something is actively preventing it.
	return true;
}

void AMainCharacter::HandleDeath()
{
	PlayAnimMontage(DeathAnimMontage);
	DisableInput(GetController<APlayerController>());
}

void AMainCharacter::PlayHurtAnimMontage(TSubclassOf<class UCameraShakeBase> CameraShakeTemplate)
{
	PlayAnimMontage(HurtAnimMontage);

	if (CameraShakeTemplate)
	{
		GetController<APlayerController>()->ClientStartCameraShake(CameraShakeTemplate);
	}
}
