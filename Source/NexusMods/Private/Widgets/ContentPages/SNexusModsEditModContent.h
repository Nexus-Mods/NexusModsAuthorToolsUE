#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsDataStore.h"
#include "Widgets/SCompoundWidget.h"

class SNexusModsTextBox;

class SNexusModsEditModContent : public SCompoundWidget
{
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
    FReply OnBrowseArchiveClicked();
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
    TSharedPtr<SNexusModsTextBox> ArchivePathTextBox;

    bool bIsUploadCodeHidden = true;
};
