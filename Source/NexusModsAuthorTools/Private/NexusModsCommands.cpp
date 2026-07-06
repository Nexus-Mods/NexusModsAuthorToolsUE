// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsCommands.h"

#define LOCTEXT_NAMESPACE "FNexusModsModule"

void FNexusModsCommands::RegisterCommands() {
	UI_COMMAND(PluginAction, "Mod Tools", "Open Nexus Mods Author Tools", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
