#include "Combat/TraceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/Fighter.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	SkeletalComp = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Return early if we aren't attacking.
	if (!bIsAttacking) { return; }

	TArray<FHitResult> AllResults;
	for (const FTraceSockets Socket : TraceSockets)
	{
		FVector StartSocketLocation{ SkeletalComp->GetSocketLocation(Socket.StartSocketName) };
		FVector EndSocketLocation{ SkeletalComp->GetSocketLocation(Socket.EndSocketName) };
		FQuat ShapeRotation{ SkeletalComp->GetSocketQuaternion(Socket.SocketRotationName) };

		double WeaponLength{ FVector::Distance(StartSocketLocation, EndSocketLocation) };
		FVector BoxHalfExtent{
			BoxCollisionLength, BoxCollisionLength, WeaponLength
		};
		BoxHalfExtent /= 2;

		TArray<FHitResult> OutResults;
		FCollisionShape CollisionBox{ FCollisionShape::MakeBox(BoxHalfExtent) };
		FCollisionQueryParams IgnoreParams{
			FName { TEXT("Ignore Params") },
			false,
			GetOwner()
		};

		bool bHasFoundTargets = GetWorld()->SweepMultiByChannel(
			OutResults,
			StartSocketLocation,
			EndSocketLocation,
			ShapeRotation,
			ECollisionChannel::ECC_GameTraceChannel1,
			CollisionBox,
			IgnoreParams
		);

		for (FHitResult Hit : OutResults)
		{
			AllResults.Add(Hit);
		}

#ifdef UE_BUILD_DEBUG
		FVector CenterPoint{
			UKismetMathLibrary::VLerp(
				StartSocketLocation,
				EndSocketLocation,
				0.5f
			)
		};

		UKismetSystemLibrary::DrawDebugBox(
			GetWorld(),
			CenterPoint,
			CollisionBox.GetExtent(),
			bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red,
			// Need to convert FQuat to FRotator.
			ShapeRotation.Rotator(),
			// This is the duration of the debug line in seconds.
			0.75f,
			// This is the thickness of the debug line.
			2.0f
		);
#endif
	}

	// Return early if there are no results from the sweep.
	if (AllResults.Num() == 0) { return; }

	float CharacterDamage{ 0.0f };

	IFighter* FighterRef{ Cast<IFighter>(GetOwner()) };
	if (FighterRef)
	{
		CharacterDamage = FighterRef->GetDamage();
	}

	FDamageEvent TargetAttackedEvent;

	// We use const here to tell the compiler that we won't be modifying the Hit variable.
	// We use & to pass the Hit variable by reference. This is more efficient than passing by value, which
	// would create a whole new copy of the Hit variable.
	for (const FHitResult& Hit : AllResults)
	{
		AActor* TargetActor{ Hit.GetActor() };

		// Skip this loop early if the target actor should be ignored.
		if (TargetsToIgnore.Contains(TargetActor)) { continue; }

		// UE built-in for sending a damage event to the target actor.
		TargetActor->TakeDamage(
			CharacterDamage,
			TargetAttackedEvent,
			GetOwner()->GetInstigatorController(),
			GetOwner()
		);

		TargetsToIgnore.AddUnique(TargetActor);
	}
}

// This function is called from the animation blueprint of the attacking character.
void UTraceComponent::HandleResetAttack()
{
	TargetsToIgnore.Empty();
}
