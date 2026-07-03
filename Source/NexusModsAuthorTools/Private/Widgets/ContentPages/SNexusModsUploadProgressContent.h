#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsApiClient.h"
#include "Services/NexusModsDataStore.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SCheckBox.h"

class SNexusModsCheckbox;
class SNexusModsOptionSelector;
class SNexusModsTextBox;
class SProgressBar;
class SVerticalBox;

class SNexusModsUploadProgressContent : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsUploadProgressContent) {}
        SLATE_ARGUMENT(FString, GameDomain)
        SLATE_ARGUMENT(FString, ApiKey)
        SLATE_ARGUMENT(FString, FileId)
        SLATE_ARGUMENT(FNexusModsModUploadState, ModState)
        SLATE_EVENT(FSimpleDelegate, OnCloseRequested)
        SLATE_EVENT(FSimpleDelegate, OnUploadStarted)
        SLATE_EVENT(FSimpleDelegate, OnUploadFinished)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void AddLogLine(const FString& Line);

private:
    /** Functions: **/
    TSharedRef<SWidget> MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget);
    TSharedRef<SWidget> MakeDescriptionSection();
    TSharedRef<SWidget> MakeLogSection();
    TSharedRef<SWidget> MakeProgressSection();

    FReply OnStartUploadClicked();
    FString GetUploadVersion() const;
    FString GetSelectedCategory() const;
    int32 GetCategoryOptionId(const FString& Category) const;
    FString GetCategoryFromOptionId(int32 OptionId) const;
    FString IncrementPatchVersion(const FString& Version) const;
    bool IsCheckBoxChecked(const TSharedPtr<SNexusModsCheckbox>& CheckBox) const;
    EVisibility GetLogVisibility() const;
    TOptional<float> GetUploadProgressPercent() const;
    FText GetUploadProgressText() const;
    void HandleUploadProgress(int64 BytesSent, int64 TotalBytes);
    void FinishUpload();
    void PersistCurrentUploadDefaults();
    void OnUploadOptionChanged(ECheckBoxState NewState);
    void OnCategoryOptionSelected(int32 SelectedOption);
    void PersistUploadDefaults(const FNexusModsUploadRequest& Request);

    /** Variables: **/
    FSimpleDelegate OnCloseRequested;
    FSimpleDelegate OnUploadStarted;
    FSimpleDelegate OnUploadFinished;

    FString GameDomain;
    FString ApiKey;
    FString FileId;
    FNexusModsModUploadState ModState;

    FNexusModsDataStore DataStore;
    TUniquePtr<FNexusModsApiClient> ApiClient;

    TSharedPtr<SVerticalBox> OptionsPanel;
    TSharedPtr<SNexusModsTextBox> LogTextBox;
    TSharedPtr<SProgressBar> UploadProgressBar;

    TSharedPtr<SNexusModsTextBox> VersionTextBox;
    TSharedPtr<SNexusModsTextBox> DescriptionTextBox;
    TSharedPtr<SNexusModsOptionSelector> CategorySelector;

    TSharedPtr<SNexusModsCheckbox> AutoIncrementVersionCheckBox;
    TSharedPtr<SNexusModsCheckbox> ArchiveExistingVersionCheckBox;
    TSharedPtr<SNexusModsCheckbox> PrimaryModManagerDownloadCheckBox;
    TSharedPtr<SNexusModsCheckbox> AllowModManagerDownloadCheckBox;
    TSharedPtr<SNexusModsCheckbox> ShowRequirementsPopUpCheckBox;

    FString LogText;
    int64 UploadBytesSent = 0;
    int64 UploadTotalBytes = 0;
    bool bUploadInProgress = false;
    bool bLogVisible = false;
};
