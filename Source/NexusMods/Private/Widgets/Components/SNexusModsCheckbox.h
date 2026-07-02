#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "NexusModsStyle.h"

/**
 * Helper for easily getting current state of the checkbox chrome.
 */
enum class ENexusModsCheckboxState : uint8 {
    Normal,
    Hovered,
    Checked,
    Disabled
};

/**
 * Nexus Mods styled checkbox.
 *
 * This draws the checkbox using the same image-based border/fill approach as
 * SNexusModsButton and SNexusModsTextBox, so branded input controls stay visually
 * consistent across the plugin.
 */
class SNexusModsCheckbox : public SCompoundWidget {

    //: _IconBrushName(NAME_None)
    //, _IconSize(FVector2D(16.0f, 16.0f))
    //, _Text(FText::GetEmpty())
    //, _IconPosition(ENexusModsButtonIconPosition::Left)
    //, _Spacing(6.0f)
    //, _Padding(FMargin(8.0f, 4.0f))
    //, _bTransparentBackground(true)
    //, _HoverStyle(ENexusModsButtonHoverStyle::Icon)
    //, _NormalIconColor(FLinearColor::White)
    //, _HoverIconColor(FNexusModsStyle::NexusOrange)
    //, _PressedIconColor(FNexusModsStyle::NexusOrangeLight)
    //, _NormalTextColor(FLinearColor::White)
    //, _HoverTextColor(FNexusModsStyle::NexusOrange)
    //, _PressedTextColor(FNexusModsStyle::NexusOrangeLight)
    //, _NormalBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
    //, _HoverBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
    //, _PressedBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.14f))
    //, _NormalBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.69f))
    //, _HoverBorderColor(FNexusModsStyle::NexusOrange)
    //, _PressedBorderColor(FNexusModsStyle::NexusOrangeLight)

    //, _NormalBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
    //, _HoverBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
    //, _FocusedBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
    //, _ReadOnlyBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.18f))
    //, _NormalBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.69f))
    //, _HoverBorderColor(FNexusModsStyle::NexusOrange)
    //, _FocusedBorderColor(FNexusModsStyle::NexusOrange)
    //, _ReadOnlyBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f))

public:
    SLATE_BEGIN_ARGS(SNexusModsCheckbox)
        : _IsChecked(ECheckBoxState::Unchecked)
        , _CheckboxEnabled(true)
        , _Size(FVector2D(18.0f, 18.0f))
        , _CheckmarkSize(FVector2D(12.0f, 12.0f))
        , _CheckmarkColor(FNexusModsStyle::NexusOrange)
        , _CheckmarkBrushName("NexusMods.CheckIcon")
        , _NormalBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
        , _HoverBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.08f))
        //, _CheckedBackgroundColor(FLinearColor(
        //    FNexusModsStyle::NexusOrange.R,
        //    FNexusModsStyle::NexusOrange.G,
        //    FNexusModsStyle::NexusOrange.B,
        //    0.14f))
        , _CheckedBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.31f))
        , _DisabledBackgroundColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.0f))
        , _NormalBorderColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.69f))
        , _HoverBorderColor(FNexusModsStyle::NexusOrange)
        , _CheckedBorderColor(FNexusModsStyle::NexusOrange)
        , _DisabledBorderColor(FLinearColor(0.10f, 0.10f, 0.10f, 1.0f))
    {}
        SLATE_ARGUMENT(ECheckBoxState, IsChecked)
        SLATE_ARGUMENT(bool, CheckboxEnabled)
        SLATE_ARGUMENT(FVector2D, Size)

        SLATE_ARGUMENT(FLinearColor, NormalBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, HoverBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, CheckedBackgroundColor)
        SLATE_ARGUMENT(FLinearColor, DisabledBackgroundColor)

        SLATE_ARGUMENT(FLinearColor, NormalBorderColor)
        SLATE_ARGUMENT(FLinearColor, HoverBorderColor)
        SLATE_ARGUMENT(FLinearColor, CheckedBorderColor)
        SLATE_ARGUMENT(FLinearColor, DisabledBorderColor)

        SLATE_ARGUMENT(FLinearColor, CheckmarkColor)
        SLATE_ARGUMENT(FName, CheckmarkBrushName)
        SLATE_ARGUMENT(FVector2D, CheckmarkSize)
        SLATE_ATTRIBUTE(FText, ToolTipText)
        SLATE_EVENT(FOnCheckStateChanged, OnCheckStateChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    bool IsChecked() const;
    ECheckBoxState GetCheckedState() const;
    void SetIsChecked(ECheckBoxState InCheckedState);

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    ENexusModsCheckboxState GetCheckboxState() const;
    EVisibility GetCheckmarkVisibility() const;
    FSlateColor GetCheckboxBackgroundColor() const;
    FSlateColor GetCheckboxBorderColor() const;
    FSlateColor GetCheckmarkColor() const;
    void SetupCheckboxImageBrushes();

private:
    FOnCheckStateChanged OnCheckStateChanged;

    FSlateBrush BorderBrush;
    FSlateBrush FillBrush;
    const FSlateBrush* CheckmarkBrush = nullptr;

    bool bIsCheckboxEnabled = true;
    ECheckBoxState CachedCheckedState = ECheckBoxState::Unchecked;
    FVector2D Size;
    FVector2D CheckmarkSize;

    FLinearColor NormalBackgroundColor;
    FLinearColor HoverBackgroundColor;
    FLinearColor CheckedBackgroundColor;
    FLinearColor DisabledBackgroundColor;

    FLinearColor NormalBorderColor;
    FLinearColor HoverBorderColor;
    FLinearColor CheckedBorderColor;
    FLinearColor DisabledBorderColor;

    FLinearColor CheckmarkColor;
    FName CheckmarkBrushName;
    TAttribute<FText> ToolTipText;
};
