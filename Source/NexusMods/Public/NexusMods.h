// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWindow;

class FNexusModsModule : public IModuleInterface {

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:
	/** Functions: */
	void RegisterMenus();
	/** Variables: */
	TSharedPtr<SWindow> PluginWindow;
	TSharedPtr<class FUICommandList> PluginCommands;

};
