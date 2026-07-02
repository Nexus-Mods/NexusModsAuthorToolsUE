#include "Widgets/Components/SNexusModsOptionSelector.h"

#include "Widgets/Components/SNexusModsButton.h"


void SNexusModsOptionSelector::Construct(const FArguments& InArgs) {
    Options = InArgs._Options;
    SelectedOption = InArgs._SelectedOption;
    OnOptionSelected = InArgs._OnOptionSelected;

    ChildSlot [
        SAssignNew(OptionsBox, SHorizontalBox)
    ];

    RebuildOptions();
}

int32 SNexusModsOptionSelector::GetSelectedOption() const {
    return SelectedOption;
}

void SNexusModsOptionSelector::SetSelectedOption(int32 InSelectedOption) {
    SelectedOption = InSelectedOption;
}

void SNexusModsOptionSelector::RebuildOptions() {
    if (!OptionsBox.IsValid()) return;
    OptionsBox->ClearChildren();

    for (int32 Index = 0; Index < Options.Num(); ++Index) {
        const FNexusModsOptionSelectorOption& Option = Options[Index];
        const bool bIsFirst = Index == 0;
        const bool bIsLast = Index == Options.Num() - 1;
        FName BorderBrush;
        FName FillBrush;
        if (Options.Num() == 1) { // Standalone button
            BorderBrush = "NexusMods.BaseBorder"; 
            FillBrush = "NexusMods.BaseFill";
        } else if (bIsFirst) {
            BorderBrush = "NexusMods.GroupBorderL";
            FillBrush = "NexusMods.GroupFillL";
        } else if (bIsLast) {
            BorderBrush = "NexusMods.GroupBorderR";
            FillBrush = "NexusMods.GroupFillR";
        } else {
            BorderBrush = "NexusMods.GroupBorderM";
            FillBrush = "NexusMods.GroupFillM";
        }
        OptionsBox->AddSlot().Padding(0).FillWidth(1.0f) [
            SNew(SNexusModsButton)
                .Text(Option.Label)
                .HoverStyle(ENexusModsButtonHoverStyle::Both)
                .bTransparentBackground(false)
                .IsSelected(this, &SNexusModsOptionSelector::IsOptionSelected, Option.Id)
                .OnClicked(this, &SNexusModsOptionSelector::OnOptionClicked, Option.Id)
                .BorderBrushName(BorderBrush)
                .FillBrushName(FillBrush)
                //.Padding(FMargin(0.0f))
        ];
    }
}

FReply SNexusModsOptionSelector::OnOptionClicked(int32 OptionId) {
    if (SelectedOption != OptionId) {
        SelectedOption = OptionId;
        OnOptionSelected.ExecuteIfBound(SelectedOption);
    }

    return FReply::Handled();
}

bool SNexusModsOptionSelector::IsOptionSelected(int32 OptionId) const {
    return SelectedOption == OptionId;
}
