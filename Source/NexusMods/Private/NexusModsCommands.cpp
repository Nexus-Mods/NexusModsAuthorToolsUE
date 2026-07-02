// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsCommands.h"

#define LOCTEXT_NAMESPACE "FNexusModsModule"

void FNexusModsCommands::RegisterCommands() {
	UI_COMMAND(PluginAction, "NexusMods", "Open NexusMods Author Tool", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
