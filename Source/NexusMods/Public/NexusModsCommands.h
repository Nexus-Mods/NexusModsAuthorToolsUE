// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "NexusModsStyle.h"

class FNexusModsCommands : public TCommands<FNexusModsCommands> {
public:

	FNexusModsCommands()
		: TCommands<FNexusModsCommands>(TEXT("NexusMods"), NSLOCTEXT("Contexts", "NexusMods", "NexusMods Plugin"), NAME_None, FNexusModsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
