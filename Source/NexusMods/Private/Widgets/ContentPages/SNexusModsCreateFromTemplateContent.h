#pragma once

#include "CoreMinimal.h"
#include "Services/NexusModsTemplateService.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SCompoundWidget.h"

class SBox;
class SNexusModsTextBox;

class SNexusModsCreateFromTemplateContent : public SCompoundWidget {

public:
    SLATE_BEGIN_ARGS(SNexusModsCreateFromTemplateContent) {}
        SLATE_EVENT(FSimpleDelegate, OnCreated)
        SLATE_EVENT(FSimpleDelegate, OnExitRequested)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    bool HandleBackRequested();

private:
    /** Functions: **/
    void RefreshContent();

    TSharedRef<SWidget> MakeTemplateSelectionSection();
    TSharedRef<SWidget> MakeTemplateGrid();
    TSharedRef<SWidget> MakeTemplateCard(const FNexusModsModTemplate& Template, int32 TemplateIndex);
    TSharedRef<SWidget> MakeTemplateIconWidget(const FNexusModsModTemplate& Template);
    TSharedRef<SWidget> MakeTemplateDetailsSection();
    TSharedRef<SWidget> MakeDetailsHeader();
    TSharedRef<SWidget> MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget);

    FText GetDetailsHeaderText() const;
    FText GetTemplateDescriptionText() const;
    FText GetDestinationPreviewText() const;
    FText GetDestinationToolTipText() const;
    FText GetStatusText() const;
    FSlateColor GetStatusTextColor() const;
    bool CanCreateMod() const;
    EVisibility GetCreateButtonVisibility() const;
    EVisibility GetExitButtonVisibility() const;

    FString GetModNameText() const;
    FString GetDestinationText() const;
    bool IsDestinationReadOnly() const;

    FReply OnTemplateClicked(int32 TemplateIndex);
    void OnModNameTextChanged(const FText& Text);
    void OnDestinationTextChanged(const FText& Text);
    FReply OnCreateClicked();
    FReply OnExitClicked();

    /** Variables: **/
    FNexusModsTemplateService TemplateService;
    TArray<FNexusModsModTemplate> Templates;
    int32 SelectedTemplateIndex = INDEX_NONE;

    FSimpleDelegate OnCreated;
    FSimpleDelegate OnExitRequested;

    TSharedPtr<SBox> ContentBox;
    TSharedPtr<SNexusModsTextBox> ModNameTextBox;
    TSharedPtr<SNexusModsTextBox> DestinationTextBox;

    FString StatusMessage;
    bool bWasLastCreateSuccessful = false;
    bool bHasCreatedMod = false;
    FString CreatedModName;
    FString DestinationPath;
    bool bDestinationManuallyEdited = false;
    bool bUpdatingDestinationFromModName = false;
    bool bShowingTemplateDetails = false;

    TArray<TSharedPtr<FSlateBrush>> TemplateImageBrushes;
};
