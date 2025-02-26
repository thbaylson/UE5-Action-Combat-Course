#pragma once

#include "CoreMinimal.h"
#include "FTraceSockets.generated.h"// Structs must include their generated header file.

/**
 * 
 */
USTRUCT(BlueprintType)
struct ACTIONCOMBAT_API FTraceSockets
{
	GENERATED_BODY();

	// Ex: "sword_bottom"
	UPROPERTY(EditAnywhere)
	FName StartSocketName;

	// Ex: "FX_Sword_Top"
	UPROPERTY(EditAnywhere)
	FName EndSocketName;

	// Ex: "sword_bottom"
	UPROPERTY(EditAnywhere)
	FName SocketRotationName;
};
