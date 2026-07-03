// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsAuthorTools.h"
#include "NexusModsStyle.h"
#include "NexusModsCommands.h"
#include "ToolMenus.h"

#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"

#include "Widgets/SNexusModsWindow.h"
//#include "Services/NexusModsApiClient.h"

#define LOCTEXT_NAMESPACE "FNexusModsModule"

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FNexusModsModule::StartupModule() {
	FNexusModsStyle::Initialize();
	FNexusModsStyle::ReloadTextures();
	FNexusModsCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FNexusModsCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FNexusModsModule::PluginButtonClicked),
		FCanExecuteAction()
	);
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNexusModsModule::RegisterMenus));
}

// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// we call this function before unloading the module.
void FNexusModsModule::ShutdownModule() {
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FNexusModsStyle::Shutdown();
	FNexusModsCommands::Unregister();
}

// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
void FNexusModsModule::RegisterMenus() {
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FNexusModsCommands::Get().PluginAction, PluginCommands);
		}
	}
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FNexusModsCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FNexusModsModule::PluginButtonClicked() {
	if (PluginWindow.IsValid()) {
		PluginWindow->BringToFront();
		return;
	}
	PluginWindow = SNew(SWindow)
		.Title(FText::FromString("Nexus Mods Upload"))
		.CreateTitleBar(false)
		//.SizingRule(ESizingRule::FixedSize)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.HasCloseButton(false)
		.ClientSize(FVector2D(618, 380))
		.MinWidth(618.0f)
		.MaxWidth(1024.0f)
		.MinHeight(380.0f)
		.MaxHeight(680.0f);

	PluginWindow->SetOnWindowClosed(
		FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>&) {
			PluginWindow.Reset();
		})
	);
	PluginWindow->SetContent(
		SNew(SNexusModsWindow)
		.ParentWindow(PluginWindow)
		.OnCloseRequested(FSimpleDelegate::CreateLambda([this]() {
			if (PluginWindow.IsValid()) {
				PluginWindow->RequestDestroyWindow();
			}
		}))
	);
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);
	FSlateApplication::Get().AddWindowAsNativeChild(
		PluginWindow.ToSharedRef(),
		ParentWindow.ToSharedRef()
	);
	if (ParentWindow.IsValid()) {
		FVector2D Position = FVector2D(802, 108);
		Position += ParentWindow->GetPositionInScreen();
		PluginWindow->MoveWindowTo(Position);
	}
}

#include "Interfaces/IPluginManager.h"

FString FNexusModsModule::GetPluginVersion() {
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NexusModsAuthorTools"));
	if (!Plugin.IsValid()) return TEXT("Unknown");
	return Plugin->GetDescriptor().VersionName;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNexusModsModule, NexusModsAuthorTools)