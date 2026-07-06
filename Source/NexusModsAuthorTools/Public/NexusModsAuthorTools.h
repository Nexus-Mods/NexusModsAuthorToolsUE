// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "NexusModsUECompatibility.h"

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
	/** Helper functions: **/
	static FString GetPluginVersion();

private:
	/** Functions: */
	void RegisterMenus();

#if UNREAL_ENGINE_VERSION_BELOW(5, 0)
	void AddMenuExtension(FMenuBuilder& Builder);
	void AddToolbarExtension(FToolBarBuilder& Builder);
#endif

	/** Variables: */
	TSharedPtr<SWindow> PluginWindow;
	TSharedPtr<class FUICommandList> PluginCommands;

#if UNREAL_ENGINE_VERSION_BELOW(5, 0)
	TSharedPtr<class FExtender> MenuExtender;
	TSharedPtr<class FExtender> ToolbarExtender;
#endif

};
