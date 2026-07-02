#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "Services/NexusModsDataStore.h"

class SBox;
class SNexusModsCreateFromTemplateContent;
class SNexusModsMainContent;
class SWindow;

enum class ENexusModsPage : uint8 {
    Main,
    EditMod,
    Upload,
    CreateFromTemplate
};

class SNexusModsWindow : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsWindow) {}
        SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
        SLATE_EVENT(FSimpleDelegate, OnCloseRequested)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    /** Functions: **/
    void RebuildPage();
    void NavigateToPage(ENexusModsPage NewPage, FGuid EntryId = FGuid());
    void GoBack();

    TSharedRef<SWidget> MakeHeader();
    TSharedRef<SWidget> MakeMainPage();
    TSharedRef<SWidget> MakeEditModPage();
    TSharedRef<SWidget> MakeUploadPage();
    TSharedRef<SWidget> MakeCreateFromTemplatePage();

    FText GetPageTitle() const;
    EVisibility GetBackButtonVisibility() const;
    EVisibility GetCloseButtonVisibility() const;
    bool CanNavigateBackOrClose() const;

    FReply OnBackClicked();
    FReply OnCloseClicked();

    /** Variables: **/
    TWeakPtr<SWindow> ParentWindow;
    FSimpleDelegate OnCloseRequested;

    FNexusModsDataStore DataStore;

    ENexusModsPage CurrentPage = ENexusModsPage::Main;
    TArray<ENexusModsPage> PageHistory;
    FGuid ActiveEntryId;
    bool bUploadInProgress = false;

    TSharedPtr<SBox> ContentBox;
    TSharedPtr<SNexusModsMainContent> MainContent;
    TSharedPtr<SNexusModsCreateFromTemplateContent> CreateFromTemplateContent;
};
