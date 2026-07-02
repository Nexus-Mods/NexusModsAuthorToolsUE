#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsDataStore.h"
#include "Widgets/SCompoundWidget.h"

class SNexusModsTextBox;
class SVerticalBox;

DECLARE_DELEGATE_OneParam(FNexusModsModActionDelegate, FGuid);

/**
 * Main page content for the Nexus Mods author tool.
 *
 * This widget owns the Config and Mods sections. The parent window owns page
 * navigation and decides which content widget is currently displayed.
 */
class SNexusModsMainContent : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsMainContent) {}
        SLATE_ARGUMENT(FNexusModsDataStore*, DataStore)
        SLATE_EVENT(FSimpleDelegate, OnAddModRequested)
        SLATE_EVENT(FSimpleDelegate, OnUseModTemplateRequested)
        SLATE_EVENT(FNexusModsModActionDelegate, OnEditModRequested)
        SLATE_EVENT(FNexusModsModActionDelegate, OnUploadModRequested)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void RebuildModList();

private:
    /** Functions: **/
    TSharedRef<SWidget> MakeConfigSection();
    TSharedRef<SWidget> MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget);
    TSharedRef<SWidget> MakeModListSection();
    TSharedRef<SWidget> MakeModRow(const FNexusModsModUploadState& ModState);
    TSharedRef<SWidget> AddNexusSpacer(float Opacity = 0.1f);

    bool CanCreateMod() const;

    FReply OnSaveConfigClicked();
    FReply OnCreateNewModClicked();
    FReply OnAddModClicked();
    FReply OnUseModTemplateClicked();
    FReply OnEditModClicked(FGuid EntryId);
    FReply OnUploadModClicked(FGuid EntryId);
    FReply OnForgetModClicked(FGuid EntryId);
    FReply OnToggleProtectedVisibility();
    bool IsApiKeyHidden() const;


    FString GetGameDomainText() const;
    void PersistCurrentGameDomain();

    /** Variables: **/
    FNexusModsDataStore* DataStore = nullptr;

    FSimpleDelegate OnAddModRequested;
    FSimpleDelegate OnUseModTemplateRequested;
    FNexusModsModActionDelegate OnEditModRequested;
    FNexusModsModActionDelegate OnUploadModRequested;

    TSharedPtr<SNexusModsTextBox> GameDomainTextBox;
    TSharedPtr<SNexusModsTextBox> ApiKeyTextBox;
    TSharedPtr<SVerticalBox> ModsListBox;

    bool bIsApiKeyHidden = true;
};
