#include "Widgets/ContentPages/SNexusModsMainContent.h"

#include "HAL/PlatformProcess.h"
#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Components/SNexusModsButton.h"
#include "Widgets/Components/SNexusModsIconToggleButton.h"
#include "Widgets/Components/SNexusModsTextBox.h"
#include "Widgets/Text/STextBlock.h"

void SNexusModsMainContent::Construct(const FArguments& InArgs) {
    DataStore = InArgs._DataStore;
    OnAddModRequested = InArgs._OnAddModRequested;
    OnUseModTemplateRequested = InArgs._OnUseModTemplateRequested;
    OnEditModRequested = InArgs._OnEditModRequested;
    OnUploadModRequested = InArgs._OnUploadModRequested;

    ChildSlot [
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [
            MakeConfigSection()
        ]
        + SVerticalBox::Slot().FillHeight(1.0f) [
            MakeModListSection()
        ]
    ];

    RebuildModList();
}

TSharedRef<SWidget> SNexusModsMainContent::MakeConfigSection() {
    FString SavedApiKey;
    if (DataStore) {
        DataStore->LoadApiKey(SavedApiKey);
        bIsApiKeyHidden = !SavedApiKey.IsEmpty();
    }

    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [
            SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                    SNew(STextBlock)
                        .Text(FText::FromString("Config"))
                        .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                ]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                    SNew(SNexusModsButton)
                        .IconBrushName("NexusMods.Icon.SaveModDetails")
                        .Text(FText::FromString("Save Config"))
                        .bTransparentBackground(false)
                        .IconPosition(ENexusModsButtonIconPosition::Right)
                        .HoverStyle(ENexusModsButtonHoverStyle::Both)
                        .ToolTipText(FText::FromString("Save plugin configuration"))
                        .OnClicked(this, &SNexusModsMainContent::OnSaveConfigClicked)
                ]
            ]
        ]
        + SVerticalBox::Slot().AutoHeight() [
            MakeFormRow(
                FText::FromString("Game Domain"),
                SAssignNew(GameDomainTextBox, SNexusModsTextBox)
                    .Text(FText::FromString(DataStore ? DataStore->GetGameDomain() : FString()))
                    .HintText(FText::FromString("e.g. gothic1remake"))
            )
        ]
        + SVerticalBox::Slot().AutoHeight() [
            MakeFormRow(
                FText::FromString("API Key"),
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.0f) [
                    SAssignNew(ApiKeyTextBox, SNexusModsTextBox)
                        .Text(FText::FromString(SavedApiKey))
                        .HintText(FText::FromString("Nexus Mods API key"))
                        .IsPassword(this, &SNexusModsMainContent::IsApiKeyHidden)
                ]
                + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::InlineControlPadding) [
                    SNew(SNexusModsIconToggleButton)
                        .IsOn(this, &SNexusModsMainContent::IsApiKeyHidden)
                        .ToolTipText(FText::FromString("Toggle API key visibility"))
                        .OnClicked(this, &SNexusModsMainContent::OnToggleProtectedVisibility)
                ]
            )
        ];
}

TSharedRef<SWidget> SNexusModsMainContent::MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget) {
    return
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(0.35f).VAlign(VAlign_Center).Padding(FNexusModsStyle::FormRowLabelPadding) [
            SNew(STextBlock).Text(Label)
        ]
        + SHorizontalBox::Slot().FillWidth(0.65f).Padding(FNexusModsStyle::FormRowValuePadding) [
            ValueWidget
        ];
}

TSharedRef<SWidget> SNexusModsMainContent::MakeModListSection() {
    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [
            SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                    SNew(STextBlock)
                        .Text(FText::FromString("Mods"))
                        .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                ]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::ButtonGroupPadding) [
                    SNew(SNexusModsButton)
                        .IconBrushName("NexusMods.Icon.CreateModPage")
                        .Text(FText::FromString("Create Mod Page"))
                        .bTransparentBackground(false)
                        .IconPosition(ENexusModsButtonIconPosition::Right)
                        .HoverStyle(ENexusModsButtonHoverStyle::Both)
                        .ToolTipText(FText::FromString("Open Nexus Mods in your browser to create a new mod page for this game."))
                        .OnClicked(this, &SNexusModsMainContent::OnCreateNewModClicked)
                        .IsEnabled(this, &SNexusModsMainContent::CanCreateMod)
                ]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::ButtonGroupPadding) [
                    SNew(SNexusModsButton)
                        .IconBrushName("NexusMods.Icon.CreateModFromTemplate")
                        .Text(FText::FromString("Use Mod Template"))
                        .bTransparentBackground(false)
                        .IconPosition(ENexusModsButtonIconPosition::Right)
                        .HoverStyle(ENexusModsButtonHoverStyle::Both)
                        .ToolTipText(FText::FromString("Create new mod files from a template."))
                        .OnClicked(this, &SNexusModsMainContent::OnUseModTemplateClicked)
                ]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                    SNew(SNexusModsButton)
                        .IconBrushName("NexusMods.Icon.AddModEntry")
                        .Text(FText::FromString("Add Mod"))
                        .bTransparentBackground(false)
                        .IconPosition(ENexusModsButtonIconPosition::Right)
                        .HoverStyle(ENexusModsButtonHoverStyle::Both)
                        .ToolTipText(FText::FromString("Add a new mod upload entry."))
                        .OnClicked(this, &SNexusModsMainContent::OnAddModClicked)
                ]
            ]
        ]
        + SVerticalBox::Slot().FillHeight(1.0f).Padding(FNexusModsStyle::ContentPadding) [
            SNew(SScrollBox)
            + SScrollBox::Slot() [
                SAssignNew(ModsListBox, SVerticalBox)
            ]
        ];
}

void SNexusModsMainContent::RebuildModList() {
    if (!ModsListBox.IsValid() || !DataStore) return;

    ModsListBox->ClearChildren();

    const TArray<FNexusModsModUploadState> Mods = DataStore->GetMods();

    if (Mods.Num() == 0) {
        ModsListBox->AddSlot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
            SNew(STextBlock)
                .Text(FText::FromString("No mods configured yet. Click Add Mod to create one."))
                .Font(FAppStyle::GetFontStyle("BoldFont"))
        ];
        return;
    }

    for (const FNexusModsModUploadState& ModState : Mods) {
        ModsListBox->AddSlot().AutoHeight().Padding(FNexusModsStyle::ModRowPadding) [
            MakeModRow(ModState)
        ];
    }
}

TSharedRef<SWidget> SNexusModsMainContent::MakeModRow(const FNexusModsModUploadState& ModState) {
    const FString RowLabel = FString::Printf(TEXT("%s"), *ModState.DisplayName);

    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight(1.0f) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center)[
                SNew(STextBlock)
                    .Text(FText::FromString(RowLabel))
                    .Font(FAppStyle::GetFontStyle("BoldFont"))
            ]
            + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::ButtonGroupPadding) [
                SNew(SNexusModsButton)
                    .IconBrushName("NexusMods.Icon.UploadModFile")
                    .bTransparentBackground(false)
                    .Text(FText::FromString("Upload"))
                    .IconPosition(ENexusModsButtonIconPosition::Right)
                    .HoverStyle(ENexusModsButtonHoverStyle::Both)
                    .ToolTipText(FText::FromString("Upload this mod"))
                    .OnClicked(this, &SNexusModsMainContent::OnUploadModClicked, ModState.EntryId)
            ]
            + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::ButtonGroupPadding)[
                SNew(SNexusModsButton)
                    .IconBrushName("NexusMods.Icon.EditModDetails")
                    .bTransparentBackground(false)
                    .Text(FText::FromString("Edit"))
                    .IconPosition(ENexusModsButtonIconPosition::Right)
                    .HoverStyle(ENexusModsButtonHoverStyle::Both)
                    .ToolTipText(FText::FromString("Edit this mod"))
                    .OnClicked(this, &SNexusModsMainContent::OnEditModClicked, ModState.EntryId)
            ]
            + SHorizontalBox::Slot().AutoWidth() [
                SNew(SNexusModsButton)
                    .IconBrushName("NexusMods.Icon.ForgetModEntry")
                    .bTransparentBackground(false)
                    .Text(FText::FromString("Delete"))
                    .IconPosition(ENexusModsButtonIconPosition::Right)
                    .HoverStyle(ENexusModsButtonHoverStyle::Both)
                    .ToolTipText(FText::FromString("Forget this mod"))
                    .OnClicked(this, &SNexusModsMainContent::OnForgetModClicked, ModState.EntryId)
            ]
        ]
        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Fill).Padding(FNexusModsStyle::DividerPadding) [
            AddNexusSpacer(0.1f)
        ];
}

TSharedRef<SWidget> SNexusModsMainContent::AddNexusSpacer(float Opacity) {
    return
        SNew(SImage)
        .Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .ColorAndOpacity(FNexusModsStyle::NexusOrange)
        .DesiredSizeOverride(FVector2D(0, 1))
        .RenderOpacity(Opacity);
}

FReply SNexusModsMainContent::OnSaveConfigClicked() {
    if (!DataStore) return FReply::Handled();

    PersistCurrentGameDomain();

    if (ApiKeyTextBox.IsValid()) {
        const FString ApiKey = ApiKeyTextBox->GetText().ToString();
        DataStore->SaveApiKey(ApiKey);
    }

    DataStore->Save();

    return FReply::Handled();
}

FReply SNexusModsMainContent::OnCreateNewModClicked() {
    FString GameDomain = GetGameDomainText();
    GameDomain.TrimStartAndEndInline();

    if (GameDomain.IsEmpty()) {
        return FReply::Handled();
    }

    const FString Url = FString::Printf(
        TEXT("https://www.nexusmods.com/games/%s?uploadMod=true"),
        *GameDomain
    );

    FPlatformProcess::LaunchURL(*Url, nullptr, nullptr);

    return FReply::Handled();
}

FReply SNexusModsMainContent::OnAddModClicked() {
    PersistCurrentGameDomain();
    OnAddModRequested.ExecuteIfBound();
    return FReply::Handled();
}

FReply SNexusModsMainContent::OnUseModTemplateClicked() {
    PersistCurrentGameDomain();
    OnUseModTemplateRequested.ExecuteIfBound();
    return FReply::Handled();
}

FReply SNexusModsMainContent::OnEditModClicked(FGuid EntryId) {
    PersistCurrentGameDomain();
    OnEditModRequested.ExecuteIfBound(EntryId);
    return FReply::Handled();
}

FReply SNexusModsMainContent::OnUploadModClicked(FGuid EntryId) {
    PersistCurrentGameDomain();
    OnUploadModRequested.ExecuteIfBound(EntryId);
    return FReply::Handled();
}

FReply SNexusModsMainContent::OnForgetModClicked(FGuid EntryId) {
    if (!DataStore) return FReply::Handled();

    DataStore->RemoveModState(EntryId);
    DataStore->Save();
    RebuildModList();

    return FReply::Handled();
}

bool SNexusModsMainContent::CanCreateMod() const {
    FString GameDomain = GetGameDomainText();
    GameDomain.TrimStartAndEndInline();

    return !GameDomain.IsEmpty();
}

FString SNexusModsMainContent::GetGameDomainText() const {
    if (GameDomainTextBox.IsValid()) {
        return GameDomainTextBox->GetText().ToString();
    }

    return DataStore ? DataStore->GetGameDomain() : FString();
}

void SNexusModsMainContent::PersistCurrentGameDomain() {
    if (!DataStore) return;

    DataStore->SetGameDomain(GetGameDomainText());
    DataStore->Save();
}

FReply SNexusModsMainContent::OnToggleProtectedVisibility() {
    bIsApiKeyHidden = !bIsApiKeyHidden;

    if (ApiKeyTextBox.IsValid()) {
        ApiKeyTextBox->RefreshDisplayText();
    }

    return FReply::Handled();
}

bool SNexusModsMainContent::IsApiKeyHidden() const {
    return bIsApiKeyHidden;
}
