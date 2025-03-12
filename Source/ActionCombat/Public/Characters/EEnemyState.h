#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EEnemyState
{
	Idle,
	Range,
	Charge,
	Melee,
	GameOver UMETA(DisplayName = "Game Over"),
};