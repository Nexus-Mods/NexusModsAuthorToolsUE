// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsStyle.h"
#include "NexusModsAuthorTools.h"
#include "NexusModsUECompatibility.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateImageBrush.h"

#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
#include "Styling/SlateStyleMacros.h"
#define RootToContentDir Style->RootToContentDir
#else
#define RootToContentDir Style->RootToContentDir
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define IMAGE_BRUSH_SVG(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#endif

TSharedPtr<FSlateStyleSet> FNexusModsStyle::StyleInstance = nullptr;

const FLinearColor FNexusModsStyle::NexusOrange = FLinearColor(FColor::FromHex(TEXT("#e6832b")));
const FLinearColor FNexusModsStyle::NexusOrangeLight = FLinearColor(FColor::FromHex(TEXT("#faa431")));
const FLinearColor FNexusModsStyle::WindowBackgroundColor = FLinearColor(0.018f, 0.018f, 0.018f, 1.0f);
const FLinearColor FNexusModsStyle::SectionHeaderBackgroundColor = FLinearColor(0.035f, 0.035f, 0.035f, 1.0f);

const FMargin FNexusModsStyle::SectionHeaderPadding = FMargin(6.0f);
const FMargin FNexusModsStyle::FormRowLabelPadding = FMargin(8.0f, 4.0f);
const FMargin FNexusModsStyle::FormRowValuePadding = FMargin(8.0f, 4.0f);
const FMargin FNexusModsStyle::InlineControlPadding = FMargin(4.0f, 0.0f, 0.0f, 0.0f);
const FMargin FNexusModsStyle::ButtonGroupPadding = FMargin(0.0f, 0.0f, 6.0f, 0.0f);
const FMargin FNexusModsStyle::ContentPadding = FMargin(8.0f);
const FMargin FNexusModsStyle::ModRowPadding = FMargin(0.0f, 2.0f);
const FMargin FNexusModsStyle::DividerPadding = FMargin(0.0f, 4.0f, 0.0f, 2.0f);
const FMargin FNexusModsStyle::TextBoxPadding = FMargin(8.0f, 6.0f);
const float FNexusModsStyle::DescriptionTextBoxHeight = 100.0f;


void FNexusModsStyle::Initialize() {
	if (!StyleInstance.IsValid()) {
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FNexusModsStyle::Shutdown() {
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FNexusModsStyle::GetStyleSetName() {
	static FName StyleSetName(TEXT("NexusModsStyle"));
	return StyleSetName;
}


const FMargin Centered9Slice(0.5f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FNexusModsStyle::Create() {
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("NexusModsStyle"));
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NexusModsAuthorTools"));
	if (Plugin.IsValid()) {
		Style->SetContentRoot(Plugin->GetBaseDir() / TEXT("Resources"));
	}
	else {
		Style->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("NexusModsAuthorTools/Resources"));
	}
	/** Main Plugin Image: **/
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
	Style->Set("NexusMods.PluginAction", new IMAGE_BRUSH_SVG(TEXT("InterfaceIcons/NexusModsIcon"), Icon20x20));
	Style->Set("NexusMods.PluginAction.TitleBar", new IMAGE_BRUSH_SVG(TEXT("InterfaceIcons/NexusModsIcon"), Icon20x20));
#else
	// UE4 cannot reliably render the SVG toolbar icon, so use the plugin PNG for the normal command icon and titlebar.
	// The legacy UE4 level editor toolbar receives a separate larger brush via AddToolbarExtension below.
	Style->Set("NexusMods.PluginAction", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
	Style->Set("NexusMods.PluginAction.TitleBar", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
	Style->Set("NexusMods.PluginAction.ToolbarUE4", new IMAGE_BRUSH(TEXT("Icon128"), Icon40x40));
	Style->Set("NexusMods.PluginAction.ToolbarUE4.Small", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
#endif
	/** Border/Fill Images: (Buttons/Textboxes/Checkboxes/etc) **/
	Style->Set("NexusMods.BaseBorder", new BOX_BRUSH(TEXT("StyleAssets/BaseBorder"), Centered9Slice));
	Style->Set("NexusMods.BaseFill", new BOX_BRUSH(TEXT("StyleAssets/BaseFill"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderL", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderL"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderM", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderM"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderR", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderR"), Centered9Slice));
	Style->Set("NexusMods.GroupFillL", new BOX_BRUSH(TEXT("StyleAssets/GroupFillL"), Centered9Slice));
	Style->Set("NexusMods.GroupFillM", new BOX_BRUSH(TEXT("StyleAssets/GroupFillM"), Centered9Slice));
	Style->Set("NexusMods.GroupFillR", new BOX_BRUSH(TEXT("StyleAssets/GroupFillR"), Centered9Slice));
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
	Style->Set("NexusMods.WindowBackground", new FSlateBrush(*NexusModsUECompatibility::GetEditorBrush("Brushes.Panel")));
	Style->Set("NexusMods.SectionHeaderBackground", new FSlateBrush(*NexusModsUECompatibility::GetEditorBrush("DetailsView.CategoryTop")));
	Style->Set("NexusMods.TitleBarBackground", new FSlateBrush(*NexusModsUECompatibility::GetEditorBrush("Brushes.Recessed")));
	Style->Set("NexusMods.TemplateCardBackground", new FSlateBrush(*NexusModsUECompatibility::GetEditorBrush("Brushes.Panel")));
#else
	Style->Set("NexusMods.WindowBackground", new FSlateColorBrush(WindowBackgroundColor));
	Style->Set("NexusMods.SectionHeaderBackground", new FSlateBrush(*NexusModsUECompatibility::GetEditorBrush("DetailsView.CategoryTop")));
	Style->Set("NexusMods.TitleBarBackground", new FSlateColorBrush(SectionHeaderBackgroundColor));
	Style->Set("NexusMods.TemplateCardBackground", new FSlateColorBrush(FLinearColor(0.025f, 0.025f, 0.025f, 1.0f)));
#endif
	/** Icon Images: **/
	Style->Set("NexusMods.Icon.NavigateBack", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-ArrowLeft"), Icon16x16));
	Style->Set("NexusMods.Icon.CheckboxChecked", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Check"), Icon16x16));
	Style->Set("NexusMods.Icon.ClosePanel", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Close"), Icon16x16));
	Style->Set("NexusMods.Icon.EditModDetails", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Edit"), Icon16x16));
	Style->Set("NexusMods.Icon.BrowseArchive", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Ellipsis"), Icon16x16));
	Style->Set("NexusMods.Icon.ShowSensitiveValue", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Eye"), Icon16x16));
	Style->Set("NexusMods.Icon.HideSensitiveValue", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-EyeSlash"), Icon16x16));
	Style->Set("NexusMods.Icon.BuiltInTemplate", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileCode"), Icon16x16));
	Style->Set("NexusMods.Icon.ForgetModEntry", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileExcel"), Icon16x16));
	Style->Set("NexusMods.Icon.CreateModFromTemplate", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileMedical"), Icon16x16));
	Style->Set("NexusMods.Icon.UnusedFileSuccess", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileSuccess"), Icon16x16));
	Style->Set("NexusMods.Icon.UnusedFileUpload", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileExport"), Icon16x16));
	Style->Set("NexusMods.Icon.ProjectFolderTemplate", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileArchive"), Icon16x16));
	Style->Set("NexusMods.Icon.AddModEntry", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-LayerGroup"), Icon16x16));
	Style->Set("NexusMods.Icon.UnusedAdd", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Plus"), Icon16x16));
	Style->Set("NexusMods.Icon.SaveModDetails", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Save"), Icon16x16));
	Style->Set("NexusMods.Icon.UploadModFile", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Upload"), Icon16x16));
	Style->Set("NexusMods.Icon.CreateModPage", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-UserPlus"), Icon16x16));
	/** Custom TextBox Styling: **/
	FTextBlockStyle TextBlockStyle = NexusModsUECompatibility::GetEditorStyle().GetWidgetStyle<FTextBlockStyle>("NormalText");
	TextBlockStyle.SetSelectedBackgroundColor(FNexusModsStyle::NexusOrange);
	FEditableTextBoxStyle TextBoxInnerStyle = FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
	const FSlateBrush* NoBrush = FCoreStyle::Get().GetBrush("NoBrush");
	TextBoxInnerStyle
		.SetBackgroundImageNormal(*NoBrush)
		.SetBackgroundImageHovered(*NoBrush)
		.SetBackgroundImageFocused(*NoBrush)
		.SetBackgroundImageReadOnly(*NoBrush)
#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 1)
		.SetTextStyle(TextBlockStyle)
#else
		.SetFont(TextBlockStyle.Font)
		.SetForegroundColor(TextBlockStyle.ColorAndOpacity)
#endif
		.SetPadding(FMargin(0.0f));
	Style->Set("NexusMods.TextBox.Inner", TextBoxInnerStyle);
	/** Transparent button style for raw Slate buttons that should not inherit UE4's light default button chrome. **/
	FButtonStyle TransparentButtonStyle = FButtonStyle()
		.SetNormal(*NoBrush)
		.SetHovered(*NoBrush)
		.SetPressed(*NoBrush)
		.SetDisabled(*NoBrush)
		.SetNormalPadding(FMargin(0.0f))
		.SetPressedPadding(FMargin(0.0f));
	Style->Set("NexusMods.Button.Transparent", TransparentButtonStyle);
	/** Custom Progress Bar Styling: **/
	const FSlateBrush* BaseFillBrush = Style->GetBrush("NexusMods.BaseFill");

	FSlateBrush ProgressBackgroundBrush = *BaseFillBrush;
	ProgressBackgroundBrush.TintColor = FSlateColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.f));
	ProgressBackgroundBrush.ImageSize = Icon16x16;

	FSlateBrush ProgressFillBrush = *BaseFillBrush;
	ProgressFillBrush.TintColor = FSlateColor(NexusOrange);
	ProgressFillBrush.ImageSize = Icon16x16;

	FProgressBarStyle ProgressBarStyle = FProgressBarStyle()
		.SetBackgroundImage(ProgressBackgroundBrush)
		.SetFillImage(ProgressFillBrush)
		.SetMarqueeImage(FSlateNoResource());

#if UNREAL_ENGINE_VERSION_AT_LEAST(5, 0)
	ProgressBarStyle.SetEnableFillAnimation(false);
#endif

	Style->Set("NexusMods.ProgressBar", ProgressBarStyle);
	/** Custom Scrollbar Styling: **/
	const FSlateColorBrush ScrollbarBackgroundBrush(FLinearColor::Transparent);
	const FSlateColorBrush ScrollbarNormalThumbBrush(FLinearColor(
		NexusOrange.R,
		NexusOrange.G,
		NexusOrange.B,
		0.45f));
	const FSlateColorBrush ScrollbarHoveredThumbBrush(NexusOrange);
	const FSlateColorBrush ScrollbarDraggedThumbBrush(NexusOrangeLight);

	FScrollBarStyle ScrollBarStyle = FScrollBarStyle()
		.SetHorizontalBackgroundImage(ScrollbarBackgroundBrush)
		.SetVerticalBackgroundImage(ScrollbarBackgroundBrush)
		.SetVerticalTopSlotImage(ScrollbarBackgroundBrush)
		.SetVerticalBottomSlotImage(ScrollbarBackgroundBrush)
		.SetHorizontalTopSlotImage(ScrollbarBackgroundBrush)
		.SetHorizontalBottomSlotImage(ScrollbarBackgroundBrush)
		.SetNormalThumbImage(ScrollbarNormalThumbBrush)
		.SetHoveredThumbImage(ScrollbarHoveredThumbBrush)
		.SetDraggedThumbImage(ScrollbarDraggedThumbBrush);

	Style->Set("NexusMods.ScrollBar", ScrollBarStyle);

	/** Custom Styles End: return style **/
	return Style;
}

void FNexusModsStyle::ReloadTextures() {
	if (FSlateApplication::IsInitialized()) {
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FNexusModsStyle::Get() {
	return *StyleInstance;
}
