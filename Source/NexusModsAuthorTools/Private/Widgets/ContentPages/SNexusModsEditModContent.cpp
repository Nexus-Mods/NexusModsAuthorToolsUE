#include "Widgets/ContentPages/SNexusModsEditModContent.h"

#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"
#include "IDesktopPlatform.h"
#include "Misc/Paths.h"
#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Components/SNexusModsButton.h"
#include "Widgets/Components/SNexusModsIconToggleButton.h"
#include "Widgets/Components/SNexusModsTextBox.h"
#include "Widgets/Text/STextBlock.h"

namespace {
    bool IsSupportedLooseModFile(const FString& FilePath) {
        const FString Extension = FPaths::GetExtension(FilePath, false).ToLower();
        return Extension == TEXT("pak") || Extension == TEXT("utoc") || Extension == TEXT("ucas");
    }

    bool IsSupportedArchiveFile(const FString& FilePath) {
        return FPaths::GetExtension(FilePath, false).Equals(TEXT("zip"), ESearchCase::IgnoreCase);
    }
}

void SNexusModsEditModContent::Construct(const FArguments& InArgs) {
    EditingEntryId = InArgs._EntryId;
    GameDomain = InArgs._GameDomain;
    OnCloseRequested = InArgs._OnCloseRequested;
    OnSaved = InArgs._OnSaved;

    DataStore.Load();
    DataStore.SetGameDomain(GameDomain);

    FNexusModsModUploadState State;
    FString SavedFileId;

    if (EditingEntryId.IsValid()) {
        DataStore.GetModState(EditingEntryId, State);
        DataStore.LoadFileId(EditingEntryId, SavedFileId);
    }

    bIsUploadCodeHidden = EditingEntryId.IsValid();
    ArchivePath = State.ArchivePath;
    ModFilePaths = State.ModFilePaths;
    RefreshModFilesWarning();

    ChildSlot [
        SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight() [
                SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                        SNew(STextBlock)
                            .Text(FText::FromString("Mod Settings"))
                            .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                        SNew(SNexusModsButton)
                            .IconBrushName("NexusMods.Icon.SaveModDetails")
                            .Text(FText::FromString("Save Mod"))
                            .bTransparentBackground(false)
                            .IconPosition(ENexusModsButtonIconPosition::Right)
                            .HoverStyle(ENexusModsButtonHoverStyle::Both)
                            .ToolTipText(FText::FromString("Save mod settings"))
                            .OnClicked(this, &SNexusModsEditModContent::OnSaveClicked)
                    ]
                ]
            ]
            + SVerticalBox::Slot().AutoHeight() [
                MakeFormRow(
                    FText::FromString("Display Name"),
                    SAssignNew(DisplayNameTextBox, SNexusModsTextBox)
                    .Text(FText::FromString(State.DisplayName))
                    .HintText(FText::FromString("My Mod"))
                )
            ]
            + SVerticalBox::Slot().AutoHeight() [
                MakeFormRow(
                    FText::FromString("Upload Code"),
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.0f) [
                        SAssignNew(FileIdTextBox, SNexusModsTextBox)
                            .Text(FText::FromString(SavedFileId))
                            .HintText(FText::FromString("File ID / upload code"))
                            .IsPassword(this, &SNexusModsEditModContent::IsUploadCodeHidden)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::InlineControlPadding) [
                        SNew(SNexusModsIconToggleButton)
                            .IsOn(this, &SNexusModsEditModContent::IsUploadCodeHidden)
                            .ToolTipText(FText::FromString("Toggle upload code visibility"))
                            .OnClicked(this, &SNexusModsEditModContent::OnToggleUploadCodeVisibility)
                    ]
                )
            ]
            + SVerticalBox::Slot().FillHeight(1.0f).Padding(FNexusModsStyle::FormRowValuePadding) [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight() [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                        SNew(STextBlock)
                            .Text(FText::FromString("Mod Files"))
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::InlineControlPadding) [
                        SNew(SNexusModsButton)
                            .Text(FText::FromString("Add Files"))
                            .IconBrushName("NexusMods.Icon.BrowseArchive")
                            .bTransparentBackground(false)
                            .HoverStyle(ENexusModsButtonHoverStyle::Both)
                            .ToolTipText(FText::FromString("Select a zip archive, or select loose .pak, .utoc and .ucas files to package before upload"))
                            .OnClicked(this, &SNexusModsEditModContent::OnAddModFilesClicked)
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::InlineControlPadding) [
                        SNew(SNexusModsButton)
                            .Text(FText::FromString("Clear Files"))
                            .IconBrushName("NexusMods.Icon.ForgetModEntry")
                            .bTransparentBackground(false)
                            .HoverStyle(ENexusModsButtonHoverStyle::Both)
                            .ToolTipText(FText::FromString("Clear the selected mod files"))
                            .OnClicked(this, &SNexusModsEditModContent::OnClearModFilesClicked)
                    ]
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::DividerPadding) [
                    SAssignNew(ModFilesWarningTextBlock, STextBlock)
                        .Text(this, &SNexusModsEditModContent::GetModFilesWarningText)
                        .ColorAndOpacity(FNexusModsStyle::NexusOrange)
                        .Visibility(this, &SNexusModsEditModContent::GetModFilesWarningVisibility)
                ]
                + SVerticalBox::Slot().FillHeight(1.0f).Padding(FNexusModsStyle::DividerPadding) [
                    SAssignNew(ModFilesTextBox, SNexusModsTextBox)
                        .Text(FText::FromString(TEXT("")))
                        .HintText(FText::FromString("Select a .zip archive, or select loose .pak/.utoc/.ucas files to zip before upload"))
                        .IsReadOnly(true)
                        .AllowMultiLine(true)
                        .AutoWrapText(true)
                        .Padding(FNexusModsStyle::TextBoxPadding)
                ]
            ]
    ];

    RefreshModFilesText();
}

TSharedRef<SWidget> SNexusModsEditModContent::MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget) {
    return
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(0.35f).VAlign(VAlign_Center).Padding(FNexusModsStyle::FormRowLabelPadding) [
            SNew(STextBlock).Text(Label)
        ]
        + SHorizontalBox::Slot().FillWidth(0.65f).Padding(FNexusModsStyle::FormRowValuePadding) [
            ValueWidget
        ];
}

FReply SNexusModsEditModContent::OnSaveClicked() {
    FNexusModsModUploadState State;

    if (EditingEntryId.IsValid()) {
        DataStore.GetModState(EditingEntryId, State);
    }
    else {
        State.EntryId = FGuid::NewGuid();
        State.Version = TEXT("1.0.0");
        State.Category = TEXT("main");
        State.bAllowModManagerDownload = true;
    }

    State.DisplayName = DisplayNameTextBox.IsValid()
        ? DisplayNameTextBox->GetText().ToString()
        : FString();

    State.ArchivePath = ArchivePath;
    State.ModFilePaths = ModFilePaths;

    if (State.Category.IsEmpty()) {
        State.Category = TEXT("main");
    }

    DataStore.SetGameDomain(GameDomain);
    DataStore.SetModState(State);
    DataStore.Save();

    const FString FileId = FileIdTextBox.IsValid()
        ? FileIdTextBox->GetText().ToString()
        : FString();

    if (!FileId.IsEmpty()) {
        DataStore.SaveFileId(State.EntryId, FileId);
    }

    OnSaved.ExecuteIfBound();
    //OnCloseRequested.ExecuteIfBound();

    return FReply::Handled();
}

FReply SNexusModsEditModContent::OnAddModFilesClicked() {
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform) return FReply::Handled();

    TArray<FString> SelectedFiles;

    const void* ParentWindowHandle = nullptr;

    TSharedPtr<SWindow> ParentWindow =
        FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);

    if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) {
        ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
    }

    const bool bSelected = DesktopPlatform->OpenFileDialog(
        ParentWindowHandle,
        TEXT("Select Mod Files"),
        FPaths::ProjectDir(),
        TEXT(""),
        TEXT("Supported Mod Files (*.zip;*.pak;*.utoc;*.ucas)|*.zip;*.pak;*.utoc;*.ucas|Zip Archives (*.zip)|*.zip|Unreal Mod Files (*.pak;*.utoc;*.ucas)|*.pak;*.utoc;*.ucas|All Files (*.*)|*.*"),
        EFileDialogFlags::Multiple,
        SelectedFiles
    );

    if (bSelected && SelectedFiles.Num() > 0) {
        for (const FString& SelectedFile : SelectedFiles) {
            if (IsSupportedArchiveFile(SelectedFile)) {
                ArchivePath = SelectedFile;
                continue;
            }

            if (IsSupportedLooseModFile(SelectedFile) && !ModFilePaths.Contains(SelectedFile)) {
                ModFilePaths.Add(SelectedFile);
            }
        }

        RefreshModFilesWarning();
        RefreshModFilesText();
    }

    return FReply::Handled();
}

FReply SNexusModsEditModContent::OnClearModFilesClicked() {
    ArchivePath.Empty();
    ModFilePaths.Empty();
    RefreshModFilesWarning();
    RefreshModFilesText();
    return FReply::Handled();
}

void SNexusModsEditModContent::RefreshModFilesText() {
    if (!ModFilesTextBox.IsValid()) {
        return;
    }

    TArray<FString> DisplayLines;

    if (!ArchivePath.IsEmpty()) {
        DisplayLines.Add(FString::Printf(TEXT("Archive: %s"), *ArchivePath));
    }

    for (const FString& ModFilePath : ModFilePaths) {
        DisplayLines.Add(ModFilePath);
    }

    ModFilesTextBox->SetText(FText::FromString(FString::Join(DisplayLines, LINE_TERMINATOR)));
}

void SNexusModsEditModContent::RefreshModFilesWarning() {
    bHasModFileSelectionWarning = !ArchivePath.IsEmpty() && ModFilePaths.Num() > 0;
}

EVisibility SNexusModsEditModContent::GetModFilesWarningVisibility() const {
    return bHasModFileSelectionWarning ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SNexusModsEditModContent::GetModFilesWarningText() const {
    return FText::FromString(TEXT("Warning: select either one pre-archived zip or loose mod files, not both. The upload step will not continue until one selection type is cleared."));
}

FReply SNexusModsEditModContent::OnToggleUploadCodeVisibility() {
    bIsUploadCodeHidden = !bIsUploadCodeHidden;

    if (FileIdTextBox.IsValid()) {
        FileIdTextBox->RefreshDisplayText();
    }

    return FReply::Handled();
}

bool SNexusModsEditModContent::IsUploadCodeHidden() const {
    return bIsUploadCodeHidden;
}
