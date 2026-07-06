#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsDataStore.h"
#include "Widgets/SCompoundWidget.h"

class SNexusModsTextBox;
class STextBlock;

class SNexusModsEditModContent : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsEditModContent) {}
        SLATE_ARGUMENT(FGuid, EntryId)
        SLATE_ARGUMENT(FString, GameDomain)
        SLATE_EVENT(FSimpleDelegate, OnCloseRequested)
        SLATE_EVENT(FSimpleDelegate, OnSaved)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    /** Functions: **/
    TSharedRef<SWidget> MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget);

    FReply OnSaveClicked();
    FReply OnAddModFilesClicked();
    FReply OnClearModFilesClicked();
    void RefreshModFilesText();
    void RefreshModFilesWarning();
    EVisibility GetModFilesWarningVisibility() const;
    FText GetModFilesWarningText() const;
    FReply OnToggleUploadCodeVisibility();
    bool IsUploadCodeHidden() const;

    /** Variables: **/
    FGuid EditingEntryId;
    FString GameDomain;

    FSimpleDelegate OnCloseRequested;
    FSimpleDelegate OnSaved;

    FNexusModsDataStore DataStore;

    TSharedPtr<SNexusModsTextBox> DisplayNameTextBox;
    TSharedPtr<SNexusModsTextBox> FileIdTextBox;
    TSharedPtr<SNexusModsTextBox> ModFilesTextBox;
    TSharedPtr<STextBlock> ModFilesWarningTextBlock;
    FString ArchivePath;
    TArray<FString> ModFilePaths;
    bool bHasModFileSelectionWarning = false;

    bool bIsUploadCodeHidden = true;
};
