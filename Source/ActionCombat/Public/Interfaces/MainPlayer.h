#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MainPlayer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMainPlayer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONCOMBAT_API IMainPlayer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool HasEnoughStamina(float StaminaCost) { return false; }

	virtual void EndLockonWithActor(AActor* ActorRef) {};
};
