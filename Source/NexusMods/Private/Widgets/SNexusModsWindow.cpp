#include "Widgets/SNexusModsWindow.h"

#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/ContentPages/SNexusModsCreateFromTemplateContent.h"
#include "Widgets/ContentPages/SNexusModsEditModContent.h"
#include "Widgets/ContentPages/SNexusModsMainContent.h"
#include "Widgets/ContentPages/SNexusModsUploadProgressContent.h"
#include "Widgets/Components/SNexusModsTitleBar.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/AppStyle.h"

//#include "NexusModsStyle.h"
//FNexusModsStyle::Get().GetBrush("NexusMods.GroupBorderM")

void SNexusModsWindow::Construct(const FArguments& InArgs) {
    ParentWindow = InArgs._ParentWindow;
    OnCloseRequested = InArgs._OnCloseRequested;

    DataStore.Load();

    ChildSlot [
        SNew(SBorder).Padding(0).BorderImage(FAppStyle::GetBrush("Brushes.Panel")) [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight() [
                MakeHeader()
            ]
            + SVerticalBox::Slot().FillHeight(1.0f) [
                SAssignNew(ContentBox, SBox)
            ]
        ]
    ];

    RebuildPage();
}

void SNexusModsWindow::RebuildPage() {
    if (!ContentBox.IsValid()) return;

    MainContent.Reset();
    CreateFromTemplateContent.Reset();

    switch (CurrentPage) {
        case ENexusModsPage::Main:
            ContentBox->SetContent(MakeMainPage());
            break;
        case ENexusModsPage::EditMod:
            ContentBox->SetContent(MakeEditModPage());
            break;
        case ENexusModsPage::Upload:
            ContentBox->SetContent(MakeUploadPage());
            break;
        case ENexusModsPage::CreateFromTemplate:
            ContentBox->SetContent(MakeCreateFromTemplatePage());
            break;
    }
}

void SNexusModsWindow::NavigateToPage(ENexusModsPage NewPage, FGuid EntryId) {
    if (bUploadInProgress) return;

    PageHistory.Add(CurrentPage);
    CurrentPage = NewPage;
    ActiveEntryId = EntryId;

    RebuildPage();
}

void SNexusModsWindow::GoBack() {
    if (bUploadInProgress) return;

    DataStore.Load();

    if (PageHistory.Num() > 0) {
        CurrentPage = PageHistory.Pop();
    }
    else {
        CurrentPage = ENexusModsPage::Main;
    }

    ActiveEntryId = FGuid();
    RebuildPage();
}

TSharedRef<SWidget> SNexusModsWindow::MakeHeader() {
    return
        SNew(SNexusModsTitleBar)
            .Window(ParentWindow)
            .Title(this, &SNexusModsWindow::GetPageTitle)
            .BackButtonVisibility(this, &SNexusModsWindow::GetBackButtonVisibility)
            .CloseButtonVisibility(this, &SNexusModsWindow::GetCloseButtonVisibility)
            .ActionsEnabled(this, &SNexusModsWindow::CanNavigateBackOrClose)
            .OnBackRequested(FSimpleDelegate::CreateLambda([this]() {
                OnBackClicked();
            }))
            .OnCloseRequested(FSimpleDelegate::CreateLambda([this]() {
                OnCloseClicked();
            }));
}

TSharedRef<SWidget> SNexusModsWindow::MakeMainPage() {
    return
        SAssignNew(MainContent, SNexusModsMainContent)
            .DataStore(&DataStore)
            .OnAddModRequested(FSimpleDelegate::CreateLambda([this]() {
                NavigateToPage(ENexusModsPage::EditMod, FGuid());
            }))
            .OnUseModTemplateRequested(FSimpleDelegate::CreateLambda([this]() {
                NavigateToPage(ENexusModsPage::CreateFromTemplate, FGuid());
            }))
            .OnEditModRequested(FNexusModsModActionDelegate::CreateLambda([this](FGuid EntryId) {
                NavigateToPage(ENexusModsPage::EditMod, EntryId);
            }))
            .OnUploadModRequested(FNexusModsModActionDelegate::CreateLambda([this](FGuid EntryId) {
                NavigateToPage(ENexusModsPage::Upload, EntryId);
            }));
}

TSharedRef<SWidget> SNexusModsWindow::MakeEditModPage() {
    return
        SNew(SNexusModsEditModContent)
            .EntryId(ActiveEntryId)
            .GameDomain(DataStore.GetGameDomain())
            .OnSaved(FSimpleDelegate::CreateLambda([this]() {
                DataStore.Load();
                CurrentPage = ENexusModsPage::Main;
                PageHistory.Empty();
                ActiveEntryId = FGuid();
                RebuildPage();
            }))
            .OnCloseRequested(FSimpleDelegate::CreateLambda([this]() {
                GoBack();
            }));
}

TSharedRef<SWidget> SNexusModsWindow::MakeUploadPage() {
    DataStore.Load();

    FNexusModsModUploadState State;
    if (!DataStore.GetModState(ActiveEntryId, State)) {
        return SNew(STextBlock).Text(FText::FromString("Unable to find selected mod."));
    }

    FString FileId;
    DataStore.LoadFileId(State.EntryId, FileId);

    FString ApiKey;
    DataStore.LoadApiKey(ApiKey);

    return
        SNew(SNexusModsUploadProgressContent)
            .GameDomain(DataStore.GetGameDomain())
            .ApiKey(ApiKey)
            .FileId(FileId)
            .ModState(State)
            .OnUploadStarted(FSimpleDelegate::CreateLambda([this]() {
                bUploadInProgress = true;
            }))
            .OnUploadFinished(FSimpleDelegate::CreateLambda([this]() {
                bUploadInProgress = false;
            }))
            .OnCloseRequested(FSimpleDelegate::CreateLambda([this]() {
                if (!bUploadInProgress) {
                    DataStore.Load();
                    GoBack();
                }
            }));
}

TSharedRef<SWidget> SNexusModsWindow::MakeCreateFromTemplatePage() {
    return
        SAssignNew(CreateFromTemplateContent, SNexusModsCreateFromTemplateContent)
            .OnCreated(FSimpleDelegate::CreateLambda([this]() {
                DataStore.Load();
            }))
            .OnExitRequested(FSimpleDelegate::CreateLambda([this]() {
                OnCloseRequested.ExecuteIfBound();
            }));
}

FText SNexusModsWindow::GetPageTitle() const {
    switch (CurrentPage) {
        case ENexusModsPage::EditMod:
            return FText::FromString("Nexus Mods: Edit Mod Details");
        case ENexusModsPage::Upload:
            return FText::FromString("Nexus Mods: Upload Mod");
        case ENexusModsPage::CreateFromTemplate:
            return FText::FromString("Nexus Mods: Create From Template");
        case ENexusModsPage::Main:
        default:
            return FText::FromString("Nexus Mods: Author Tool - vALPHA");
    }
}

EVisibility SNexusModsWindow::GetBackButtonVisibility() const {
    return CurrentPage == ENexusModsPage::Main ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SNexusModsWindow::GetCloseButtonVisibility() const {
    return CurrentPage == ENexusModsPage::Main ? EVisibility::Visible : EVisibility::Collapsed;
}

bool SNexusModsWindow::CanNavigateBackOrClose() const {
    return !bUploadInProgress;
}

FReply SNexusModsWindow::OnBackClicked() {
    if (!bUploadInProgress && CurrentPage != ENexusModsPage::Main) {
        if (CurrentPage == ENexusModsPage::CreateFromTemplate && CreateFromTemplateContent.IsValid()) {
            if (CreateFromTemplateContent->HandleBackRequested()) {
                return FReply::Handled();
            }
        }

        GoBack();
    }

    return FReply::Handled();
}

FReply SNexusModsWindow::OnCloseClicked() {
    if (!bUploadInProgress) {
        OnCloseRequested.ExecuteIfBound();
    }

    return FReply::Handled();
}
