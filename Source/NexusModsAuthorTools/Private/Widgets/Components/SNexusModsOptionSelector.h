#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"

DECLARE_DELEGATE_OneParam(FOnNexusModsOptionSelected, int32);

struct FNexusModsOptionSelectorOption {
    int32 Id;
    FText Label;

    FNexusModsOptionSelectorOption()
        : Id(INDEX_NONE)
        , Label(FText::GetEmpty())
    {
    }

    FNexusModsOptionSelectorOption(int32 InId, const FText& InLabel)
        : Id(InId)
        , Label(InLabel)
    {
    }
};

class SNexusModsOptionSelector : public SCompoundWidget {
public:
    SLATE_BEGIN_ARGS(SNexusModsOptionSelector)
        : _SelectedOption(INDEX_NONE)
    {
    }

        SLATE_ARGUMENT(TArray<FNexusModsOptionSelectorOption>, Options)
        SLATE_ARGUMENT(int32, SelectedOption)
        SLATE_EVENT(FOnNexusModsOptionSelected, OnOptionSelected)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    int32 GetSelectedOption() const;
    void SetSelectedOption(int32 InSelectedOption);

private:
    void RebuildOptions();

    FReply OnOptionClicked(int32 OptionId);
    bool IsOptionSelected(int32 OptionId) const;

private:
    TArray<FNexusModsOptionSelectorOption> Options;
    int32 SelectedOption;

    FOnNexusModsOptionSelected OnOptionSelected;

    TSharedPtr<SHorizontalBox> OptionsBox;
};
