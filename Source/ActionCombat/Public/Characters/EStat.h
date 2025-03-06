#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EStat
{
	None UMETA(DisplayName= "None Selected"),
	Health,
	MaxHealth UMETA(DisplayName = "Max Health"),
	Stamina,
	MaxStamina UMETA(DisplayName = "Max Stamina"),
	Strength,
	BossMeleeRange UMETA(DisplayName = "Boss Melee Range"),
};