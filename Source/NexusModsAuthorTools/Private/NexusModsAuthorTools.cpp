// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsAuthorTools.h"
#include "NexusModsStyle.h"
#include "NexusModsCommands.h"
#include "ToolMenus.h"

#if UNREAL_ENGINE_VERSION_BELOW(5, 0)
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#endif

#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"

#include "Widgets/SNexusModsWindow.h"
//#include "Services/NexusModsApiClient.h"
#include "Widgets/SNexusModsMarkdownWindow.h"
#include "Interfaces/IPluginManager.h"


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
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNexusModsModule::RegisterMenus));
#else
	RegisterMenus();
#endif
}

// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// we call this function before unloading the module.
void FNexusModsModule::ShutdownModule() {
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
#else
	if (FModuleManager::Get().IsModuleLoaded(TEXT("LevelEditor"))) {
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

		if (MenuExtender.IsValid()) {
			LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
			MenuExtender.Reset();
		}

		if (ToolbarExtender.IsValid()) {
			LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);
			ToolbarExtender.Reset();
		}
	}
#endif
	FNexusModsStyle::Shutdown();
	FNexusModsCommands::Unregister();
}

// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
void FNexusModsModule::RegisterMenus() {
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
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
#else
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	MenuExtender = MakeShareable(new FExtender);
	MenuExtender->AddMenuExtension(
		"WindowLayout",
		EExtensionHook::After,
		PluginCommands,
		FMenuExtensionDelegate::CreateRaw(this, &FNexusModsModule::AddMenuExtension)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Settings",
		EExtensionHook::After,
		PluginCommands,
		FToolBarExtensionDelegate::CreateRaw(this, &FNexusModsModule::AddToolbarExtension)
	);
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
#endif
}

#if UNREAL_ENGINE_VERSION_BELOW(5, 0)
void FNexusModsModule::AddMenuExtension(FMenuBuilder& Builder) {
	Builder.AddMenuEntry(FNexusModsCommands::Get().PluginAction);
}

void FNexusModsModule::AddToolbarExtension(FToolBarBuilder& Builder) {
	Builder.AddToolBarButton(
		FNexusModsCommands::Get().PluginAction,
		NAME_None,
		TAttribute<FText>(),
		TAttribute<FText>(),
		FSlateIcon(FNexusModsStyle::GetStyleSetName(), TEXT("NexusMods.PluginAction.ToolbarUE4"), TEXT("NexusMods.PluginAction.ToolbarUE4.Small"))
	);
}
#endif

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

	//// markdown file tests: 
	//const FString MarkdownFilePath = FPaths::ProjectPluginsDir() / TEXT("NexusModsAuthorTools/Resources/Markdown/HelpInfo.md");

	//TSharedPtr<SWindow> HelpWindow = SNew(SWindow)
	//	.Title(FText::FromString(TEXT("Help Info")))
	//	.ClientSize(FVector2D(900.0f, 700.0f))
	//	.CreateTitleBar(false)
	//	.SupportsMaximize(false)
	//	.SupportsMinimize(false);

	//HelpWindow->SetContent(
	//	SNew(SNexusModsMarkdownWindow)
	//		.ParentWindow(HelpWindow)
	//		.Title(FText::FromString(TEXT("Help Info")))
	//		.MarkdownFilePath(MarkdownFilePath)
	//		.OnCloseRequested(FSimpleDelegate::CreateLambda([HelpWindow]() {
	//			HelpWindow->RequestDestroyWindow();
	//		}))
	//);
	//FSlateApplication::Get().AddWindowAsNativeChild(
	//	HelpWindow.ToSharedRef(),
	//	ParentWindow.ToSharedRef()
	//);
	//if (ParentWindow.IsValid()) {
	//	FVector2D Position = FVector2D(802 + 618.0, 108);
	//	Position += ParentWindow->GetPositionInScreen();
	//	HelpWindow->MoveWindowTo(Position);
	//}
}

FString FNexusModsModule::GetPluginVersion() {
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NexusModsAuthorTools"));
	if (!Plugin.IsValid()) return TEXT("Unknown");
	return Plugin->GetDescriptor().VersionName;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNexusModsModule, NexusModsAuthorTools)