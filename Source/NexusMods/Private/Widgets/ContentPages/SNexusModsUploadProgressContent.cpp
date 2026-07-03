#include "Widgets/ContentPages/SNexusModsUploadProgressContent.h"

#include "Misc/Paths.h"
#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Components/SNexusModsButton.h"
#include "Widgets/Components/SNexusModsCheckbox.h"
#include "Widgets/Components/SNexusModsOptionSelector.h"
#include "Widgets/Components/SNexusModsTextBox.h"

namespace {
    ECheckBoxState BoolToCheckBoxState(const bool bValue) {
        return bValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
}

void SNexusModsUploadProgressContent::Construct(const FArguments& InArgs) {
    OnCloseRequested = InArgs._OnCloseRequested;
    OnUploadStarted = InArgs._OnUploadStarted;
    OnUploadFinished = InArgs._OnUploadFinished;

    GameDomain = InArgs._GameDomain;
    ApiKey = InArgs._ApiKey;
    FileId = InArgs._FileId;
    ModState = InArgs._ModState;

    if (ModState.Category.IsEmpty()) {
        ModState.Category = TEXT("main");
    }

    ChildSlot [
        SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight() [
                SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                        SNew(STextBlock)
                            .Text(FText::FromString(FString::Printf(TEXT("Upload %s"), *ModState.DisplayName)))
                            .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                    ]
                    + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center) [
                        SNew(SNexusModsButton)
                            .IconBrushName("NexusMods.Icon.UploadModFile")
                            .Text(FText::FromString("Start Upload"))
                            .bTransparentBackground(false)
                            .IconPosition(ENexusModsButtonIconPosition::Right)
                            .HoverStyle(ENexusModsButtonHoverStyle::Both)
                            .ToolTipText(FText::FromString("Start upload"))
                            .IsEnabled_Lambda([this]() {
                                return !bUploadInProgress;
                            })
                            .OnClicked(this, &SNexusModsUploadProgressContent::OnStartUploadClicked)
                    ]
                ]
            ]
            + SVerticalBox::Slot().AutoHeight() [
                SAssignNew(OptionsPanel, SVerticalBox)
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Version"),
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                            SAssignNew(VersionTextBox, SNexusModsTextBox)
                            .Text(FText::FromString(ModState.Version))
                            .HintText(FText::FromString("1.0.0"))
                        ]
                        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(FNexusModsStyle::InlineControlPadding) [
                            SAssignNew(AutoIncrementVersionCheckBox, SNexusModsCheckbox)
                            .IsChecked(BoolToCheckBoxState(ModState.bAutoIncrementVersion))
                            .ToolTipText(FText::FromString("Automatically increment the patch version before uploading."))
                            .OnCheckStateChanged(this, &SNexusModsUploadProgressContent::OnUploadOptionChanged)
                        ]
                    )
                ]
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Category"),
                        SAssignNew(CategorySelector, SNexusModsOptionSelector)
                        .Options({
                            FNexusModsOptionSelectorOption(0, FText::FromString("Main")),
                            FNexusModsOptionSelectorOption(1, FText::FromString("Optional")),
                            FNexusModsOptionSelectorOption(2, FText::FromString("Misc"))
                        })
                        .SelectedOption(GetCategoryOptionId(ModState.Category))
                        .OnOptionSelected(this, &SNexusModsUploadProgressContent::OnCategoryOptionSelected)
                    )
                ]
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Archive Existing Version"),
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth() [
                            SAssignNew(ArchiveExistingVersionCheckBox, SNexusModsCheckbox)
                            .IsChecked(BoolToCheckBoxState(ModState.bArchiveExistingVersion))
                            .ToolTipText(FText::FromString("Archive the existing file version on Nexus Mods before uploading this one."))
                            .OnCheckStateChanged(this, &SNexusModsUploadProgressContent::OnUploadOptionChanged)
                        ]
                    )
                ]
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Primary Mod Manager Download"),
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth() [
                            SAssignNew(PrimaryModManagerDownloadCheckBox, SNexusModsCheckbox)
                            .IsChecked(BoolToCheckBoxState(ModState.bPrimaryModManagerDownload))
                            .ToolTipText(FText::FromString("Mark this file as the primary mod manager download."))
                            .OnCheckStateChanged(this, &SNexusModsUploadProgressContent::OnUploadOptionChanged)
                        ]
                    )
                ]
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Allow Mod Manager Download"),
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth() [
                            SAssignNew(AllowModManagerDownloadCheckBox, SNexusModsCheckbox)
                            .IsChecked(BoolToCheckBoxState(ModState.bAllowModManagerDownload))
                            .ToolTipText(FText::FromString("Allow this file to be downloaded through mod managers."))
                            .OnCheckStateChanged(this, &SNexusModsUploadProgressContent::OnUploadOptionChanged)
                        ]
                    )
                ]
                + SVerticalBox::Slot().AutoHeight() [
                    MakeFormRow(
                        FText::FromString("Show Requirements Pop-up"),
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth() [
                            SAssignNew(ShowRequirementsPopUpCheckBox, SNexusModsCheckbox)
                            .IsChecked(BoolToCheckBoxState(ModState.bShowRequirementsPopUp))
                            .ToolTipText(FText::FromString("Show the requirements pop-up before users download this file."))
                            .OnCheckStateChanged(this, &SNexusModsUploadProgressContent::OnUploadOptionChanged)
                        ]
                    )
                ]
                + SVerticalBox::Slot().AutoHeight()[
                    MakeDescriptionSection()
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::ContentPadding) [
                MakeProgressSection()
            ]
            + SVerticalBox::Slot().FillHeight(1.0f).Padding(FNexusModsStyle::ContentPadding) [
                MakeLogSection()
            ]
    ];
}

TSharedRef<SWidget> SNexusModsUploadProgressContent::MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget) {
    return
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(0.35f).VAlign(VAlign_Center).Padding(FNexusModsStyle::FormRowLabelPadding) [
            SNew(STextBlock).Text(Label)
        ]
        + SHorizontalBox::Slot().FillWidth(0.65f).Padding(FNexusModsStyle::FormRowValuePadding) [
            ValueWidget
        ];
}

TSharedRef<SWidget> SNexusModsUploadProgressContent::MakeDescriptionSection() {
    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
            SNew(STextBlock)
            .Text(FText::FromString("Description"))
            //.Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
            SNew(SBox).HeightOverride(FNexusModsStyle::DescriptionTextBoxHeight) [
                SAssignNew(DescriptionTextBox, SNexusModsTextBox)
                .Text(FText::FromString(ModState.Description))
                .HintText(FText::FromString("Description for this uploaded file"))
                .AllowMultiLine(true)
                .AutoWrapText(true)
                .Padding(FNexusModsStyle::TextBoxPadding)
            ]
        ];
}

TSharedRef<SWidget> SNexusModsUploadProgressContent::MakeLogSection() {
    return
        SNew(SBox)
        .Visibility(this, &SNexusModsUploadProgressContent::GetLogVisibility) [
            SAssignNew(LogTextBox, SNexusModsTextBox)
                .IsReadOnly(true)
                .AllowMultiLine(true)
                .AutoWrapText(true)
                .Padding(FNexusModsStyle::TextBoxPadding)
                .Text(FText::FromString(LogText))
        ];
}

TSharedRef<SWidget> SNexusModsUploadProgressContent::MakeProgressSection() {
    return
        SNew(SBox)
        .Visibility(this, &SNexusModsUploadProgressContent::GetLogVisibility) [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
                SNew(STextBlock)
                    .Text(this, &SNexusModsUploadProgressContent::GetUploadProgressText)
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
                SNew(SBox).HeightOverride(12.0f) [
                    SAssignNew(UploadProgressBar, SProgressBar)
                        .Style(&FNexusModsStyle::Get().GetWidgetStyle<FProgressBarStyle>("NexusMods.ProgressBar"))
                        .Percent(this, &SNexusModsUploadProgressContent::GetUploadProgressPercent)
                        //.Percent(0.69f)
                        //.FillColorAndOpacity(FNexusModsStyle::NexusOrange)
                        //.FillColorAndOpacity(FLinearColor::White)
                ]
            ]
        ];
}

void SNexusModsUploadProgressContent::AddLogLine(const FString& Line) {
    if (!LogText.IsEmpty()) {
        LogText += LINE_TERMINATOR;
    }
    LogText += Line;
    if (LogTextBox.IsValid()) {
        LogTextBox->SetText(FText::FromString(LogText));
    }
}

FReply SNexusModsUploadProgressContent::OnStartUploadClicked() {
    if (bUploadInProgress) return FReply::Handled();

    bUploadInProgress = true;
    bLogVisible = true;
    UploadBytesSent = 0;
    UploadTotalBytes = 0;
    OnUploadStarted.ExecuteIfBound();

    if (OptionsPanel.IsValid()) {
        OptionsPanel->SetVisibility(EVisibility::Collapsed);
    }

    AddLogLine(TEXT("Validating upload settings..."));

    if (GameDomain.IsEmpty()) {
        AddLogLine(TEXT("Game domain is missing."));
        FinishUpload();
        return FReply::Handled();
    }
    if (ApiKey.IsEmpty()) {
        AddLogLine(TEXT("API key is missing."));
        FinishUpload();
        return FReply::Handled();
    }
    if (FileId.IsEmpty()) {
        AddLogLine(TEXT("Upload code is missing."));
        FinishUpload();
        return FReply::Handled();
    }
    if (ModState.ArchivePath.IsEmpty()) {
        AddLogLine(TEXT("Archive path is missing."));
        FinishUpload();
        return FReply::Handled();
    }
    if (!FPaths::FileExists(ModState.ArchivePath)) {
        AddLogLine(TEXT("Archive file does not exist."));
        FinishUpload();
        return FReply::Handled();
    }

    FString Version = GetUploadVersion();
    if (IsCheckBoxChecked(AutoIncrementVersionCheckBox)) {
        Version = IncrementPatchVersion(Version);

        if (VersionTextBox.IsValid()) {
            VersionTextBox->SetText(FText::FromString(Version));
        }

        AddLogLine(FString::Printf(TEXT("Auto-incremented version to %s."), *Version));
    }

    if (Version.IsEmpty()) {
        AddLogLine(TEXT("Version is missing."));
        FinishUpload();
        return FReply::Handled();
    }

    FNexusModsUploadRequest Request;
    Request.GameDomain = GameDomain;
    Request.ApiKey = ApiKey;
    Request.FileId = FileId;
    Request.Filename = FPaths::GetCleanFilename(ModState.ArchivePath);
    Request.ArchivePath = ModState.ArchivePath;
    Request.Version = Version;
    Request.DisplayName = ModState.DisplayName;
    Request.Description = DescriptionTextBox.IsValid() ? DescriptionTextBox->GetText().ToString() : FString();
    Request.Category = GetSelectedCategory();
    Request.bArchiveExistingVersion = IsCheckBoxChecked(ArchiveExistingVersionCheckBox);
    Request.bPrimaryModManagerDownload = IsCheckBoxChecked(PrimaryModManagerDownloadCheckBox);
    Request.bAllowModManagerDownload = IsCheckBoxChecked(AllowModManagerDownloadCheckBox);
    Request.bShowRequirementsPopUp = IsCheckBoxChecked(ShowRequirementsPopUpCheckBox);

    if (Request.Category.IsEmpty()) {
        Request.Category = TEXT("main");
    }

    PersistUploadDefaults(Request);

    AddLogLine(TEXT("Archive found."));

    ApiClient = MakeUnique<FNexusModsApiClient>();
    ApiClient->UploadModFile(
        Request,
        FNexusModsUploadLogDelegate::CreateLambda([this](const FString& Line) {
            AddLogLine(Line);
        }),
        FNexusModsUploadProgressDelegate::CreateLambda([this](int64 BytesSent, int64 TotalBytes) {
            HandleUploadProgress(BytesSent, TotalBytes);
        }),
        FNexusModsUploadCompleteDelegate::CreateLambda([this](bool bSuccess) {
            AddLogLine(bSuccess ? TEXT("Upload complete!") : TEXT("Upload failed!!"));
            FinishUpload();
        })
    );

    return FReply::Handled();
}

FString SNexusModsUploadProgressContent::GetUploadVersion() const {
    return VersionTextBox.IsValid() ? VersionTextBox->GetText().ToString() : FString();
}

FString SNexusModsUploadProgressContent::GetSelectedCategory() const {
    if (!CategorySelector.IsValid()) {
        return TEXT("main");
    }

    return GetCategoryFromOptionId(CategorySelector->GetSelectedOption());
}

int32 SNexusModsUploadProgressContent::GetCategoryOptionId(const FString& Category) const {
    if (Category.Equals(TEXT("optional"), ESearchCase::IgnoreCase)) {
        return 1;
    }

    if (Category.Equals(TEXT("misc"), ESearchCase::IgnoreCase)) {
        return 2;
    }

    return 0;
}

FString SNexusModsUploadProgressContent::GetCategoryFromOptionId(int32 OptionId) const {
    switch (OptionId) {
    case 1:
        return TEXT("optional");
    case 2:
        return TEXT("misc");
    case 0:
    default:
        return TEXT("main");
    }
}

FString SNexusModsUploadProgressContent::IncrementPatchVersion(const FString& Version) const {
    TArray<FString> Parts;
    Version.ParseIntoArray(Parts, TEXT("."), true);

    if (Parts.Num() == 0) {
        return TEXT("1.0.0");
    }

    int32 LastNumber = 0;
    if (!LexTryParseString(LastNumber, *Parts.Last())) {
        return Version;
    }

    Parts.Last() = FString::FromInt(LastNumber + 1);
    return FString::Join(Parts, TEXT("."));
}

bool SNexusModsUploadProgressContent::IsCheckBoxChecked(const TSharedPtr<SNexusModsCheckbox>& CheckBox) const {
    return CheckBox.IsValid() && CheckBox->IsChecked();
}

EVisibility SNexusModsUploadProgressContent::GetLogVisibility() const {
    return bLogVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SNexusModsUploadProgressContent::GetUploadProgressPercent() const {
    if (UploadTotalBytes <= 0) {
        return 0.0f;
    }

    return FMath::Clamp(static_cast<float>(UploadBytesSent) / static_cast<float>(UploadTotalBytes), 0.0f, 1.0f);
}

FText SNexusModsUploadProgressContent::GetUploadProgressText() const {
    if (UploadTotalBytes <= 0) {
        return FText::FromString(TEXT("Preparing upload..."));
    }

    const float Percent = FMath::Clamp(static_cast<float>(UploadBytesSent) / static_cast<float>(UploadTotalBytes), 0.0f, 1.0f) * 100.0f;
    return FText::FromString(FString::Printf(TEXT("Uploaded %.0f%% (%lld / %lld bytes)"), Percent, UploadBytesSent, UploadTotalBytes));
}

void SNexusModsUploadProgressContent::HandleUploadProgress(int64 BytesSent, int64 TotalBytes) {
    UploadBytesSent = BytesSent;
    UploadTotalBytes = TotalBytes;
}

void SNexusModsUploadProgressContent::FinishUpload() {
    bUploadInProgress = false;
    OnUploadFinished.ExecuteIfBound();
}

void SNexusModsUploadProgressContent::PersistCurrentUploadDefaults() {
    FNexusModsUploadRequest Request;
    Request.Version = GetUploadVersion();
    Request.Description = DescriptionTextBox.IsValid() ? DescriptionTextBox->GetText().ToString() : FString();
    Request.Category = GetSelectedCategory();
    Request.bArchiveExistingVersion = IsCheckBoxChecked(ArchiveExistingVersionCheckBox);
    Request.bPrimaryModManagerDownload = IsCheckBoxChecked(PrimaryModManagerDownloadCheckBox);
    Request.bAllowModManagerDownload = IsCheckBoxChecked(AllowModManagerDownloadCheckBox);
    Request.bShowRequirementsPopUp = IsCheckBoxChecked(ShowRequirementsPopUpCheckBox);

    if (Request.Category.IsEmpty()) {
        Request.Category = TEXT("main");
    }

    PersistUploadDefaults(Request);
}

void SNexusModsUploadProgressContent::OnUploadOptionChanged(ECheckBoxState NewState) {
    PersistCurrentUploadDefaults();
}

void SNexusModsUploadProgressContent::OnCategoryOptionSelected(int32 SelectedOption) {
    PersistCurrentUploadDefaults();
}

void SNexusModsUploadProgressContent::PersistUploadDefaults(const FNexusModsUploadRequest& Request) {
    DataStore.Load();
    DataStore.SetGameDomain(GameDomain);

    FNexusModsModUploadState UpdatedState = ModState;
    UpdatedState.Version = Request.Version;
    UpdatedState.Description = Request.Description;
    UpdatedState.Category = Request.Category;
    UpdatedState.bAutoIncrementVersion = IsCheckBoxChecked(AutoIncrementVersionCheckBox);
    UpdatedState.bArchiveExistingVersion = Request.bArchiveExistingVersion;
    UpdatedState.bPrimaryModManagerDownload = Request.bPrimaryModManagerDownload;
    UpdatedState.bAllowModManagerDownload = Request.bAllowModManagerDownload;
    UpdatedState.bShowRequirementsPopUp = Request.bShowRequirementsPopUp;

    DataStore.SetModState(UpdatedState);
    DataStore.Save();

    ModState = UpdatedState;
}
