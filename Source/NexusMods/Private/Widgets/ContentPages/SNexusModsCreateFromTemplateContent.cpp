#include "Widgets/ContentPages/SNexusModsCreateFromTemplateContent.h"

#include "NexusModsStyle.h"
#include "Styling/AppStyle.h"
#include "Engine/Texture2D.h"
#include "Widgets/Components/SNexusModsButton.h"
#include "Widgets/Components/SNexusModsTextBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SNexusModsCreateFromTemplateContent::Construct(const FArguments& InArgs) {
    OnCreated = InArgs._OnCreated;
    OnExitRequested = InArgs._OnExitRequested;

    Templates = TemplateService.GetAvailableTemplates();
    SelectedTemplateIndex = INDEX_NONE;
    bShowingTemplateDetails = false;

    ChildSlot [
        SAssignNew(ContentBox, SBox)
    ];

    RefreshContent();
}

bool SNexusModsCreateFromTemplateContent::HandleBackRequested() {
    if (bShowingTemplateDetails) {
        bShowingTemplateDetails = false;
        SelectedTemplateIndex = INDEX_NONE;
        StatusMessage.Reset();
        bWasLastCreateSuccessful = false;
        bHasCreatedMod = false;
        CreatedModName.Reset();
        DestinationPath.Reset();
        bDestinationManuallyEdited = false;
        ModNameTextBox.Reset();
        DestinationTextBox.Reset();
        RefreshContent();
        return true;
    }

    return false;
}

void SNexusModsCreateFromTemplateContent::RefreshContent() {
    if (!ContentBox.IsValid()) return;

    ContentBox->SetContent(
        SNew(SScrollBox)
        + SScrollBox::Slot() [
            bShowingTemplateDetails ? MakeTemplateDetailsSection() : MakeTemplateSelectionSection()
        ]
    );
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeTemplateSelectionSection() {
    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [
            SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                SNew(STextBlock)
                    .Text(FText::FromString("Select Template"))
                    .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
            ]
        ]
        + SVerticalBox::Slot().FillHeight(1.0f).Padding(FNexusModsStyle::ContentPadding) [
            SNew(SScrollBox)
            + SScrollBox::Slot() [
                MakeTemplateGrid()
            ]
        ];
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeTemplateGrid() {
    TSharedRef<SUniformGridPanel> TemplateGrid = SNew(SUniformGridPanel).SlotPadding(FNexusModsStyle::ContentPadding);

    constexpr int32 Columns = 2;
    for (int32 TemplateIndex = 0; TemplateIndex < Templates.Num(); ++TemplateIndex) {
        TemplateGrid->AddSlot(TemplateIndex % Columns, TemplateIndex / Columns) [
            MakeTemplateCard(Templates[TemplateIndex], TemplateIndex)
        ];
    }

    return TemplateGrid;
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeTemplateCard(const FNexusModsModTemplate& Template, int32 TemplateIndex) {
    return
        SNew(SButton)
            .ButtonStyle(FAppStyle::Get(), "SimpleButton")
            .ContentPadding(0)
            .ToolTipText(Template.Description)
            .OnClicked(this, &SNexusModsCreateFromTemplateContent::OnTemplateClicked, TemplateIndex) [
                SNew(SBorder).Padding(FNexusModsStyle::ContentPadding).BorderImage(FAppStyle::GetBrush("Brushes.Panel")) [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(FNexusModsStyle::FormRowValuePadding) [
                        MakeTemplateIconWidget(Template)
                    ]
                    + SVerticalBox::Slot().AutoHeight() [
                        SNew(STextBlock)
                            .Text(Template.DisplayName)
                            .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                    ]
                    + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
                        SNew(STextBlock)
                            .Text(Template.Description)
                            .AutoWrapText(true)
                    ]
                    + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::FormRowValuePadding) [
                        SNew(STextBlock)
                            .Text(Template.IsBuiltIn() ? FText::FromString("Built-in Template") : FText::FromString("Project Template"))
                            .ColorAndOpacity(Template.IsBuiltIn() ? FSlateColor(FNexusModsStyle::NexusOrangeLight) : FSlateColor::UseForeground())
                    ]
                ]
            ];
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeTemplateIconWidget(const FNexusModsModTemplate& Template) {
    float IconSize = 32.f;
    float BannerWidth = 180.0f;
    if (!Template.TemplateImageAssetPath.IsEmpty()) {
        UObject* TemplateImageObject = StaticLoadObject(UTexture2D::StaticClass(), nullptr, *Template.TemplateImageAssetPath);
        UTexture2D* TemplateImage = Cast<UTexture2D>(TemplateImageObject);

        if (TemplateImage) {
            TSharedPtr<FSlateBrush> TemplateImageBrush = MakeShared<FSlateBrush>();
            TemplateImageBrush->SetResourceObject(TemplateImage);
            TemplateImageBrush->ImageSize = FVector2D(TemplateImage->GetSizeX(), TemplateImage->GetSizeY());
            TemplateImageBrushes.Add(TemplateImageBrush);

            return SNew(SBox).WidthOverride(BannerWidth).HeightOverride(IconSize) [
                SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::Both) [
                    SNew(SImage)
                    .Image(TemplateImageBrush.Get())
                    .ColorAndOpacity(FNexusModsStyle::NexusOrangeLight)
                ]
            ];
        }
    }

    return SNew(SBox).WidthOverride(IconSize).HeightOverride(IconSize) [
        SNew(SImage)
        .Image(FNexusModsStyle::Get().GetBrush(Template.IconBrushName))
        .ColorAndOpacity(FNexusModsStyle::NexusOrangeLight)
    ];
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeTemplateDetailsSection() {
    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight() [
            MakeDetailsHeader()
        ]
        + SVerticalBox::Slot().AutoHeight() [
            MakeFormRow(
                FText::FromString("Mod Name"),
                SAssignNew(ModNameTextBox, SNexusModsTextBox)
                    .Text(FText::FromString(CreatedModName))
                    .HintText(FText::FromString("e.g. MyAwesomeLogicMod"))
                    .ToolTipText(FText::FromString("The mod name is used to create the destination folder under /Game/Mods."))
                    .IsReadOnly(bHasCreatedMod)
                    .OnTextChanged(this, &SNexusModsCreateFromTemplateContent::OnModNameTextChanged)
            )
        ]
        + SVerticalBox::Slot().AutoHeight() [
            MakeFormRow(
                FText::FromString("Destination"),
                SAssignNew(DestinationTextBox, SNexusModsTextBox)
                    .Text(this, &SNexusModsCreateFromTemplateContent::GetDestinationPreviewText)
                    .IsReadOnly(IsDestinationReadOnly())
                    .ToolTipText(this, &SNexusModsCreateFromTemplateContent::GetDestinationToolTipText)
                    .OnTextChanged(this, &SNexusModsCreateFromTemplateContent::OnDestinationTextChanged)
            )
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::ContentPadding) [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight() [
                SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
                    SNew(STextBlock)
                        .Text(FText::FromString("Description"))
                        .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                ]
            ]
            + SVerticalBox::Slot().AutoHeight() [
                SNew(SBox)
                    .HeightOverride(FNexusModsStyle::DescriptionTextBoxHeight)
                    [
                        SNew(SNexusModsTextBox)
                            .Text(this, &SNexusModsCreateFromTemplateContent::GetTemplateDescriptionText)
                            .IsReadOnly(true)
                            .AllowMultiLine(true)
                            .AutoWrapText(true)
                    ]
            ]
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(FNexusModsStyle::ContentPadding) [
            SNew(STextBlock)
                .Text(this, &SNexusModsCreateFromTemplateContent::GetStatusText)
                .ColorAndOpacity(this, &SNexusModsCreateFromTemplateContent::GetStatusTextColor)
        ];
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeDetailsHeader() {
    return
        SNew(SBorder).Padding(FNexusModsStyle::SectionHeaderPadding).BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop")) [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center) [
                SNew(STextBlock)
                    .Text(this, &SNexusModsCreateFromTemplateContent::GetDetailsHeaderText)
                    .Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
            ]
            + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::ButtonGroupPadding) [
                SNew(SNexusModsButton)
                    .IconBrushName("NexusMods.FilePlusIcon")
                    .Text(FText::FromString("Create Mod"))
                    .bTransparentBackground(false)
                    .IconPosition(ENexusModsButtonIconPosition::Right)
                    .HoverStyle(ENexusModsButtonHoverStyle::Both)
                    .ToolTipText(FText::FromString("Create the selected mod template in your project content folder."))
                    .Visibility(this, &SNexusModsCreateFromTemplateContent::GetCreateButtonVisibility)
                    .IsEnabled(this, &SNexusModsCreateFromTemplateContent::CanCreateMod)
                    .OnClicked(this, &SNexusModsCreateFromTemplateContent::OnCreateClicked)
            ]
            + SHorizontalBox::Slot().AutoWidth().Padding(FNexusModsStyle::ButtonGroupPadding) [
                SNew(SNexusModsButton)
                    .IconBrushName("NexusMods.CloseIcon")
                    .Text(FText::FromString("Exit"))
                    .bTransparentBackground(false)
                    .IconPosition(ENexusModsButtonIconPosition::Right)
                    .HoverStyle(ENexusModsButtonHoverStyle::Both)
                    .ToolTipText(FText::FromString("Close the Nexus Mods author tool so you can start working on the created mod."))
                    .Visibility(this, &SNexusModsCreateFromTemplateContent::GetExitButtonVisibility)
                    .OnClicked(this, &SNexusModsCreateFromTemplateContent::OnExitClicked)
            ]
        ];
}

TSharedRef<SWidget> SNexusModsCreateFromTemplateContent::MakeFormRow(const FText& Label, const TSharedRef<SWidget>& ValueWidget) {
    return
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(0.35f).VAlign(VAlign_Center).Padding(FNexusModsStyle::FormRowLabelPadding) [
            SNew(STextBlock).Text(Label)
        ]
        + SHorizontalBox::Slot().FillWidth(0.65f).Padding(FNexusModsStyle::FormRowValuePadding) [
            ValueWidget
        ];
}

FText SNexusModsCreateFromTemplateContent::GetDetailsHeaderText() const {
    if (SelectedTemplateIndex != INDEX_NONE && Templates.IsValidIndex(SelectedTemplateIndex)) {
        return FText::Format(FText::FromString("Create Mod From Template: {0}"), Templates[SelectedTemplateIndex].DisplayName);
    }

    return FText::FromString("Create Mod From Template");
}

FText SNexusModsCreateFromTemplateContent::GetTemplateDescriptionText() const {
    if (SelectedTemplateIndex != INDEX_NONE && Templates.IsValidIndex(SelectedTemplateIndex)) {
        return Templates[SelectedTemplateIndex].Description;
    }

    return FText::FromString("No template selected.");
}

FText SNexusModsCreateFromTemplateContent::GetDestinationPreviewText() const {
    if (!DestinationPath.IsEmpty()) {
        return FText::FromString(DestinationPath);
    }

    return FText::FromString(TemplateService.GetDefaultDestinationPath(GetModNameText()));
}

FText SNexusModsCreateFromTemplateContent::GetDestinationToolTipText() const {
    if (SelectedTemplateIndex != INDEX_NONE && Templates.IsValidIndex(SelectedTemplateIndex) && !Templates[SelectedTemplateIndex].IsBuiltIn()) {
        return FText::FromString("The destination folder to copy the selected template into. This must be inside /Game.");
    }

    return FText::FromString("The generated ModActor blueprint will be created in this destination folder.");
}

FText SNexusModsCreateFromTemplateContent::GetStatusText() const {
    return FText::FromString(StatusMessage);
}

FSlateColor SNexusModsCreateFromTemplateContent::GetStatusTextColor() const {
    return bWasLastCreateSuccessful ? FSlateColor(FNexusModsStyle::NexusOrangeLight) : FSlateColor(FLinearColor::White);
}

bool SNexusModsCreateFromTemplateContent::CanCreateMod() const {
    return !bHasCreatedMod && SelectedTemplateIndex != INDEX_NONE && !TemplateService.SanitizeModName(GetModNameText()).IsEmpty() && !GetDestinationText().IsEmpty();
}

EVisibility SNexusModsCreateFromTemplateContent::GetCreateButtonVisibility() const {
    return bHasCreatedMod ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SNexusModsCreateFromTemplateContent::GetExitButtonVisibility() const {
    return bHasCreatedMod ? EVisibility::Visible : EVisibility::Collapsed;
}

FString SNexusModsCreateFromTemplateContent::GetModNameText() const {
    if (ModNameTextBox.IsValid()) {
        return ModNameTextBox->GetText().ToString();
    }

    return CreatedModName;
}

FString SNexusModsCreateFromTemplateContent::GetDestinationText() const {
    if (DestinationTextBox.IsValid()) {
        return DestinationTextBox->GetText().ToString();
    }

    if (!DestinationPath.IsEmpty()) {
        return DestinationPath;
    }

    return TemplateService.GetDefaultDestinationPath(GetModNameText());
}

bool SNexusModsCreateFromTemplateContent::IsDestinationReadOnly() const {
    if (bHasCreatedMod) return true;

    if (SelectedTemplateIndex != INDEX_NONE && Templates.IsValidIndex(SelectedTemplateIndex)) {
        return Templates[SelectedTemplateIndex].IsBuiltIn();
    }

    return true;
}

FReply SNexusModsCreateFromTemplateContent::OnTemplateClicked(int32 TemplateIndex) {
    if (!Templates.IsValidIndex(TemplateIndex)) return FReply::Handled();

    SelectedTemplateIndex = TemplateIndex;
    StatusMessage.Reset();
    bWasLastCreateSuccessful = false;
    bHasCreatedMod = false;
    CreatedModName.Reset();
    DestinationPath = TemplateService.GetDefaultDestinationPath(CreatedModName);
    bDestinationManuallyEdited = false;
    bShowingTemplateDetails = true;
    RefreshContent();

    return FReply::Handled();
}

void SNexusModsCreateFromTemplateContent::OnModNameTextChanged(const FText& Text) {
    if (bHasCreatedMod || bDestinationManuallyEdited) return;

    DestinationPath = TemplateService.GetDefaultDestinationPath(Text.ToString());

    if (DestinationTextBox.IsValid()) {
        bUpdatingDestinationFromModName = true;
        DestinationTextBox->SetText(FText::FromString(DestinationPath));
        bUpdatingDestinationFromModName = false;
    }
}

void SNexusModsCreateFromTemplateContent::OnDestinationTextChanged(const FText& Text) {
    if (bUpdatingDestinationFromModName || IsDestinationReadOnly()) return;

    DestinationPath = Text.ToString();
    bDestinationManuallyEdited = true;
}

FReply SNexusModsCreateFromTemplateContent::OnCreateClicked() {
    if (SelectedTemplateIndex == INDEX_NONE || !Templates.IsValidIndex(SelectedTemplateIndex)) {
        StatusMessage = TEXT("No template is selected.");
        bWasLastCreateSuccessful = false;
        return FReply::Handled();
    }

    FString ErrorMessage;
    if (!TemplateService.CreateFromTemplate(Templates[SelectedTemplateIndex], GetModNameText(), GetDestinationText(), ErrorMessage)) {
        StatusMessage = ErrorMessage;
        bWasLastCreateSuccessful = false;
        return FReply::Handled();
    }

    CreatedModName = GetModNameText();
    DestinationPath = GetDestinationText();
    StatusMessage = FString::Printf(TEXT("Created %s."), *DestinationPath);
    bWasLastCreateSuccessful = true;
    bHasCreatedMod = true;
    OnCreated.ExecuteIfBound();
    RefreshContent();

    return FReply::Handled();
}

FReply SNexusModsCreateFromTemplateContent::OnExitClicked() {
    OnExitRequested.ExecuteIfBound();
    return FReply::Handled();
}
