// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusModsStyle.h"
#include "NexusMods.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FNexusModsStyle::StyleInstance = nullptr;

const FLinearColor FNexusModsStyle::NexusOrange = FLinearColor(FColor::FromHex(TEXT("#e6832b")));
const FLinearColor FNexusModsStyle::NexusOrangeLight = FLinearColor(FColor::FromHex(TEXT("#faa431")));

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

TSharedRef< FSlateStyleSet > FNexusModsStyle::Create() {
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("NexusModsStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("NexusMods")->GetBaseDir() / TEXT("Resources"));
	/** Main Plugin Image: **/
	Style->Set("NexusMods.PluginAction", new IMAGE_BRUSH_SVG(TEXT("InterfaceIcons/NexusModsIcon"), Icon20x20));
	/** Border/Fill Images: (Buttons/Textboxes/Checkboxes/etc) **/
	Style->Set("NexusMods.BaseBorder", new BOX_BRUSH(TEXT("StyleAssets/BaseBorder"), Centered9Slice));
	Style->Set("NexusMods.BaseFill", new BOX_BRUSH(TEXT("StyleAssets/BaseFill"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderL", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderL"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderM", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderM"), Centered9Slice));
	Style->Set("NexusMods.GroupBorderR", new BOX_BRUSH(TEXT("StyleAssets/GroupBorderR"), Centered9Slice));
	Style->Set("NexusMods.GroupFillL", new BOX_BRUSH(TEXT("StyleAssets/GroupFillL"), Centered9Slice));
	Style->Set("NexusMods.GroupFillM", new BOX_BRUSH(TEXT("StyleAssets/GroupFillM"), Centered9Slice));
	Style->Set("NexusMods.GroupFillR", new BOX_BRUSH(TEXT("StyleAssets/GroupFillR"), Centered9Slice));
	/** Icon Images: **/
	Style->Set("NexusMods.BackIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Back"), Icon16x16));
	Style->Set("NexusMods.CheckIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Check"), Icon16x16));
	Style->Set("NexusMods.CloseIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Close"), Icon16x16));
	Style->Set("NexusMods.EditIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Edit"), Icon16x16));
	Style->Set("NexusMods.FileCodeIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileCode"), Icon16x16));
	Style->Set("NexusMods.FileErrorIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileError"), Icon16x16));
	Style->Set("NexusMods.FilePlusIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FilePlus"), Icon16x16));
	Style->Set("NexusMods.FileSuccessIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileSuccess"), Icon16x16));
	Style->Set("NexusMods.FileUploadIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileUpload"), Icon16x16));
	Style->Set("NexusMods.FileUserIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-FileUser"), Icon16x16));
	Style->Set("NexusMods.LayerPlusIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-LayerPlus"), Icon16x16));
	Style->Set("NexusMods.PlusIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Plus"), Icon16x16));
	Style->Set("NexusMods.SaveIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Save"), Icon16x16));
	Style->Set("NexusMods.UploadIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Upload"), Icon16x16));
	Style->Set("NexusMods.UserPlusIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-UserPlus"), Icon16x16));
	Style->Set("NexusMods.EyeIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Eye"), Icon16x16));
	Style->Set("NexusMods.EyeSlashIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-EyeSlash"), Icon16x16));
	Style->Set("NexusMods.EllipsisIcon", new IMAGE_BRUSH(TEXT("InterfaceIcons/FA-Ellipsis"), Icon16x16));
	/** Custom TextBox Styling: **/
	FTextBlockStyle TextBlockStyle = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	TextBlockStyle.SetSelectedBackgroundColor(FNexusModsStyle::NexusOrange);
	FEditableTextBoxStyle TextBoxInnerStyle = FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
	const FSlateBrush* NoBrush = FCoreStyle::Get().GetBrush("NoBrush");
	TextBoxInnerStyle
		.SetBackgroundImageNormal(*NoBrush)
		.SetBackgroundImageHovered(*NoBrush)
		.SetBackgroundImageFocused(*NoBrush)
		.SetBackgroundImageReadOnly(*NoBrush)
		.SetTextStyle(TextBlockStyle)
		.SetPadding(FMargin(0.0f));
	Style->Set("NexusMods.TextBox.Inner", TextBoxInnerStyle);
	/** Custom Progress BarStyling: **/
	const FSlateBrush* BaseFillBrush = Style->GetBrush("NexusMods.BaseFill");

	FSlateBrush ProgressBackgroundBrush = *BaseFillBrush;
	ProgressBackgroundBrush.TintColor = FSlateColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.f));
	ProgressBackgroundBrush.ImageSize = FVector2D(16.f, 16.f);

	FSlateBrush ProgressFillBrush = *BaseFillBrush;
	ProgressFillBrush.TintColor = FSlateColor(NexusOrange);
	ProgressFillBrush.ImageSize = FVector2D(16.f, 16.f);

	FProgressBarStyle ProgressBarStyle = FProgressBarStyle()
		.SetBackgroundImage(ProgressBackgroundBrush)
		.SetFillImage(ProgressFillBrush)
		.SetMarqueeImage(FSlateNoResource())
		.SetEnableFillAnimation(false);

	Style->Set("NexusMods.ProgressBar", ProgressBarStyle);
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
