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
                            .IconBrushName("NexusMods.SaveIcon")
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
            + SVerticalBox::Slot().AutoHeight() [
                MakeFormRow(
                    FText::FromString("Archive Path"),
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.0f) [
                        SAssignNew(ArchivePathTextBox, SNexusModsTextBox)
                            .Text(FText::FromString(State.ArchivePath))
                            .HintText(FText::FromString("Path to ZIP archive"))
                    ]
                    + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::InlineControlPadding) [
                        SNew(SNexusModsButton)
                            //.Text(FText::FromString("..."))
                            .IconBrushName("NexusMods.EllipsisIcon")
                            .bTransparentBackground(false)
                            .HoverStyle(ENexusModsButtonHoverStyle::Both)
                            .ToolTipText(FText::FromString("Browse for mod archive"))
                            .OnClicked(this, &SNexusModsEditModContent::OnBrowseArchiveClicked)
                    ]
                )
            ]
    ];
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

    State.ArchivePath = ArchivePathTextBox.IsValid()
        ? ArchivePathTextBox->GetText().ToString()
        : FString();

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

FReply SNexusModsEditModContent::OnBrowseArchiveClicked() {
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
        TEXT("Select Mod Archive"),
        FPaths::ProjectDir(),
        TEXT(""),
        TEXT("Zip Archives (*.zip)|*.zip|All Files (*.*)|*.*"),
        EFileDialogFlags::None,
        SelectedFiles
    );

    if (bSelected && SelectedFiles.Num() > 0 && ArchivePathTextBox.IsValid()) {
        ArchivePathTextBox->SetText(FText::FromString(SelectedFiles[0]));
    }

    return FReply::Handled();
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
