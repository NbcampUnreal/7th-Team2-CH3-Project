// Copyright © 2026 비전공회담. All rights reserved.


#include "Tem2ExodusGameMode.h"
#include "ABaseCharacter.h"
#include "AExodusPlayerController.h"

ATeam2ExodusGameMode::ATeam2ExodusGameMode()
{
	DefaultPawnClass = AABaseCharacter::StaticClass();
	PlayerControllerClass = AAExodusPlayerController::StaticClass();
}