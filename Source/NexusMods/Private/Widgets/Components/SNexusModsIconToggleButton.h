#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Components/SNexusModsButton.h"

class SBox;

class SNexusModsIconToggleButton : public SCompoundWidget {
public:
    SLATE_BEGIN_ARGS(SNexusModsIconToggleButton)
        : _IsOn(false)
        , _OnIconBrushName("NexusMods.EyeSlashIcon")
        , _OffIconBrushName("NexusMods.EyeIcon")
        , _IconSize(FVector2D(16.0f, 16.0f))
        , _Padding(FNexusModsStyle::FormRowLabelPadding)
        , _bTransparentBackground(false)
        , _HoverStyle(ENexusModsButtonHoverStyle::Both)
        {}
        SLATE_ATTRIBUTE(bool, IsOn)
        SLATE_ARGUMENT(FName, OnIconBrushName)
        SLATE_ARGUMENT(FName, OffIconBrushName)
        SLATE_ARGUMENT(FVector2D, IconSize)
        SLATE_ARGUMENT(FMargin, Padding)
        SLATE_ARGUMENT(bool, bTransparentBackground)
        SLATE_ARGUMENT(ENexusModsButtonHoverStyle, HoverStyle)
        SLATE_ATTRIBUTE(FText, ToolTipText)
        SLATE_EVENT(FOnClicked, OnClicked)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedRef<SWidget> MakeButton() const;
    FReply OnButtonClicked();
    FName GetCurrentIconBrushName() const;
    void RefreshButton();

private:
    TAttribute<bool> IsOn;
    FName OnIconBrushName;
    FName OffIconBrushName;
    FVector2D IconSize;
    FMargin Padding;
    bool bTransparentBackground = false;
    ENexusModsButtonHoverStyle HoverStyle = ENexusModsButtonHoverStyle::Both;
    TAttribute<FText> ToolTipText;
    FOnClicked OnClicked;

    TSharedPtr<SBox> ButtonContainer;
};
